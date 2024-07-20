#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <map>
#include <optional>
#include <functional>
#include <iostream>
#include "Core/Pipeline.h"
#include "Core/RioUtil.h"

// Define types
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

template<typename T, int N, typename... StageCriteria>
class MemoryTracker {
    static_assert(std::is_same<T, u8>::value || std::is_same<T, u16>::value || 
                  std::is_same<T, u32>::value || std::is_same<T, float>::value,
                  "Unsupported type. Only u8, u16, u32, and float are allowed.");

    u32 address;
    Pipeline<T, N> pipeline;
    std::tuple<StageCriteria...> stage_criteria;

    // std::vector<std::function<void()>> downstreamCallback;

public:
    MemoryTracker(u32 addr, StageCriteria... crit)
        : address(addr), stage_criteria(std::make_tuple(crit...)) {
        reset();
    }

    void reset() {
        pipeline.clear();
    }

    u32 get_address() { return address; }

    T read(const Core::CPUThreadGuard& guard) {
        T mem_val;
        if constexpr(std::is_same<T, u8>::value){
            mem_val = PowerPC::MMU::HostRead_U8(guard, address);
        }
        else if constexpr(std::is_same<T, u16>::value){
            mem_val = PowerPC::MMU::HostRead_U16(guard, address);
        }
        else if constexpr(std::is_same<T, u32>::value){
            mem_val = PowerPC::MMU::HostRead_U32(guard, address);
        }
        else if constexpr(std::is_same<T, float>::value){
            mem_val = RioUtil::floatConverter(PowerPC::MMU::HostRead_U32(guard, address));
        }
        return mem_val;
    }

    void run(const Core::CPUThreadGuard& guard) {
        // Shift pipeline
        pipeline.advance();

        // Read new value into pipeline[0]
        pipeline.enqueue(read(guard));
    }

    // stage = 0 (now), stage = 1 (previous read)
    std::optional<T> getValue(std::size_t stage=0) const {
        return pipeline.inspect(stage);
    }


    std::optional<u8> getByteValue(std::size_t stage, std::size_t byte_index) const {
        // Get the value from the specified stage
        auto value = getValue(stage);
        if (!value) {
            return std::nullopt;
        }

        // Check if type is float, which does not allow byte access
        if constexpr(std::is_same<T, float>::value) {
            std::cerr << "Byte access is not allowed for float type" << std::endl;
        } else {
            // Ensure the byte index is within bounds
            if (byte_index >= sizeof(T)) {
                std::cerr << "Byte index out of range for the type" << std::endl;
            }

            // Access the specific byte
            const u8* byte_ptr = reinterpret_cast<const u8*>(&(*value));
            return byte_ptr[byte_index];
        }
    }

    template<typename Criterion>
    bool checkCriterion(std::size_t stage, Criterion criterion) const {
        auto value = getValue(stage);
        if (!value) {
            // std::cout << std::hex << address << " checkCriterion | stage: " << std::to_string(stage) << " Value: null" << "\n";
            return false;
        }
        // std::cout << std::hex << address << " checkCriterion | stage: " << std::to_string(stage) << " Value: " <<  std::to_string(*value) << "\n";
        return criterion(*value);
    }

    template<std::size_t... Is>
    bool isActiveImpl(std::index_sequence<Is...>) const {
        return (checkCriterion(std::get<Is>(stage_criteria).first, std::get<Is>(stage_criteria).second) && ...);
    }

    // Overload for when there are no criteria
    bool isActiveImpl(std::index_sequence<>) const {
        return true; // Default behavior when no criteria are provided
    }

    bool isActive() const {
        return isActiveImpl(std::make_index_sequence<sizeof...(StageCriteria)>{});
    }

    // void print(std::string name) {
    //     std::cout << "\nMemoryTracker::print() " << name << std::hex << "(0x" << address << ")\n";
    //     pipeline.print();
    //     std::cout << "\n";
    // }
};

// Helper function to create an array of MemoryTrackers
template<typename T, int Stages, typename... Criteria, std::size_t... Is>
constexpr auto createMemoryTrackers(u32 base_address, u32 offset, std::index_sequence<Is...>, Criteria... crit) {
    return std::array<MemoryTracker<T, Stages, Criteria...>, sizeof...(Is)>{
        MemoryTracker<T, Stages, Criteria...>(base_address + (Is * offset), crit...)...
    };
}

// Wrapper class to manage an array of MemoryTracker objects
template<typename T, int Stages, int Size, typename... Criteria>
class MemoryTrackerArray {
public:
    std::array<MemoryTracker<T, Stages, Criteria...>, Size> readers;

    MemoryTrackerArray(u32 base_address, u32 offset, Criteria... crit)
        : readers(createMemoryTrackers<T, Stages, Criteria...>(base_address, offset, std::make_index_sequence<Size>{}, crit...)) {
        // print();
    }

    // Provide direct access to the underlying array
    MemoryTracker<T, Stages, Criteria...>& operator[](size_t index) {
        return readers[index];
    }

    const MemoryTracker<T, Stages, Criteria...>& operator[](size_t index) const {
        return readers[index];
    }

    // Run method to call run on all MemoryTrackers
    void run(const Core::CPUThreadGuard& guard) {
        for (auto& reader : readers) {
            reader.run(guard);
        }
    }

    // void print() {
    //     for (int i=0; i < Size; ++i) {
    //         std::cout << "index=" << std::to_string(i) << " Addr=" << std::to_string(readers[i].get_address()) << "\n";
    //     }
    // }
};
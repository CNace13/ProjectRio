#include <iostream>
#include <vector>
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

template<typename T, u8 N, typename... StageCriteria>
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

    u32 getAddress() { return address; }

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
        if constexpr (std::is_invocable_r_v<bool, Criterion, T, T>) {
            auto value = getValue(stage);
            auto compare_value = getValue(criterion.stage);
            if (!value || !compare_value) {
                return false;
            }
            return criterion(*value, *compare_value);
        } else {
            auto value = getValue(stage);
            if (!value) {
                return false;
            }
            return criterion(*value);
        }
    }


    template<std::size_t... Is>
    bool isActiveImpl(std::index_sequence<Is...>) const {
        return (checkCriterion(std::get<Is>(stage_criteria).first, std::get<Is>(stage_criteria).second) && ...);
    }

    // Overload for when there are no criteria
    bool isActiveImpl(std::index_sequence<>) const {
        return false; // Default behavior when no criteria are provided
    }

    bool isActive() const {
        return isActiveImpl(std::make_index_sequence<sizeof...(StageCriteria)>{});
    }
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

#include <vector>

//Sadly, I was not able to figure out how to implement this class so that it could be constucted
//at compile time, so I am using vectors to hold all the run time info (though I could probably use arrays to be safer)
//The orginal Array class can handle simple arrays at compile time, but for N-D arrays this class must be used
template<typename T, uint8_t Stages, typename... Criteria>
class MemoryTrackerNestedArray {
private:
    uint32_t base_address;
    std::vector<uint32_t> sizes;
    std::vector<uint32_t> offsets;
    std::vector<MemoryTracker<T, Stages, Criteria...>> trackers;

    uint32_t calculateAddress(const std::vector<uint32_t>& indices) const {
        uint32_t address = base_address;
        std::cout << "Calculating address for indices: ";
        for (auto idx : indices) std::cout << idx << " ";
        std::cout << std::endl;

        for (size_t i = 0; i < indices.size(); ++i) {
            uint32_t offset = offsets[i] * indices[i];
            address += offset;
            std::cout << "  Dimension " << i << ": offset = " << std::hex << offsets[i] 
                      << " * " << std::dec << indices[i] << " = " << std::hex << offset << std::endl;
        }
        std::cout << "  Final address: 0x" << std::hex << address << std::dec << std::endl;
        return address;
    }

    uint32_t calculateTotalSize() const {
        uint32_t total = 1;
        for (uint32_t size : sizes) {
            total *= size;
        }
        std::cout << "Total size calculated: " << total << std::endl;
        return total;
    }

    template<typename... Args>
    void generateTrackers(std::vector<uint32_t>& indices, size_t depth, const Args&... args) {
        if (depth == indices.size()) {
            uint32_t address = calculateAddress(indices);
            trackers.emplace_back(address, args...);
            return;
        }

        for (uint32_t i = 0; i < sizes[depth]; ++i) {
            indices[depth] = i;
            generateTrackers(indices, depth + 1, args...);
        }
    }

public:
    MemoryTrackerNestedArray(uint32_t base_addr,
                       const std::vector<uint32_t>& sizes_, 
                       const std::vector<uint32_t>& offsets_,
                       const Criteria&... criteria)
        : base_address(base_addr), sizes(sizes_), offsets(offsets_)
    {
        // std::cout << "Initializing MemoryTrackerNestedArray" << std::endl;
        // std::cout << "Base address: 0x" << std::hex << base_address << std::dec << std::endl;
        // std::cout << "Sizes: ";
        // for (auto size : sizes) std::cout << size << " ";
        // std::cout << std::endl;
        // std::cout << "Offsets: ";
        // for (auto offset : offsets) std::cout << "0x" << std::hex << offset << " ";
        // std::cout << std::dec << std::endl;

        if (sizes.size() != offsets.size()) {
            std::cerr << "Sizes and offsets must have the same length\n";
        }

        uint32_t total_size = calculateTotalSize();
        trackers.reserve(total_size);

        std::vector<uint32_t> indices(sizes.size(), 0);

        generateTrackers(indices, 0, criteria...);
        // std::cout << "MemoryTrackerNestedArray initialization complete" << std::endl;
    }

    MemoryTracker<T, Stages, Criteria...>& at(const std::vector<uint32_t>& indices) {
        // std::cout << "Accessing MemoryTracker at indices: ";
        // for (auto idx : indices) std::cout << idx << " ";
        // std::cout << std::endl;

        if (indices.size() != sizes.size()) {
            std::cerr << "Number of indices must match the number of dimensions\n";
        }

        uint32_t index = 0;
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] >= sizes[i]) {
                std::cerr << "Index out of range\n";
            }
            if (i < indices.size()-1){
                index += indices[i] * sizes[indices.size()-1-i];
            }
            else{
                index += indices[i];
            }
        }

        // std::cout << "Calculated flat index: " << std::dec<<  index << std::hex << std::endl;
        return trackers[index];
    }

    const MemoryTracker<T, Stages, Criteria...>& at(const std::vector<uint32_t>& indices) const {
        return const_cast<MemoryTrackerNestedArray*>(this)->at(indices);
    }

    void run(const Core::CPUThreadGuard& guard) {
        // std::cout << "Running all MemoryTrackers with value: " << val << std::endl;
        for (auto& tracker : trackers) {
            tracker.run(guard);
        }
        // std::cout << "Run complete for all MemoryTrackers" << std::endl;
    }

    void reset() {
        // std::cout << "Resetting all MemoryTrackers" << std::endl;
        for (auto& tracker : trackers) {
            tracker.reset();
        }
        // std::cout << "Reset complete for all MemoryTrackers" << std::endl;
    }
};

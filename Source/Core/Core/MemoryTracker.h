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

// Define types
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

template<typename T, int N>
class MemoryTracker {
    static_assert(std::is_same<T, u8>::value || std::is_same<T, u16>::value || 
                  std::is_same<T, u32>::value || std::is_same<T, float>::value,
                  "Unsupported type. Only u8, u16, u32, and float are allowed.");

    u32 address;
    Pipeline<T, N> pipeline;
    std::optional<std::map<uint32_t, T>> activeCriteria;
    // std::vector<std::function<void()>> downstreamCallback;

public:
    MemoryTracker(u32 addr, std::optional<std::map<u32, T>> criteria = std::nullopt) : address(addr), activeCriteria(criteria) {
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
        else if constexpr(std::is_same<T, u32>::value || std::is_same<T, float>::value){
            mem_val = PowerPC::MMU::HostRead_U32(guard, address);
        }
        return mem_val;
    }

    void run(const Core::CPUThreadGuard& guard) {
        // Shift pipeline
        pipeline.advance();

        // Read new value into pipeline[0]
        pipeline.enqueue(read(guard));

        // Check if the current state is active and trigger downstream if necessary
        // if (isActive() && !downstreamCallback.empty()) {
        //     for callback in downstreamCallback {
        //         callback();
        //     }
        // }
    }

    // stage = 0 (now), stage = 1 (previous read)
    std::optional<T> getValue(std::size_t stage=0) const {
        return pipeline.inspect(stage);
    }


    std::optional<u8> getByteValue(std::size_t stage, std::size_t byte_index) const {
        std::cout << "MemoryReader::getByteValue stage=" << stage << " byte_index=" << byte_index << std::endl;

        // Get the value from the specified stage
        auto value = getValue(stage);
        if (!value) {
            std::cout << "MemoryReader::getByteValue - No value at specified stage" << std::endl;
            return std::nullopt;
        }

        // Check if type is float, which does not allow byte access
        if constexpr(std::is_same<T, float>::value) {
            std::cerr << "Byte access is not allowed for float type" << std::endl;
        } else {
            // Ensure the byte index is within bounds
            if (byte_index >= sizeof(T)) {
                std::cout << "MemoryReader::getByteValue - Byte index out of range" << std::endl;
                std::cerr << "Byte index out of range for the type" << std::endl;
            }

            // Access the specific byte
            const u8* byte_ptr = reinterpret_cast<const u8*>(&(*value));
            std::cout << "MemoryReader::getByteValue - Returning byte value " << static_cast<int>(byte_ptr[byte_index]) << std::endl;
            return byte_ptr[byte_index];
        }
    }

    bool setActiveCriteria(const std::optional<std::map<uint32_t, T>>& criteria) {
        // Validate criteria
        for (const auto& [stage, value] : *criteria) {
            if (stage < 0 || stage >= N) {
                std::cerr << "Criteria contains bad value (out of range)" << std::endl;
            }
        }
        activeCriteria = criteria;
        return true;
    }

    bool isActive() const {
        if (activeCriteria) {
            for (const auto& [stage, value] : *activeCriteria) {
                if (stage < 0 || stage >= N || !getValue(stage) || getValue(stage).value() != value) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    // void addDownstreamCallback(std::vector<std::function<void()>> callback) {
    //     downstreamCallback.emplace(callback);
    // }
};

// Helper function to create MemoryTracker objects
template<typename T, int Stages, std::size_t... Is>
auto createMemoryTrackers(u32 base_address, u32 offset, const std::optional<std::map<u32, T>>& criteria, std::index_sequence<Is...>) {
    return std::array<MemoryTracker<T, Stages>, sizeof...(Is)>{
        MemoryTracker<T, Stages>(base_address + (Is * offset), criteria)...
    };
}

// Wrapper class to manage an array of MemoryTracker objects
template<typename T, int Stages, int Size>
class MemoryTrackerArray {
public:
    std::array<MemoryTracker<T, Stages>, Size> readers;

    MemoryTrackerArray(u32 base_address, u32 offset, std::optional<std::map<u32, T>> criteria = std::nullopt)
        : readers(createMemoryTrackers<T, Stages>(base_address, offset, criteria, std::make_index_sequence<Size>{})) {
            print();
        }

    // Provide direct access to the underlying array
    MemoryTracker<T, Stages>& operator[](size_t index) {
        return readers[index];
    }

    const MemoryTracker<T, Stages>& operator[](size_t index) const {
        return readers[index];
    }

    void print() {
        for (int i=0; i < Size; ++i) {
            std::cout << "index=" << std::to_string(i) << " ReaderAddr=" << std::to_string(readers[i].get_address());
        }
    }
};
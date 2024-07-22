#include "Core/RioUtil.h"
#include <iostream>
#include <random>
#include <cstdint>

namespace {
    // Anonymous namespace for internal linkage of float_converter
    union {
        RioUtil::u32 num;
        float fnum;
    } float_union;
}

namespace RioUtil {
    float floatConverter(u32 in_value) {
        float_union.num = in_value;
        return float_union.fnum;
    }


    u32 genRand32() {
        // Create a random device to seed the random number generator
        std::random_device rd;

        // Use Mersenne Twister 19937 generator, seeded with a random device
        std::mt19937_64 gen(rd());

        // Define a uniform distribution for uint32_t
        std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

        // Generate and return a random uint32_t
        return dist(gen);
    }
}

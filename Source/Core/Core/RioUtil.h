#ifndef RIOUTILS_H
#define RIOUTILS_H

#include <cstdint>

namespace RioUtil {
    // Type definition for 32-bit unsigned integer
    using u32 = uint32_t;

    // Function to convert u32 to float
    float floatConverter(u32 in_value);

    u32 genRand32();
}

#endif // UTILS_H

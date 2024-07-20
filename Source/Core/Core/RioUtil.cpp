#include "Core/RioUtil.h"

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
}

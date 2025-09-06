module;

#include <icu.h>

export module gm:core;

import std;

namespace gm {

    // -----------------
    // fundamental types
    // -----------------

    export {

        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;

        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;

        using f32 = float;
        using f64 = double;

    }

    // --------------------
    // character properties
    // --------------------

    bool is_line_break(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_LINE_BREAK)) {
        case U_LB_MANDATORY_BREAK:
        case U_LB_CARRIAGE_RETURN:
        case U_LB_LINE_FEED:
        case U_LB_NEXT_LINE:
            return true;
        default:
            return false;
        }
    }

    bool is_wide(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH)) {
        case U_EA_FULLWIDTH:
        case U_EA_WIDE:
            return true;
        default:
            return false;
        }
    }

}

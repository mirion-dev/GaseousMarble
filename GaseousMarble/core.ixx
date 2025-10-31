module;

#include <icu.h>

export module gm:core;

import std;

namespace gm {

#pragma region fundamental types

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

#pragma endregion

#pragma region error handling

    template <class T, class W>
    struct Wrapped {
        T result;
        W warning;
    };

    template <class W>
    struct Wrapped<void, W> {
        W warning;
    };

    template <class T, class W>
    Wrapped(T, W) -> Wrapped<T, W>;

    template <class W>
    Wrapped(W) -> Wrapped<void, W>;

    template <class T, class W, class E>
    using Result = std::expected<Wrapped<T, W>, E>;

#pragma endregion

#pragma region text handling

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

    template <class Pr = decltype([](i32) { return true; })>
    std::optional<std::u16string> utf8_to_utf16(std::string_view text, Pr filter = {}) {
        const char* u8_ptr{ text.data() };
        u32 u8_size{ text.size() };
        std::u16string u16;
        bool error{};
        u16.resize_and_overwrite(u8_size,
            [&](char16_t* u16_ptr, u32) {
                u32 u16_size{};
                for (u32 i{}; i != u8_size;) {
                    i32 ch;
                    U8_NEXT(u8_ptr, i, u8_size, ch);
                    if (ch < 0 || !filter(ch)) {
                        error = true;
                        return 0uz;
                    }

                    U16_APPEND_UNSAFE(u16_ptr, u16_size, ch);
                }
                return u16_size;
            });

        if (error) {
            return {};
        }
        return u16;
    }

#pragma endregion

}

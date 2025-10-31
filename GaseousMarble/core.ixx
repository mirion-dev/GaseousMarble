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

    bool is_white_space(u32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

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

    bool unicode_for_each(std::string_view utf8, auto func) noexcept {
        const char* ptr{ utf8.data() };
        u32 size{ utf8.size() };
        for (u32 i{}; i != size;) {
            i32 ch;
            U8_NEXT(ptr, i, size, ch);
            if (ch < 0 || !func(ch)) {
                return false;
            }
        }
        return true;
    }

    bool word_break_for_each(std::u16string_view utf16, auto func) noexcept {
        const char16_t* ptr{ utf16.data() };
        u32 size{ utf16.size() };
        UErrorCode error{};
        std::unique_ptr<UBreakIterator, decltype(&ubrk_close)> iter{
            ubrk_open(UBRK_WORD, nullptr, ptr, size, &error),
            ubrk_close
        };
        if (U_FAILURE(error)) {
            return false;
        }

        for (i32 first{ ubrk_first(iter.get()) }, last; ; first = last) {
            last = ubrk_next(iter.get());
            if (last == UBRK_DONE) {
                break;
            }

            if (!func(std::u16string_view{ ptr + first, ptr + last }, ubrk_getRuleStatus(iter.get()))) {
                return false;
            }
        }
        return true;
    }

#pragma endregion

}

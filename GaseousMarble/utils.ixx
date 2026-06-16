module;

#include <icu.h>
#include <wil/resource.h>

export module gm.utils;

import std;
import gm.types;

namespace gm {

    export template <class T, auto Deleter, T Null = {}>
    using Handle = wil::unique_any<T, decltype(Deleter), Deleter, wil::details::pointer_access_all, T, T, Null>;

    export bool is_white_space(u32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

    export bool is_line_break(u32 ch) noexcept {
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

    export bool is_wide(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH)) {
        case U_EA_FULLWIDTH:
        case U_EA_WIDE:
            return true;
        default:
            return false;
        }
    }

    export bool unicode_for_each(std::string_view str, const auto& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, str.data(), str.size(), &error) };
        if (!iter) {
            return false;
        }

        while (true) {
            auto ch{ static_cast<u32>(utext_next32(iter.get())) };
            if (ch == -1 || !func(ch)) {
                return true;
            }
        }
    }

    export bool word_break_for_each(std::string_view str, const auto& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, str.data(), str.size(), &error) };
        if (!iter) {
            return false;
        }

        Handle<UBreakIterator*, ubrk_close> breaker{ ubrk_open(UBRK_WORD, "", nullptr, 0, &error) };
        if (!breaker) {
            return false;
        }

        ubrk_setUText(breaker.get(), iter.get(), &error);
        if (error > 0) {
            return false;
        }

        const char* ptr{ str.data() };
        usize first{};
        while (true) {
            auto last{ static_cast<usize>(ubrk_next(breaker.get())) };
            if (last == -1 || !func(
                std::string_view{ ptr + first, ptr + last },
                static_cast<u32>(ubrk_getRuleStatus(breaker.get()))
            )) {
                return true;
            }
            first = last;
        }
    }

}

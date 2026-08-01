module;

#include <icu.h>
#include <wil/resource.h>

export module gm.utils;

import std;
import gm.types;

namespace gm {

    struct Bound {
        int a{};
        int b{};

        constexpr friend bool operator<(Bound left, Bound right) noexcept {
            int max_diff{ std::max(left.a, right.b) - std::max(left.b, right.a) };
            int min_diff{ std::min(left.a, right.b) - std::min(left.b, right.a) };
            return max_diff < 0 || max_diff == 0 && min_diff < 0;
        }
    };

    template <class T>
        requires std::is_arithmetic_v<T>
    constexpr Bound upper_bound() {
        static constexpr int D{ std::numeric_limits<T>::digits };
        static constexpr int E{ std::numeric_limits<T>::max_exponent };
        return std::integral<T> ? Bound{ D, 0 } : Bound{ E, E - D };
    }

    template <class T>
        requires std::is_arithmetic_v<T>
    constexpr Bound neg_lower_bound() {
        static constexpr int D{ std::numeric_limits<T>::digits };
        static constexpr int E{ std::numeric_limits<T>::max_exponent };
        return std::unsigned_integral<T> ? Bound{ 0, 0 }
               : std::signed_integral<T> ? Bound{ D, D + 1 }
                                         : Bound{ E - D, E };
    }

    export template <class R, class T>
        requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
    R saturating_cast(T num, R neg_overflow, R pos_overflow) noexcept {
        if constexpr (std::floating_point<T>) {
            if (std::isnan(num)) {
                return R{};
            }

            if (std::isinf(num)) {
                return num > T{} ? pos_overflow : neg_overflow;
            }
        }

        if constexpr (neg_lower_bound<T>() < neg_lower_bound<R>()) {
            if (num < static_cast<T>(std::numeric_limits<R>::lowest())) {
                return neg_overflow;
            }
        }

        if constexpr (upper_bound<R>() < upper_bound<T>()) {
            if constexpr (
                std::floating_point<T>
                && std::integral<R>
                && std::numeric_limits<T>::digits < std::numeric_limits<R>::digits
            ) {
                if (num >= std::exp2(static_cast<T>(std::numeric_limits<R>::digits))) {
                    return pos_overflow;
                }
            } else {
                if (num > static_cast<T>(std::numeric_limits<R>::max())) {
                    return pos_overflow;
                }
            }
        }

        return static_cast<R>(num);
    }

    export template <class R, class T>
        requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
    R saturating_cast(T num, R overflow) noexcept {
        return gm::saturating_cast(num, overflow, overflow);
    }

    export template <class R, class T>
        requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
    R saturating_cast(T num) noexcept {
        return gm::saturating_cast(num, std::numeric_limits<R>::lowest(), std::numeric_limits<R>::max());
    }

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

    export template <class Fn>
    bool unicode_for_each(std::string_view str, Fn&& func) noexcept {
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

    export template <class Fn>
    bool word_break_for_each(std::string_view str, Fn&& func) noexcept {
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
            if (last == -1
                || !func(
                    std::string_view{ ptr + first, ptr + last }, static_cast<u32>(ubrk_getRuleStatus(breaker.get()))
                )) {
                return true;
            }
            first = last;
        }
    }

}

module;

#include <wil/com.h>

export module gm.utils;

import std;
import gm.types;

namespace gm {

    template <class T, bool Max>
        requires std::is_arithmetic_v<T>
    class Limit {
        static constexpr int D{ std::numeric_limits<T>::digits };
        static constexpr int E{ std::numeric_limits<T>::max_exponent };

        int _a{
            Max
            ? (std::integral<T> ? D : E)
            : (std::unsigned_integral<T> ? 0 : std::signed_integral<T> ? D : E - D)
        };
        int _b{
            Max
            ? (std::integral<T> ? 0 : E - D)
            : (std::unsigned_integral<T> ? 0 : std::signed_integral<T> ? D + 1 : E)
        };

    public:
        template <class T1, bool Max1, class T2, bool Max2>
        constexpr friend bool operator<(Limit<T1, Max1> left, Limit<T2, Max2> right) noexcept {
            int max_diff{ std::max(left._a, right._b) - std::max(left._b, right._a) };
            int min_diff{ std::min(left._a, right._b) - std::min(left._b, right._a) };
            return max_diff < 0 || max_diff == 0 && min_diff < 0;
        }
    };

    template <class T>
        requires std::is_arithmetic_v<T>
    static constexpr Limit<T, true> max_limit;

    template <class T>
        requires std::is_arithmetic_v<T>
    static constexpr Limit<T, false> min_limit;

    export template <class R, class T>
        requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
    R saturating_cast(T num, R neg_overflow, R pos_overflow) noexcept {
        if constexpr (std::floating_point<T>) {
            if (std::isnan(num)) {
                return R{};
            }
        }
        if constexpr (min_limit<T> < min_limit<R>) {
            if (num < static_cast<T>(std::numeric_limits<R>::lowest())) {
                return neg_overflow;
            }
        }
        if constexpr (max_limit<R> < max_limit<T>) {
            if (num > static_cast<T>(std::numeric_limits<R>::max())) {
                return pos_overflow;
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

    export std::string to_string(std::wstring_view str) {
        std::u8string str_u8{ std::filesystem::path{ str }.u8string() };
        return { reinterpret_cast<char*>(str_u8.data()), str_u8.size() };
    }

    export std::wstring to_wstring(std::string_view str) {
        std::u8string str_u8{ str.begin(), str.end() };
        return std::filesystem::path{ str_u8 }.wstring();
    }

    export struct Hash {
        template <class T>
        usize operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
        }

        template <class T>
        usize operator()(wil::com_ptr<T> value) const noexcept {
            return std::hash<T*>{}(value.get());
        }
    };

    export template <class H, class... Args>
    usize hash_combine(H&& hash, Args&&... values) noexcept {
        usize result{};
        return (
            (result ^= std::forward<H>(hash)(std::forward<Args>(values)) + 0x9e3779b9 + (result << 6) + (result >> 2)),
            ...
        );
    }

}

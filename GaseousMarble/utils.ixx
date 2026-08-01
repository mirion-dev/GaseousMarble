module;

#include <wil/com.h>

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

    template <class T>
    void unique_deleter(const T&) noexcept {}

    export template <class T, T Null = {}>
    using Unique = wil::
        unique_any<T, decltype(unique_deleter<T>), unique_deleter<T>, wil::details::pointer_access_all, T, T, Null, T>;

}

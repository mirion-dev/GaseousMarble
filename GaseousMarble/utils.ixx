module;

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

    export struct Hash {
        template <class T>
        usize operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
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

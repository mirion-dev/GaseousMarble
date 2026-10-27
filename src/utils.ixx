module;

#include <cassert>
#include <eigen3/Eigen/Core>
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

    export template <std::floating_point T>
    T deg_to_rad(T deg) noexcept {
        return deg * (std::numbers::pi_v<T> / 180);
    }

    export u32 lerp_color(u32 from, u32 to, f32 factor) noexcept {
        assert(0 <= factor && factor <= 1);

        u32 result{};
        for (int shift{}; shift < 32; shift += 8) {
            result |= std::lround(std::lerp(from >> shift & 0xff, to >> shift & 0xff, factor)) << shift;
        }

        return result;
    }

    export u32 bilerp_color(u32 tl, u32 tr, u32 bl, u32 br, f32 factor_x, f32 factor_y) noexcept {
        assert(0 <= factor_x && factor_x <= 1 && 0 <= factor_y && factor_y <= 1);
        return lerp_color(lerp_color(tl, tr, factor_x), lerp_color(bl, br, factor_x), factor_y);
    }

    export u32 swap_red_blue(u32 color) noexcept {
        return color & 0xff00ff00 | color >> 16 & 0xff | (color & 0xff) << 16;
    }

    export u32 with_opacity(u32 color, f32 opacity) noexcept {
        assert(0 <= opacity && opacity <= 1);
        return color & 0x00ffffff | static_cast<u32>(std::lround((color >> 24) * opacity)) << 24;
    }

    export u32 make_rgb(u8 red, u8 green, u8 blue, u8 alpha = 255) noexcept {
        return static_cast<u32>(alpha) << 24 | static_cast<u32>(red) << 16 | static_cast<u32>(green) << 8 | blue;
    }

    export u32 make_hsv(u8 hue, u8 saturation, u8 value, u8 alpha = 255) noexcept {
        f32 h{ hue / 255.f * 6 };
        f32 s{ saturation / 255.f };
        f32 v{ value / 255.f };
        f32 c{ v * s };
        f32 x{ c * (1 - std::abs(std::fmod(h, 2.f) - 1)) };
        f32 m{ v - c };
        f32 r, g, b;
        if (h < 1) {
            std::tie(r, g, b) = std::tuple{ c, x, 0 };
        } else if (h < 2) {
            std::tie(r, g, b) = std::tuple{ x, c, 0 };
        } else if (h < 3) {
            std::tie(r, g, b) = std::tuple{ 0, c, x };
        } else if (h < 4) {
            std::tie(r, g, b) = std::tuple{ 0, x, c };
        } else if (h < 5) {
            std::tie(r, g, b) = std::tuple{ x, 0, c };
        } else {
            std::tie(r, g, b) = std::tuple{ c, 0, x };
        }

        return make_rgb(
            static_cast<u8>(std::lround((r + m) * 255)),
            static_cast<u8>(std::lround((g + m) * 255)),
            static_cast<u8>(std::lround((b + m) * 255)),
            alpha
        );
    }

    export std::string to_string(std::wstring_view str) {
        std::u8string str_u8{ std::filesystem::path{ str }.u8string() };
        return { reinterpret_cast<char*>(str_u8.data()), str_u8.size() };
    }

    export std::wstring to_wstring(std::string_view str) {
        std::u8string str_u8{ str.begin(), str.end() };
        return std::filesystem::path{ str_u8 }.wstring();
    }

    export std::string filename(std::string_view path) noexcept {
        usize pos{ path.find_last_of("/\\") };
        return std::string{ pos == -1 ? path : path.substr(pos + 1) };
    }

    export template <class H, class... Args>
    usize hash_combine(H&& hash, Args&&... values) noexcept {
        usize result{};
        return (
            (result ^= std::forward<H>(hash)(std::forward<Args>(values)) + 0x9e3779b9 + (result << 6) + (result >> 2)),
            ...
        );
    }

    export template <class T, auto Deleter, T Null = {}>
    using Handle = wil::unique_any<T, decltype(Deleter), Deleter, wil::details::pointer_access_all, T, T, Null>;

    export struct Hash {
        template <class T>
        usize operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
        }

        template <class T>
        usize operator()(const Eigen::Vector2<T>& value) const noexcept {
            return gm::hash_combine(Hash{}, value.x(), value.y());
        }

        template <class T>
        usize operator()(const wil::com_ptr<T>& value) const noexcept {
            return Hash{}(value.get());
        }
    };

}

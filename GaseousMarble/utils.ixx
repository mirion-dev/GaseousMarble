module;

#include <wil/com.h>

export module gm.utils;

import std;
import gm.types;

namespace gm {

    template <class T>
        requires std::is_arithmetic_v<T>
    static constexpr int min_level{
        std::signed_integral<T>
        ? std::numeric_limits<T>::digits
        : std::unsigned_integral<T>
        ? std::numeric_limits<int>::min()
        : std::numeric_limits<T>::max_exponent
    };

    template <class T>
        requires std::is_arithmetic_v<T>
    static constexpr int max_level{
        std::integral<T>
        ? std::numeric_limits<T>::digits
        : std::numeric_limits<T>::max_exponent
    };

    export template <class R, class T>
        requires std::is_arithmetic_v<R> && std::is_arithmetic_v<T>
    R saturating_cast(T num) noexcept {
        static constexpr R min{ std::numeric_limits<R>::min() };
        static constexpr R max{ std::numeric_limits<R>::max() };

        if constexpr (std::floating_point<T>) {
            if (std::isnan(num)) {
                return R{};
            }
        }
        if constexpr (min_level<T> > min_level<R>) {
            if (num <= T{ min }) {
                return min;
            }
        }
        if constexpr (max_level<T> > max_level<R>) {
            if (num >= T{ max }) {
                return max;
            }
        }

        return static_cast<R>(num);
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
        return ((result ^=
            std::forward<H>(hash)(std::forward<Args>(values)) + 0x9e3779b9 + (result << 6) + (result >> 2)), ...);
    }

}

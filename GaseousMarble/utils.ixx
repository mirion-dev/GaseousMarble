module;

#include <wil/com.h>

export module gm.utils;

import std;
import gm.types;

namespace gm {

    export std::wstring to_wstring(std::string_view str) noexcept {
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

        template <class... Args>
        static usize combine(const Args&... values) noexcept {
            usize result{};
            return ((result = result ^ Hash{}(values) + 0x9e3779b9 + (result << 6) + (result >> 2)), ...);
        }
    };

}

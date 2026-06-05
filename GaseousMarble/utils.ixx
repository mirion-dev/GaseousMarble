export module gm.utils;

import std;

namespace gm {

    export std::wstring to_wstring(std::string_view str) noexcept {
        std::u8string str_u8{ str.begin(), str.end() };
        return std::filesystem::path{ str_u8 }.wstring();
    }

}

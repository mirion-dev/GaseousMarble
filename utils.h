#pragma once

#include <string>

namespace gm {

    inline std::wstring utf8_to_ucs2(std::string_view str) noexcept {
        std::wstring ret;
        auto i{ str.begin() }, end{ str.end() };
        while (i != end) {
            wchar_t ch;
            if ((*i & 0x80) == 0) {
                ch = i[0];
                i += 1;
            }
            else if ((*i & 0xe0) == 0xc0) {
                ch = (i[0] & 0x1f) << 6 | i[1] & 0x3f;
                i += 2;
            }
            else if ((*i & 0xf0) == 0xe0) {
                ch = i[0] << 12 | (i[1] & 0x3f) << 6 | i[2] & 0x3f;
                i += 3;
            }
            else {
                ++i;
                continue;
            }

            if (iswblank(ch)) {
                ret.push_back(' ');
            }
            else if (!iswcntrl(ch) || ch == '\n') {
                ret.push_back(ch);
            }
        }
        return ret;
    }

}
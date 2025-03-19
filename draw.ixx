module;

#include <assert.h>
#include <d3dx8.h>
#include <Windows.h>

export module gm.draw;

import std;
import gm.core;
import gm.engine;

namespace gm::draw {

    class Font {
        ID3DXFont* _font{};

    public:
        Font() noexcept = default;

        Font(std::wstring_view name, u32 height, u32 width = 0, bool bold = false, bool italic = false) noexcept {
            LOGFONT logical_font{
                static_cast<long>(height),
                static_cast<long>(width),
                0,
                0,
                bold ? FW_BOLD : FW_NORMAL,
                italic,
                false,
                false,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE
            };
            std::memcpy(logical_font.lfFaceName, name.data(), name.size());
            D3DXCreateFontIndirect(gm::engine::direct3d.device(), &logical_font, &_font);
        }

        ~Font() noexcept {
            if (_font != nullptr) {
                _font->Release();
            }
        }

        operator bool() const noexcept {
            return _font != nullptr;
        }

        void draw(std::wstring_view text) noexcept {
            assert(*this);
            auto [width, height] {gm::engine::direct3d.size()};
            RECT rect{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
            _font->DrawText(text.data(), -1, &rect, DT_CENTER | DT_VCENTER, 0xffffffff);
        }
    };

    class Draw {

    public:
        Draw() noexcept {};

        void test() noexcept {
            Font{ L"Microsoft YaHei", 16 }.draw(L"1234567890ABCEDFGHIJKLMN一二三四五六七八九十");
        }
    };

    export Draw draw;

}
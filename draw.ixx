module;

#include <d3dx8.h>
#include <Windows.h>

export module gm.draw;

import std;
import gm.core;
import gm.engine;

namespace gm::draw {

    class Draw {
        HFONT _gdi_font{};
        ID3DXFont* _font{};

    public:
        Draw() noexcept = default;

        void init() noexcept {
            _gdi_font = CreateFont(18, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("DengXian"));
            D3DXCreateFont(gm::engine::direct3d.device(), _gdi_font, &_font);
        }

        ~Draw() noexcept {
            if (_gdi_font != nullptr) {
                _font->Release();
                DeleteObject(_gdi_font);
            }
        }

        void test() noexcept {
            RECT text_rect{ 0, 0, 800, 600 };
            _font->DrawText(TEXT("0123456789ABCDEFGHIJKLMN"), -1, &text_rect, DT_CENTER | DT_VCENTER, 0xffffffff);
        }
    };

    export Draw draw;

}
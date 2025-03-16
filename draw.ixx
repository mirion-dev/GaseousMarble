module;

#include <d3dx8.h>
#include <Windows.h>

export module gm.draw;

import std;
import gm.core;
import gm.engine;

namespace gm::draw {

    class Draw {
        u32 _sprite_id{ static_cast<u32>(-1) };
        IDirect3DTexture8* _texture;
        IDirect3DSurface8* _surface;
        IDirect3DSurface8* _working_surface;
        HFONT _gdi_font;
        ID3DXFont* _font;

    public:
        Draw() noexcept = default;

        void init() noexcept {
            IDirect3DDevice8* device{ gm::engine::direct3d.device() };
            auto [width, height] {gm::engine::direct3d.size()};

            _sprite_id = gm::engine::function[gm::engine::FunctionId::sprite_create_from_screen].call<u32, Real, Real, Real, Real, Real, Real, Real, Real>(0, 0, width, height, false, false, 0, 0);
            _texture = gm::engine::sprite[_sprite_id][0].data();
            _texture->GetSurfaceLevel(0, &_surface);

            _gdi_font = CreateFont(12, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
            D3DXCreateFont(device, _gdi_font, &_font);

            device->CreateRenderTarget(width, height, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, true, &_working_surface);
        }

        ~Draw() noexcept {
            if (_sprite_id != -1) {
                gm::engine::function[gm::engine::FunctionId::sprite_delete].call<void, Real>(_sprite_id);
                DeleteObject(_gdi_font);
                _font->Release();
                _working_surface->Release();
            }
        }

        void test() noexcept {
            IDirect3DDevice8* device{ gm::engine::direct3d.device() };

            IDirect3DSurface8* last_surface;
            device->GetRenderTarget(&last_surface);
            device->SetRenderTarget(_working_surface, nullptr);

            RECT text_rect{ 0, 0, 251, 226 };
            device->Clear(0, nullptr, D3DCLEAR_TARGET, 0x00000000, 1, 0);
            _font->DrawText(TEXT("0123456789ABCDEFGHIJKLMN"), -1, &text_rect, DT_CENTER | DT_VCENTER, 0xffffffff);
            device->CopyRects(_working_surface, nullptr, 0, _surface, nullptr);

            device->SetRenderTarget(last_surface, nullptr);

            gm::engine::function[gm::engine::FunctionId::draw_sprite].call<void, Real, Real, Real, Real>(_sprite_id, 0, 0, 0);
        }
    };

    export Draw draw;

}
module;

#include <d2d1.h>
#include <d3dx8.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

export module gm:dx_draw;

import std;
import :core;
import :engine;

using Microsoft::WRL::ComPtr;

namespace gm {

    export class DxDraw {
        ComPtr<IWICImagingFactory> _factory_wic;
        ComPtr<ID2D1Factory> _factory_d2d;
        ComPtr<IDWriteFactory> _factory_dw;

        ComPtr<IWICBitmap> _bitmap;
        ComPtr<ID2D1RenderTarget> _render_target;
        ComPtr<ID2D1SolidColorBrush> _brush;
        ComPtr<IDWriteTextFormat> _format;

        ComPtr<IDirect3DTexture8> _texture;
        ComPtr<ID3DXSprite> _sprite;

    public:
        bool init() noexcept {
            IDirect3DDevice8* device{ Direct3D::device() };
            u32 width{ Direct3D::render_width() };
            u32 height{ Direct3D::render_height() };

            if (CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_factory_wic))
                || D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&_factory_d2d))
                || DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_factory_dw)
                || _factory_wic->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand,
                    &_bitmap)
                || _factory_d2d->CreateWicBitmapRenderTarget(
                    _bitmap.Get(),
                    D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                    ),
                    &_render_target
                )
                || _render_target->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &_brush)
                || _factory_dw->CreateTextFormat(
                    L"Microsoft YaHei",
                    nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    100,
                    L"",
                    &_format
                )
                || D3DXCreateTexture(device, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &_texture)
                || D3DXCreateSprite(device, &_sprite)) {
                return false;
            }

            return true;
        }

        bool text(f64 x, f64 y, std::string_view) const noexcept {
            u32 width{ Direct3D::render_width() };
            u32 height{ Direct3D::render_height() };

            auto text{ L"Hello World" };
            ComPtr<IDWriteTextLayout> layout;
            if (_factory_dw->CreateTextLayout(text, std::wcslen(text), _format.Get(), FLT_MAX, FLT_MAX, &layout)) {
                return false;
            }

            _render_target->BeginDraw();
            _render_target->Clear();
            _render_target->DrawTextLayout(D2D1::Point2F(), layout.Get(), _brush.Get());
            if (_render_target->EndDraw()) {
                return false;
            }

            WICRect bitmap_rect{ 0, 0, static_cast<i32>(width), static_cast<i32>(height) };
            ComPtr<IWICBitmapLock> bitmap_lock;
            D3DLOCKED_RECT texture_lock;
            if (_bitmap->Lock(&bitmap_rect, WICBitmapLockRead, &bitmap_lock)
                || _texture->LockRect(0, &texture_lock, nullptr, 0)) {
                return false;
            }

            u32 texture_stride{ static_cast<u32>(texture_lock.Pitch) }, bitmap_stride, _;
            u8 *texture_data{ static_cast<u8*>(texture_lock.pBits) }, *bitmap_data;
            if (bitmap_lock->GetStride(&bitmap_stride)
                || bitmap_lock->GetDataPointer(&_, &bitmap_data)) {
                return false;
            }

            for (u32 y{}; y < height; ++y) {
                std::memcpy(texture_data + y * texture_stride, bitmap_data + y * bitmap_stride, width * 4);
            }

            if (_texture->UnlockRect(0)) {
                return false;
            }

            D3DXVECTOR2 pos{ static_cast<f32>(x), static_cast<f32>(y) };
            if (_sprite->Draw(_texture.Get(), nullptr, nullptr, nullptr, 0, &pos, 0xffffffff)) {
                return false;
            }

            return true;
        }
    };

}

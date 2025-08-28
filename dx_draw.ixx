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

        struct Checker {
            Checker& operator=(HRESULT result) noexcept {
                if (FAILED(result)) {
                    std::abort();
                }
                return *this;
            }
        };

    public:
        void init() noexcept {
            IDirect3DDevice8* device{ Direct3D::device() };
            u32 width{ Direct3D::render_width() };
            u32 height{ Direct3D::render_height() };
            Checker checker;

            checker = CoCreateInstance(
                CLSID_WICImagingFactory2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&_factory_wic)
            );
            checker = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&_factory_d2d));
            checker = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_factory_dw);

            checker = _factory_wic->CreateBitmap(
                width,
                height,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapCacheOnDemand,
                &_bitmap
            );
            checker = _factory_d2d->CreateWicBitmapRenderTarget(
                _bitmap.Get(),
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                ),
                &_render_target
            );
            checker = _render_target->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &_brush);
            checker = _factory_dw->CreateTextFormat(
                L"Microsoft YaHei",
                nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                100,
                L"",
                &_format
            );

            checker = D3DXCreateTexture(device, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &_texture);
            checker = D3DXCreateSprite(device, &_sprite);
        }

        void text() const noexcept {
            u32 width{ Direct3D::render_width() };
            u32 height{ Direct3D::render_height() };
            Checker checker;

            auto text{ L"Hello World" };
            ComPtr<IDWriteTextLayout> layout;
            checker = _factory_dw->CreateTextLayout(text, std::wcslen(text), _format.Get(), FLT_MAX, FLT_MAX, &layout);

            _render_target->BeginDraw();
            _render_target->Clear();
            _render_target->DrawTextLayout(D2D1::Point2F(), layout.Get(), _brush.Get());
            checker = _render_target->EndDraw();

            WICRect bitmap_rect{ 0, 0, static_cast<i32>(width), static_cast<i32>(height) };
            ComPtr<IWICBitmapLock> bitmap_lock;
            D3DLOCKED_RECT texture_lock;
            checker = _bitmap->Lock(&bitmap_rect, WICBitmapLockRead, &bitmap_lock);
            checker = _texture->LockRect(0, &texture_lock, nullptr, 0);

            u32 texture_stride{ static_cast<u32>(texture_lock.Pitch) }, bitmap_stride, _;
            u8 *texture_data{ static_cast<u8*>(texture_lock.pBits) }, *bitmap_data;
            checker = bitmap_lock->GetStride(&bitmap_stride);
            checker = bitmap_lock->GetDataPointer(&_, &bitmap_data);

            for (u32 y{}; y < height; ++y) {
                std::memcpy(texture_data + y * texture_stride, bitmap_data + y * bitmap_stride, width * 4);
            }

            checker = _texture->UnlockRect(0);

            D3DXVECTOR2 pos{ 0, 0 };
            checker = _sprite->Draw(_texture.Get(), nullptr, nullptr, nullptr, 0, &pos, 0xffffffff);
        }
    };

}

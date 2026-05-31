module;

#include <d2d1.h>
#include <d3dx8.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#undef max

export module gm.draw;

import std;
import gm.types;
import gm.utils;
import gm.engine;

using Microsoft::WRL::ComPtr;

namespace gm {

    export class Draw {
    public:
        struct Setting {
            std::wstring font{ L"Microsoft YaHei" };
            DWRITE_FONT_WEIGHT weight{ DWRITE_FONT_WEIGHT_NORMAL };
            DWRITE_FONT_STYLE style{ DWRITE_FONT_STYLE_NORMAL };
            DWRITE_FONT_STRETCH stretch{ DWRITE_FONT_STRETCH_NORMAL };
            D2D1::ColorF color{ D2D1::ColorF::White };
            std::wstring locale{};
            f32 size{ 100 };
            f32 origin_x{};
            f32 origin_y{};
            f32 box_width{ std::numeric_limits<f32>::max() };
            f32 box_height{ std::numeric_limits<f32>::max() };
        };

    private:
        ComPtr<IWICImagingFactory> _factory_wic;
        ComPtr<ID2D1Factory> _factory_d2d;
        ComPtr<IDWriteFactory> _factory_dw;

        u32 _width{}, _height{};
        ComPtr<IWICBitmap> _bitmap;
        ComPtr<ID2D1RenderTarget> _render_target;
        ComPtr<IDirect3DTexture8> _texture;
        ComPtr<ID3DXSprite> _sprite;

        ComPtr<ID2D1SolidColorBrush> _brush;
        ComPtr<IDWriteTextFormat> _format;

        Setting _setting;

        using Chain = InvokeChain<HRESULT, decltype([](HRESULT error) noexcept { return SUCCEEDED(error); })>;

        HRESULT _update_objects() noexcept {
            IDirect3DDevice8* device{ Direct3D::device() };
            return Chain{}
                .and_then(
                    [&] noexcept {
                        return _factory_wic->CreateBitmap(
                            _width,
                            _height,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapCacheOnDemand,
                            &_bitmap
                        );
                    }
                )
                .and_then(
                    [&] noexcept {
                        return _factory_d2d->CreateWicBitmapRenderTarget(
                            _bitmap.Get(),
                            D2D1::RenderTargetProperties(
                                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                            ),
                            &_render_target
                        );
                    }
                )
                .and_then(
                    D3DXCreateTexture,
                    device,
                    _width,
                    _height,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &_texture
                )
                .and_then(D3DXCreateSprite, device, &_sprite)
                .and_then([&] noexcept { return _update_brush(); });
        }

        HRESULT _update_brush() noexcept {
            return _render_target->CreateSolidColorBrush(_setting.color, &_brush);
        }

        HRESULT _update_format() noexcept {
            return _factory_dw->CreateTextFormat(
                _setting.font.data(),
                nullptr,
                _setting.weight,
                _setting.style,
                _setting.stretch,
                _setting.size,
                _setting.locale.data(),
                &_format
            );
        }

    public:
        Draw() {
            HRESULT error{
                Chain{}
                .and_then(
                    CoCreateInstance,
                    CLSID_WICImagingFactory2,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    IID_PPV_ARGS(&_factory_wic)
                )
                .and_then(
                    [&] noexcept {
                        return D2D1CreateFactory(
                            D2D1_FACTORY_TYPE_SINGLE_THREADED,
                            static_cast<ID2D1Factory**>(&_factory_d2d)
                        );
                    }
                )
                .and_then(
                    DWriteCreateFactory,
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(IDWriteFactory),
                    &_factory_dw
                )
            };
            if (error != S_OK) {
                throw error;
            }
        }

        HRESULT text(f32 x, f32 y, std::string_view text) noexcept {
            // TEST
            _update_format();
            std::wstring u16(text.size(), '\0');
            std::ranges::copy(text, u16.begin());

            ComPtr<IDWriteTextLayout> layout;
            ComPtr<IWICBitmapLock> bitmap_lock;
            D3DLOCKED_RECT texture_lock;
            u32 texture_stride, bitmap_stride;
            u8 *texture_data, *bitmap_data;
            return Chain{}
                .and_then(
                    [&] noexcept {
                        u32 width{ Direct3D::render_width() };
                        u32 height{ Direct3D::render_height() };
                        if (_width == width && _height == height) {
                            return S_OK;
                        }

                        _width = width;
                        _height = height;
                        return _update_objects();
                    }
                )
                .and_then(
                    [&] noexcept {
                        return _factory_dw->CreateTextLayout(
                            u16.data(),
                            u16.size(),
                            _format.Get(),
                            _setting.box_width,
                            _setting.box_height,
                            &layout
                        );
                    }
                )
                .and_then(
                    [&] noexcept {
                        _render_target->BeginDraw();
                        _render_target->Clear();
                        _render_target->DrawTextLayout(
                            D2D1::Point2F(_setting.origin_x, _setting.origin_y),
                            layout.Get(),
                            _brush.Get()
                        );
                        return _render_target->EndDraw();
                    }
                )
                .and_then(
                    [&] noexcept {
                        WICRect rect{ 0, 0, static_cast<i32>(_width), static_cast<i32>(_height) };
                        return _bitmap->Lock(&rect, WICBitmapLockRead, &bitmap_lock);
                    }
                )
                .and_then(
                    [&] noexcept {
                        return bitmap_lock->GetStride(&bitmap_stride);
                    }
                )
                .and_then(
                    [&] noexcept {
                        u32 _;
                        return bitmap_lock->GetDataPointer(&_, &bitmap_data);
                    }
                )
                .and_then(
                    [&] noexcept {
                        HRESULT error{ _texture->LockRect(0, &texture_lock, nullptr, 0) };
                        texture_stride = texture_lock.Pitch;
                        texture_data = static_cast<u8*>(texture_lock.pBits);
                        return error;
                    }
                )
                .and_then(
                    [&] noexcept {
                        for (u32 y{}; y < _height; ++y) {
                            std::memcpy(texture_data + y * texture_stride, bitmap_data + y * bitmap_stride, _width * 4);
                        }

                        bitmap_lock.Reset();
                        return _texture->UnlockRect(0);
                    }
                )
                .and_then(
                    [&] noexcept {
                        D3DXVECTOR2 pos{ x, y };
                        return _sprite->Draw(
                            _texture.Get(),
                            nullptr,
                            nullptr,
                            nullptr,
                            0,
                            &pos,
                            D3DCOLOR_XRGB(255, 255, 255)
                        );
                    }
                );
        }
    };

}

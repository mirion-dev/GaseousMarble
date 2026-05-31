module;

#include <d2d1_3.h>
#include <d3dx8.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <wil/com.h>

export module gm.draw;

import std;
import gm.types;
import gm.engine;

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
        wil::com_ptr<IWICImagingFactory2> _factory_wic;
        wil::com_ptr<ID2D1Factory7> _factory_d2d;
        wil::com_ptr<IDWriteFactory7> _factory_dw;

        u32 _render_width{};
        u32 _render_height{};
        wil::com_ptr<IWICBitmap> _bitmap;
        wil::com_ptr<ID2D1RenderTarget> _render_target;
        wil::com_ptr<IDirect3DTexture8> _texture;
        wil::com_ptr<ID3DXSprite> _sprite;

        wil::com_ptr<ID2D1SolidColorBrush> _brush;
        wil::com_ptr<IDWriteTextFormat> _format;

        Setting _setting;

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

        HRESULT _update_objects() noexcept {
            RETURN_IF_FAILED(
                _factory_wic->CreateBitmap(
                    _render_width,
                    _render_height,
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapCacheOnDemand,
                    &_bitmap
                )
            );
            RETURN_IF_FAILED(
                _factory_d2d->CreateWicBitmapRenderTarget(
                    _bitmap.get(),
                    D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                    ),
                    &_render_target
                )
            );
            RETURN_IF_FAILED(
                D3DXCreateTexture(
                    Direct3D::device(),
                    _render_width,
                    _render_height,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &_texture
                )
            );
            RETURN_IF_FAILED(
                D3DXCreateSprite(
                    Direct3D::device(),
                    &_sprite
                )
            );
            return _update_brush();
        }

        HRESULT _update_brush() noexcept {
            return _render_target->CreateSolidColorBrush(_setting.color, &_brush);
        }

    public:
        Draw() {
            THROW_IF_FAILED(
                CoCreateInstance(
                    CLSID_WICImagingFactory2,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    __uuidof(*_factory_wic),
                    _factory_wic.put_void()
                )
            );
            THROW_IF_FAILED(
                D2D1CreateFactory(
                    D2D1_FACTORY_TYPE_SINGLE_THREADED,
                    &_factory_d2d
                )
            );
            THROW_IF_FAILED(
                DWriteCreateFactory(
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(*_factory_dw),
                    _factory_dw.put_unknown()
                )
            );
        }

        HRESULT text(f32 x, f32 y, std::string_view text) noexcept {
            THROW_IF_FAILED(_update_format());

            u32 render_width{ Direct3D::render_width() }, render_height{ Direct3D::render_height() };
            if (_render_width != render_width || _render_height != render_height) {
                _render_width = render_width;
                _render_height = render_height;
                RETURN_IF_FAILED(_update_objects());
            }

            std::wstring u16{ text.begin(), text.end() };
            wil::com_ptr<IDWriteTextLayout> layout;
            RETURN_IF_FAILED(
                _factory_dw->CreateTextLayout(
                    u16.data(),
                    u16.size(),
                    _format.get(),
                    _setting.box_width,
                    _setting.box_height,
                    &layout
                )
            );

            _render_target->BeginDraw();
            _render_target->Clear();
            _render_target->DrawTextLayout(
                D2D1::Point2F(_setting.origin_x, _setting.origin_y),
                layout.get(),
                _brush.get()
            );
            RETURN_IF_FAILED(_render_target->EndDraw());

            wil::com_ptr<IWICBitmapLock> bitmap_lock;
            WICRect rect{ 0, 0, static_cast<i32>(_render_width), static_cast<i32>(_render_height) };
            u32 bitmap_stride, _;
            u8* bitmap_data;
            RETURN_IF_FAILED(_bitmap->Lock(&rect, WICBitmapLockRead, &bitmap_lock));
            RETURN_IF_FAILED(bitmap_lock->GetStride(&bitmap_stride));
            RETURN_IF_FAILED(bitmap_lock->GetDataPointer(&_, &bitmap_data));

            D3DLOCKED_RECT texture_lock;
            RETURN_IF_FAILED(_texture->LockRect(0, &texture_lock, nullptr, 0));
            auto texture_stride{ static_cast<u32>(texture_lock.Pitch) };
            auto texture_data{ static_cast<u8*>(texture_lock.pBits) };

            for (u32 y{}; y < _render_height; ++y) {
                std::memcpy(texture_data + y * texture_stride, bitmap_data + y * bitmap_stride, _render_width * 4);
            }

            bitmap_lock.reset();
            RETURN_IF_FAILED(_texture->UnlockRect(0));

            D3DXVECTOR2 pos{ x, y };
            RETURN_IF_FAILED(
                _sprite->Draw(
                    _texture.get(),
                    nullptr,
                    nullptr,
                    nullptr,
                    0,
                    &pos,
                    D3DCOLOR_XRGB(255, 255, 255)
                )
            );

            return 0;
        }
    };

}

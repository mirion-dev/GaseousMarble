module;

#include <d2d1_3.h>
#include <d3d11_4.h>
#include <d3dx8.h>
#include <dwrite_3.h>
#include <wil/com.h>

export module gm.draw;

import std;
import gm.types;
import gm.env;
import gm.engine;

namespace gm {

    export class Draw {
    public:
        struct Option {
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
        Option _option;

        u32 _render_width{};
        u32 _render_height{};
        wil::com_ptr<ID3D11Texture2D1> _texture;
        wil::com_ptr<IDXGISurface2> _surface;
        wil::com_ptr<ID2D1Bitmap1> _bitmap;
        wil::com_ptr<ID3D11Texture2D1> _texture_cpu;

        wil::com_ptr<IDirect3DTexture8> _target;
        wil::com_ptr<ID3DXSprite> _sprite;

        std::wstring _text;
        wil::com_ptr<IDWriteTextFormat3> _format;
        wil::com_ptr<IDWriteTextLayout4> _layout;
        wil::com_ptr<ID2D1SolidColorBrush> _brush;

        void _update_objects() {
            D3D11_TEXTURE2D_DESC1 desc{
                .Width      = _render_width,
                .Height     = _render_height,
                .MipLevels  = 1,
                .ArraySize  = 1,
                .Format     = DXGI_FORMAT_B8G8R8A8_UNORM,
                .SampleDesc = { 1 },
                .Usage      = D3D11_USAGE_DEFAULT,
                .BindFlags  = D3D11_BIND_RENDER_TARGET
            };
            D3D11_TEXTURE2D_DESC1 desc_cpu{
                .Width          = _render_width,
                .Height         = _render_height,
                .MipLevels      = 1,
                .ArraySize      = 1,
                .Format         = DXGI_FORMAT_B8G8R8A8_UNORM,
                .SampleDesc     = { 1 },
                .Usage          = D3D11_USAGE_STAGING,
                .CPUAccessFlags = D3D11_CPU_ACCESS_READ
            };
            THROW_IF_FAILED(env::d3d_device->CreateTexture2D1(&desc, nullptr, &_texture));
            THROW_IF_FAILED(env::d3d_device->CreateTexture2D1(&desc_cpu, nullptr, &_texture_cpu));
            _surface = _texture.query<decltype(_surface)::element_type>();
            THROW_IF_FAILED(env::d2d_context->CreateBitmapFromDxgiSurface(_surface.get(), nullptr, &_bitmap));

            THROW_IF_FAILED(
                Direct3D::device()->CreateTexture(
                    _render_width,
                    _render_height,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &_target
                )
            );
            THROW_IF_FAILED(
                D3DXCreateSprite(
                    Direct3D::device(),
                    &_sprite
                )
            );
        }

        void _update_format() {
            wil::com_ptr<IDWriteTextFormat> format_base;
            THROW_IF_FAILED(
                env::dw_factory->CreateTextFormat(
                    _option.font.data(),
                    nullptr,
                    _option.weight,
                    _option.style,
                    _option.stretch,
                    _option.size,
                    _option.locale.data(),
                    &format_base
                )
            );
            _format = format_base.query<decltype(_format)::element_type>();
        }

        void _update_layout() {
            wil::com_ptr<IDWriteTextLayout> layout_base;
            THROW_IF_FAILED(
                env::dw_factory->CreateTextLayout(
                    _text.data(),
                    _text.size(),
                    _format.get(),
                    _option.box_width,
                    _option.box_height,
                    &layout_base
                )
            );
            _layout = layout_base.query<decltype(_layout)::element_type>();
        }

        void _update_brush() {
            THROW_IF_FAILED(env::d2d_context->CreateSolidColorBrush(_option.color, &_brush));
        }

    public:
        Draw() {
            _update_format();
            _update_layout();
            _update_brush();
        }

        void text(f32 x, f32 y, std::string_view text) {
            u32 render_width{ Direct3D::render_width() }, render_height{ Direct3D::render_height() };
            if (_render_width != render_width || _render_height != render_height) {
                _render_width = render_width;
                _render_height = render_height;
                _update_objects();
            }

            std::u8string text_u8{ text.begin(), text.end() };
            std::wstring text_u16{ std::filesystem::path{ text_u8 }.wstring() };
            if (_text != text_u16) {
                _text = text_u16;
                _update_layout();
            }

            {
                env::d2d_context->SetTarget(_bitmap.get());
                auto _{ wil::scope_exit([&] { env::d2d_context->SetTarget(nullptr); }) };
                env::d2d_context->BeginDraw();
                auto _2{ wil::scope_exit([&] { env::d2d_context->EndDraw(); }) };

                env::d2d_context->Clear();
                env::d2d_context->DrawTextLayout(
                    D2D1::Point2F(_option.origin_x, _option.origin_y),
                    _layout.get(),
                    _brush.get()
                );
            }

            env::d3d_context->CopyResource(_texture_cpu.get(), _texture.get());

            {
                D3D11_MAPPED_SUBRESOURCE dx11_res;
                D3DLOCKED_RECT dx8_res;
                THROW_IF_FAILED(env::d3d_context->Map(_texture_cpu.get(), 0, D3D11_MAP_READ, 0, &dx11_res));
                auto _{ wil::scope_exit([&] { env::d3d_context->Unmap(_texture_cpu.get(), 0); }) };
                THROW_IF_FAILED(_target->LockRect(0, &dx8_res, nullptr, 0));
                auto _2{ wil::scope_exit([&] { _target->UnlockRect(0); }) };

                u32 dx11_pitch{ dx11_res.RowPitch };
                auto dx11_data{ static_cast<u8*>(dx11_res.pData) };
                auto dx8_pitch{ static_cast<u32>(dx8_res.Pitch) };
                auto dx8_data{ static_cast<u8*>(dx8_res.pBits) };
                if (dx11_pitch == dx8_pitch) {
                    std::memcpy(dx8_data, dx11_data, dx11_pitch * _render_height);
                }
                else {
                    for (u32 y{}; y < _render_height; ++y) {
                        std::memcpy(dx8_data + y * dx8_pitch, dx11_data + y * dx11_pitch, _render_width * sizeof(u32));
                    }
                }
            }

            D3DXVECTOR2 pos{ x, y };
            THROW_IF_FAILED(
                _sprite->Draw(
                    _target.get(),
                    nullptr,
                    nullptr,
                    nullptr,
                    0,
                    &pos,
                    D3DCOLOR_XRGB(255, 255, 255)
                )
            );
        }
    };

}

module;

#include <d3d8.h>
#include <wil/com.h>

export module gm.draw;

import std;
import gm.types;
import gm.utils;
import gm.env;
import gm.glyph;
import gm.font;
import gm.layout;

namespace gm {

    export struct DrawOption : LayoutOption {
        // TODO: generate_font(): fill, stroke_width, stroke_fill, shadow_offset, shadow_fill
        // TODO: gm_set_color2(color_top, color_bottom)
        // TODO: gm_set_alpha(alpha)
        // TODO: gm_set_offset(x, y)
        // TODO: gm_set_scale(x, y)
        // TODO: gm_set_rotation(rotation)
    };

    export class Draw {
        struct Vertex {
            f32 x;
            f32 y;
            f32 z;
            f32 rhw;
            u32 color;
            f32 u;
            f32 v;
        };

        DrawOption _option;
        LayoutCache _layout;

    public:
        Draw(usize cache_size) noexcept
            : _layout{ cache_size } {}

        auto& option(this auto& self) noexcept {
            return std::forward_like<decltype(self)>(self._option);
        }

        void text(f32 x, f32 y, std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            Font& font{ _option.font.first->get(_option.font.second) };
            f32 texture_width{ static_cast<f32>(font.atlas.texture_width()) };
            f32 texture_height{ static_cast<f32>(font.atlas.texture_height()) };
            auto glyphs{ _layout.get({ text, _option }).glyphs };
            auto glyph_meta{ font.atlas.get(
                glyphs,
                [](const GlyphInstance& glyph) noexcept -> const GlyphDesc& { return glyph; },
                [](const GlyphInstance& glyph) noexcept -> const GlyphSpec& { return glyph; }
            ) };

            u32 color{ D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff) };
            std::unordered_map<wil::com_ptr<IDirect3DTexture8>, std::vector<Vertex>, Hash> batches;
            for (auto [glyph, meta] : std::views::zip(glyphs, glyph_meta)) {
                if (!meta->texture) {
                    continue;
                }

                f32 x1{ x + glyph.x + meta->offset_x - .5f };
                f32 y1{ y + glyph.y + meta->offset_y - .5f };
                f32 x2{ x1 + meta->width };
                f32 y2{ y1 + meta->height };
                f32 u1{ meta->x / texture_width };
                f32 v1{ meta->y / texture_height };
                f32 u2{ (meta->x + meta->width) / texture_width };
                f32 v2{ (meta->y + meta->height) / texture_height };
                Vertex a{ x1, y1, 0, 1, color, u1, v1 };
                Vertex b{ x2, y1, 0, 1, color, u2, v1 };
                Vertex c{ x1, y2, 0, 1, color, u1, v2 };
                Vertex d{ x2, y2, 0, 1, color, u2, v2 };
                batches[meta->texture].append_range(std::array{ a, b, c, d, c, b });
            }

            auto device{ env::d3d_device() };

            DWORD old_fvf;
            THROW_IF_FAILED(device->GetVertexShader(&old_fvf));
            auto fvf_guard{ wil::scope_exit([&] noexcept { device->SetVertexShader(old_fvf); }) };

            wil::com_ptr<IDirect3DBaseTexture8> old_texture;
            THROW_IF_FAILED(device->GetTexture(0, &old_texture));
            auto texture_guard{ wil::scope_exit([&] noexcept { device->SetTexture(0, old_texture.get()); }) };

            THROW_IF_FAILED(device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
            for (auto& [texture, vertices] : batches) {
                THROW_IF_FAILED(device->SetTexture(0, texture.get()));
                THROW_IF_FAILED(
                    device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vertices.size() / 3, vertices.data(), sizeof(Vertex))
                );
            }
        }

        f32 text_width(std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }
            return _layout.get({ text, _option }).width;
        }

        f32 text_height(std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }
            return _layout.get({ text, _option }).height;
        }
    };

}

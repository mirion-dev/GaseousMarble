module;

#include "log.h"

#include <cassert>
#include <d3d8.h>
#include <spdlog/spdlog.h>
#include <wil/com.h>

export module gm.draw;

import std;
import gm.log;
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
        Draw() noexcept = default;

        explicit Draw(usize cache_size) noexcept
            : _layout{ cache_size } {}

        operator bool() noexcept {
            return _layout;
        }

        auto& option(this auto& self) noexcept {
            assert(self);
            return std::forward_like<decltype(self)>(self._option);
        }

        void text(f32 x, f32 y, std::wstring_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            GlyphAtlas& atlas{ _option.font.first->atlas() };
            f32 texture_width{ static_cast<f32>(atlas.option().texture_width) };
            f32 texture_height{ static_cast<f32>(atlas.option().texture_height) };
            auto glyphs{ _layout.get({ text, _option }).glyphs };
            auto glyph_meta{ atlas.get(
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
            DWORD state_block;
            GM_THROW_IF_FAILED(device->CreateStateBlock(D3DSBT_ALL, &state_block));
            auto _{ wil::scope_exit([&] noexcept {
                device->ApplyStateBlock(state_block);
                device->DeleteStateBlock(state_block);
            }) };

            GM_THROW_IF_FAILED(device->SetPixelShader(0));
            GM_THROW_IF_FAILED(device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ALPHABLENDENABLE, true));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ALPHATESTENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CLIPPING, true));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0));
            GM_THROW_IF_FAILED(device->SetRenderState(
                D3DRS_COLORWRITEENABLE,
                D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE
                    | D3DCOLORWRITEENABLE_ALPHA
            ));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_EDGEANTIALIAS, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_FOGENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_LIGHTING, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_STENCILENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_WRAP0, 0));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ZENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ZWRITEENABLE, false));

            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE));

            for (auto& [texture, vertices] : batches) {
                GM_THROW_IF_FAILED(device->SetTexture(0, texture.get()));
                GM_THROW_IF_FAILED(
                    device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vertices.size() / 3, vertices.data(), sizeof(Vertex))
                );
            }
        }

        f32 text_width(std::wstring_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            return _layout.get({ text, _option }).width;
        }

        f32 text_height(std::wstring_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            return _layout.get({ text, _option }).height;
        }
    };

}

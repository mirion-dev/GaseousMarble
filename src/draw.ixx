module;

#include "log.h"

#include <cassert>
#include <d3d8.h>
#include <eigen3/Eigen/Geometry>
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
        Eigen::Vector2f origin{ 0, 0 };
        Eigen::Affine2f transform{ Eigen::Affine2f::Identity() };
        u32 color_tl{ 0xffffffff };
        u32 color_tr{ 0xffffffff };
        u32 color_bl{ 0xffffffff };
        u32 color_br{ 0xffffffff };
        f32 opacity{ 1 };
        // TODO: generate_font(): stroke_width, stroke_fill, shadow_offset, shadow_fill

        bool is_valid() const noexcept {
            return LayoutOption::is_valid() && 0 <= opacity && opacity <= 1;
        }
    };

    export class Draw {
        static void _delete_state_block(DWORD state_block) noexcept {
            GM_WARN_IF_FAILED(env::d3d_device()->DeleteStateBlock(state_block));
        }

        using StateBlock = Handle<DWORD, _delete_state_block, -1>;

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
        StateBlock _state_block;

        static void _init() {
            auto device{ env::d3d_device() };
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, false));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CLIPPING, true));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_EDGEANTIALIAS, false));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_LIGHTING, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_FOGENABLE, false));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ALPHATESTENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_STENCILENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ZENABLE, false));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ZWRITEENABLE, false));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_ALPHABLENDENABLE, true));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
            GM_THROW_IF_FAILED(device->SetRenderState(
                D3DRS_COLORWRITEENABLE,
                D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE
                    | D3DCOLORWRITEENABLE_ALPHA
            ));

            GM_THROW_IF_FAILED(device->SetPixelShader(0));
            GM_THROW_IF_FAILED(device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));

            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));

            GM_THROW_IF_FAILED(device->SetRenderState(D3DRS_WRAP0, 0));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE));

            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT));

            GM_THROW_IF_FAILED(device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE));
            GM_THROW_IF_FAILED(device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE));

            GM_THROW_IF_FAILED(device->SetTexture(0, nullptr));
            GM_THROW_IF_FAILED(device->SetStreamSource(0, nullptr, 0));
        }

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

        void text(Eigen::Vector2f pos, std::wstring_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            auto& [glyphs, text_aabb]{ _layout.get({ text, _option }) };
            GlyphAtlas& atlas{ _option.font.first->atlas() };
            auto glyph_meta{ atlas.get(
                glyphs,
                [](const GlyphInstance& glyph) noexcept -> const GlyphDesc& { return glyph; },
                [](const GlyphInstance& glyph) noexcept -> const GlyphSpec& { return glyph; }
            ) };

            auto device{ env::d3d_device() };
            D3DVIEWPORT8 d3d_view;
            GM_THROW_IF_FAILED(device->GetViewport(&d3d_view));

            Eigen::Vector2f view_start{ static_cast<f32>(d3d_view.X), static_cast<f32>(d3d_view.Y) };
            Eigen::Vector2f view_size{ static_cast<f32>(d3d_view.Width), static_cast<f32>(d3d_view.Height) };
            Eigen::AlignedBox2f view{ view_start, view_start + view_size };

            Eigen::Affine2f transform{ _option.transform };
            if (_option.is_box_model()) {
                transform.translate(-_option.max_size.cwiseProduct(_option.origin));
            }
            transform.pretranslate(pos - Eigen::Vector2f::Constant(.5));

            Eigen::Vector2f texture_size{ atlas.option().texture_size.cast<f32>() };
            std::unordered_map<wil::com_ptr<IDirect3DTexture8>, std::vector<Vertex>, Hash> batches;
            for (auto [glyph, meta] : std::views::zip(glyphs, glyph_meta)) {
                if (!meta->texture) {
                    continue;
                }

                Eigen::Vector2f start{ glyph.pos + meta->offset.cast<f32>() };
                Eigen::Vector2f end{ start + (meta->end - meta->start).cast<f32>() };
                Eigen::AlignedBox2f aabb{ start, end };
                if (!view.intersects(aabb.transformed(transform))) {
                    continue;
                }

                Eigen::AlignedBox2f uv_aabb{ meta->start.cast<f32>().cwiseQuotient(texture_size),
                                             meta->end.cast<f32>().cwiseQuotient(texture_size) };
                Eigen::AlignedBox2f norm_aabb{
                    (aabb.min() - text_aabb.min()).cwiseQuotient(text_aabb.sizes()).cwiseMax(0).cwiseMin(1),
                    (aabb.max() - text_aabb.min()).cwiseQuotient(text_aabb.sizes()).cwiseMax(0).cwiseMin(1)
                };
                std::array<Vertex, 4> vertices;
                for (usize i{}; i < vertices.size(); ++i) {
                    Eigen::Vector2f corner{ transform * aabb.corner(static_cast<Eigen::AlignedBox2f::CornerType>(i)) };
                    Eigen::Vector2f uv{ uv_aabb.corner(static_cast<Eigen::AlignedBox2f::CornerType>(i)) };
                    Eigen::Vector2f factor{ norm_aabb.corner(static_cast<Eigen::AlignedBox2f::CornerType>(i)) };
                    u32 color{ with_opacity(
                        bilerp_color(
                            _option.color_tl,
                            _option.color_tr,
                            _option.color_bl,
                            _option.color_br,
                            factor.x(),
                            factor.y()
                        ),
                        _option.opacity
                    ) };
                    vertices[i] = { corner.x(), corner.y(), 0, 1, color, uv.x(), uv.y() };
                }

                batches[meta->texture].append_range(
                    std::array{ vertices[0], vertices[1], vertices[2], vertices[3], vertices[2], vertices[1] }
                );
            }

            if (!_state_block) {
                StateBlock state_block;
                {
                    GM_THROW_IF_FAILED(device->BeginStateBlock());
                    auto _{ wil::scope_exit([&] noexcept { GM_WARN_IF_FAILED(device->EndStateBlock(&state_block)); }) };
                    _init();
                }
                _state_block = std::move(state_block);
            }

            GM_THROW_IF_FAILED(device->CaptureStateBlock(_state_block.get()));
            auto _{ wil::scope_exit([&] noexcept { GM_WARN_IF_FAILED(device->ApplyStateBlock(_state_block.get())); }) };
            _init();

            for (auto& [texture, vertices] : batches) {
                GM_THROW_IF_FAILED(device->SetTexture(0, texture.get()));
                GM_THROW_IF_FAILED(
                    device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vertices.size() / 3, vertices.data(), sizeof(Vertex))
                );
            }
        }

        Eigen::AlignedBox2f text_aabb(std::wstring_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            Eigen::AlignedBox2f aabb{ _layout.get({ text, _option }).aabb };
            if (_option.is_box_model()) {
                aabb.translate(-_option.max_size.cwiseProduct(_option.origin));
            }

            aabb.transform(_option.transform);
            aabb.translate(-Eigen::Vector2f::Constant(.5));
            return aabb;
        }
    };

}

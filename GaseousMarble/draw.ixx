module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <wil/com.h>
#include <wil/cppwinrt.h>

export module gm.draw;

import std;
import gm.types;
import gm.utils;
import gm.env;
import gm.engine;
import gm.font;

namespace gm {

    struct Glyph : GlyphId {
        f32 x;
        f32 y;
    };

    struct Layout {
        std::vector<Glyph> glyphs;
    };

    class LayoutCollector : public winrt::implements<LayoutCollector, IDWriteTextRenderer/*1*/> {
    public:
        STDMETHODIMP IsPixelSnappingDisabled(void*, BOOL*) noexcept {
            return S_OK;
        }

        STDMETHODIMP GetCurrentTransform(void*, DWRITE_MATRIX*) noexcept {
            return S_OK;
        }

        STDMETHODIMP GetPixelsPerDip(void*, FLOAT*) noexcept {
            return S_OK;
        }

        STDMETHODIMP DrawGlyphRun(
            void* client_drawing_context,
            FLOAT baseline_origin_x,
            FLOAT baseline_origin_y,
            DWRITE_MEASURING_MODE measuring_mode,
            const DWRITE_GLYPH_RUN* glyph_run,
            const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_description,
            IUnknown* client_drawing_effect
        ) noexcept {
            assert(client_drawing_context != nullptr);

            auto& glyphs{ static_cast<Layout*>(client_drawing_context)->glyphs };
            f32 x{ baseline_origin_x }, y{ baseline_origin_y };

            wil::com_ptr_nothrow face_base{ glyph_run->fontFace };
            wil::com_ptr<IDWriteFontFace5> face;
            RETURN_IF_FAILED(face_base.query_to<decltype(face)::element_type>(&face));

            f32 size{ glyph_run->fontEmSize };
            // ignore glyph_run->isSideways
            bool is_ltr{ glyph_run->bidiLevel % 2 == 0 };
            for (usize i{}; i < glyph_run->glyphCount; ++i) {
                u16 gid{ glyph_run->glyphIndices[i] };
                f32 advance{ glyph_run->glyphAdvances[i] };
                f32 offset_x{ glyph_run->glyphOffsets[i].advanceOffset };
                f32 offset_y{ glyph_run->glyphOffsets[i].ascenderOffset };

                if (is_ltr) {
                    glyphs.emplace_back(GlyphId{ face, size, gid }, x + offset_x, y - offset_y);
                    x += advance;
                }
                else {
                    glyphs.emplace_back(GlyphId{ face, size, gid }, x - offset_x, y - offset_y);
                    x -= advance;
                }
            }

            return S_OK;
        }

        STDMETHODIMP DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) noexcept {
            return S_OK;
        }

        STDMETHODIMP DrawStrikethrough(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) noexcept {
            return S_OK;
        }

        STDMETHODIMP DrawUnderline(void*, FLOAT, FLOAT, const DWRITE_UNDERLINE*, IUnknown*) noexcept {
            return S_OK;
        }
    };

    class LayoutCache {
        usize _cache_size{};

        std::list<std::pair<std::wstring, Layout>> _data;
        std::unordered_map<std::wstring_view, decltype(_data)::iterator> _map;

    public:
        LayoutCache() noexcept = default;

        LayoutCache(usize cache_size) noexcept :
            _cache_size{ cache_size } {

            assert(_cache_size > 0);
        }

        LayoutCache(LayoutCache&&) noexcept = default;

        LayoutCache& operator=(LayoutCache&&) noexcept = default;

        usize cache_size() const noexcept {
            assert(!empty());
            return _cache_size;
        }

        bool empty() const noexcept {
            return _cache_size == 0;
        }

        const Layout& get(std::wstring_view text, wil::com_ptr<IDWriteTextFormat3> format, f32 x, f32 y) {
            assert(!empty() && format);

            auto map_iter{ _map.find(text) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            wil::com_ptr<IDWriteTextLayout> dw_layout_base;
            THROW_IF_FAILED(
                env::dw_factory->CreateTextLayout(
                    text.data(),
                    text.size(),
                    format.get(),
                    std::numeric_limits<f32>::max(),
                    std::numeric_limits<f32>::max(),
                    &dw_layout_base
                )
            );
            wil::com_ptr dw_layout{ dw_layout_base.query<IDWriteTextLayout4>() };

            Layout layout;
            wil::com_ptr<LayoutCollector> collector;
            collector.attach(winrt::make_self<LayoutCollector>().detach());
            THROW_IF_FAILED(dw_layout->Draw(&layout, collector.get(), x, y));

            auto iter{ _data.emplace(_data.end(), text, std::move(layout)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }

        void clear() noexcept {
            _map.clear();
            _data.clear();
        }
    };

    struct Vertex {
        f32 x;
        f32 y;
        f32 z;
        f32 rhw;
        u32 color;
        f32 u;
        f32 v;
    };

    export class Draw {
        wil::com_ptr<IDWriteTextFormat3> _format;
        LayoutCache _layout{ 1024 };
        Font _font{ L"Microsoft YaHei", 100 };

        wil::com_ptr<IDWriteTextFormat3> _new_format() {
            wil::com_ptr<IDWriteTextFormat> format_base;
            THROW_IF_FAILED(
                env::dw_factory->CreateTextFormat(
                    _font.name().data(),
                    nullptr,
                    _font.weight(),
                    _font.style(),
                    _font.stretch(),
                    _font.size(),
                    _font.locale().data(),
                    &format_base
                )
            );
            return format_base.query<decltype(_format)::element_type>();
        }

    public:
        void text(f32 x, f32 y, std::string_view text) {
            if (!_format) {
                _format = _new_format();
            }

            std::unordered_map<wil::com_ptr<IDirect3DTexture8>, std::vector<Vertex>, Hash> batches;
            auto& glyphs{ _layout.get(to_wstring(text), _format, x, y).glyphs };
            auto glyph_meta{
                _font.get(
                    glyphs | std::views::transform([](const Glyph& glyph) { return static_cast<GlyphId>(glyph); })
                )
            };
            for (auto&& [glyph, meta] : std::views::zip(glyphs, glyph_meta)) {
                f32 x1{ glyph.x + meta->offset_x - .5f };
                f32 y1{ glyph.y + meta->offset_y - .5f };
                f32 x2{ x1 + meta->width };
                f32 y2{ y1 + meta->height };

                f32 width{ static_cast<f32>(_font.texture_width()) };
                f32 height{ static_cast<f32>(_font.texture_height()) };
                f32 u1{ meta->x / width };
                f32 v1{ meta->y / height };
                f32 u2{ (meta->x + meta->width) / width };
                f32 v2{ (meta->y + meta->height) / height };

                u32 color{ D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff) };
                Vertex a{ x1, y1, 0, 1, color, u1, v1 };
                Vertex b{ x2, y1, 0, 1, color, u2, v1 };
                Vertex c{ x1, y2, 0, 1, color, u1, v2 };
                Vertex d{ x2, y2, 0, 1, color, u2, v2 };
                batches[meta->texture].append_range(std::array{ a, b, c, d, c, b });
            }

            auto device{ Direct3d::device() };

            DWORD old_fvf;
            THROW_IF_FAILED(device->GetVertexShader(&old_fvf));
            auto fvf_guard{ wil::scope_exit([&] { device->SetVertexShader(old_fvf); }) };

            wil::com_ptr<IDirect3DBaseTexture8> old_texture;
            THROW_IF_FAILED(device->GetTexture(0, &old_texture));
            auto texture_guard{ wil::scope_exit([&] { device->SetTexture(0, old_texture.get()); }) };

            THROW_IF_FAILED(device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
            for (auto& [texture, vertices] : batches) {
                THROW_IF_FAILED(device->SetTexture(0, texture.get()));
                THROW_IF_FAILED(
                    device->DrawPrimitiveUP(
                        D3DPT_TRIANGLELIST,
                        vertices.size() / 3,
                        vertices.data(),
                        sizeof(Vertex)
                    )
                );
            }
        }
    };

}

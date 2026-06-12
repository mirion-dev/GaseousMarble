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
import gm.font;

namespace gm {

    struct Glyph : GlyphId {
        f32 x;
        f32 y;
    };

    struct Layout {
        std::vector<Glyph> glyphs;
    };

    class LayoutCollector : public winrt::implements<LayoutCollector, env::DwTextRenderer> {
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
            wil::com_ptr<env::DwFontFace> face;
            RETURN_IF_FAILED(face_base.query_to<env::DwFontFace>(&face));

            f32 size{ glyph_run->fontEmSize };
            // isSideways is unsupported
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

    export struct DrawOption {
        enum Alignment : u8 {
            alignment_left        = 0x0,
            alignment_center_h    = 0x1,
            alignment_right       = 0x2,
            alignment_justified_h = 0x3,

            alignment_top         = 0x0,
            alignment_center_v    = 0x4,
            alignment_bottom      = 0x8,
            alignment_justified_v = 0xc,

            alignment_mask_h = 0x3,
            alignment_mask_v = 0xc,
            alignment_mask   = 0xf
        };

        // IDWriteTextFormat
        u8 alignment;
        // WordWrapping            : unimplemented
        // ReadingDirection        : unsupported
        // FlowDirection           : unsupported
        // IncrementalTabStop      : unimplemented
        // Trimming                : unsupported
        // LineSpacing             : unimplemented

        // IDWriteTextFormat1
        // LastLineWrapping        : unsupported
        // VerticalGlyphOrientation: unsupported
        // OpticalAlignment        : unsupported
        // FontFallback            : unsupported

        // IDWriteTextFormat2
        // LineSpacing             : unimplemented

        // IDWriteTextFormat3
        // FontAxisValues          : unsupported
        // AutomaticFontAxes       : unsupported

        // IDWriteTextLayout
        f32 max_width{ std::numeric_limits<f32>::max() };
        f32 max_height{ std::numeric_limits<f32>::max() };
        std::pair<const std::string, Font>* font{};
        // Underline               : unsupported
        // Strikethrough           : unsupported
        // DrawingEffect           : unsupported
        // InlineObject            : unsupported
        // Typography              : unsupported

        // IDWriteTextLayout1
        // PairKerning             : unimplemented
        // CharacterSpacing        : unimplemented

        friend bool operator==(const DrawOption& left, const DrawOption& right) noexcept = default;

        bool is_valid() const noexcept {
            return alignment <= alignment_mask && max_width > 0 && max_height > 0 && font != nullptr;
        }
    };

    class LayoutCache {
        struct Key {
            std::wstring text;
            DrawOption option;
        };

        struct KeyRef {
            std::wstring_view text;
            const DrawOption* option;

            KeyRef(const Key& key) noexcept :
                text{ key.text },
                option{ &key.option } {}

            KeyRef(std::wstring_view text, const DrawOption& option) noexcept :
                text{ text },
                option{ &option } {}

            friend bool operator==(const KeyRef& left, const KeyRef& right) noexcept {
                return left.text == right.text && *left.option == *right.option;
            }
        };

        struct Hash : gm::Hash {
            using gm::Hash::operator();

            usize operator()(const DrawOption& value) const noexcept {
                return hash_combine(Hash{}, value.alignment, value.max_width, value.max_height, value.font);
            }

            usize operator()(const KeyRef& value) const noexcept {
                return hash_combine(Hash{}, value.text, *value.option);
            }
        };

        usize _cache_size{};

        std::list<std::pair<Key, Layout>> _data;
        std::unordered_map<KeyRef, decltype(_data)::iterator, Hash> _map;

    public:
        LayoutCache() noexcept = default;

        LayoutCache(usize cache_size) noexcept :
            _cache_size{ cache_size } {

            assert(_cache_size > 0);
        }

        LayoutCache(LayoutCache&&) noexcept = default;

        LayoutCache& operator=(LayoutCache&&) noexcept = default;

        operator bool() const noexcept {
            return _cache_size != 0;
        }

        usize cache_size() const noexcept {
            assert(*this);
            return _cache_size;
        }

        const Layout& get(std::wstring_view text, const DrawOption& option, wil::com_ptr<env::DwTextFormat> format) {
            assert(*this && option.is_valid() && format);

            auto map_iter{ _map.find({ text, option }) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            wil::com_ptr<env::DwTextLayoutBase> dw_layout_base;
            THROW_IF_FAILED(
                env::dw_factory()->CreateTextLayout(
                    text.data(),
                    text.size(),
                    format.get(),
                    std::numeric_limits<f32>::max(),
                    std::numeric_limits<f32>::max(),
                    &dw_layout_base
                )
            );
            wil::com_ptr dw_layout{ dw_layout_base.query<env::DwTextLayout>() };

            switch (option.alignment & option.alignment_mask_h) {
            case DrawOption::alignment_left:
                THROW_IF_FAILED(dw_layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
                break;
            case DrawOption::alignment_center_h:
                THROW_IF_FAILED(dw_layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
                break;
            case DrawOption::alignment_right:
                THROW_IF_FAILED(dw_layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
                break;
            case DrawOption::alignment_justified_h:
                THROW_IF_FAILED(dw_layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED));
                break;
            }

            switch (option.alignment & option.alignment_mask_v) {
            case DrawOption::alignment_top:
                THROW_IF_FAILED(dw_layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
                break;
            case DrawOption::alignment_center_v:
                THROW_IF_FAILED(dw_layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
                break;
            case DrawOption::alignment_bottom:
                THROW_IF_FAILED(dw_layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
                break;
            }

            THROW_IF_FAILED(dw_layout->SetMaxWidth(option.max_width));
            THROW_IF_FAILED(dw_layout->SetMaxHeight(option.max_height));

            DWRITE_TEXT_RANGE range{ 0, text.size() };
            THROW_IF_FAILED(dw_layout->SetFontFamilyName(option.font->second.name().data(), range));
            THROW_IF_FAILED(dw_layout->SetFontSize(option.font->second.size(), range));
            THROW_IF_FAILED(dw_layout->SetFontWeight(option.font->second.weight(), range));
            THROW_IF_FAILED(dw_layout->SetFontStyle(option.font->second.style(), range));
            THROW_IF_FAILED(dw_layout->SetFontStretch(option.font->second.stretch(), range));
            THROW_IF_FAILED(dw_layout->SetLocaleName(option.font->second.locale().data(), range));

            Layout layout;
            wil::com_ptr<LayoutCollector> collector;
            collector.attach(winrt::make_self<LayoutCollector>().detach());
            THROW_IF_FAILED(dw_layout->Draw(&layout, collector.get(), 0, 0));

            auto iter{ _data.emplace(_data.end(), Key{ std::wstring{ text }, option }, std::move(layout)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }
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
        wil::com_ptr<env::DwTextFormat> _format;
        LayoutCache _layout{ 1024 };

        static wil::com_ptr<env::DwTextFormat> _new_format() {
            wil::com_ptr<env::DwTextFormatBase> format_base;
            THROW_IF_FAILED(
                env::dw_factory()->CreateTextFormat(
                    L"Arial",
                    nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    12,
                    L"en-US",
                    &format_base
                )
            );
            return format_base.query<env::DwTextFormat>();
        }

    public:
        Draw() noexcept = default;

        Draw(Draw&&) noexcept = default;

        Draw& operator=(Draw&&) noexcept = default;

        u8 alignment() const noexcept {
            return _option.alignment;
        }

        f32 max_width() const noexcept {
            return _option.max_width;
        }

        f32 max_height() const noexcept {
            return _option.max_height;
        }

        auto font() const noexcept {
            return _option.font;
        }

        void set_alignment(u8 alignment) noexcept {
            _option.alignment = alignment;
        }

        void set_max_width(f32 max_width) noexcept {
            _option.max_width = max_width;
        }

        void set_max_height(f32 max_height) noexcept {
            _option.max_height = max_height;
        }

        void set_font(std::pair<const std::string, Font>* font) noexcept {
            _option.font = font;
        }

        void text(f32 x, f32 y, std::string_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw arguments." };
            }

            if (!_format) {
                _format = _new_format();
            }

            std::unordered_map<wil::com_ptr<IDirect3DTexture8>, std::vector<Vertex>, Hash> batches;
            auto& glyphs{ _layout.get(to_wstring(text), _option, _format).glyphs };
            auto glyph_meta{
                _option.font->second.get(
                    glyphs | std::views::transform([](const Glyph& glyph) { return static_cast<GlyphId>(glyph); })
                )
            };
            for (auto&& [glyph, meta] : std::views::zip(glyphs, glyph_meta)) {
                if (meta == nullptr) {
                    continue;
                }

                f32 x1{ x + glyph.x + meta->offset_x - .5f };
                f32 y1{ y + glyph.y + meta->offset_y - .5f };
                f32 x2{ x1 + meta->width };
                f32 y2{ y1 + meta->height };

                f32 width{ static_cast<f32>(_option.font->second.texture_width()) };
                f32 height{ static_cast<f32>(_option.font->second.texture_height()) };
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

            auto device{ env::d3d_device() };

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

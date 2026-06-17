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

    struct Glyph : GlyphRasterization {
        f32 x;
        f32 y;
    };

    struct Layout {
        std::vector<Glyph> glyphs;
        f32 width;
        f32 height;
    };

    class LayoutCollector : public winrt::implements<LayoutCollector, env::DwTextRenderer> {
    public:
        STDMETHODIMP IsPixelSnappingDisabled(void* client_drawing_context, BOOL* is_disabled) noexcept {
            *is_disabled = true;
            return S_OK;
        }

        STDMETHODIMP GetCurrentTransform(void* client_drawing_context, DWRITE_MATRIX* transform) noexcept {
            *transform = { 1, 0, 0, 1, 0, 0 };
            return S_OK;
        }

        STDMETHODIMP GetPixelsPerDip(void* client_drawing_context, FLOAT* pixels_per_dip) noexcept {
            *pixels_per_dip = 1;
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

            // Ignore isSideways because it's used for unsupported vertical writing mode
            f32 size{ glyph_run->fontEmSize };
            bool is_ltr{ glyph_run->bidiLevel % 2 == 0 };

            for (usize i{}; i < glyph_run->glyphCount; ++i) {
                u16 gid{ glyph_run->glyphIndices[i] };
                f32 advance{ glyph_run->glyphAdvances == nullptr ? 0 : glyph_run->glyphAdvances[i] };
                auto [offset_x, offset_y]{
                    glyph_run->glyphOffsets == nullptr ? DWRITE_GLYPH_OFFSET{} : glyph_run->glyphOffsets[i]
                };

                if (is_ltr) {
                    glyphs.emplace_back(GlyphRasterization{ { face, gid }, size }, x + offset_x, y - offset_y);
                    x += advance;
                }
                else {
                    x -= advance;
                    glyphs.emplace_back(GlyphRasterization{ { face, gid }, size }, x - offset_x, y - offset_y);
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

    struct LayoutOption {
        static constexpr u8 ALIGNMENT_H_MASK{ 0x3 };
        static constexpr int ALIGNMENT_H_OFFSET{};
        static constexpr u8 ALIGNMENT_V_MASK{ 0xc };
        static constexpr int ALIGNMENT_V_OFFSET{ 2 };
        static constexpr u8 ALIGNMENT_MASK{ 0xf };

        static constexpr u8 DIRECTION_H_MASK{ 0x1 };
        static constexpr u8 DIRECTION_V_MASK{ 0x2 };
        static constexpr u8 DIRECTION_MASK{ 0x3 };

        // IDWriteTextFormat
        u8 alignment{};
        // WordWrapping            : unimplemented
        u8 direction{};
        // IncrementalTabStop      : unimplemented
        // Trimming                : unimplemented

        // IDWriteTextLayout
        f32 max_width{ std::numeric_limits<f32>::max() };
        f32 max_height{ std::numeric_limits<f32>::max() };
        std::pair<const std::string, Font>* font{};
        // Underline               : Decoration is unsupported
        // Strikethrough           : Decoration is unsupported
        // DrawingEffect           : Decoration is unsupported
        // InlineObject            : Inline objects are unsupported
        // Typography              : uninvestigated

        // IDWriteTextLayout1
        bool use_pair_kerning{ true };
        f32 letter_spacing{};

        // IDWriteTextLayout2
        // VerticalGlyphOrientation: Vertical writing mode is unsupported
        // LastLineWrapping        : uninvestigated
        // OpticalAlignment        : uninvestigated
        // FontFallback            : uninvestigated

        // IDWriteTextLayout3
        bool is_fixed_line_spacing{};
        f32 line_height{ 1 };
        f32 baseline{ 1 };

        // IDWriteTextLayout4
        // FontAxisValues          : VF is unsupported
        // AutomaticFontAxes       : VF is unsupported

        friend bool operator==(const LayoutOption& left, const LayoutOption& right) noexcept = default;

        bool is_valid() const noexcept {
            return alignment <= ALIGNMENT_MASK && direction <= DIRECTION_MASK
                && max_width > 0 && max_height > 0 && font != nullptr;
        }
    };

    class LayoutCache {
        struct Key {
            std::wstring text;
            LayoutOption option;
        };

        struct KeyRef {
            std::wstring_view text;
            const LayoutOption* option;

            KeyRef(const Key& key) noexcept :
                text{ key.text },
                option{ &key.option } {}

            KeyRef(std::wstring_view text, const LayoutOption& option) noexcept :
                text{ text },
                option{ &option } {}

            friend bool operator==(const KeyRef& left, const KeyRef& right) noexcept {
                return left.text == right.text && *left.option == *right.option;
            }
        };

        struct Hash : gm::Hash {
            using gm::Hash::operator();

            usize operator()(const LayoutOption& value) const noexcept {
                return hash_combine(
                    Hash{},
                    value.alignment,
                    value.direction,
                    value.max_width,
                    value.max_height,
                    value.font,
                    value.use_pair_kerning,
                    value.letter_spacing,
                    value.is_fixed_line_spacing,
                    value.line_height,
                    value.baseline
                );
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

        const Layout& get(std::wstring_view text, const LayoutOption& option, wil::com_ptr<env::DwTextFormat> format) {
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

            f32 x{}, y{};
            DWRITE_TEXT_RANGE range{ 0, text.size() };

            auto alignment_h{ static_cast<DWRITE_TEXT_ALIGNMENT>(
                (option.alignment & LayoutOption::ALIGNMENT_H_MASK) >> LayoutOption::ALIGNMENT_H_OFFSET
            ) };
            auto alignment_v{ static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(
                (option.alignment & LayoutOption::ALIGNMENT_V_MASK) >> LayoutOption::ALIGNMENT_V_OFFSET
            ) };
            DWRITE_READING_DIRECTION direction_h{
                option.direction & LayoutOption::DIRECTION_H_MASK
                ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT
                : DWRITE_READING_DIRECTION_LEFT_TO_RIGHT
            };
            DWRITE_FLOW_DIRECTION direction_v{
                option.direction & LayoutOption::DIRECTION_V_MASK
                ? DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP
                : DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM
            };
            THROW_IF_FAILED(dw_layout->SetTextAlignment(alignment_h));
            THROW_IF_FAILED(dw_layout->SetParagraphAlignment(alignment_v));
            THROW_IF_FAILED(dw_layout->SetReadingDirection(direction_h));
            THROW_IF_FAILED(dw_layout->SetFlowDirection(direction_v));

            THROW_IF_FAILED(dw_layout->SetMaxWidth(option.max_width + option.letter_spacing));
            THROW_IF_FAILED(dw_layout->SetMaxHeight(option.max_height));
            THROW_IF_FAILED(dw_layout->SetFontFamilyName(option.font->second.name().data(), range));
            THROW_IF_FAILED(dw_layout->SetFontSize(option.font->second.size(), range));
            THROW_IF_FAILED(dw_layout->SetFontWeight(option.font->second.weight(), range));
            THROW_IF_FAILED(dw_layout->SetFontStyle(option.font->second.style(), range));
            THROW_IF_FAILED(dw_layout->SetFontStretch(option.font->second.stretch(), range));
            THROW_IF_FAILED(dw_layout->SetLocaleName(option.font->second.locale().data(), range));

            THROW_IF_FAILED(dw_layout->SetPairKerning(option.use_pair_kerning, range));
            if (alignment_h == DWRITE_TEXT_ALIGNMENT_LEADING) {
                THROW_IF_FAILED(dw_layout->SetCharacterSpacing(0, option.letter_spacing, 0, range));
            }
            else if (alignment_h == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                THROW_IF_FAILED(dw_layout->SetCharacterSpacing(option.letter_spacing, 0, 0, range));
                x -= option.letter_spacing;
            }
            else {
                THROW_IF_FAILED(
                    dw_layout->SetCharacterSpacing(
                        option.letter_spacing / 2,
                        option.letter_spacing / 2,
                        0,
                        range
                    )
                );
                x -= option.letter_spacing / 2;
            }

            DWRITE_LINE_SPACING line_spacing{
                option.is_fixed_line_spacing
                ? DWRITE_LINE_SPACING_METHOD_UNIFORM
                : DWRITE_LINE_SPACING_METHOD_PROPORTIONAL,
                option.line_height,
                option.baseline,
                0, // leadingBefore is unsupported
                DWRITE_FONT_LINE_GAP_USAGE_ENABLED
            };
            THROW_IF_FAILED(dw_layout->SetLineSpacing(&line_spacing));

            Layout layout;
            wil::com_ptr<LayoutCollector> collector;
            collector.attach(winrt::make_self<LayoutCollector>().detach());
            THROW_IF_FAILED(dw_layout->Draw(&layout, collector.get(), x, y));

            DWRITE_TEXT_METRICS metrics;
            THROW_IF_FAILED(dw_layout->GetMetrics(&metrics));

            layout.width = std::max(metrics.width - option.letter_spacing, 0.f);
            layout.height = metrics.height;

            auto iter{ _data.emplace(_data.end(), Key{ std::wstring{ text }, option }, std::move(layout)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }
    };

    export struct DrawOption : LayoutOption {};

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

        const Layout& _text_layout(std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            if (!_format) {
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
                _format = format_base.query<env::DwTextFormat>();
            }

            return _layout.get(text, _option, _format);
        }

    public:
        Draw() noexcept = default;

        Draw(Draw&&) noexcept = default;

        Draw& operator=(Draw&&) noexcept = default;

        u8 alignment() const noexcept {
            return _option.alignment;
        }

        u8 direction() const noexcept {
            return _option.direction;
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

        bool use_pair_kerning() const noexcept {
            return _option.use_pair_kerning;
        }

        f32 letter_spacing() const noexcept {
            return _option.letter_spacing;
        }

        bool is_fixed_line_spacing() const noexcept {
            return _option.is_fixed_line_spacing;
        }

        f32 line_height() const noexcept {
            return _option.line_height;
        }

        f32 baseline() const noexcept {
            return _option.baseline;
        }

        void set_alignment(u8 alignment) noexcept {
            _option.alignment = alignment;
        }

        void set_direction(u8 direction) noexcept {
            _option.direction = direction;
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

        void set_pair_kerning(bool use_pair_kerning) noexcept {
            _option.use_pair_kerning = use_pair_kerning;
        }

        void set_letter_spacing(f32 letter_spacing) noexcept {
            _option.letter_spacing = letter_spacing;
        }

        void set_fixed_line_spacing(bool is_fixed_line_spacing) noexcept {
            _option.is_fixed_line_spacing = is_fixed_line_spacing;
        }

        void set_line_height(f32 line_height) noexcept {
            _option.line_height = line_height;
        }

        void set_baseline(f32 baseline) noexcept {
            _option.baseline = baseline;
        }

        void text(f32 x, f32 y, std::wstring_view text) {
            auto glyphs{ _text_layout(text).glyphs };
            auto glyph_meta{
                _option.font->second.get(
                    glyphs,
                    [](const Glyph& glyph) -> const GlyphId& { return glyph; },
                    [](const Glyph& glyph) -> const GlyphRasterization& { return glyph; }
                )
            };

            std::unordered_map<wil::com_ptr<IDirect3DTexture8>, std::vector<Vertex>, Hash> batches;
            for (auto [glyph, meta] : std::views::zip(glyphs, glyph_meta)) {
                if (!meta->texture) {
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

        f32 text_width(std::wstring_view text) {
            return _text_layout(text).width;
        }

        f32 text_height(std::wstring_view text) {
            return _text_layout(text).height;
        }
    };

}

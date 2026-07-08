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

    // UNSUPPORTED: `DWRITE_GLYPH_RUN::isSideways`; used for vertical writing mode
    class LayoutCollector : public winrt::implements<LayoutCollector, env::DwTextRenderer> {
    public:
        STDMETHODIMP IsPixelSnappingDisabled(void* client_drawing_context, BOOL* is_disabled) noexcept {
            *is_disabled = true;
            return S_OK;
        }

        STDMETHODIMP GetCurrentTransform(void* client_drawing_context, DWRITE_MATRIX* transform) noexcept {
            *transform = { 1, 0, 0, 1 };
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
        static constexpr u8 TEXT_ALIGNMENT_MASK{ 0x3 };
        static constexpr int TEXT_ALIGNMENT_OFFSET{};
        static constexpr u8 PAR_ALIGNMENT_MASK{ 0xc };
        static constexpr int PAR_ALIGNMENT_OFFSET{ 2 };
        static constexpr u8 ALIGNMENT_MASK{ 0xf };

        static constexpr u8 TEXT_DIRECTION_MASK{ 0x3 };
        static constexpr int TEXT_DIRECTION_OFFSET{};
        static constexpr u8 PAR_DIRECTION_MASK{ 0xc };
        static constexpr int PAR_DIRECTION_OFFSET{ 2 };
        static constexpr u8 DIRECTION_MASK{ 0xf };

        // [IDWriteTextFormat]
        u8 alignment{};
        // UNSUPPORTED: `WRAP`; `EMERGENCY_BREAK` and `WHOLE_WORD` are more specific
        DWRITE_WORD_WRAPPING word_wrapping{ DWRITE_WORD_WRAPPING_WHOLE_WORD };
        // UNSUPPORTED: Enumerators used for vertical writing mode
        u8 direction{};
        f32 tab_spacing{ 48 };
        // UNSUPPORTED: `trimmingSign`; inline object
        // UNSUPPORTED: Delimiters
        DWRITE_TRIMMING_GRANULARITY trimming{};

        // [IDWriteTextLayout]
        f32 max_width{ std::numeric_limits<f32>::max() };
        f32 max_height{ std::numeric_limits<f32>::max() };
        // TODO: `FontCollection`; used for loading from font files
        std::pair<FontManager*, usize> font{};
        // UNSUPPORTED: `Underline`, `Strikethrough`, `Strikethrough`; decorations
        // UNSUPPORTED: `InlineObject`
        // UNSUPPORTED: `Typography`                                 ; advanced typography properties

        // [IDWriteTextLayout1]
        // UNSUPPORTED: `PairKerning`; advanced typography property
        f32 letter_spacing{}; // Simulated by `CharacterSpacing`

        // [IDWriteTextLayout2]
        // UNSUPPORTED: `VerticalGlyphOrientation`                            ; used for vertical writing mode
        // UNSUPPORTED: `LastLineWrapping`, `OpticalAlignment`, `FontFallback`; advanced typography properties

        // [IDWriteTextLayout3]
        // UNSUPPORTED: `DEFAULT`                          ; `PROPORTIONAL` is superior
        // UNSUPPORTED: `leadingBefore`, `fontLineGapUsage`; advanced typography properties
        DWRITE_LINE_SPACING_METHOD line_spacing_type{ DWRITE_LINE_SPACING_METHOD_PROPORTIONAL };
        f32 line_height{ 1 };
        f32 baseline{ 1 };

        // [IDWriteTextLayout4]
        // UNSUPPORTED: `FontAxisValues`   ; VF
        // UNSUPPORTED: `AutomaticFontAxes`; VF

        DWRITE_TEXT_ALIGNMENT text_alignment() const noexcept {
            return static_cast<DWRITE_TEXT_ALIGNMENT>((alignment & TEXT_ALIGNMENT_MASK) >> TEXT_ALIGNMENT_OFFSET);
        }

        DWRITE_PARAGRAPH_ALIGNMENT par_alignment() const noexcept {
            return static_cast<DWRITE_PARAGRAPH_ALIGNMENT>((alignment & PAR_ALIGNMENT_MASK) >> PAR_ALIGNMENT_OFFSET);
        }

        DWRITE_READING_DIRECTION text_direction() const noexcept {
            return static_cast<DWRITE_READING_DIRECTION>((direction & TEXT_DIRECTION_MASK) >> TEXT_DIRECTION_OFFSET);
        }

        DWRITE_FLOW_DIRECTION par_direction() const noexcept {
            return static_cast<DWRITE_FLOW_DIRECTION>((direction & PAR_DIRECTION_MASK) >> PAR_DIRECTION_OFFSET);
        }

        bool is_valid() const noexcept {
            return alignment <= ALIGNMENT_MASK
                && word_wrapping != DWRITE_WORD_WRAPPING_WRAP
                && direction <= DIRECTION_MASK
                && text_direction() != DWRITE_READING_DIRECTION_TOP_TO_BOTTOM
                && text_direction() != DWRITE_READING_DIRECTION_BOTTOM_TO_TOP
                && par_direction() != DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT
                && par_direction() != DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT
                && tab_spacing >= 0
                && max_width >= 0
                && max_height >= 0
                && font.first != nullptr
                && font.second != 0
                && line_spacing_type != DWRITE_LINE_SPACING_METHOD_DEFAULT
                && line_height >= 0;
        }

        friend bool operator==(const LayoutOption& left, const LayoutOption& right) noexcept = default;
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
                    value.word_wrapping,
                    value.direction,
                    value.tab_spacing,
                    value.trimming,
                    value.max_width,
                    value.max_height,
                    value.font.first,
                    value.font.second,
                    value.letter_spacing,
                    value.line_spacing_type,
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

        const Layout& get(std::wstring_view text, const LayoutOption& option) {
            assert(*this && option.is_valid());

            auto map_iter{ _map.find({ text, option }) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            const Font& font{ *option.font.first->get(option.font.second) };
            wil::com_ptr<IDWriteTextFormat> format_base;
            THROW_IF_FAILED(
                env::dw_factory()->CreateTextFormat(
                    font.name().data(),
                    option.font.first->collection(),
                    font.weight(),
                    font.style(),
                    font.stretch(),
                    font.size(),
                    font.locale().data(),
                    &format_base
                )
            );
            wil::com_ptr format{ format_base.query<env::DwTextFormat>() };

            wil::com_ptr<IDWriteTextLayout> layout_base;
            THROW_IF_FAILED(
                env::dw_factory()->CreateTextLayout(
                    text.data(),
                    text.size(),
                    format.get(),
                    std::numeric_limits<f32>::max(),
                    std::numeric_limits<f32>::max(),
                    &layout_base
                )
            );
            wil::com_ptr layout{ layout_base.query<env::DwTextLayout>() };

            f32 x{}, y{};
            DWRITE_TEXT_RANGE range{ 0, text.size() };

            // [IDWriteTextFormat]
            THROW_IF_FAILED(layout->SetTextAlignment(option.text_alignment()));
            THROW_IF_FAILED(layout->SetParagraphAlignment(option.par_alignment()));
            THROW_IF_FAILED(layout->SetWordWrapping(option.word_wrapping));
            THROW_IF_FAILED(layout->SetReadingDirection(option.text_direction()));
            THROW_IF_FAILED(layout->SetFlowDirection(option.par_direction()));
            THROW_IF_FAILED(layout->SetIncrementalTabStop(option.tab_spacing));

            DWRITE_TRIMMING trimming{ option.trimming };
            THROW_IF_FAILED(layout->SetTrimming(&trimming, nullptr));

            // [IDWriteTextLayout]
            THROW_IF_FAILED(layout->SetMaxWidth(option.max_width + option.letter_spacing));
            THROW_IF_FAILED(layout->SetMaxHeight(option.max_height));

            // [IDWriteTextLayout1]
            if (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_LEADING) {
                THROW_IF_FAILED(layout->SetCharacterSpacing(0, option.letter_spacing, 0, range));
            }
            else if (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                THROW_IF_FAILED(layout->SetCharacterSpacing(option.letter_spacing, 0, 0, range));
                x -= option.letter_spacing;
            }
            else {
                THROW_IF_FAILED(
                    layout->SetCharacterSpacing(
                        option.letter_spacing / 2,
                        option.letter_spacing / 2,
                        0,
                        range
                    )
                );
                x -= option.letter_spacing / 2;
            }

            // [IDWriteTextLayout3]
            DWRITE_LINE_SPACING line_spacing{
                option.line_spacing_type,
                option.line_height,
                option.baseline,
                0,
                DWRITE_FONT_LINE_GAP_USAGE_ENABLED
            };
            THROW_IF_FAILED(layout->SetLineSpacing(&line_spacing));

            Layout gm_layout;
            wil::com_ptr<LayoutCollector> collector;
            collector.attach(winrt::make_self<LayoutCollector>().detach());
            THROW_IF_FAILED(layout->Draw(&gm_layout, collector.get(), x, y));

            DWRITE_TEXT_METRICS metrics;
            THROW_IF_FAILED(layout->GetMetrics(&metrics));

            gm_layout.width = std::max(metrics.width - option.letter_spacing, 0.f);
            gm_layout.height = metrics.height;

            auto iter{ _data.emplace(_data.end(), Key{ std::wstring{ text }, option }, std::move(gm_layout)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }
    };

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
        LayoutCache _layout{ 1024 };

    public:
        Draw() noexcept = default;

        Draw(Draw&&) noexcept = default;

        Draw& operator=(Draw&&) noexcept = default;

        auto& option(this auto& self) noexcept {
            return std::forward_like<decltype(self)>(_option);
        }

        void text(f32 x, f32 y, std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }

            Font& font{ *_option.font.first->get(_option.font.second) };
            auto glyphs{ _layout.get(text, _option).glyphs };
            auto glyph_meta{
                font.get(
                    glyphs,
                    [](const Glyph& glyph) noexcept -> const GlyphId& { return glyph; },
                    [](const Glyph& glyph) noexcept -> const GlyphRasterization& { return glyph; }
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

                f32 width{ static_cast<f32>(font.texture_width()) };
                f32 height{ static_cast<f32>(font.texture_height()) };
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
            auto fvf_guard{ wil::scope_exit([&] noexcept { device->SetVertexShader(old_fvf); }) };

            wil::com_ptr<IDirect3DBaseTexture8> old_texture;
            THROW_IF_FAILED(device->GetTexture(0, &old_texture));
            auto texture_guard{ wil::scope_exit([&] noexcept { device->SetTexture(0, old_texture.get()); }) };

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
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }
            return _layout.get(text, _option).width;
        }

        f32 text_height(std::wstring_view text) {
            if (!_option.is_valid()) {
                throw std::invalid_argument{ "Invalid draw options." };
            }
            return _layout.get(text, _option).height;
        }
    };

}

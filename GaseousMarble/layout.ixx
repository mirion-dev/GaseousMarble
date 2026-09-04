module;

#include "log.h"

#include <dwrite_3.h>
#include <spdlog/spdlog.h>
#include <wil/com.h>
#include <wil/cppwinrt.h>

export module gm.layout;

import std;
import gm.log;
import gm.types;
import gm.utils;
import gm.env;
import gm.glyph;
import gm.font;

namespace gm {

    export struct GlyphInstance : GlyphSpec {
        f32 x;
        f32 y;
    };

    // UNSUPPORTED: `DWRITE_GLYPH_RUN::isSideways`; used for vertical writing mode
    class LayoutCollector : public winrt::implements<LayoutCollector, env::DwTextRenderer> {
    public:
        STDMETHODIMP IsPixelSnappingDisabled(void* client_drawing_context, BOOL* is_disabled) noexcept override {
            *is_disabled = true;
            return S_OK;
        }

        STDMETHODIMP GetCurrentTransform(void* client_drawing_context, DWRITE_MATRIX* transform) noexcept override {
            *transform = { 1, 0, 0, 1 };
            return S_OK;
        }

        STDMETHODIMP GetPixelsPerDip(void* client_drawing_context, FLOAT* pixels_per_dip) noexcept override {
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
        ) noexcept override {
            assert(client_drawing_context != nullptr);

            auto& glyphs{ *static_cast<std::vector<GlyphInstance>*>(client_drawing_context) };
            f32 x{ baseline_origin_x }, y{ baseline_origin_y };

            wil::com_ptr_nothrow face_base{ glyph_run->fontFace };
            wil::com_ptr<env::DwFontFace> face;
            GM_RETURN_IF_FAILED(face_base.query_to<env::DwFontFace>(&face));

            f32 size{ glyph_run->fontEmSize };
            bool is_ltr{ glyph_run->bidiLevel % 2 == 0 };

            for (usize i{}; i < glyph_run->glyphCount; ++i) {
                u16 gid{ glyph_run->glyphIndices[i] };
                f32 advance{ glyph_run->glyphAdvances == nullptr ? 0 : glyph_run->glyphAdvances[i] };
                auto [offset_x, offset_y]{ glyph_run->glyphOffsets == nullptr ? DWRITE_GLYPH_OFFSET{}
                                                                              : glyph_run->glyphOffsets[i] };

                if (is_ltr) {
                    glyphs.emplace_back(GlyphSpec{ face, gid, size }, x + offset_x, y - offset_y);
                    x += advance;
                } else {
                    x -= advance;
                    glyphs.emplace_back(GlyphSpec{ face, gid, size }, x - offset_x, y - offset_y);
                }
            }

            return S_OK;
        }

        STDMETHODIMP
        DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) noexcept override {
            return S_OK;
        }

        STDMETHODIMP DrawStrikethrough(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) noexcept override {
            return S_OK;
        }

        STDMETHODIMP DrawUnderline(void*, FLOAT, FLOAT, const DWRITE_UNDERLINE*, IUnknown*) noexcept override {
            return S_OK;
        }
    };

    export struct LayoutOption {
        static constexpr f32 INFINITY_{ 1e5 };

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
        f32 max_width{};
        f32 max_height{};
        std::pair<Font*, usize> font{};
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
                   && font.second > 0
                   && line_spacing_type != DWRITE_LINE_SPACING_METHOD_DEFAULT
                   && line_height >= 0;
        }

        friend bool operator==(const LayoutOption& left, const LayoutOption& right) noexcept = default;
    };

    export struct Layout {
        std::vector<GlyphInstance> glyphs;
        f32 width;
        f32 height;

        static Layout from(std::wstring_view text, const LayoutOption& option) {
            assert(option.is_valid());

            const FontDesc& font_desc{ option.font.first->desc() };
            wil::com_ptr<IDWriteTextFormat> format_base;
            GM_THROW_IF_FAILED(
                env::dw_factory()->CreateTextFormat(
                    font_desc.name.data(),
                    font_desc.collection.get(),
                    font_desc.weight(),
                    font_desc.style(),
                    font_desc.stretch(),
                    font_desc.size,
                    font_desc.locale.data(),
                    &format_base
                )
            );
            wil::com_ptr format{ format_base.query<env::DwTextFormat>() };

            wil::com_ptr<IDWriteTextLayout> layout_base;
            GM_THROW_IF_FAILED(
                env::dw_factory()->CreateTextLayout(
                    text.data(),
                    text.size(),
                    format.get(),
                    LayoutOption::INFINITY_,
                    LayoutOption::INFINITY_,
                    &layout_base
                )
            );
            wil::com_ptr layout{ layout_base.query<env::DwTextLayout>() };

            f32 x{}, y{};
            DWRITE_TEXT_RANGE range{ 0, text.size() };

            // [IDWriteTextFormat]
            GM_THROW_IF_FAILED(layout->SetTextAlignment(option.text_alignment()));
            GM_THROW_IF_FAILED(layout->SetParagraphAlignment(option.par_alignment()));
            GM_THROW_IF_FAILED(layout->SetWordWrapping(option.word_wrapping));
            GM_THROW_IF_FAILED(layout->SetReadingDirection(option.text_direction()));
            GM_THROW_IF_FAILED(layout->SetFlowDirection(option.par_direction()));
            GM_THROW_IF_FAILED(layout->SetIncrementalTabStop(option.tab_spacing));

            DWRITE_TRIMMING trimming{ option.trimming };
            GM_THROW_IF_FAILED(layout->SetTrimming(&trimming, nullptr));

            // [IDWriteTextLayout]
            f32 max_width{ option.max_width }, max_height{ option.max_height };
            if (max_width == 0) {
                max_width = LayoutOption::INFINITY_;
            }
            if (max_height == 0) {
                max_height = LayoutOption::INFINITY_;
            }
            GM_THROW_IF_FAILED(layout->SetMaxWidth(max_width + option.letter_spacing));
            GM_THROW_IF_FAILED(layout->SetMaxHeight(max_height));

            if (option.max_width == 0 || option.max_height == 0) {
                if (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_CENTER) {
                    x -= max_width / 2;
                } else if (
                    (option.text_direction() == DWRITE_READING_DIRECTION_LEFT_TO_RIGHT)
                    == (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_TRAILING)
                ) {
                    x -= max_width;
                }

                if (option.par_alignment() == DWRITE_PARAGRAPH_ALIGNMENT_CENTER) {
                    y -= max_height / 2;
                } else if (
                    (option.par_direction() == DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM)
                    == (option.par_alignment() == DWRITE_PARAGRAPH_ALIGNMENT_FAR)
                ) {
                    y -= max_height;
                }
            }

            // [IDWriteTextLayout1]
            if (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_LEADING) {
                GM_THROW_IF_FAILED(layout->SetCharacterSpacing(0, option.letter_spacing, 0, range));
            } else if (option.text_alignment() == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                GM_THROW_IF_FAILED(layout->SetCharacterSpacing(option.letter_spacing, 0, 0, range));
                x -= option.letter_spacing;
            } else {
                GM_THROW_IF_FAILED(
                    layout->SetCharacterSpacing(option.letter_spacing / 2, option.letter_spacing / 2, 0, range)
                );
                x -= option.letter_spacing / 2;
            }

            // [IDWriteTextLayout3]
            DWRITE_LINE_SPACING line_spacing{
                option.line_spacing_type, option.line_height, option.baseline, 0, DWRITE_FONT_LINE_GAP_USAGE_ENABLED
            };
            GM_THROW_IF_FAILED(layout->SetLineSpacing(&line_spacing));

            Layout result;
            wil::com_ptr<LayoutCollector> collector;
            collector.attach(winrt::make_self<LayoutCollector>().detach());
            GM_THROW_IF_FAILED(layout->Draw(&result.glyphs, collector.get(), x, y));

            DWRITE_TEXT_METRICS metrics;
            GM_THROW_IF_FAILED(layout->GetMetrics(&metrics));

            result.width = std::max(metrics.width - option.letter_spacing, 0.f);
            result.height = metrics.height;
            return result;
        }
    };

    export struct LayoutSpecRef {
        std::wstring_view text;
        const LayoutOption& option;

        friend bool operator==(const LayoutSpecRef& left, const LayoutSpecRef& right) noexcept {
            return left.text == right.text && left.option == right.option;
        }
    };

    export struct LayoutSpec {
        std::wstring text;
        LayoutOption option;

        template <std::convertible_to<LayoutSpecRef> R>
        static LayoutSpec from(R&& spec) noexcept {
            auto [text, option]{ static_cast<LayoutSpecRef>(spec) };
            return { std::wstring{ text }, option };
        }

        operator LayoutSpecRef() const noexcept {
            return { text, option };
        }
    };

    export class LayoutCache {
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
                    value.font.second,
                    value.letter_spacing,
                    value.line_spacing_type,
                    value.line_height,
                    value.baseline
                );
            }

            usize operator()(const LayoutSpecRef& value) const noexcept {
                return hash_combine(Hash{}, value.text, value.option);
            }
        };

        usize _cache_size{};

        std::list<std::pair<LayoutSpec, Layout>> _data;
        std::unordered_map<LayoutSpecRef, decltype(_data)::iterator, Hash> _map;

    public:
        LayoutCache() noexcept = default;

        explicit LayoutCache(usize cache_size) noexcept
            : _cache_size{ cache_size } {

            assert(_cache_size > 0);
        }

        LayoutCache(LayoutCache&&) noexcept = default;

        LayoutCache& operator=(LayoutCache&&) noexcept = default;

        operator bool() const noexcept {
            return _cache_size > 0;
        }

        usize cache_size() const noexcept {
            assert(*this);
            return _cache_size;
        }

        const Layout& get(const LayoutSpecRef& spec) {
            assert(*this && spec.option.is_valid());

            auto map_iter{ _map.find(spec) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            auto iter{ _data.emplace(_data.end(), LayoutSpec::from(spec), Layout::from(spec.text, spec.option)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }
    };

}

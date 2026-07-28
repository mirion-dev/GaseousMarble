module;

#include <cassert>
#include <dwrite_3.h>
#include <wil/com.h>

export module gm.font;

import std;
import gm.types;
import gm.utils;
import gm.env;
import gm.glyph;

namespace gm {

    export struct FontDesc {
        static constexpr u16 WEIGHT_MASK{ 0x3ff };
        static constexpr int WEIGHT_OFFSET{};
        static constexpr u16 STYLE_MASK{ 0xc00 };
        static constexpr int STYLE_OFFSET{ 10 };
        static constexpr u16 STRETCH_MASK{ 0xf000 };
        static constexpr int STRETCH_OFFSET{ 12 };

        std::wstring name;
        u16 properties{ DWRITE_FONT_WEIGHT_NORMAL << WEIGHT_OFFSET | DWRITE_FONT_STYLE_NORMAL << STYLE_OFFSET
                        | DWRITE_FONT_STRETCH_NORMAL << STRETCH_OFFSET };
        f32 size{};
        std::wstring locale{ L"en-US" };

        DWRITE_FONT_WEIGHT weight() const noexcept {
            return static_cast<DWRITE_FONT_WEIGHT>((properties & WEIGHT_MASK) >> WEIGHT_OFFSET);
        }

        DWRITE_FONT_STYLE style() const noexcept {
            return static_cast<DWRITE_FONT_STYLE>((properties & STYLE_MASK) >> STYLE_OFFSET);
        }

        DWRITE_FONT_STRETCH stretch() const noexcept {
            return static_cast<DWRITE_FONT_STRETCH>((properties & STRETCH_MASK) >> STRETCH_OFFSET);
        }

        bool is_valid() const noexcept {
            return weight() >= 1 && weight() <= 1000 && stretch() <= 9 && size > 0;
        }
    };

    export struct Font {
        FontDesc desc;
        GlyphAtlas atlas;
        wil::com_ptr<env::DwFontSet> set;
    };

    export class FontManager {
        usize _max_font_num{};

        std::vector<Font> _data;
        wil::com_ptr<env::DwFontCollection> _collection;

      public:
        FontManager(usize max_font_num) noexcept
            : _max_font_num{ max_font_num } {

            assert(_max_font_num > 0);
        }

        FontManager(FontManager&&) noexcept = default;

        FontManager& operator=(FontManager&&) noexcept = default;

        operator bool() const noexcept {
            return _max_font_num > 0;
        }

        usize max_font_num() const noexcept {
            assert(*this);
            return _max_font_num;
        }

        auto& get(this auto& self, usize id) noexcept {
            assert(self && id > 0 && id <= self._data.size());
            return std::forward_like<decltype(self)>(self._data[id - 1]);
        }

        env::DwFontCollection* collection() const noexcept {
            assert(*this);
            return _collection.get();
        }

        usize add(const FontDesc& desc, GlyphAtlas&& atlas) {
            assert(*this);

            if (!desc.is_valid()) {
                throw std::invalid_argument{ "Invalid font description." };
            }

            if (_data.size() >= _max_font_num) {
                throw std::runtime_error{ "Too many fonts." };
            }

            wil::com_ptr<env::DwFontSet> set;
            if (std::filesystem::is_regular_file(desc.name)) {
                wil::com_ptr<env::DwFontSetBuilder> builder;
                THROW_IF_FAILED(env::dw_factory()->CreateFontSetBuilder(&builder));

                wil::com_ptr<IDWriteFontSet> set_base;
                THROW_IF_FAILED(builder->AddFontFile(desc.name.data()));
                THROW_IF_FAILED(builder->CreateFontSet(&set_base));
                set = set_base.query<env::DwFontSet>();

                wil::com_ptr<env::DwFontSetBuilder> merge_builder;
                THROW_IF_FAILED(env::dw_factory()->CreateFontSetBuilder(&merge_builder));

                THROW_IF_FAILED(builder->AddFontSet(set.get()));
                for (const auto& [desc, atlas, set] : _data) {
                    if (set) {
                        THROW_IF_FAILED(builder->AddFontSet(set.get()));
                    }
                }

                wil::com_ptr<IDWriteFontSet> merged_set_base;
                THROW_IF_FAILED(builder->CreateFontSet(&merged_set_base));
                wil::com_ptr merged_set{ merged_set_base.query<env::DwFontSet>() };

                wil::com_ptr<IDWriteFontCollection2> collection_base;
                THROW_IF_FAILED(
                    env::dw_factory()->CreateFontCollectionFromFontSet(
                        merged_set.get(), DWRITE_FONT_FAMILY_MODEL_WEIGHT_STRETCH_STYLE, &collection_base
                    )
                );
                _collection = collection_base.query<env::DwFontCollection>();
            }

            _data.emplace_back(desc, std::move(atlas), set);
            return _data.size();
        }
    };

}

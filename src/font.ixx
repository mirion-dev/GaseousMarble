module;

#include "log.h"

#include <dwrite_3.h>
#include <spdlog/spdlog.h>
#include <wil/com.h>

export module gm.font;

import std;
import gm.log;
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

        wil::com_ptr<env::DwFontCollection> collection;
        std::wstring name;
        u16 properties{ DWRITE_FONT_WEIGHT_NORMAL << WEIGHT_OFFSET | DWRITE_FONT_STYLE_NORMAL << STYLE_OFFSET
                        | DWRITE_FONT_STRETCH_NORMAL << STRETCH_OFFSET };
        f32 size{};
        std::wstring locale{ L"en-US" };

        static FontDesc from(std::wstring_view path, u16 properties, f32 size, std::wstring_view locale) {
            wil::com_ptr<env::DwFontSetBuilder> builder;
            GM_THROW_IF_FAILED(env::dw_factory()->CreateFontSetBuilder(&builder));

            auto abs_path{ std::filesystem::absolute(path) };
            wil::com_ptr<IDWriteFontSet> set_base;
            GM_THROW_IF_FAILED(builder->AddFontFile(abs_path.c_str()));
            GM_THROW_IF_FAILED(builder->CreateFontSet(&set_base));
            wil::com_ptr set{ set_base.query<env::DwFontSet>() };

            wil::com_ptr<env::DwFontCollection> collection;
            GM_THROW_IF_FAILED(
                env::dw_factory()->CreateFontCollectionFromFontSet(
                    set.get(), DWRITE_FONT_FAMILY_MODEL_WEIGHT_STRETCH_STYLE, &collection
                )
            );

            wil::com_ptr<env::DwLocalizedStrings> names;
            BOOL exists;
            GM_THROW_IF_FAILED(
                set->GetPropertyValues(0, DWRITE_FONT_PROPERTY_ID_WEIGHT_STRETCH_STYLE_FAMILY_NAME, &exists, &names)
            );
            if (!exists) {
                throw std::runtime_error{ "Font name not found." };
            }

            std::wstring locale_name{ locale };
            u32 index;
            BOOL locale_exists;
            GM_THROW_IF_FAILED(names->FindLocaleName(locale_name.data(), &index, &locale_exists));
            if (!locale_exists && locale_name != L"en-US") {
                GM_WARN("Failed to find the specified locale, falling back to \"en-US\".");
                GM_THROW_IF_FAILED(names->FindLocaleName(L"en-US", &index, &locale_exists));
            }
            if (!locale_exists) {
                GM_WARN("Failed to find the specified locale, falling back to the first font family.");
                index = 0;
            }

            u32 name_size;
            GM_THROW_IF_FAILED(names->GetStringLength(index, &name_size));

            std::wstring name(name_size + 1, '\0');
            GM_THROW_IF_FAILED(names->GetString(index, name.data(), name.size()));
            name.pop_back();

            return { collection, std::move(name), properties, size, std::move(locale_name) };
        }

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
    };

    export class FontManager {
        static inline usize _next_id{};

        std::unordered_map<usize, Font> _data;

    public:
        FontManager() noexcept = default;

        FontManager(FontManager&&) noexcept = default;

        FontManager& operator=(FontManager&&) noexcept = default;

        template <class... Args>
        usize insert(Args&&... args) noexcept {
            return _data.try_emplace(_next_id++, std::forward<Args>(args)...).first->first;
        }

        bool erase(usize id) noexcept {
            return _data.erase(id);
        }

        Font* get(usize id) noexcept {
            auto iter{ _data.find(id) };
            return iter == _data.end() ? nullptr : &iter->second;
        }

        const Font* get(usize id) const noexcept {
            auto iter{ _data.find(id) };
            return iter == _data.end() ? nullptr : &iter->second;
        }
    };

}

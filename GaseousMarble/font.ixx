module;

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

        wil::com_ptr<env::DwFontCollection> collection;
        std::wstring name;
        u16 properties{ DWRITE_FONT_WEIGHT_NORMAL << WEIGHT_OFFSET | DWRITE_FONT_STYLE_NORMAL << STYLE_OFFSET
                        | DWRITE_FONT_STRETCH_NORMAL << STRETCH_OFFSET };
        f32 size{};
        std::wstring locale{ L"en-US" };

        static FontDesc from(std::wstring_view path, u16 properties, f32 size, std::wstring_view locale) {
            wil::com_ptr<env::DwFontSetBuilder> builder;
            THROW_IF_FAILED(env::dw_factory()->CreateFontSetBuilder(&builder));

            auto abs_path{ std::filesystem::absolute(path) };
            wil::com_ptr<IDWriteFontSet> set_base;
            THROW_IF_FAILED(builder->AddFontFile(abs_path.c_str()));
            THROW_IF_FAILED(builder->CreateFontSet(&set_base));
            wil::com_ptr set{ set_base.query<env::DwFontSet>() };

            wil::com_ptr<IDWriteFontCollection2> collection_base;
            THROW_IF_FAILED(
                env::dw_factory()->CreateFontCollectionFromFontSet(
                    set.get(), DWRITE_FONT_FAMILY_MODEL_WEIGHT_STRETCH_STYLE, &collection_base
                )
            );
            wil::com_ptr collection{ collection_base.query<env::DwFontCollection>() };

            wil::com_ptr<env::DwLocalizedStrings> names;
            BOOL exists;
            THROW_IF_FAILED(
                set->GetPropertyValues(0, DWRITE_FONT_PROPERTY_ID_WEIGHT_STRETCH_STYLE_FAMILY_NAME, &exists, &names)
            );
            if (!exists) {
                throw std::runtime_error{ "Font name not found." };
            }

            std::wstring locale_name{ locale };
            u32 index;
            BOOL locale_exists;
            THROW_IF_FAILED(names->FindLocaleName(locale_name.data(), &index, &locale_exists));
            if (!locale_exists) {
                THROW_IF_FAILED(names->FindLocaleName(L"en-US", &index, &locale_exists));
                if (!locale_exists) {
                    index = 0;
                }
            }

            u32 name_size;
            THROW_IF_FAILED(names->GetStringLength(index, &name_size));

            std::wstring name(name_size + 1, '\0');
            THROW_IF_FAILED(names->GetString(index, name.data(), name.size()));
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

    export class Font {
        static inline usize _id_counter{ 1 };

        usize _id{ _id_counter++ };
        FontDesc _desc;
        GlyphAtlas _atlas;

    public:
        Font() noexcept = default;

        Font(FontDesc desc, GlyphAtlas&& atlas) noexcept
            : _desc{ std::move(desc) }, _atlas{ std::move(atlas) } {}

        usize id() const noexcept {
            return _id;
        }

        const FontDesc& desc() const noexcept {
            return _desc;
        }

        auto& atlas(this auto& self) noexcept {
            return std::forward_like<decltype(self)>(self._atlas);
        }
    };

}

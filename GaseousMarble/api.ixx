#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

std::unordered_map<std::u8string, Font, Hash, std::equal_to<>> font_map;
Text::Option option;
Cache<std::u8string, Text, 1024> cache;

std::expected<Text, Text::Error> create_text(std::u8string_view str) noexcept {
    try {
        return cache.try_emplace(std::u8string{ str }, str, option).first->second;
    }
    catch (Text::Error error) {
        return std::unexpected{ error };
    }
}

API Real gm_font(StringView font_name, StringView sprite_path) noexcept {
    if (font_name.empty()) {
        return -100; // invalid argument
    }

    bool inserted;
    try {
        inserted = font_map.try_emplace(std::u8string{ font_name }, font_name, sprite_path).second;
    }
    catch (Font::Error error) {
        return static_cast<int>(error);
    }

    if (!inserted) {
        return 1; // font already exists
    }
    return 0;
}

API Real gm_free(StringView font_name) noexcept {
    auto iter{ font_map.find(std::u8string_view{ font_name }) };
    if (iter == font_map.end()) {
        return 1; // font not found
    }

    if (option.font == &iter->second) {
        option.font = {};
        cache.clear();
    }

    font_map.erase(iter);
    return 0;
}

API Real gm_clear() noexcept {
    option.font = {};
    cache.clear();
    font_map.clear();
    return 0;
}

API Real gm_draw(Real x, Real y, StringView str) noexcept {
    auto exp{
        create_text(str).and_then([&](const Text& text) noexcept {
            return text.draw(x, y, option);
        })
    };
    if (!exp) {
        return static_cast<int>(exp.error());
    }
    return 0;
}

API Real gm_width(StringView str) noexcept {
    auto exp{ create_text(str) };
    if (!exp) {
        return static_cast<int>(exp.error());
    }
    return exp->width();
}

API Real gm_height(StringView text) noexcept {
    auto exp{ create_text(text) };
    if (!exp) {
        return static_cast<int>(exp.error());
    }
    return exp->height();
}

API Real gm_set_font(StringView font_name) noexcept {
    auto iter{ font_map.find(std::u8string_view{ font_name }) };
    if (iter == font_map.end()) {
        return -1; // font not found
    }

    if (option.font != &iter->second) {
        option.font = &iter->second;
        cache.clear();
    }
    return 0;
}

API Real gm_set_halign(Real align) noexcept {
    option.halign = static_cast<i8>(align);
    return 0;
}

API Real gm_set_valign(Real align) noexcept {
    option.valign = static_cast<i8>(align);
    return 0;
}

API Real gm_set_justified(Real justified) noexcept {
    if (option.justified != static_cast<bool>(justified)) {
        option.justified = justified;
        cache.clear();
    }
    return 0;
}

API Real gm_set_align(Real halign, Real valign) noexcept {
    gm_set_halign(halign);
    gm_set_valign(valign);
    return 0;
}

API Real gm_set_align3(Real halign, Real valign, Real justified) noexcept {
    gm_set_align(halign, valign);
    gm_set_justified(justified);
    return 0;
}

API Real gm_set_color2(Real color_top, Real color_bottom) noexcept {
    option.color_top = static_cast<u32>(color_top);
    option.color_bottom = static_cast<u32>(color_bottom);
    return 0;
}

API Real gm_set_color(Real color) noexcept {
    gm_set_color2(color, color);
    return 0;
}

API Real gm_set_alpha(Real alpha) noexcept {
    option.alpha = alpha;
    return 0;
}

API Real gm_set_letter_spacing(Real spacing) noexcept {
    if (option.letter_spacing != spacing) {
        option.letter_spacing = spacing;
        cache.clear();
    }
    return 0;
}

API Real gm_set_word_spacing(Real spacing) noexcept {
    if (option.word_spacing != spacing) {
        option.word_spacing = spacing;
        cache.clear();
    }
    return 0;
}

API Real gm_set_paragraph_spacing(Real spacing) noexcept {
    if (option.paragraph_spacing != spacing) {
        option.paragraph_spacing = spacing;
        cache.clear();
    }
    return 0;
}

API Real gm_set_line_height(Real height) noexcept {
    if (option.line_height != height) {
        option.line_height = height;
        cache.clear();
    }
    return 0;
}

API Real gm_set_max_line_length(Real length) noexcept {
    option.max_line_length = std::max(length, 0.);
    if (option.max_line_length != length) {
        option.max_line_length = length;
        cache.clear();
    }
    return 0;
}

API Real gm_set_offset(Real x, Real y) noexcept {
    option.offset_x = x;
    option.offset_y = y;
    return 0;
}

API Real gm_set_scale(Real x, Real y) noexcept {
    if (x <= 0 || y <= 0) {
        return -1; // invalid argument(s)
    }

    if (option.scale_x != x) {
        option.scale_x = x;
        cache.clear();
    }
    option.scale_y = y;
    return 0;
}

API Real gm_set_rotation(Real theta) noexcept {
    option.rotation = theta;
    return 0;
}

API StringView gm_get_font() noexcept {
    if (option.font == nullptr) {
        return u8""; // font unspecified
    }
    return option.font->name();
}

API Real gm_get_halign() noexcept {
    return option.halign;
}

API Real gm_get_valign() noexcept {
    return option.valign;
}

API Real gm_is_justified() noexcept {
    return option.justified;
}

API Real gm_get_color_top() noexcept {
    return option.color_top;
}

API Real gm_get_color_bottom() noexcept {
    return option.color_bottom;
}

API Real gm_get_alpha() noexcept {
    return option.alpha;
}

API Real gm_get_letter_spacing() noexcept {
    return option.letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return option.word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return option.paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return option.line_height;
}

API Real gm_get_max_line_length() noexcept {
    return option.max_line_length;
}

API Real gm_get_offset_x() noexcept {
    return option.offset_x;
}

API Real gm_get_offset_y() noexcept {
    return option.offset_y;
}

API Real gm_get_scale_x() noexcept {
    return option.scale_x;
}

API Real gm_get_scale_y() noexcept {
    return option.scale_y;
}

API Real gm_get_rotation() noexcept {
    return option.rotation;
}

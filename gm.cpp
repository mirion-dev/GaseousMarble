module gm;

std::unordered_map<std::string, gm::old::draw::Font> font_map;
gm::old::draw::Draw draw;

Real gm_font(StringView name, StringView sprite_path, StringView glyph_path) noexcept {
    auto iter{ font_map.find(std::string{ name }) };
    if (iter != font_map.end()) {
        return true;
    }

    gm::old::draw::Font font{ name, sprite_path, glyph_path };
    if (!font) {
        return false;
    }

    font_map.emplace_hint(iter, name, std::move(font));
    return true;
}

Real gm_width(StringView text) noexcept {
    return draw.width(text);
}

Real gm_height(StringView text) noexcept {
    return draw.height(text);
}

Real gm_draw(Real x, Real y, StringView text) noexcept {
    return draw.text(x, y, text);
}

Real gm_free(StringView name) noexcept {
    auto iter{ font_map.find(std::string{ name }) };
    if (iter == font_map.end() || &iter->second == draw.setting().font) {
        return false;
    }

    font_map.erase(iter);
    return true;
}

Real gm_clear() noexcept {
    font_map.clear();
    return true;
}

Real gm_set_font(StringView name) noexcept {
    auto iter{ font_map.find(std::string{ name }) };
    if (iter == font_map.end()) {
        return false;
    }

    draw.setting().font = &iter->second;
    return true;
}

Real gm_set_color(Real color) noexcept {
    return gm_set_color2(color, color);
}

Real gm_set_color2(Real color_top, Real color_bottom) noexcept {
    draw.setting().color_top = static_cast<u32>(color_top);
    draw.setting().color_bottom = static_cast<u32>(color_bottom);
    return true;
}

Real gm_set_alpha(Real alpha) noexcept {
    if (alpha < 0 || alpha > 1) {
        return false;
    }
    draw.setting().alpha = alpha;
    return true;
}

Real gm_set_halign(Real align) noexcept {
    draw.setting().halign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return true;
}

Real gm_set_valign(Real align) noexcept {
    draw.setting().valign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return true;
}

Real gm_set_align(Real halign, Real valign) noexcept {
    gm_set_halign(halign);
    gm_set_valign(valign);
    return true;
}

Real gm_set_max_line_width(Real max_width) noexcept {
    if (max_width < 0) {
        return false;
    }
    draw.setting().max_line_width = std::abs(max_width);
    return true;
}

Real gm_set_letter_spacing(Real spacing) noexcept {
    draw.setting().letter_spacing = spacing;
    return true;
}

Real gm_set_word_spacing(Real spacing) noexcept {
    draw.setting().word_spacing = spacing;
    return true;
}

Real gm_set_line_height(Real height) noexcept {
    if (height <= 0) {
        return false;
    }
    draw.setting().line_height = height;
    return true;
}

Real gm_set_offset(Real x, Real y) noexcept {
    draw.setting().offset_x = x;
    draw.setting().offset_y = y;
    return true;
}

Real gm_set_scale(Real x, Real y) noexcept {
    if (x <= 0 || y <= 0) {
        return false;
    }
    draw.setting().scale_x = x;
    draw.setting().scale_y = y;
    return true;
}

const char* gm_get_font() noexcept {
    return draw.setting().font->name().data();
}

Real gm_get_color_top() noexcept {
    return draw.setting().color_top;
}

Real gm_get_color_bottom() noexcept {
    return draw.setting().color_bottom;
}

Real gm_get_alpha() noexcept {
    return draw.setting().alpha;
}

Real gm_get_halign() noexcept {
    return draw.setting().halign;
}

Real gm_get_valign() noexcept {
    return draw.setting().valign;
}

Real gm_get_max_line_width() noexcept {
    return draw.setting().max_line_width;
}

Real gm_get_letter_spacing() noexcept {
    return draw.setting().letter_spacing;
}

Real gm_get_word_spacing() noexcept {
    return draw.setting().word_spacing;
}

Real gm_get_line_height() noexcept {
    return draw.setting().line_height;
}

Real gm_get_offset_x() noexcept {
    return draw.setting().offset_x;
}

Real gm_get_offset_y() noexcept {
    return draw.setting().offset_y;
}

Real gm_get_scale_x() noexcept {
    return draw.setting().scale_x;
}

Real gm_get_scale_y() noexcept {
    return draw.setting().scale_y;
}

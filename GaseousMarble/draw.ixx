module;

#include <cassert>

export module gm.draw;

import std;
import gm.types;
import gm.env;
import gm.layout;

namespace gm {

    export struct DrawOption : LayoutOption {
        i8 halign{ -1 };
        i8 valign{ -1 };
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f32 alpha{ 1 };
        f32 offset_x{};
        f32 offset_y{};
        f32 scale_x{ 1 };
        f32 scale_y{ 1 };
        f32 rotation{};

        bool is_valid() const noexcept {
            return LayoutOption::is_valid() && scale_x > 0 && scale_y > 0;
        }
    };

    export class Draw {
        DrawOption _option;
        LayoutCache _layout;

        std::pair<f32, f32> _text_size(const Layout& layout) const noexcept {
            f32 rotation{ -_option.rotation / 180 * std::numbers::pi_v<f32> };
            f32 cos{ std::abs(std::cos(rotation)) };
            f32 sin{ std::abs(std::sin(rotation)) };
            f32 width{ std::abs(layout.width * _option.scale_x) };
            f32 height{ std::abs(layout.height * _option.scale_y) };
            return { width * cos + height * sin, width * sin + height * cos };
        }

    public:
        Draw() noexcept = default;

        explicit Draw(usize cache_size) noexcept
            : _layout{ cache_size } {}

        operator bool() const noexcept {
            return _layout;
        }

        auto& option(this auto& self) noexcept {
            assert(self);
            return std::forward_like<decltype(self)>(self._option);
        }

        void text(f32 x, f32 y, std::string_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::system_error{ LayoutError::invalid_option };
            }

            const Layout& layout{ _layout.get({ text, _option }) };
            x += _option.offset_x;
            y += _option.offset_y + _option.font.first->glyph_top();
            f32 origin_x{ x }, origin_y{ y };

            if (_option.valign == 0) {
                y -= layout.height / 2;
            } else if (_option.valign > 0) {
                y -= layout.height;
            }

            static env::Function draw_sprite_general{ env::FunctionId::draw_sprite_general };
            usize glyph_height{ _option.font.first->glyph_height() };
            usize sprite{ _option.font.first->sprite() };
            auto& glyphs{ _option.font.first->glyphs() };
            f32 rotation{ -_option.rotation / 180 * std::numbers::pi_v<f32> };
            f32 cos{ std::cos(rotation) };
            f32 sin{ std::sin(rotation) };
            for (auto& line : layout.lines) {
                f32 cursor{ x };
                if (_option.halign == 0) {
                    cursor -= line.width / 2;
                } else if (_option.halign > 0) {
                    cursor -= line.width;
                }

                for (auto& token : line.tokens) {
                    f32 token_cursor{ cursor };
                    std::string_view token_text{ layout.text.data() + token.first, token.visual_last - token.first };
                    if (!unicode_for_each(token_text, [&](const UnicodeToken& u_token) noexcept {
                            auto glyph_iter{ glyphs.find(u_token.ch) };
                            if (glyph_iter == glyphs.end()) {
                                return true;
                            }

                            auto& [sprite_x, sprite_y, width, advance, left]{ glyph_iter->second };
                            f32 draw_x{ token_cursor + left };
                            f32 draw_y{ y };
                            f32 delta_x{ (draw_x - origin_x) * _option.scale_x };
                            f32 delta_y{ (draw_y - origin_y) * _option.scale_y };
                            draw_sprite_general(
                                sprite,
                                0,
                                sprite_x,
                                sprite_y,
                                width,
                                glyph_height,
                                origin_x + delta_x * cos - delta_y * sin,
                                origin_y + delta_y * cos + delta_x * sin,
                                _option.scale_x,
                                _option.scale_y,
                                _option.rotation,
                                _option.color_top,
                                _option.color_top,
                                _option.color_bottom,
                                _option.color_bottom,
                                _option.alpha
                            );

                            token_cursor += advance + _option.letter_spacing;
                            if (is_white_space(u_token.ch)) {
                                token_cursor += _option.word_spacing;
                            }

                            return true;
                        })) {
                        throw std::system_error{ LayoutError::failed_to_decode };
                    }

                    cursor += token.advance;
                    if (_option.justified) {
                        cursor += line.justified_spacing;
                    }
                }

                y += line.height;
            }
        }

        f32 text_width(std::string_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::system_error{ LayoutError::invalid_option };
            }

            return _text_size(_layout.get({ text, _option })).first;
        }

        f32 text_height(std::string_view text) {
            assert(*this);

            if (!_option.is_valid()) {
                throw std::system_error{ LayoutError::invalid_option };
            }

            return _text_size(_layout.get({ text, _option })).second;
        }
    };

}

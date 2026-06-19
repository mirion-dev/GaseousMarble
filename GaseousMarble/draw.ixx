module;

#include <cassert>
#include <icu.h>

export module gm.draw;

import std;
import gm.types;
import gm.utils;
import gm.engine;
import gm.font;

namespace gm {

    export struct TextOption {
        const Font* font{};
        f32 letter_spacing{};
        f32 word_spacing{};
        f32 paragraph_spacing{};
        f32 line_height{ 1 };
        f32 max_line_length{};
    };

    export struct DrawOption {
        i8 halign{ -1 };
        i8 valign{ -1 };
        bool justified{};
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f32 alpha{ 1 };
        f32 offset_x{};
        f32 offset_y{};
        f32 scale_x{ 1 };
        f32 scale_y{ 1 };
        f32 rotation{};
    };

    export enum class TextError {
        failed_to_decode     = -1,
        failed_to_word_break = -2,
        invalid_option       = -3
    };

    export class Text {
        struct Token {
            std::string str;
            bool continuous;
        };

        struct Line {
            std::vector<Token> tokens;
            f32 width;
            f32 height;
            f32 justified_spacing;
        };

        struct Layout {
            std::vector<Line> lines;
            f32 width;
            f32 height;
        };

        TextOption _option;
        Layout _layout{};

    public:
        Text() noexcept = default;

        Text(std::string_view str, const TextOption& option) :
            _option{ option } {

            if (_option.font == nullptr || _option.max_line_length < 0) {
                throw TextError::invalid_option;
            }

            auto height{ static_cast<f32>(_option.font->height()) };
            auto& glyphs{ _option.font->glyphs() };

            const char* first{ str.data() };
            const char* last{ first };
            bool cont{};

            Line line{ .height = height };
            f32 cursor{};
            usize justified_count{};

            auto push_token{
                [&] noexcept {
                    if (first == last) {
                        return;
                    }

                    if (!cont) {
                        ++justified_count;
                    }
                    line.tokens.emplace_back(std::string{ first, last }, cont);
                }
            };

            auto push_line{
                [&](bool hard = false, bool last = false) noexcept {
                    push_token();

                    if (!hard && justified_count > 1) {
                        line.justified_spacing = (_option.max_line_length - line.width) / (justified_count - 1);
                        line.width = _option.max_line_length;
                    }

                    if (!last) {
                        line.height *= _option.line_height;
                    }

                    _layout.width = std::max(_layout.width, line.width);
                    _layout.height += line.height;
                    _layout.lines.emplace_back(std::move(line));

                    line = { .height = height };
                    cursor = 0;
                    justified_count = 0;
                }
            };

            auto push_word{
                [&](std::string_view word, u32 type) {
                    const char* word_begin{ word.data() };
                    const char* word_end{ word_begin + word.size() };
                    f32 next_cursor{ cursor }, next_line_width{ line.width };
                    bool first_ch{ true };
                    bool line_break{};
                    if (!unicode_for_each(
                        word,
                        [&](u32 ch) noexcept {
                            if (first_ch) {
                                first_ch = false;

                                if (is_line_break(ch)) {
                                    line.height += _option.paragraph_spacing;
                                    push_line(true);
                                    first = word_end;
                                    cont = false;
                                    line_break = true;
                                    return false;
                                }

                                bool word_cont{ type >= UBRK_WORD_KANA || type == UBRK_WORD_NONE && is_wide(ch) };
                                if (cont != word_cont) {
                                    push_token();
                                    first = word_begin;
                                    cont = word_cont;
                                }
                            }

                            auto glyph_iter{ glyphs.find(ch) };
                            if (glyph_iter == glyphs.end()) {
                                return true;
                            }

                            auto& [sprite_x, sprite_y, width, advance, left]{ glyph_iter->second };
                            if (_option.max_line_length != 0 && cursor != 0
                                && next_cursor + left + width > _option.max_line_length) {
                                next_cursor -= cursor;
                                push_line();
                                first = word_begin;
                            }

                            next_line_width = next_cursor + left + width;
                            next_cursor += advance + _option.letter_spacing;
                            if (is_white_space(ch)) {
                                next_cursor += _option.word_spacing;
                            }
                            if (cont) {
                                ++justified_count;
                            }
                            return true;
                        }
                    )) {
                        throw TextError::failed_to_decode;
                    }

                    if (!line_break) {
                        cursor = next_cursor;
                        line.width = next_line_width;
                    }

                    last = word_end;
                    return true;
                }
            };

            if (!word_break_for_each(str, push_word)) {
                throw TextError::failed_to_word_break;
            }

            push_line(true, true);
        }

        operator bool() const noexcept {
            return _option.font != nullptr;
        }

        void draw(f32 x, f32 y, const DrawOption& draw_option) const {
            assert(*this);

            if (draw_option.scale_x <= 0 || draw_option.scale_y <= 0) {
                throw TextError::invalid_option;
            }

            x += draw_option.offset_x;
            y += draw_option.offset_y + _option.font->top();
            f32 origin_x{ x }, origin_y{ y };

            if (draw_option.valign == 0) {
                y -= _layout.height / 2;
            }
            else if (draw_option.valign > 0) {
                y -= _layout.height;
            }

            static Function draw_sprite_general{ FunctionId::draw_sprite_general };
            u16 height{ _option.font->height() };
            usize sprite{ _option.font->sprite() };
            auto& glyphs{ _option.font->glyphs() };
            f32 rotation{ -draw_option.rotation / 180 * std::numbers::pi_v<f32> };
            f32 cos{ std::cos(rotation) };
            f32 sin{ std::sin(rotation) };
            for (auto& [tokens, line_width, line_height, justified_spacing] : _layout.lines) {
                f32 cursor{ x };
                if (draw_option.halign == 0) {
                    cursor -= line_width / 2;
                }
                else if (draw_option.halign > 0) {
                    cursor -= line_width;
                }

                for (auto& [str, continuous] : tokens) {
                    if (!unicode_for_each(
                        str,
                        [&](u32 ch) noexcept {
                            auto glyph_iter{ glyphs.find(ch) };
                            if (glyph_iter == glyphs.end()) {
                                return true;
                            }

                            auto& [sprite_x, sprite_y, width, advance, left]{ glyph_iter->second };
                            f32 delta_x{ cursor + left - origin_x };
                            f32 delta_y{ y - origin_y };
                            f32 draw_x{ origin_x + delta_x * cos - delta_y * sin };
                            f32 draw_y{ origin_y + delta_y * cos + delta_x * sin };
                            draw_sprite_general(
                                sprite,
                                0,
                                sprite_x,
                                sprite_y,
                                width,
                                height,
                                draw_x * draw_option.scale_x,
                                draw_y * draw_option.scale_y,
                                draw_option.scale_x,
                                draw_option.scale_y,
                                draw_option.rotation,
                                draw_option.color_top,
                                draw_option.color_top,
                                draw_option.color_bottom,
                                draw_option.color_bottom,
                                draw_option.alpha
                            );

                            cursor += advance + _option.letter_spacing;
                            if (is_white_space(ch)) {
                                cursor += _option.word_spacing;
                            }
                            if (draw_option.justified && continuous) {
                                cursor += justified_spacing;
                            }
                            return true;
                        }
                    )) {
                        throw TextError::failed_to_decode;
                    }

                    if (draw_option.justified && !continuous) {
                        cursor += justified_spacing;
                    }
                }

                y += line_height;
            }
        }

        f32 width() const noexcept {
            return std::abs(_layout.width);
        }

        f32 height() const noexcept {
            return std::abs(_layout.height);
        }
    };

    export class TextCache {
        usize _cache_size{};

        std::list<std::pair<std::string, Text>> _data;
        std::unordered_map<std::string_view, decltype(_data)::iterator> _map;

    public:
        TextCache() noexcept = default;

        TextCache(usize cache_size) noexcept :
            _cache_size{ cache_size } {

            assert(_cache_size > 0);
        }

        TextCache(TextCache&&) noexcept = default;

        TextCache& operator=(TextCache&&) noexcept = default;

        operator bool() const noexcept {
            return _cache_size != 0;
        }

        usize cache_size() const noexcept {
            assert(*this);
            return _cache_size;
        }

        const Text& get(std::string_view text, const TextOption& option) {
            assert(*this);

            auto map_iter{ _map.find(text) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            auto iter{ _data.emplace(_data.end(), text, Text{ text, option }) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }

        void clear() noexcept {
            _map.clear();
            _data.clear();
        }
    };

}

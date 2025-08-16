module;

#include <cassert>
#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;

namespace gm {

    export class SpriteHandle {
        static constexpr i32 NULL_ID{ -1 };

        i32 _id{ NULL_ID };

    public:
        SpriteHandle() noexcept = default;

        SpriteHandle(std::nullptr_t) noexcept {};

        SpriteHandle(i32 id) noexcept :
            _id{ id } {}

        operator bool() const noexcept {
            return _id != NULL_ID;
        }

        bool operator==(SpriteHandle other) const noexcept {
            return _id == other._id;
        }

        i32 id() const noexcept {
            return _id;
        }
    };

    export struct SpriteDeleter {
        using pointer = SpriteHandle;

        void operator()(pointer handle) const noexcept {
            static IFunction sprite_delete{ IFunctionResource::at(FunctionId::sprite_delete) };
            sprite_delete(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        i16 advance;
        i16 left;
    };

    export class InvalidHeaderError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    export class DataCorruptionError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    export class SpriteAddFailure : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    export class Font {
        u16 _height;
        i16 _top;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<i32, GlyphData> _glyph_data;

    public:
        Font() noexcept = default;

        Font(std::string_view font_name, std::string_view sprite_path) {
            auto glyph_path{ std::string{ sprite_path.substr(0, sprite_path.find_last_of('.')) } + ".gly" };
            std::ifstream file{ glyph_path, std::ios::binary };
            if (!file.is_open()) {
                throw std::ios_base::failure{ std::format("Unable to open the file \"{}\".", glyph_path) };
            }

            static constexpr char GLYPH_SIGN[]{ "GLY\x00\x12\x00" };
            char sign[sizeof(GLYPH_SIGN) - 1];
            file.read(sign, sizeof(sign));
            if (!file || std::strncmp(sign, GLYPH_SIGN, sizeof(sign)) != 0) {
                throw InvalidHeaderError{ std::format("Invalid file header in \"{}\".", glyph_path) };
            }

            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
            file.read(reinterpret_cast<char*>(&_top), sizeof(_top));
            while (!file.eof()) {
                u32 ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph_data[ch]), sizeof(_glyph_data[ch]));
                if (!file) {
                    throw DataCorruptionError{ std::format("File \"{}\" is corrupt.", glyph_path) };
            }
            }

            _name = font_name;

            static IFunction sprite_add{ IFunctionResource::at(FunctionId::sprite_add) };
            _sprite.reset(static_cast<i32>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            if (_sprite == nullptr) {
                throw SpriteAddFailure{ std::format("Unable to add sprite \"{}\"", sprite_path) };
        }
        }

        operator bool() const noexcept {
            return _sprite != nullptr;
        }

        bool operator==(const Font& other) const noexcept {
            return _sprite == other._sprite;
        }

        u16 height() const noexcept {
            assert(_sprite != nullptr);
            return _height;
        }

        i16 top() const noexcept {
            assert(_sprite != nullptr);
            return _top;
        }

        const std::string& name() const noexcept {
            assert(_sprite != nullptr);
            return _name;
        }

        SpriteHandle sprite() const noexcept {
            assert(_sprite != nullptr);
            return _sprite.get();
        }

        const auto& glyph_data() const noexcept {
            assert(_sprite != nullptr);
            return _glyph_data;
        }
    };

    export struct DrawSetting {
        Font* font{};
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f64 alpha{ 1 };
        i8 halign{ -1 };
        i8 valign{ -1 };
        bool justified{};
        f64 letter_spacing{};
        f64 word_spacing{};
        f64 paragraph_spacing{};
        f64 line_height{ 1 };
        f64 max_line_length{};
        f64 offset_x{};
        f64 offset_y{};
        f64 scale_x{ 1 };
        f64 scale_y{ 1 };
    };

    export class Draw {
        struct Token {
            std::u16string_view text;
            UWordBreak type;
        };

        struct Tokens {
            std::u16string text;
            std::vector<Token> tokens;
        };

        enum struct TokenizeError {
            missing_glyphs      = 1,
            success             = 0,
            invalid_encoding    = -1,
            segmentation_failed = -2
        };

        std::pair<Tokens, TokenizeError> _tokenize(std::string_view text) const noexcept {
            TokenizeError error{};
            u32 res_size{};

            auto filter{
                [&glyph_data{ setting.font->glyph_data() }](u32 ch) {
                    return glyph_data.contains(ch) || u_getIntPropertyValue(ch, UCHAR_LINE_BREAK);
                }
            };
            const char* text_ptr{ text.data() };
            u32 text_size{ text.size() };
            i32 ch;
            for (u32 i{}; i != text_size;) {
                U8_NEXT(text_ptr, i, text_size, ch);
                if (ch < 0) {
                    return { {}, TokenizeError::invalid_encoding };
                }

                if (filter(ch)) {
                    res_size += U16_LENGTH(ch);
                }
                else {
                    error = TokenizeError::missing_glyphs;
                }
            }

            std::u16string res(res_size, '\0');

            char16_t* res_ptr{ res.data() };
            for (u32 i{}, j{}; i != text_size;) {
                U8_NEXT_UNSAFE(text_ptr, i, ch);
                if (filter(ch)) {
                    U16_APPEND_UNSAFE(res_ptr, j, ch);
                }
            }

            UErrorCode icu_error{};
            std::unique_ptr<UBreakIterator, decltype(&ubrk_close)> iter{
                ubrk_open(UBRK_WORD, nullptr, res_ptr, res_size, &icu_error),
                ubrk_close
            };
            if (U_FAILURE(icu_error)) {
                return { {}, TokenizeError::segmentation_failed };
            }

            std::vector<Token> tokens;
            i32 first{ ubrk_first(iter.get()) };
            while (true) {
                i32 last{ ubrk_next(iter.get()) };
                if (last == UBRK_DONE) {
                    break;
                }

                tokens.emplace_back(
                    std::u16string_view{ res_ptr + first, res_ptr + last },
                    static_cast<UWordBreak>(ubrk_getRuleStatus(iter.get()))
                );

                first = last;
            }

            return { { std::move(res), std::move(tokens) }, error };
        }

        static std::generator<u32> _unicode_view(std::u16string_view text) noexcept {
            const char16_t* ptr{ text.data() };
            std::size_t size{ text.size() }, i{};
            u32 ch;
            while (i != size) {
                U16_NEXT(ptr, i, size, ch);
                co_yield ch;
            }
        }

        struct LineMetrics {
            std::vector<Token> tokens;
                f64 width;
                f64 height;
            f64 justified_spacing;
            };

        struct TextMetrics {
            std::u16string text;
            std::vector<LineMetrics> lines;
            f64 width;
            f64 height;
        };

        std::optional<TextMetrics> _measure(std::string_view text) const noexcept {
            auto opt_utf16{ to_utf16(text) };
            if (!opt_utf16) {
                return {};
            }
            std::u16string utf16{ std::move(*opt_utf16) };

            auto opt_tokens{ tokenize(utf16) };
            if (!opt_tokens) {
                return {};
            }
            std::vector tokens{ std::move(*opt_tokens) };

            TextMetrics metrics;

            auto& glyph_data{ setting.font->glyph_data() };
            f64 max_line_length{ setting.max_line_length / setting.scale_x };
            auto add_line{
                   // @formatter:off
                [this, max_line_length, &metrics](
                    std::u32string_view text,
                    f64 line_length,
                    f64 is_full,
                    f64 is_paragraph_end
                ) {
                    // @formatter:on
                    f64 width{ line_length };
                    f64 letter_spacing{ setting.letter_spacing };
                    if (setting.justified && max_line_length != 0 && is_full && text.size() != 1) {
                        width = max_line_length;
                        letter_spacing += (max_line_length - line_length) / (text.size() - 1);
                    }

                    f64 height{ setting.font->height() * setting.line_height };
                    if (is_paragraph_end) {
                        height += setting.paragraph_spacing;
                    }

                    metrics.lines.emplace_back(std::u32string{ text }, width, height, letter_spacing);
                    metrics.width = std::max(metrics.width, width);
                    metrics.height += height;
                }
            };

            f64 line_length{}, last_spacing{};
            auto begin{ filtered_text.begin() }, end{ filtered_text.end() }, i{ begin };
            while (i != end) {
                if (*i != '\n') {
                    auto& [glyph_x, glyph_y, glyph_width, offset_x]{ glyph_map.at(*i) };

                    f64 right{ static_cast<f64>(offset_x + glyph_width) };
                    f64 spacing{ _setting.letter_spacing };
                    if (*i == ' ') {
                        spacing += _setting.word_spacing;
                    }

                    if (max_line_length != 0 && line_length + right > max_line_length) {
                        add_line({ begin, i }, line_length - last_spacing, true, false);
                        begin = i;
                        line_length = 0;
                    }

                    line_length += right + spacing;
                    last_spacing = spacing;
                    ++i;
                }
                else {
                    add_line({ begin, i }, line_length - last_spacing, false, true);
                    begin = ++i;
                    line_length = last_spacing = 0;
                }
            }
            add_line({ begin, i }, line_length - last_spacing, false, false);

            return metrics;
        }

        void _line(f64 x, f64 y, const LineMetrics& line) const noexcept {
            static IFunction draw_sprite_general{ IFunctionResource::at(FunctionId::draw_sprite_general) };

            auto& glyph_data{ setting.font->glyph_data() };
            u16 height{ setting.font->height() };
            u32 spr_id{ setting.font->sprite().id() };

            for (auto& [text, type] : line.tokens) {
                for (u32 ch : _unicode_view(text)) {
                    auto& [spr_x, spr_y, width, advance, left]{ glyph_data.at(ch) };

                    draw_sprite_general(
                        spr_id,
                    0,
                        spr_x,
                        spr_y,
                    width,
                        height,
                    (x + left) * setting.scale_x,
                    y * setting.scale_y,
                    setting.scale_x,
                    setting.scale_y,
                    0,
                    setting.color_top,
                    setting.color_top,
                    setting.color_bottom,
                    setting.color_bottom,
                    setting.alpha
                );

                x += left + width + letter_spacing;
                if (ch == ' ') {
                    x += setting.word_spacing;
                }
            }
        }
        }

    public:
        DrawSetting setting;

        f64 width(std::string_view text) const noexcept {
            auto opt_metrics{ _measure(text) };
            return opt_metrics ? opt_metrics->width : -1;
        }

        f64 height(std::string_view text) const noexcept {
            auto opt_metrics{ _measure(text) };
            return opt_metrics ? opt_metrics->height : -1;
        }

        enum class DrawTextError {
            success          = 0,
            font_not_set     = -1,
            measuring_failed = -2
        };

        DrawTextError text(f64 x, f64 y, std::string_view text) const noexcept {
            if (setting.font == nullptr) {
                return DrawTextError::font_not_set;
            }

            auto opt_metrics{ _measure(text) };
            if (!opt_metrics) {
                return DrawTextError::measuring_failed;
            }
            TextMetrics metrics{ std::move(*opt_metrics) };

            x += setting.offset_x / setting.scale_x;
            y += (setting.offset_y + setting.font->top()) / setting.scale_y;

            if (setting.valign == 0) {
                y -= metrics.height / 2;
            }
            else if (setting.valign > 0) {
                y -= metrics.height;
            }

            for (auto& line : metrics.lines) {
                f64 xx{ x };
                if (setting.halign == 0) {
                    xx -= line.width / 2;
                }
                else if (setting.halign > 0) {
                    xx -= line.width;
                }

                _line(xx, y, line);

                y += line.height;
            }

            return DrawTextError::success;
        }
    };

}

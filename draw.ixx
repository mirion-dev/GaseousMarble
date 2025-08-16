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
            static Function sprite_delete{ Function::Id::sprite_delete };
            sprite_delete(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        i16 advance;
        i16 left;
    };

    export class Font {
        u16 _height;
        i16 _top;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<i32, GlyphData> _glyph_data;

    public:
        class InvalidHeaderError : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
        };

        class DataCorruptionError : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
        };

        class SpriteAddFailure : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
        };

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

            static Function sprite_add{ Function::Id::sprite_add };
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

    export class Draw {
    public:
        struct Setting {
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

        struct LineMetrics {
            std::vector<Token> tokens;
            f64 width;
            f64 height;
            f64 justified_spacing;
        };

        struct TextMetrics {
            std::vector<LineMetrics> lines;
            f64 width;
            f64 height;
        };

        enum class Warning {
            no_warning,
            missing_glyphs
        };

        enum class Error {
            invalid_encoding,
            tokenization_failed,
            font_not_set
        };

        template <class T>
        struct ResultWarning {
            T result;
            Warning warning;
        };

        template <class T>
        using Result = std::expected<ResultWarning<T>, Error>;

        Setting setting;

        Result<TextMetrics> measure(std::string_view text) const noexcept {
            Warning warning{};
            u32 u16_size{};

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
                    return std::unexpected{ Error::invalid_encoding };
                }

                if (filter(ch)) {
                    u16_size += U16_LENGTH(ch);
                }
                else {
                    warning = Warning::missing_glyphs;
                }
            }

            std::u16string u16(u16_size, '\0');
            char16_t* u16_ptr{ u16.data() };
            for (u32 i{}, j{}; i != text_size;) {
                U8_NEXT_UNSAFE(text_ptr, i, ch);
                if (filter(ch)) {
                    U16_APPEND_UNSAFE(u16_ptr, j, ch);
                }
            }

            auto opt_tokens{ tokenize(u16) };
            if (!opt_tokens) {
                return std::unexpected{ Error::tokenization_failed };
            }
            std::vector tokens{ std::move(*opt_tokens) };

            TextMetrics metrics{};

            // TODO

            return ResultWarning{ std::move(metrics), warning };
        }

        Result<std::monostate> text(f64 x, f64 y, std::string_view text) const noexcept {
            if (setting.font == nullptr) {
                return std::unexpected{ Error::font_not_set };
            }

            auto exp_metrics{ measure(text) };
            if (!exp_metrics) {
                return std::unexpected{ exp_metrics.error() };
            }
            auto& [metrics, warning]{ *exp_metrics };

            x += setting.offset_x / setting.scale_x;
            y += (setting.offset_y + setting.font->top()) / setting.scale_y;

            if (setting.valign == 0) {
                y -= metrics.height / 2;
            }
            else if (setting.valign > 0) {
                y -= metrics.height;
            }

            static Function draw_sprite_general{ Function::Id::draw_sprite_general };
            auto& glyph_data{ setting.font->glyph_data() };
            i32 spr_id{ setting.font->sprite().id() };
            u16 height{ setting.font->height() };
            for (auto& [tokens, line_width, line_height, justified_spacing] : metrics.lines) {
                f64 xx{ x };
                if (setting.halign == 0) {
                    xx -= line_width / 2;
                }
                else if (setting.halign > 0) {
                    xx -= line_width;
                }

                for (auto& [text, type] : tokens) {
                    for (u32 ch : unicode_view(text)) {
                        auto& [spr_x, spr_y, width, advance, left]{ glyph_data.at(ch) };

                        draw_sprite_general(
                            spr_id,
                            0,
                            spr_x,
                            spr_y,
                            width,
                            height,
                            (xx + left) * setting.scale_x,
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

                        xx += advance + setting.letter_spacing;
                        if (u_isUWhiteSpace(ch)) {
                            xx += setting.word_spacing;
                        }
                        if (type >= UBRK_WORD_KANA) {
                            xx += justified_spacing;
                        }
                    }
                    if (type < UBRK_WORD_KANA) {
                        xx += justified_spacing;
                    }
                }

                y += line_height;
            }

            return ResultWarning{ std::monostate{}, warning };
        }
    };

}

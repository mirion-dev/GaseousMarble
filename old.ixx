module;

#include <cassert>

export module gm:old;

import std;
import :core;
import :engine;

// interface of GameMaker function resources
namespace gm::old {

    enum class ValueType {
        real,
        string
    };

    class Value {
        ValueType _type;
        Real _real;
        String _string;

    public:
        Value(Real real = {}) noexcept :
            _type{ ValueType::real },
            _real{ real } {}

        Value(const String& string) noexcept :
            _type{ ValueType::string },
            _real{},
            _string{ string } {}

        operator Real() const noexcept {
            assert(_type == ValueType::real);
            return _real;
        }

        operator String() const noexcept {
            assert(_type == ValueType::string);
            return _string;
        }

        ValueType type() const noexcept {
            return _type;
        }
    };

    struct FunctionData {
        u8 name_length;
        char name[67];
        void* address;
        i32 arg_count;
        bool require_pro;
    };

    struct FunctionResource {
        FunctionData* data;
        u32 count;
    };

    export enum class FunctionId {
#include "FunctionId.inc"
    };

    export class IFunction {
        const FunctionData* _data;

    public:
        IFunction(const FunctionData* data) noexcept :
            _data{ data } {};

        std::string_view name() const noexcept {
            return { _data->name, _data->name_length };
        }

        // -1 indicates variable arguments
        i32 arg_count() const noexcept {
            return _data->arg_count;
        }

        void* address() const noexcept {
            return _data->address;
        }

        template <class R, class... Args>
        R call(Args... args) const noexcept {
            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr u32 args_count{ sizeof...(args) };
            assert(_data->arg_count == -1 || _data->arg_count == args_count);

            Value args_wrapped[]{ args... }, ret;
            Value* args_ptr{ args_wrapped };
            Value* ret_ptr{ &ret };
            void* fn_ptr{ _data->address };

            __asm {
                push args_ptr;
                push args_count;
                push ret_ptr;
                call fn_ptr;
                }

            return static_cast<R>(ret);
        }
    };

    export class IFunctionResource {
        static constexpr auto _resource{ reinterpret_cast<FunctionResource*>(0x00686b1c) };

    public:
        static IFunction at(FunctionId id) noexcept {
            return _resource->data + static_cast<i32>(id);
        }

        static u32 count() noexcept {
            return _resource->count;
        }
    };

}

// Font
namespace gm::old {

    class SpriteHandle {
        static constexpr auto _null_id{ static_cast<u32>(-1) };

        u32 _id{ _null_id };

    public:
        SpriteHandle() noexcept = default;

        SpriteHandle(std::nullptr_t) noexcept {};

        SpriteHandle(u32 id) noexcept :
            _id{ id } {}

        operator bool() const noexcept {
            return _id != _null_id;
        }

        bool operator==(SpriteHandle other) const noexcept {
            return _id == other._id;
        }

        u32 id() const noexcept {
            return _id;
        }
    };

    struct SpriteDeleter {
        using pointer = SpriteHandle;

        void operator()(pointer handle) const noexcept {
            IFunctionResource::at(FunctionId::sprite_delete).call<void, Real>(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        i16 offset_x;
    };

    export class Font {
        u16 _height;
        i16 _offset_y;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<u32, GlyphData> _glyph_map;

    public:
        Font() noexcept = default;

        Font(std::string_view name, std::string_view sprite_path) noexcept :
            _name{ name } {

            auto glyph_path{ std::string{ sprite_path.substr(0, sprite_path.find_last_of('.')) } + ".gly" };
            std::ifstream file{ glyph_path, std::ios::binary };
            if (!file) {
                return;
            }

            char magic[4];
            file.read(magic, sizeof(magic));
            if (std::strncmp(magic, "GLY", sizeof(magic)) != 0) {
                return;
            }

            _sprite.reset(
                IFunctionResource::at(FunctionId::sprite_add)
                .call<u32, String, Real, Real, Real, Real, Real>(
                    sprite_path,
                    1,
                    false,
                    false,
                    0,
                    0
                )
            );

            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
            file.read(reinterpret_cast<char*>(&_offset_y), sizeof(_offset_y));
            while (file) {
                u32 ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph_map[ch]), sizeof(_glyph_map[ch]));
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

        i16 offset_y() const noexcept {
            assert(_sprite != nullptr);
            return _offset_y;
        }

        const std::string& name() const noexcept {
            assert(_sprite != nullptr);
            return _name;
        }

        SpriteHandle sprite() const noexcept {
            assert(_sprite != nullptr);
            return _sprite.get();
        }

        const auto& glyph_map() const noexcept {
            assert(_sprite != nullptr);
            return _glyph_map;
        }
    };

}

// Draw
namespace gm::old {

    export struct DrawSetting {
        Font* font{};
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f64 alpha{ 1 };
        i8 halign{ -1 };
        i8 valign{ -1 };
        f64 letter_spacing{};
        f64 word_spacing{};
        f64 line_height{ 1 };
        f64 max_line_length{};
        f64 offset_x{};
        f64 offset_y{};
        f64 scale_x{ 1 };
        f64 scale_y{ 1 };
    };

    export class Draw {
        DrawSetting _setting;

        std::u32string _filter(std::string_view text) const noexcept {
            auto& glyph_map{ _setting.font->glyph_map() };
            return std::ranges::to<std::u32string>(
                utf8_decode(text)
                | std::views::filter([&](u32 ch) { return ch == '\n' || glyph_map.contains(ch); })
            );
        }

        auto _split(std::u32string_view text) const noexcept {
            std::vector<std::pair<std::u32string, f64>> lines;

            auto& glyph_map{ _setting.font->glyph_map() };
            f64 max_line_length{ _setting.max_line_length == 0 ? std::numeric_limits<f64>::max() : _setting.max_line_length / _setting.scale_x };

            f64 line_length{}, last_spacing{};
            auto begin{ text.begin() }, end{ text.end() }, i{ begin };
            while (i != end) {
                if (*i != '\n') {
                    auto& glyph{ glyph_map.at(*i) };
                    f64 char_width{ static_cast<f64>(glyph.offset_x + glyph.width) };
                    f64 spacing{ _setting.letter_spacing };
                    if (*i == ' ') {
                        spacing += _setting.word_spacing;
                    }

                    if (line_length + char_width > max_line_length) {
                        lines.emplace_back(std::u32string{ begin, i }, line_length - last_spacing);
                        begin = i;
                        line_length = 0;
                    }

                    line_length += char_width + spacing;
                    last_spacing = spacing;
                    ++i;
                }
                else {
                    lines.emplace_back(std::u32string{ begin, i }, line_length - last_spacing);
                    begin = ++i;
                    line_length = last_spacing = 0;
                }
            }
            lines.emplace_back(std::u32string{ begin, end }, line_length - last_spacing);

            return lines;
        }

        void _glyph(f64 x, f64 y, const GlyphData& glyph) const noexcept {
            IFunctionResource::at(FunctionId::draw_sprite_general)
                .call<void, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real>(
                    _setting.font->sprite().id(),
                    0,
                    glyph.x,
                    glyph.y,
                    glyph.width,
                    _setting.font->height(),
                    (x + glyph.offset_x) * _setting.scale_x,
                    y * _setting.scale_y,
                    _setting.scale_x,
                    _setting.scale_y,
                    0,
                    _setting.color_top,
                    _setting.color_top,
                    _setting.color_bottom,
                    _setting.color_bottom,
                    _setting.alpha
                );
        }

        void _line(f64 x, f64 y, std::u32string_view text) const noexcept {
            auto& glyph_map{ _setting.font->glyph_map() };
            for (u32 ch : text) {
                auto& glyph{ glyph_map.at(ch) };
                _glyph(x, y, glyph);

                x += glyph.offset_x + glyph.width + _setting.letter_spacing;
                if (ch == ' ') {
                    x += _setting.word_spacing;
                }
            }
        }

    public:
        auto&& setting(this auto&& self) noexcept {
            return std::forward_like<decltype(self)>(self._setting);
        }

        f64 width(std::string_view text) const noexcept {
            return std::ranges::max(_split(_filter(text)) | std::views::values) * _setting.scale_x;
        }

        f64 height(std::string_view text) const noexcept {
            return _setting.line_height * _setting.font->height() * _split(_filter(text)).size() * _setting.scale_y;
        }

        bool text(f64 x, f64 y, std::string_view text) const noexcept {
            if (_setting.font == nullptr) {
                return false;
            }

            x += _setting.offset_x;
            y += _setting.offset_y + _setting.font->offset_y();

            auto lines{ _split(_filter(text)) };
            f64 line_height{ _setting.line_height * _setting.font->height() };
            if (_setting.valign == 0) {
                y -= line_height * lines.size() / 2;
            }
            else if (_setting.valign > 0) {
                y -= line_height * lines.size();
            }

            if (_setting.halign < 0) {
                for (auto& text : lines | std::views::keys) {
                    _line(x, y, text);
                    y += line_height;
                }
            }
            else if (_setting.halign == 0) {
                for (auto& [text, width] : lines) {
                    _line(x - width / 2, y, text);
                    y += line_height;
                }
            }
            else {
                for (auto& [text, width] : lines) {
                    _line(x - width, y, text);
                    y += line_height;
                }
            }

            return true;
        }
    };

}

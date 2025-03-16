module;

#include <assert.h>
#include <d3dx8tex.h>

export module gm.engine;

import std;
import gm.core;

// Delphi UnicodeString
namespace gm::engine {

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        u32 ref_count;
        u32 size;
    };

    // used for external strings since their lifetime cannot be managed
    export
        template<class T>
    class BasicStringView {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(T) };

        const T* _data;

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        BasicStringView(const T* str = {}) noexcept :
            _data{ str } {}

        operator std::basic_string_view<T>() const noexcept {
            assert(_data != nullptr);
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            assert(_data != nullptr);
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            assert(_data != nullptr);
            return _header()->ref_count;
        }

        const T* data() const noexcept {
            return _data;
        }
    };

    // used for the implementation of Value
    export
        template<class T>
    class BasicString {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(T) };

        T* _data;

        auto _header() noexcept {
            return reinterpret_cast<StringHeader*>(_data - _offset);
        }

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        BasicString() noexcept {
            static BasicString empty_str{ "" };

            _data = empty_str._data;
            ++_header()->ref_count;
        }

        BasicString(std::basic_string_view<T> str) noexcept :
            _data{ new T[_offset + str.size() + 1] + _offset } {

            new(_header()) StringHeader{
                sizeof(T) == 1 ? 65001 : sizeof(T) == 2 ? 1200 : 12000,
                sizeof(T),
                1,
                str.size()
            };
            new(std::uninitialized_copy(str.begin(), str.end(), _data)) T{};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](_data - _offset);
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            std::swap(_data, temp._data);
            return *this;
        }

        operator BasicStringView<T>() const noexcept {
            return _data;
        }

        operator std::basic_string_view<T>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const T* data() const noexcept {
            return _data;
        }
    };

}

// fundamental types of GML
export {

    using Real = f64;

    using String = gm::engine::BasicString<char>;
    using String16 = gm::engine::BasicString<char16_t>;
    using String32 = gm::engine::BasicString<char32_t>;

    using StringView = gm::engine::BasicStringView<char>;
    using String16View = gm::engine::BasicStringView<char16_t>;
    using String32View = gm::engine::BasicStringView<char32_t>;

}

// interface of GameMaker function resources
namespace gm::engine {

    enum class ValueType {
        real,
        string
    };

    class Value {
        ValueType _type;
        Real _real;
        String _string;

    public:
        Value(Real real = 0) noexcept :
            _type{ ValueType::real },
            _real{ real } {}

        Value(String string) noexcept :
            _type{ ValueType::string },
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

    export class Function {
        u8 _name_length;
        char _name[67];
        void* _address;
        i32 _arg_count;
        bool _require_pro;

    public:
        Function() = delete;

        std::string_view name() const noexcept {
            return { _name, _name_length };
        }

        // -1 indicates variable arguments
        i32 arg_count() const noexcept {
            return _arg_count;
        }

        void* address() const noexcept {
            return _address;
        }

        template<class R, class... Args>
        R call(Args... args) const noexcept {
            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr u32 args_count{ sizeof...(args) };
            assert(_arg_count == -1 || _arg_count == args_count);

            Value args_wrapped[]{ args... }, ret;
            Value* args_ptr{ args_wrapped };
            Value* ret_ptr{ &ret };
            void* fn_ptr{ _address };

            __asm {
                push args_ptr;
                push args_count;
                push ret_ptr;
                call fn_ptr;
            }

            return static_cast<R>(ret);
        }
    };

    struct FunctionResource {
        Function* functions;
        u32 count;
    };

    export enum class FunctionId {
#include "inc/FunctionId.inc"
    };

    class IFunction {
        FunctionResource* _resource{ reinterpret_cast<FunctionResource*>(0x00686b1c) };

    public:
        IFunction() noexcept = default;

        Function& operator[](FunctionId id) const noexcept {
            return _resource->functions[static_cast<u32>(id)];
        }

        u32 count() const noexcept {
            return _resource->count;
        }
    };

    export IFunction function;

}

// interface of GameMaker texture resources
namespace gm::engine {

    export class Texture {
        IDirect3DTexture8* _data;
        gm::core::Size _image_size;
        gm::core::Size _texture_size;
        bool _is_valid;

    public:
        Texture() = delete;

        auto&& image_size(this auto& self) noexcept {
            assert(self._is_valid);
            return std::forward_like<decltype(self)>(self._image_size);
        }

        auto&& texture_size(this auto& self) noexcept {
            assert(self._is_valid);
            return std::forward_like<decltype(self)>(self._texture_size);
        }

        IDirect3DTexture8* data() noexcept {
            assert(_is_valid);
            return _data;
        }

        const IDirect3DTexture8* data() const noexcept {
            assert(_is_valid);
            return _data;
        }
    };

    class ITexture {
        Texture** _textures{ reinterpret_cast<Texture**>(0x0085b3c4) };
        u32* _count{ reinterpret_cast<u32*>(0x006886f0) };

    public:
        ITexture() noexcept = default;

        Texture& operator[](u32 id) const noexcept {
            assert(id < count());
            return (*_textures)[id];
        }

        u32 count() const noexcept {
            return *_count;
        }
    };

    export ITexture texture;

}

// interface of GameMaker sprite resources
namespace gm::engine {

    export class Bitmap {
        void* _rtti;
        gm::core::Size _size;
        void* _data;

    public:
        Bitmap() = delete;

        auto&& size(this auto&& self) noexcept {
            return std::forward_like<decltype(self)>(self._size);
        }
    };

    struct SpriteData {
        void* rtti;
        u32 subimage_count;
        Bitmap** bitmaps;
        gm::core::Point origin;
        gm::core::BoundingBox bounding_box;
        void* masks;
        bool seperate_masks;
        u32* texture_ids;
    };

    export class Sprite {
        SpriteData* _data;
        String16View _name;

    public:
        Sprite() = delete;

        Sprite(SpriteData* data, String16View name) noexcept :
            _data{ data },
            _name{ name } {}

        std::u16string_view name() const noexcept {
            return _name;
        }

        u32 subimage_count() const noexcept {
            return _data->subimage_count;
        }

        auto&& origin(this auto&& self) noexcept {
            return std::forward_like<decltype(self)>(self._data->origin);
        }

        auto&& bounding_box(this auto&& self) noexcept {
            return std::forward_like<decltype(self)>(self._data->bounding_box);
        }

        auto&& bitmap(this auto&& self, u32 index) noexcept {
            assert(index < self.subimage_count());
            return std::forward_like<decltype(self)>(*self._data->bitmaps[index]);
        }

        auto&& texture(this auto&& self, u32 index) noexcept {
            assert(index < self.subimage_count());
            return std::forward_like<decltype(self)>(gm::engine::texture[self._data->texture_ids[index]]);
        }

        void set_texture(u32 index, u32 id) noexcept {
            assert(index < subimage_count() && id < gm::engine::texture.count());
            _data->texture_ids[index] = id;
        }
    };

    struct SpriteResource {
        SpriteData** sprites;
        String16View* names;
        u32 count;
    };

    class ISprite {
        SpriteResource* _resource{ reinterpret_cast<SpriteResource*>(0x00686ac8) };

    public:
        ISprite() noexcept = default;

        Sprite operator[](u32 id) const noexcept {
            assert(id < count());
            return { _resource->sprites[id], _resource->names[id] };
        }

        u32 count() const noexcept {
            return _resource->count;
        }

        u32 find(std::u16string_view name) const noexcept {
            for (u32 id{}; id != _resource->count; ++id) {
                if (_resource->names[id] == name) {
                    return id;
                }
            }
            return -1;
        }
    };

    export ISprite sprite;

}
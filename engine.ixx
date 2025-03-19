module;

#include <assert.h>
#include <d3dx8.h>

#undef interface

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

// interface of GameMaker Direct3D resources
namespace gm::engine {

    struct Direct3DResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };

    class IDirect3D {
        Direct3DResource* _resource{ reinterpret_cast<Direct3DResource*>(0x006886a4) };

    public:
        IDirect3D() noexcept = default;

        IDirect3D8* interface() const noexcept {
            return _resource->interface;
        }

        IDirect3DDevice8* device() const noexcept {
            return _resource->device;
        }

        u32 render_width() const noexcept {
            return _resource->render_width;
        }

        u32 render_height() const noexcept {
            return _resource->render_height;
        }
    };

    export IDirect3D direct3d;

}
module;

#include <cassert>
#include <d3dx8.h>

#undef interface

export module gm:engine;

import std;
import :core;

// Delphi UnicodeString
namespace gm {

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        u32 ref_count;
        u32 size;
    };

    // used for external strings
    export template <class T>
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

    // used for implementing Value
    export template <class T>
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
export namespace gm {

    using Real = f64;

    using String = BasicString<char>;
    using String16 = BasicString<char16_t>;
    using String32 = BasicString<char32_t>;

    using StringView = BasicStringView<char>;
    using String16View = BasicStringView<char16_t>;
    using String32View = BasicStringView<char32_t>;

}

// interface of GameMaker function resources
namespace gm {

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

    class IFunction {
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

    struct FunctionResource {
        FunctionData* data;
        u32 count;
    };

    export enum class FunctionId {
#include "inc/FunctionId.inc"
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

// interface of GameMaker Direct3D resources
namespace gm {

    struct Direct3DResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };

    export class IDirect3DResource {
        static constexpr auto _resource{ reinterpret_cast<Direct3DResource*>(0x006886a4) };

    public:
        static IDirect3D8* interface() noexcept {
            return _resource->interface;
        }

        static IDirect3DDevice8* device() noexcept {
            return _resource->device;
        }

        static u32 render_width() noexcept {
            return _resource->render_width;
        }

        static u32 render_height() noexcept {
            return _resource->render_height;
        }
    };

}

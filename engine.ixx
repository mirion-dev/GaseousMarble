module;

#include <cassert>

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

    export template <class Ch>
    class BasicStringView {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(Ch) };

        const Ch* _data;

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        // `str` must point to a Delphi UnicodeString structure or accessing its header is undefined
        BasicStringView(std::basic_string_view<Ch> str = {}) noexcept :
            _data{ str.data() } {}

        operator std::basic_string_view<Ch>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const Ch* data() const noexcept {
            return _data;
        }
    };

    export template <class Ch>
    class BasicString {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(Ch) };
        static constexpr u16 _cp_utf8{ 65001 };
        static constexpr u16 _cp_utf16{ 1200 };
        static constexpr u16 _cp_utf32{ 12000 };

        Ch* _data;

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

        BasicString(std::basic_string_view<Ch> str) noexcept :
            _data{ new Ch[_offset + str.size() + 1] + _offset } {

            new(_header()) StringHeader{
                sizeof(Ch) == 1 ? _cp_utf8 : sizeof(Ch) == 2 ? _cp_utf16 : _cp_utf32,
                sizeof(Ch),
                1,
                str.size()
            };
            new(std::uninitialized_copy(str.begin(), str.end(), _data)) Ch{};
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

        operator BasicStringView<Ch>() const noexcept {
            return _data;
        }

        operator std::basic_string_view<Ch>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const Ch* data() const noexcept {
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

    struct FunctionResource {
        FunctionData* data;
        u32 count;
    };

    export enum class FunctionId {
#include "FunctionId.inc"
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

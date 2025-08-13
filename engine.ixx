module;

#include <cassert>

export module gm:engine;

import std;
import :core;

namespace gm {

    // ------------------------
    // fundamental types of GML
    // ------------------------

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        u32 ref_count;
        u32 size;
    };

    export template <class Ch>
    class BasicStringView {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(Ch) };

        const Ch* _data{};

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        BasicStringView() noexcept = default;

        BasicStringView(std::nullptr_t) noexcept = delete;

        // `str` must point to a Delphi UnicodeString structure or accessing its header is undefined
        BasicStringView(const std::convertible_to<std::basic_string_view<Ch>> auto& str) noexcept :
            _data{ static_cast<std::basic_string_view<Ch>>(str).data() } {}

        operator std::basic_string_view<Ch>() const noexcept {
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

        BasicString(std::nullptr_t) noexcept = delete;

        BasicString(const std::convertible_to<std::basic_string_view<Ch>> auto& str) noexcept {
            auto view{ static_cast<std::basic_string_view<Ch>>(str) };

            _data = new Ch[_offset + view.size() + 1] + _offset;

            new(_header()) StringHeader{
                sizeof(Ch) == 1 ? _cp_utf8 : sizeof(Ch) == 2 ? _cp_utf16 : _cp_utf32,
                sizeof(Ch),
                1,
                view.size()
            };
            new(std::uninitialized_copy(view.begin(), view.end(), _data)) Ch{};
        }

        BasicString(BasicStringView<Ch> str) noexcept :
            _data{ new Ch[_offset + str.size() + 1] + _offset } {

            std::uninitialized_copy(str.data() - _offset, str.data() + str.size() + 1, _data - _offset);
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

        operator std::basic_string_view<Ch>() const noexcept {
            return { _data, _header()->size };
        }

        operator BasicStringView<Ch>() const noexcept {
            return _data;
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

    export {

        using Real = f64;

        using String = BasicString<char>;
        using StringView = BasicStringView<char>;

    }

    // -----------------------------------------
    // interface of GameMaker function resources
    // -----------------------------------------

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

        Value(StringView string) noexcept :
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
            _data{ data } {}

        Value operator()(const auto&... args) const noexcept {
            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr u32 args_count{ sizeof...(args) };
            assert(_data->arg_count == -1 || _data->arg_count == args_count);

            Value args_wrapped[]{ static_cast<Value>(args)... }, ret;
            Value* args_ptr{ args_wrapped };
            Value* ret_ptr{ &ret };
            void* fn_ptr{ _data->address };

            // @formatter:off
            __asm {
                push args_ptr;
                push args_count;
                push ret_ptr;
                call fn_ptr;
            }
            // @formatter:on

            return ret;
        }

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

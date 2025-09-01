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
        static constexpr u32 HEADER_SIZE{ sizeof(StringHeader) / sizeof(Ch) };

        const Ch* _data{};

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE);
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
        static constexpr u32 HEADER_SIZE{ sizeof(StringHeader) / sizeof(Ch) };
        static constexpr u16 CP_UTF8{ 65001 };
        static constexpr u16 CP_UTF16{ 1200 };
        static constexpr u16 CP_UTF32{ 12000 };

        Ch* _data;

        auto _header() noexcept {
            return reinterpret_cast<StringHeader*>(_data - HEADER_SIZE);
        }

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE);
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

            _data = new Ch[HEADER_SIZE + view.size() + 1] + HEADER_SIZE;

            new(_header()) StringHeader{
                sizeof(Ch) == 1 ? CP_UTF8 : sizeof(Ch) == 2 ? CP_UTF16 : CP_UTF32,
                sizeof(Ch),
                1,
                view.size()
            };
            new(std::uninitialized_copy(view.begin(), view.end(), _data)) Ch{};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](_data - HEADER_SIZE);
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

    class Value {
    public:
        enum class Type {
            real,
            string
        };

    private:
        Type _type;
        Real _real;
        String _string;

    public:
        Value(Real real = {}) noexcept :
            _type{ Type::real },
            _real{ real } {}

        Value(StringView string) noexcept :
            _type{ Type::string },
            _real{},
            _string{ string } {}

        operator Real() const noexcept {
            assert(_type == Type::real);
            return _real;
        }

        operator String() const noexcept {
            assert(_type == Type::string);
            return _string;
        }

        Type type() const noexcept {
            return _type;
        }
    };

    export class Function {
        struct Data {
            u8 name_length;
            char name[67];
            void* address;
            i32 arg_count;
            bool require_pro;
        };

        struct Resource {
            Data* data;
            u32 count;
        };

        static constexpr auto RESOURCE_PTR{ reinterpret_cast<Resource*>(0x00686b1c) };

        const Data* _data;

    public:
        enum class Id {
#include "detail/FunctionId.inc"
        };

        Function(Id id) noexcept :
            _data{ RESOURCE_PTR->data + static_cast<u32>(id) } {}

        static u32 count() noexcept {
            return RESOURCE_PTR->count;
        }

        Value operator()(const auto&... args) const noexcept {
            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr u32 ARGS_COUNT{ sizeof...(args) };
            assert(_data->arg_count == -1 || _data->arg_count == ARGS_COUNT);

            Value args_wrapped[]{ static_cast<Value>(args)... }, ret;
            Value* args_ptr{ args_wrapped };
            Value* ret_ptr{ &ret };
            void* fn_ptr{ _data->address };

            // @formatter:off
            __asm {
                push args_ptr;
                push ARGS_COUNT;
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

}

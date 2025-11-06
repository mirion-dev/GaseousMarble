module;

#include <cassert>

export module gm:engine;

import std;
import :core;

namespace gm {

#pragma region fundamental types of GML

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        usize ref_count;
        usize size;
    };

    export template <class C>
    class BasicStringView {
        static constexpr usize HEADER_SIZE{ sizeof(StringHeader) / sizeof(C) };

        const C* _data{};

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE);
        }

    public:
        BasicStringView() noexcept = default;

        BasicStringView(std::nullptr_t) noexcept = delete;

        // `str` must point to a Delphi UnicodeString structure or accessing its header is undefined
        BasicStringView(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept :
            _data{ static_cast<std::basic_string_view<C>>(str).data() } {}

        operator std::basic_string_view<C>() const noexcept {
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

        const C* data() const noexcept {
            return _data;
        }
    };

    export template <class C>
    class BasicString {
        static constexpr usize HEADER_SIZE{ sizeof(StringHeader) / sizeof(C) };
        static constexpr u16 CP_UTF8{ 65001 };
        static constexpr u16 CP_UTF16{ 1200 };
        static constexpr u16 CP_UTF32{ 12000 };

        C* _data;

        auto _header() noexcept {
            return reinterpret_cast<StringHeader*>(_data - HEADER_SIZE);
        }

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE);
        }

    public:
        BasicString() noexcept {
            static BasicString empty_str{ u8"" };
            _data = empty_str._data;

            ++_header()->ref_count;
        }

        BasicString(std::nullptr_t) noexcept = delete;

        BasicString(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept {
            auto view{ static_cast<std::basic_string_view<C>>(str) };

            _data = new C[HEADER_SIZE + view.size() + 1] + HEADER_SIZE;

            new(_header()) StringHeader{
                sizeof(C) == 1 ? CP_UTF8 : sizeof(C) == 2 ? CP_UTF16 : CP_UTF32,
                sizeof(C),
                1,
                view.size()
            };
            new(std::uninitialized_copy(view.begin(), view.end(), _data)) C{};
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

        operator std::basic_string_view<C>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    export {

        using Real = f64;

        using String = BasicString<c8>;
        using StringView = BasicStringView<c8>;

    }

#pragma endregion

#pragma region interface of GameMaker function resources

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
    public:
        enum class Id : u16 {
#include "detail/FunctionId.inc"
        };

        static constexpr auto ARG_VARIABLE{ static_cast<usize>(-1) };

    private:
        struct Data {
            u8 name_size;
            c8 name[67];
            void* address;
            usize arg_count;
            bool require_pro;
        };

        struct Resource {
            Data* data;
            usize count;
        };

        static constexpr auto RESOURCE_PTR{ reinterpret_cast<Resource*>(0x00686b1c) };

        Data* _data{};

    public:
        Function() noexcept = default;

        Function(Id id) noexcept {
            assert(static_cast<usize>(id) < max_id());
            _data = RESOURCE_PTR->data + static_cast<usize>(id);
        }

        static usize max_id() noexcept {
            return RESOURCE_PTR->count;
        }

        Value operator()(auto&&... args) const noexcept {
            assert(_data != nullptr);

            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr usize ARGS_COUNT{ sizeof...(args) };
            assert(_data->arg_count == ARG_VARIABLE || _data->arg_count == ARGS_COUNT);

            Value args_wrapped[]{ static_cast<Value>(args)... }, ret;
            Value *args_ptr{ args_wrapped }, *ret_ptr{ &ret };
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

        std::u8string_view name() const noexcept {
            assert(_data != nullptr);
            return { _data->name, _data->name_size };
        }

        usize arg_count() const noexcept {
            assert(_data != nullptr);
            return _data->arg_count;
        }

        void* address() const noexcept {
            assert(_data != nullptr);
            return _data->address;
        }
    };

#pragma endregion

}

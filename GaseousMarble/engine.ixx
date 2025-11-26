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

    template <class C>
    static constexpr u16 CODE_PAGE{ sizeof(C) == 1 ? 65001 : sizeof(C) == 2 ? 1200 : 12000 };

    template <class C>
    class EmptyString {
        StringHeader _header{ CODE_PAGE<C>, sizeof(C), 1, 0 };
        C _data[1]{};

    public:
        C* data() noexcept {
            return _data;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    template <class C>
    EmptyString<C> empty_string;

    export template <class C>
    class BasicStringView {
        static constexpr usize HEADER_SIZE{ sizeof(StringHeader) / sizeof(C) };

        const C* _data{ empty_string<C>.data() };

        auto _header() const noexcept {
            return std::launder(reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE));
        }

    public:
        BasicStringView() noexcept = default;

        BasicStringView(std::nullptr_t) noexcept = delete;

        // `str` must represent a Delphi UnicodeString
        BasicStringView(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept :
            _data{ static_cast<std::basic_string_view<C>>(str).data() } {}

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        usize size() const noexcept {
            return _header()->size;
        }

        usize ref_count() const noexcept {
            return _header()->ref_count;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    export template <class C>
    class BasicString {
        static constexpr usize HEADER_SIZE{ sizeof(StringHeader) / sizeof(C) };

        C* _data{ empty_string<C>.data() };

        auto _header() noexcept {
            return std::launder(reinterpret_cast<StringHeader*>(_data - HEADER_SIZE));
        }

        auto _header() const noexcept {
            return std::launder(reinterpret_cast<const StringHeader*>(_data - HEADER_SIZE));
        }

    public:
        BasicString() noexcept {
            ++_header()->ref_count;
        }

        BasicString(std::nullptr_t) noexcept = delete;

        BasicString(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept {
            auto view{ static_cast<std::basic_string_view<C>>(str) };

            _data = new C[HEADER_SIZE + view.size() + 1];
            new(_data) StringHeader{ CODE_PAGE<C>, sizeof(C), 1, view.size() };
            _data += HEADER_SIZE;
            new(std::uninitialized_copy(view.begin(), view.end(), _data)) C{};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        BasicString(BasicString&& other) noexcept {
            swap(other);
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](_data - HEADER_SIZE);
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            swap(temp);
            return *this;
        }

        BasicString& operator=(BasicString&& other) noexcept {
            swap(other);
            return *this;
        }

        void swap(BasicString& other) noexcept {
            std::swap(_data, other._data);
        }

        friend void swap(BasicString& left, BasicString& right) noexcept {
            left.swap(right);
        }

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        usize size() const noexcept {
            return _header()->size;
        }

        usize ref_count() const noexcept {
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
        Type _type{};
        Real _real{};
        String _string;

    public:
        Value() noexcept = default;

        Value(Real real) noexcept :
            _real{ real } {}

        Value(StringView string) noexcept :
            _type{ Type::string },
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

        static inline const auto _resource_ptr{ reinterpret_cast<Resource*>(0x00686b1c) };

    public:
        enum class Id {
#include "detail/FunctionId.inc"
        };

        static constexpr auto ARG_VARIABLE{ static_cast<usize>(-1) };

    private:
        Data* _data{};

    public:
        Function() noexcept = default;

        Function(Id id) noexcept {
            assert(static_cast<usize>(id) < max_id());
            _data = _resource_ptr->data + static_cast<usize>(id);
        }

        static usize max_id() noexcept {
            return _resource_ptr->count;
        }

        bool empty() const noexcept {
            return _data == nullptr;
        }

        std::u8string_view name() const noexcept {
            assert(!empty());
            return { _data->name, _data->name_size };
        }

        usize arg_count() const noexcept {
            assert(!empty());
            return _data->arg_count;
        }

        void* address() const noexcept {
            assert(!empty());
            return _data->address;
        }

        Value operator()(auto&&... args) const noexcept {
            assert(!empty());

            // this assertion may fail on game exit since GameMaker has already released function resources
            static constexpr usize ARGS_COUNT{ sizeof...(args) };
            assert(arg_count() == ARG_VARIABLE || arg_count() == ARGS_COUNT);

            std::array<Value, ARGS_COUNT> args_arr{ static_cast<Value>(args)... };
            Value res;
            Value* args_ptr{ args_arr.data() };
            Value* res_ptr{ &res };
            void* func_ptr{ address() };

            // @formatter:off
            __asm {
                push args_ptr;
                push ARGS_COUNT;
                push res_ptr;
                call func_ptr;
            }
            // @formatter:on

            return res;
        }
    };

#pragma endregion

}

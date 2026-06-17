module;

#include <cassert>

export module gm.engine;

import std;
import gm.types;

namespace gm {

    // See https://docwiki.embarcadero.com/RADStudio/Athens/en/Unicode_in_RAD_Studio#New_String_Type:_UnicodeString
    export template <class C>
    class BasicString {
        struct Header {
            u16 code_page{ sizeof(C) == 1 ? 65001 : sizeof(C) == 2 ? 1200 : 12000 };
            u16 char_size{ sizeof(C) };
            u32 ref_count{ 1 };
            u32 size{};
        };

        struct Empty {
            C* data;

            Empty() noexcept {
                // WORKAROUND: Prevent _empty from being placed in .rdata.
                auto storage{ new u8[sizeof(Header) + sizeof(C)] };
                *reinterpret_cast<Header*>(storage) = {};
                data = reinterpret_cast<C*>(storage + sizeof(Header));
                *data = {};
            }
        };

        static inline Empty _empty;

        C* _data{ _empty.data };

        auto _header() noexcept {
            return std::launder(reinterpret_cast<Header*>(reinterpret_cast<u8*>(_data) - sizeof(Header)));
        }

        auto _header() const noexcept {
            return std::launder(reinterpret_cast<const Header*>(reinterpret_cast<const u8*>(_data) - sizeof(Header)));
        }

    public:
        BasicString() noexcept {
            ++_header()->ref_count;
        }

        template <std::convertible_to<std::basic_string_view<C>> V>
        explicit BasicString(const V& str) noexcept {
            auto view{ static_cast<std::basic_string_view<C>>(str) };
            auto storage{ new u8[sizeof(Header) + (view.size() + 1) * sizeof(C)] };
            *reinterpret_cast<Header*>(storage) = { .size = view.size() };
            _data = reinterpret_cast<C*>(storage + sizeof(Header));
            *std::ranges::copy(view, _data).out = {};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        BasicString(BasicString&& other) noexcept :
            BasicString{} {

            this->swap(other);
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](reinterpret_cast<u8*>(_data) - sizeof(Header));
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            this->swap(temp);
            return *this;
        }

        BasicString& operator=(BasicString&& other) noexcept {
            this->swap(other);
            return *this;
        }

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        void swap(BasicString& other) noexcept {
            std::ranges::swap(_data, other._data);
        }

        friend void swap(BasicString& left, BasicString& right) noexcept {
            left.swap(right);
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

    export using Real = f64;
    export using String = BasicString<char>;
    export using StringRef = const char*;

    enum class ValueType {
        real,
        string
    };

    class Value {
        ValueType _type{};
        Real _real{};
        String _string;

    public:
        Value() noexcept = default;

        Value(Real real) noexcept :
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

    struct FunctionData {
        u8 name_size;
        char name[67];
        void* address;
        u32 arg_count;
        bool require_pro;
    };

    struct FunctionResource {
        FunctionData* data;
        u32 size;
    };

    static const auto function_resource{ reinterpret_cast<FunctionResource*>(0x00686b1c) };

    export enum class FunctionId {
#include "inc/FunctionId.inc"
    };

    export class Function {
    public:
        static constexpr auto ARGS_VARIABLE{ static_cast<usize>(-1) };

    private:
        FunctionData* _data{};

    public:
        Function() noexcept = default;

        Function(FunctionId id) noexcept {
            assert(static_cast<usize>(id) < function_resource->size);
            _data = function_resource->data + static_cast<usize>(id);
        }

        operator bool() const noexcept {
            return _data != nullptr;
        }

        std::string_view name() const noexcept {
            assert(*this);
            return { _data->name, _data->name_size };
        }

        usize arg_count() const noexcept {
            assert(*this);
            return _data->arg_count;
        }

        void* address() const noexcept {
            assert(*this);
            return _data->address;
        }

        template <class... Args>
        Value operator()(Args&&... args) const noexcept {
            assert(*this);

            // Cannot use assert() because GameMaker releases the function resource before unloading the DLL
            static constexpr usize ARGS_COUNT{ sizeof...(args) };
            if (arg_count() != ARGS_VARIABLE && arg_count() != ARGS_COUNT) {
                return {};
            }

            std::array<Value, ARGS_COUNT> args_arr{ static_cast<Value>(
                static_cast<std::conditional_t<std::convertible_to<Args, Real>, Real, String>>(args)
            )... };
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

}

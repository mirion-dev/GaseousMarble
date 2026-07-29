module;

#include <cassert>

export module gm.env;

import std;
import gm.types;

namespace gm::env {

    struct FunctionData {
        u8 name_size;
        char name[67];
        void* address;
        u32 arg_count;
        bool is_pro;
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

            std::array<Value, ARGS_COUNT> value_args{ static_cast<Value>(
                static_cast<std::conditional_t<std::convertible_to<Args, Real>, Real, String>>(args)
            )... };
            Value* args_ptr{ value_args.data() };
            Value result;
            Value* result_ptr{ &result };
            void* func_ptr{ address() };

            __asm {
                push args_ptr;
                push ARGS_COUNT;
                push result_ptr;
                call func_ptr;
            }

            return result;
        }
    };

}

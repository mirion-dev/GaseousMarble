module;

#include "log.h"

#include <cassert>
#include <d3d8.h>
#include <spdlog/spdlog.h>
#include <wil/resource.h>

export module gm.hook;

import std;
import gm.log;
import gm.types;
import gm.utils;
import gm.env;

namespace gm::hook {

    export using D3dResetFn = std::function<void(D3DPRESENT_PARAMETERS*)>;

    class D3dReset {
        using Fn = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice8*, D3DPRESENT_PARAMETERS*);

        static constexpr usize INDEX{ 14 };

        static inline D3dReset* _singleton{};

        Fn* _ptr;
        Fn _original;
        D3dResetFn _func;

        static STDMETHODIMP _reset(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* parameters) noexcept {
            if (_singleton->_func) {
                _singleton->_func(parameters);
            }
            return _singleton->_original(device, parameters);
        }

        void _bind(Fn func) const noexcept {
            DWORD old_flags;
            GM_WARN_IF_FAILED_WIN32_BOOL(VirtualProtect(_ptr, sizeof(Fn), PAGE_READWRITE, &old_flags));
            auto _{ wil::scope_exit([&] noexcept {
                DWORD flags;
                GM_WARN_IF_FAILED_WIN32_BOOL(VirtualProtect(_ptr, sizeof(Fn), old_flags, &flags));
            }) };

            *_ptr = func;
        }

    public:
        D3dReset() noexcept
            : _ptr{ *reinterpret_cast<Fn**>(env::d3d().device) + INDEX }, _original{ *_ptr } {

            assert(_singleton == nullptr);
            _singleton = this;
            _bind(_reset);
        }

        D3dReset(D3dReset&&) noexcept = delete;

        ~D3dReset() noexcept {
            _bind(_original);
            _singleton = nullptr;
        }

        D3dReset& operator=(D3dReset&&) noexcept = delete;

        D3dResetFn bind(const D3dResetFn& func) noexcept {
            return std::exchange(_func, func);
        }
    };

    export D3dResetFn d3d_reset(const D3dResetFn& func) noexcept {
        static D3dReset hook;
        return hook.bind(func);
    }

}

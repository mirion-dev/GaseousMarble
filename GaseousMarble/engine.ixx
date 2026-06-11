module;

#include <d3d8.h>
#undef interface

export module gm.engine;

import std;
import gm.types;

namespace gm {

    export class Direct3d {
        struct Resource {
            IDirect3D8* interface;
            IDirect3DDevice8* device;
            u64 _;
            u32 render_width;
            u32 render_height;
        };

        static inline const auto _resource_ptr{ reinterpret_cast<Resource*>(0x006886a4) };

    public:
        static IDirect3D8* interface() noexcept {
            return _resource_ptr->interface;
        }

        static IDirect3DDevice8* device() noexcept {
            return _resource_ptr->device;
        }

        static usize render_width() noexcept {
            return _resource_ptr->render_width;
        }

        static usize render_height() noexcept {
            return _resource_ptr->render_height;
        }
    };

}

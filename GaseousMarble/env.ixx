module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <wil/com.h>
#undef interface

export module gm.env;

import std;
import gm.types;

namespace gm::env {

    struct D3dResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };

    static const auto d3d_resource{ reinterpret_cast<D3dResource*>(0x006886a4) };

    export IDirect3D8* d3d_interface() noexcept {
        return d3d_resource->interface;
    }

    export IDirect3DDevice8* d3d_device() noexcept {
        return d3d_resource->device;
    }

    struct DwResource {
        wil::com_ptr<IDWriteFactory7> factory;
    };

    const DwResource& dw_resource() {
        static DwResource resource;

        if (!resource.factory) {
            THROW_IF_FAILED(
                DWriteCreateFactory(
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(IDWriteFactory7),
                    resource.factory.put_unknown()
                )
            );
        }

        return resource;
    }

    export IDWriteFactory5* dw_factory() {
        return dw_resource().factory.get();
    }

}

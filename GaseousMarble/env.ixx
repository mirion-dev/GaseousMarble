module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <wil/com.h>
#undef interface

export module gm.env;

import std;
import gm.types;

namespace gm::env {

    namespace {
    struct D3dResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };
    }

    static const auto d3d_resource{ reinterpret_cast<D3dResource*>(0x006886a4) };

    export IDirect3D8* d3d_interface() noexcept {
        return d3d_resource->interface;
    }

    export IDirect3DDevice8* d3d_device() noexcept {
        return d3d_resource->device;
    }

    export {
        using DwFactory = IDWriteFactory7;
        using DwFontFace = IDWriteFontFace5;
        using DwGlyphRunAnalysis = IDWriteGlyphRunAnalysis;
        // UNSUPPORTED: `IDWriteTextRenderer1::orientationAngle`; used for vertical writing mode
        using DwTextRenderer = IDWriteTextRenderer;
        using DwTextFormat = IDWriteTextFormat3;
        using DwTextLayout = IDWriteTextLayout4;
    }

    namespace {
    struct DwResource {
        wil::com_ptr<DwFactory> factory;

        DwResource() {
            THROW_IF_FAILED(
                DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DwFactory), factory.put_unknown())
            );
        }
    };
    }

    static const DwResource& dw_resource() {
        static DwResource resource;
        return resource;
    }

    export DwFactory* dw_factory() {
        return dw_resource().factory.get();
    }

}

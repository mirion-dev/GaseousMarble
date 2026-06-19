module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <wil/com.h>
#undef interface

export module gm.env;

import std;
import gm.types;

namespace gm::env {

    export std::wstring default_locale() {
        static std::wstring locale;
        if (locale.empty()) {
            std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> user_locale;
            auto size{ static_cast<usize>(GetUserDefaultLocaleName(user_locale.data(), user_locale.size())) };
            THROW_LAST_ERROR_IF(size == 0);
            locale = { user_locale.data(), size - 1 };
        }
        return locale;
    }

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

    export {

        using DwFactoryBase = IDWriteFactory;
        using DwFactory = IDWriteFactory7;

        using DwFontFaceBase = IDWriteFontFace;
        using DwFontFace = IDWriteFontFace5;

        using DwGlyphRunAnalysisBase = IDWriteGlyphRunAnalysis;
        using DwGlyphRunAnalysis = IDWriteGlyphRunAnalysis;

        using DwTextRendererBase = IDWriteTextRenderer;
        // UNSUPPORTED: `IDWriteTextRenderer1::orientationAngle`; used for vertical writing mode
        using DwTextRenderer = IDWriteTextRenderer;

        using DwTextFormatBase = IDWriteTextFormat;
        using DwTextFormat = IDWriteTextFormat3;

        using DwTextLayoutBase = IDWriteTextLayout;
        using DwTextLayout = IDWriteTextLayout4;

    }

    struct DwResource {
        wil::com_ptr<DwFactory> factory;

        DwResource() {
            THROW_IF_FAILED(
                DWriteCreateFactory(
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(DwFactory),
                    factory.put_unknown()
                )
            );
        }
    };

    const DwResource& dw_resource() {
        static DwResource resource;
        return resource;
    }

    export DwFactory* dw_factory() {
        return dw_resource().factory.get();
    }

}

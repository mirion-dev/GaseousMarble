module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <glaze/toml.hpp>
#include <wil/com.h>
#undef interface

export module gm.env;

import std;
import gm.types;

namespace gm::env {

    export struct Config {
        usize atlas_max_texture_num{ 16 };
        usize max_font_num{ 64 };
        usize layout_cache_size{ 1024 };

        bool is_valid() const noexcept {
            return atlas_max_texture_num > 0 && max_font_num > 0 && layout_cache_size > 0;
        }
    };

    static Config _config{ [] noexcept {
        std::string raw;
        try {
            std::ifstream file{ "gm.toml" };
            raw = { std::istreambuf_iterator{ file }, {} };
        } catch (const std::exception&) {
            return Config{};
        }

        Config result;
        if (glz::read<glz::toml::toml_opts{ .error_on_unknown_keys = false }>(result, raw) || !result.is_valid()) {
            return Config{};
        }

        return result;
    }() };

    export const Config& config() noexcept {
        return _config;
    }

    struct D3dResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };

    static const D3dResource& d3d_resource() noexcept {
        return *reinterpret_cast<D3dResource*>(0x006886a4);
    }

    export IDirect3D8* d3d_interface() noexcept {
        return d3d_resource().interface;
    }

    export IDirect3DDevice8* d3d_device() noexcept {
        return d3d_resource().device;
    }

    export {
        using DwFactory = IDWriteFactory7;
        using DwFontFace = IDWriteFontFace5;
        using DwFontSet = IDWriteFontSet4;
        using DwFontSetBuilder = IDWriteFontSetBuilder2;
        using DwFontCollection = IDWriteFontCollection3;
        using DwLocalizedStrings = IDWriteLocalizedStrings;
        using DwGlyphRunAnalysis = IDWriteGlyphRunAnalysis;
        // UNSUPPORTED: `IDWriteTextRenderer1::orientationAngle`; used for vertical writing mode
        using DwTextRenderer = IDWriteTextRenderer;
        using DwTextFormat = IDWriteTextFormat3;
        using DwTextLayout = IDWriteTextLayout4;
    }

    struct DwResource {
        wil::com_ptr<DwFactory> factory;
    };

    static const DwResource& dw_resource() {
        static DwResource value{ [] {
            DwResource result;
            THROW_IF_FAILED(
                DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DwFactory), result.factory.put_unknown())
            );
            return result;
        }() };
        return value;
    }

    export DwFactory* dw_factory() {
        return dw_resource().factory.get();
    }

}

module;

#include "log.h"

#include <d3d8.h>
#include <dwrite_3.h>
#include <glaze/toml.hpp>
#include <spdlog/spdlog.h>
#include <wil/com.h>

#undef interface

export module gm.env;

import std;
import gm.log;
import gm.types;

namespace gm {

    export struct AtlasOption {
        usize texture_width{ 1024 };
        usize texture_height{ 1024 };
        usize max_texture_num{ 16 };

        bool is_valid() const noexcept {
            return texture_width > 0 && texture_height > 0 && max_texture_num > 0;
        }
    };

    export struct RasterOption {
        f32 min_antialiasing_h_size{};
        f32 min_antialiasing_v_size{ 24 };

        bool is_valid() const noexcept {
            return min_antialiasing_h_size >= 0 && min_antialiasing_v_size >= 0;
        }
    };

}

namespace gm::env {

    static constexpr auto CONFIG_PATH{ "gm2.toml" };

    export struct Config {
        AtlasOption atlas;
        RasterOption raster;
        usize max_font_num{ 64 };
        usize layout_cache_size{ 1024 };

        bool is_valid() const noexcept {
            return atlas.is_valid() && raster.is_valid() && max_font_num > 0 && layout_cache_size > 0;
        }
    };

    export const Config& config() noexcept {
        static auto value{ [] noexcept -> Config {
            std::ifstream file{ CONFIG_PATH };
            if (!file.is_open()) {
                return {};
            }

            std::string raw;
            try {
                raw = { std::istreambuf_iterator{ file }, {} };
            } catch (const std::exception&) {
                GM_WARN("Failed to read the config, falling back to the default config.");
                return {};
            }

            Config result;
            if (glz::read<glz::toml::toml_opts{ .error_on_unknown_keys = false }>(result, raw)) {
                GM_WARN("Failed to parse the config, falling back to the default config.");
                return {};
            }
            if (!result.is_valid()) {
                GM_WARN("Invalid config, falling back to the default config.");
                return {};
            }

            return result;
        }() };
        return value;
    }

    export const std::wstring& default_locale() noexcept {
        static auto value{ [] noexcept -> std::wstring {
            std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> raw;
            auto size{ static_cast<usize>(GetUserDefaultLocaleName(raw.data(), raw.size())) };
            if (size == 0) {
                GM_WARN("Failed to get the user's default locale, falling back to \"en-US\".");
                return L"en-US";
            }

            return { raw.data(), size - 1 };
        }() };
        return value;
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
        static auto value{ [] {
            DwResource result;
            GM_THROW_IF_FAILED(
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

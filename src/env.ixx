module;

#include "log.h"

#include <d3d8.h>
#include <dwrite_3.h>
#include <eigen3/Eigen/Core>
#include <glaze/toml.hpp>
#include <spdlog/spdlog.h>
#include <wil/com.h>

#undef interface

export module gm.env;

import std;
import gm.log;
import gm.types;
import gm.utils;

namespace gm {

    export struct AtlasOption {
        Eigen::Vector2<u32> texture_size{ 1024, 1024 };
        u32 max_texture_num{ 16 };

        bool is_valid() const noexcept {
            return (texture_size.array() > 0).all() && max_texture_num > 0;
        }
    };

    export struct RasterOption {
        Eigen::Vector2<f32> min_antialiasing_size{ 0, 0 };

        bool is_valid() const noexcept {
            return (min_antialiasing_size.array() >= 0).all();
        }
    };

}

template <>
struct glz::meta<gm::AtlasOption> {
    using T = gm::AtlasOption;

    static constexpr auto value{ object(
        "texture_width",
        [](T& self) noexcept -> auto& { return self.texture_size.x(); },
        "texture_height",
        [](T& self) noexcept -> auto& { return self.texture_size.y(); },
        "max_texture_num",
        &T::max_texture_num
    ) };
};

template <>
struct glz::meta<gm::RasterOption> {
    using T = gm::RasterOption;

    static constexpr auto value{ object(
        "min_antialiasing_size_h",
        [](T& self) noexcept -> auto& { return self.min_antialiasing_size.x(); },
        "min_antialiasing_size_v",
        [](T& self) noexcept -> auto& { return self.min_antialiasing_size.y(); }
    ) };
};

namespace gm::env {

    constexpr auto CONFIG_PATH{ "gm2.toml" };

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
            std::error_code error;
            if (!std::filesystem::exists(CONFIG_PATH, error)) {
                GM_INFO("Config not found, using the default config.");
                return {};
            }

            std::string raw;
            try {
                std::ifstream file{ CONFIG_PATH };
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

            std::wstring result{ raw.data(), size - 1 };
            GM_INFO("Resolved the user's default locale to \"{}\".", to_string(result));
            return result;
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

    const D3dResource& d3d_resource() noexcept {
        return *reinterpret_cast<D3dResource*>(0x006886a4);
    }

    export IDirect3D8* d3d_interface() noexcept {
        return d3d_resource().interface;
    }

    export IDirect3DDevice8* d3d_device() noexcept {
        return d3d_resource().device;
    }

    export {
        // Latest `IDWriteFactory7` requires Windows 10, Version 1803
        using DwFactory = IDWriteFactory6;
        using DwFontFace = IDWriteFontFace5;
        // Latest `IDWriteFontSet4` requires Windows 11, Version 22H2
        using DwFontSet = IDWriteFontSet1;
        using DwFontSetBuilder = IDWriteFontSetBuilder2;
        // Latest `IDWriteFontCollection3` requires Windows 10, Version 1803
        using DwFontCollection = IDWriteFontCollection2;
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

    const DwResource& dw_resource() {
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

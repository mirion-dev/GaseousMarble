module;

#include "log.h"

#include <d3d8.h>
#include <dwrite_3.h>
#include <eigen3/Eigen/Core>
#include <rectpack2D/finders_interface.h>
#include <spdlog/spdlog.h>
#include <wil/com.h>

export module gm.glyph;

import std;
import gm.log;
import gm.types;
import gm.utils;
import gm.env;

namespace gm {

    class TextureLock {
        wil::com_ptr<IDirect3DTexture8> _texture;
        std::mdspan<u32, std::dextents<usize, 2>, std::layout_stride> _data;

    public:
        TextureLock() noexcept = default;

        TextureLock(const wil::com_ptr<IDirect3DTexture8>& texture) {
            assert(texture);

            D3DSURFACE_DESC desc;
            GM_THROW_IF_FAILED(texture->GetLevelDesc(0, &desc));

            D3DLOCKED_RECT lock;
            GM_THROW_IF_FAILED(texture->LockRect(0, &lock, nullptr, D3DLOCK_NO_DIRTY_UPDATE));

            _texture = texture;
            _data = { static_cast<u32*>(lock.pBits),
                      { std::extents{ desc.Height, desc.Width }, std::array{ lock.Pitch / sizeof(u32), 1uz } } };
        }

        TextureLock(TextureLock&& other) noexcept {
            std::ranges::swap(*this, other);
        }

        ~TextureLock() noexcept {
            if (_texture) {
                GM_WARN_IF_FAILED(_texture->UnlockRect(0));
            }
        }

        TextureLock& operator=(TextureLock&& other) noexcept {
            std::ranges::swap(*this, other);
            return *this;
        }

        operator bool() const noexcept {
            return _texture != nullptr;
        }

        friend void swap(TextureLock& left, TextureLock& right) noexcept {
            std::ranges::swap(left._texture, right._texture);
            std::ranges::swap(left._data, right._data);
        }

        void update(Eigen::Vector2<usize> pos, const std::mdspan<u8, std::dextents<usize, 2>>& data) const {
            assert(*this);

            Eigen::Vector2<usize> size{ data.extents().extent(1), data.extents().extent(0) };
            Eigen::Vector2<usize> end{ pos + size };
            RECT rect{ static_cast<isize>(pos.x()),
                       static_cast<isize>(pos.y()),
                       static_cast<isize>(end.x()),
                       static_cast<isize>(end.y()) };
            GM_THROW_IF_FAILED(_texture->AddDirtyRect(&rect));

            for (usize j{}; j < size.y(); ++j) {
                for (usize i{}; i < size.x(); ++i) {
                    _data[pos.y() + j, pos.x() + i] = D3DCOLOR_RGBA(0xff, 0xff, 0xff, (data[j, i]));
                }
            }
        }
    };

    export struct GlyphDesc {
        wil::com_ptr<env::DwFontFace> face;
        u16 gid;

        friend bool operator==(GlyphDesc left, GlyphDesc right) noexcept = default;
    };

    export struct GlyphMeta {
        wil::com_ptr<IDirect3DTexture8> texture;
        Eigen::Vector2<usize> pos{ 0, 0 };
        Eigen::Vector2<usize> size{ 0, 0 };
        Eigen::Vector2<isize> offset{ 0, 0 };
    };

    export struct GlyphSpec : GlyphDesc {
        f32 size;
    };

    export class GlyphAtlas {
        struct Hash {
            usize operator()(GlyphDesc value) const noexcept {
                return hash_combine(gm::Hash{}, value.face, value.gid);
            }
        };

        AtlasOption _option{ .max_texture_num = 0 };
        RasterOption _raster_option;

        std::unordered_map<GlyphDesc, GlyphMeta, Hash> _data;
        usize _texture_num{};
        wil::com_ptr<IDirect3DTexture8> _current_texture;
        rectpack2D::empty_spaces<false> _current_bin{ {} };

    public:
        GlyphAtlas() noexcept = default;

        GlyphAtlas(AtlasOption option, RasterOption raster_option) noexcept
            : _option{ std::move(option) }, _raster_option{ std::move(raster_option) } {

            assert(_option.is_valid() && _raster_option.is_valid());
        }

        GlyphAtlas(GlyphAtlas&&) noexcept = default;

        GlyphAtlas& operator=(GlyphAtlas&&) noexcept = default;

        operator bool() const noexcept {
            return _option.max_texture_num > 0;
        }

        const AtlasOption& option() const noexcept {
            assert(*this);
            return _option;
        }

        const RasterOption& raster_option() const noexcept {
            assert(*this);
            return _raster_option;
        }

        template <std::ranges::input_range R, class DescFn, class SpecFn>
        std::vector<const GlyphMeta*> get(R&& items, DescFn&& desc_func, SpecFn&& spec_func) {
            assert(*this);

            TextureLock lock;
            std::vector<const GlyphMeta*> result;
            for (auto& item : std::forward<R>(items)) {
                auto iter{ _data.find(std::forward<DescFn>(desc_func)(item)) };
                if (iter != _data.end()) {
                    result.push_back(&iter->second);
                    continue;
                }

                GlyphSpec spec{ std::forward<SpecFn>(spec_func)(item) };
                DWRITE_GLYPH_RUN run{ spec.face.get(), spec.size, 1, &spec.gid };
                DWRITE_RENDERING_MODE1 antialiasing{ spec.size < _raster_option.min_antialiasing_size.x()
                                                         ? DWRITE_RENDERING_MODE1_ALIASED
                                                     : spec.size < _raster_option.min_antialiasing_size.y()
                                                         ? DWRITE_RENDERING_MODE1_NATURAL
                                                         : DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC };
                wil::com_ptr<env::DwGlyphRunAnalysis> rasterizer;
                GM_THROW_IF_FAILED(
                    env::dw_factory()->CreateGlyphRunAnalysis(
                        &run,
                        nullptr,
                        antialiasing,
                        DWRITE_MEASURING_MODE_NATURAL,
                        DWRITE_GRID_FIT_MODE_DISABLED,
                        DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE,
                        0,
                        0,
                        &rasterizer
                    )
                );

                RECT aabb;
                GM_THROW_IF_FAILED(rasterizer->GetAlphaTextureBounds(DWRITE_TEXTURE_ALIASED_1x1, &aabb));

                Eigen::Vector2<isize> start{ aabb.left, aabb.top };
                Eigen::Vector2<isize> end{ aabb.right, aabb.bottom };
                Eigen::Vector2<usize> size{ (end - start).cast<usize>() };
                if ((size.array() == 0).any()) {
                    result.push_back(&_data.try_emplace(static_cast<GlyphDesc>(spec)).first->second);
                    continue;
                }

                if ((size.array() > _option.texture_size.array()).any()) {
                    throw std::runtime_error{ "Glyph too large." };
                }

                std::vector<u8> alpha(size.prod());
                GM_THROW_IF_FAILED(
                    rasterizer->CreateAlphaTexture(DWRITE_TEXTURE_ALIASED_1x1, &aabb, alpha.data(), alpha.size())
                );

                usize texture_num{ _texture_num };
                wil::com_ptr texture{ _current_texture };
                rectpack2D::empty_spaces bin{ _current_bin };
                auto insert_result{ bin.insert({ static_cast<isize>(size.x()), static_cast<isize>(size.y()) }) };
                if (!insert_result) {
                    if (texture_num++ >= _option.max_texture_num) {
                        throw std::runtime_error{ "Too many textures." };
                    }

                    GM_THROW_IF_FAILED(
                        env::d3d_device()->CreateTexture(
                            _option.texture_size.x(),
                            _option.texture_size.y(),
                            1,
                            0,
                            D3DFMT_A8R8G8B8,
                            D3DPOOL_MANAGED,
                            &texture
                        )
                    );

                    lock = texture;
                    bin.reset(
                        { static_cast<isize>(_option.texture_size.x()), static_cast<isize>(_option.texture_size.y()) }
                    );
                    insert_result = bin.insert({ static_cast<isize>(size.x()), static_cast<isize>(size.y()) });
                }

                Eigen::Vector2<usize> pos{ static_cast<usize>(insert_result->x), static_cast<usize>(insert_result->y) };
                if (!lock) {
                    lock = _current_texture;
                }
                lock.update(pos, std::mdspan{ alpha.data(), size.y(), size.x() });

                _texture_num = texture_num;
                _current_texture = texture;
                _current_bin = std::move(bin);

                result.push_back(
                    &_data.try_emplace(static_cast<GlyphDesc>(spec), _current_texture, pos, size, start).first->second
                );
            }

            return result;
        }
    };

}

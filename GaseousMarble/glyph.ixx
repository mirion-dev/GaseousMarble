module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <rectpack2D/finders_interface.h>
#include <wil/com.h>

export module gm.glyph;

import std;
import gm.types;
import gm.utils;
import gm.env;

namespace gm {

    class TextureLock {
        wil::com_ptr<IDirect3DTexture8> _texture;
        std::mdspan<u32, std::dextents<usize, 2>, std::layout_stride> _data;

    public:
        TextureLock() noexcept = default;

        TextureLock(wil::com_ptr<IDirect3DTexture8> texture, usize x, usize y, usize width, usize height) {
            assert(texture && width > 0 && height > 0);

            D3DLOCKED_RECT lock;
            RECT rect{ static_cast<isize>(x),
                       static_cast<isize>(y),
                       static_cast<isize>(x + width),
                       static_cast<isize>(y + height) };
            THROW_IF_FAILED(texture->LockRect(0, &lock, &rect, D3DLOCK_NO_DIRTY_UPDATE));

            _texture = texture;
            _data = { static_cast<u32*>(lock.pBits),
                      { std::extents{ height, width }, std::array{ lock.Pitch / sizeof(u32), 1uz } } };
        }

        TextureLock(TextureLock&& other) noexcept {
            std::ranges::swap(*this, other);
        }

        ~TextureLock() noexcept {
            if (_texture) {
                _texture->UnlockRect(0);
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

        void update(usize x, usize y, const std::mdspan<u8, std::dextents<usize, 2>>& data) const {
            assert(*this);

            usize height{ data.extents().extent(0) }, width{ data.extents().extent(1) };
            RECT rect{ static_cast<isize>(x),
                       static_cast<isize>(y),
                       static_cast<isize>(x + width),
                       static_cast<isize>(y + height) };
            THROW_IF_FAILED(_texture->AddDirtyRect(&rect));

            for (usize j{}; j < height; ++j) {
                for (usize i{}; i < width; ++i) {
                    _data[y + j, x + i] = D3DCOLOR_RGBA(0xff, 0xff, 0xff, (data[j, i]));
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
        usize x;
        usize y;
        usize width;
        usize height;
        isize offset_x;
        isize offset_y;
    };

    export struct GlyphSpec : GlyphDesc {
        f32 size;
    };

    export struct RasterOption {
        f32 min_aa_h_size{};
        f32 min_aa_v_size{ 24 };

        bool is_valid() const noexcept {
            return min_aa_h_size >= 0 && min_aa_v_size >= 0;
        }
    };

    export class GlyphAtlas {
        struct Hash {
            usize operator()(GlyphDesc value) const noexcept {
                return hash_combine(gm::Hash{}, value.face, value.gid);
            }
        };

        usize _texture_width{};
        usize _texture_height{};
        usize _max_texture_num{};
        RasterOption _option;

        std::unordered_map<GlyphDesc, GlyphMeta, Hash> _data;

        usize _texture_num{};
        wil::com_ptr<IDirect3DTexture8> _current_texture;
        rectpack2D::empty_spaces<false> _current_bin{ {} };

    public:
        GlyphAtlas() noexcept = default;

        GlyphAtlas(usize texture_width, usize texture_height, usize max_texture_num, RasterOption option = {})
            : _texture_width{ texture_width },
              _texture_height{ texture_height },
              _max_texture_num{ max_texture_num },
              _option{ option } {

            assert(_texture_width > 0 && _texture_height > 0 && _max_texture_num > 0);

            if (!option.is_valid()) {
                throw std::invalid_argument{ "Invalid rasterization options." };
            }
        }

        GlyphAtlas(GlyphAtlas&&) noexcept = default;

        GlyphAtlas& operator=(GlyphAtlas&&) noexcept = default;

        operator bool() const noexcept {
            return _max_texture_num > 0;
        }

        usize texture_width() const noexcept {
            assert(*this);
            return _texture_width;
        }

        usize texture_height() const noexcept {
            assert(*this);
            return _texture_height;
        }

        usize max_texture_num() const noexcept {
            assert(*this);
            return _max_texture_num;
        }

        auto& option(this auto&& self) noexcept {
            assert(self);
            return std::forward_like<decltype(self)>(self._option);
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
                DWRITE_RENDERING_MODE1 aa{ spec.size < _option.min_aa_h_size ? DWRITE_RENDERING_MODE1_ALIASED
                                           : spec.size < _option.min_aa_v_size
                                               ? DWRITE_RENDERING_MODE1_NATURAL
                                               : DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC };
                wil::com_ptr<env::DwGlyphRunAnalysis> rasterizer;
                THROW_IF_FAILED(
                    env::dw_factory()->CreateGlyphRunAnalysis(
                        &run,
                        nullptr,
                        aa,
                        DWRITE_MEASURING_MODE_NATURAL,
                        DWRITE_GRID_FIT_MODE_DISABLED,
                        DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE,
                        0,
                        0,
                        &rasterizer
                    )
                );

                RECT bbox;
                THROW_IF_FAILED(rasterizer->GetAlphaTextureBounds(DWRITE_TEXTURE_ALIASED_1x1, &bbox));

                auto width{ static_cast<usize>(bbox.right - bbox.left) };
                auto height{ static_cast<usize>(bbox.bottom - bbox.top) };
                if (width == 0 || height == 0) {
                    result.push_back(
                        &_data.try_emplace(static_cast<GlyphDesc>(spec), nullptr, 0, 0, 0, 0, 0, 0).first->second
                    );
                    continue;
                }

                if (width > _texture_width || height > _texture_height) {
                    throw std::runtime_error{ "Glyph too large." };
                }

                std::vector<u8> alpha(width * height);
                THROW_IF_FAILED(
                    rasterizer->CreateAlphaTexture(DWRITE_TEXTURE_ALIASED_1x1, &bbox, alpha.data(), alpha.size())
                );

                usize texture_num{ _texture_num };
                wil::com_ptr texture{ _current_texture };
                rectpack2D::empty_spaces bin{ _current_bin };
                auto insert_result{ bin.insert({ static_cast<isize>(width), static_cast<isize>(height) }) };
                if (!insert_result) {
                    if (texture_num++ >= _max_texture_num) {
                        throw std::runtime_error{ "Too many textures." };
                    }

                    THROW_IF_FAILED(
                        env::d3d_device()->CreateTexture(
                            _texture_width, _texture_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture
                        )
                    );

                    lock = { texture, 0, 0, _texture_width, _texture_height };
                    bin.reset({ static_cast<isize>(_texture_width), static_cast<isize>(_texture_height) });
                    insert_result = bin.insert({ static_cast<isize>(width), static_cast<isize>(height) });
                }

                auto x{ static_cast<usize>(insert_result->x) }, y{ static_cast<usize>(insert_result->y) };
                auto w{ static_cast<usize>(insert_result->w) }, h{ static_cast<usize>(insert_result->h) };
                if (!lock) {
                    lock = { _current_texture, 0, 0, _texture_width, _texture_height };
                }
                lock.update(x, y, std::mdspan{ alpha.data(), h, w });

                _texture_num = texture_num;
                _current_texture = texture;
                _current_bin = std::move(bin);

                result.push_back(
                    &_data.try_emplace(static_cast<GlyphDesc>(spec), _current_texture, x, y, w, h, bbox.left, bbox.top)
                         .first->second
                );
            }

            return result;
        }
    };

}

module;

#include <d3d8.h>
#include <dwrite_3.h>
#include <rectpack2D/finders_interface.h>
#include <wil/com.h>

export module gm.font;

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
            RECT rect{
                static_cast<isize>(x),
                static_cast<isize>(y),
                static_cast<isize>(x + width),
                static_cast<isize>(y + height)
            };
            THROW_IF_FAILED(texture->LockRect(0, &lock, &rect, 0));

            _texture = texture;
            _data = {
                static_cast<u32*>(lock.pBits),
                { std::extents{ height, width }, std::array{ lock.Pitch / sizeof(u32), 1uz } }
            };
        }

        TextureLock(TextureLock&& other) noexcept {
            swap(other);
        }

        ~TextureLock() noexcept {
            if (_texture) {
                _texture->UnlockRect(0);
            }
        }

        TextureLock& operator=(TextureLock&& other) noexcept {
            swap(other);
            return *this;
        }

        operator bool() const noexcept {
            return _texture != nullptr;
        }

        void swap(TextureLock& other) noexcept {
            std::ranges::swap(_texture, other._texture);
            std::ranges::swap(_data, other._data);
        }

        friend void swap(TextureLock& left, TextureLock& right) noexcept {
            left.swap(right);
        }

        const auto& data() const noexcept {
            assert(*this);
            return _data;
        }
    };

    export struct GlyphId {
        wil::com_ptr<env::DwFontFace> face;
        f32 size;
        u16 gid;

        friend bool operator==(const GlyphId& left, const GlyphId& right) noexcept = default;
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

    export class Font {
        struct Hash {
            usize operator()(const GlyphId& value) const noexcept {
                return hash_combine(gm::Hash{}, value.face, value.size, value.gid);
            }
        };

        // FontCollection is unimplemented; used for loading from font files
        std::wstring _name;
        f32 _size{};
        DWRITE_FONT_WEIGHT _weight{};
        DWRITE_FONT_STYLE _style{};
        DWRITE_FONT_STRETCH _stretch{};
        std::wstring _locale;
        usize _texture_width{};
        usize _texture_height{};
        usize _max_texture_num{};
        f32 _min_antialiasing_v_size{};

        std::unordered_map<GlyphId, GlyphMeta, Hash> _data;

        usize _texture_num{};
        wil::com_ptr<IDirect3DTexture8> _current_texture;
        rectpack2D::empty_spaces<false> _current_bin{ {} };

        auto _new_texture() {
            wil::com_ptr<IDirect3DTexture8> texture;
            THROW_IF_FAILED(
                env::d3d_device()->CreateTexture(
                    _texture_width,
                    _texture_height,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &texture
                )
            );
            ++_texture_num;
            return texture;
        }

    public:
        Font() noexcept = default;

        Font(
            std::wstring_view name,
            f32 size,
            DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL,
            std::wstring_view locale = L"",
            usize texture_width = 1024,
            usize texture_height = 1024,
            usize max_texture_num = 16,
            f32 min_antialiasing_v_size = 24
        ) :
            _name{ name },
            _size{ size },
            _weight{ weight },
            _style{ style },
            _stretch{ stretch },
            _locale{ locale },
            _texture_width{ texture_width },
            _texture_height{ texture_height },
            _max_texture_num{ max_texture_num },
            _min_antialiasing_v_size{ min_antialiasing_v_size } {

            if (_name.empty() || _size <= 0 || _texture_width <= 0 || _texture_height <= 0 || _max_texture_num == 0) {
                throw std::invalid_argument{ "Invalid font arguments." };
            }

            if (_locale.empty()) {
                std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> default_locale;
                usize default_locale_size{
                    static_cast<usize>(GetUserDefaultLocaleName(default_locale.data(), default_locale.size()))
                };
                THROW_LAST_ERROR_IF(default_locale_size == 0);
                _locale = { default_locale.data(), default_locale_size };
            }
        }

        Font(Font&&) noexcept = default;

        Font& operator=(Font&&) noexcept = default;

        operator bool() const noexcept {
            return !_name.empty();
        }

        std::wstring_view name() const noexcept {
            assert(*this);
            return _name;
        }

        f32 size() const noexcept {
            assert(*this);
            return _size;
        }

        DWRITE_FONT_WEIGHT weight() const noexcept {
            assert(*this);
            return _weight;
        }

        DWRITE_FONT_STYLE style() const noexcept {
            assert(*this);
            return _style;
        }

        DWRITE_FONT_STRETCH stretch() const noexcept {
            assert(*this);
            return _stretch;
        }

        std::wstring_view locale() const noexcept {
            assert(*this);
            return _locale;
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

        f32 min_antialiasing_v_size() const noexcept {
            assert(*this);
            return _min_antialiasing_v_size;
        }

        template <std::ranges::input_range R>
            requires std::same_as<std::ranges::range_value_t<R>, GlyphId>
        std::vector<const GlyphMeta*> get(R&& ids) {
            assert(*this);

            std::vector<const GlyphMeta*> result;
            std::vector<std::pair<usize, GlyphId>> missing;
            for (auto [i, id] : std::forward<R>(ids) | std::views::enumerate) {
                auto iter{ _data.find(id) };
                if (iter != _data.end()) {
                    result.push_back(&iter->second);
                }
                else {
                    result.push_back(nullptr);
                    missing.emplace_back(i, id);
                }
            }

            if (missing.empty()) {
                return result;
            }

            TextureLock lock;
            if (_current_texture) {
                lock = { _current_texture, 0, 0, _texture_width, _texture_height };
            }
            else {
                auto texture{ _new_texture() };
                lock = { texture, 0, 0, _texture_width, _texture_height };

                _current_texture = texture;
                _current_bin.reset({ static_cast<isize>(_texture_width), static_cast<isize>(_texture_height) });
            }

            for (auto& [i, id] : missing) {
                DWRITE_GLYPH_RUN run{ id.face.get(), id.size, 1, &id.gid };
                wil::com_ptr<env::DwGlyphRunAnalysis> rasterizer;
                THROW_IF_FAILED(
                    env::dw_factory()->CreateGlyphRunAnalysis(
                        &run,
                        nullptr,
                        id.size < _min_antialiasing_v_size
                        ? DWRITE_RENDERING_MODE1_NATURAL
                        : DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC,
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
                    continue;
                }
                if (width > _texture_width || height > _texture_height) {
                    throw std::runtime_error{ "Glyph too large." };
                }

                std::vector<u8> alpha(width * height);
                THROW_IF_FAILED(
                    rasterizer->CreateAlphaTexture(
                        DWRITE_TEXTURE_ALIASED_1x1,
                        &bbox,
                        alpha.data(),
                        alpha.size()
                    )
                );

                auto insert_result{ _current_bin.insert({ static_cast<isize>(width), static_cast<isize>(height) }) };
                if (!insert_result) {
                    if (_texture_num >= _max_texture_num) {
                        throw std::runtime_error{ "Too many textures." };
                    }

                    wil::com_ptr texture{ _new_texture() };
                    lock = { texture, 0, 0, _texture_width, _texture_height };

                    _current_texture = texture;
                    _current_bin.reset({ static_cast<isize>(_texture_width), static_cast<isize>(_texture_height) });

                    insert_result = _current_bin.insert({ static_cast<isize>(width), static_cast<isize>(height) });
                }

                auto x{ static_cast<usize>(insert_result->x) }, y{ static_cast<usize>(insert_result->y) };
                auto w{ static_cast<usize>(insert_result->w) }, h{ static_cast<usize>(insert_result->h) };
                std::mdspan src{ alpha.data(), h, w };
                auto& dest{ lock.data() };
                for (usize i{}; i < h; ++i) {
                    for (usize j{}; j < w; ++j) {
                        dest[y + i, x + j] = D3DCOLOR_RGBA(0xff, 0xff, 0xff, (src[i, j]));
                    }
                }

                result[i] = &_data.try_emplace(
                    std::move(id),
                    _current_texture,
                    x,
                    y,
                    w,
                    h,
                    bbox.left,
                    bbox.top
                ).first->second;
            }

            return result;
        }
    };

}

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
        u16 gid;

        friend bool operator==(GlyphId left, GlyphId right) noexcept = default;
    };

    export struct GlyphRasterization : GlyphId {
        f32 size;
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
    public:
        static constexpr u32 WEIGHT_MASK{ 0x3ff };
        static constexpr int WEIGHT_OFFSET{};
        static constexpr u32 STYLE_MASK{ 0xc00 };
        static constexpr int STYLE_OFFSET{ 10 };
        static constexpr u32 STRETCH_MASK{ 0xf000 };
        static constexpr int STRETCH_OFFSET{ 12 };

    private:
        struct Hash {
            usize operator()(GlyphId value) const noexcept {
                return hash_combine(gm::Hash{}, value.face, value.gid);
            }
        };

        std::wstring _name;
        f32 _size{};
        u32 _properties{};
        std::wstring _locale;
        f32 _min_aa_h_size{};
        f32 _min_aa_v_size{};
        usize _max_texture_num{};
        usize _texture_width{};
        usize _texture_height{};

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
            u32 properties = DWRITE_FONT_WEIGHT_NORMAL,
            std::wstring_view locale = L"",
            f32 min_aa_h_size = 0,
            f32 min_aa_v_size = 24,
            usize max_texture_num = 16,
            usize texture_width = 1024,
            usize texture_height = 1024
        ) :
            _name{ name },
            _size{ size },
            _properties{ properties },
            _locale{ locale },
            _min_aa_h_size{ min_aa_h_size },
            _min_aa_v_size{ min_aa_v_size },
            _max_texture_num{ max_texture_num },
            _texture_width{ texture_width },
            _texture_height{ texture_height } {

            if (_name.empty() || _size <= 0 || weight() < 1 || weight() > 1000 || stretch() > 9
                || _texture_width <= 0 || _texture_height <= 0 || _max_texture_num == 0) {
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
            return static_cast<DWRITE_FONT_WEIGHT>((_properties & WEIGHT_MASK) >> WEIGHT_OFFSET);
        }

        DWRITE_FONT_STYLE style() const noexcept {
            assert(*this);
            return static_cast<DWRITE_FONT_STYLE>((_properties & STYLE_MASK) >> STYLE_OFFSET);
        }

        DWRITE_FONT_STRETCH stretch() const noexcept {
            assert(*this);
            return static_cast<DWRITE_FONT_STRETCH>((_properties & STRETCH_MASK) >> STRETCH_OFFSET);
        }

        std::wstring_view locale() const noexcept {
            assert(*this);
            return _locale;
        }

        f32 min_aa_h_size() const noexcept {
            assert(*this);
            return _min_aa_h_size;
        }

        f32 min_aa_v_size() const noexcept {
            assert(*this);
            return _min_aa_v_size;
        }

        usize max_texture_num() const noexcept {
            assert(*this);
            return _max_texture_num;
        }

        usize texture_width() const noexcept {
            assert(*this);
            return _texture_width;
        }

        usize texture_height() const noexcept {
            assert(*this);
            return _texture_height;
        }

        template <std::ranges::input_range R, class KeyFn, class DataFn>
        std::vector<const GlyphMeta*> get(R&& items, KeyFn&& key_func, DataFn&& data_func) {
            assert(*this);

            std::vector<const GlyphMeta*> result;
            std::vector<std::pair<usize, GlyphRasterization>> missing;
            for (auto [i, item] : std::forward<R>(items) | std::views::enumerate) {
                auto iter{ _data.find(std::forward<KeyFn>(key_func)(item)) };
                if (iter != _data.end()) {
                    result.push_back(&iter->second);
                }
                else {
                    result.push_back(nullptr);
                    missing.emplace_back(i, std::forward<DataFn>(data_func)(item));
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

            for (auto& [i, data] : missing) {
                DWRITE_GLYPH_RUN run{ data.face.get(), data.size, 1, &data.gid };
                DWRITE_RENDERING_MODE1 aa{
                    data.size < _min_aa_h_size
                    ? DWRITE_RENDERING_MODE1_ALIASED
                    : data.size < _min_aa_v_size
                    ? DWRITE_RENDERING_MODE1_NATURAL
                    : DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC
                };
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
                    result[i] = &_data.try_emplace(
                        std::move(static_cast<GlyphId>(data)),
                        nullptr,
                        0,
                        0,
                        0,
                        0,
                        0,
                        0
                    ).first->second;
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
                    std::move(static_cast<GlyphId>(data)),
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

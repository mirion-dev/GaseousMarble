module;

#include <d3dx8.h>
#include <dwrite_3.h>
#include <rectpack2D/finders_interface.h>
#include <wil/com.h>
#include <wil/cppwinrt.h>

export module gm.draw;

import std;
import gm.types;
import gm.utils;
import gm.env;
import gm.engine;

namespace gm {

    class TextureLock {
        wil::com_ptr<IDirect3DTexture8> _texture;
        std::mdspan<u8, std::dextents<usize, 2>, std::layout_stride> _data;

    public:
        TextureLock() noexcept = default;

        TextureLock(wil::com_ptr<IDirect3DTexture8> texture, usize x, usize y, usize width, usize height) {
            if (texture) {
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
                    static_cast<u8*>(lock.pBits),
                    { std::extents{ height, width }, std::array{ lock.Pitch, 1 } }
                };
            }
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

        void swap(TextureLock& other) noexcept {
            std::ranges::swap(_texture, other._texture);
            std::ranges::swap(_data, other._data);
        }

        friend void swap(TextureLock& left, TextureLock& right) noexcept {
            left.swap(right);
        }

        const auto& data() const noexcept {
            return _data;
        }
    };

    template <usize N, usize Size = 1024>
        requires (N > 0 && Size > 0)
    class GlyphAtlas {
    public:
        struct Glyph {
            wil::com_ptr<IDirect3DTexture8> texture;
            usize x;
            usize y;
            usize width;
            usize height;
            isize offset_x;
            isize offset_y;
        };

        struct Key {
            wil::com_ptr<IDWriteFontFace7> face;
            u16 gid;

            friend bool operator==(Key left, Key right) noexcept {
                return left.face.get() == right.face.get() && left.gid == right.gid;
            }
        };

    private:
        struct Hash {
            template <class T>
            usize operator()(const T& value) const noexcept {
                if constexpr (std::same_as<T, Key>) {
                    usize a{ std::hash<usize>{}(reinterpret_cast<usize>(value.face.get())) };
                    usize b{ std::hash<u16>{}(value.gid) };
                    return a ^ b + 0x9e3779b9 + (a << 6) + (a >> 2);
                }
                else {
                    return std::hash<T>{}(value);
                }
            }
        };

        std::unordered_map<Key, Glyph, Hash> _data;
        std::deque<std::vector<Key>> _order;
        wil::com_ptr<IDirect3DTexture8> _current_texture;
        rectpack2D::empty_spaces<false> _current_bin{ { Size, Size } };
        std::vector<Key> _current_keys;

        static auto _new_texture() {
            wil::com_ptr<IDirect3DTexture8> texture;
            THROW_IF_FAILED(
                Direct3D::device()->CreateTexture(
                    Size,
                    Size,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &texture
                )
            );
            return texture;
        }

    public:
        TextureLock lock() {
            if (!_current_texture) {
                _current_texture = _new_texture();
            }
            return { _current_texture, 0, 0, Size, Size };
        }

        template <class Fn>
        std::pair<const Glyph&, bool> get(Key key, TextureLock& lock, Fn&& func) {
            auto iter{ _data.find(key) };
            if (iter != _data.end()) {
                return { iter->second, false };
            }

            auto [alpha, width, height, offset_x, offset_y]{ std::forward<Fn>(func)() };
            auto result{ _current_bin.insert({ static_cast<isize>(width), static_cast<isize>(height) }) };
            if (!result) {
                rectpack2D::empty_spaces<false> bin{ { Size, Size } };
                result = bin.insert({ static_cast<isize>(width), static_cast<isize>(height) });
                if (!result) {
                    throw std::runtime_error{ "The glyph is too large." };
                }

                wil::com_ptr texture{ _new_texture() };
                lock = { texture, 0, 0, Size, Size };

                _current_bin = std::move(bin);
                _current_texture = std::move(texture);
                _order.emplace_back(std::move(_current_keys));
                _current_keys.clear();

                if (_order.size() >= N) {
                    for (auto& key : _order.front()) {
                        _data.erase(key);
                    }
                    _order.pop_front();
                }
            }

            auto x{ static_cast<usize>(result->x) }, y{ static_cast<usize>(result->y) };
            auto w{ static_cast<usize>(result->w) }, h{ static_cast<usize>(result->h) };
            std::mdspan<u8, std::dextents<usize, 2>> src{ alpha.data(), h, w };
            for (usize i{}; i < h; ++i) {
                std::memcpy(&lock.data()[y + i, x], &src[i, 0], w);
            }

            iter = _data.emplace(key, Glyph{ _current_texture, x, y, w, h, offset_x, offset_y }).first;
            _current_keys.push_back(key);
            return { iter->second, true };
        }
    };

    struct Layout {
        struct Glyph {
            wil::com_ptr<IDWriteFontFace7> face;
            f32 size;
            u16 gid;
            f32 x;
            f32 y;
        };

        std::vector<Glyph> glyphs;
    };

    class LayoutCollector : public winrt::implements<LayoutCollector, IDWriteTextRenderer/*1*/> {
    public:
        STDMETHODIMP IsPixelSnappingDisabled(void*, BOOL*) noexcept {
            return 0;
        }

        STDMETHODIMP GetCurrentTransform(void*, DWRITE_MATRIX*) noexcept {
            return 0;
        }

        STDMETHODIMP GetPixelsPerDip(void*, FLOAT*) noexcept {
            return 0;
        }

        STDMETHODIMP DrawGlyphRun(
            void* client_drawing_context,
            FLOAT baseline_origin_x,
            FLOAT baseline_origin_y,
            DWRITE_MEASURING_MODE measuring_mode,
            const DWRITE_GLYPH_RUN* glyph_run,
            const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_description,
            IUnknown* client_drawing_effect
        ) noexcept {
            auto& glyphs{ static_cast<Layout*>(client_drawing_context)->glyphs };
            f32 x{ baseline_origin_x }, y{ baseline_origin_y };
            wil::com_ptr face_base{ glyph_run->fontFace };
            wil::com_ptr face{ face_base.query<decltype(Layout::Glyph::face)::element_type>() };
            f32 size{ glyph_run->fontEmSize };
            // ignore glyph_run->isSideways
            bool is_ltr{ glyph_run->bidiLevel % 2 == 0 };
            for (usize i{}; i < glyph_run->glyphCount; ++i) {
                u16 gid{ glyph_run->glyphIndices[i] };
                f32 advance{ glyph_run->glyphAdvances[i] };
                f32 offset_x{ glyph_run->glyphOffsets[i].advanceOffset };
                f32 offset_y{ glyph_run->glyphOffsets[i].ascenderOffset };
                if (is_ltr) {
                    glyphs.push_back({ face, size, gid, x + offset_x, y - offset_y });
                    x += advance;
                }
                else {
                    glyphs.push_back({ face, size, gid, x - offset_x, y - offset_y });
                    x -= advance;
                }
            }
            return 0;
        }

        STDMETHODIMP DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) noexcept {
            return 0;
        }

        STDMETHODIMP DrawStrikethrough(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) noexcept {
            return 0;
        }

        STDMETHODIMP DrawUnderline(void*, FLOAT, FLOAT, const DWRITE_UNDERLINE*, IUnknown*) noexcept {
            return 0;
        }
    };

    struct Option {
        std::wstring font{ L"Microsoft YaHei" };
        DWRITE_FONT_WEIGHT weight{ DWRITE_FONT_WEIGHT_NORMAL };
        DWRITE_FONT_STYLE style{ DWRITE_FONT_STYLE_NORMAL };
        DWRITE_FONT_STRETCH stretch{ DWRITE_FONT_STRETCH_NORMAL };
        f32 size{ 100 };
        std::wstring locale{};
        f32 box_width{ std::numeric_limits<f32>::max() };
        f32 box_height{ std::numeric_limits<f32>::max() };
        f32 origin_x{};
        f32 origin_y{};
    };

    export class Draw {
        Option _option;

        usize _render_width{};
        usize _render_height{};
        wil::com_ptr<IDirect3DTexture8> _target;
        wil::com_ptr<ID3DXSprite> _sprite;

        wil::com_ptr<IDWriteTextFormat3> _format;
        Cache<std::wstring, Layout, 1024> _layout;
        GlyphAtlas<4> _atlas;

        void _update_target() {
            THROW_IF_FAILED(
                Direct3D::device()->CreateTexture(
                    _render_width,
                    _render_height,
                    1,
                    0,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_MANAGED,
                    &_target
                )
            );

            THROW_IF_FAILED(
                D3DXCreateSprite(
                    Direct3D::device(),
                    &_sprite
                )
            );
        }

        void _update_format() {
            wil::com_ptr<IDWriteTextFormat> format_base;
            THROW_IF_FAILED(
                env::dw_factory->CreateTextFormat(
                    _option.font.data(),
                    nullptr,
                    _option.weight,
                    _option.style,
                    _option.stretch,
                    _option.size,
                    _option.locale.data(),
                    &format_base
                )
            );
            _format = format_base.query<decltype(_format)::element_type>();

            _layout.clear();
        }

    public:
        void text(f32 x, f32 y, std::string_view text) {
            usize render_width{ Direct3D::render_width() }, render_height{ Direct3D::render_height() };
            if (!_target || !_sprite || _render_width != render_width || _render_height != render_height) {
                _render_width = render_width;
                _render_height = render_height;
                _update_target();
            }

            if (!_format) {
                _update_format();
            }

            std::wstring text_u16{ to_wstring(text) };
            auto& glyph_layout{ _layout.get(
                text_u16,
                [&] {
                    wil::com_ptr<IDWriteTextLayout> dw_layout_base;
                    THROW_IF_FAILED(
                        env::dw_factory->CreateTextLayout(
                            text_u16.data(),
                            text_u16.size(),
                            _format.get(),
                            _option.box_width,
                            _option.box_height,
                            &dw_layout_base
                        )
                    );
                    wil::com_ptr dw_layout{ dw_layout_base.query<IDWriteTextLayout4>() };

                    wil::com_ptr<LayoutCollector> collector;
                    collector.attach(winrt::make_self<LayoutCollector>().detach());
                    Layout layout;
                    THROW_IF_FAILED(
                        dw_layout->Draw(
                            &layout,
                            collector.get(),
                            x - _option.origin_x,
                            y - _option.origin_y
                        )
                    );

                    return layout;
                }
            ).first.glyphs };

            std::vector<decltype(_atlas)::Glyph> glyphs;
            {
                auto lock{ _atlas.lock() };
                for (auto& glyph : glyph_layout) {
                    glyphs.push_back(
                        _atlas.get(
                            { glyph.face, glyph.gid },
                            lock,
                            [&] {
                                f32 advance{};
                                DWRITE_GLYPH_OFFSET offsets{};
                                DWRITE_GLYPH_RUN run{
                                    glyph.face.get(),
                                    glyph.size,
                                    1,
                                    &glyph.gid,
                                    &advance,
                                    &offsets,
                                };
                                wil::com_ptr<IDWriteGlyphRunAnalysis> rasterizer;
                                THROW_IF_FAILED(
                                    env::dw_factory->CreateGlyphRunAnalysis(
                                        &run,
                                        nullptr,
                                        DWRITE_RENDERING_MODE1_NATURAL,
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
                                std::vector<u8> alpha(width * height);
                                THROW_IF_FAILED(
                                    rasterizer->CreateAlphaTexture(
                                        DWRITE_TEXTURE_ALIASED_1x1,
                                        &bbox,
                                        alpha.data(),
                                        alpha.size()
                                    )
                                );

                                return std::tuple{ alpha, width, height, bbox.left, bbox.top };
                            }
                        ).first
                    );
                }
            }

            (void)glyphs;
        }
    };

}

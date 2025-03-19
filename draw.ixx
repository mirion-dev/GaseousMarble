module;

#include <assert.h>
#include <d3dx8.h>
#include <Windows.h>

export module gm.draw;

import std;
import gm.core;
import gm.engine;

namespace gm::draw {

    struct FontDeleter {
        void operator()(ID3DXFont* font) const {
            if (font != nullptr) {
                font->Release();
            }
        }
    };

    struct DrawSetting {
        u32 color{ 0xffffffff };
    };

    export class Font {
        std::unique_ptr<ID3DXFont, FontDeleter> _font;
        LOGFONTW _attrs{ .lfCharSet{ DEFAULT_CHARSET } };

        u32 _draw_format{};
        DrawSetting _draw_setting;

        bool _update() noexcept {
            ID3DXFont* font;
            if (D3DXCreateFontIndirect(gm::engine::direct3d.device(), &_attrs, &font)) {
                return false;
            }
            _font.reset(font);
            return true;
        }

    public:
        Font() noexcept = default;

        std::wstring name() noexcept {
            return _attrs.lfFaceName;
        }

        u32 size() noexcept {
            return _attrs.lfHeight;
        }

        u32 width() noexcept {
            return _attrs.lfWidth;
        }

        u32 weight() noexcept {
            return _attrs.lfWeight;
        }

        bool italic() noexcept {
            return _attrs.lfItalic;
        }

        // an empty string indicates the default font
        bool set_name(std::wstring_view new_name) noexcept {
            assert(new_name.size() < 32);
            if (name() == new_name) {
                return true;
            }
            std::memcpy(_attrs.lfFaceName, new_name.data(), new_name.size() * sizeof(wchar_t));
            return _update();
        }

        // 0 indicates the default size
        bool set_size(u32 new_size) noexcept {
            if (size() == new_size) {
                return true;
            }
            _attrs.lfHeight = new_size;
            return _update();
        }

        // 0 indicates keeping the aspect ratio
        bool set_width(u32 new_width) noexcept {
            if (width() == new_width) {
                return true;
            }
            _attrs.lfWidth = new_width;
            return _update();
        }

        // 0 indicates the default weight
        bool set_weight(u32 new_weight) noexcept {
            if (weight() == new_weight) {
                return true;
            }
            _attrs.lfWeight = new_weight;
            return _update();
        }

        bool set_italic(bool new_italic) noexcept {
            if (italic() == new_italic) {
                return true;
            }
            _attrs.lfItalic = new_italic;
            return _update();
        }

        bool draw(u32 x, u32 y, std::wstring_view text) noexcept {
            auto [width, height] { gm::engine::direct3d.render_size() };
            RECT rect{ 0, 0, width, height };
            _font->DrawTextW(text.data(), -1, &rect, _draw_format, _draw_setting.color);
        }
    };

}
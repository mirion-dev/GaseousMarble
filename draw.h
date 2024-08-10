#pragma once

#include "utils.h"

#include <fstream>
#include <ranges>
#include <unordered_map>

namespace gm {

	struct gm_api {
		function<void*, string> get_function_pointer;
		function<size_t, string, real, real, real, real, real> sprite_add;
		function<void, real> sprite_delete;
		function<void, real, real, real, real, real, real, real, real, real, real, real, real, real, real, real, real> draw_sprite_general;

		gm_api(void* ptr = nullptr) {
			if (ptr == nullptr) {
				return;
			}
			get_function_pointer = ptr;
			sprite_add = get_function_pointer("sprite_add");
			sprite_delete = get_function_pointer("sprite_delete");
			draw_sprite_general = get_function_pointer("draw_sprite_general");
		}
	};

	struct glyph_data {
		uint16_t x, y;
		uint16_t width;
		int16_t left;
	};

	struct font_data {
		size_t sprite_id;
		uint16_t size;
		uint16_t glyph_height;
		std::unordered_map<wchar_t, glyph_data> glyph;
	};

	class font_system : std::vector<font_data> {
		using base = std::vector<font_data>;

		gm_api _api;

	public:
		using base::size;
		using base::operator[];

		font_system(const gm_api& api = nullptr) {
			_api = api;
		}

		bool contains(size_t font_id) {
			return font_id < size() && (*this)[font_id].size != 0;
		}

		bool add(std::string_view sprite_path, std::string_view glyph_path) {
			std::ifstream file{glyph_path.data(), std::ios::binary};

			char magic[4];
			file.read(magic, 4);
			if (strcmp(magic, "GLY") != 0) {
				return false;
			}

			font_data font;
			font.sprite_id = _api.sprite_add(sprite_path.data(), 1, false, false, 0, 0);
			file.read((char*)&font.size, 2);
			file.read((char*)&font.glyph_height, 2);
			if (font.size == 0 || font.glyph_height == 0) {
				return false;
			}

			while (file) {
				wchar_t ch;
				glyph_data glyph;
				file.read((char*)&ch, 2);
				file.read((char*)&glyph, 8);
				font.glyph[ch] = std::move(glyph);
			}

			push_back(std::move(font));
			return true;
		}

		bool remove(size_t font_id) {
			if (!contains(font_id)) {
				return false;
			}
			font_data& font{(*this)[font_id]};
			_api.sprite_delete(font.sprite_id);
			font.size = 0;
			font.glyph.clear();
			return true;
		}

		void clear() {
			for (auto& i : *this) {
				_api.sprite_delete(i.sprite_id);
			}
			base::clear();
		}
	};

	struct draw_setting {
		size_t font_id{0};
		uint32_t color_top{0xffffff}, color_bottom{0xffffff};
		double alpha{1};
		int halign{-1}, valign{-1};
		double max_line_width{0};
		double letter_spacing{0};
		double word_spacing{0};
		double line_height{1};
		double offset_x{0}, offset_y{0};
		double scale_x{1}, scale_y{1};
	};

	class draw_system {
		gm_api _api;
		font_system _font;
		draw_setting _setting;

		void _drawChar(double x, double y, wchar_t ch) {
			font_data& font{_font[_setting.font_id]};
			glyph_data& glyph{font.glyph[ch]};
			_api.draw_sprite_general(
				font.sprite_id,
				0,
				glyph.x,
				glyph.y,
				glyph.width,
				font.glyph_height,
				x + glyph.left * _setting.scale_x,
				y,
				_setting.scale_x,
				_setting.scale_y,
				0,
				_setting.color_top,
				_setting.color_top,
				_setting.color_bottom,
				_setting.color_bottom,
				_setting.alpha
			);
		}

		void _drawLine(double x, double y, std::wstring_view line) {
			double scaled_letter_spacing{_setting.letter_spacing * _setting.scale_x};
			double scaled_word_spacing{_setting.word_spacing * _setting.scale_x};

			font_data& font{_font[_setting.font_id]};
			for (wchar_t ch : line) {
				glyph_data& glyph{font.glyph[ch]};
				_drawChar(x, y, ch);
				x += (glyph.left + glyph.width) * _setting.scale_x + scaled_letter_spacing;
				if (ch == ' ') {
					x += scaled_word_spacing;
				}
			}
		}

		void _drawLineR(double x, double y, std::wstring_view line) {
			double scaled_letter_spacing{_setting.letter_spacing * _setting.scale_x};
			double scaled_word_spacing{_setting.word_spacing * _setting.scale_x};

			font_data& font{_font[_setting.font_id]};
			for (wchar_t ch : line | std::views::reverse) {
				glyph_data& glyph{font.glyph[ch]};
				x -= (glyph.left + glyph.width) * _setting.scale_x;
				_drawChar(x, y, ch);
				x -= scaled_letter_spacing;
				if (ch == ' ') {
					x -= scaled_word_spacing;
				}
			}
		}

	public:
		draw_system(const gm_api& api = nullptr) {
			_api = api;
			_font = api;
		}

		font_system& font() {
			return _font;
		}

		draw_setting& setting() {
			return _setting;
		}

		bool draw(double x, double y, std::wstring_view text) {
			if (!_font.contains(_setting.font_id)) {
				return false;
			}

			font_data& font{_font[_setting.font_id]};

			double scaled_max_line_width{_setting.max_line_width * _setting.scale_x};
			double scaled_letter_spacing{_setting.letter_spacing * _setting.scale_x};
			double scaled_word_spacing{_setting.word_spacing * _setting.scale_x};
			double scaled_line_height{_setting.line_height * _setting.scale_y * font.size};
			double scaled_offset_x{_setting.offset_x * _setting.scale_x};
			double scaled_offset_y{_setting.offset_y * _setting.scale_y};

			std::vector<std::wstring_view> line;
			std::vector<double> offset;
			if (_setting.halign != 0 && scaled_max_line_width == 0) {
				for (auto&& i : text | std::views::split('\n')) {
					line.emplace_back(i);
				}
			}
			else {
				double line_width{0};
				auto begin{text.begin()}, end{text.end()};
				for (auto p{begin}; p != end; ++p) {
					if (*p == '\n') {
						offset.push_back((scaled_letter_spacing - line_width) / 2);
						line.emplace_back(begin, p);
						line_width = 0;
						begin = p + 1;
					}
					else {
						glyph_data& glyph{font.glyph[*p]};
						double char_width{(glyph.left + glyph.width) * _setting.scale_x + scaled_letter_spacing};
						if (*p == ' ') {
							char_width += scaled_word_spacing;
						}
						if (scaled_max_line_width == 0 || line_width + char_width <= scaled_max_line_width) {
							line_width += char_width;
						}
						else {
							offset.push_back((scaled_letter_spacing - line_width) / 2);
							line.emplace_back(begin, p);
							line_width = char_width;
							begin = p;
						}
					}
				}
				offset.push_back((scaled_letter_spacing - line_width) / 2);
				line.emplace_back(begin, end);
			}

			x += scaled_offset_x;
			y += scaled_offset_y;
			if (_setting.valign >= 0) {
				double text_height{scaled_line_height * line.size()};
				y -= _setting.valign == 0 ? text_height / 2 : text_height;
			}

			if (_setting.halign < 0) {
				for (auto& i : line) {
					_drawLine(x, y, i);
					y += scaled_line_height;
				}
			}
			else if (_setting.halign == 0) {
				auto q{offset.begin()};
				for (auto& i : line) {
					_drawLine(x + *q++, y, i);
					y += scaled_line_height;
				}
			}
			else {
				for (auto& i : line) {
					_drawLineR(x, y, i);
					y += scaled_line_height;
				}
			}

			return true;
		}
	};
}
#pragma once

#include "pch.h"

namespace gm {

	using real = double;
	using string = const char*;

	class var {
		std::uint32_t _type;
		double _real;
		char* _string;

	public:
		var(real r = 0) {
			_type = 0;
			_real = r;
			_string = nullptr;
		}

		var(string s) {
			_type = 1;
			_real = 0;
			if (s == nullptr) {
				_string = nullptr;
				return;
			}
			std::size_t size{std::strlen(s)};
			char* data{new char[size + 13]};
			new(data) std::uint32_t(0);
			new(data + 4) std::uint32_t(0);
			new(data + 8) std::uint32_t(size);
			std::memcpy(data + 12, s, size + 1);
			_string = data + 12;
		}

		~var() {
			if (_string == nullptr) {
				return;
			}
			delete[](_string - 12);
		}

		operator real() {
			return _real;
		}

		operator string() {
			return _string;
		}
	};

	template<class ret_t, class... args_t>
	class function {
		void* _ptr;

	public:
		function(real r = 0) {
			if (r <= 0) {
				_ptr = nullptr;
				return;
			}
			_ptr = (void*)(std::size_t)r;
		}

		ret_t operator()(args_t... a) {
			if (_ptr == nullptr) {
				return {};
			}
			var args[]{a...}, * pargs{args};
			std::size_t count{sizeof...(a)};
			var ret{}, * pret{&ret};
			void* pfunc{_ptr};
			__asm {
				push pargs;
				push count;
				push pret;
				call pfunc;
			}
			return ret;
		}
	};

	static function<real, string> get_function_pointer;
	static function<real, string, real, real, real, real, real> sprite_add;
	static function<real, real> sprite_get_width;
	static function<real, real> sprite_get_height;
	static function<real, real, real, real, real, real, real, real, real, real, real, real, real, real, real, real, real> draw_sprite_general;

	inline void init(real ptr) {
		get_function_pointer = ptr;
		sprite_add = get_function_pointer("sprite_add");
		sprite_get_width = get_function_pointer("sprite_get_width");
		sprite_get_height = get_function_pointer("sprite_get_height");
		draw_sprite_general = get_function_pointer("draw_sprite_general");
	}

}
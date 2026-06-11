#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include <dwrite_3.h>
#include <wil/result.h>

import std;
import gm;

using namespace gm;

static std::vector<Real> real_stack;
static std::vector<String> string_stack;

static std::unordered_map<std::string, Font> font_map;
static Draw draw;

auto internal_call_guard(usize real_count, usize string_count) {
    auto guard{ wil::scope_exit(
        [&] {
            real_stack.clear();
            string_stack.clear();
        }
    ) };

    if (real_stack.size() != real_count || string_stack.size() != string_count) {
        throw std::invalid_argument{ "Unexpected argument count." };
    }

    return guard;
}

API Real gm_internal_push_real(Real value) noexcept {
    real_stack.push_back(value);
    return S_OK;
}

API Real gm_internal_push_string(const char* value) noexcept {
    string_stack.push_back(value);
    return S_OK;
}

API Real gm_internal_new_font() noexcept
try {
    auto _{ internal_call_guard(4, 3) };
    return font_map.try_emplace(
            static_cast<std::string>(string_stack[0]),
            Font{
                to_wstring(string_stack[1]),
                static_cast<f32>(real_stack[0]),
                static_cast<DWRITE_FONT_WEIGHT>(real_stack[1]),
                static_cast<DWRITE_FONT_STYLE>(real_stack[2]),
                static_cast<DWRITE_FONT_STRETCH>(real_stack[3]),
                to_wstring(string_stack[2])
            }
        ).second ?
        S_OK :
        S_FALSE;
}
CATCH_RETURN()

API Real gm_init() noexcept
try {
    return env::init() ? S_OK : S_FALSE;
}
CATCH_RETURN()

API Real gm_set_font(const char* font_key_ptr) noexcept
try {
    auto iter{ font_map.find(font_key_ptr) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Invalid font key." };
    }

    draw.set_font(&iter->second);
    return S_OK;
}
CATCH_RETURN()

API Real gm_draw(Real x, Real y, const char* text_ptr) noexcept
try {
    draw.text(static_cast<f32>(x), static_cast<f32>(y), String{ text_ptr });
    return S_OK;
}
CATCH_RETURN()

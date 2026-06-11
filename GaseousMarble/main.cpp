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

static std::vector<f64> real_stack;
static std::vector<std::string> string_stack;

static std::unordered_map<std::string, Font> font_map;
static Draw draw;

static auto internal_call_guard(usize real_count, usize string_count) {
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

API f64 gm_internal_push_real(f64 value) noexcept {
    real_stack.push_back(value);
    return S_OK;
}

API f64 gm_internal_push_string(const char* value) noexcept {
    string_stack.push_back(value);
    return S_OK;
}

API f64 gm_init() noexcept
try {
    return env::init() ? S_OK : S_FALSE;
}
CATCH_RETURN()

API f64 gm_internal_new_font() noexcept
try {
    auto _{ internal_call_guard(4, 3) };

    std::string key{ string_stack[0] };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    return font_map.try_emplace(
            std::move(key),
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

API f64 gm_delete_font(const char* font_key) noexcept {
    auto iter{ font_map.find(font_key) };
    if (iter == font_map.end()) {
        return S_FALSE;
    }

    if (draw.font() == &*iter) {
        draw.set_font(nullptr);
    }

    font_map.erase(iter);
    return S_OK;
}

API f64 gm_set_font(const char* font_key) noexcept
try {
    auto iter{ font_map.find(font_key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API const char* gm_get_font() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? "" : font->first.data();
}

API f64 gm_draw(f64 x, f64 y, const char* text) noexcept
try {
    draw.text(static_cast<f32>(x), static_cast<f32>(y), text);
    return S_OK;
}
CATCH_RETURN()

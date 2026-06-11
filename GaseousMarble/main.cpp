#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include <wil/result.h>

import std;
import gm;

using namespace gm;

static Draw draw;

API Real gm_init() noexcept
try {
    return env::init() ? S_OK : S_FALSE;
}
CATCH_RETURN();

API Real gm_draw(Real x, Real y, const char* text_ptr) noexcept {
    return wil::ResultFromException([&] { draw.text(static_cast<f32>(x), static_cast<f32>(y), String{ text_ptr }); });
}

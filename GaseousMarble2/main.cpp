#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

static Draw draw;

API Real gm_draw(Real x, Real y, StringView text) noexcept {
    return draw.text(static_cast<f32>(x), static_cast<f32>(y), text);
}

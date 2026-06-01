#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include <wil/result.h>

import std;
import gm;

using namespace gm;

static std::optional<Draw> draw;

API Real gm_init() noexcept {
    return wil::ResultFromException(
        [&] {
            env::init();
            draw.emplace();
        }
    );
}

API Real gm_draw(Real x, Real y, StringView text) noexcept {
    return wil::ResultFromException(
        [&] {
            draw->text(static_cast<f32>(x), static_cast<f32>(y), text);
        }
    );
}

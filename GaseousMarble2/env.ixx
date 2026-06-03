module;

#include <dwrite_3.h>
#include <wil/com.h>

export module gm.env;

import std;

namespace gm::env {

    export wil::com_ptr<IDWriteFactory7> dw_factory;

    static bool initialized;

    export void init() {
        if (initialized) {
            return;
        }

        THROW_IF_FAILED(
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(decltype(dw_factory)::element_type),
                dw_factory.put_unknown()
            )
        );

        initialized = true;
    }

}

module;

#include <d2d1_3.h>
#include <d3d11_4.h>
#include <dwrite_3.h>
#include <wil/com.h>

export module gm.env;

import std;

namespace gm::env {

    export wil::com_ptr<ID3D11Device5> d3d_device;
    export wil::com_ptr<ID3D11DeviceContext4> d3d_context;
    export wil::com_ptr<IDXGIDevice4> dxgi_device;

    export wil::com_ptr<ID2D1Factory7> d2d_factory;
    export wil::com_ptr<ID2D1Device6> d2d_device;
    export wil::com_ptr<ID2D1DeviceContext6> d2d_context;

    export wil::com_ptr<IDWriteFactory7> dw_factory;

    static bool initialized;

    export void init() {
        if (initialized) {
            return;
        }

        wil::com_ptr<ID3D11Device> d3d_device_base;
        wil::com_ptr<ID3D11DeviceContext> d3d_context_base;
        std::array level{
            D3D_FEATURE_LEVEL_12_2,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        THROW_IF_FAILED(
            D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                level.data(),
                level.size(),
                D3D11_SDK_VERSION,
                &d3d_device_base,
                nullptr,
                &d3d_context_base
            )
        );
        d3d_device = d3d_device_base.query<decltype(d3d_device)::element_type>();
        d3d_context = d3d_context_base.query<decltype(d3d_context)::element_type>();
        dxgi_device = d3d_device.query<decltype(dxgi_device)::element_type>();

        THROW_IF_FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory));
        THROW_IF_FAILED(d2d_factory->CreateDevice(dxgi_device.get(), &d2d_device));
        THROW_IF_FAILED(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_context));

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

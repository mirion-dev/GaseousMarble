#include <Windows.h>
#include <gdiplus.h>

import gm.draw;

ULONG_PTR token;

BOOL APIENTRY DllMain(HMODULE, DWORD event, LPVOID) {
    switch (event) {
    case DLL_PROCESS_ATTACH: {
        gm::draw::draw.init();

        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&token, &input, nullptr);
        break;
    }
    case DLL_PROCESS_DETACH:
        Gdiplus::GdiplusShutdown(token);
        break;
    }
    return TRUE;
}
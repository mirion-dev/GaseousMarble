#include <Windows.h>

import gm.draw;

BOOL APIENTRY DllMain(HMODULE, DWORD event, LPVOID) {
    switch (event) {
    case DLL_PROCESS_ATTACH:
        gm::draw::draw.init();
        break;
    }
    return TRUE;
}
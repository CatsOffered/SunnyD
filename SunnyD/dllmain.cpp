#define _CRT_SECURE_NO_WARNINGS
#include "sunnyd.h"
#include "Hooks/Hooks.h"


void OnAttach(HMODULE hMod)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

    SunnyD::hInst = hMod;
    SunnyD::hThread = GetCurrentThread();
    SunnyD::hWin = CreateThread(0, 0, SunnyD::UIThread, 0, 0, 0);
    Hooks::Install();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OnAttach(hModule);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        MessageBox(0, "PROCESS_DETACH", "Info", MB_ICONINFORMATION);
        break;
    }
    return TRUE;
}


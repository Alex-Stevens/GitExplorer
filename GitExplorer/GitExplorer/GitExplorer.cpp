#include <Windows.h>
#include "GitExplorer.h"

// Handle the the DLL's module
GitExplorerModule atlModule;
HINSTANCE g_hInst;

// Standard DLL functions
STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void* lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        g_hInst = hInstance;
        DisableThreadLibraryCalls(hInstance);
    }

    return atlModule.DllMain(dwReason, lpReserved);
}

STDAPI DllCanUnloadNow()
{
    return atlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void** ppv)
{
    return atlModule.GetClassObject(clsid, riid, ppv);
}

STDAPI DllRegisterServer()
{
    return atlModule.DllRegisterServer(FALSE);
}

STDAPI DllUnregisterServer()
{
    return atlModule.DllUnregisterServer(FALSE);
}
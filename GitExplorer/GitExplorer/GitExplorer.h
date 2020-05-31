#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>

class GitExplorerModule : public CAtlDllModuleT<GitExplorerModule>
{
    //DECLARE_REGISTRY_RESOURCE(MODULESCRIPT)
};

extern GitExplorerModule atlModule;
extern HINSTANCE g_hInst;

#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

typedef HRESULT(*PFN_ExplorerCommandExecute)(IShellItemArray* psiItemArray, IUnknown* pv);

struct FVCOMMANDITEM
{
    const GUID* pguidCanonicalName;
    DWORD dwTitleID;
    DWORD dwToolTipID;
    PCWSTR pszIcon;
    EXPCMDFLAGS ecFlags;
    PFN_ExplorerCommandExecute pfnInvoke;
    const FVCOMMANDITEM* pFVCIChildren;
    UINT uChildCommands;
};
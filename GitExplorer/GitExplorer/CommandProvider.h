#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <atlbase.h>
#include <atlcom.h>

#include "CommandItem.h"

class CFolderViewCommandProvider : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                                 , public IExplorerCommandProvider
{
public:

    BEGIN_COM_MAP(CFolderViewCommandProvider)
        COM_INTERFACE_ENTRY(IExplorerCommandProvider)
    END_COM_MAP()

    // IExplorerCommandProvider
    IFACEMETHODIMP GetCommands(IUnknown* punkSite, REFIID riid, void** ppv);
    IFACEMETHODIMP GetCommand(REFGUID /* rguidCommandId */, REFIID /* riid */, void** ppv);

private:

    static HRESULT s_OnDisplay(IShellItemArray* psiItemArray, IUnknown* pv);
    static HRESULT s_OnSetting1(IShellItemArray* psiItemArray, IUnknown* pv);
    static HRESULT s_OnSetting2(IShellItemArray* psiItemArray, IUnknown* pv);
    static HRESULT s_OnSetting3(IShellItemArray* psiItemArray, IUnknown* pv);
    static HRESULT s_OnRequest(IShellItemArray* psiItemArray, IUnknown* pv);

private:
    static const FVCOMMANDITEM c_FVTaskSettings[];
    static const FVCOMMANDITEM c_FVTasks[];
};

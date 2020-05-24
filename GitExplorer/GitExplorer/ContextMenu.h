#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>

#include "Guid.h"

class CFolderViewImplContextMenu : public ATL::CComObjectRootEx<CComMultiThreadModel>
                                 , public ATL::CComCoClass<CFolderViewImplContextMenu, &CLSID_ContextMenu>
                                 , public IContextMenu
                                 , public IShellExtInit
                                 , public IObjectWithSite
{
public:

    BEGIN_COM_MAP(CFolderViewImplContextMenu)
        COM_INTERFACE_ENTRY(IContextMenu)
        COM_INTERFACE_ENTRY(IShellExtInit)
        COM_INTERFACE_ENTRY(IObjectWithSite)
    END_COM_MAP()

    DECLARE_NO_REGISTRY();

    CFolderViewImplContextMenu();
    ~CFolderViewImplContextMenu();
    
    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pRes, LPSTR pszName, UINT cchMax);

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID);

    // IObjectWithSite
    IFACEMETHODIMP SetSite(IUnknown* punkSite);
    IFACEMETHODIMP GetSite(REFIID riid, void** ppvSite);

private:

    IDataObject* _pdtobj;
    IUnknown* _punkSite;
};

OBJECT_ENTRY_AUTO(CLSID_ContextMenu, CFolderViewImplContextMenu)
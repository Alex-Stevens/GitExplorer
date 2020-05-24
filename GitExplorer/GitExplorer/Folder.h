#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <atlbase.h>
#include <atlcom.h>

#include "PidlDef.h"
#include "Utils.h"
#include "Guid.h"

class CFolderViewImplFolder : public ATL::CComObjectRootEx<CComMultiThreadModel>
                            , public ATL::CComCoClass<CFolderViewImplFolder, &CLSID_Namespace>
                            , public IShellFolder2
                            , public IPersistFolder2
                            , public IExplorerPaneVisibility
{
public:
    BEGIN_COM_MAP(CFolderViewImplFolder)
        COM_INTERFACE_ENTRY(IShellFolder2)
        COM_INTERFACE_ENTRY(IShellFolder)
        COM_INTERFACE_ENTRY(IPersistFolder2)
        COM_INTERFACE_ENTRY(IPersistFolder)
        COM_INTERFACE_ENTRY(IPersist)
        COM_INTERFACE_ENTRY(IExplorerPaneVisibility)
    END_COM_MAP()

    DECLARE_REGISTRY_RESOURCE(FolderRegistration)

    // IShellFolder
    IFACEMETHODIMP ParseDisplayName(HWND hwnd, IBindCtx* pbc, PWSTR pszName, ULONG* pchEaten, PIDLIST_RELATIVE* ppidl, ULONG* pdwAttributes);
    IFACEMETHODIMP EnumObjects(HWND hwnd, DWORD grfFlags, IEnumIDList** ppenumIDList);
    IFACEMETHODIMP BindToObject(PCUIDLIST_RELATIVE pidl, IBindCtx* pbc, REFIID riid, void** ppv);
    IFACEMETHODIMP BindToStorage(PCUIDLIST_RELATIVE pidl, IBindCtx* pbc, REFIID riid, void** ppv);
    IFACEMETHODIMP CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2);
    IFACEMETHODIMP CreateViewObject(HWND hwnd, REFIID riid, void** ppv);
    IFACEMETHODIMP GetAttributesOf(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, ULONG* rgfInOut);
    IFACEMETHODIMP GetUIObjectOf(HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* prgfInOut, void** ppv);
    IFACEMETHODIMP GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF shgdnFlags, STRRET* pName);
    IFACEMETHODIMP SetNameOf(HWND hwnd, PCUITEMID_CHILD pidl, PCWSTR pszName, DWORD uFlags, PITEMID_CHILD* ppidlOut);

    // IShellFolder2
    IFACEMETHODIMP GetDefaultSearchGUID(GUID* pGuid);
    IFACEMETHODIMP EnumSearches(IEnumExtraSearch** ppenum);
    IFACEMETHODIMP GetDefaultColumn(DWORD dwRes, ULONG* pSort, ULONG* pDisplay);
    IFACEMETHODIMP GetDefaultColumnState(UINT iColumn, SHCOLSTATEF* pbState);
    IFACEMETHODIMP GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY* pkey, VARIANT* pv);
    IFACEMETHODIMP GetDetailsOf(PCUITEMID_CHILD pidl, UINT iColumn, SHELLDETAILS* pDetails);
    IFACEMETHODIMP MapColumnToSCID(UINT iColumn, PROPERTYKEY* pkey);

    // IPersist
    IFACEMETHODIMP GetClassID(CLSID* pClassID);

    // IPersistFolder
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidl);

    // IPersistFolder2
    IFACEMETHODIMP GetCurFolder(PIDLIST_ABSOLUTE* ppidl);

    IFACEMETHODIMP GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE* peps);

    HRESULT _GetColumnDisplayName(PCUITEMID_CHILD pidl, const PROPERTYKEY* pkey, VARIANT* pv, PWSTR pszRet, UINT cch);

    static HRESULT CreateInstance(unsigned int level, CFolderViewImplFolder** ppv);

    static const int g_nMaxLevel = 5;

    CFolderViewImplFolder();
    ~CFolderViewImplFolder();

private:

    HRESULT SetLevel(unsigned int level);

    int                 m_nLevel;
    PIDLIST_ABSOLUTE    m_pidl;             // where this folder is in the name space
    PWSTR               m_rgNames[MAX_OBJS];
    WCHAR               m_szModuleName[MAX_PATH];    
};

OBJECT_ENTRY_AUTO(CLSID_Namespace, CFolderViewImplFolder)
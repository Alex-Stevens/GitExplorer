#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <shlobj.h>
#include <windows.h>

class CFolderViewCB : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                    , public IShellFolderViewCB
                    , public IFolderViewSettings
{
public:

    BEGIN_COM_MAP(CFolderViewCB)
        COM_INTERFACE_ENTRY(IShellFolderViewCB)
        COM_INTERFACE_ENTRY(IFolderViewSettings)
    END_COM_MAP()

    IFACEMETHODIMP MessageSFVCB(UINT, WPARAM, LPARAM);
    IFACEMETHODIMP GetColumnPropertyList(REFIID, void** ppv);
    IFACEMETHODIMP GetGroupByProperty(PROPERTYKEY*, BOOL*);
    IFACEMETHODIMP GetViewMode(FOLDERLOGICALVIEWMODE*);
    IFACEMETHODIMP GetIconSize(UINT*);
    IFACEMETHODIMP GetFolderFlags(FOLDERFLAGS* pfolderMask, FOLDERFLAGS* pfolderFlags);
    IFACEMETHODIMP GetSortColumns(SORTCOLUMN*, UINT, UINT*);
    IFACEMETHODIMP GetGroupSubsetCount(UINT*);

    static HRESULT CreateInstance(CFolderViewCB** ppv);
};
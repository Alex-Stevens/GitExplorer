#include "Callbacks.h"

IFACEMETHODIMP CFolderViewCB::MessageSFVCB(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetColumnPropertyList(REFIID /* riid */, void** ppv)
{
    *ppv = NULL; return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetGroupByProperty(PROPERTYKEY* /* pkey */, BOOL* /* pfGroupAscending */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetViewMode(FOLDERLOGICALVIEWMODE* /* plvm */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetIconSize(UINT* /* puIconSize */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetSortColumns(SORTCOLUMN* /* rgSortColumns */, UINT /* cColumnsIn */, UINT* /* pcColumnsOut */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetGroupSubsetCount(UINT* /* pcVisibleRows */)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP CFolderViewCB::GetFolderFlags(FOLDERFLAGS* pfolderMask, FOLDERFLAGS* pfolderFlags)
{
    if (pfolderMask)
    {
        *pfolderMask = FWF_USESEARCHFOLDER;
    }

    if (pfolderFlags)
    {
        *pfolderFlags = FWF_USESEARCHFOLDER;
    }

    return S_OK;
}

HRESULT CFolderViewCB::CreateInstance(CFolderViewCB** ppv)
{
    ATL::CComObject<CFolderViewCB>* callbacks;
    HRESULT hr = ATL::CComObject<CFolderViewCB>::CreateInstance(&callbacks);
    ATL::CComPtr<CFolderViewCB> callbacksPtr(callbacks);
    hr = callbacksPtr.CopyTo(ppv);
    return hr;
}
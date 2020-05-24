#include "Folder.h"
#include "PidlManager.h"

// Item idlists passed to folder methods are guaranteed to have accessible memory as specified
// by the cbSize in the itemid.  However they may be loaded from a persisted form (for example
// shortcuts on disk) where they could be corrupted.  It is the shell folder's responsibility
// to make sure it's safe against corrupted or malicious itemids.
PCFVITEMID PidlManager::IsValid(PCUIDLIST_RELATIVE pidl)
{
    PCFVITEMID pidmine = NULL;
    if (pidl)
    {
        pidmine = (PCFVITEMID)pidl;
        if (!(pidmine->cb && MYOBJID == pidmine->MyObjID && pidmine->nLevel <= CFolderViewImplFolder::g_nMaxLevel))
        {
            pidmine = NULL;
        }
    }
    return pidmine;
}

HRESULT PidlManager::GetProperty(PCUIDLIST_RELATIVE pidl, const PROPERTYKEY* pkey, VARIANT* pv)
{
    HRESULT hr = E_FAIL;

    BOOL pfIsFolder = FALSE;
    hr = PidlManager::GetFolderness(pidl, &pfIsFolder);
    if (FAILED(hr))
    {
        return hr;
    }

    if (!pfIsFolder && IsEqualPropertyKey(*pkey, PKEY_PropList_PreviewDetails))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom (or right) of the explorer browser.
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"prop:System.ItemNameDisplay;System.DateModified;System.Size");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_PropList_PreviewTitle))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom (or right) of the explorer browser.
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"prop:System.ItemNameDisplay;System.ItemType");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (pfIsFolder && IsEqualPropertyKey(*pkey, PKEY_Kind))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom (or right) of the explorer browser.
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(KIND_FOLDER);
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (pfIsFolder && IsEqualPropertyKey(*pkey, PKEY_KindText))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom (or right) of the explorer browser.
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"Kind text");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_PropList_TileInfo))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"prop:System.ItemNameDisplay");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_PropList_ExtendedTileInfo))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"prop:System.ItemNameDisplay");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_PropList_ContentViewModeForBrowse))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"prop:System.ItemName;System.DateModified;System.Size");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_LayoutPattern_ContentViewModeForBrowse))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = ::SysAllocString(LAYOUTPATTERN_CVMFB_ALPHA);
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_PerceivedType))
    {
        pv->vt = VT_INT;
        pv->intVal = 1;
        hr = S_OK;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_ItemType))
    {
        pv->vt = VT_BSTR;
        if (pfIsFolder)
        {
            pv->bstrVal = SysAllocString(L"Directory");
        }
        else
        {
            LPWSTR name = NULL;
            hr = PidlManager::GetName(pidl, &name);
            LPWSTR ext = PathFindExtension(name);
            pv->bstrVal = SysAllocString(ext);

            ::CoTaskMemFree(name);
        }

        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_ItemTypeText))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"A type text!");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_ItemSubType))
    {
        pv->vt = VT_BSTR;
        pv->bstrVal = SysAllocString(L"A subtype text!");
        hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_DateModified))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom of the explorer browser.
        pv->vt = VT_DATE;
        pv->date = 5.875;
        hr = S_OK;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_Size))
    {
        // This proplist indicates what properties are shown in the details pane at the bottom of the explorer browser.
        pv->vt = VT_UI8;
        pv->ullVal = 100;
        hr = S_OK;
    }
    else
    {
        ::OutputDebugString(L"Request for unknown property key.\n");
        hr = S_FALSE;
    }

    return hr;
}

HRESULT PidlManager::GetName(PCUIDLIST_RELATIVE pidl, PWSTR pszName, int cchMax)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        // StringCchCopy requires aligned strings, and itemids are not necessarily aligned.
        int i = 0;
        for (; i < cchMax; i++)
        {
            pszName[i] = pMyObj->szName[i];
            if (0 == pszName[i])
            {
                break;
            }
        }

        // Make sure the string is null-terminated.
        if (i == cchMax)
        {
            pszName[cchMax - 1] = 0;
        }
    }
    return hr;
}

HRESULT PidlManager::GetName(PCUIDLIST_RELATIVE pidl, PWSTR* ppsz)
{
    *ppsz = 0;
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        int const cch = pMyObj->cchName;
        *ppsz = (PWSTR)CoTaskMemAlloc(cch * sizeof(**ppsz));
        hr = *ppsz ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hr))
        {
            hr = GetName(pidl, *ppsz, cch);
        }
    }
    return hr;
}


HRESULT PidlManager::GetSides(PCUIDLIST_RELATIVE pidl, int* pSides)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        *pSides = pMyObj->nSides;
    }
    return hr;
}

HRESULT PidlManager::GetLevel(PCUIDLIST_RELATIVE pidl, int* pLevel)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        *pLevel = pMyObj->nLevel;
    }
    else  // If this fails we are at level zero.
    {
        *pLevel = 0;
    }
    return hr;
}

HRESULT PidlManager::GetSize(PCUIDLIST_RELATIVE pidl, int* pSize)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        *pSize = pMyObj->nSize;
    }
    return hr;
}

HRESULT PidlManager::GetFolderness(PCUIDLIST_RELATIVE pidl, BOOL* pfIsFolder)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    HRESULT hr = pMyObj ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        *pfIsFolder = pMyObj->fIsFolder;
    }
    return hr;
}

HRESULT PidlManager::ValidatePidl(PCUIDLIST_RELATIVE pidl)
{
    PCFVITEMID pMyObj = IsValid(pidl);
    return pMyObj ? S_OK : E_INVALIDARG;
}

HRESULT PidlManager::CreateChildID(PCWSTR pszName, int nLevel, int nSize, int nSides, BOOL fIsFolder, PITEMID_CHILD* ppidl)
{
    // Sizeof an object plus the next cb plus the characters in the string.
    UINT nIDSize = sizeof(FVITEMID) + sizeof(USHORT) + (lstrlen(pszName) * sizeof(WCHAR)) + sizeof(WCHAR);

    // Allocate and zero the memory.
    FVITEMID* lpMyObj = (FVITEMID*)CoTaskMemAlloc(nIDSize);

    HRESULT hr = lpMyObj ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        ZeroMemory(lpMyObj, nIDSize);
        lpMyObj->cb = static_cast<short>(nIDSize - sizeof(lpMyObj->cb));
        lpMyObj->MyObjID = MYOBJID;
        lpMyObj->cchName = (BYTE)(lstrlen(pszName) + 1);
        lpMyObj->nLevel = (BYTE)nLevel;
        lpMyObj->nSize = (BYTE)nSize;
        lpMyObj->nSides = (BYTE)nSides;
        lpMyObj->fIsFolder = (BOOL)fIsFolder;

        hr = StringCchCopy(lpMyObj->szName, lpMyObj->cchName, pszName);
        if (SUCCEEDED(hr))
        {
            *ppidl = (PITEMID_CHILD)lpMyObj;
        }
    }
    return hr;
}
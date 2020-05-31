#include "Folder.h"
#include "IdListEnumerator.h"
#include "CommandProvider.h"
#include "PidlManager.h"
#include "Guid.h"
#include "Callbacks.h"
#include "ExtractIcon.h"

#include <string>
#include <pathcch.h>

CFolderViewImplFolder::CFolderViewImplFolder() : m_nLevel(0)
{
}

CFolderViewImplFolder::~CFolderViewImplFolder()
{
    ::CoTaskMemFree(m_pidl);
    for (int i = 0; i < ARRAYSIZE(m_rgNames); i++)
    {
        CoTaskMemFree(m_rgNames[i]);
    }
}

//  Translates a display name into an item identifier list.
HRESULT CFolderViewImplFolder::ParseDisplayName(HWND hwnd, IBindCtx* pbc, PWSTR pszName, ULONG* pchEaten, PIDLIST_RELATIVE* ppidl, ULONG* pdwAttributes)
{
    HRESULT hr = E_INVALIDARG;

    if (NULL != pszName)
    {
        WCHAR szNameComponent[MAX_PATH] = {};

        // extract first component of the display name
        PWSTR pszNext = ::PathFindNextComponent(pszName);
        if (pszNext && *pszNext)
        {
            hr = ::StringCchCopyN(szNameComponent, ARRAYSIZE(szNameComponent), pszName, lstrlen(pszName) - lstrlen(pszNext));
        }
        else
        {
            hr = ::StringCchCopy(szNameComponent, ARRAYSIZE(szNameComponent), pszName);
        }

        if (SUCCEEDED(hr))
        {
            PathRemoveBackslash(szNameComponent);

            UINT uIndex = 0;
            hr = GetIndexFromDisplayString(szNameComponent, &uIndex);
            if (SUCCEEDED(hr))
            {
                BOOL fIsFolder = ISFOLDERFROMINDEX(uIndex);
                PIDLIST_RELATIVE pidlCurrent = NULL;
                hr = PidlManager::CreateChildID(szNameComponent, m_nLevel + 1, uIndex, 3, fIsFolder, &pidlCurrent);
                if (SUCCEEDED(hr))
                {
                    // If there are more components to parse, delegate to the child folder to handle the rest.
                    if (pszNext && *pszNext)
                    {
                        // Bind to current item
                        IShellFolder* psf;
                        hr = BindToObject(pidlCurrent, pbc, IID_PPV_ARGS(&psf));
                        if (SUCCEEDED(hr))
                        {
                            PIDLIST_RELATIVE pidlNext = NULL;
                            hr = psf->ParseDisplayName(hwnd, pbc, pszNext, pchEaten, &pidlNext, pdwAttributes);
                            if (SUCCEEDED(hr))
                            {
                                *ppidl = ILCombine(pidlCurrent, pidlNext);
                                ILFree(pidlNext);
                            }
                            psf->Release();
                        }

                        ILFree(pidlCurrent);
                    }
                    else
                    {
                        // transfer ownership to caller
                        *ppidl = pidlCurrent;
                    }
                }
            }
        }
    }
    return hr;
}

//  Allows a client to determine the contents of a folder by creating an item identifier enumeration object and returning
//  its IEnumIDList interface. The methods supported by that interface can then be used to enumerate the folder's contents.
HRESULT CFolderViewImplFolder::EnumObjects(HWND /* hwnd */, DWORD grfFlags, IEnumIDList** ppenumIDList)
{
    HRESULT hr;
    if (m_nLevel >= g_nMaxLevel)
    {
        *ppenumIDList = NULL;
        hr = S_FALSE; // S_FALSE is allowed with NULL out param to indicate no contents.
    }
    else
    {
        ATL::CComPtr<CFolderViewImplEnumIDList> idListEnumeratorPtr;
        hr = CFolderViewImplEnumIDList::CreateInstance(grfFlags, m_nLevel + 1, &idListEnumeratorPtr);
        idListEnumeratorPtr.QueryInterface(ppenumIDList);
    }

    return hr;
}


//  Factory for handlers for the specified item.
HRESULT CFolderViewImplFolder::BindToObject(PCUIDLIST_RELATIVE pidl, IBindCtx* pbc, REFIID riid, void** ppv)
{
    *ppv = NULL;
    HRESULT hr = PidlManager::ValidatePidl(pidl);
    if (SUCCEEDED(hr))
    {
        CFolderViewImplFolder* pCFolderViewImplFolder;
        hr = CFolderViewImplFolder::CreateInstance(m_nLevel + 1, &pCFolderViewImplFolder);

        if (SUCCEEDED(hr))
        {
            // Initialize it.
            PITEMID_CHILD pidlFirst = ILCloneFirst(pidl);
            hr = pidlFirst ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {
                PIDLIST_ABSOLUTE pidlBind = ILCombine(m_pidl, pidlFirst);
                hr = pidlBind ? S_OK : E_OUTOFMEMORY;
                if (SUCCEEDED(hr))
                {
                    hr = pCFolderViewImplFolder->Initialize(pidlBind);
                    if (SUCCEEDED(hr))
                    {
                        PCUIDLIST_RELATIVE pidlNext = ILNext(pidl);
                        if (ILIsEmpty(pidlNext))
                        {
                            // If we're reached the end of the idlist, return the interfaces we support for this item.
                            // Other potential handlers to return include IPropertyStore, IStream, IStorage, etc.
                            hr = pCFolderViewImplFolder->QueryInterface(riid, ppv);
                        }
                        else
                        {
                            // Otherwise we delegate to our child folder to let it bind to the next level.
                            hr = pCFolderViewImplFolder->BindToObject(pidlNext, pbc, riid, ppv);
                        }
                    }
                    CoTaskMemFree(pidlBind);
                }
                ILFree(pidlFirst);
            }
            pCFolderViewImplFolder->Release();
        }
    }
    return hr;
}

HRESULT CFolderViewImplFolder::BindToStorage(PCUIDLIST_RELATIVE pidl,
    IBindCtx* pbc, REFIID riid, void** ppv)
{
    return BindToObject(pidl, pbc, riid, ppv);
}


//  Helper function to help compare relative IDs.
HRESULT _ILCompareRelIDs(IShellFolder* psfParent, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2,
    LPARAM lParam)
{
    HRESULT hr;
    PCUIDLIST_RELATIVE pidlRel1 = ILNext(pidl1);
    PCUIDLIST_RELATIVE pidlRel2 = ILNext(pidl2);

    if (ILIsEmpty(pidlRel1))
    {
        if (ILIsEmpty(pidlRel2))
        {
            hr = ResultFromShort(0);  // Both empty
        }
        else
        {
            hr = ResultFromShort(-1);   // 1 is empty, 2 is not.
        }
    }
    else
    {
        if (ILIsEmpty(pidlRel2))
        {
            hr = ResultFromShort(1);  // 2 is empty, 1 is not
        }
        else
        {
            // pidlRel1 and pidlRel2 point to something, so:
            //  (1) Bind to the next level of the IShellFolder
            //  (2) Call its CompareIDs to let it compare the rest of IDs.
            PIDLIST_RELATIVE pidlNext = ILCloneFirst(pidl1);    // pidl2 would work as well
            hr = pidlNext ? S_OK : E_OUTOFMEMORY;
            if (pidlNext)
            {
                IShellFolder* psfNext;
                hr = psfParent->BindToObject(pidlNext, NULL, IID_PPV_ARGS(&psfNext));
                if (SUCCEEDED(hr))
                {
                    // We do not want to pass the lParam is IShellFolder2 isn't supported.
                    // Although it isn't important for this example it shoud be considered
                    // if you are implementing this for other situations.
                    IShellFolder2* psf2;
                    if (SUCCEEDED(psfNext->QueryInterface(&psf2)))
                    {
                        psf2->Release();  // We can use the lParam
                    }
                    else
                    {
                        lParam = 0;       // We can't use the lParam
                    }

                    // Also, the column mask will not be relevant and should never be passed.
                    hr = psfNext->CompareIDs((lParam & ~SHCIDS_COLUMNMASK), pidlRel1, pidlRel2);
                    psfNext->Release();
                }
                ::CoTaskMemFree(pidlNext);
            }
        }
    }
    return hr;
}

//  Called to determine the equivalence and/or sort order of two idlists.
HRESULT CFolderViewImplFolder::CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2)
{
    HRESULT hr;
    if (lParam & (SHCIDS_CANONICALONLY | SHCIDS_ALLFIELDS))
    {
        // First do a "canonical" comparison, meaning that we compare with the intent to determine item
        // identity as quickly as possible.  The sort order is arbitrary but it must be consistent.
        PWSTR psz1;
        hr = PidlManager::GetName(pidl1, &psz1);
        if (SUCCEEDED(hr))
        {
            PWSTR psz2;
            hr = PidlManager::GetName(pidl2, &psz2);
            if (SUCCEEDED(hr))
            {
                hr = ResultFromShort(StrCmp(psz1, psz2));
                ::CoTaskMemFree(psz2);
            }
            ::CoTaskMemFree(psz1);
        }

        // If we've been asked to do an all-fields comparison, test for any other fields that
        // may be different in an item that shares the same identity.  For example if the item
        // represents a file, the identity may be just the filename but the other fields contained
        // in the idlist may be file size and file modified date, and those may change over time.
        // In our example let's say that "level" is the data that could be different on the same item.
        if ((ResultFromShort(0) == hr) && (lParam & SHCIDS_ALLFIELDS))
        {
            int cLevel1 = 0, cLevel2 = 0;
            hr = PidlManager::GetLevel(pidl1, &cLevel1);
            if (SUCCEEDED(hr))
            {
                hr = PidlManager::GetLevel(pidl2, &cLevel2);
                if (SUCCEEDED(hr))
                {
                    hr = ResultFromShort(cLevel1 - cLevel2);
                }
            }
        }
    }
    else
    {
        // Compare child ids by column data (lParam & SHCIDS_COLUMNMASK).
        hr = ResultFromShort(0);
        switch (lParam & SHCIDS_COLUMNMASK)
        {
        case 0: // Column one, Name.
        {
            // Load the strings that represent the names
            if (!m_rgNames[0])
            {
                hr = LoadFolderViewImplDisplayStrings(m_rgNames, ARRAYSIZE(m_rgNames));
            }
            if (SUCCEEDED(hr))
            {
                PWSTR psz1;
                hr = PidlManager::GetName(pidl1, &psz1);
                if (SUCCEEDED(hr))
                {
                    PWSTR psz2;
                    hr = PidlManager::GetName(pidl2, &psz2);
                    if (SUCCEEDED(hr))
                    {
                        // Find their place in the array.
                        // This is a display sort so we want to sort by "one" "two" "three" instead of alphabetically.
                        int nPidlOne = 0, nPidlTwo = 0;
                        for (int i = 0; i < ARRAYSIZE(m_rgNames); i++)
                        {
                            if (0 == StrCmp(psz1, m_rgNames[i]))
                            {
                                nPidlOne = i;
                            }

                            if (0 == StrCmp(psz2, m_rgNames[i]))
                            {
                                nPidlTwo = i;
                            }
                        }

                        hr = ResultFromShort(nPidlOne - nPidlTwo);
                        ::CoTaskMemFree(psz2);
                    }
                    ::CoTaskMemFree(psz1);
                }
            }
            break;
        }
        case 1: // Column two, Size.
        {
            int nSize1 = 0, nSize2 = 0;
            hr = PidlManager::GetSize(pidl1, &nSize1);
            if (SUCCEEDED(hr))
            {
                hr = PidlManager::GetSize(pidl2, &nSize2);
                if (SUCCEEDED(hr))
                {
                    hr = ResultFromShort(nSize1 - nSize2);
                }
            }
            break;
        }
        case 2: // Column Three, Sides.
        {
            int nSides1 = 0, nSides2 = 0;
            hr = PidlManager::GetSides(pidl1, &nSides1);
            if (SUCCEEDED(hr))
            {
                hr = PidlManager::GetSides(pidl2, &nSides2);
                if (SUCCEEDED(hr))
                {
                    hr = ResultFromShort(nSides1 - nSides2);
                }
            }
            break;
        }
        case 3: // Column four, Level.
        {
            int cLevel1 = 0, cLevel2 = 0;
            hr = PidlManager::GetLevel(pidl1, &cLevel1);
            if (SUCCEEDED(hr))
            {
                hr = PidlManager::GetLevel(pidl2, &cLevel2);
                if (SUCCEEDED(hr))
                {
                    hr = ResultFromShort(cLevel1 - cLevel2);
                }
            }
            break;
        }
        default:
        {
            hr = ResultFromShort(1);
        }
        }
    }

    if (ResultFromShort(0) == hr)
    {
        // Continue on by binding to the next level.
        hr = _ILCompareRelIDs(this, pidl1, pidl2, lParam);
    }
    return hr;
}

//  Called by the Shell to create the View Object and return it.
HRESULT CFolderViewImplFolder::CreateViewObject(HWND hwnd, REFIID riid, void** ppv)
{
    *ppv = NULL;

    HRESULT hr = E_NOINTERFACE;
    if (riid == IID_IShellView)
    {
        SFV_CREATE csfv = { sizeof(csfv), 0 };
        hr = QueryInterface(IID_PPV_ARGS(&csfv.pshf));
        if (SUCCEEDED(hr))
        {
            ATL::CComPtr<CFolderViewCB> callbacks;
            hr = CFolderViewCB::CreateInstance(&callbacks);
            if (SUCCEEDED(hr))
            {
                hr = callbacks->QueryInterface(IID_PPV_ARGS(&csfv.psfvcb));
                hr = ::SHCreateShellFolderView(&csfv, (IShellView**)ppv);
            }
            csfv.pshf->Release();
        }
    }
    else if (riid == IID_IContextMenu)
    {
        // This is the background context menu for the folder itself, not the context menu on items within it.
        DEFCONTEXTMENU dcm = { hwnd, NULL, m_pidl, static_cast<IShellFolder2*>(this), 0, NULL, NULL, 0, NULL };
        hr = ::SHCreateDefaultContextMenu(&dcm, riid, ppv);
    }
    else if (riid == IID_IExplorerCommandProvider)
    {
        ATL::CComObject<CFolderViewCommandProvider>* commandProvider;
        hr = ATL::CComObject<CFolderViewCommandProvider>::CreateInstance(&commandProvider);
        ATL::CComPtr<IExplorerCommandProvider> commandProviderPtr(commandProvider);
        commandProviderPtr.CopyTo((IExplorerCommandProvider**)ppv);
    }
    return hr;
}

//  Retrieves the attributes of one or more file objects or subfolders.
HRESULT CFolderViewImplFolder::GetAttributesOf(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, ULONG* rgfInOut)
{
    // If SFGAO_FILESYSTEM is returned, GetDisplayNameOf(SHGDN_FORPARSING) on that item MUST return a filesystem path.
    HRESULT hr = E_INVALIDARG;
    if (1 == cidl)
    {
        int nLevel = 0;
        hr = PidlManager::GetLevel(apidl[0], &nLevel);
        if (SUCCEEDED(hr))
        {
            BOOL fIsFolder = FALSE;
            hr = PidlManager::GetFolderness(apidl[0], &fIsFolder);
            if (SUCCEEDED(hr))
            {
                DWORD dwAttribs = 0;

                dwAttribs |= SFGAO_HASPROPSHEET;

                if (fIsFolder)
                {
                    dwAttribs |= SFGAO_FOLDER;
                }
                if (nLevel < g_nMaxLevel)
                {
                    dwAttribs |= SFGAO_HASSUBFOLDER;
                }
                *rgfInOut &= dwAttribs;
            }
        }
    }
    return hr;
}

//  Retrieves an OLE interface that can be used to carry out actions on the specified file objects or folders.
HRESULT CFolderViewImplFolder::GetUIObjectOf(HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT* /* prgfInOut */, void** ppv)
{
    *ppv = NULL;
    HRESULT hr;

    if (riid == IID_IContextMenu)
    {
        // The default context menu will call back for IQueryAssociations to determine the
        // file associations with which to populate the menu.
        DEFCONTEXTMENU const dcm = { hwnd, NULL, m_pidl, static_cast<IShellFolder2*>(this),
                               cidl, apidl, NULL, 0, NULL };
        hr = SHCreateDefaultContextMenu(&dcm, riid, ppv);
    }
    else if (riid == IID_IExtractIconW)
    {
        PWSTR name;
        hr = PidlManager::GetName(apidl[0], &name);

        ATL::CComPtr<CExtractIcon> iconExtractor;
        hr = CExtractIcon::CreateInstance(name, &iconExtractor);
        hr = iconExtractor->QueryInterface(riid, ppv);

        ::CoTaskMemFree(name);
        return hr;

        IDefaultExtractIconInit* pdxi;
        hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&pdxi));
        if (SUCCEEDED(hr))
        {
            BOOL fIsFolder = FALSE;
            hr = PidlManager::GetFolderness(apidl[0], &fIsFolder);
            if (SUCCEEDED(hr))
            {
                // This refers to icon indices in shell32.  You can also supply custom icons or
                // register IExtractImage to support general images.
                hr = pdxi->SetNormalIcon(L"shell32.dll", fIsFolder ? 4 : 1);
            }
            if (SUCCEEDED(hr))
            {
                hr = pdxi->QueryInterface(riid, ppv);
            }
            pdxi->Release();
        }
    }
    else if (riid == IID_IDataObject)
    {
        hr = SHCreateDataObject(m_pidl, cidl, apidl, NULL, riid, ppv);
    }
    else if (riid == IID_IQueryAssociations)
    {
        BOOL fIsFolder = FALSE;
        hr = PidlManager::GetFolderness(apidl[0], &fIsFolder);
        if (SUCCEEDED(hr))
        {
            if (fIsFolder)
            {
                ASSOCIATIONELEMENT const rgAssocFolder[] =
                {
                    { ASSOCCLASS_PROGID_STR, NULL, L"GitExplorerType"},
                    { ASSOCCLASS_FOLDER, NULL, NULL},
                };
                hr = ::AssocCreateForClasses(rgAssocFolder, ARRAYSIZE(rgAssocFolder), riid, ppv);
            }
            else
            {
                ASSOCIATIONELEMENT const rgAssocItem[] =
                {
                    { ASSOCCLASS_PROGID_STR, NULL, L"GitExplorerType"},
                };
                hr = ::AssocCreateForClasses(rgAssocItem, ARRAYSIZE(rgAssocItem), riid, ppv);
            }
        }
    }
    else
    {
        hr = E_NOINTERFACE;
    }
    return hr;
}

//  Retrieves the display name for the specified file object or subfolder.
HRESULT CFolderViewImplFolder::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF shgdnFlags, STRRET* pName)
{
    SHELLFLAGSTATE flags;
    ::SHGetSettings(&flags, SSF_SHOWEXTENSIONS);

    HRESULT hr = S_OK;
    if (shgdnFlags & SHGDN_FORPARSING)
    {
        WCHAR szDisplayName[MAX_PATH];
        if (shgdnFlags & SHGDN_INFOLDER)
        {
            // This form of the display name needs to be handled by ParseDisplayName.
            hr = PidlManager::GetName(pidl, szDisplayName, ARRAYSIZE(szDisplayName));
        }
        else
        {
            PWSTR pszThisFolder;
            hr = SHGetNameFromIDList(m_pidl, (shgdnFlags & SHGDN_FORADDRESSBAR) ? SIGDN_DESKTOPABSOLUTEEDITING : SIGDN_DESKTOPABSOLUTEPARSING, &pszThisFolder);
            if (SUCCEEDED(hr))
            {
                StringCchCopy(szDisplayName, ARRAYSIZE(szDisplayName), pszThisFolder);
                StringCchCat(szDisplayName, ARRAYSIZE(szDisplayName), L"\\");

                WCHAR szName[MAX_PATH];
                hr = PidlManager::GetName(pidl, szName, ARRAYSIZE(szName));
                if (SUCCEEDED(hr))
                {
                    StringCchCat(szDisplayName, ARRAYSIZE(szDisplayName), szName);
                }
                CoTaskMemFree(pszThisFolder);
            }
        }
        if (SUCCEEDED(hr))
        {
            if (!flags.fShowExtensions)
            {
                hr = PathCchRemoveExtension(szDisplayName, MAX_PATH);
            }

            hr = StringToStrRet(szDisplayName, pName);
        }
    }
    else
    {
        PWSTR pszName;
        hr = PidlManager::GetName(pidl, &pszName);
        if (SUCCEEDED(hr))
        {
            if (!flags.fShowExtensions)
            {
                WCHAR bu[MAX_PATH] = { 0 };
                StringCchCopy(bu, MAX_PATH, pszName);
                hr = PathCchRemoveExtension(bu, MAX_PATH);

                hr = StringToStrRet(bu, pName);
                CoTaskMemFree(pszName);

                return S_OK;
            }

            hr = StringToStrRet(pszName, pName);
            CoTaskMemFree(pszName);
        }
    }
    return hr;
}

//  Sets the display name of a file object or subfolder, changing the item identifier in the process.
HRESULT CFolderViewImplFolder::SetNameOf(HWND /* hwnd */, PCUITEMID_CHILD /* pidl */, PCWSTR /* pszName */, DWORD /* uFlags */, PITEMID_CHILD* ppidlOut)
{
    HRESULT hr = E_NOTIMPL;
    *ppidlOut = NULL;
    return hr;
}

//  IPersist method
HRESULT CFolderViewImplFolder::GetClassID(CLSID* pClassID)
{
    *pClassID = CLSID_Namespace;
    return S_OK;
}

//  IPersistFolder method
HRESULT CFolderViewImplFolder::Initialize(PCIDLIST_ABSOLUTE pidl)
{
    ZeroMemory(m_rgNames, sizeof(m_rgNames));
    m_pidl = ILCloneFull(pidl);
    return m_pidl ? S_OK : E_FAIL;
}

HRESULT CFolderViewImplFolder::EnumSearches(IEnumExtraSearch** ppEnum)
{
    *ppEnum = NULL;
    return E_NOINTERFACE;
}

//  Retrieves the default sorting and display column (indices from GetDetailsOf).
HRESULT CFolderViewImplFolder::GetDefaultColumn(DWORD /* dwRes */, ULONG* pSort, ULONG* pDisplay)
{
    *pSort = 0;
    *pDisplay = 0;
    return S_OK;
}

//  Retrieves the default state for a specified column.
HRESULT CFolderViewImplFolder::GetDefaultColumnState(UINT iColumn, SHCOLSTATEF* pcsFlags)
{
    HRESULT hr = (iColumn < 3) ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        *pcsFlags = SHCOLSTATE_ONBYDEFAULT | SHCOLSTATE_TYPE_STR;
    }
    return hr;
}

//  Requests the GUID of the default search object for the folder.
HRESULT CFolderViewImplFolder::GetDefaultSearchGUID(GUID* /* pguid */)
{
    return E_NOTIMPL;
}

//  Helper function for getting the display name for a column.
//  IMPORTANT: If cch is set to 0 the value is returned in the VARIANT.
HRESULT CFolderViewImplFolder::_GetColumnDisplayName(PCUITEMID_CHILD pidl, const PROPERTYKEY* pkey, VARIANT* pv, PWSTR pszRet, UINT cch)
{
    BOOL fIsFolder = FALSE;
    HRESULT hr = PidlManager::GetFolderness(pidl, &fIsFolder);
    if (SUCCEEDED(hr))
    {
        if (IsEqualPropertyKey(*pkey, PKEY_ItemNameDisplay))
        {
            PWSTR pszName;
            hr = PidlManager::GetName(pidl, &pszName);
            if (SUCCEEDED(hr))
            {
                if (pv != NULL)
                {
                    pv->vt = VT_BSTR;
                    pv->bstrVal = SysAllocString(pszName);
                    hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
                }
                else
                {
                    hr = StringCchCopy(pszRet, cch, pszName);
                }
                CoTaskMemFree(pszName);
            }
        }
        else if ((IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_AreaSize)) &&
            (!fIsFolder))
        {
            int nSize = 0;
            hr = PidlManager::GetSize(pidl, &nSize);
            if (SUCCEEDED(hr))
            {
                //This property is declared as "String" type.  See:  ExplorerDataProvider.propdesc
                WCHAR szFormattedSize[12];
                hr = StringCchPrintf(szFormattedSize, ARRAYSIZE(szFormattedSize), L"%d Sq. Ft.", nSize);
                if (cch)
                {
                    hr = StringCchCopy(pszRet, cch, szFormattedSize);
                }
                else
                {
                    pv->vt = VT_BSTR;
                    pv->bstrVal = SysAllocString(szFormattedSize);
                    hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
                }
            }
        }
        else if ((IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_NumberOfSides)) &&
            (!fIsFolder))
        {
            int nSides = 0;
            hr = PidlManager::GetSides(pidl, &nSides);
            if (SUCCEEDED(hr))
            {
                if (cch)
                {
                    hr = StringCchPrintf(pszRet, cch, L"%d", nSides);
                }
                else
                {
                    pv->vt = VT_I4;
                    pv->lVal = nSides;
                }
            }
        }
        else if (IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_DirectoryLevel))
        {
            int nLevel = 0;
            hr = PidlManager::GetLevel(pidl, &nLevel);
            if (SUCCEEDED(hr))
            {
                if (cch)
                {
                    hr = StringCchPrintf(pszRet, cch, L"%d", nLevel);
                }
                else
                {
                    pv->vt = VT_I4;
                    pv->lVal = nLevel;
                }
            }
        }
        else if ((IsEqualPropertyKey(*pkey, PKEY_ItemType)) &&
            (!fIsFolder))
        {
            int nSize = 0;
            hr = PidlManager::GetSize(pidl, &nSize);
            if (SUCCEEDED(hr))
            {
                WCHAR szFormattedSize[12];
                hr = StringCchPrintf(szFormattedSize, ARRAYSIZE(szFormattedSize), L"%d a", nSize);
                if (cch)
                {
                    hr = StringCchCopy(pszRet, cch, szFormattedSize);
                }
                else
                {
                    pv->vt = VT_BSTR;
                    pv->bstrVal = SysAllocString(szFormattedSize);
                    hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
                }
            }
        }
        else if ((IsEqualPropertyKey(*pkey, PKEY_ItemTypeText)) &&
            (!fIsFolder))
        {
            int nSize = 0;
            hr = PidlManager::GetSize(pidl, &nSize);
            if (SUCCEEDED(hr))
            {
                WCHAR szFormattedSize[12];
                hr = StringCchPrintf(szFormattedSize, ARRAYSIZE(szFormattedSize), L"%d b", nSize);
                if (cch)
                {
                    hr = StringCchCopy(pszRet, cch, szFormattedSize);
                }
                else
                {
                    pv->vt = VT_BSTR;
                    pv->bstrVal = SysAllocString(szFormattedSize);
                    hr = pv->bstrVal ? S_OK : E_OUTOFMEMORY;
                }
            }
        }
        else
        {
            if (pv)
            {
                VariantInit(pv);
            }

            if (pszRet)
            {
                *pszRet = '\0';
            }
        }
    }
    return hr;
}

//  Retrieves detailed information, identified by a property set ID (FMTID) and property ID (PID), on an item in a Shell folder.
HRESULT CFolderViewImplFolder::GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY* pkey, VARIANT* pv)
{
    HRESULT hr = PidlManager::GetProperty(pidl, pkey, pv);

    if (hr != S_OK)
    {
        hr = _GetColumnDisplayName(pidl, pkey, pv, NULL, 0);
    }

    return hr;
}

//  Retrieves detailed information, identified by a column index, on an item in a Shell folder.
HRESULT CFolderViewImplFolder::GetDetailsOf(PCUITEMID_CHILD pidl,
    UINT iColumn,
    SHELLDETAILS* pDetails)
{
    PROPERTYKEY key;
    HRESULT hr = MapColumnToSCID(iColumn, &key);
    pDetails->cxChar = 24;
    WCHAR szRet[MAX_PATH];

    if (!pidl)
    {
        // No item means we're returning information about the column itself.
        switch (iColumn)
        {
        case 0:
            pDetails->fmt = LVCFMT_LEFT;
            hr = StringCchCopy(szRet, ARRAYSIZE(szRet), L"Name");
            break;
        case 1:
            pDetails->fmt = LVCFMT_CENTER;
            hr = StringCchCopy(szRet, ARRAYSIZE(szRet), L"Size");
            break;
        case 2:
            pDetails->fmt = LVCFMT_CENTER;
            hr = StringCchCopy(szRet, ARRAYSIZE(szRet), L"Sides");
            break;
        case 3:
            pDetails->fmt = LVCFMT_CENTER;
            hr = StringCchCopy(szRet, ARRAYSIZE(szRet), L"Level");
            break;
        case 4:
            pDetails->fmt = LVCFMT_LEFT;
            hr = StringCchCopy(szRet, ARRAYSIZE(szRet), L"Type");
            break;
        default:
            // GetDetailsOf is called with increasing column indices until failure.
            hr = E_FAIL;
            break;
        }
    }
    else if (SUCCEEDED(hr))
    {
        hr = _GetColumnDisplayName(pidl, &key, NULL, szRet, ARRAYSIZE(szRet));
    }

    if (SUCCEEDED(hr))
    {
        hr = StringToStrRet(szRet, &pDetails->str);
    }

    return hr;
}

//  Converts a column name to the appropriate property set ID (FMTID) and property ID (PID).
HRESULT CFolderViewImplFolder::MapColumnToSCID(UINT iColumn, PROPERTYKEY* pkey)
{
    // The property keys returned here are used by the categorizer.
    HRESULT hr = S_OK;
    switch (iColumn)
    {
    case 0:
        *pkey = PKEY_ItemNameDisplay;
        break;
    case 1:
        *pkey = PKEY_Microsoft_SDKSample_AreaSize;
        break;
    case 2:
        *pkey = PKEY_Microsoft_SDKSample_NumberOfSides;
        break;
    case 3:
        *pkey = PKEY_Microsoft_SDKSample_DirectoryLevel;
        break;
    case 4:
        *pkey = PKEY_ItemType;
        break;
    default:
        hr = E_FAIL;
        break;
    }
    return hr;
}

//  Retrieves the PIDLIST_ABSOLUTE for the folder object.
HRESULT CFolderViewImplFolder::GetCurFolder(PIDLIST_ABSOLUTE* ppidl)
{
    *ppidl = NULL;
    HRESULT hr = m_pidl ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        *ppidl = ILCloneFull(m_pidl);
        hr = *ppidl ? S_OK : E_OUTOFMEMORY;
    }
    return hr;
}

IFACEMETHODIMP CFolderViewImplFolder::GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE* peps)
{
    HRESULT hr = S_OK;

    //if (ep == EP_Ribbon)
    //{
    //    *peps = EPS_FORCE | EPS_DEFAULT_ON;
    //}

    return hr;
}

HRESULT CFolderViewImplFolder::SetLevel(unsigned int level)
{
    this->m_nLevel = level;
    return S_OK;
}

HRESULT CFolderViewImplFolder::CreateInstance(unsigned int level, CFolderViewImplFolder** ppv)
{
    ATL::CComObject<CFolderViewImplFolder>* folderImplementation;
    HRESULT hr = ATL::CComObject<CFolderViewImplFolder>::CreateInstance(&folderImplementation);
    ATL::CComPtr<CFolderViewImplFolder> folderImplementationPtr(folderImplementation);

    hr = folderImplementationPtr->SetLevel(level);
    folderImplementationPtr.CopyTo(ppv);
    return hr;
}


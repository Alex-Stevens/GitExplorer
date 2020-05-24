#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>

#include "GitExplorer.h"
#include "CategoryProvider.h"
#include "CategoryEnumerator.h"
#include "CategorizerName.h"
#include "Guid.h"
#include "resource.h"

// ICategoryProvider methods
HRESULT CFolderViewImplCategoryProvider::CanCategorizeOnSCID(const PROPERTYKEY* pkey)
{
    HRESULT hr = S_FALSE;

    if (IsEqualPropertyKey(*pkey, PKEY_ItemNameDisplay) ||
        IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_AreaSize) ||
        IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_NumberOfSides) ||
        IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_DirectoryLevel))
    {
        hr = S_OK;
    }
    return hr;
}

//  Creates a category object.
HRESULT CFolderViewImplCategoryProvider::CreateCategory(const GUID* pguid, REFIID riid, void** ppv)
{
    HRESULT hr;
    *ppv = NULL;
    if (riid == IID_ICategorizer)
    {
        if (IsEqualGUID(*pguid, CAT_GUID_NAME))
        {
            ATL::CComPtr<IShellFolder2> _this;
            QueryInterface(IID_IShellFolder2, (void**)&_this);

            ATL::CComPtr<CFolderViewImplCategorizer_Name> ptr;
            hr = CFolderViewImplCategorizer_Name::CreateInstance(_this, &ptr);
            ptr->QueryInterface(riid, ppv);
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }
    else
    {
        hr = E_NOINTERFACE;
    }
    return hr;
}

HRESULT CFolderViewImplCategoryProvider::CreateInstance(ATL::CComPtr<IShellFolder2> psf, CFolderViewImplCategoryProvider** ppv)
{
    ATL::CComObject<CFolderViewImplCategoryProvider>* object;
    HRESULT hr = ATL::CComObject<CFolderViewImplCategoryProvider>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewImplCategoryProvider> ptr(object);

    hr = ptr->Initialize(psf);

    ptr.CopyTo(ppv);

    return hr;
}

HRESULT CFolderViewImplCategoryProvider::Initialize(ATL::CComPtr<IShellFolder2> psf)
{
    this->m_psf = psf;
    return S_OK;
}

//  Retrieves the enumerator for the categories.
HRESULT CFolderViewImplCategoryProvider::EnumCategories(IEnumGUID** penum)
{
    ATL::CComPtr<CFolderViewImplEnumGUID> enumerator;
    HRESULT hr = CFolderViewImplEnumGUID::CreateInstance(&enumerator);
    enumerator.QueryInterface<IEnumGUID>(penum);

    return hr;
}

//  Retrieves a globally unique identifier (GUID) that represents
//  the categorizer to use for the specified Shell column.
HRESULT CFolderViewImplCategoryProvider::GetCategoryForSCID(const PROPERTYKEY* pkey,
    GUID* pguid)
{
    HRESULT hr = S_OK;
    if (IsEqualPropertyKey(*pkey, PKEY_ItemNameDisplay))
    {
        *pguid = CAT_GUID_NAME;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_AreaSize))
    {
        *pguid = CAT_GUID_SIZE;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_NumberOfSides))
    {
        *pguid = CAT_GUID_SIDES;
    }
    else if (IsEqualPropertyKey(*pkey, PKEY_Microsoft_SDKSample_DirectoryLevel))
    {
        *pguid = CAT_GUID_LEVEL;
    }
    else if (IsEqualGUID(pkey->fmtid, GUID_NULL))
    {
        /* This can be called with a NULL
        format ID. This will happen if you have a category,
        not based on a column, that gets stored in the
        property bag. When a return is made to this item,
        it will call this function with a NULL format id. */
        *pguid = CAT_GUID_VALUE;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

//  Retrieves the name of the specified category. This is where
//  additional categories that appear under the column
//  related categories in the UI, get their display names.
HRESULT CFolderViewImplCategoryProvider::GetCategoryName(const GUID* pguid,
    PWSTR pszName,
    UINT cch)
{
    HRESULT hr = E_FAIL;
    if (IsEqualGUID(*pguid, CAT_GUID_VALUE))
    {
        hr = LoadString(g_hInst, IDS_VALUE, pszName, cch) ? S_OK : E_FAIL;
    }
    return hr;
}

//  Enables the folder to override the default grouping.
HRESULT CFolderViewImplCategoryProvider::GetDefaultCategory(GUID* pguid,
    PROPERTYKEY* pkey)
{
    *pguid = CAT_GUID_LEVEL;
    pkey = NULL;
    return S_OK;
}
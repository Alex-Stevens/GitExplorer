#include "CategorizerName.h"
#include "Utils.h"
#include "resource.h"
#include "GitExplorer.h"

//
//  Determines the relative order of two items in their item identifier lists.
//

HRESULT CFolderViewImplCategorizer_Name::CompareCategory(CATSORT_FLAGS /* csfFlags */, DWORD dwCategoryId1, DWORD dwCategoryId2)
{
    return ResultFromShort((short)(dwCategoryId1 - dwCategoryId2));
}

HRESULT CFolderViewImplCategorizer_Name::CreateInstance(ATL::CComPtr<IShellFolder2> psf, CFolderViewImplCategorizer_Name** ppv)
{
    ATL::CComObject<CFolderViewImplCategorizer_Name>* object;
    HRESULT hr = ATL::CComObject<CFolderViewImplCategorizer_Name>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewImplCategorizer_Name> ptr(object);

    ptr->Initialize(psf);

    return hr;
}

HRESULT CFolderViewImplCategorizer_Name::Initialize(ATL::CComPtr<IShellFolder2> psf)
{
    this->m_psf = psf;
    return S_OK;
}


//
//  Retrieves a list of categories associated with a list of identifiers.
//

HRESULT CFolderViewImplCategorizer_Name::GetCategory(UINT cidl,
    PCUITEMID_CHILD_ARRAY apidl,
    DWORD* rgCategoryIds)
{
    HRESULT hr = E_INVALIDARG; // cidl == 0
    for (UINT i = 0; i < cidl; i++)
    {
        VARIANT v = { };
        hr = m_psf->GetDetailsEx(apidl[i], &PKEY_ItemNameDisplay, &v);
        if (SUCCEEDED(hr))
        {
            rgCategoryIds[i] = (DWORD)v.bstrVal[0];
            VariantClear(&v);
        }
        else
        {
            break;
        }
    }
    return hr;
}

//  Retrieves information about a category, such as the default
//  display and the text to display in the user interface.
HRESULT CFolderViewImplCategorizer_Name::GetCategoryInfo(DWORD dwCategoryId,
    CATEGORY_INFO* pci)
{
    return StringCchPrintf(pci->wszName, ARRAYSIZE(pci->wszName), L"%C", (WCHAR)dwCategoryId);
}

//  Retrieves the name of a categorizer, such as "Group By Device
//  Type", that can be displayed in the user interface.
HRESULT CFolderViewImplCategorizer_Name::GetDescription(PWSTR pszDesc, UINT cch)
{
    return LoadString(g_hInst, IDS_GROUPBYALPHA, pszDesc, cch) ? S_OK : E_FAIL;
}


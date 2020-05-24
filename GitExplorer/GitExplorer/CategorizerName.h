#pragma once

#include <Windows.h>
#include <ShlObj_core.h>

#include <atlbase.h>
#include <atlcom.h>

class CFolderViewImplCategorizer_Name : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                                      , public ICategorizer
{
public:

    BEGIN_COM_MAP(CFolderViewImplCategorizer_Name)
        COM_INTERFACE_ENTRY(ICategorizer)
    END_COM_MAP()

    // ICategorizer
    IFACEMETHODIMP GetDescription(PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, DWORD* rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, CATEGORY_INFO* pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, DWORD dwCategoryId1, DWORD dwCategoryId2);

    static HRESULT CreateInstance(ATL::CComPtr<IShellFolder2> psf, CFolderViewImplCategorizer_Name** ppv);
    HRESULT Initialize(ATL::CComPtr<IShellFolder2> psf);

private:
    ATL::CComPtr<IShellFolder2> m_psf;
};
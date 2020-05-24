#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>

#include <atlbase.h>
#include <atlcom.h>

#define INITGUID
#include <guiddef.h>

class CFolderViewImplCategoryProvider : public ATL::CComObjectRoot
                                      , public ICategoryProvider
{
public:

    BEGIN_COM_MAP(CFolderViewImplCategoryProvider)
        COM_INTERFACE_ENTRY(ICategoryProvider)
    END_COM_MAP()

    // ICategoryProvider methods.
    IFACEMETHODIMP CanCategorizeOnSCID(const PROPERTYKEY* key);
    IFACEMETHODIMP GetDefaultCategory(GUID* pguid, PROPERTYKEY* key);
    IFACEMETHODIMP GetCategoryForSCID(const PROPERTYKEY* key, GUID* pguid);
    IFACEMETHODIMP EnumCategories(IEnumGUID** penum);
    IFACEMETHODIMP GetCategoryName(const GUID* pguid, PWSTR pszName, UINT cch);
    IFACEMETHODIMP CreateCategory(const GUID* pguid, REFIID riid, void** ppv);

    static HRESULT CreateInstance(ATL::CComPtr<IShellFolder2> psf, CFolderViewImplCategoryProvider** ppv);
    HRESULT Initialize(ATL::CComPtr<IShellFolder2> psf);

private:
    
    ATL::CComPtr<IShellFolder2> m_psf;
};

#define MAX_CATEGORIES      1  // These are additional categories beyond the columns

// Name
// {77559890-8E11-48e2-9B72-585FBA9CFFF4}
DEFINE_GUID(CAT_GUID_NAME, 0x77559890, 0x8e11, 0x48e2, 0x9b, 0x72, 0x58, 0x5f, 0xba, 0x9c, 0xff, 0xf4);

// Size
// {0210C647-A9B8-41fb-ACB7-3C57D27C5BC1}
DEFINE_GUID(CAT_GUID_SIZE, 0x210c647, 0xa9b8, 0x41fb, 0xac, 0xb7, 0x3c, 0x57, 0xd2, 0x7c, 0x5b, 0xc1);

// Type
// {D1873FB3-76BA-474d-BE69-ED52476DD7E3}
DEFINE_GUID(CAT_GUID_SIDES, 0xd1873fb3, 0x76ba, 0x474d, 0xbe, 0x69, 0xed, 0x52, 0x47, 0x6d, 0xd7, 0xe3);

// Level
// {F5A38A1F-3093-4dd9-9E6C-8762E4224F10}
DEFINE_GUID(CAT_GUID_LEVEL, 0xf5a38a1f, 0x3093, 0x4dd9, 0x9e, 0x6c, 0x87, 0x62, 0xe4, 0x22, 0x4f, 0x10);

// The next category guid is NOT based on a column
// {5DE8798F-9E55-4a78-804E-274A906BC3B3}
DEFINE_GUID(CAT_GUID_VALUE, 0x5de8798f, 0x9e55, 0x4a78, 0x80, 0x4e, 0x27, 0x4a, 0x90, 0x6b, 0xc3, 0xb3);
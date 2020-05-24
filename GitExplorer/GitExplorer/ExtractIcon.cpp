#include "ExtractIcon.h"

#include <strsafe.h>
#include <shellapi.h>
#include <pathcch.h>

HRESULT CExtractIcon::GetIconLocation(UINT uFlags, PWSTR pszIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
{
    HRESULT hr = S_OK;

    //PCWSTR ext = NULL;
    //size_t size = this->name.size();
    //hr = PathCchFindExtension(this->name.c_str(), size, &ext);
    LPWSTR ext = PathFindExtension(this->name.c_str());

    SHFILEINFOW info = { 0 };
    DWORD_PTR result = ::SHGetFileInfo(ext, FILE_ATTRIBUTE_NORMAL, &info, sizeof(SHFILEINFOW), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON);

    // Use system image list
    ::StringCchCopy(pszIconFile, cchMax, L"*");
    *piIndex = info.iIcon;
    *pwFlags |= GIL_NOTFILENAME;

    return hr;
}

HRESULT CExtractIcon::Extract(PCWSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize)
{
    return E_NOTIMPL;
}

HRESULT CExtractIcon::CreateInstance(std::wstring name, CExtractIcon** ppv)
{
    ATL::CComObject<CExtractIcon>* object;
    HRESULT hr = ATL::CComObject<CExtractIcon>::CreateInstance(&object);
    ATL::CComPtr<CExtractIcon> ptr(object);

    hr = ptr->Initialize(name);
    ptr.CopyTo(ppv);
    return hr;
}

HRESULT CExtractIcon::Initialize(const std::wstring& name)
{
    this->name = name;
    return S_OK;
}

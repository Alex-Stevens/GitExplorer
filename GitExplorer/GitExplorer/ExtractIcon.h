#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <shlobj_core.h>
#include <string>
#include <Windows.h>

class CExtractIcon : public ATL::CComObjectRootEx<CComMultiThreadModel>
                   , public IExtractIcon
{
public:
    BEGIN_COM_MAP(CExtractIcon)
        COM_INTERFACE_ENTRY(IExtractIconW)
    END_COM_MAP()

    IFACEMETHODIMP GetIconLocation(UINT uFlags, PWSTR pszIconFile, UINT cchMax, int* piIndex, UINT* pwFlags);
    IFACEMETHODIMP Extract(PCWSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize);

    static HRESULT CreateInstance(std::wstring name, CExtractIcon** ppv);
    HRESULT Initialize(const std::wstring& name);

private:

    std::wstring name;
};
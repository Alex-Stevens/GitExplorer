#include "PropertySheet.h"
#include "GitExplorer.h"
#include "resource.h"

HRESULT CPropertySheet::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
    ::InitCommonControls();

    ATL::CComPtr<IShellItem> ptr;
    HRESULT hr = ::SHCreateItemFromIDList(pidlFolder, IID_IShellItem, (void**) &ptr);
    if (SUCCEEDED(hr))
    {
        PWSTR name = NULL;
        hr = ptr->GetDisplayName(SIGDN_NORMALDISPLAY, &name);
        ::CoTaskMemFree(name);
    }

    ATL::CComPtr<IShellItem> ptr2;
    hr = ::SHGetItemFromDataObject(pdtobj, DOGIF_DEFAULT, IID_IShellItem, (void**)&ptr2);
    if (SUCCEEDED(hr))
    {
        PWSTR name2 = NULL;
        hr = ptr2->GetDisplayName(SIGDN_NORMALDISPLAY, &name2);
        ::CoTaskMemFree(name2);
    }

    return S_OK;
}

HRESULT CPropertySheet::AddPages(LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam)
{
    PROPSHEETPAGE psp;
    ::ZeroMemory(&psp, sizeof(PROPSHEETPAGE));

    psp.dwSize = sizeof(PROPSHEETPAGE);
    psp.dwFlags = PSP_USEREFPARENT | PSP_USETITLE | PSP_DEFAULT | PSP_HIDEHEADER;
    psp.hInstance = g_hInst;
    psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_MEDIUM);
    psp.pszTitle = __TEXT("Title");
    psp.pfnDlgProc = Callback;
    psp.lParam = NULL;
    psp.pcRefParent = reinterpret_cast<UINT*>(&atlModule.m_nLockCnt);

    HPROPSHEETPAGE hPage = ::CreatePropertySheetPage(&psp);

    if (hPage != NULL)
    {
        if (!pfnAddPage(hPage, lParam))
        {
            ::DestroyPropertySheetPage(hPage);
        }

        return NOERROR;
    }

    return E_INVALIDARG;
}

HRESULT CPropertySheet::ReplacePage(EXPPS uPageID, LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam)
{
    return E_NOTIMPL;
}

INT_PTR CPropertySheet::Callback(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
    case WM_INITDIALOG:
        return FALSE;
    default:
        break;
    }

    return FALSE;
}

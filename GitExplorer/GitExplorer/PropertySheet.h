#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <ShlObj.h>

#include "Guid.h"

class CPropertySheet : public ATL::CComObjectRootEx<CComMultiThreadModel>
                     , public ATL::CComCoClass<CPropertySheet, &CLSID_PROPERTYPAGE>
                     , public IShellExtInit
                     , public IShellPropSheetExt
{
public:

    BEGIN_COM_MAP(CPropertySheet)
        COM_INTERFACE_ENTRY(IShellExtInit)
        COM_INTERFACE_ENTRY(IShellPropSheetExt)
    END_COM_MAP()

    // Register property sheet
    DECLARE_REGISTRY_RESOURCE(FolderRegistration)

    // Inherited via IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID);

    // Inherited via IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);
    IFACEMETHODIMP ReplacePage(EXPPS uPageID, LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam);
    static INT_PTR CALLBACK Callback(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
};

OBJECT_ENTRY_AUTO(CLSID_PROPERTYPAGE, CPropertySheet)
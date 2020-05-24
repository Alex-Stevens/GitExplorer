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

#include "CommandItem.h"

// GUIDs for our commands
DEFINE_GUID(GUID_Display, 0xca133333, 0xd7d, 0x47ff, 0xbe, 0xe5, 0x2c, 0x99, 0xda, 0x47, 0xa7, 0xd9);
DEFINE_GUID(GUID_Settings, 0xbe901c03, 0xea3b, 0x4d3d, 0xb6, 0xf7, 0xc3, 0xd2, 0xfe, 0x94, 0xbe, 0x69);
DEFINE_GUID(GUID_Setting1, 0x3f6fa710, 0x63a8, 0x4843, 0x92, 0xce, 0xf3, 0x21, 0x6d, 0x2b, 0x20, 0xd6);
DEFINE_GUID(GUID_Setting2, 0x43077c60, 0x29c, 0x4e1e, 0x9c, 0xf6, 0x9c, 0x38, 0xb9, 0x51, 0x23, 0x42);
DEFINE_GUID(GUID_Setting3, 0xb8f3f98f, 0xddb6, 0x48b3, 0xbf, 0x93, 0x81, 0xd2, 0xa7, 0x91, 0xbf, 0xf9);
DEFINE_GUID(GUID_Request, 0x2d57b93c, 0x8ae8, 0x4f73, 0xa7, 0x5, 0xad, 0x11, 0x8f, 0x22, 0x57, 0xd6);

class CFolderViewCommand : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                         , public IExplorerCommand
{
public:

    BEGIN_COM_MAP(CFolderViewCommand)
        COM_INTERFACE_ENTRY(IExplorerCommand)
    END_COM_MAP()

    // IExplorerCommand
    IFACEMETHODIMP GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName);
    IFACEMETHODIMP GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon);
    IFACEMETHODIMP GetToolTip(IShellItemArray* psiItemArray, LPWSTR* ppszInfotip);
    IFACEMETHODIMP GetCanonicalName(GUID* pguidCommandName);
    IFACEMETHODIMP GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState);
    IFACEMETHODIMP Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc);
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS* pFlags);
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand** ppEnum);

    static HRESULT CreateInstance(FVCOMMANDITEM* pfvci, CFolderViewCommand** ppv);
    HRESULT Initialize(FVCOMMANDITEM* pfvci);

private:

    const FVCOMMANDITEM* _pfvci;
};
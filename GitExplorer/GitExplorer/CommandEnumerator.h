#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <atlbase.h>
#include <atlcom.h>

#include "CommandItem.h"

class CFolderViewCommandEnumerator : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                                   , public IEnumExplorerCommand
{
public:

    BEGIN_COM_MAP(CFolderViewCommandEnumerator)
        COM_INTERFACE_ENTRY(IEnumExplorerCommand)
    END_COM_MAP()

    // IEnumExplorerCommand
    IFACEMETHODIMP Next(ULONG celt, IExplorerCommand** apUICommand, ULONG* pceltFetched);
    IFACEMETHODIMP Skip(ULONG celt);
    IFACEMETHODIMP Reset();
    IFACEMETHODIMP Clone(IEnumExplorerCommand** ppenum);

    static HRESULT CreateInstance(const FVCOMMANDITEM* apfvc, UINT uCommands, CFolderViewCommandEnumerator** ppv);
    HRESULT Initialize(const FVCOMMANDITEM* apfvc, UINT uCommands);

private:
    HRESULT _CreateCommandFromCommandItem(FVCOMMANDITEM* pfvci, IExplorerCommand** ppExplorerCommand);
    
    ULONG _uCurrent;
    ULONG _uCommands;
    const FVCOMMANDITEM* _apfvci;
};
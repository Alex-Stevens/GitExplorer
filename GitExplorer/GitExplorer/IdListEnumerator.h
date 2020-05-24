#pragma once

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

#include <atlbase.h>
#include <atlcom.h>

#include "Folder.h"
#include "ShellPidlDef.h"

class CFolderViewImplEnumIDList : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                                , public IEnumIDList
{
public:

    BEGIN_COM_MAP(CFolderViewImplEnumIDList)
        COM_INTERFACE_ENTRY(IEnumIDList)
    END_COM_MAP()

    // IEnumIDList
    IFACEMETHODIMP Next(ULONG celt, PITEMID_CHILD* rgelt, ULONG* pceltFetched);
    IFACEMETHODIMP Skip(DWORD celt);
    IFACEMETHODIMP Reset();
    IFACEMETHODIMP Clone(IEnumIDList** ppenum);

    static HRESULT CreateInstance(DWORD grfFlags, int nCurrent, CFolderViewImplEnumIDList** ppv);
    HRESULT Initialize(DWORD grfFlags, int nCurrent);

private:

    DWORD m_grfFlags;
    int m_nItem;
    int m_nLevel;
    ITEMDATA m_aData[MAX_OBJS];
};
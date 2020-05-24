#pragma once

#include <ShlObj_core.h>
#include <atlbase.h>
#include <atlcom.h>

class CFolderViewImplEnumGUID : public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>
                              , public IEnumGUID
{
public:

    BEGIN_COM_MAP(CFolderViewImplEnumGUID)
        COM_INTERFACE_ENTRY(IEnumGUID)
    END_COM_MAP()

    // IEnumGUID
    IFACEMETHODIMP Next(ULONG celt, GUID* rgelt, ULONG* pceltFetched);
    IFACEMETHODIMP Skip(ULONG);
    IFACEMETHODIMP Reset();
    IFACEMETHODIMP Clone(IEnumGUID**);

    static HRESULT CreateInstance(CFolderViewImplEnumGUID** ppv);

private:

    ULONG m_ulCurrentIndex;
};
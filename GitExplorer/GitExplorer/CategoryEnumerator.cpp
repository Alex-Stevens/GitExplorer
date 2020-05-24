#include "CategoryEnumerator.h"
#include "CategoryProvider.h"
#include "Guid.h"
#include "GitExplorer.h"

//  Get next category GUID. The categories enumerated here
//  appear under the categories that are not releated to the
//  columns but can be categorized.
HRESULT CFolderViewImplEnumGUID::Next(ULONG celt, GUID* rgelt, ULONG* pceltFetched)
{
    HRESULT hr = (celt != 1) ? E_INVALIDARG : S_OK;
    if (SUCCEEDED(hr))
    {
        hr = (m_ulCurrentIndex < MAX_CATEGORIES) ? S_OK : S_FALSE;
        if (S_OK == hr)
        {
            switch (m_ulCurrentIndex++)
            {
            case 0:
                *rgelt = CAT_GUID_VALUE;
                break;
            }
        }
    }
    if (pceltFetched)
    {
        *pceltFetched = (S_OK == hr) ? 1 : 0;
    }
    return hr;
}

//  Skip cch number of categories.
HRESULT CFolderViewImplEnumGUID::Skip(ULONG cch)
{
    m_ulCurrentIndex += cch;
    return S_OK;
}

//  Reset the count to zero.
HRESULT CFolderViewImplEnumGUID::Reset()
{
    m_ulCurrentIndex = 0;
    return S_OK;
}

HRESULT CFolderViewImplEnumGUID::Clone(IEnumGUID** ppEnum)
{
    *ppEnum = NULL;
    return E_NOTIMPL;
}

HRESULT CFolderViewImplEnumGUID::CreateInstance(CFolderViewImplEnumGUID** ppv)
{
    ATL::CComObject<CFolderViewImplEnumGUID>* object;
    HRESULT hr = ATL::CComObject<CFolderViewImplEnumGUID>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewImplEnumGUID> ptr(object);

    return hr;
}

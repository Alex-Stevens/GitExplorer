#include "PidlManager.h"
#include "IdListEnumerator.h"

HRESULT CFolderViewImplEnumIDList::CreateInstance(DWORD grfFlags, int nCurrent, CFolderViewImplEnumIDList** ppv)
{
    ATL::CComObject<CFolderViewImplEnumIDList>* object;
    HRESULT hr = ATL::CComObject<CFolderViewImplEnumIDList>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewImplEnumIDList> ptr(object);

    hr = ptr->Initialize(grfFlags, nCurrent);
    ptr.CopyTo(ppv);
    return hr;
}

// This initializes the enumerator.  In this case we set up a default array of items, this represents our
// data source.  In a real-world implementation the array would be replaced and would be backed by some
// other data source that you traverse and convert into IShellFolder item IDs.
HRESULT CFolderViewImplEnumIDList::Initialize(DWORD grfFlags, int nCurrent)
{
    this->m_grfFlags = grfFlags;
    this->m_nLevel = nCurrent;
    this->m_nItem = 0;

    ::ZeroMemory(m_aData, sizeof(m_aData));

    HRESULT hr = S_OK;
    for (int i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(m_aData); i++)
    {
        hr = LoadFolderViewImplDisplayString(i, m_aData[i].szName, ARRAYSIZE(m_aData[i].szName));
        if (SUCCEEDED(hr))
        {
            // Just hardcode the values here now.
            m_aData[i].nSize = i;
            m_aData[i].nSides = 3;
            m_aData[i].fIsFolder = ISFOLDERFROMINDEX(i);
        }
    }
    return hr;
}

// Retrieves the specified number of item identifiers in
// the enumeration sequence and advances the current position
// by the number of items retrieved.
HRESULT CFolderViewImplEnumIDList::Next(ULONG celt, PITEMID_CHILD* rgelt, ULONG* pceltFetched)
{
    ULONG celtFetched = 0;

    HRESULT hr = (pceltFetched || celt <= 1) ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        ULONG i = 0;
        while (SUCCEEDED(hr) && i < celt && m_nItem < ARRAYSIZE(m_aData))
        {
            BOOL fSkip = FALSE;
            if (!(m_grfFlags & SHCONTF_STORAGE))
            {
                if (m_aData[m_nItem].fIsFolder)
                {
                    if (!(m_grfFlags & SHCONTF_FOLDERS))
                    {
                        // this is a folder, but caller doesnt want folders
                        fSkip = TRUE;
                    }
                }
                else
                {
                    if (!(m_grfFlags & SHCONTF_NONFOLDERS))
                    {
                        // this is a file, but caller doesnt want files
                        fSkip = TRUE;
                    }
                }
            }

            if (!fSkip)
            {
                hr = PidlManager::CreateChildID(m_aData[m_nItem].szName, m_nLevel, m_aData[m_nItem].nSize, m_aData[m_nItem].nSides, m_aData[m_nItem].fIsFolder, &rgelt[i]);
                if (SUCCEEDED(hr))
                {
                    celtFetched++;
                    i++;
                }
            }

            m_nItem++;
        }
    }

    if (pceltFetched)
    {
        *pceltFetched = celtFetched;
    }

    return (celtFetched == celt) ? S_OK : S_FALSE;
}

HRESULT CFolderViewImplEnumIDList::Skip(DWORD celt)
{
    m_nItem += celt;
    return S_OK;
}

HRESULT CFolderViewImplEnumIDList::Reset()
{
    m_nItem = 0;
    return S_OK;
}

HRESULT CFolderViewImplEnumIDList::Clone(IEnumIDList** ppenum)
{
    *ppenum = NULL;
    return E_NOTIMPL;
}
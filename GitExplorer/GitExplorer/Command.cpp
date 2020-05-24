#include "CommandEnumerator.h"
#include "Command.h"
#include "GitExplorer.h"

#include <atlbase.h>
#include <atlcom.h>

IFACEMETHODIMP CFolderViewCommand::GetTitle(IShellItemArray* /* psiItemArray */, LPWSTR* ppszName)
{
    *ppszName = NULL;
    HRESULT hr = E_FAIL;
    if (_pfvci)
    {
        WCHAR sz[100];
        hr = LoadString(g_hInst, _pfvci->dwTitleID, sz, ARRAYSIZE(sz)) ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            hr = SHStrDup(sz, ppszName);
        }
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommand::GetToolTip(IShellItemArray* /* psiItemArray */, LPWSTR* ppszInfotip)
{
    *ppszInfotip = NULL;
    HRESULT hr = E_FAIL;
    if (_pfvci)
    {
        WCHAR sz[100];
        hr = LoadString(g_hInst, _pfvci->dwToolTipID, sz, ARRAYSIZE(sz)) ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            hr = SHStrDup(sz, ppszInfotip);
        }
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommand::GetIcon(IShellItemArray* /* psiItemArray */, LPWSTR* ppszIcon)
{
    *ppszIcon = NULL;
    HRESULT hr = E_FAIL;
    if (_pfvci)
    {
        hr = SHStrDup(_pfvci->pszIcon, ppszIcon);
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommand::GetState(IShellItemArray* psiItemArray, BOOL /* fOkToBeSlow */, EXPCMDSTATE* pCmdState)
{
    HRESULT hr = S_OK;
    *pCmdState = ECS_DISABLED;
    if (_pfvci)
    {
        if (*(_pfvci->pguidCanonicalName) == GUID_Display)
        {
            if (psiItemArray)
            {
                DWORD dwNumItems;
                hr = psiItemArray->GetCount(&dwNumItems);
                if ((SUCCEEDED(hr)) && (dwNumItems > 0))
                {
                    *pCmdState = ECS_ENABLED;
                }
            }
        }
        else
        {
            *pCmdState = ECS_ENABLED;
        }
    }

    return hr;
}

IFACEMETHODIMP CFolderViewCommand::GetFlags(EXPCMDFLAGS* pFlags)
{
    if (_pfvci)
    {
        *pFlags = _pfvci->ecFlags;
    }
    return S_OK;
}


IFACEMETHODIMP CFolderViewCommand::GetCanonicalName(GUID* pguidCommandName)
{
    if (_pfvci)
    {
        *pguidCommandName = *(_pfvci->pguidCanonicalName);
    }
    return S_OK;
}

IFACEMETHODIMP CFolderViewCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc)
{
    HRESULT hr = S_OK; // If no function defined - just return S_OK
    if (_pfvci && _pfvci->pfnInvoke)
    {
        hr = _pfvci->pfnInvoke(psiItemArray, pbc);
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommand::EnumSubCommands(IEnumExplorerCommand** ppEnum)
{
    ATL::CComPtr<CFolderViewCommandEnumerator> ptr;
    HRESULT hr = CFolderViewCommandEnumerator::CreateInstance(_pfvci->pFVCIChildren, _pfvci->uChildCommands, &ptr);
    hr = ptr.QueryInterface<IEnumExplorerCommand>(ppEnum);

    return hr;
}

HRESULT CFolderViewCommand::CreateInstance(FVCOMMANDITEM* pfvci, CFolderViewCommand** ppv)
{
    ATL::CComObject<CFolderViewCommand>* object;
    HRESULT hr = ATL::CComObject<CFolderViewCommand>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewCommand> ptr(object);

    hr = ptr->Initialize(pfvci);
    ptr.CopyTo(ppv);

    return hr;
}

HRESULT CFolderViewCommand::Initialize(FVCOMMANDITEM* pfvci)
{
    this->_pfvci = pfvci;
    return S_OK;
}

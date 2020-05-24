#include "CommandEnumerator.h"
#include "Command.h"

HRESULT CFolderViewCommandEnumerator::_CreateCommandFromCommandItem(FVCOMMANDITEM* pfvci, IExplorerCommand** ppExplorerCommand)
{
    ATL::CComPtr<CFolderViewCommand> ptr;
    HRESULT hr = CFolderViewCommand::CreateInstance(pfvci, &ptr);
    hr = ptr.QueryInterface<IExplorerCommand>(ppExplorerCommand);
    return hr;
}

IFACEMETHODIMP CFolderViewCommandEnumerator::Next(ULONG celt, IExplorerCommand** apUICommand, ULONG* pceltFetched)
{
    HRESULT hr = S_FALSE;
    if (_uCurrent <= _uCommands)
    {
        UINT uIndex = 0;
        HRESULT hrLocal = S_OK;
        while (uIndex < celt && _uCurrent < _uCommands && SUCCEEDED(hrLocal))
        {
            hrLocal = _CreateCommandFromCommandItem((FVCOMMANDITEM*)&(_apfvci[_uCurrent]), &(apUICommand[uIndex]));
            uIndex++;
            _uCurrent++;
        }

        if (pceltFetched != NULL)
        {
            *pceltFetched = uIndex;
        }

        if (uIndex == celt)
        {
            hr = S_OK;
        }
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommandEnumerator::Skip(ULONG celt)
{
    _uCurrent += celt;

    HRESULT hr = S_OK;
    if (_uCurrent > _uCommands)
    {
        _uCurrent = _uCommands;
        hr = S_FALSE;
    }
    return hr;
}

IFACEMETHODIMP CFolderViewCommandEnumerator::Reset()
{
    _uCurrent = 0;
    return S_OK;
}

IFACEMETHODIMP CFolderViewCommandEnumerator::Clone(IEnumExplorerCommand** ppenum)
{
    *ppenum = NULL; return E_NOTIMPL;
}

HRESULT CFolderViewCommandEnumerator::CreateInstance(const FVCOMMANDITEM* apfvc, UINT uCommands, CFolderViewCommandEnumerator** ppv)
{
    ATL::CComObject<CFolderViewCommandEnumerator>* object;
    HRESULT hr = ATL::CComObject<CFolderViewCommandEnumerator>::CreateInstance(&object);
    ATL::CComPtr<CFolderViewCommandEnumerator> ptr(object);

    hr = ptr->Initialize(apfvc, uCommands);
    hr = ptr.CopyTo(ppv);
    return hr;
}

HRESULT CFolderViewCommandEnumerator::Initialize(const FVCOMMANDITEM* apfvc, UINT uCommands)
{
    this->_uCurrent = 0;
    this->_apfvci = apfvc;
    this->_uCommands = uCommands;

    return S_OK;
}

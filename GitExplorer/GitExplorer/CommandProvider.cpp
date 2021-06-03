#include "CommandProvider.h"
#include "CommandEnumerator.h"
#include "Command.h"
#include "resource.h"
#include "Guid.h"
#include "GitExplorer.h"
#include "Utils.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

const FVCOMMANDITEM CFolderViewCommandProvider::c_FVTaskSettings[] =
{
    // Icon reference should be replaced by absolute reference to own icon resource.
    {&GUID_Setting1, IDS_SETTING1,   IDS_SETTING1_TT,  L"shell32.dll,-16710",  ECF_DEFAULT, CFolderViewCommandProvider::s_OnSetting1, NULL, 0},
    {&GUID_Setting2, IDS_SETTING2,   IDS_SETTING2_TT,  L"shell32.dll,-16710",  ECF_DEFAULT, CFolderViewCommandProvider::s_OnSetting2, NULL, 0},
    {&GUID_Setting3, IDS_SETTING3,   IDS_SETTING3_TT,  L"shell32.dll,-16710",  ECF_DEFAULT, CFolderViewCommandProvider::s_OnSetting3, NULL, 0}
};

// Top-level commands
const FVCOMMANDITEM CFolderViewCommandProvider::c_FVTasks[] =
{
    // Icon reference should be replaced by absolute reference to own icon resource.
    {&GUID_Display,  IDS_DISPLAY,  IDS_DISPLAY_TT,   L"shell32.dll,-42",     ECF_DEFAULT,        CFolderViewCommandProvider::s_OnDisplay, NULL,             0 },
    {&GUID_Settings, IDS_SETTINGS, IDS_SETTINGS_TT,  L"shell32.dll,-16710",  ECF_HASSUBCOMMANDS, NULL,                                    c_FVTaskSettings, ARRAYSIZE(c_FVTaskSettings)},
    {&GUID_Request,  IDS_REQUEST,  IDS_REQUEST_TT,   L"shell32.dll,-16710",  ECF_DEFAULT,        CFolderViewCommandProvider::s_OnRequest, NULL,             0 }
};

IFACEMETHODIMP CFolderViewCommandProvider::GetCommand(REFGUID /* rguidCommandId */, REFIID /* riid */, void** ppv)
{
    *ppv = NULL; 
    return E_NOTIMPL;
}


IFACEMETHODIMP CFolderViewCommandProvider::GetCommands(IUnknown* /* punkSite */, REFIID riid, void** ppv)
{
    *ppv = NULL;

    ATL::CComPtr<CFolderViewCommandEnumerator> ptr;
    HRESULT hr = CFolderViewCommandEnumerator::CreateInstance(c_FVTasks, ARRAYSIZE(c_FVTasks), &ptr);
    hr = ptr->QueryInterface(riid, ppv);

    return hr;
}

HRESULT CFolderViewCommandProvider::s_OnDisplay(IShellItemArray* psiItemArray, IUnknown* /* pv */)
{
    return DisplayItem(psiItemArray, NULL);
}

HRESULT CFolderViewCommandProvider::s_OnRequest(IShellItemArray* psiItemArray, IUnknown* /* pv */)
{
    http_client client(U("https://api.github.com/"));

    // Build request URI and start the request.
    uri_builder builder(U("/zen"));
    pplx::task<http_response> requestTask = client.request(methods::GET, builder.to_string());
    requestTask.then([](pplx::task<http_response> response)
    {
        try
        {
            auto result = response.get();
            auto text = result.extract_string().get();
            MessageBox(NULL, text.c_str(), L"Result", MB_OK);
        }
        catch (const std::exception& ex)
        {
            MessageBoxA(NULL, ex.what(), "Result", MB_OK);
        }
    });
    return S_OK;
}

HRESULT CFolderViewCommandProvider::s_OnSetting1(IShellItemArray* /* psiItemArray */, IUnknown* /* pv */)
{
    WCHAR sz[100];
    HRESULT hr = LoadString(g_hInst, IDS_SETTING1, sz, ARRAYSIZE(sz)) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, sz, sz, MB_OK);
    }
    return hr;
}

HRESULT CFolderViewCommandProvider::s_OnSetting2(IShellItemArray* /* psiItemArray */, IUnknown* /* pv */)
{
    WCHAR sz[100];
    HRESULT hr = LoadString(g_hInst, IDS_SETTING2, sz, ARRAYSIZE(sz)) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, sz, sz, MB_OK);
    }
    return hr;
}

HRESULT CFolderViewCommandProvider::s_OnSetting3(IShellItemArray* /* psiItemArray */, IUnknown* /* pv */)
{
    WCHAR sz[100];
    HRESULT hr = LoadString(g_hInst, IDS_SETTING3, sz, ARRAYSIZE(sz)) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, sz, sz, MB_OK);
    }
    return hr;
}

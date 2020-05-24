#pragma once

#include <Windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <string>

#define ISFOLDERFROMINDEX(u) (BOOL)(u % 2)

HRESULT LoadFolderViewImplDisplayString(UINT uIndex, PWSTR psz, UINT cch);
HRESULT LoadFolderViewImplDisplayStrings(PWSTR wszArrStrings[], UINT cArray);
HRESULT GetIndexFromDisplayString(PCWSTR psz, UINT* puIndex);
STDAPI StringToStrRet(PCWSTR pszName, STRRET* pStrRet);
HRESULT DisplayItem(IShellItemArray* psia, HWND hwndParent);

#ifndef ResultFromShort
#define ResultFromShort(i)      MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(i))
#endif

__inline HRESULT ResultFromKnownLastError() { const DWORD err = GetLastError(); return err == ERROR_SUCCESS ? E_FAIL : HRESULT_FROM_WIN32(err); }

#define MAX_OBJS    10

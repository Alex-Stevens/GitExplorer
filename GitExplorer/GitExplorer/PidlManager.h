#pragma once

#include <Windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include "PidlDef.h"
#include "ShellPidlDef.h"

class PidlManager
{
public:
    static HRESULT GetName(PCUIDLIST_RELATIVE pidl, PWSTR pszName, int cchMax);
    static HRESULT GetName(PCUIDLIST_RELATIVE pidl, PWSTR* pszName);
    static HRESULT GetSides(PCUIDLIST_RELATIVE pidl, int* pSides);
    static HRESULT GetLevel(PCUIDLIST_RELATIVE pidl, int* pLevel);
    static HRESULT GetSize(PCUIDLIST_RELATIVE pidl, int* pSize);
    static HRESULT GetFolderness(PCUIDLIST_RELATIVE pidl, BOOL* pfIsFolder);
    static HRESULT ValidatePidl(PCUIDLIST_RELATIVE pidl);
    static PCFVITEMID IsValid(PCUIDLIST_RELATIVE pidl);

    // Temporary: Use CItemIDFactory class
    static HRESULT GetProperty(PCUIDLIST_RELATIVE pidl, const PROPERTYKEY* pkey, VARIANT* pv);

    // IDList constructor public for the enumerator object
    static HRESULT CreateChildID(PCWSTR pszName, int nLevel, int nSize, int nSides, BOOL fIsFolder, PITEMID_CHILD* ppidl);
};
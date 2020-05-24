#pragma once

#include <Windows.h>

typedef struct
{
    int nLevel;
    int nSize;
    int nSides;
    BOOL fIsFolder;
    WCHAR szName[MAX_PATH];
} ITEMDATA;
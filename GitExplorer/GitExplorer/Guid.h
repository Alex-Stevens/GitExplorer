#pragma once

// This file contains the CLSID and Property Keys used in this sample.
#define INITGUID
#include <guiddef.h>
#include <propkeydef.h>


// Command Line,
// explorer ::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{BA16CE0E-728C-4FC9-98E5-D0B35B384597}

DEFINE_GUID(CLSID_Namespace, 0x7db52233, 0x6fd2, 0x4810, 0x9d, 0xa5, 0x3c, 0xd7, 0x60, 0x4f, 0xaa, 0xa6);

DEFINE_GUID(CLSID_ContextMenu, 0x6abd2266, 0xad0d, 0x4801, 0xad, 0x51, 0x8f, 0x0d, 0x66, 0x92, 0xe0, 0x11);

// Col 2
// name="Microsoft.SDKSample.AreaSize"
// {CE8B09DD-B2D6-4751-A207-4FFDD1A0F65C}
DEFINE_PROPERTYKEY(PKEY_Microsoft_SDKSample_AreaSize, 0xce8b09dd, 0xb2d6, 0x4751, 0xa2, 0x7, 0x4f, 0xfd, 0xd1, 0xa0, 0xf6, 0x5c, 3);

// Col 3
// name="Microsoft.SDKSample.NumberOfSides"
// {DADD3288-0380-4dd3-A5C4-CD7AE2A099F0}
DEFINE_PROPERTYKEY(PKEY_Microsoft_SDKSample_NumberOfSides, 0xdadd3288, 0x380, 0x4dd3, 0xa5, 0xc4, 0xcd, 0x7a, 0xe2, 0xa0, 0x99, 0xf0, 3);

// Col 4
// name="Microsoft.SDKSample.DirectoryLevel"
// {581CF603-2925-4acf-BB5A-3D3EB39EACD3}
DEFINE_PROPERTYKEY(PKEY_Microsoft_SDKSample_DirectoryLevel, 0x581cf603, 0x2925, 0x4acf, 0xbb, 0x5a, 0x3d, 0x3e, 0xb3, 0x9e, 0xac, 0xd3, 3);

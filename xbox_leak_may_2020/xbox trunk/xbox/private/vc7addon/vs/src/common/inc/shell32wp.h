//---------------------------------------------------------------------------
//  Microsoft Confidential
//  Copyright (C) Microsoft Corporation, 2000 - 2001.  All Rights Reserved.
//
//  File:       shell32wp.h
//
//  Owner: Microsoft Visual Studio
//----------------------------------------------------------------------------

#ifndef _INC_SHELL32WP_H
#define _INC_SHELL32WP_H

#include "shlobj.h"

#define SHGetSettings OSHGetSettings

extern "C" {
void WINAPI OSHGetSettings(LPSHELLFLAGSTATE, DWORD);
}

#endif _INC_SHELL32WP_H
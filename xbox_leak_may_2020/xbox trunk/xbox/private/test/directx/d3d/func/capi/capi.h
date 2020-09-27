/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	capi.h

Abstract:

	capi utility interface - Created to test Xbox capi

Author:

	Jeff Sullivan (jeffsul) 26-Jun-2001

[Notes:]

	optional-notes

Revision History:

	26-Jun-2001 jeffsul
		Initial Version

--*/

#ifndef __CAPI_H__
#define __CAPI_H__

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include <xgmath.h>

typedef struct _CAPI 
{
	IDirect3DDevice8*          pDev;
	D3DPRESENT_PARAMETERS	   d3dpp;
	BOOL                       bQuit;
} CAPI;

extern CAPI capiTest;

#define CUSTOMVERTEX_FVF ( D3DFVF_XYZRHW|D3DFVF_DIFFUSE )

#define EXECUTECHECK( func )                                        \
{                                                                   \
	hr = func;                                                      \
	if ( FAILED( hr ) )												\
	{																\
		DbgPrint( #func ## " failed, returning: 0x%x\n", hr );		\
	    return hr;													\
	}                                                               \
}

#endif // #ifndef __CAPI_H__
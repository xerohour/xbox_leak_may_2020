/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    typetran.h

Author:

    Matt Bronder

Description:

    Win32 Direct3D8 data types.

*******************************************************************************/

#ifndef __TYPETRAN_H__
#define __TYPETRAN_H__

//******************************************************************************
typedef struct _WINX_D3DSURFACE_DESC {
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    UINT                Width;
    UINT                Height;
} WINX_D3DSURFACE_DESC;

//******************************************************************************
typedef struct _WINX_D3DVOLUME_DESC {
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;
    UINT                Width;
    UINT                Height;
    UINT                Depth;
} WINX_D3DVOLUME_DESC;

//******************************************************************************
typedef struct _WINX_D3DVERTEXBUFFER_DESC {
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;
    DWORD               FVF;
} WINX_D3DVERTEXBUFFER_DESC;

//******************************************************************************
typedef struct _WINX_D3DINDEXBUFFER_DESC {
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;
} WINX_D3DINDEXBUFFER_DESC;

#endif // __TYPETRAN_H__

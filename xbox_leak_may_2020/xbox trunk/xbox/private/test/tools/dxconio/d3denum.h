/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    d3denum.h

Description:

    Direct3D enumeration routines.

*******************************************************************************/

#ifndef __D3DENUM_H__
#define __D3DENUM_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

//******************************************************************************
// Macros
//******************************************************************************

#define ANTIALIAS_SUPPORTED(pdevd, pdm, bWindowed) \
    ((bWindowed && pdevd->d3dmstWindowed != D3DMULTISAMPLE_NONE) || \
    (!bWindowed && pdm->d3dmst != D3DMULTISAMPLE_NONE))

namespace DXCONIO {

//******************************************************************************
// Data types
//******************************************************************************

typedef BOOL (* CHECKDISPLAYPROC)(LPDIRECT3D8, D3DCAPS8*, D3DDISPLAYMODE*);

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Description of a display mode
typedef struct _DISPLAYMODE {
    D3DDISPLAYMODE              d3ddm;
    D3DFORMAT                   fmtd;
    D3DMULTISAMPLE_TYPE         d3dmst;
    struct _DISPLAYMODE*        pdmNext;
} DISPLAYMODE, *PDISPLAYMODE;

//******************************************************************************
// Description of a device
typedef struct _DEVICEDESC {
    D3DDEVTYPE                  d3ddt;
    D3DCAPS8                    d3dcaps;
    DWORD                       dwBehavior;
    BOOL                        bCanRenderWindowed;
    D3DFORMAT                   fmtdWindowed;
    D3DMULTISAMPLE_TYPE         d3dmstWindowed;
    PDISPLAYMODE                pdmList;
    struct _DEVICEDESC*         pdevdNext;
} DEVICEDESC, *PDEVICEDESC;

//******************************************************************************
// Description of an adapter
typedef struct _ADAPTERDESC {
    UINT                        uAdapter;
    D3DADAPTER_IDENTIFIER8      d3dai;
    D3DFORMAT                   fmtrWindowed;
    PDEVICEDESC                 pdevdList;
    struct _ADAPTERDESC*        padpdNext;
} ADAPTERDESC, *PADAPTERDESC;

//******************************************************************************
// Globals
//******************************************************************************

extern ADAPTERDESC              g_adpdDefault;
extern DEVICEDESC               g_devdDefault;
extern DISPLAYMODE              g_dmDefault;
extern D3DFORMAT                g_fmtdDefault;

//******************************************************************************
// Function prototypes
//******************************************************************************

PADAPTERDESC        EnumDisplays(LPDIRECT3D8 pd3d);

PADAPTERDESC        SelectAdapter(PADAPTERDESC padpdList);
PDEVICEDESC         SelectDevice(PADAPTERDESC padpd);
PDISPLAYMODE        SelectDisplayMode(PDEVICEDESC pdevd);

DWORD               FormatToBitDepth(D3DFORMAT d3dfmt);

void                InitCheckDisplayProc(CHECKDISPLAYPROC pfnValidate);

} // namespace DXCONIO

#endif //__D3DENUM_H__

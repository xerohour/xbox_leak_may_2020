#ifndef _NVD3DCAP_H_
#define _NVD3DCAP_H_
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVD3DCAP.H                                                        *
*   Define the capabilities of the NV D3D driver.                           *
*   This is mainly just a hardware selection include file.  The specific    *
*   hardware device capabilities are in the files:                          *
*   nv3dcaps.h, nv4dcaps.h, ...                                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - created.                     *
*                                                                           *
\***************************************************************************/
/*
 * Select the NV3 hardware capabilities.
 */
#ifdef  NV3
#include    "nv3dcaps.h"

#define NV_DEVDESC_FLAGS            NV3_DEVDESC_FLAGS
#define NV_COLORMODEL               NV3_COLORMODEL
#define NV_DEVCAPS_FLAGS            NV3_DEVCAPS_FLAGS
#define NV_TRANSFORMCAPS            NV3_TRANSFORMCAPS
#define NV_3DCLIPPINGCAPS           NV3_3DCLIPPINGCAPS
#define NV_LIGHTINGCAPS             NV3_LIGHTINGCAPS
#define NV_LINECAPS                 NV3_LINECAPS
#define NV_TRICAPS                  NV3_TRICAPS
#define NV_RENDERBITDEPTHCAPS       NV3_RENDERBITDEPTHCAPS
#define NV_ZBUFFERBITDEPTHCAPS      NV3_ZBUFFERBITDEPTHCAPS
#define NV_MAXBUFFERSIZE            NV3_MAXBUFFERSIZE
#define NV_MAXVERTEXCOUNT           NV3_MAXVERTEXCOUNT
#endif  // NV3

/*
 * Select the NV4 hardware capabilities.
 */
#ifdef  NV4
#include    "nv4dcaps.h"

#define NV_DEVDESC_FLAGS            NV4_DEVDESC_FLAGS
#define NV_COLORMODEL               NV4_COLORMODEL
#define NV_DEVCAPS_FLAGS            NV4_DEVCAPS_FLAGS
#define NV_TRANSFORMCAPS            NV4_TRANSFORMCAPS
#define NV_3DCLIPPINGCAPS           NV4_3DCLIPPINGCAPS
#define NV_LIGHTINGCAPS             NV4_LIGHTINGCAPS
#define NV_LINECAPS                 NV4_LINECAPS
#define NV_TRICAPS                  NV4_TRICAPS
#define NV_RENDERBITDEPTHCAPS       NV4_RENDERBITDEPTHCAPS
#define NV_ZBUFFERBITDEPTHCAPS      NV4_ZBUFFERBITDEPTHCAPS
#define NV_MAXBUFFERSIZE            NV4_MAXBUFFERSIZE
#define NV_MAXVERTEXCOUNT           NV4_MAXVERTEXCOUNT
#endif  // NV4
#endif  // _NVD3DCAP_H_
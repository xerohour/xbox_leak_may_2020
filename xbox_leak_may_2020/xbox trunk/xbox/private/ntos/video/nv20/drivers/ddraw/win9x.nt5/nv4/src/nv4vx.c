/*
 * **** MACHINE GENERATED - DO NOT EDIT ****
 * **** MACHINE GENERATED - DO NOT EDIT ****
 * **** MACHINE GENERATED - DO NOT EDIT ****
 *
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4VX.C                                                           *
*   NV4 Triangle Inner Loops. (Originally Machine Generated)                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 05/23/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3d.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"

#ifdef NV_FASTLOOPS

DWORD dwDrawPrimitiveTable[] =
{
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x040*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x080*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x0c0*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x100*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x140*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x180*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x1c0*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x200*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x240*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x280*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x2c0*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x300*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x340*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x380*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x3c0*/
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x400*/
   (DWORD)nvAACapturePrimitive,
   (DWORD)nvDX5TriangleSetup,
   (DWORD)nvAACapturePrimitive,
   (DWORD)nvDX6TriangleSetup,
};

#else //~NV_FASTLOOPS

/*
 * Function Tables
 */
LPNVDRAWPRIM fnDX5Table[] =
{
    nvDX5Triangle                           ,
    nvDX5TriangleExpFog                     ,
    nvDX5TriangleExp2Fog                    ,
    nvDX5TriangleLinearFog                  ,
    nvDX5TriangleWBuf                       ,
    nvDX5TriangleExpFogWBuf                 ,
    nvDX5TriangleExp2FogWBuf                ,
    nvDX5TriangleLinearFogWBuf              ,
    nvDX5TriangleAACapture                  ,
    nvDX5TriangleAACaptureWBuf              ,
    nvDX5TriangleSetup
};

LPNVINDEXEDPRIM fnDX5IndexedTable[] =
{
    nvDX5TriangleIndexed                    ,
    nvDX5TriangleExpFogIndexed              ,
    nvDX5TriangleExp2FogIndexed             ,
    nvDX5TriangleLinearFogIndexed           ,
    nvDX5TriangleIndexedWBuf                ,
    nvDX5TriangleExpFogIndexedWBuf          ,
    nvDX5TriangleExp2FogIndexedWBuf         ,
    nvDX5TriangleLinearFogIndexedWBuf       ,
    nvDX5TriangleIndexedAACapture           ,
    nvDX5TriangleIndexedAACaptureWBuf       ,
    nvDX5IndexedTriangleSetup
};

LPNVFVFDRAWPRIM fnDX5FlexTable[] =
{
    nvDX5FlexTriangle                       ,
    nvDX5FlexTriangleExpFog                 ,
    nvDX5FlexTriangleExp2Fog                ,
    nvDX5FlexTriangleLinearFog              ,
    nvDX5FlexTriangleWBuf                   ,
    nvDX5FlexTriangleExpFogWBuf             ,
    nvDX5FlexTriangleExp2FogWBuf            ,
    nvDX5FlexTriangleLinearFogWBuf          ,
    nvDX5FlexTriangleAACapture              ,
    nvDX5FlexTriangleAACaptureWBuf          ,
    nvDX5FlexTriangleSetup
};

LPNVFVFINDEXEDPRIM fnDX5FlexIndexedTable[] =
{
    nvDX5FlexTriangleIndexed                ,
    nvDX5FlexTriangleExpFogIndexed          ,
    nvDX5FlexTriangleExp2FogIndexed         ,
    nvDX5FlexTriangleLinearFogIndexed       ,
    nvDX5FlexTriangleIndexedWBuf            ,
    nvDX5FlexTriangleExpFogIndexedWBuf      ,
    nvDX5FlexTriangleExp2FogIndexedWBuf     ,
    nvDX5FlexTriangleLinearFogIndexedWBuf   ,
    nvDX5FlexTriangleIndexedAACapture       ,
    nvDX5FlexTriangleIndexedAACaptureWBuf   ,
    nvDX5FlexIndexedTriangleSetup
};

LPNVFVFDRAWPRIM fnDX6FlexTable[] =
{
    nvDX6FlexTriangle                       ,
    nvDX6FlexTriangleExpFog                 ,
    nvDX6FlexTriangleExp2Fog                ,
    nvDX6FlexTriangleLinearFog              ,
    nvDX6FlexTriangleWBuf                   ,
    nvDX6FlexTriangleExpFogWBuf             ,
    nvDX6FlexTriangleExp2FogWBuf            ,
    nvDX6FlexTriangleLinearFogWBuf          ,
    nvDX6FlexTriangleAACapture              ,
    nvDX6FlexTriangleAACaptureWBuf          ,
    nvDX6FlexTriangleSetup
};

LPNVFVFINDEXEDPRIM fnDX6FlexIndexedTable[] =
{
    nvDX6FlexTriangleIndexed                ,
    nvDX6FlexTriangleExpFogIndexed          ,
    nvDX6FlexTriangleExp2FogIndexed         ,
    nvDX6FlexTriangleLinearFogIndexed       ,
    nvDX6FlexTriangleIndexedWBuf            ,
    nvDX6FlexTriangleExpFogIndexedWBuf      ,
    nvDX6FlexTriangleExp2FogIndexedWBuf     ,
    nvDX6FlexTriangleLinearFogIndexedWBuf   ,
    nvDX6FlexTriangleIndexedAACapture       ,
    nvDX6FlexTriangleIndexedAACaptureWBuf   ,
    nvDX6FlexIndexedTriangleSetup
};


/*
 * Function declarations
 */

// nvDX5Triangle
#define  PROC_        "nvDX5Triangle                           "
#define  PROC         nvDX5Triangle
#define  VERTEX       nvglDX5TriangleVertex
#define  VXSIZE       sizeDX5TriangleVertex
#include "nv4loop.c"
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExpFog
#define  PROC_        "nvDX5TriangleExpFog                     "
#define  PROC         nvDX5TriangleExpFog
#define  VERTEX       nvglDX5TriangleVertexExpFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#include "nv4loop.c"
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExp2Fog
#define  PROC_        "nvDX5TriangleExp2Fog                    "
#define  PROC         nvDX5TriangleExp2Fog
#define  VERTEX       nvglDX5TriangleVertexExp2Fog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#include "nv4loop.c"
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleLinearFog
#define  PROC_        "nvDX5TriangleLinearFog                  "
#define  PROC         nvDX5TriangleLinearFog
#define  VERTEX       nvglDX5TriangleVertexLinearFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#include "nv4loop.c"
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleWBuf
#define  PROC_        "nvDX5TriangleWBuf                       "
#define  PROC         nvDX5TriangleWBuf
#define  VERTEX       nvglDX5TriangleVertexWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#include "nv4loop.c"
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExpFogWBuf
#define  PROC_        "nvDX5TriangleExpFogWBuf                 "
#define  PROC         nvDX5TriangleExpFogWBuf
#define  VERTEX       nvglDX5TriangleVertexExpFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  WBUFFER
#include "nv4loop.c"
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExp2FogWBuf
#define  PROC_        "nvDX5TriangleExp2FogWBuf                "
#define  PROC         nvDX5TriangleExp2FogWBuf
#define  VERTEX       nvglDX5TriangleVertexExp2FogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  WBUFFER
#include "nv4loop.c"
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleLinearFogWBuf
#define  PROC_        "nvDX5TriangleLinearFogWBuf              "
#define  PROC         nvDX5TriangleLinearFogWBuf
#define  VERTEX       nvglDX5TriangleVertexLinearFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  WBUFFER
#include "nv4loop.c"
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleIndexed
#define  PROC_        "nvDX5TriangleIndexed                    "
#define  PROC         nvDX5TriangleIndexed
#define  VERTEX       nvglDX5TriangleVertex
#define  VXSIZE       sizeDX5TriangleVertex
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExpFogIndexed
#define  PROC_        "nvDX5TriangleExpFogIndexed              "
#define  PROC         nvDX5TriangleExpFogIndexed
#define  VERTEX       nvglDX5TriangleVertexExpFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExp2FogIndexed
#define  PROC_        "nvDX5TriangleExp2FogIndexed             "
#define  PROC         nvDX5TriangleExp2FogIndexed
#define  VERTEX       nvglDX5TriangleVertexExp2Fog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleLinearFogIndexed
#define  PROC_        "nvDX5TriangleLinearFogIndexed           "
#define  PROC         nvDX5TriangleLinearFogIndexed
#define  VERTEX       nvglDX5TriangleVertexLinearFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleIndexedWBuf
#define  PROC_        "nvDX5TriangleIndexedWBuf                "
#define  PROC         nvDX5TriangleIndexedWBuf
#define  VERTEX       nvglDX5TriangleVertexWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExpFogIndexedWBuf
#define  PROC_        "nvDX5TriangleExpFogIndexedWBuf          "
#define  PROC         nvDX5TriangleExpFogIndexedWBuf
#define  VERTEX       nvglDX5TriangleVertexExpFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  WBUFFER
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleExp2FogIndexedWBuf
#define  PROC_        "nvDX5TriangleExp2FogIndexedWBuf         "
#define  PROC         nvDX5TriangleExp2FogIndexedWBuf
#define  VERTEX       nvglDX5TriangleVertexExp2FogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  WBUFFER
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleLinearFogIndexedWBuf
#define  PROC_        "nvDX5TriangleLinearFogIndexedWBuf       "
#define  PROC         nvDX5TriangleLinearFogIndexedWBuf
#define  VERTEX       nvglDX5TriangleVertexLinearFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  WBUFFER
#define  INDEXED
#include "nv4loop.c"
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleAACapture
#define  PROC_        "nvDX5TriangleAACapture                  "
#define  PROC         nvDX5TriangleAACapture
#define  VERTEX       nvglDX5TriangleVertexAACapture
#define  VXSIZE       sizeDX5TriangleVertex
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleAACaptureWBuf
#define  PROC_        "nvDX5TriangleAACaptureWBuf              "
#define  PROC         nvDX5TriangleAACaptureWBuf
#define  VERTEX       nvglDX5TriangleVertexAACaptureWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleIndexedAACapture
#define  PROC_        "nvDX5TriangleIndexedAACapture           "
#define  PROC         nvDX5TriangleIndexedAACapture
#define  VERTEX       nvglDX5TriangleVertexAACapture
#define  VXSIZE       sizeDX5TriangleVertex
#define  INDEXED
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   INDEXED
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5TriangleIndexedAACaptureWBuf
#define  PROC_        "nvDX5TriangleIndexedAACaptureWBuf       "
#define  PROC         nvDX5TriangleIndexedAACaptureWBuf
#define  VERTEX       nvglDX5TriangleVertexAACaptureWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  INDEXED
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangle
#define  PROC_        "nvDX5FlexTriangle                       "
#define  PROC         nvDX5FlexTriangle
#define  VERTEX       nvglDX5FlexTriangleVertex
#define  VXSIZE       sizeDX5TriangleVertex
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExpFog
#define  PROC_        "nvDX5FlexTriangleExpFog                 "
#define  PROC         nvDX5FlexTriangleExpFog
#define  VERTEX       nvglDX5FlexTriangleVertexExpFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExp2Fog
#define  PROC_        "nvDX5FlexTriangleExp2Fog                "
#define  PROC         nvDX5FlexTriangleExp2Fog
#define  VERTEX       nvglDX5FlexTriangleVertexExp2Fog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleLinearFog
#define  PROC_        "nvDX5FlexTriangleLinearFog              "
#define  PROC         nvDX5FlexTriangleLinearFog
#define  VERTEX       nvglDX5FlexTriangleVertexLinearFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleWBuf
#define  PROC_        "nvDX5FlexTriangleWBuf                   "
#define  PROC         nvDX5FlexTriangleWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExpFogWBuf
#define  PROC_        "nvDX5FlexTriangleExpFogWBuf             "
#define  PROC         nvDX5FlexTriangleExpFogWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexExpFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExp2FogWBuf
#define  PROC_        "nvDX5FlexTriangleExp2FogWBuf            "
#define  PROC         nvDX5FlexTriangleExp2FogWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexExp2FogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleLinearFogWBuf
#define  PROC_        "nvDX5FlexTriangleLinearFogWBuf          "
#define  PROC         nvDX5FlexTriangleLinearFogWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexLinearFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleIndexed
#define  PROC_        "nvDX5FlexTriangleIndexed                "
#define  PROC         nvDX5FlexTriangleIndexed
#define  VERTEX       nvglDX5FlexTriangleVertex
#define  VXSIZE       sizeDX5TriangleVertex
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExpFogIndexed
#define  PROC_        "nvDX5FlexTriangleExpFogIndexed          "
#define  PROC         nvDX5FlexTriangleExpFogIndexed
#define  VERTEX       nvglDX5FlexTriangleVertexExpFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExp2FogIndexed
#define  PROC_        "nvDX5FlexTriangleExp2FogIndexed         "
#define  PROC         nvDX5FlexTriangleExp2FogIndexed
#define  VERTEX       nvglDX5FlexTriangleVertexExp2Fog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleLinearFogIndexed
#define  PROC_        "nvDX5FlexTriangleLinearFogIndexed       "
#define  PROC         nvDX5FlexTriangleLinearFogIndexed
#define  VERTEX       nvglDX5FlexTriangleVertexLinearFog
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleIndexedWBuf
#define  PROC_        "nvDX5FlexTriangleIndexedWBuf            "
#define  PROC         nvDX5FlexTriangleIndexedWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExpFogIndexedWBuf
#define  PROC_        "nvDX5FlexTriangleExpFogIndexedWBuf      "
#define  PROC         nvDX5FlexTriangleExpFogIndexedWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexExpFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          1
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleExp2FogIndexedWBuf
#define  PROC_        "nvDX5FlexTriangleExp2FogIndexedWBuf     "
#define  PROC         nvDX5FlexTriangleExp2FogIndexedWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexExp2FogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          2
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleLinearFogIndexedWBuf
#define  PROC_        "nvDX5FlexTriangleLinearFogIndexedWBuf   "
#define  PROC         nvDX5FlexTriangleLinearFogIndexedWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexLinearFogWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  FOG          3
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleAACapture
#define  PROC_        "nvDX5FlexTriangleAACapture              "
#define  PROC         nvDX5FlexTriangleAACapture
#define  VERTEX       nvglDX5FlexTriangleVertexAACapture
#define  VXSIZE       sizeDX5TriangleVertex
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleAACaptureWBuf
#define  PROC_        "nvDX5FlexTriangleAACaptureWBuf          "
#define  PROC         nvDX5FlexTriangleAACaptureWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexAACaptureWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleIndexedAACapture
#define  PROC_        "nvDX5FlexTriangleIndexedAACapture       "
#define  PROC         nvDX5FlexTriangleIndexedAACapture
#define  VERTEX       nvglDX5FlexTriangleVertexAACapture
#define  VXSIZE       sizeDX5TriangleVertex
#define  INDEXED
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   INDEXED
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX5FlexTriangleIndexedAACaptureWBuf
#define  PROC_        "nvDX5FlexTriangleIndexedAACaptureWBuf   "
#define  PROC         nvDX5FlexTriangleIndexedAACaptureWBuf
#define  VERTEX       nvglDX5FlexTriangleVertexAACaptureWBuf
#define  VXSIZE       sizeDX5TriangleVertex
#define  WBUFFER
#define  INDEXED
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangle
#define  PROC_        "nvDX6FlexTriangle                       "
#define  PROC         nvDX6FlexTriangle
#define  VERTEX       nvglDX6FlexTriangleVertex
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExpFog
#define  PROC_        "nvDX6FlexTriangleExpFog                 "
#define  PROC         nvDX6FlexTriangleExpFog
#define  VERTEX       nvglDX6FlexTriangleVertexExpFog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          1
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExp2Fog
#define  PROC_        "nvDX6FlexTriangleExp2Fog                "
#define  PROC         nvDX6FlexTriangleExp2Fog
#define  VERTEX       nvglDX6FlexTriangleVertexExp2Fog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          2
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleLinearFog
#define  PROC_        "nvDX6FlexTriangleLinearFog              "
#define  PROC         nvDX6FlexTriangleLinearFog
#define  VERTEX       nvglDX6FlexTriangleVertexLinearFog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          3
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleWBuf
#define  PROC_        "nvDX6FlexTriangleWBuf                   "
#define  PROC         nvDX6FlexTriangleWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExpFogWBuf
#define  PROC_        "nvDX6FlexTriangleExpFogWBuf             "
#define  PROC         nvDX6FlexTriangleExpFogWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexExpFogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          1
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExp2FogWBuf
#define  PROC_        "nvDX6FlexTriangleExp2FogWBuf            "
#define  PROC         nvDX6FlexTriangleExp2FogWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexExp2FogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          2
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleLinearFogWBuf
#define  PROC_        "nvDX6FlexTriangleLinearFogWBuf          "
#define  PROC         nvDX6FlexTriangleLinearFogWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexLinearFogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          3
#define  WBUFFER
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleIndexed
#define  PROC_        "nvDX6FlexTriangleIndexed                "
#define  PROC         nvDX6FlexTriangleIndexed
#define  VERTEX       nvglDX6FlexTriangleVertex
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExpFogIndexed
#define  PROC_        "nvDX6FlexTriangleExpFogIndexed          "
#define  PROC         nvDX6FlexTriangleExpFogIndexed
#define  VERTEX       nvglDX6FlexTriangleVertexExpFog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          1
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExp2FogIndexed
#define  PROC_        "nvDX6FlexTriangleExp2FogIndexed         "
#define  PROC         nvDX6FlexTriangleExp2FogIndexed
#define  VERTEX       nvglDX6FlexTriangleVertexExp2Fog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          2
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleLinearFogIndexed
#define  PROC_        "nvDX6FlexTriangleLinearFogIndexed       "
#define  PROC         nvDX6FlexTriangleLinearFogIndexed
#define  VERTEX       nvglDX6FlexTriangleVertexLinearFog
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          3
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleIndexedWBuf
#define  PROC_        "nvDX6FlexTriangleIndexedWBuf            "
#define  PROC         nvDX6FlexTriangleIndexedWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExpFogIndexedWBuf
#define  PROC_        "nvDX6FlexTriangleExpFogIndexedWBuf      "
#define  PROC         nvDX6FlexTriangleExpFogIndexedWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexExpFogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          1
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleExp2FogIndexedWBuf
#define  PROC_        "nvDX6FlexTriangleExp2FogIndexedWBuf     "
#define  PROC         nvDX6FlexTriangleExp2FogIndexedWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexExp2FogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          2
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleLinearFogIndexedWBuf
#define  PROC_        "nvDX6FlexTriangleLinearFogIndexedWBuf   "
#define  PROC         nvDX6FlexTriangleLinearFogIndexedWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexLinearFogWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FOG          3
#define  WBUFFER
#define  INDEXED
#define  FVF
#include "nv4loop.c"
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   FOG
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleAACapture
#define  PROC_        "nvDX6FlexTriangleAACapture              "
#define  PROC         nvDX6FlexTriangleAACapture
#define  VERTEX       nvglDX6FlexTriangleVertexAACapture
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleAACaptureWBuf
#define  PROC_        "nvDX6FlexTriangleAACaptureWBuf          "
#define  PROC         nvDX6FlexTriangleAACaptureWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexAACaptureWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  WBUFFER
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   WBUFFER
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleIndexedAACapture
#define  PROC_        "nvDX6FlexTriangleIndexedAACapture       "
#define  PROC         nvDX6FlexTriangleIndexedAACapture
#define  VERTEX       nvglDX6FlexTriangleVertexAACapture
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  INDEXED
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   INDEXED
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

// nvDX6FlexTriangleIndexedAACaptureWBuf
#define  PROC_        "nvDX6FlexTriangleIndexedAACaptureWBuf   "
#define  PROC         nvDX6FlexTriangleIndexedAACaptureWBuf
#define  VERTEX       nvglDX6FlexTriangleVertexAACaptureWBuf
#define  DX6
#define  VXSIZE       sizeDX6TriangleVertex
#define  WBUFFER
#define  INDEXED
#define  FVF
#define  ANTIALIAS
#include "nv4loop.c"
#undef   ANTIALIAS
#undef   FVF
#undef   INDEXED
#undef   WBUFFER
#undef   VXSIZE
#undef   DX6
#undef   VERTEX
#undef   PROC
#undef   PROC_

#endif //!NV_FASTLOOPS

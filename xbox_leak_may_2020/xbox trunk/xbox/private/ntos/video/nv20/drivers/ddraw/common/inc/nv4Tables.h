#ifndef _NV4TABLES_H
#define _NV4TABLES_H

/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nv4tables.h                                                      *
*     Constants and macros associated with nv4tables.cpp                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)    28Oct99     created                *
*                                                                           *
\***************************************************************************/

#if (NVARCH >= 0x004)

#define DRAW_PRIM_TABLE_ENTRIES      0x800

extern DWORD dwDrawPrimitiveTable [DRAW_PRIM_TABLE_ENTRIES+4];

#define LIST_STRIDES   0x00000303
#define STRIP_STRIDES  0x01010101
#define FAN_STRIDES    0x02000001
#define LEGACY_STRIDES 0x03000404

extern DWORD nv4DP2OpToStride [D3D_DP2OP_MAX+1];

#endif  // NVARCH >= 0x010

#endif  // _NV4TABLES_H




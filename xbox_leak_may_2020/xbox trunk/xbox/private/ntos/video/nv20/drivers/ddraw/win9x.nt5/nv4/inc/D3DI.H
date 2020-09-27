/*==========================================================================;
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3di.h
 *  Content:    Direct3D internal include file
 *
 ***************************************************************************/

#ifndef _D3DI_H
#define _D3DI_H

#include "ddraw.h"
#include "d3d.h"


typedef DWORD D3DI_BUFFERHANDLE, *LPD3DI_BUFFERHANDLE;

/*
 * Internal version of executedata
 */
typedef struct _D3DI_ExecuteData {
    DWORD       dwSize;
    D3DI_BUFFERHANDLE dwHandle;         /* Handle allocated by driver */
    DWORD       dwVertexOffset;
    DWORD       dwVertexCount;
    DWORD       dwInstructionOffset;
    DWORD       dwInstructionLength;
    DWORD       dwHVertexOffset;
    D3DSTATUS   dsStatus;               /* Status after execute */
} D3DI_EXECUTEDATA, *LPD3DI_EXECUTEDATA;

/*
 * Internal version of lightdata and constants for flags
 */

#define D3DLIGHTI_ATT0_IS_NONZERO       (0x00010000)    
#define D3DLIGHTI_ATT1_IS_NONZERO       (0x00020000)
#define D3DLIGHTI_ATT2_IS_NONZERO       (0x00040000)
#define D3DLIGHTI_LINEAR_FALLOFF        (0x00080000)
#define D3DLIGHTI_UNIT_SCALE            (0x00100000)
#define D3DLIGHTI_LIGHT_AT_EYE          (0x00200000)

typedef struct _D3DI_LIGHT {
    D3DLIGHTTYPE        type;
        DWORD                   version;        /* matches number on D3DLIGHT struct */
    BOOL                        valid;
    D3DVALUE            red, green, blue, shade;
    D3DVECTOR           position;
    D3DVECTOR           model_position;
    D3DVECTOR           direction;
    D3DVECTOR           model_direction;
    D3DVECTOR           halfway;
        D3DVECTOR               model_eye;              /* direction from eye in model space */
        D3DVECTOR               model_scale;    /* model scale for proper range computations */
    D3DVALUE            range;
    D3DVALUE            range_squared;
    D3DVALUE            falloff;
    D3DVALUE            attenuation0;
    D3DVALUE            attenuation1;
    D3DVALUE            attenuation2;
    D3DVALUE            cos_theta_by_2;
    D3DVALUE            cos_phi_by_2;
        DWORD                   flags;
} D3DI_LIGHT, *LPD3DI_LIGHT;


#endif /* _D3DI_H */

#ifndef _NVCMRSVD_H_
#define _NVCMRSVD_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NVTYPES_INCLUDED
#include "nvtypes.h"
#endif

//------------------------------------------------------------------------------
// Configuration manager reserved properties.
//
// #define NV_CFGEX_RESERVED 150
//------------------------------------------------------------------------------

typedef struct
{
    U032   Property;
    U032 * In;
    U032 * Out;
} NV_CFGEX_RESERVED_PROPERTY;

enum
{
    // Register read and write.
     PROPERTY_REG_RD08              // In:[Offset]       Out:[Data]
    ,PROPERTY_REG_RD16              // In:[Offset]       Out:[Data]
    ,PROPERTY_REG_RD32              // In:[Offset]       Out:[Data]
    ,PROPERTY_REG_WR08              // In:[Offset, Data] Out:[]
    ,PROPERTY_REG_WR16              // In:[Offset, Data] Out:[]
    ,PROPERTY_REG_WR32              // In:[Offset, Data] Out:[]

    // Frame buffer read and write.
    ,PROPERTY_FB_RD08               // In:[Offset]       Out:[Data]
    ,PROPERTY_FB_RD16               // In:[Offset]       Out:[Data]
    ,PROPERTY_FB_RD32               // In:[Offset]       Out:[Data]
    ,PROPERTY_FB_WR08               // In:[Offset, Data] Out:[]
    ,PROPERTY_FB_WR16               // In:[Offset, Data] Out:[]
    ,PROPERTY_FB_WR32               // In:[Offset, Data] Out:[]

    // PCI read and write.
    ,PROPERTY_PCI_RD08              // In:[Bus, Device, Function, Offset]       Out:[Data]
    ,PROPERTY_PCI_RD16              // In:[Bus, Device, Function, Offset]       Out:[Data]
    ,PROPERTY_PCI_RD32              // In:[Bus, Device, Function, Offset]       Out:[Data]
    ,PROPERTY_PCI_WR08              // In:[Bus, Device, Function, Offset, Data] Out:[]
    ,PROPERTY_PCI_WR16              // In:[Bus, Device, Function, Offset, Data] Out:[]
    ,PROPERTY_PCI_WR32              // In:[Bus, Device, Function, Offset, Data] Out:[]

    // Set clocks.
    ,PROPERTY_SET_GRAPHICS_CLOCK    // In:[Frequency (Hz)]       Out:[]
    ,PROPERTY_SET_MEMORY_CLOCK      // In:[Frequency (Hz)]       Out:[]
    ,PROPERTY_SET_PIXEL_CLOCK       // In:[Head, Frequency (Hz)] Out:[]
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVCMRSVD_H_

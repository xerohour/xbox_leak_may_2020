#ifndef _MC_H_
#define _MC_H_
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
/****************************** Master Control *****************************\
*                                                                           *
* Module: MC.H                                                              *
*       Master Exception dispatcher.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Macros.
//
//---------------------------------------------------------------------------

//
// Since the hardware ref manuals don't give us macros to decode
// the PMC_BOOT_0 register, we provide a few here.
//
#define MC_GET_MASKREVISION(b0)         (b0 & 0x000000FF)
#define MC_GET_REVISION(b0)             ((b0 & 0x000F0000) >> 16)
#define MC_GET_IMPLEMENTATION(b0)       ((b0 & 0x00F00000) >> 20)
#define MC_GET_ARCHITECTURE(b0)         ((b0 & 0xFF000000) >> 20)

//
// Architecture constants.
//
#define MC_ARCHITECTURE_NV04            0x04
#define MC_ARCHITECTURE_NV10            0x10
#define MC_ARCHITECTURE_NV20            0x20

//
// Implementation constants.
// These must be unique within a single architecture.
//
#define MC_IMPLEMENTATION_NV04          0x00
#define MC_IMPLEMENTATION_NV05          0x01
#define MC_IMPLEMENTATION_NV0A          0x02
#define MC_IMPLEMENTATION_NV10          0x00
#define MC_IMPLEMENTATION_NV11          0x01
#define MC_IMPLEMENTATION_NV15          0x05
#define MC_IMPLEMENTATION_NV20          0x00

//
// MaskRevision constants.
//
#define MC_MASK_REVISION_A1             0xA1
#define MC_MASK_REVISION_A2             0xA2
#define MC_MASK_REVISION_A3             0xA3
#define MC_MASK_REVISION_A6             0xA6
#define MC_MASK_REVISION_B1             0xB1
#define MC_MASK_REVISION_B2             0xB2

//
// Revision constants.
// 
#define MC_REVISION_0                   0x00
#define MC_REVISION_1                   0x01
#define MC_REVISION_2                   0x02
#define MC_REVISION_3                   0x03

// Return from D3 ACPI state requires a delay before strap register is stable
#define D0_DELAY_RETRIES    10

//---------------------------------------------------------------------------
//
//  Chip ID objects.
//
//---------------------------------------------------------------------------

typedef struct _def_chip_id_object
{
    OBJECT     Base;
    PDMAOBJECT ChipTokenXlate;
} CHIPIDOBJECT, *PCHIPIDOBJECT;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS initMc(PHWINFO);
VOID      mcService(PHWINFO);
RM_STATUS mcPowerStateTrigger(PHWINFO);
RM_STATUS chpCreateChipID(PCLASSOBJECT, U032, POBJECT *);
RM_STATUS chpDeleteChipID(POBJECT);
RM_STATUS mcPowerState(PHWINFO, U032);
BOOL IsNV4(PHWINFO);
BOOL IsNV5(PHWINFO);
BOOL IsNV0A(PHWINFO);
BOOL IsNV10(PHWINFO);
BOOL IsNV11(PHWINFO);
BOOL IsNV15(PHWINFO);
BOOL IsNV20(PHWINFO);

BOOL IsNV5orBetter(PHWINFO);
BOOL IsNV10orBetter(PHWINFO);
BOOL IsNV10MaskRevA03orBetter(PHWINFO);
BOOL IsNV15orBetter(PHWINFO);
BOOL IsNV15MaskRevA01(PHWINFO);
BOOL IsNV15MaskRevA02(PHWINFO);
BOOL IsNV15MaskRevA03(PHWINFO);
BOOL IsNV11orBetter(PHWINFO);
BOOL IsNV20orBetter(PHWINFO);

RM_STATUS mcSetBiosRevision(PHWINFO);

#endif // _MC_H_

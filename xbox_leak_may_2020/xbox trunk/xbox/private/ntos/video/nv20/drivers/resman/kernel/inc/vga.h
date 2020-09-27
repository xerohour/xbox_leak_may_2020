#ifndef _NVVGA_H_
#define _NVVGA_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: VGA.H                                                             *
*       Resource Manager VGA defines and structures used throughout.        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//
// CRTC Access Macros
//
// Access CRTC indexed pairs
#define CRTC_WRITE(i,d)   {REG_WR08(NV_PRMCIO_CRX__COLOR, (i) & 0x7F);\
                        REG_WR08(NV_PRMCIO_CR__COLOR, (d));}
#define CRTC_READ(i,d)   {REG_WR08(NV_PRMCIO_CRX__COLOR, (i) & 0x7F);\
                        (d) = REG_RD08(NV_PRMCIO_CR__COLOR);}

// For dual head devices, CRTC2 is accessed at CRTC1 address + 0x2000
// i.e. CRTC1 = 0x6013D4, CRTC2 = 0x6033D4
// These macros can replace CRTC_RD/CRTC_WR.
#define CRTC_WR(i,d,h)  {REG_WR08(NV_PRMCIO_CRX__COLOR + CurDacAdr(h), i);\
                        REG_WR08(NV_PRMCIO_CR__COLOR + CurDacAdr(h), d);}

#define CRTC_RD(i,d,h)  {REG_WR08(NV_PRMCIO_CRX__COLOR + CurDacAdr(h), i);\
                        (d) = REG_RD08(NV_PRMCIO_CR__COLOR + CurDacAdr(h));}

// We need special versions of these macros for HAL use...
#define HAL_CRTC_WR(i,d,a)  {REG_WR08(NV_PRMCIO_CRX__COLOR + a, i);\
                             REG_WR08(NV_PRMCIO_CR__COLOR + a, d);}

#define HAL_CRTC_RD(i,d,a)  {REG_WR08(NV_PRMCIO_CRX__COLOR + a, i);\
                             (d) = REG_RD08(NV_PRMCIO_CR__COLOR + a);}

// Access Attribute Controller indexed register pair 
#define ATR_WR(i,d,h)    {REG_WR08(NV_PRMCIO_ARX + CurDacAdr(h), i);\
                        REG_WR08(NV_PRMCIO_AR_PALETTE__WRITE + CurDacAdr(h), d);}

#define ATR_RD(i,d,h)    {REG_WR08(NV_PRMCIO_ARX + CurDacAdr, (i) & 0x7F);\
                        (d) = REG_RD08(NV_PRMCIO_AR__PALETTE_READ + CurDacAdr(h));}

// Mono CRTC Access
#define CRTC_WRM(i,d)   {REG_WR08(NV_PRMCIO_CRX__MONO, (i) & 0x7F);\
                        REG_WR08(NV_PRMCIO_CR__MONO, (d));}
#define CRTC_RDM(i,d)   {REG_WR08(NV_PRMCIO_CRX__MONO, (i) & 0x7F);\
                        (d) = REG_RD08(NV_PRMCIO_CR__MONO);}

#endif // _NVVGA_H_

// Bit definitions
#define BITS0_7   0xff
#define BITS0_4   0x1f
#define BITS0_3   0x0f
#define BIT0      0x01
#define BIT1      0x02
#define BIT2      0x04
#define BIT3      0x08
#define BIT4      0x10
#define BIT5      0x20
#define BIT6      0x40
#define BIT7      0x80
#define BIT8      0x100
#define BIT9      0x200
#define BIT10     0x400
#define BIT11     0x800
#define BIT12     0x1000
#define BIT13     0x2000
#define BIT14     0x4000
#define BIT15     0x8000


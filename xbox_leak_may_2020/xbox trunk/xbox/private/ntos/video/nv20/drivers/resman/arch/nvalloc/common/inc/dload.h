/*****************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2001 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2001  NVIDIA, Corp.    NVIDIA  design  patents     *|
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
|*****************************************************************************|
|*                                                                           *|
|* Module: dload.c                                                           *|
|*    These are the OS-specific bits of implementing dynamic loading of      *|
|* driver code.  Be that from file, ROM/BIOS, etc.                           *|
|*                                                                           *|
|*****************************************************************************|
|*                                                                           *|
|* History:                                                                  *|
|*       Ken Adams (kadams)     02/2001 - wrote it based upon investigatory  *|
|*                                        code from Todd Poynter.            *|
|*                                                                           *|
\*****************************************************************************/
#ifndef _DLOAD_H
#define _DLOAD_H


RM_STATUS initDload     ( PHWINFO);
RM_STATUS DldLoadModule ( PHWINFO, U008 *rawImage );


NV_UINTPTR_T DldFindMemOffset       (PDLD_MODULE, char *); 
NV_UINTPTR_T DldGetImageOffset      (PDLD_MODULE, unsigned);
NvU16        DldGetSymbolSection    (PDLD_MODULE, unsigned symIndex);
NV_UINTPTR_T DldGetSectionMemOffset (PDLD_MODULE, unsigned secNumber);
NV_UINTPTR_T DldGetMemOffset        (PDLD_MODULE, unsigned symIndex);
void         DldPrintImgInfo        (PDLD_MODULE);
void         DldPrintSectionInfo    (PDLD_MODULE);
void         DldPrintRelocationInfo (PDLD_MODULE);
void         DldPrintSymbolTableInfo(PDLD_MODULE);

RM_STATUS    DldInflate             (NvU8 *in, NvU8* out);
#endif

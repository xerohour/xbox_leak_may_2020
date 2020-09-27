#ifndef _FB_H_
#define _FB_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/**************************** Frambuffer Module ******** *******************\
*                                                                           *
* Module: FB.H                                                              *
*       Frambuffer routines for allocation and deallocation of off-screen   *
* memory.                                                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    10/29/94 - rewrote it.                  *
*                                                                           *
\***************************************************************************/

//
// Amount of instance space to set aside for a BIOS image
//
#define BIOS_IMAGE_PAD  0x10000      // 64k

//---------------------------------------------------------------------------
//
//  Function prototypes for Frambuffer.
//
//---------------------------------------------------------------------------

RM_STATUS initFb(PHWINFO);
RM_STATUS fbInitializeInstMemBitMap(PHWINFO);
RM_STATUS fbAllocInstMem(PHWINFO, U032 *, U032);
RM_STATUS fbFreeInstMem(PHWINFO, U032, U032);
RM_STATUS fbAllocInstMemAlign(PHWINFO, U032 *, U032, U032);
RM_STATUS fbFreeInstMemAlign(U032, U032, U032);
V032      fbService(PHWINFO);

RM_STATUS fbUpdateStartAddress(PHWINFO, U032);
RM_STATUS fbGetScanline(U032 *);
RM_STATUS fbCheckConfig(PHWINFO);

#endif // _FB_H_

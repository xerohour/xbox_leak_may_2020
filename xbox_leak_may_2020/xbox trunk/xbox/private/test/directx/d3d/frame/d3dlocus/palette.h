/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    palette.h

Description:

    Direct3D palette routines.

*******************************************************************************/

#ifndef __PALETTE_H__
#define __PALETTE_H__

#ifdef UNDER_XBOX

//******************************************************************************
// Function prototypes
//******************************************************************************

CPalette8*              CreatePalette(CDevice8* pDevice, D3DCOLOR* pcColors = NULL, 
                                      D3DPALETTESIZE palsSize = D3DPALETTE_256);
void                    ReleasePalette(CPalette8* pd3dp);
void                    ReleasePalettes();
void                    RemovePalette(CPalette8* pd3dp);

#endif // UNDER_XBOX

#endif // __PALETTE_H__

/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fontmap.h

Author:

    Matt Bronder

Description:

    Functions for creating and indexing into a font texture.

*******************************************************************************/

#ifndef __FONTMAP_H__
#define __FONTMAP_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define FONT_TCOORD_WIDTH   0.0625f
#define FONT_TCOORD_HEIGHT  0.1171875f

//******************************************************************************
// Macros
//******************************************************************************

#define CHAR_TO_TCOORD(c)  (&g_ptcFont[(c < ' ' || c > '~') ? (' ') : (c - ' ')])

//******************************************************************************
// Globals
//******************************************************************************

extern LPDIRECT3DTEXTURE8   g_pd3dtFontMap;
extern TCOORD2              g_ptcFont[96];

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                        CreateFontMap(LPDIRECT3DDEVICE8 pDevice, D3DFORMAT fmt = D3DFMT_A1R5G5B5);
void                        ReleaseFontMap();

#endif //__FONTMAP_H__

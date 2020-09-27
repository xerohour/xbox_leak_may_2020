/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fontmap.h

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

#define CHAR_TO_TCOORD(c)  (&g_ptcFont[(c < ' ' || c > '~') ? (' ') : (c - ' ')])

namespace DXCONIO {

//******************************************************************************
// Globals
//******************************************************************************

extern LPDIRECT3DTEXTURE8   g_pd3dtFontMap;
extern TCOORD               g_ptcFont[96];

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                        CreateFontMap(D3DFORMAT fmt = D3DFMT_A1R5G5B5);
void                        ReleaseFontMap();

} // namespace DXCONIO

#endif //__FONTMAP_H__

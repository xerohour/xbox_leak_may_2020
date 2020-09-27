//-----------------------------------------------------------------------------
// File: CXFont.h
//
// Desc: Wrapper for the XFont class to get some additional functionality
//       
//
// Hist: 12.03.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <xtl.h>
#include <xfont.h>

//-----------------------------------------------------------------------------
#define CXFONT_LEFT     0x00000000
#define CXFONT_RIGHT    0x00000001
#define CXFONT_CENTER_X 0x00000002
#define CXFONT_CENTER_Y 0x00000004


//-----------------------------------------------------------------------------
class CXFont
{
	XFONT*			    m_pXFont;
    LPDIRECT3DSURFACE8  m_pBackBuffer;

public:

	CXFont();

	~CXFont();

    // Functions to create and destroy the internal objects
    HRESULT Create( const DWORD dwCacheSize, 
                    const WCHAR* strFontResourceFileName,
                    const LPDIRECT3DSURFACE8 pBackBuffer);

    HRESULT Destroy();

    // Returns the dimensions of a text string
    HRESULT GetTextExtent( const WCHAR* strText, LONG* pWidth,
                           LONG* pHeight, BOOL bFirstLineOnly = FALSE ) const;

	// Draw string to screen location specified.
    HRESULT DrawText( LONG fOriginX, LONG fOriginY,  DWORD dwColor, 
                      const WCHAR* wstrText, DWORD dwFlags=0L );
};

//-----------------------------------------------------------------------------




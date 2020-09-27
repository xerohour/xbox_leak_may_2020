//-----------------------------------------------------------------------------
// File: CXFont.cpp
//
// Desc: Wrapper for the XFont class to get some additional functionality
//       
//
// Hist: 12.03.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xbapp.h"
#include "cxfont.h"

//-----------------------------------------------------------------------------
// Name: CXFont
// Desc: Constructor
CXFont::CXFont()
{
	m_pXFont = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CXFont
// Desc: Destructor
CXFont::~CXFont()
{
	Destroy();
}




//-----------------------------------------------------------------------------
// Name: Create
// Desc: Function to create the internal objects
HRESULT CXFont::Create( const DWORD dwCacheSize,
						const WCHAR* wstrFontResourceFileName,
                        const LPDIRECT3DSURFACE8 pBackBuffer )
{
    if( FAILED( XFONT_OpenBitmapFont( wstrFontResourceFileName,
                                      dwCacheSize,&m_pXFont ) ) )
    {
        return XBAPPERR_MEDIANOTFOUND;
    }

    m_pBackBuffer = pBackBuffer;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy
// Desc: Function to destroy the internal objects
HRESULT CXFont::Destroy()
{
	m_pXFont->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent
// Desc: Returns the dimensions of a text string
HRESULT CXFont::GetTextExtent( const WCHAR* strText, LONG* pWidth, 
							   LONG* pHeight, BOOL bFirstLineOnly) const
{
    // Check parameters
    if( NULL==strText || NULL==pWidth || NULL==pHeight )
        return E_INVALIDARG;

    // Set default text extent in output parameters
    (*pWidth)   = 0;
    (*pHeight)  = 0;

    // Initialize counters that keep track of text extent
    LONG sx = 0;
    LONG sy = m_pXFont->GetTextHeight() + 1;

    // Loop through each character and update text extent
    while( *strText )
    {
        WCHAR letter = *strText++;
        
        // Handle newline character
        if( letter == L'\n' )
        {
            if( bFirstLineOnly )
                return S_OK;

            sx  = 0;
            sy += m_pXFont->GetTextHeight() + 1;
        }

        // Get text extent for this character's glyph
        unsigned localWidth = 0;
        m_pXFont->GetTextExtent(strText,1,&localWidth);
        sx += localWidth;

        // Store text extent of string in output parameters
        if( sx > (*pWidth) )   (*pWidth)  = sx;
        if( sy > (*pHeight) )  (*pHeight) = sy;
     }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawText
// Desc: Draw string to screen location specified.
HRESULT CXFont::DrawText( LONG fOriginX, LONG fOriginY, DWORD dwColor,
                          const WCHAR* wstrText, DWORD dwFlags )
{
    // Set the starting screen position
    LONG sx = fOriginX;
    LONG sy = fOriginY;

    // Set the color of the text.
    D3DCOLOR textColor(dwColor);
    m_pXFont->SetTextColor(textColor);

    // If vertically centered, offset the starting sy value
    if( dwFlags & CXFONT_CENTER_Y )
    {
        LONG w, h;
        GetTextExtent( wstrText, &w, &h );
        sy = sy - h/2;
    }

    // Set a flag so we can determine initial justification effects
    BOOL bStartingNewLine = TRUE;

    while( *wstrText )
    {
        // If starting text on a new line, determine justification effects
        if( bStartingNewLine )
        {
            if( dwFlags & (CXFONT_RIGHT|CXFONT_CENTER_X) )
            {
                // Get the extent of this line
                LONG w, h;
                GetTextExtent( wstrText, &w, &h, TRUE );

                // Offset this line's starting sx value
                if( dwFlags & CXFONT_RIGHT )
                    sx = fOriginX - w;
                if( dwFlags & CXFONT_CENTER_X )
                    sx = fOriginX - w/2;
            }
            bStartingNewLine = FALSE;
        }

        // Count Characters to the newline character
        WCHAR *wstrNextText = const_cast<WCHAR *> (wstrText);
        while ( *wstrNextText && ( (*wstrNextText) != L'\n') )
            ++wstrNextText;

        DWORD dwLocalStringLength = wstrNextText - wstrText;

        //Output the text to the screen.
        m_pXFont->TextOut( m_pBackBuffer, wstrText, dwLocalStringLength, sx, sy );

        // Handle the newline character
        if( ( (*wstrNextText) == L'\n' ) )
        {
            ++wstrNextText;
            sx  = fOriginX;
            sy += m_pXFont->GetTextHeight() + 1;
            bStartingNewLine = TRUE;
        }

        wstrText = wstrNextText;
    }

    return S_OK;
}


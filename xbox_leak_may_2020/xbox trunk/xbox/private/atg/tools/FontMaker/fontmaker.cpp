//-----------------------------------------------------------------------------
// File: FontMaker.cpp
//
// Desc: Creates an Xbox font file (.abc) for the specified font. This app
//       overides the common font selection dialog. After the user selects a
//       font and a glyph range, this app creates a bitmap containing all
//       glyphs in the selected range. Finally, the bitmap is saved as a .tga
//       texture file, and the glyph spacing info is saved in a .abc binary
//       file. Both files (the .tga and the .abe) are used by Xbox XDK samples
//       to render texture-based text. Additionally, please note that the .tga
//       file can be modified with image-editing software to add colors,
//       drop shadows and other effects.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       03.07.01 - Added glyph range selection for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>   // To get to the common font dialog
#include "resource.h"  // To override the common font dialog

// Note: In case you plan to modify the dialog in this app's .rc file, keep in
// mind that the common font dialog uses pre-determined constants for it's 
// controls.




//-----------------------------------------------------------------------------
// Name: struct GLYPH_ATTR
// Desc: A structure to hold attributes for one glpyh. The left, right, etc.
//       values are texture coordinate offsets into the resulting texture image
//       (which ends up in the .tga file). The offset, width, etc. values are
//       spacing information, used when rendering the font.
//-----------------------------------------------------------------------------
struct GLYPH_ATTR
{
    FLOAT left, top, right, bottom;
    SHORT wOffset;
    SHORT wWidth;
    SHORT wAdvance;
};




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
TCHAR        g_strFontName[512];         // Font properties
DWORD        g_dwFontHeight;
DWORD        g_dwFontEffectiveHeight;
DWORD        g_dwFontWeight = 400;       // Default value of 400 = normal weight
BOOL         g_bItalicized  = 0;         // Default value of FALSE = not-italicized

WCHAR        g_dwStartGlyph = 0;         // Range of glyphs to render
WCHAR        g_dwEndGlyph   = 0;
DWORD        g_dwNumGlyphs  = 0;

DWORD        g_dwVersion   = 0x00000004; // Texture file properties
DWORD        g_dwTexWidth  = 64;         // Default texture dimensions
DWORD        g_dwTexHeight = 64;       
DWORD        g_dwTexBPP    = 16;




//-----------------------------------------------------------------------------
// Name: CalcTextureSize()
// Desc: Calculates how big a texture will need to be to hold the glyphs
//       for a given font. Before calling this function, the caller needs to
//       load the font into the DC passed as input here.
//-----------------------------------------------------------------------------
HRESULT CalcTextureSize( HDC hFontDC, WCHAR dwStartGlyph, WCHAR dwEndGlyph, 
                         DWORD* pdwTexWidth, DWORD* pdwTexHeight )
{
    DWORD dwTexWidth  = 64; // Initial (minimum) texture dimensions
    DWORD dwTexHeight = 64;
    DWORD x           =  0; // Initial cursor position is (0,0)
    DWORD y           =  0; 
    
    // Loop through the glyph range to calculate how big the texture should be
    for( WCHAR c = dwStartGlyph; c && c <= dwEndGlyph; c++ )
    {
        WCHAR str[2] = L"x";
        SIZE  size;
        ABC   abc;

        // Get the size needed to render the glyph
        str[0] = c;
        GetTextExtentPoint32W( hFontDC, str, 1, &size );

        // Check if the glpyh would exceed the right edge of the texture
        if( (x+2*size.cx+1) > dwTexWidth )
        {
            // If so, advance the cursor down a line
            x  = 0;
            y += size.cy+1;

            // Now check if we go past the bottom edge of the texture
            if( (y+size.cy+1) > dwTexHeight )
            {
                // If so, grow the texture size by a power of two
                if( dwTexWidth < dwTexHeight )
                    dwTexWidth *= 2;
                else
                    dwTexHeight *= 2;

                // And restart the loop at the first glyph
                x = 0;
                y = 0;
                c = dwStartGlyph;
                str[0] = c;
                GetTextExtentPoint32W( hFontDC, str, 1, &size );
            }
        }

        // Update the cursor from the ABC widths for the letter
        if( FALSE == GetCharABCWidthsW( hFontDC, str[0], str[0], &abc ) )
            x += size.cx + 2;
        else
            x += abc.abcB + 2;
    }

    // Return the result
    (*pdwTexWidth)  = dwTexWidth;
    (*pdwTexHeight) = dwTexHeight;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteTargaFile()
// Desc: Writes 32-bit RGBA data to a .tga file
//-----------------------------------------------------------------------------
HRESULT WriteTargaFile( TCHAR* strFileName, DWORD dwWidth, DWORD dwHeight,
                        DWORD* pRGBAData )
{
    struct TargaHeader
    {
        BYTE IDLength;
        BYTE ColormapType;
        BYTE ImageType;
        BYTE ColormapSpecification[5];
        WORD XOrigin;
        WORD YOrigin;
        WORD ImageWidth;
        WORD ImageHeight;
        BYTE PixelDepth;
        BYTE ImageDescriptor;
    } tga;

    // Create the file
    FILE* file = fopen( strFileName, "wb" );
    if( NULL == file )
        return E_FAIL;

    // Write the TGA header
    ZeroMemory( &tga, sizeof(tga) );
    tga.IDLength        = 0;
    tga.ImageType       = 2;
    tga.ImageWidth      = (WORD)dwWidth;
    tga.ImageHeight     = (WORD)dwHeight;
    tga.PixelDepth      = 32;
    tga.ImageDescriptor = 0x28;
    fwrite( &tga, sizeof(TargaHeader), 1, file );

    // Write the pixels
    fwrite( pRGBAData, sizeof(DWORD), dwHeight*dwWidth, file );

    // Close the file and return okay
    fclose( file );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteFontInfoFile()
// Desc: Writes the font's glyph info to a file
//-----------------------------------------------------------------------------
HRESULT WriteFontInfoFile( TCHAR* strFileName, GLYPH_ATTR* pGlyphs )
{
    // Create the info file
    FILE* file = fopen( strFileName, "wb" );

    // Write out the file version
    fwrite( &g_dwVersion,    sizeof(DWORD), 1, file ); 

    // Write out the font height
    fwrite( &g_dwFontEffectiveHeight, sizeof(DWORD), 1, file ); 

    // Write out the texture values
    fwrite( &g_dwTexWidth,   sizeof(DWORD), 1, file ); 
    fwrite( &g_dwTexHeight,  sizeof(DWORD), 1, file ); 
    fwrite( &g_dwTexBPP,     sizeof(DWORD), 1, file ); 

    // Write out the glpyh range
    fwrite( &g_dwStartGlyph, sizeof(WCHAR), 1, file ); 
    fwrite( &g_dwEndGlyph,   sizeof(WCHAR), 1, file ); 

    // Write the glyph attributes to the file
    fwrite( &g_dwNumGlyphs, sizeof(DWORD), 1, file ); 
    fwrite( pGlyphs, sizeof(GLYPH_ATTR), g_dwNumGlyphs, file ); 

    // Close the file and return okay
    fclose( file );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFontFiles()
// Desc: Creates the font and renders all glyphs to a bitmap. Then, the bitmap
//       image is written out to a .tga file, and the glyph spacing info is
//       written to a .abc file.
//-----------------------------------------------------------------------------
HRESULT CreateFontFiles()
{
    // Note: if you want to expand this app to accept more unicode characters, 
    // start by allocating space to hold glyph ranges as here:
    //    GLYPHSET* pGlyphSet = (GLYPHSET*)new BYTE[GetFontUnicodeRanges( hDC, NULL )];
    //    GetFontUnicodeRanges( hDC, pGlyphSet );
    //    delete pGlyphSet;
    // This information will then need to be saved to the font info (.abc) file
    // which will be read and used by code to render the font on an Xbox. (In 
    // other words, if using the XBFont sample code, you will need to modify
    // that code as well.)

    // Allocate space for the glyph attributes
    GLYPH_ATTR* pGlyphs = new GLYPH_ATTR[g_dwNumGlyphs];

    // Create a DC for rendering text
    HDC hDC = CreateCompatibleDC( NULL );

    // Create a font. Note that the the font height is converted to what a form
    // that CreateFont expects. Also note that by specifying ANTIALIASED_QUALITY,
    // we might get an antialiased font, but this is not guaranteed.
    HFONT hFont = CreateFont( -MulDiv( g_dwFontHeight, GetDeviceCaps( hDC, LOGPIXELSY ), 72 ), 
                              0, 0, 0, g_dwFontWeight, g_bItalicized,
                              FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                              VARIABLE_PITCH, g_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    // Setup the DC for the font
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor( hDC, 0x00000000 );
    SetBkMode( hDC, TRANSPARENT );
    SelectObject( hDC, hFont );
    SetTextAlign( hDC, TA_TOP );
    SetMapMode( hDC, MM_TEXT );

    // Determine the texture size
    CalcTextureSize( hDC, g_dwStartGlyph, g_dwEndGlyph,
                          &g_dwTexWidth, &g_dwTexHeight );

    // Recreate the font. Okay, this is weird, but, apparently, calling text
    // functions such as GetTextExtentPoint32() or GetCharABCWidths() before
    // creating a DIB section (the bitmap below) breaks antialiasing of the
    // font. The CalcTextureSize() function made some of these calls, so we
    // now get around the problem by re-creating the font.
    DeleteObject( hFont );
    hFont = CreateFont( -MulDiv( g_dwFontHeight, GetDeviceCaps( hDC, LOGPIXELSY ), 72 ), 
                    0, 0, 0, g_dwFontWeight, g_bItalicized,
                    FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                    VARIABLE_PITCH, g_strFontName );
    if( NULL==hFont )
        return E_FAIL;
    SelectObject( hDC, hFont );

    // Prepare to create a bitmap
    DWORD*     pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)g_dwTexWidth;
    bmi.bmiHeader.biHeight      = -(int)g_dwTexHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a bitmap for the font
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    SelectObject( hDC, hbmBitmap );

    // Get the effective font height
    WCHAR str[2] = L"A";
    SIZE  size;
    GetTextExtentPoint32W( hDC, str, 1, &size );
    g_dwFontEffectiveHeight = size.cy;

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD index = 0;
    DWORD x     = 0;
    DWORD y     = 0;

    for( WCHAR c = g_dwStartGlyph; c && c <= g_dwEndGlyph; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32W( hDC, str, 1, &size );

        if( (DWORD)(x+2*size.cx+1) > g_dwTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        // Get the ABC widths for the letter
        ABC abc;
        if( FALSE == GetCharABCWidthsW( hDC, str[0], str[0], &abc ) )
        {
            abc.abcA = 0;
            abc.abcB = size.cx;
            abc.abcC = 0;
        }

        int w = abc.abcB + 1;
        int h = size.cy + 1;

        // Store the letter attributes
        pGlyphs[index].wOffset  = (short)(abc.abcA);
        pGlyphs[index].wWidth   = (short)(abc.abcB);
        pGlyphs[index].wAdvance = (short)(abc.abcB + abc.abcC);
        pGlyphs[index].left     = (FLOAT)(x+0) / (FLOAT)(g_dwTexWidth-1);
        pGlyphs[index].top      = (FLOAT)(y+0) / (FLOAT)(g_dwTexHeight-1);
        pGlyphs[index].right    = (FLOAT)(x+w) / (FLOAT)(g_dwTexWidth-1);
        pGlyphs[index].bottom   = (FLOAT)(y+h) / (FLOAT)(g_dwTexHeight-1);
        index++;

        // Output the letter
        ExtTextOutW( hDC, x-abc.abcA, y, ETO_OPAQUE, NULL, str, 1, NULL );

        // Advance the cursor to the next position
        x += abc.abcB + 2;
    }

    // Add alpha channel
    for( y=0; y < g_dwTexHeight; y++ )
    {
        for( x=0; x < g_dwTexWidth; x++ )
        {
            DWORD color = 0x00000000;
            DWORD alpha = ( pBitmapBits[g_dwTexWidth*y + x] & 0x000000ff );
            if( alpha > 0 )
                color = (alpha<<24) | 0x00ffffff;
            pBitmapBits[g_dwTexWidth*y + x] = color;
        }
    }

    // Write out the files
    TCHAR strFileName[80];

    // Write out the texture file
    sprintf( strFileName, "%s_%d.tga", g_strFontName, g_dwFontHeight );
    WriteTargaFile( strFileName, g_dwTexWidth, g_dwTexHeight, pBitmapBits );

    // Write the info file
    sprintf( strFileName, "%s_%d.abc", g_strFontName, g_dwFontHeight );
    WriteFontInfoFile( strFileName, pGlyphs );

    // Clean up used objects and return
    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );
    delete pGlyphs;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ChooseFontProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI ChooseFontProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static TCHAR strStartGlyph[80] = _T("");
    static TCHAR strEndGlyph[80]   = _T("");
    static TCHAR strFontName[80]   = _T("");

    switch( msg )
    {
        case WM_COMMAND:
            // Handle a font selection change
            if( CBN_SELCHANGE == HIWORD(wParam) && IDC_FONT == LOWORD(wParam) )
            {
                LONG lCurSel = SendDlgItemMessage( hWnd, IDC_FONT, CB_GETCURSEL, 0, 0 );
                SendDlgItemMessage( hWnd, IDC_FONT, CB_GETLBTEXT, (WPARAM)lCurSel, (LPARAM)strFontName );

                HDC   hDC   = CreateCompatibleDC( NULL );
                HFONT hFont = CreateFont( 0, 0, 0, 0, FW_DONTCARE,  FALSE,
                                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                          VARIABLE_PITCH, strFontName );
                if( hFont )
                {
                    SelectObject( hDC, hFont );

                    // Get the starting and ending glyph range
                    GLYPHSET* pGlyphSet = (GLYPHSET*)new BYTE[GetFontUnicodeRanges( hDC, NULL )];
                    GetFontUnicodeRanges( hDC, pGlyphSet );
                    DWORD dwStartGlyph = pGlyphSet->ranges[0].wcLow;
                    DWORD dwEndGlyph   = dwStartGlyph + (WCHAR)pGlyphSet->ranges[0].cGlyphs;

                    // Update the dialog edit controls
                    _stprintf( strStartGlyph, "%ld", dwStartGlyph );
                    _stprintf( strEndGlyph,   "%ld", dwEndGlyph );
                    SendDlgItemMessage( hWnd, IDC_GLYPHRANGE_START, WM_SETTEXT, 0, (LPARAM)strStartGlyph );
                    SendDlgItemMessage( hWnd, IDC_GLYPHRANGE_END,   WM_SETTEXT, 0, (LPARAM)strEndGlyph );

                    delete pGlyphSet;
                    DeleteObject( hFont );
                }
                DeleteDC( hDC );
            }

            // Handle the dialog's OK button
            if( IDOK == LOWORD(wParam) )
            {
                // Report an error if no font was chosen
                if( 0 == strFontName[0] )
                {
                    MessageBox( hWnd, _T("Please choose a font!"), 
                                      _T("XBFontMaker"), MB_ICONERROR | MB_OK ); 
                    return TRUE;
                }

                // Extract the glyph range
                SendDlgItemMessage( hWnd, IDC_GLYPHRANGE_START, WM_GETTEXT, 80, (LPARAM)strStartGlyph );
                SendDlgItemMessage( hWnd, IDC_GLYPHRANGE_END,   WM_GETTEXT, 80, (LPARAM)strEndGlyph );
                LONG lStartGlyph = atol( strStartGlyph );
                LONG lEndGlyph   = atol( strEndGlyph );

                // Report an error if the glyph range does not make sense
                if( ( lStartGlyph >= lEndGlyph ) || ( lStartGlyph <= 0 ) || ( lEndGlyph > 65535 ) )
                {
                    MessageBox( hWnd, _T("Invalid glyph range! The glyph\n")
                                      _T("range must be 1 and 65535"), 
                                      _T("XBFontMaker"), MB_ICONERROR | MB_OK ); 
                    return TRUE;
                }

                // Globally store the glyph range
                g_dwStartGlyph = (WCHAR)lStartGlyph;
                g_dwEndGlyph   = (WCHAR)lEndGlyph;
                g_dwNumGlyphs  = (DWORD)(g_dwEndGlyph - g_dwStartGlyph + 1);
            }

            break;
    }

    // Return zero to invoke the default font dialog handler
    return 0;
}




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
int _cdecl main( int argc, CHAR** argv )
{
    // Get the font's properties from a customized ChooseFont dialog
    CHOOSEFONT cf;
    LOGFONT    lf;
    ZeroMemory( &cf, sizeof(cf) );
    cf.lStructSize    = sizeof(cf);
    cf.lpLogFont      = &lf;
    cf.Flags          = CF_ENABLEHOOK | CF_SCREENFONTS | CF_ENABLETEMPLATE;
    cf.hInstance      = GetModuleHandle(NULL);
    cf.lpTemplateName = MAKEINTRESOURCE(IDD_FONTMAKER); // The custom dialog
    cf.lpfnHook       = (LPCFHOOKPROC)ChooseFontProc;   // The custom dlg proc
    if( !ChooseFont( &cf ) )
        return 0;

    // Store font properties globally
    _tcscpy( g_strFontName, cf.lpLogFont->lfFaceName );
    g_dwFontHeight = cf.iPointSize / 10;
    g_dwFontWeight = cf.lpLogFont->lfWeight;
    g_bItalicized  = cf.lpLogFont->lfItalic;

    // Create the texture and the info file for the font
    if( FAILED( CreateFontFiles() ) )
        printf( "ERROR: Could not create font files!\n" );

    return 0;
}




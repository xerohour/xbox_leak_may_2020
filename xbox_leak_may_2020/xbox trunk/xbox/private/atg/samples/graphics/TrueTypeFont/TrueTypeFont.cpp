//-----------------------------------------------------------------------------
// File: TrueTypeFont.cpp
//
// Desc: Shows how to use the New XFONT extensions for TrueType Fonts on Xbox
//
// Hist: 11.16.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

//-----------------------------------------------------------------------------
// XFONT_TRUETYPE must be defined before the include of xfont.h or the 
// XFONT_OpenTrueTypeFont function will not be defined.  This function is
// used to load the raw .ttf TrueType Font files and use them directly in the
// XFONT class on the fly.
//-----------------------------------------------------------------------------
#define XFONT_TRUETYPE
#include <xfont.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_2, L"Change Font Style" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )




//-----------------------------------------------------------------------------
// defines for the FontDrawStyle
//-----------------------------------------------------------------------------
#define FONT_DRAW_STYLE_1       1
#define FONT_DRAW_STYLE_2       2
#define FONT_DRAW_STYLE_3       3
#define FONT_DRAW_STYLE_4       4
#define FONT_DRAW_STYLE_5       5
#define FONT_DRAW_STYLE_6       6
#define FONT_DRAW_STYLE_MAX     FONT_DRAW_STYLE_6




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen

    XFONT*      m_pDefaultTrueTypeFont;  // Pointer to the Default Arial TrueTypeFont
    XFONT*      m_pArialTrueTypeFont;    // Pointer to the Arial TrueTypeFont
    XFONT*      m_pArial16BitmapFont;    // Pointer to the Arial16Normal Bitmap font
                                         // Created with the MakeFont tool.
    XFONT*      m_pKanji16BitmapFont;    // Pointer to the Arial16Kanji Bitmap font
                                         // Created with the MakeFont tool.
    USHORT*     m_pKanjiMessage;         // Contains the first 20 Unicode Kanji Characters

    void*       m_pFontMemory;           // Memory to hold image of below font.
    XFONT*      m_pArial24BitmapFont;    // Pointer to the Arial24Italic Bitmap font
                                         // Created with the MakeFont tool.


    DWORD       m_dwFontDrawStyle;       // Which Font draw style are we showing.
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Size of the Font Cache in Bytes
    DWORD dwFontCacheSize = 16 * 1024;

    // Create the Default Arial TrueTypeFont
    if( FAILED( XFONT_OpenDefaultFont( &m_pDefaultTrueTypeFont ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the Arial TrueTypeFont
    if( FAILED( XFONT_OpenTrueTypeFont( L"D:\\media\\fonts\\arialuni.ttf",
                                        dwFontCacheSize,&m_pArialTrueTypeFont ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // If the FontCacheSize is as large as the bitmap font file or larger, the font
    // renders MUCH more quickly.  If its smaller then file size, the cache holds
    // the most recently drawn glyphs and recycles them as it needs to.  This can
    // be fast if you experiment with the size.
    dwFontCacheSize = 16*1024;

    // Create the Arial16Normal Bitmap Font
    if( FAILED( XFONT_OpenBitmapFont( L"D:\\media\\fonts\\Arial16Normal.bmf",
                                      dwFontCacheSize,&m_pArial16BitmapFont ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the Arial16Kanji Bitmap Font
    if( FAILED( XFONT_OpenBitmapFont( L"D:\\media\\fonts\\Arial16Kanji.bmf",
                                      dwFontCacheSize,&m_pKanji16BitmapFont ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    //Preload the Arial24Italic Bitmap Font
    HANDLE hFileHandle = CreateFile( "D:\\media\\fonts\\Arial24Italic.bmf",
                                     GENERIC_READ,
                                     FILE_SHARE_READ,
                                     0,
                                     OPEN_EXISTING,
                                     0,0);

    if( hFileHandle == INVALID_HANDLE_VALUE )
        return XBAPPERR_MEDIANOTFOUND;

    DWORD dwNumBytesRead = 0;
    DWORD dwFontMemorySize = GetFileSize( hFileHandle, 0 );

    m_pFontMemory = malloc( dwFontMemorySize );

    if( ReadFile( hFileHandle, m_pFontMemory, dwFontMemorySize, &dwNumBytesRead, 0 ) == 0 )
        return XBAPPERR_MEDIANOTFOUND;

    CloseHandle( hFileHandle );

    // Create the Arial24Italic Bitmap Font from the file loaded above.
    // The memory associated with this file must NOT be deleted until you no
    // longer wish to draw text with this font!
    if( FAILED( XFONT_OpenBitmapFontFromMemory( m_pFontMemory,
                                                dwFontMemorySize, &m_pArial24BitmapFont ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_dwFontDrawStyle = FONT_DRAW_STYLE_1;

    m_pKanjiMessage = new USHORT[21];
    for (USHORT i=0x4e00; i<0x4e14; i++)
        m_pKanjiMessage[i-0x4e00] = i;

    m_pKanjiMessage[20] = 0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle Font Draw Styles
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        ++m_dwFontDrawStyle;
        if( m_dwFontDrawStyle > FONT_DRAW_STYLE_MAX )
            m_dwFontDrawStyle = FONT_DRAW_STYLE_1;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    LPDIRECT3DSURFACE8 pFrontBuffer;
    m_pd3dDevice->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&pFrontBuffer);

    switch (m_dwFontDrawStyle)
    {
        case FONT_DRAW_STYLE_1:
        {
            // NOTE:  Cannot set the Font Style, Height and Anti-alias if font is the default font.

            // Draw a message in the defined TrueType Font.
			m_pDefaultTrueTypeFont->TextOut( pFrontBuffer, 
                                           L"This is the Default Arial 24 Pixel Font", -1, 60, 100 );
        }
        break;

        case FONT_DRAW_STYLE_2:
        {
			// Changing the Font Size, Style and Anti-Alias on the fly
			// like this is VERY slow.

            // Change Font Size - In Pixels
			m_pArialTrueTypeFont->SetTextHeight( 20 );

            // Change Font Style - XFONT_NORMAL, XFONT_BOLD, 
			//                     XFONT_ITALICS, XFONT_BOLDITALICS
			m_pArialTrueTypeFont->SetTextStyle( XFONT_NORMAL );

            // Anti-Alias the font -- 0 for no anti-alias, 2 for some, 4 for MAX!
			m_pArialTrueTypeFont->SetTextAntialiasLevel( 0 );

            // Draw a message in the defined TrueType Font.
			m_pArialTrueTypeFont->TextOut( pFrontBuffer, 
                                            L"This is Arial True Type 20 Pixel Normal", -1, 60, 100 );
        }
        break;

        case FONT_DRAW_STYLE_3:
        {
			// Changing the Font Size, Style and Anti-Alias on the fly
			// like this is VERY slow.

            // Change Font Size - In Pixels
			m_pArialTrueTypeFont->SetTextHeight( 18 );

            // Change Font Style - XFONT_NORMAL, XFONT_BOLD, 
			//                     XFONT_ITALICS, XFONT_BOLDITALICS
			m_pArialTrueTypeFont->SetTextStyle( XFONT_BOLDITALICS );

            // Anti-Alias the font -- 0 for no anti-alias, 2 for some, 4 for MAX!
			m_pArialTrueTypeFont->SetTextAntialiasLevel( 0 );

            // Draw a message in the defined TrueType Font.
			m_pArialTrueTypeFont->TextOut( pFrontBuffer, 
                                            L"This is Arial TrueType 18 pixel Bold Italic", 
                                            -1, 60, 120 );
        }
        break;

        case FONT_DRAW_STYLE_4:
        {
            // NOTE:  Cannot set the Font Parameters if font is from Bitmap file.
            // These are set when the font is created using the MakeFont tool.

            // Draw a message in the defined TrueType Font.
            m_pArial16BitmapFont->TextOut( pFrontBuffer, 
                                           L"This is Arial Bitmap in 16 Pixel Normal", 
                                           -1, 60, 100 );
        }
        break;
    
        case FONT_DRAW_STYLE_5:
        {
            // NOTE:  Cannot set the Font Parameters if font is from Bitmap file.
            // These are set when the font is created using the MakeFont tool.
    
            // Draw a message in the defined TrueType Font.
            m_pArial24BitmapFont->TextOut( pFrontBuffer, 
                                           L"This is Arial Bitmap 24 pixel Italic", 
                                           -1, 60, 120 );
        }
        break;

        case FONT_DRAW_STYLE_6:
        {
            // NOTE:  Cannot set the Font Parameters if font is from Bitmap file.
            // These are set when the font is created using the MakeFont tool.
    
            // Draw a message in the defined TrueType Font.
            m_pArial16BitmapFont->TextOut( pFrontBuffer, 
                                           L"The first 20 characters in Kanji Bitmap 16 pixel", 
                                           -1, 60, 120 );

            m_pKanji16BitmapFont->TextOut( pFrontBuffer,
                                           m_pKanjiMessage, -1, 60, 140 );
        }
        break;
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"TrueTypeFont" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    pFrontBuffer->Release();
    return S_OK;
}


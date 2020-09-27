#include "stdafx.h"
#include "xfontfuncs.h"

// Get the pixel width of a string
//
// Parameters:
//      string -- The string to get the length in pixels
//      length -- OPTIONAL.  The length of the string (-1 if NULL terminated, this is default)
int GetStringPixelWidth( XFONT* pFont, IN LPCWSTR string, IN int length /*=-1*/ ) 
{
    if( NULL == string || NULL == pFont )
    {
        DebugPrint( "GetStringPixelWidth( WCHAR* ):Invalid argument(s) passed in!\n" );

        return -1;
    }

	unsigned int outlen = 0;        // Return value

	XFONT_GetTextExtent( pFont, string, length, &outlen );
	
    return outlen;
}


// Get the width of the char*
int GetStringPixelWidth( XFONT* pFont, IN char* string, IN int length /*=-1*/ ) 
{
    if( NULL == string || NULL == pFont )
    {
        DebugPrint( "GetStringPixelWidth( char* ):Invalid argument(s) passed in!\n" );

        return -1;
    }

    WCHAR newWStr[MAX_PATH];
    ZeroMemory( newWStr, MAX_PATH * sizeof( WCHAR ) );

    _snwprintf( newWStr, MAX_PATH - 1, L"%S", string );

    return GetStringPixelWidth( pFont, newWStr, length );
}


// Get the size of the current font
void GetFontSize( XFONT* pFont, unsigned int* pheight, unsigned int* pdecent )
{
    // Verify the parameters that were passed in are corred
    if( NULL == pheight || NULL == pdecent || NULL == pFont )
    {
        DebugPrint( "GetFontSize():Invalid argument(s) passed!!\n" );

        return;
    }

    XFONT_GetFontMetrics( pFont, pheight, pdecent );
}


// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( XFONT* pFont, char* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( NULL == pString ) || ( 0 > ( x2 - x1 ) ) || ( NULL == pFont ) )
    {
        DebugPrint( "CXboxVideo::TruncateStringToFit():Invalid argument(s)!! x1 - '%f', x2 - '%f'\n", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pFont, pString, -1 ) >= maxLen )
    {
        pString[strlen( pString ) - 1] = '\0';

        // Ensure we don't end up in an endless loop
        if( '\0' == pString[0] )
        {
            break;
        }
    }

    return S_OK;
}


// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( XFONT* pFont, WCHAR* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( NULL == pString ) || ( 0 > ( x2 - x1 ) ) || ( NULL == pFont ) )
    {
        DebugPrint( "CXboxVideo::TruncateStringToFit():Invalid argument(s)!! x1 - '%f', x2 - '%f'\n", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pFont, pString, -1 ) >= maxLen )
    {
        pString[wcslen( pString ) - 1] = L'\0';

        // Ensure we don't end up in an endless loop
        if( L'\0' == pString[0] )
        {
            break;
        }
    }

    return S_OK;
}


// Open the font file specified and use the font pointer passed in
HRESULT OpenTTFontFile( XFONT*& pFont, const WCHAR* pwszFontFilename )
{
    if( NULL != pFont || NULL == pwszFontFilename )
    {
        DebugPrint( "OpenFontFile():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    XFONT_OpenTrueTypeFont( pwszFontFilename, 4096, &pFont );

	if( NULL == pFont )
    {
        DebugPrint( "OpenFontFile():Failed to Open the True Type Font!!\n" );

        return E_FAIL;
    }

    XFONT_SetTextColor( pFont, gc_dwCOLOR_WHITE );
    XFONT_SetBkColor( pFont, gc_dwCOLOR_BLACK );
    XFONT_SetTextHeight( pFont, gc_uiDEFAULT_FONT_HEIGHT );
    XFONT_SetTextAntialiasLevel( pFont, gc_uiDEFAULT_FONT_ALIAS_LEVEL );
    XFONT_SetTextStyle( pFont, XFONT_NORMAL );

    return S_OK;
}


// Close an opened font
HRESULT CloseTTFont( XFONT*& pFont )
{
    if( NULL == pFont )
    {
        DebugPrint( "CloseTTFont():Invalid argument passed in!!\n" );

        return E_INVALIDARG;
    }

    XFONT_Release( pFont );
    pFont = NULL;

    return S_OK;
}

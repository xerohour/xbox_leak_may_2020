#include "stdafx.h"
#include "xfontfuncs.h"

XFONT *g_pFont;

// Get the pixel width of a string
//
// Parameters:
//      string -- The string to get the length in pixels
//      length -- OPTIONAL.  The length of the string (-1 if NULL terminated, this is default)
int GetStringPixelWidth( IN LPCWSTR string, IN int length /*=-1*/ ) 
{
    if( !string )
    {
        XDBGWRN( APP_TITLE_NAME_A, "GetStringPixelWidth( WCHAR* ):Invalid (string) pointer passed in!" );

        return -1;
    }

	unsigned int outlen = 0;        // Return value

	XFONT_GetTextExtent( g_pFont, string, length, &outlen );
	
    return outlen;
}


// Get the width of the char*
int GetStringPixelWidth( IN char* string, IN int length /*=-1*/ ) 
{
    if( !string )
    {
        XDBGWRN( APP_TITLE_NAME_A, "GetStringPixelWidth( char* ):Invalid pointer passed in!" );

        return -1;
    }

    int len = strlen( string );

    WCHAR newWStr[MAX_PATH];
    ZeroMemory( newWStr, MAX_PATH * sizeof( WCHAR ) );

    _snwprintf( newWStr, MAX_PATH - 1, L"%S", string );

    return GetStringPixelWidth( newWStr, length );
}


// Get the size of the current font
void GetFontSize( unsigned int* pheight, unsigned int* pdecent )
{
    // Verify the parameters that were passed in are corred
    if( ( !pheight ) || ( !pdecent ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "GetFontSize():Invalid arguments passed!!" );

        return;
    }

    XFONT_GetFontMetrics( g_pFont, pheight, pdecent );
}


// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( char* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( !pString ) || ( 0 > ( x2 - x1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXboxVideo::TruncateStringToFit():Invalid argument!! x1 - '%f', x2 - '%f'", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pString, -1 ) >= maxLen )
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
HRESULT TruncateStringToFit( WCHAR* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( !pString ) || ( 0 > ( x2 - x1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXboxVideo::TruncateStringToFit():Invalid argument!! x1 - '%f', x2 - '%f'", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pString, -1 ) >= maxLen )
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

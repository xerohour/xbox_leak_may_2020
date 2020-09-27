#ifndef _XFONTFUNCS_H_
#define _XFONTFUNCS_H_

#include <xfont.h>

// Get the pixel width of a string
int GetStringPixelWidth( XFONT* pFont,
                         IN LPCWSTR string,
                         IN int length = -1 );

// Get the pixel width of a char*
int GetStringPixelWidth( XFONT* pFont,
                         IN char* string,
                         IN int length = -1 );

// Get the size of the currently selected font
void GetFontSize( XFONT* pFont,
                  unsigned int* pheight,
                  unsigned int* pdecent );

// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( XFONT* pFont,
                             char* pString,
                             unsigned int nStrSize,
                             float x1,
                             float x2 );

// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( XFONT* pFont,
                             WCHAR* pString,
                             unsigned int nStrSize,
                             float x1,
                             float x2 );

// Open the font file specified and use the font pointer passed in
HRESULT OpenTTFontFile( XFONT*& pFont,
                        const WCHAR* pwszFontFilename );

// Close an opened font
HRESULT CloseTTFont( XFONT*& pFont );

#endif // _XFONTFUNCS_H_
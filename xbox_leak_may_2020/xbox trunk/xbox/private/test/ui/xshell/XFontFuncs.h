#ifndef _XFONTFUNCS_H_
#define _XFONTFUNCS_H_

// Get the pixel width of a string
int GetStringPixelWidth( IN LPCWSTR string,
                         IN int length = -1 );

// Get the pixel width of a char*
int GetStringPixelWidth( IN char* string,
                         IN int length = -1 );

// Get the size of the currently selected font
void GetFontSize( unsigned int* pheight,
                  unsigned int* pdecent );

// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( char* pString,
                             unsigned int nStrSize,
                             float x1,
                             float x2 );

// Truncate a string to fit within a certain screen size
HRESULT TruncateStringToFit( WCHAR* pString,
                             unsigned int nStrSize,
                             float x1,
                             float x2 );

#endif // _XFONTFUNCS_H_
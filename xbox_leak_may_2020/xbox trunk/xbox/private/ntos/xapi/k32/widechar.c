#include "basedll.h"

int
UnicodeToUTF8(
    IN LPCWSTR lpWideCharStr,
    IN int cchWideChar,
    OUT LPSTR lpUTF8Str,
    IN int cbUTF8
    );

int
UTF8ToUnicode(
    IN LPCSTR lpUTF8Str,
    IN int cbUTF8,
    OUT LPWSTR lpWideCharStr,
    IN int cchWideChar
    );

int WINAPI MultiByteToWideChar(
    IN UINT     CodePage,
    IN DWORD    dwFlags,
    IN LPCSTR   lpMultiByteStr,
    IN int      cbMultiByte,
    OUT LPWSTR  lpWideCharStr,
    IN int      cchWideChar)
{
    USHORT LengthRequires;
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    NTSTATUS Status;

    if ( CodePage == 65000 ) {
        ASSERT( 0 && "CP_UTF7 is not supported" );
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if ( CodePage == CP_UTF8 ) {
        return UTF8ToUnicode(lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
    }

    if ( cbMultiByte == -1 ) {
        LengthRequires = (USHORT)(strlen(lpMultiByteStr) + sizeof(ANSI_NULL));
    } else {
        LengthRequires = (USHORT)(cbMultiByte + sizeof(ANSI_NULL));
    }

    if ( cchWideChar == 0 ) {
        return LengthRequires;
    }
    else if ( cchWideChar < (int)LengthRequires ) {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return 0;        
    }

    Ansi.Buffer = (PSTR)lpMultiByteStr;
    Ansi.Length = (USHORT)(LengthRequires - sizeof(ANSI_NULL));
    Ansi.MaximumLength = (USHORT)(LengthRequires);

    Unicode.Buffer = lpWideCharStr;
    Unicode.Length = (USHORT)((LengthRequires - sizeof(ANSI_NULL)) * sizeof(WCHAR));
    Unicode.MaximumLength = cchWideChar * sizeof(WCHAR);

    Status = RtlAnsiStringToUnicodeString( &Unicode, &Ansi, FALSE );

    if ( NT_SUCCESS(Status) ) {
        return LengthRequires;
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
        return 0;
    }
}

int WINAPI WideCharToMultiByte(
    IN UINT     CodePage,
    IN DWORD    dwFlags,
    IN LPCWSTR  lpWideCharStr,
    IN int      cchWideChar,
    OUT LPSTR   lpMultiByteStr,
    IN int      cbMultiByte,
    IN LPCSTR   lpDefaultChar,
    OUT LPBOOL  lpUsedDefaultChar)
{
    USHORT LengthRequires;
    UNICODE_STRING Unicode;
    ANSI_STRING Ansi;
    NTSTATUS Status;

    if ( CodePage == 65000 ) {
        ASSERT( 0 && "CP_UTF7 is not supported" );
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if ( CodePage == CP_UTF8 ) {
        return UnicodeToUTF8(lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte);
    }

    if ( lpDefaultChar ) {
        ASSERT( 0 && "lpDefaultChar is not supported" );
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if ( lpUsedDefaultChar ) {
        *lpUsedDefaultChar = FALSE;
    }

    if ( cchWideChar == -1 ) {
        LengthRequires = wcslen(lpWideCharStr) + sizeof(ANSI_NULL);
    } else {
        LengthRequires = cchWideChar + sizeof(ANSI_NULL);
    }

    if ( cbMultiByte == 0 ) {
        return LengthRequires;
    }
    else if ( cbMultiByte < (int)LengthRequires ) {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return 0;        
    }

    Ansi.Buffer = lpMultiByteStr;
    Ansi.Length = 0;
    Ansi.MaximumLength = (USHORT)cbMultiByte;

    Unicode.Buffer = (PWSTR)lpWideCharStr;
    Unicode.Length = (USHORT)(LengthRequires * sizeof(WCHAR) - sizeof(UNICODE_NULL));
    Unicode.MaximumLength = (USHORT)(LengthRequires * sizeof(WCHAR));

    Status = RtlUnicodeStringToAnsiString( &Ansi, &Unicode, FALSE );

    if ( NT_SUCCESS(Status) ) {
        return LengthRequires;
    } else {
        SetLastError( RtlNtStatusToDosError(Status) );
        return 0;
    }
}


//*****************************************************************************
// UTF8 <-> Unicode conversion functions
//

#define HIGH_BITS1 0x80
#define HIGH_BITS2 0xC0
#define HIGH_BITS3 0xE0
#define HIGH_BITS4 0xF0
#define LOW_BITS4  0x0F
#define LOW_BITS5  0x1F
#define LOW_BITS6  0x3F

unsigned long offsetsFromUTF8[6] = { 0x00000000, 0x00003080, 0x000E2080,
                                     0x03C82080, 0xFA082080, 0x82082080};

char bytesFromUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};

unsigned long kReplacementCharacter = 0x0000FFFD;
unsigned long kMaximumSimpleUniChar = 0x0000FFFF;
unsigned long kMaximumUniChar       = 0x0010FFFF;
int           halfShift             = 10;
unsigned long halfBase              = 0x0010000;
unsigned long halfMask              = 0x3FF;
unsigned long kSurrogateHighStart   = 0xD800;
unsigned long kSurrogateLowStart    = 0xDC00;

/*******************************************************************************
UTF8ToUnicode
 
Converts a UTF8 string to a Unicode string.
 
Returns:
    DWORD size of Unicode string
*******************************************************************************/
int
UTF8ToUnicode(
    IN LPCSTR lpUTF8Str,
    IN int cbUTF8,
    OUT LPWSTR lpWideCharStr,
    IN int cchWideChar
    )
{
    int LengthRequires = 0;
    unsigned short *pchWideChar = (unsigned short *) lpWideCharStr;
    unsigned char *lpUTF8StrPtr = (unsigned char *)lpUTF8Str;
    unsigned long ch = 0;
    unsigned short extraBytesToWrite = 0;

    if ( cbUTF8 == -1 )
    {
        cbUTF8 = strlen(lpUTF8Str) + sizeof(ANSI_NULL);
    }

    while ((lpUTF8StrPtr - ((unsigned char *)lpUTF8Str)) < cbUTF8)
    {
        ch = 0;
        extraBytesToWrite = bytesFromUTF8[*lpUTF8StrPtr];

        if (((lpUTF8StrPtr - ((unsigned char *)lpUTF8Str)) + extraBytesToWrite) > cbUTF8)
        {
            break;
        }

        switch(extraBytesToWrite)
        {
            case 5:ch += *lpUTF8StrPtr++; ch <<=6;
            case 4:ch += *lpUTF8StrPtr++; ch <<=6;
            case 3:ch += *lpUTF8StrPtr++; ch <<=6;
            case 2:ch += *lpUTF8StrPtr++; ch <<=6;
            case 1:ch += *lpUTF8StrPtr++; ch <<=6;
            case 0:ch += *lpUTF8StrPtr++;
        }

        ch -= offsetsFromUTF8[extraBytesToWrite];

        if (ch <= kMaximumSimpleUniChar)
        {
            if (++LengthRequires <= cchWideChar)
            {
                *((unsigned long *)pchWideChar) = ch;
                pchWideChar++;
            }
        }
        else if (ch > kMaximumUniChar)
        {
            if (++LengthRequires <= cchWideChar)
            {
                *((unsigned long *)pchWideChar) = kReplacementCharacter;
                pchWideChar++;
            }
        }
        else
        {
            ch -= halfBase;

            if (++LengthRequires <= cchWideChar)
            {
                *((unsigned long *)pchWideChar) = ((ch >> halfShift) + kSurrogateHighStart);
                pchWideChar++;
            }

            if (++LengthRequires <= cchWideChar)
            {
                *((unsigned long *)pchWideChar) = ((ch & halfMask) + kSurrogateLowStart);
                pchWideChar++;
            }
        }
    }

    if ( cchWideChar == 0 )
    {
        return LengthRequires;
    }
    else if ( cchWideChar < LengthRequires )
    {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return 0;        
    }

    return LengthRequires;
}

/*******************************************************************************
UnicodeToUTF8
 
Converts a Unicode string to a UTF-8 string.
 
Returns:
    DWORD size of UTF8 string
*******************************************************************************/
int
UnicodeToUTF8(
    IN LPCWSTR lpWideCharStr,
    IN int cchWideChar,
    OUT LPSTR lpUTF8Str,
    IN int cbUTF8
    )
{
    BYTE b;
    int LengthRequires = 0;
    HRESULT hr = S_OK;
    int i = 0;
 
    if ( cchWideChar == -1 ) {
        cchWideChar = wcslen(lpWideCharStr) + sizeof(ANSI_NULL);
    }

    for ( i = 0; i < cchWideChar; i += 1 )
    {
        if (*lpWideCharStr <= 0x7F)
        {
            // Single byte encoding
            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL) 
                { 
                    *lpUTF8Str++ = (char) *lpWideCharStr; 
                }
            }
        }
        else if (*lpWideCharStr <= 0x7FF)
        {
            // Double byte encoding
            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL)
                {
                    b = (BYTE) ((*lpWideCharStr & (LOW_BITS5 << 6)) >> 6);
                    b |= HIGH_BITS2;
                    *lpUTF8Str++ = (char) b;
                }
            } 

            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL)
                {
                    b = (BYTE) (*lpWideCharStr & LOW_BITS6);
                    b |= HIGH_BITS1;
                    *lpUTF8Str++ = (char) b;
                }
            }
        }
        else 
        {
            // Triple byte encoding
            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL)
                {
                    b = (BYTE) ((*lpWideCharStr & (LOW_BITS4 << 12)) >> 12);
                    b |= HIGH_BITS3;
                    *lpUTF8Str++ = (char) b;
                }
            }

            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL)
                {
                    b = (BYTE) ((*lpWideCharStr & (LOW_BITS6 << 6)) >> 6);
                    b |= HIGH_BITS1;
                    *lpUTF8Str++ = (char) b;
                }
            }

            if (++LengthRequires <= cbUTF8)
            {
                if (lpUTF8Str != NULL)
                {
                    b = (BYTE) (*lpWideCharStr & LOW_BITS6);
                    b |= HIGH_BITS1;
                    *lpUTF8Str++ = (char) b;
                }
            }
        }
 
        lpWideCharStr++;
    }

    if ( cbUTF8 == 0 )
    {
        return LengthRequires;
    }
    else if ( cbUTF8 < LengthRequires )
    {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return 0;        
    }

    return LengthRequires;
}

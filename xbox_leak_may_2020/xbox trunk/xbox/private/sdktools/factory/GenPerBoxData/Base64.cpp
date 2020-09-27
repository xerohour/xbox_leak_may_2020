/*++

    Copyright (c) Microsoft Corporation

    Module Name:

        Base64.cpp

    Abstract:

        Base64 encode/decode functions.

--*/

#include <windows.h>
#include <assert.h>
#include "Base64.h"
#include "gpbdver.h"

BYTE g_achBase64DecodeTable[256]={
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
    52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,64,0,1,2,3,4,5,6,7,8,9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,64,26,27,
    28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64
};

CHAR g_achBase64EncodeTable[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

BOOL WINAPI
Base64Decode(
    LPCSTR  pszSrc,
    ULONG   ulSrcSize,
    LPBYTE  pDst,
    PULONG  pulDstSize
    )

/*++

Routine Description:

    Base64Decode    This method decodes a Base-64 encoded chunk of binary data.

Arguments:

    pszSrc      Points to Base64-encoded string to decode.

    ulSrcSize   Length of the string pointed to by pszSrc, not including 
                terminating null character.

    pDst        Buffer to copy the decoded binary data into.  The size of this
                buffer must be at least BASE64_DECODE_BUFFER_SIZE(ulSrcSize)

    pulDstSize  On return contains the length in bytes of the decoded buffer.

Return Value:

     0 - Decode was successful.

--*/

{
    ULONG   ulIndex;
    ULONG   ulCurOut;
    DWORD   dwGroup;
    DWORD   dwCur;
    DWORD   dwPadSeen;
    CHAR    chCur;

    dwGroup  = 0;
    dwPadSeen = 0;
    ulCurOut = 0;

    if(ulSrcSize % 4 != 0)
        return FALSE;

    for(ulIndex = 0; ulIndex < ulSrcSize; ulIndex++)
    {
        chCur = pszSrc[ulIndex];
        if(chCur == '=')
        {
            dwPadSeen++;
            dwCur = 0;
        }
        else
        {
            dwCur = g_achBase64DecodeTable[chCur];
        }
        assert( dwCur != 64 );
        
        dwGroup |= (dwCur << (6 * (ulIndex & 0x3)));

        if((ulIndex & 0x3) == 0x3)
        {
            pDst[ulCurOut++] = (CHAR)( dwGroup        & 0xff);

            if(dwPadSeen <= 1)
            {
                pDst[ulCurOut++] = (CHAR)((dwGroup >>  8) & 0xff);

                if(dwPadSeen == 0)
                    pDst[ulCurOut++] = (CHAR)((dwGroup >> 16) & 0xff);
            }

            dwGroup = 0;
        }
    }

    *pulDstSize = ulCurOut;
    assert( ulCurOut <= BASE64_DECODE_BUFFER_SIZE(ulSrcSize) );
    
    return TRUE;
}

BOOL WINAPI
Base64Encode(
    const LPBYTE    pSrc,
    ULONG           ulSrcSize,
    LPSTR           pszDst
    )

/*++

Routine Description:

    Base64Encode    This method converts a buffer of arbitrary binary data into
                    a Base-64 encoded string.

Arguments:

    pSrc        Points to the buffer of bytes to encode.

    ulSrcSize   Number of bytes in pSrc to encode.

    pszDst      Buffer to copy the encoded output into.  The length of the buffer
                must be at least BASE64_ENCODE_BUFFER_SIZE(ulSrcSize).

Return Value:

     0 - Encoding successful.

--*/

{
    DWORD   dwGroup;
    ULONG   ulCurGroup = 0, ulIndex;

    for(ulIndex = 0; ulIndex < (ulSrcSize - (ulSrcSize % 3)); ulIndex += 3)
    {
        dwGroup = (pSrc[ulIndex+2] << 16) | (pSrc[ulIndex+1] << 8) | pSrc[ulIndex];

        pszDst[ulCurGroup++] = g_achBase64EncodeTable[  dwGroup        & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 6)  & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 12) & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 18) & 0x3f ];
    }

    //  Do the end special case.
    switch(ulSrcSize % 3)
    {
    case 2:
        dwGroup = (pSrc[ulIndex+1]  << 8)| pSrc[ulIndex];

        pszDst[ulCurGroup++] = g_achBase64EncodeTable[  dwGroup        & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 6)  & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 12) & 0x3f ];
        pszDst[ulCurGroup++] = '=';

        break;

    case 1:
        dwGroup = pSrc[ulIndex];

        pszDst[ulCurGroup++] = g_achBase64EncodeTable[  dwGroup        & 0x3f ];
        pszDst[ulCurGroup++] = g_achBase64EncodeTable[ (dwGroup >> 6)  & 0x3f ];
        pszDst[ulCurGroup++] = '=';
        pszDst[ulCurGroup++] = '=';

        break;
    }

    //  Null terminate the string.
    pszDst[ulCurGroup] = 0; 
    assert( ulCurGroup < BASE64_ENCODE_BUFFER_SIZE(ulSrcSize) );
    
    return TRUE;
}

// EOF

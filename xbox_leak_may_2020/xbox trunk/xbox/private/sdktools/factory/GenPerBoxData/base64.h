/*++

    Copyright (c) Microsoft Corporation

    Module Name:

        Base64.h

--*/

#ifndef __BASE64_H
#define __BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

#define BASE64_ENCODE_BUFFER_SIZE( ulSrcSize ) (((((ulSrcSize) + 2) / 3) * 4) + 1)

BOOL WINAPI
Base64Encode(               
    const LPBYTE    pSrc,
    ULONG           ulSrcSize,
    LPSTR           pszDst
    );

#define BASE64_DECODE_BUFFER_SIZE( ulSrcSize ) (((ulSrcSize) / 4) * 3)

BOOL WINAPI
Base64Decode(
    LPCSTR      pszSrc,
    ULONG       ulSrcSize,
    LPBYTE      pDst,
    PULONG      pulDstSize
    );

#ifdef __cplusplus
}
#endif

#endif // __BASE64_H

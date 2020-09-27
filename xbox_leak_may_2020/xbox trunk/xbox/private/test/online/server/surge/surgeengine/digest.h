/*++

    Copyright (c) 1998 Microsoft Corporation

    Module Name:

        CDigestAuth.h

    Abstract:

        This class performs digest authentication MD5 calculations.

    Author:

        Darren L. Anderson (darrenan) 5-Aug-1998

    Revision History:

        5-Aug-1998 darrenan

            Created.

--*/

#ifndef __DIGESTAUTH_H
#define __DIGESTAUTH_H

#define DIGESTBUF_LEN  33

enum DIGEST_AUTH_NAMES
{
    DIGEST_AUTH_USERNAME=0,
    DIGEST_AUTH_REALM,
    DIGEST_AUTH_NONCE,
    DIGEST_AUTH_URI,
    DIGEST_AUTH_RESPONSE,
    DIGEST_AUTH_DIGEST,
    DIGEST_AUTH_ALGORITHM,
    DIGEST_AUTH_OPAQUE,
    DIGEST_AUTH_CNONCE,
    DIGEST_AUTH_QOP,
    DIGEST_AUTH_NC,
    DIGEST_AUTH_LAST
};

#ifdef __cplusplus
extern "C" {
#endif

VOID WINAPI
ToHex(
    LPBYTE pSrc,
    UINT   cSrc,
    LPSTR  pDst
    );
    
HRESULT WINAPI
DigestFromCreds(
    IN  LPCSTR  pszAlgorithm,
    IN  LPCSTR  pszUsername,
    IN  LPCSTR  pszRealm,
    IN  LPCSTR  pszPassword,
    IN  LPCSTR  pszNonce,
    IN  LPCSTR  pszNonceCount,
    IN  LPCSTR  pszCNonce,
    IN  LPCSTR  pszQOP,
    IN  LPCSTR  pszMethod,
    IN  LPCSTR  pszURI,
    IN  LPCSTR  pszEntityDigest,
    OUT LPSTR   pszSessionKey,
    OUT LPSTR   pszResult
    );

HRESULT WINAPI
DigestFromKey(
    IN  LPCSTR  pszAlgorithm,
    IN  LPCSTR  pszSessionKey,
    IN  LPCSTR  pszNonce,
    IN  LPCSTR  pszNonceCount,
    IN  LPCSTR  pszCNonce,
    IN  LPCSTR  pszQOP,
    IN  LPCSTR  pszMethod,
    IN  LPCSTR  pszURI,
    IN  LPCSTR  pszEntityDigest,
    OUT LPSTR   pszResult
    );

HRESULT WINAPI
Base64Encode(               
    const LPBYTE    pSrc,
    ULONG           ulSrcSize,
    LPSTR           pszDst
    );

HRESULT WINAPI
Base64Decode(
    LPCSTR      pszSrc,
    ULONG       ulSrcSize,
    LPBYTE      pDst
    );

BOOL
ParseAuthorizationHeader(
    LPSTR pszHeader, 
    LPSTR pValueTable[DIGEST_AUTH_LAST]
    );

#ifdef __cplusplus
}
#endif

#endif // __DIGESTAUTH_H

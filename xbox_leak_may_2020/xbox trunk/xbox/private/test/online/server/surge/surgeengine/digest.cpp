/*++

    Copyright (c) 1998 Microsoft Corporation

    Module Name:

        DigestAuth.cpp

    Abstract:

        Performs digest authentication MD5 calculations.
        MD5.cpp is an external dependency.

    Author:

        Darren L. Anderson (darrenan) 5-Aug-1998

    Revision History:

        5-Aug-1998 darrenan

            Created.

--*/
#include "stdafx.h"
#include "md5.h"
#include "digest.h"


typedef enum { ALGO_MD5, ALGO_MD5_SESS } DIGEST_ALGORITHM;
typedef enum { QOP_NONE, QOP_AUTH, QOP_AUTH_INT } DIGEST_QOP;

static DIGEST_ALGORITHM WINAPI
AlgorithmFromString(
    LPCSTR  pszAlgorithm
    )
{
    DIGEST_ALGORITHM Algorithm;

    if(pszAlgorithm == NULL)
        Algorithm = ALGO_MD5;
    else if(_stricmp("MD5-sess", pszAlgorithm) == 0)
        Algorithm = ALGO_MD5_SESS;
    else if(_stricmp("MD5", pszAlgorithm) == 0)
        Algorithm = ALGO_MD5;
    else
        Algorithm = ALGO_MD5;

    return Algorithm;
}

static DIGEST_QOP WINAPI
QopFromString(
    LPCSTR  pszQOP
    )
{
    DIGEST_QOP  Qop;

    if(pszQOP == NULL)
        Qop = QOP_NONE;
    else if(strcmp("auth", pszQOP) == 0)
        Qop = QOP_AUTH;
    else if(strcmp("auth-int", pszQOP) == 0)
        Qop = QOP_AUTH_INT;
    else
        Qop = QOP_NONE;

    return Qop;
}

VOID WINAPI
ToHex(
    LPBYTE pSrc,
    UINT   cSrc,
    LPSTR  pDst
    )

/*++

Routine Description:

    Convert binary data to ASCII hex representation

Arguments:

    pSrc - binary data to convert
    cSrc - length of binary data
    pDst - buffer receiving ASCII representation of pSrc

Return Value:

    Nothing

--*/

{
#define TOHEX(a) ((a)>=10 ? 'a'+(a)-10 : '0'+(a))

    for ( UINT x = 0, y = 0 ; x < cSrc ; ++x )
    {
        UINT v;
        v = pSrc[x]>>4;
        pDst[y++] = (char)TOHEX( v );
        v = pSrc[x]&0x0f;
        pDst[y++] = (char)TOHEX( v );
    }
    pDst[y] = '\0';
}


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
    )

/*++

Routine Description:

    DigestFromCreds         Produces a hexadecimally encoded string containing the
                            Digest response given the arguments below.

Arguments:

    pszAlgorithm            The algorithm being used to calculate the response.
                            If NULL or "", assume "MD5".  Possible values are
                            "MD5" and "MD5-Sess".

    pszUsername             Member's passport ID.

    pszRealm                Realm, should be constant.
    
    pszPassword             Member's password.

    pszNonce                Nonce.    
    
    pszNonceCount           The Nonce Count.  MUST be NULL if pszQOP is NULL or "".
                            Otherwise, Nonce Count is REQUIRED.

    pszCNonce               The Client nonce.  May be NULL or "".
    
    pszQOP                  The Quality of Privacy.  May be NULL, "", "auth" or 
                            "auth-int".  If it's NULL or "" then RFC 2069 style
                            digest is being performed.

    pszMethod               HTTP method used in the request.  REQUIRED.

    pszURI                  Resource being requested.  REQUIRED.

    pszEntityDigest         Entity Digest.  May be NULL if qop="auth" or nothing.
                            REQUIRED if qop="auth-int".

    pszSessionKey           Session key returned to caller.  Session key is MD5(A1).

    pszResult               Destination buffer for result.  Should point to a buffer
                            of at least MIN_OUTPUT_BUF_LEN characters.
    
Return Value:

    S_OK                    Call was successful.


--*/

{
    HRESULT             hr;
    DIGEST_ALGORITHM    Algorithm;
    DIGEST_QOP          Qop;
    CHAR                achWork     [32768];
    UCHAR               achDigest   [ 16];
    CHAR                achHashOfA1 [DIGESTBUF_LEN];

    if(!pszUsername) pszUsername = "";
    if(!pszRealm) pszRealm = "";
    if(!pszPassword) pszPassword = "";
    if(!pszNonce) pszNonce = "";
    if(!pszCNonce) pszCNonce = "";
    if(!pszAlgorithm) pszAlgorithm = "";
    if(!pszNonceCount) pszNonceCount = "";
    if(!pszQOP) pszQOP = "";
    if(!pszMethod) pszMethod = "";
    if(!pszURI) pszURI = "";
    if(!pszEntityDigest) pszEntityDigest = "";

    //
    //  Detect the algorithm and QOP.
    //
    Algorithm = AlgorithmFromString(pszAlgorithm);
    Qop       = QopFromString(pszQOP);

    //  Compute the digest.

    //
    //  Build A1.
    //  For MD5 this is username@domain:realm:password
    //  For MD5-Sess this is MD5(username@domain:realm:password):nonce:cnonce
    //
    strcpy(achWork, pszUsername);
    strcat(achWork, ":");
    strcat(achWork, pszRealm);
    strcat(achWork, ":");
    strcat(achWork, pszPassword);

    if(Algorithm == ALGO_MD5_SESS)
    {
        //  Hash it.
        MD5((UCHAR*)achWork, strlen(achWork), achDigest);
        ToHex(achDigest, 16, achHashOfA1);

        strcpy(achWork, achHashOfA1);
        strcat(achWork, ":");
        strcat(achWork, pszNonce);
        strcat(achWork, ":");
        strcat(achWork, pszCNonce);
    }

    //  Hash it.
    MD5((UCHAR*)achWork, strlen(achWork), achDigest);
    ToHex(achDigest, 16, achHashOfA1);
    
    hr = DigestFromKey(
                pszAlgorithm,
                achHashOfA1,
                pszNonce,
                pszNonceCount,
                pszCNonce,
                pszQOP,
                pszMethod,
                pszURI,
                pszEntityDigest,
                pszResult
                );

    if(hr != S_OK)
        goto Cleanup;

    strcpy(pszSessionKey, achHashOfA1);

Cleanup:
    return hr;
}


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
    )

/*++

Routine Description:

    DigestFromCreds         Produces a hexadecimally encoded string containing the
                            Digest response given the arguments below.

Arguments:

    pszAlgorithm            The algorithm being used to calculate the response.
                            If NULL or "", assume "MD5".  Possible values are
                            "MD5" and "MD5-Sess".

    pszSessionKey           Pre-computed MD5(A1).

    pszNonce                Nonce.    
    
    pszNonceCount           The Nonce Count.  MUST be NULL if pszQOP is NULL or "".
                            Otherwise, Nonce Count is REQUIRED.

    pszCNonce               The Client nonce.  May be NULL or "".
    
    pszQOP                  The Quality of Privacy.  May be NULL, "", "auth" or 
                            "auth-int".  If it's NULL or "" then RFC 2069 style
                            digest is being performed.

    pszMethod               HTTP method used in the request.  REQUIRED.

    pszURI                  Resource being requested.  REQUIRED.

    pszEntityDigest         Entity Digest.  May be NULL if qop="auth" or nothing.
                            REQUIRED if qop="auth-int".

    pszResult               Destination buffer for result.  Should point to a buffer
                            of at least MIN_OUTPUT_BUF_LEN characters.
    
Return Value:

    S_OK                    Call was successful.
    E_POINTER               A required parameter was NULL.

--*/

{
    HRESULT             hr;
    DIGEST_ALGORITHM    Algorithm;
    DIGEST_QOP          Qop;
    CHAR                achWork     [32768];
    UCHAR               achDigest   [ 16];
    CHAR                achHashOut  [ 36];

    //
    //  Detect the algorithm and QOP.
    //

    Algorithm = AlgorithmFromString(pszAlgorithm);
    Qop       = QopFromString(pszQOP);

    //  Compute the digest.

    //
    //  Build A2
    //  For qop="auth" this is method:uri
    //  For qop="auth-int" this is method:uri:entitydigest
    //

    strcpy(achWork, pszMethod);
    strcat(achWork, ":");
    strcat(achWork, pszURI);

    if(Qop == QOP_AUTH_INT)
    {
        strcat(achWork, ":");
        strcat(achWork, pszEntityDigest);
    }

    //  Hash it.
    MD5((UCHAR*)achWork, strlen(achWork), achDigest);
    ToHex(achDigest, 16, achHashOut);

    //
    //  Compute final chunk.
    //  For qop="" this is MD5(key:nonce:MD5(A2))
    //  For qop="auth" or qop="auth-int" this is 
    //      MD5(key:nonce:nc:cnonce:qop:MD5(A2))
    //

    strcpy(achWork, pszSessionKey);
    strcat(achWork, ":");
    strcat(achWork, pszNonce);
    strcat(achWork, ":");

    if(Qop != QOP_NONE)
    {
        strcat(achWork, pszNonceCount);
        strcat(achWork, ":");
        strcat(achWork, pszCNonce);
        strcat(achWork, ":");
        strcat(achWork, pszQOP);
        strcat(achWork, ":");
    }
     
    strcat(achWork, achHashOut);
    
    //  Hash it.
    MD5((UCHAR*)achWork, strlen(achWork), achDigest);
    ToHex(achDigest, 16, pszResult);

    hr = S_OK;

    return hr;
}
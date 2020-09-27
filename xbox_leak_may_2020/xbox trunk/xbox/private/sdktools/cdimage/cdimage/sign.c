
#include "precomp.h"
#pragma hdrstop

LPSTR SignatureRpcEndpointName;
LPSTR SignatureRpcEndpointBindingString;

RPC_BINDING_HANDLE SignatureRpcBindingHandle;

LPSTR SignatureDescription;

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t cBytes) {
    return((void __RPC_FAR *) malloc(cBytes));
    }

void __RPC_USER MIDL_user_free(void __RPC_FAR * p) {
    free(p);
    }


BOOL
ConnectToSignatureServer(
    LPSTR LoggingText
    )
    {
    RPC_STATUS RpcStatus;
    PCHAR p, q;

    //
    //  Connect to RPC signature server now to make sure we have
    //  sufficient permissions before wasting time reading files
    //  and writing the image.
    //
    //  If SignatureRpcEndpointName contains a '[' character, assume
    //  the entire string is a fully composed RPC binding string (the
    //  user knows what he/she is doing).  If no '[' character, look
    //  for ':' character to split server name and endpoint name.  If
    //  no ':' exists, blow up; otherwise, expect string to be of form
    //  "ServerName:EndpointName" and create a valid RPC binding string
    //  from that.
    //

    q = strchr( SignatureRpcEndpointName, '[' );
    p = strchr( SignatureRpcEndpointName, ':' );

    if ( p == NULL ) {

        //
        //  Either form should have colon in it.
        //

        ErrorExit(
            0,
            "ERROR: RPC signature server and endpoint must be specified in the form...\r\n\r\n"
            "           \"-sServerName:EndPointName\"\r\n\r\n"
            "       ...or as a fully qualified RPC binding string of the form...\r\n\r\n"
            "           \"-sProtSeq:ServerName[EndPoint,Options]\r\n\r\n"
            );
        }

    if ( q != NULL ) {

        //
        //  Assume string is valid RPC binding string, so simply point
        //  the SignatureRpcEndpointBindingString to the input string.
        //

        SignatureRpcEndpointBindingString = SignatureRpcEndpointName;

        }

    else {

        //
        //  No '[' in string, so assume it is form "Server:Endpoint".
        //
        //  Allocate memory for full binding string.  The strlen of
        //  SignatureRpcEndpointName includes the colon which will be
        //  stripped, but we'll need that extra byte for the terminator.
        //

        SignatureRpcEndpointBindingString =
                MyAllocNeverFree(
                      strlen( SignatureRpcEndpointName )
                    + strlen( "ncacn_np:%s[\\\\pipe\\\\%s]" )
                    - strlen( "%s%s" )
                    );

        //
        //  Split SignatureRpcEndpointName into server name and endpoint
        //  name (endpoint name will then be referenced through p.
        //

        *p++ = 0;

        //
        //  If server name has leading backslashes, skip over them.
        //

        while ( *SignatureRpcEndpointName == '\\' )
            SignatureRpcEndpointName++;

        //
        //  If endpoint name has leading backslashes, skip over them.
        //

        while ( *p == '\\' )
            p++;

        //
        //  If endpoint name begins with "pipe" followed by backslash,
        //  skip over it and the trailing backslashes.
        //

        if ( _strnicmp( p, "pipe\\", 5 ) == 0 ) {

            p += 5;

            while ( *p == '\\' )
                p++;

            }

        sprintf(
            SignatureRpcEndpointBindingString,
            "ncacn_np:%s[\\\\pipe\\\\%s]",
            SignatureRpcEndpointName,
            p
            );

        }

    RpcStatus = RpcBindingFromStringBinding(
                    (PUCHAR)SignatureRpcEndpointBindingString,
                    &SignatureRpcBindingHandle
                    );

    if ( RpcStatus != RPC_S_OK ) {
        ErrorExit(
            RpcStatus,
            "ERROR: Failed to create RPC binding from RPC binding string specified\r\n"
            "       \"%s\"\r\n",
            SignatureRpcEndpointBindingString
            );
        }

    RpcStatus = RpcBindingSetAuthInfo(
                    SignatureRpcBindingHandle,
                    NULL,                           // server principal name
                    RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // encrypted packets
                    RPC_C_AUTHN_WINNT,              // NTLMSSP
                    NULL,                           // current user
                    0                               // not used by RPC_C_AUTHN_WINNT
                    );

    if ( RpcStatus != RPC_S_OK ) {
        ErrorExit(
            RpcStatus,
            "ERROR: Failed to establish authentication level for RPC binding\r\n"
            );
        }

    RpcTryExcept {

        BOOL Success;

        Success = RpcSignatureServerAuthenticate(
                      SignatureRpcBindingHandle,        // [in]
                      (PUCHAR) LoggingText              // [in,  string]
                      );

        if ( Success ) {
            RpcStatus = RPC_S_OK;
            }
        else {
            RpcStatus = GetLastError();
            if ( RpcStatus == RPC_S_OK ) {
                RpcStatus = RPC_S_CALL_FAILED;
                }
            }
        }
    RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
        RpcStatus = RpcExceptionCode();
        }
    RpcEndExcept;

    SetLastError( RpcStatus );

    return ( RpcStatus == RPC_S_OK );
    }


BOOL
GenerateSignatureFromHashValue(
    IN  PMD5_HASH HashValue,
    IN  LPSTR     LoggingText,
    OUT PDWORD    SignatureId,
    OUT PUCHAR    Signature
    )
    {
    RPC_STATUS RpcStatus = RPC_S_OK;
    BOOL       Success;

    RpcTryExcept {

        Success = RpcSignatureServerGenSignature(
                      SignatureRpcBindingHandle,        // [in]
                      (PUCHAR) LoggingText,             // [in,  string]
                      (PUCHAR) HashValue,               // [in,  size_is( HASH_SIZE )]
                      (PDWORD) SignatureId,             // [out]
                      (PUCHAR) Signature                // [out, size_is( SIGN_SIZE )]
                      );

        if ( Success ) {
            RpcStatus = RPC_S_OK;
            }
        else {
            RpcStatus = GetLastError();
            if ( RpcStatus == RPC_S_OK ) {
                RpcStatus = RPC_S_CALL_FAILED;
                }
            }
        }
    RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
        RpcStatus = RpcExceptionCode();
        }
    RpcEndExcept;

    SetLastError( RpcStatus );

    return ( RpcStatus == RPC_S_OK );
    }





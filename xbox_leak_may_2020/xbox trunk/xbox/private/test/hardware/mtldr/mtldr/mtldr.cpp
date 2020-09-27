/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    mtldr.cpp

Abstract:

    This is a main module of manufacturing test boot-loader (mtldr). Mtldr
    is used for downloading external test application, which is an unsigned,
    uncertified, from a test server and execute the test application.

    Binary format of test application is regular Win32 portable executable (PE)
    format created by Xbox software development kit (XDK). However only .EXE
    will be loaded by the mtldr, we don't support loading .DLL format because
    XDK won't produce .DLL image

--*/

#include "mtldrp.h"

EXTERN_C
DWORD
WINAPI
MtLdrDownloadFileToMemory(
    IN  LPCSTR pszDownloadUrl,
    OUT PVOID  *Buffer
    );

//
// The URL is hard-coded to "http://xboxostestsql/demo.exe"
//

const CHAR *HttpServer  = "schanbai0";
const CHAR *HttpFileUrl = "/demo.exe";

ULONG
HttpGetServerAddress(
    VOID
    )
/*++

Routine Description:

    This routine looks up the address of global server name specified by
    global variable "HttpServer" above.

Arguments:

    None

Return Value:

    IPv4 address of the server or INADDR_NONE, call WSAGetLastError to
    see the error information.

--*/
{
    HOSTENT *h = gethostbyname( HttpServer );

    if ( h ) {
        return *(PULONG)h->h_addr_list[0];
    }

    return INADDR_NONE;
}

BOOL
HttpSendRequest(
    IN  PCSTR  Request,
    OUT PSTR   Response,
    IN  SIZE_T ResponseSize
    )
/*++

Routine Description:

    Helper routine that sends request to HTTP server and receives response
    back.  The HTTP response code will NOT be checked on behalf of the caller.

Arguments:

    Request - HTTP request line, including headers

    Response - Buffer to receive HTTP response

    ResponseSize - Size of the response buffer

Return Value:

    TRUE if operation success. FALSE if error occurred, call WSAGetLastError
    for more information.

--*/
{
    INT err;
    SOCKET s;
    ULONG ServerIP;
    SOCKADDR_IN si;
    BOOL b = FALSE;
    SIZE_T BytesReceived;

    ServerIP = HttpGetServerAddress();

    if ( ServerIP == INADDR_NONE ) {
        return FALSE;
    }

    s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( s == INVALID_SOCKET ) {
        return FALSE;
    }

    ZeroMemory( &si, sizeof(si) );
    si.sin_family = AF_INET;
    si.sin_port = htons( 80 );
    si.sin_addr.s_addr = ServerIP;

    err = connect( s, (SOCKADDR *)&si, sizeof(si) );

    if ( err == SOCKET_ERROR ) {
        goto cleanup;
    }

    //
    // Assuming size of data to be sent is small enough to send successfully
    // in one call
    //

    err = send( s, Request, strlen(Request), 0 );

    if ( err == SOCKET_ERROR ) {
        goto cleanup;
    }

    //
    // Loop until we couldn't get any more data
    //

    BytesReceived = 0;
    do {
        err = recv( s, Response+BytesReceived, ResponseSize-BytesReceived, 0 );
        BytesReceived += err;
    } while ( err != SOCKET_ERROR && err != 0 && BytesReceived < ResponseSize);

    if ( err == SOCKET_ERROR) {
        goto cleanup;
    }

    if ( err != 0 ) {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        goto cleanup;
    }

    b = TRUE;

cleanup:
    closesocket( s );
    return b;
}

LPVOID
MtldrDownloadFileToMemory(
    VOID
    )
/*++

Routine Description:

    This routine connects to the http server specified by global HttpServer
    variable and downloads the content of the image specified by global
    HttpFileUrl variable to memory

Arguments:

    None

Return Value:

    Pointer to memory that contains downloaded image or NULL.  Call
    GetLastError to get more information on error in case the routine
    returns NULL

--*/
{
    BOOL b;
    INT len;
    PCHAR ImageBuffer = NULL;
    PCHAR p, Buffer = NULL;
    SIZE_T FileSize, Size;
    const SIZE_T BufferLen = 4096;

    Buffer = (PCHAR)LocalAlloc( LPTR, BufferLen );

    if ( !Buffer ) {
        return NULL;
    }

    len = sprintf( Buffer, "HEAD %s HTTP/1.0\r\n\r\n", HttpFileUrl );
    b = HttpSendRequest( Buffer, Buffer, BufferLen );

    if ( !b ) {
        goto cleanup;
    }

    p = strstr( Buffer, "200 OK" );

    if ( p ) {
        p = strstr( Buffer, "Content-Length:" );
    }

    if ( !p ) {
        SetLastError( ERROR_FILE_NOT_FOUND );
        goto cleanup;
    }

    p += sizeof("Content-Length:") - 1;
    FileSize = atol( p );

    if ( !FileSize ) {
        SetLastError( WSAEINVAL );
        goto cleanup;
    }

    Size = FileSize + 4096;
    ImageBuffer = (PCHAR)LocalAlloc( LMEM_FIXED, Size );

    if ( !ImageBuffer ) {
        goto cleanup;
    }

    len = sprintf( Buffer, "GET %s HTTP/1.0\r\n\r\n", HttpFileUrl );
    b = HttpSendRequest( Buffer, ImageBuffer, Size );

    if ( b ) {

        p = strstr( ImageBuffer, "\r\n\r\n" );

        if ( p ) {

            p += sizeof("\r\n\r\n") - 1;
            if ( *(PWORD)p != IMAGE_DOS_SIGNATURE ) {
                SetLastError( ERROR_BAD_EXE_FORMAT );
            } else {
                RtlMoveMemory( ImageBuffer, p, FileSize );
                goto cleanup;
            }
        }
    }

    LocalFree( ImageBuffer );
    ImageBuffer = NULL;

cleanup:
    LocalFree( Buffer );
    return ImageBuffer;
}

int __cdecl main( void )
{
    INT err;
    DWORD Error;
    WSADATA wsa;
    PVOID ImageBuffer;

    //
    // Load and initialize Xnet/socket library
    //

    err = XnetInitialize( NULL, TRUE );

    if ( err != NO_ERROR ) {
        MtDbgPrint(( "MTLDR: XnetInitialize failed (%lu)\n", err ));
        return err;
    }

    ZeroMemory( &wsa, sizeof(wsa) );

    err = WSAStartup( MAKEWORD(2, 2), &wsa );

    if ( err != NO_ERROR ) {
        MtDbgPrint(( "MTLDR: WSAStartup failed (%lu)\n", err ));
        XnetCleanup();
        return err;
    }

    ImageBuffer = MtldrDownloadFileToMemory();

    //
    // Unload socket library so that the application can use it
    //

    WSACleanup();
    XnetCleanup();

    if ( !ImageBuffer ) {
        MtDbgPrint(( "MTLDR: unable to download http://%s%s (%lu)\n",
                     HttpServer, HttpFileUrl, GetLastError() ));
    } else {
        //
        // Let the loader fix up and snap import table
        //
        Error = LdrLoadSystemImage( ImageBuffer, HttpFileUrl+1 );

        if ( Error != ERROR_SUCCESS ) {
            MtDbgPrint(( "MTLDR: unable to load system image (%lu)\n", Error ));
        }
    }
}

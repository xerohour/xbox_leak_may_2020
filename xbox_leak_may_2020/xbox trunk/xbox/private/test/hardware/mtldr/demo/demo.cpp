/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    demo.cpp

Abstract:

    Sample manufacturing test application. This is a regular PE image created
    by Xbox development kit. Note that we will use only PE images, the MTLDR
    will not load XBE image.

--*/

#include "demop.h"

void __cdecl main( void )
/*++

Routine Description:

    This is a main entry point of manufacturing test application.

Arguments:

    None

Return Value:

    None

--*/
{
    WSADATA wsa;

    XnetInitialize( NULL, TRUE );
    WSAStartup( MAKEWORD(2, 2), &wsa );
    OutputDebugString( TEXT("hello world!\n") );
    WSACleanup();
    XnetCleanup();

    Sleep( INFINITE );
}
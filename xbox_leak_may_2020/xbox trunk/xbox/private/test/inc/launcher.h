/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    launcher.h

Abstract:

    Public launcher header

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-31-2000  Created

*/
#ifndef _LAUNCHER_H_
#define _LAUNCHER_H_

#include <winsockx.h>


/*

Routine Description:

    SockPrint

    Formatted print function that sends the output through a connected SOCKET.

Arguments:

    SOCKET sock - connected socket
    char* format - printf style format specifier
    ... - arguments specified by the format specifier

Return Value:

    int - number of bytes sent out

Notes:

*/
extern int SockPrint(SOCKET sock, char* format, ...);

#endif //_LAUNCHER_H_
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sgdata.h

Abstract:

  This module defines the common data

Author:

  Steven Kehrli (steveke) 5-Feb-2002

------------------------------------------------------------------------------*/

#pragma once



namespace SGDataNamespace {

// Buffers

#define BUFFER_10_LEN   10
#define BUFFER_UDP_LEN  1430
#define BUFFER_TCP_LEN  1418

typedef struct _TEST_OBJECT {
    HANDLE   hMemObject;                 // Handle to the memory object
    SOCKET   sSessionSocket;             // Specifies the session socket descriptor
    SOCKET   sUDPSocket;                 // Specifies the UDP socket descriptor
    SOCKET   sTCPSocket;                 // Specifies the TCP socket descriptor
} TEST_OBJECT, *PTEST_OBJECT;



// Functions

BOOL
SGDataTest(
    IN HANDLE  hLog
);

} // namespace SGDataNamespace

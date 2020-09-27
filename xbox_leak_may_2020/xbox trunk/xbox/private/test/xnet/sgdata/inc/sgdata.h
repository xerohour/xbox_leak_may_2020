/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sgdata.h

Abstract:

  This module defines the common data for the sgdata client and server

Author:

  Steven Kehrli (steveke) 24-Jan-2002

------------------------------------------------------------------------------*/

#pragma once



namespace SGDataNamespace {



// Messages

#define SGDATA_ENUM_MSG          1
#define SGDATA_ENUM_REPLY        2
#define SGDATA_CONNECT_MSG       3
#define SGDATA_CONNECT_SUCCESS   4
#define SGDATA_CONNECT_FAILED    5
#define SGDATA_DISCONNECT_MSG    6
#define SGDATA_DISCONNECT_REPLY  7

typedef struct _SGDATA_MESSAGE {
    DWORD  dwMessageId;  // Specifies the message id
    DWORD  dwErrorCode;  // Specifies the error code
} SGDATA_MESSAGE, *PSGDATA_MESSAGE;

} // namespace SGDataNamespace

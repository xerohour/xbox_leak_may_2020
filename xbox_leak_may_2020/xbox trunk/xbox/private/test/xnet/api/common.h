/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  common.h

Abstract:

  This module contains the common definitions

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#pragma once

#include "precomp.h"

namespace XNetAPINamespace {

// Netsync flags

#define VS_NT   1
#define VS_XBOX 2



// Misc

#define AF_UNIX          1
#define MAX_CONNECTIONS  5



// Socket flags

#define SOCKET_INT_MIN         0x00000001
#define SOCKET_NEG_ONE         0x00000002
#define SOCKET_ZERO            0x00000004
#define SOCKET_INT_MAX         0x00000008
#define SOCKET_INVALID_SOCKET  0x00000010
#define SOCKET_TCP             0x00000020
#define SOCKET_UDP             0x00000040
#define SOCKET_CLOSED          0x00000080
#define SOCKET_BROADCAST       0x00000100



// Buffers

#define BUFFER_10_LEN          10
#define BUFFER_TCPSEGMENT_LEN  1418
#define BUFFER_TCPLARGE_LEN    8192
#define BUFFER_UDPLARGE_LEN    1430
#define BUFFER_LARGE_LEN       8192


    
// Sleep times

#define SLEEP_ZERO_TIME        250
#define SLEEP_LOW_TIME         2200
#define SLEEP_MIDLOW_TIME      2400
#define SLEEP_MEAN_TIME        2500
#define SLEEP_MIDHIGH_TIME     2600
#define SLEEP_HIGH_TIME        2800

} // namespace XNetAPINamespace

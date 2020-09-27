//-----------------------------------------------------------------------------
// File: XbVoice.cpp
//
// Desc: Shortcut macros and helper functions for the Xbox voice samples
//       Requires linking with XVOICE[D].LIB
//
// Hist: 10.21.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XBVoice.h"




//-----------------------------------------------------------------------------
// Name: XBVoice_HasDevice()
// Desc: TRUE if an Xbox Communicator is currently available
//-----------------------------------------------------------------------------
BOOL XBVoice_HasDevice()
{
    DWORD dwConnectedMicrophones = XGetDevices( XDEVICE_TYPE_VOICE_MICROPHONE );
    DWORD dwConnectedHeadphones = XGetDevices( XDEVICE_TYPE_VOICE_HEADPHONE );

    // Voice is available if there's at least one mike and one headphone
    return( dwConnectedMicrophones >= 1 &&
            dwConnectedHeadphones  >= 1 );
}





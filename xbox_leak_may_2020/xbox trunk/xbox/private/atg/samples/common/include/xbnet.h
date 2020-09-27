//-----------------------------------------------------------------------------
// File: XbNet.h
//
// Desc: Shortcut macros and helper functions for the Xbox networking samples
//       Requires linking with XNET[D][S].LIB or XONLINE[D][S].LIB
//
// Hist: 09.06.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBNET_H
#define XBNET_H

#include <xtl.h>
#include "XBStopWatch.h"
#include <xonline.h>



//-----------------------------------------------------------------------------
// Name: XBNet_Init() / XBNet_Cleanup()
// Desc: Performs all required initialization/shutdown for  
//       network play
//-----------------------------------------------------------------------------
HRESULT XBNet_Init( BYTE cfgFlags = 0 );
HRESULT XBNet_Cleanup();




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineInit() / XBNet_OnlineCleanup()
// Desc: Performs all required initialization/shutdown for online and 
//       network play
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineInit( BYTE cfgFlags = 0, 
						  PXONLINE_STARTUP_PARAMS pxosp = NULL );
HRESULT XBNet_OnlineCleanup();




//-----------------------------------------------------------------------------
// Name: class CXBNetLink
// Desc: Xbox network link
//-----------------------------------------------------------------------------
class CXBNetLink
{
    mutable CXBStopWatch m_LinkStatusTimer;

public:

    CXBNetLink();
    BOOL IsActive() const;
};




#endif // XBNET_H


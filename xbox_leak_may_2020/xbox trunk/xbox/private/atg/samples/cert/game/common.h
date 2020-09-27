//-----------------------------------------------------------------------------
// File: Common.h
//
// Desc: Include files, pragmas, macros, types and constants used across all 
//       source files
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_COMMON_H
#define TECH_CERT_GAME_COMMON_H

#if defined(_XBOX)
    #include <xtl.h>
    #include <XBUtil.h>
#else
    #include <Windows.h>
    #include <D3D8.h>
    #include <D3DX8.h>
    #include <DXUtil.h>
#endif

#include <cassert>




//-----------------------------------------------------------------------------
// #pragmas
//-----------------------------------------------------------------------------
#pragma warning( disable: 4714 )    // ignore function not inlined
#pragma warning( disable: 4786 )    // ignore warning about identifier truncation




//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define USED( x )   static_cast<VOID>( x )  // avoid warning about unused var




#endif // TECH_CERT_GAME_COMMON_H

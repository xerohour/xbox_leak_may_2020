/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       globals.cpp
 *  Content:    Contains the declarations for all global variables that
 *              are externally visible to titles (and so can be linked
 *              to directly)
 *
 ***************************************************************************/
 
#include "precomp.hpp"

extern "C"
{

//------------------------------------------------------------------------------
// Externally Visible Globals - Debug and Retail

// Current state as set by SetRenderState:

DWORD D3D__RenderState[D3DRS_MAX];

// Current state as set by SetTextureStageState:

DWORD D3D__TextureState[D3DTSS_MAXSTAGES][D3DTSS_MAX];

// Dirty flags for deferred render and texture state:

DWORD D3D__DirtyFlags;

// This points to the data of the currently selected index buffer:

WORD* D3D__IndexData;

// Set D3D__NullHardware to TRUE to enable infinitely fast hardware (so fast 
// you can't even see the results).  

BOOL D3D__NullHardware;

// Used for setting up a fake pControlDma for D3D__NullHardware:

DWORD g_NullHardwareGetPut[2];

// Push-buffer state:

volatile DWORD *D3D__GpuReg; 

// For the tvtest app.

DWORD D3D__AvInfo;

//------------------------------------------------------------------------------
// Externally Visible Globals - Debug Builds Only

#if DBG

// Set D3D__SingleStepPusher to TRUE to have the pusher pause after each 
// EndPush until the push buffer is empty.

BOOL D3D__SingleStepPusher;

// Timeout value used when checking for possible deadlocks

DWORD D3D__DeadlockTimeOutVal = 10000;

// Set D3D__Parser to TRUE before the device is created in order to enable the
// dumper to parse and track all register writes to the hardware.  Marked as 
// 'extern "C"' so that stress or whatever can enable it.

BOOL D3D__Parser;

// Start push-buffer point for StartBracket:

PPUSH g_StartPut;

#endif

} // end extern "C"

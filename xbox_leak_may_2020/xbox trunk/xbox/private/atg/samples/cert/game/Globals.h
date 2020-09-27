//-----------------------------------------------------------------------------
// File: Globals.h
//
// Desc: Global objects
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_GLOBALS_H
#define TECH_CERT_GAME_GLOBALS_H

#include "Common.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <vector>
#pragma warning( pop )




//-----------------------------------------------------------------------------
// Name: struct DebugLine
// Desc: Debugging information
//-----------------------------------------------------------------------------
struct DebugLine
{
	D3DVECTOR pos1;
	D3DCOLOR  color1;
	D3DVECTOR pos2;
	D3DCOLOR  color2;
};




//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class PhysicsSystem;




//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------
typedef std::vector< DebugLine > DebugLineList;




//-----------------------------------------------------------------------------
// Global objects
//-----------------------------------------------------------------------------
extern LPDIRECT3DDEVICE8 g_pd3dDevice;
extern BOOL              g_bVisualizeStrips;
extern BOOL              g_bWireframe;
extern BOOL              g_bShowNormals;
extern BOOL              g_bShowBasisVectors;
extern BOOL              g_bDrawDebugLines;
extern DOUBLE            g_PhysicsTime;
extern DOUBLE            g_CollisionTime;
extern DOUBLE            g_TransformTime;
extern DOUBLE            g_ClipTime;
extern DebugLineList     g_DebugLines;




#endif // TECH_CERT_GAME_GLOBALS_H

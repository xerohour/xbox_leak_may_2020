//-----------------------------------------------------------------------------
// File: Globals.cpp
//
// Desc: Global objects
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Globals.h"




//-----------------------------------------------------------------------------
// Global objects
//-----------------------------------------------------------------------------
BOOL              g_bVisualizeStrips  = FALSE;          // See Mesh.cpp
BOOL              g_bWireframe        = FALSE;          // See Mesh.cpp
BOOL              g_bShowNormals      = FALSE;          // See Mesh.cpp
BOOL              g_bShowBasisVectors = FALSE;          // See Mesh.cpp
BOOL              g_bDrawDebugLines   = TRUE;
DOUBLE            g_PhysicsTime       = 0.0;            // See PhysicsSystem.cpp
DOUBLE            g_CollisionTime     = 0.0;            // See PhysicsSystem.cpp
DOUBLE            g_TransformTime     = 0.0;            // See Polyhedron.cpp
DOUBLE            g_ClipTime          = 0.0;            // See Polyhedron.cpp
DebugLineList     g_DebugLines;


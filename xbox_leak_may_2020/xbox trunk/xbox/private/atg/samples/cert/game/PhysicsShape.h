//-----------------------------------------------------------------------------
// FILE: PhysicsShape.h
//
// Desc: Collision primitive base class for all objects.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef TECH_CERT_GAME_PHYSICS_SHAPE_H
#define TECH_CERT_GAME_PHYSICS_SHAPE_H

#include "Math3d.h"


// Structure conaining the information necessary for resolving an intersection.
struct IntersectionInfo
{
	bool bAtowardsB;				// a towards b if true, else b towards a.
	float fPenetrationDepth;
	Vector3 v3CenterOfIntersection;
	Vector3 v3ExtractionDirection;
};


#endif // TECH_CERT_GAME_PHYSICS_SHAPE_H

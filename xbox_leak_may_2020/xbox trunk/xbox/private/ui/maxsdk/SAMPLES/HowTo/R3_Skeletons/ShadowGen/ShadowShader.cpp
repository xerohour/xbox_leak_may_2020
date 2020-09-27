/*===========================================================================*\
 | 
 |  FILE:	ShadowShader.cpp
 |			Skeleton project and code for a Shadow Generator
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 10-3-99
 | 
\*===========================================================================*/

#include "ShadowGen.h"


/*===========================================================================*\
 |	The Update function is called each frame, for the shadow generator
 |	to update itself, generate a shadow buffer, etc
\*===========================================================================*/

int SkeletonGen::Update(TimeValue t,const RendContext& rendCntxt,RenderGlobalContext *rgc,
		Matrix3& ltToWorld,float aspect,float param,float clipDist)
{
	lightToWorld  = ltToWorld;
	worldToLight = Inverse(lightToWorld);

	return 1;
}




/*===========================================================================*\
 |	This shadow generator now must calculate whether or not the current
 |	surface point is in shadow (return 0.0f) or not
\*===========================================================================*/

float SkeletonGen::Sample(ShadeContext &sc, Point3 &norm, Color& color)
{ 
	Point3 ptSurf = sc.P();
	Point3 ptNorm = norm;
	Point3 plt = camToLight*ptSurf;

	return 1.0f;
}

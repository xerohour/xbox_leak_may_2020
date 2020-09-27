/*===========================================================================*\
 | 
 |  FILE:	TexShade.cpp
 |			Skeleton project and code for a Texture Map
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 11-3-99
 | 
\*===========================================================================*/

#include "Texmap.h"


static AColor white(1.0f,1.0f,1.0f,1.0f);


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/


AColor SkeletonTexmap::EvalColor(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	AColor c0 = subTex[0]&&mapOn[0]? subTex[0]->EvalColor(sc): white;

	return c0*=simple;
}

float SkeletonTexmap::EvalMono(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	float c0 = subTex[0]&&mapOn[0]? subTex[0]->EvalMono(sc): 1.0f;

	return c0*=simple;
}

Point3 SkeletonTexmap::EvalNormalPerturb(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	Point3 p0  = subTex[0]&&mapOn[0]? subTex[0]->EvalNormalPerturb(sc): Point3(0.0f,0.0f,0.0f);

	return p0*=simple;
}

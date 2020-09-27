/*===========================================================================*\
 | 
 |  FILE:	MtlShade.cpp
 |			Skeleton project and code for a Material
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 11-3-99
 | 
\*===========================================================================*/

#include "Material.h"



/*===========================================================================*\
 |	Determine the characteristics of the material
\*===========================================================================*/

void SkeletonMaterial::SetAmbient(Color c, TimeValue t) {}		
void SkeletonMaterial::SetDiffuse(Color c, TimeValue t) {}		
void SkeletonMaterial::SetSpecular(Color c, TimeValue t) {}
void SkeletonMaterial::SetShininess(float v, TimeValue t) {}
				
Color SkeletonMaterial::GetAmbient(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetAmbient(mtlNum,backFace):Color(0,0,0);
}

Color SkeletonMaterial::GetDiffuse(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetDiffuse(mtlNum,backFace):Color(0,0,0);
}

Color SkeletonMaterial::GetSpecular(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetSpecular(mtlNum,backFace):Color(0,0,0);
}

float SkeletonMaterial::GetXParency(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetXParency(mtlNum,backFace):Color(0,0,0);
}

float SkeletonMaterial::GetShininess(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetShininess(mtlNum,backFace):Color(0,0,0);
}

float SkeletonMaterial::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->GetShinStr(mtlNum,backFace):Color(0,0,0);
}

float SkeletonMaterial::WireSize(int mtlNum, BOOL backFace)
{
	return submtl[0]?submtl[0]->WireSize(mtlNum,backFace):Color(0,0,0);
}

		
/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void SkeletonMaterial::Shade(ShadeContext& sc) 
{
	Mtl *sm1 = mapOn[0]?submtl[0]:NULL;
	if (gbufID) sc.SetGBufferID(gbufID);

	if(sm1) sm1->Shade(sc);

	sc.out.c*=simple;
}

float SkeletonMaterial::EvalDisplacement(ShadeContext& sc)
{
	Mtl *sm1 = mapOn[0]?submtl[0]:NULL;
	return (sm1)?sm1->EvalDisplacement(sc):0.0f;
}

Interval SkeletonMaterial::DisplacementValidity(TimeValue t)
{
	Mtl *sm1 = mapOn[0]?submtl[0]:NULL;

	Interval iv; iv.SetInfinite();
	if(sm1) iv &= sm1->DisplacementValidity(t);

	return iv;	
}
	

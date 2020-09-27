/*===========================================================================*\
 | 
 |  FILE:	GetSet.cpp
 |			Skeleton project and code for a Material Shader
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-1-99
 | 
\*===========================================================================*/


#include "Shader.h"



/*===========================================================================*\
 |	Shader specific transaction
\*===========================================================================*/

void SkeletonShader::SetBrightness(float v, TimeValue t)
{ 
	brightness= v; 
	pblock->SetValue( sk_brightness, t, v); 
}

float SkeletonShader::GetBrightness(int mtlNum, BOOL backFace)
{ 
	return brightness; 
}	

float SkeletonShader::GetBrightness( TimeValue t)
{
	return pblock->GetFloat(sk_brightness,t);  
}		

void SkeletonShader::SetGlossiness(float v, TimeValue t)
{ 
}

float SkeletonShader::GetGlossiness(int mtlNum, BOOL backFace)
{ 
	return 40.0f; 
}	

float SkeletonShader::GetGlossiness( TimeValue t)
{
	return 40.0f;  
}		


/*===========================================================================*\
 |	Diffuse Transactions
\*===========================================================================*/

void SkeletonShader::SetDiffuseClr(Color c, TimeValue t)		
{ 
	diffuse = c; pblock->SetValue( sk_diffuse, t, c); 
}

Color SkeletonShader::GetDiffuseClr(int mtlNum, BOOL backFace)
{ 
	return diffuse;
}

Color SkeletonShader::GetDiffuseClr(TimeValue t)
{ 
	return pblock->GetColor(sk_diffuse,t); 
}


/*===========================================================================*\
 |	Ambient Transactions
\*===========================================================================*/

void SkeletonShader::SetAmbientClr(Color c, TimeValue t)
{
}

Color SkeletonShader::GetAmbientClr(int mtlNum, BOOL backFace)
{ 
	return diffuse * 0.5f;
}

Color SkeletonShader::GetAmbientClr(TimeValue t)
{ 
	return diffuse * 0.5f; 
}


/*===========================================================================*\
 |	Specular Transactions
\*===========================================================================*/

void SkeletonShader::SetSpecularClr(Color c, TimeValue t)
{
}

void SkeletonShader::SetSpecularLevel(float v, TimeValue t)
{
}
		
Color SkeletonShader::GetSpecularClr(int mtlNum, BOOL backFace)
{ 
	return Color(0.9f,0.9f,0.9f); 
}

float SkeletonShader::GetSpecularLevel(int mtlNum, BOOL backFace)
{ 
	return 1.0f; 
}

Color SkeletonShader::GetSpecularClr(TimeValue t)
{ 
	return Color(0.9f,0.9f,0.9f);
}

float SkeletonShader::GetSpecularLevel(TimeValue t)
{ 
	return 1.0f; 
}


/*===========================================================================*\
 |	SelfIllum Transactions
\*===========================================================================*/

void SkeletonShader::SetSelfIllum(float v, TimeValue t)	
{
}

float SkeletonShader::GetSelfIllum(int mtlNum, BOOL backFace)
{ 
	return 0.0f; 
}

void SkeletonShader::SetSelfIllumClrOn( BOOL on )
{
}

BOOL SkeletonShader::IsSelfIllumClrOn()
{ 
	return FALSE; 
}

BOOL SkeletonShader::IsSelfIllumClrOn(int mtlNum, BOOL backFace)
{ 
	return FALSE; 
}

void SkeletonShader::SetSelfIllumClr(Color c, TimeValue t)
{
}

Color SkeletonShader::GetSelfIllumClr(int mtlNum, BOOL backFace)
{ 
	return Color(0,0,0); 
}

float SkeletonShader::GetSelfIllum(TimeValue t)
{ 
	return 0.0f;
}		

Color SkeletonShader::GetSelfIllumClr(TimeValue t)
{ 
	return Color(0,0,0);
}		


/*===========================================================================*\
 |	Soften Transactions
\*===========================================================================*/

void SkeletonShader::SetSoftenLevel(float v, TimeValue t)
{
}

float SkeletonShader::GetSoftenLevel(int mtlNum, BOOL backFace)
{ 
	return 0.0f; 
}

float SkeletonShader::GetSoftenLevel(TimeValue t)
{ 
	return  0.0f; 
}

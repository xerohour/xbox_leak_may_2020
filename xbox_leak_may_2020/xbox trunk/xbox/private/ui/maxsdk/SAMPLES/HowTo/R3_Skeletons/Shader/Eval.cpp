/*===========================================================================*\
 | 
 |  FILE:	Eval.cpp
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
 |	Actually shade the surface
\*===========================================================================*/

void SkeletonShader::GetIllumParams( ShadeContext &sc, IllumParams &ip )
{
	ip.stdParams = SupportStdParams();
	ip.channels[S_DI] = diffuse;
	ip.channels[S_BR].r = brightness;
}

void SkeletonShader::Illum(ShadeContext &sc, IllumParams &ip)
{
	LightDesc *l;
	Color lightCol;

	// Get our parameters our of the channels
	Color Cd		= ip.channels[ S_DI ];
	float bright	= ip.channels[ S_BR ].r;

	// Very similar to Blinn/Phong method here - just using 
	//	our brightness parameter to tie a couple of parameters together

	BOOL is_shiny	= (bright > 0.0f) ? 1:0; 
	double phExp	= pow(2.0, bright * 10.0) * 4.0; 

	for (int i=0; i<sc.nLights; i++) {
		l = sc.Light(i);
		register float NL, diffCoef;
		Point3 L;
		if (l->Illuminate(sc,ip.N,lightCol,L,NL,diffCoef)) {
			if (l->ambientOnly) {
				ip.ambIllumOut += lightCol;
				continue;
				}
			if (NL<=0.0f) 
				continue;

			if (l->affectDiffuse)
				ip.diffIllumOut += diffCoef * lightCol;

			if (is_shiny&&l->affectSpecular) {
				Point3 H = Normalize(L-ip.V);
				float c = DotProd(ip.N,H);	 
				if (c>0.0f) {
					c = (float)pow((double)c, phExp); 
					ip.specIllumOut += c * bright * lightCol;
					}
				}
 			}
		}



	// now we can multiply by the clrs
	ip.ambIllumOut *= Cd * 0.5f; 
	ip.diffIllumOut *= Cd;
}


void SkeletonShader::AffectReflection(ShadeContext &sc, IllumParams &ip, Color &rcol) 
{ 
}

// Called to combine the various color and shading components
void SkeletonShader::CombineComponents( ShadeContext &sc, IllumParams& ip )
{ 
	ip.finalC = ip.finalOpac * (ip.ambIllumOut + ip.diffIllumOut  + ip.selfIllumOut)
			+ ip.specIllumOut + ip.reflIllumOut + ip.transIllumOut; 
}

// Display our hilite curve in the UI, at point 'x'
float SkeletonShader::EvalHiliteCurve(float x) 
{
	double phExp	= pow(2.0, brightness * 10.0) * 4.0; 
	return 1.0f*(float)pow((double)cos(x*PI), phExp );  
}


BOOL SkeletonShader::IsMetal() 
{ 
	return FALSE; 
}
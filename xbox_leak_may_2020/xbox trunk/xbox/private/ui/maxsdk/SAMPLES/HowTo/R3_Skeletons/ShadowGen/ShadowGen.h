/*===========================================================================*\
 | 
 |  FILE:	ShadowGen.h
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

#ifndef __SHADOWSKEL__H
#define __SHADOWSKEL__H

#include "max.h"
#include "render.h"
#include "shadgen.h"
#include "genlight.h"
#include "iparamm2.h"
#include "notify.h"
#include <bmmlib.h>
#include "resource.h"
#include "shadbuf.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SHADOW_CLASSID		Class_ID(0x4f02540e, 0x4c000797)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonShdwGenDesc();


// Paramblock2 name
enum { shd_params, }; 
// Paramblock2 parameter list
enum { shd_simple_param, };


class SkeletonShadow;
/*===========================================================================*\
 |	SkeletonGen class defn.
\*===========================================================================*/

class SkeletonGen: public ShadowGenerator 
{ 
public:
	SkeletonShadow *theShad;

	// Various transforms
	Matrix3 lightToWorld;
	Matrix3 worldToLight;
	Matrix3 camToLight;

	LightObject *light;
	ObjLightDesc *ltDesc;



	SkeletonGen(SkeletonShadow *shad, LightObject *l,  ObjLightDesc *ld, ULONG flags) { 
		theShad = shad;
		light = l;
		ltDesc = ld;
		} 

	
	SkeletonGen() { } 
	~SkeletonGen() { FreeBuffer();  }


	// Update the shadow generator per frame
	int Update(TimeValue t,const RendContext& rendCntxt,RenderGlobalContext *rgc,
		Matrix3& lightToWorld,float aspect,float param,float clipDist);

	int UpdateViewDepParams(const Matrix3& worldToCam) { 
		Matrix3 camToWorld = Inverse(worldToCam);
		camToLight = camToWorld*worldToLight;
		return 1;
		}


	// Clean up methods
	void FreeBuffer() {}
	void DeleteThis() {delete this; }


	// Perform the shadow sampling
	float Sample(ShadeContext &sc, Point3 &norm, Color& color);
};

class SkeletonParam: public ShadowParamDlg 
{
public:
	SkeletonShadow *theShad;
	Interface *ip;
	IParamMap2 *pmap;

	// Constructor
	SkeletonParam(SkeletonShadow *shad, Interface *iface);
	~SkeletonParam();

	void DeleteThis() { delete this; }
};


class SkeletonShadow: public ShadowType 
{
	public:
		IParamBlock2 *pblock;
		SkeletonParam *theParam;

		SkeletonShadow();
		~SkeletonShadow() {
			if (theParam) theParam->theShad = NULL;
			}


		// Create the shadow system's UI
		ShadowParamDlg *CreateShadowParamDlg(Interface *ip) { 
			theParam = new SkeletonParam(this, ip); return theParam;
			}

		// Create a shadow generator instance - only exists during a render
		ShadowGenerator* CreateShadowGenerator(LightObject *l,  ObjLightDesc *ld, ULONG flags) {
			return new SkeletonGen(this,l,ld,flags);
			}

		BOOL SupportStdMapInterface() { return FALSE; }
		
		SClass_ID SuperClassID() { return SHADOW_TYPE_CLASS_ID;}
		Class_ID ClassID() { return SHADOW_CLASSID;}

		int NumSubs() {return 1;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message);

		void DeleteThis() { delete this; }
};

#endif
/*===========================================================================*\
 | 
 |  FILE:	SimpMod.h
 |			Skeleton project and code for a Simple Modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/


#ifndef __SMPSKEL__H
#define __SMPSKEL__H

#include "max.h"
#include "simpmod.h"
#include "simpobj.h"
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SIMPM_CLASSID		Class_ID(0x2d587cd4, 0x38b4754e)
#define	SMWSM_CLASSID		Class_ID(0x5cd4456f, 0x2939563a)


TCHAR *GetString(int id);
extern ClassDesc* GetSimpleModDesc();
extern ClassDesc* GetSimpleWSMDesc();


// Paramblock2 name
enum { simpm_params, }; 
// Paramblock2 parameter list
enum { simpm_simple_param, };




/*===========================================================================*\
 |	SkeletonSimpMod class defn
\*===========================================================================*/

class SkeletonSimpMod : public SimpleMod2 {	
	public:
		static IObjParam *ip;

		SkeletonSimpMod();

		// Plugin identification
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_CLASSNAME); }  
		virtual Class_ID ClassID() { return SIMPM_CLASSID;}		
		TCHAR *GetObjectName() { return GetString(IDS_CLASSNAME);}

		// Open/Close our UI
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);

		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

		// Load and clone methods
		IOResult Load(ILoad *iload);
		RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// Deformer
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
		Interval GetValidity(TimeValue t);

		void InvalidateUI();
	};



/*===========================================================================*\
 |	SkeletonDeformer class defn
 |  This is the system that gets applied to each deformable point
 |
 |  Implimented in simpleDeform.cpp
\*===========================================================================*/

class SkeletonDeformer: public Deformer {
	public:
		Matrix3 tm,invtm, tmAbove, tmBelow;
		Box3 bbox;
		TimeValue time;
		float def_sparam; 
		
		// Bounding box center
		Point3 bpt;

		SkeletonDeformer();
		SkeletonDeformer(TimeValue t, ModContext &mc,
			Matrix3& modmat, Matrix3& modinv,
			float sparam);

		void SetAxis(Matrix3 &tmAxis);

		Point3 Map(int i, Point3 p); 
	};



/*===========================================================================*\
 |	SkeletonSimpWSM class defn
 |  Automatically support this plugin as a WSM
\*===========================================================================*/

class SkeletonSimpWSM : public SimpleOSMToWSMObject {
	public:
		SkeletonSimpWSM() {}
		SkeletonSimpWSM(SkeletonSimpMod *m) : SimpleOSMToWSMObject(m) {}

		void DeleteThis() { delete this; }
		SClass_ID SuperClassID() {return WSM_OBJECT_CLASS_ID;}
		Class_ID ClassID() {return SMWSM_CLASSID;} 

		RefTargetHandle Clone(RemapDir& remap)
		{
				return (new SkeletonSimpWSM((SkeletonSimpMod*)mod->Clone(remap)))->SimpleOSMToWSMClone(this,remap);
		}

		TCHAR *GetObjectName() {return GetString(IDS_CLASSNAME);}
	};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonModDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonSimpMod *ssm;

		SkeletonModDlgProc() {}
		SkeletonModDlgProc(SkeletonSimpMod *ssm_in) { ssm = ssm_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			ssm = (SkeletonSimpMod*)m;
			}

};


#endif
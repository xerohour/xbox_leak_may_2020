/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple Object
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 8-3-99
 | 
\*===========================================================================*/


#ifndef __SMPOBJSKEL__H
#define __SMPOBJSKEL__H

#include "max.h"
#include "simpobj.h"
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SIMPO_CLASSID		Class_ID(0x4fa6059, 0x288649d1)


TCHAR *GetString(int id);
extern ClassDesc* GetSimpleObjDesc();


// Paramblock2 name
enum { simpo_params, }; 
// Paramblock2 parameter list
enum { simpo_simple_param, simpo_genuv, };



/*===========================================================================*\
 |	SkeletonSimpObj class defn
\*===========================================================================*/

class SkeletonSimpObj : public SimpleObject2
{
	public:	
		static IObjParam *ip;

		SkeletonSimpObj();
		
		// Plugin identification
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return SIMPO_CLASSID; } 
		TCHAR *GetObjectName() { return GetString(IDS_CLASSNAME); }
		
		// Open/Close our UI
		void BeginEditParams( IObjParam  *ip, ULONG flags, Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags, Animatable *next);

		// Automatic UVW Generation
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		
		// Conversion to other objects?
		int CanConvertToType(Class_ID obtype);
		Object* ConvertToType(TimeValue t, Class_ID obtype);
		void GetCollapseTypes(Tab<Class_ID> &clist, Tab<TSTR*> &nlist);

		// From GeomObject
		int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);
		
		// Direct paramblock access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

		IOResult Load(ILoad *iload);
		
		// Create the object itself
		void BuildMesh(TimeValue t);
		BOOL OKtoDisplay(TimeValue t);
		CreateMouseCallBack* GetCreateMouseCallBack();


		void InvalidateUI();
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
};

class SkeletonObjCreateCallBack : public CreateMouseCallBack {
	SkeletonSimpObj *sso;
	IPoint2 sp0;
	Point3 p0;

	public:
	
	int proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(SkeletonSimpObj *obj) {sso = obj;}
};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonObjDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonSimpObj *sso;

		SkeletonObjDlgProc() {}
		SkeletonObjDlgProc(SkeletonSimpObj *sso_in) { sso = sso_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			sso = (SkeletonSimpObj*)m;
			}

};
#endif
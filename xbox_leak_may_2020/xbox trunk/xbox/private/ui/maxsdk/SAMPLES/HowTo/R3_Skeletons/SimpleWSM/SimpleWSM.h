/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple WSM modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 17-3-99
 | 
\*===========================================================================*/

#ifndef __WSMSKEL__H
#define __WSMSKEL__H

#include "max.h"
#include "simpmod.h"
#include "simpobj.h"
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	WSMMOD_CLASSID		Class_ID(0x38d202c, 0xda726e0)
#define	WSMOBJ_CLASSID		Class_ID(0x67924340, 0x57870f5)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonSimpleWSMDesc();
extern ClassDesc* GetSkeletonWSMObjectDesc();


// WSM Object Paramblock2 Support
// Paramblock2 name
enum { simpwsmobj_params, }; 
// Paramblock2 parameter list
enum { simpwsmobj_simple_param, simpwsmobj_radius_param, };

// WSM Modifier Paramblock2 Support
// Paramblock2 name
enum { simpwsm_params, }; 
// Paramblock2 parameter list
enum { simpwsm_simple_param, };



/*===========================================================================*\
 |	SimpleWSMObject2 and SkeletonWSMObject class defn
\*===========================================================================*/

class SimpleWSMObject2 : public SimpleWSMObject 
{
	public:
		IParamBlock2* pblock2;
		// From ref
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) { pblock2 = (IParamBlock2*)rtarg; SimpleWSMObject::SetReference(i, rtarg);}		
};

class SkeletonWSMObject : public SimpleWSMObject2
{	
	public:		
		IObjParam *ip;
		HWND hSot;
					
		SkeletonWSMObject();		

		void DeleteThis() {delete this;}		
		Class_ID ClassID() {return WSMOBJ_CLASSID;}		
		TCHAR *GetObjectName() {return GetString(IDS_CLASSNAME);}		

		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		
		void InvalidateUI();

		int DoOwnSelectHilite() {return TRUE;}
		CreateMouseCallBack* GetCreateMouseCallBack();

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		IOResult Load(ILoad *iload);

		Modifier *CreateWSMMod(INode *node);
		void BuildMesh(TimeValue t);
		
		// Direct paramblock access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }
};

class SkeletonWSMObjCreateCallBack : public CreateMouseCallBack 
{
	SkeletonWSMObject *swo;
	IPoint2 sp0;
	Point3 p0;

	public:
	
	int proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(SkeletonWSMObject *obj) {swo = obj;}
};


/*===========================================================================*\
 |	Dialog Processor (WSM Object)
\*===========================================================================*/

class SkeletonWSMObjDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonWSMObject *swo;

		SkeletonWSMObjDlgProc() {}
		SkeletonWSMObjDlgProc(SkeletonWSMObject *swo_in) { swo = swo_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			swo = (SkeletonWSMObject*)m;
			}

};



/*===========================================================================*\
 |	SkeletonWSMMod class defn
\*===========================================================================*/

class SimpleWSMMod2 : public SimpleWSMMod 
{
	public:
		IParamBlock2* pblock2;

		// From ref
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);		
		Animatable* SubAnim(int i);
};

class SkeletonWSMMod : public SimpleWSMMod2 
{
	public:		

		static IObjParam *ip;

		SkeletonWSMMod() {}
		SkeletonWSMMod(INode *node,SkeletonWSMObject *obj);		

		SClass_ID SuperClassID() { return WSM_CLASS_ID; }
		Class_ID ClassID() { return WSMMOD_CLASSID; } 
		TCHAR *GetObjectName() {return GetString(IDS_BINDING);}
		void GetClassName(TSTR& s) { s= GetString(IDS_CLASSNAME); }
		void DeleteThis() {delete this;}
		
		IOResult Load(ILoad *iload);
		RefTargetHandle Clone(RemapDir& remap = NoRemap());

		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		

		Interval GetValidity(TimeValue t);
		void InvalidateUI();

		// Direct paramblock access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }
};



/*===========================================================================*\
 |	Dialog Processor (WSM Modifier)
\*===========================================================================*/

class SkeletonWSMModDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonWSMMod *swmo;

		SkeletonWSMModDlgProc() {}
		SkeletonWSMModDlgProc(SkeletonWSMMod *swmo_in) { swmo = swmo_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			swmo = (SkeletonWSMMod*)m;
			}

};



/*===========================================================================*\
 |	SkeletonWSMDeformer class defn
 |  This is the system that gets applied to each deformable point
 |
 |  Implimented in simpleDeform.cpp
\*===========================================================================*/

class SkeletonWSMDeformer: public Deformer 
{
	public:
		Matrix3 tm,itm;
		TimeValue time;
		float sparam_wsm; 
		float sparam_obj;
		
		SkeletonWSMDeformer() {}

		Point3 Map(int i, Point3 p); 
};



#endif
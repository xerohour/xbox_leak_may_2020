/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Helper object
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 16-3-99
 | 
\*===========================================================================*/

#ifndef __HELPSKEL__H
#define __HELPSKEL__H

#include "max.h"
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	HELP_CLASSID		Class_ID(0x7bb85668, 0x3ac32c1f)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonHelperDesc();


// Paramblock2 name
enum { shelp_params, }; 
// Paramblock2 parameter list
enum { shelp_simple_param, };




class SkeletontHelpObject: public HelperObject 
{
	public:			
		IParamBlock2 *pblock2;
		static IObjParam *ip;

		// Snap suspension flag (TRUE during creation only)
		BOOL suspendSnap;
					
		float simple;
 		int extDispFlags;

		SkeletontHelpObject();
		
		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		void SetExtendedDisplay(int flags);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CreateMouseCallBack* GetCreateMouseCallBack();

		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		void InvalidateUI();

		// From Object
		ObjectState Eval(TimeValue time);

		void InitNodeName(TSTR& s) { s = GetString(IDS_CLASSNAME); }
		ObjectHandle ApplyTransform(Matrix3& matrix) {return this;}
		Interval ObjectValidity(TimeValue t);

		// We don't convert to anything
		int CanConvertToType(Class_ID obtype) {return FALSE;}
		Object* ConvertToType(TimeValue t, Class_ID obtype) {assert(0);return NULL;}
		
		void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
		void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
		int DoOwnSelectHilite()	{ return 1; }

		// Animatable methods
		void DeleteThis() { delete this; }
		Class_ID ClassID() { return HELP_CLASSID; }  
		void GetClassName(TSTR& s) { s = GetString(IDS_CLASSNAME); }
		int IsKeyable(){ return 0;}
		
		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

		int NumSubs() { return 1; }  
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

		// From ref
 		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );		
};				

class SkeletonHelperCreateCallBack : public CreateMouseCallBack {
	SkeletontHelpObject *sho;
	IPoint2 sp0;
	Point3 p0;

	public:
	
	int proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(SkeletontHelpObject *obj) {sho = obj;}
};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonHelpDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletontHelpObject *sho;

		SkeletonHelpDlgProc() {}
		SkeletonHelpDlgProc(SkeletontHelpObject *sho_in) { sho = sho_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			sho = (SkeletontHelpObject*)m;
			}

};

#endif
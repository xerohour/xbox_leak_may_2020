/*===========================================================================*\
 | 
 |  FILE:	Atmospheric.h
 |			Skeleton project and code for a Atmospheric effect
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 31-3-99
 | 
\*===========================================================================*/

#ifndef __RFXSKEL__H
#define __RFXSKEL__H

#include "max.h"
#include <bmmlib.h>
#include "iparamm2.h"
#include "render.h"  
#include "texutil.h"
#include "gizmo.h"
#include "gizmoimp.h"
#include "istdplug.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	ATMOS_CLASSID		Class_ID(0x76433eca, 0x79ed048b)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonAtmosDesc();


// Paramblock2 name
enum { skatmos_params, }; 
// Paramblock2 parameter list
enum { skatmos_simple_param, skatmos_gizmos, };


/*===========================================================================*\
 |	Validator class for ParamBlock2
\*===========================================================================*/

class SkeletonAtmospheric;

class GizmoValidatorClass : public PBValidator
{
	public:
		SkeletonAtmospheric *atp;
	private:
		BOOL Validate(PB2Value &v);
};


/*===========================================================================*\
 |	SkeletonAtmospheric class defn
\*===========================================================================*/

class SkeletonAtmospheric : public Atmospheric {
	public:
		// Parameters
		IParamBlock2 *pblock; 
		GizmoValidatorClass validator;

		SkeletonAtmospheric();
		~SkeletonAtmospheric() {}
		void DeleteThis() {delete this;}

		// SubAnim support
		int NumSubs() {return 1;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		
		// Reference support
		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message);

		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }

		// Plugin identification
		Class_ID ClassID() {return ATMOS_CLASSID;}
		void GetClassName(TSTR& s) {s=GetString(IDS_CLASSNAME);}
		TSTR GetName() {return GetString(IDS_CLASSNAME);}


		// Create our UI
		AtmosParamDlg *CreateParamDialog(IRendParams *ip);


		// Called before and after rendering time
		int RenderBegin(TimeValue t, ULONG flags);
		int RenderEnd(TimeValue t);


		// Doing the actual rendering 
		void Update(TimeValue t, Interval& valid);
		void Shade(ShadeContext& sc,const Point3& p0,const Point3& p1,Color& color, Color& trans, BOOL isBG);


		// Support for gizmos in our atmospheric
		int NumGizmos() ;
		INode *GetGizmo(int i);
		void DeleteGizmo(int i);
		void InsertGizmo(int i, INode *node);
		void AppendGizmo(INode *node);
		BOOL OKGizmo(INode *node); 
 		void EditGizmo(INode *node);
};



/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonAtmosDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonAtmospheric *ska;

		SkeletonAtmosDlgProc() {}
		SkeletonAtmosDlgProc(SkeletonAtmospheric *ska_in) { ska = ska_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			ska = (SkeletonAtmospheric*)m;
			}

};


/*===========================================================================*\
 |	Parammap handler
\*===========================================================================*/

class SkelAtmosParamDlg : public AtmosParamDlg 
{
	public:
		SkeletonAtmospheric *atmos;
		IRendParams *ip;
		IParamMap2 *pmap;

		SkelAtmosParamDlg(IRendParams *i,SkeletonAtmospheric *a);
		Class_ID ClassID() {return ATMOS_CLASSID;}
		ReferenceTarget* GetThing() {return atmos;}
		void SetThing(ReferenceTarget *m);		
		void DeleteThis();
};


#endif
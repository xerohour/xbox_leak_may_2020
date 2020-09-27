/*===========================================================================*\
 | 
 |  FILE:	Material.h
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

#ifndef __MTLSKEL__H
#define __MTLSKEL__H

#include "max.h"
#include <bmmlib.h>
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	MTL_CLASSID		Class_ID(0x22c0542e, 0x37232afa)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonMtlDesc();


// Paramblock2 name
enum { mtl_params, }; 
// Paramblock2 parameter list
enum { mtl_simple_param, mtl_mat1, mtl_mat1_on };


// How many submtl's we support
#define NSUBMTL		1


/*===========================================================================*\
 |	SkeletonMaterial class defn
\*===========================================================================*/

class SkeletonMaterial : public Mtl {	
	public:

		// various variables
		float simple;
		Mtl *submtl[NSUBMTL];		
		BOOL mapOn[NSUBMTL];
		Interval ivalid;

		// Parameter and UI management
		IParamBlock2 *pblock; 	
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Update(TimeValue t, Interval& valid);
		Interval Validity(TimeValue t);
		void Reset();


		SkeletonMaterial(BOOL loading);
		Class_ID ClassID() {return MTL_CLASSID; }
		SClass_ID SuperClassID() {return MATERIAL_CLASS_ID;}
		void GetClassName(TSTR& s) {s=GetString(IDS_CLASSNAME);}  
		void DeleteThis() {delete this;}
		

		void NotifyChanged();


		// From MtlBase and Mtl
		void SetAmbient(Color c, TimeValue t);		
		void SetDiffuse(Color c, TimeValue t);		
		void SetSpecular(Color c, TimeValue t);
		void SetShininess(float v, TimeValue t);
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);
				

		// Shade and displacement calculation
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
	

		// SubMaterial access methods
		int NumSubMtls() {return NSUBMTL;}
		Mtl* GetSubMtl(int i);
		void SetSubMtl(int i, Mtl *m);
		TSTR GetSubMtlSlotName(int i);
		TSTR GetSubMtlTVName(int i);

		// SubTexmap access methods
		int NumSubTexmaps() {return 0;}
		Texmap* GetSubTexmap(int i);
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		TSTR GetSubTexmapTVName(int i);

		// Number of subanims
		int NumSubs() {return 1+NSUBMTL;} 
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		// Number of references
 		int NumRefs() {return 1+NSUBMTL;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);


		RefTargetHandle Clone(RemapDir &remap = NoRemap());


		IOResult Save(ISave *isave); 
		IOResult Load(ILoad *iload); 


		// Direct Paramblock2 access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } 
		BOOL SetDlgThing(ParamDlg* dlg);
};



/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkelMtlDlgProc : public ParamMap2UserDlgProc 
{
	public:
		IParamMap *pmap;
		SkeletonMaterial *sm;

		SkelMtlDlgProc(SkeletonMaterial *m) {sm = m;}		
		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }
		void SetThing(ReferenceTarget *m) {
			sm = (SkeletonMaterial*)m;
			}
};

#endif
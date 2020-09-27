/*===========================================================================*\
 | 
 |  FILE:	Texmap.h
 |			Skeleton project and code for a Texture Map
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 11-3-99
 | 
\*===========================================================================*/

#ifndef __TEXSKEL__H
#define __TEXSKEL__H

#include "max.h"
#include "imtl.h"
#include "texutil.h"
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	TEX_CLASSID		Class_ID(0x7dd18ef, 0x400e305f)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonTexmapDesc();


// Paramblock2 name
enum { tex_params, }; 
// Paramblock2 parameter list
enum { tex_simple_param, tex_map1, tex_map1_on };


// How many subtex's we support
#define NSUBTEX		1


/*===========================================================================*\
 |	SkeletonTexmap class defn
\*===========================================================================*/

class SkeletonTexmap: public Texmap 
{ 
	public:
		Interval ivalid;
		Texmap* subTex[NSUBTEX];

		// various variables
		float simple;
		BOOL mapOn[NSUBTEX];


		SkeletonTexmap();

		// Parameter and UI management
		IParamBlock2 *pblock;
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }

		void NotifyChanged();

		// Evaluate the color of map for the context.
		AColor EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);

		// For Bump mapping, need a perturbation to apply to a normal.
		Point3 EvalNormalPerturb(ShadeContext& sc);

		// Methods to access texture maps of material
		int NumSubTexmaps() { return NSUBTEX; }
		Texmap* GetSubTexmap(int i) { return subTex[i]; }
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);

		Class_ID ClassID() {	return TEX_CLASSID; }
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) { s= GetString(IDS_CLASSNAME); }  
		void DeleteThis() { delete this; }	

		int NumSubs() { return NSUBTEX+1; }  
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) { return subNum; }

		// From ref
 		int NumRefs() { return NSUBTEX+1; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		int	NumParamBlocks() { return 1; }				
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } 
};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkelTexDlgProc : public ParamMap2UserDlgProc 
{
	public:
		IParamMap *pmap;
		SkeletonTexmap *st;

		SkelTexDlgProc(SkeletonTexmap *m) {st = m;}		
		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }
		void SetThing(ReferenceTarget *m) {
			st = (SkeletonTexmap*)m;
			}
};



#endif
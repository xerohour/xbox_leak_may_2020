/*===========================================================================*\
 | 
 |  FILE:	RenderEffect.h
 |			Skeleton project and code for a RenderEffect
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#ifndef __RFXSKEL__H
#define __RFXSKEL__H

#include "max.h"
#include <bmmlib.h>
#include "iparamm2.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	RFX_CLASSID		Class_ID(0x213e3901, 0x385fc0)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonRFXDesc();


// Paramblock2 name
enum { skeleton_params, }; 
// Paramblock2 parameter list
enum { skeleton_simple_param, };



/*===========================================================================*\
 |	SkeletonEffect class defn
\*===========================================================================*/

class SkeletonEffect: public Effect 
{
	public:
		// Parameters
		IParamBlock2 *pblock;
		
		SkeletonEffect();
		~SkeletonEffect() { 	}
		void DeleteThis() { delete this; }

		// Animatable/Reference
		int NumSubs() {return 1;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

		// References
		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rTarget);

		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }

		// ClassID/ClassName identifiers
		Class_ID ClassID() { return RFX_CLASSID; }
		void GetClassName(TSTR& s) { s=GetString(IDS_CLASSNAME); }

		// References have changed
		RefResult NotifyRefChanged(
					Interval changeInt, 
					RefTargetHandle hTarget, 
					PartID& partID,  
					RefMessage message	);

		// Standard load
		IOResult Load(ILoad *iload);

		// Effect class methods
		TSTR GetName() { return GetString(IDS_CLASSNAME); }
		EffectParamDlg *CreateParamDialog( IRendParams *pParams );
		int RenderBegin( TimeValue t, ULONG flags );
		int RenderEnd( TimeValue t );
		void Update( TimeValue t, Interval& valid );
		void Apply( TimeValue t, Bitmap *pBM, RenderGlobalContext *pGC, CheckAbortCallback *checkAbort);
};



/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonEffectDlgProc : public ParamMap2UserDlgProc 
{
	public:
		IParamMap *pmap;

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }
};




#endif
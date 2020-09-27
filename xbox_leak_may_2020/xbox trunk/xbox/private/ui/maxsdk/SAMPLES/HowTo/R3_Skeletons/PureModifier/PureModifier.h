/*===========================================================================*\
 | 
 |  FILE:	PureModifier.h
 |			Skeleton project and code for a full modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 31-3-99
 | 
\*===========================================================================*/


#ifndef __PMODSKEL__H
#define __PMODSKEL__H

#include "max.h"
#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	PUREM_CLASSID		Class_ID(0x566e006d, 0x6cdc6974)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonPureModDesc();


// Paramblock2 name
enum { skpurem_params, }; 
// Paramblock2 parameter list
enum { skpurem_simple_param, };



/*===========================================================================*\
 |	SkeletonModifier class defn
\*===========================================================================*/

class SkeletonModifier : public Modifier{
	public:

		// Access to the interface
		static IObjParam *ip;

		// Global parameter block
		IParamBlock2	*pblock;



		//Constructor/Destructor
		SkeletonModifier();
		~SkeletonModifier() {}
		void DeleteThis() { delete this; }


		// Plugin identification
		void GetClassName(TSTR& s) { s= TSTR(GetString(IDS_CLASSNAME)); }  
		virtual Class_ID ClassID() { return PUREM_CLASSID;}		
		TCHAR *GetObjectName() { return GetString(IDS_CLASSNAME); }


		// Defines the behavior for this modifier
		// This is currently setup to be basic geometry 
		// modification of deformable objects
		ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO; }
		ChannelMask ChannelsChanged() { return PART_GEOM; }
		Class_ID InputType() { return defObjectClassID; }
		BOOL ChangeTopology() {return FALSE;}


		// Calculate the local validity from the parameters
		Interval LocalValidity(TimeValue t);
		Interval GetValidity(TimeValue t);


		// Object modification and notification of change
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);


		// Reference support
		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		
		// SubAnim support
		int NumSubs() { return 1; }
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);


		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }
		int GetParamBlockIndex(int id) {return id;}

		// Does not use createmouse callbacks
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

		// Load and unload our UI
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		void InvalidateUI();
};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class SkeletonPureModDlgProc : public ParamMap2UserDlgProc 
{
	public:
		SkeletonModifier *spm;

		SkeletonPureModDlgProc() {}
		SkeletonPureModDlgProc(SkeletonModifier *spm_in) { spm = spm_in; }

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			spm = (SkeletonModifier*)m;
			}

};


#endif

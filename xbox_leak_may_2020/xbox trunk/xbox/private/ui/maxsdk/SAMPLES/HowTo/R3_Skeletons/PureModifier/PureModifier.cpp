/*===========================================================================*\
 | 
 |  FILE:	PureModifier.cpp
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

#include "PureModifier.h"


IObjParam* SkeletonModifier::ip = NULL;


/*===========================================================================*\
 |	Class Descriptor OSM
\*===========================================================================*/

class SkeletonPModClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonModifier; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return OSM_CLASS_ID; }
	Class_ID 		ClassID()					{ return PUREM_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonPureMod"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonPModClassDesc SkeletonPureModCD;
ClassDesc* GetSkeletonPureModDesc() {return &SkeletonPureModCD;}


/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonPureModDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (msg) 
	{
		case WM_INITDIALOG:
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			break;
	}
	return FALSE;
}


/*===========================================================================*\
 |	Paramblock2 Descriptor
\*===========================================================================*/

static ParamBlockDesc2 skpurem_param_blk ( skpurem_params, _T("SkeletonPureModParams"),  0, &SkeletonPureModCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_PMOD, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	skpurem_simple_param,	_T("skpurem_simple_param"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		0.0f,
		p_range, 		0.0f, 10.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIMPLE_EDIT, IDC_SIMPLE_SPIN, 0.1f,
		end,
	end
	);



/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonModifier::SkeletonModifier()
	{
	SkeletonPureModCD.MakeAutoParamBlocks(this);
	assert(pblock);
	}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletonModifier::InvalidateUI()
{
	skpurem_param_blk.InvalidateUI(pblock->LastNotifyParamID());
}



/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletonModifier::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;

	SkeletonPureModCD.BeginEditParams(ip, this, flags, prev);

	skpurem_param_blk.SetUserDlgProc(new SkeletonPureModDlgProc(this));
}
		
void SkeletonModifier::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	SkeletonPureModCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}



/*===========================================================================*\
 |	Standard clone
\*===========================================================================*/


RefTargetHandle SkeletonModifier::Clone(RemapDir& remap) 
{	
	SkeletonModifier* newmod = new SkeletonModifier();	
	newmod->ReplaceReference(0,pblock->Clone(remap));
	return(newmod);
}




/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletonModifier::SubAnim(int i) 	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
TSTR SkeletonModifier::SubAnimName(int i) {
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
	}

RefTargetHandle SkeletonModifier::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
void SkeletonModifier::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock = (IParamBlock2*)rtarg; break;
		}
	}
RefResult SkeletonModifier::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_CHANGE:
			skpurem_param_blk.InvalidateUI();
			break;
		}
	return REF_SUCCEED;
	}




/*===========================================================================*\
 |	The validity of our parameters
 |	Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/

Interval SkeletonModifier::GetValidity(TimeValue t)
{
	float f;	
	Interval valid = FOREVER;
	pblock->GetValue(skpurem_simple_param, t, f, valid);
	return valid;
}

Interval SkeletonModifier::LocalValidity(TimeValue t)
{
	return GetValidity(t);
}


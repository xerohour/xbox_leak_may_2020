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

#include "Helper.h"


IObjParam* SkeletontHelpObject::ip = NULL;


/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonHelperClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletontHelpObject; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return HELPER_CLASS_ID; }
	Class_ID 		ClassID()					{ return HELP_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonHelper"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonHelperClassDesc SkeletonHelpCD;
ClassDesc* GetSkeletonHelperDesc() {return &SkeletonHelpCD;}




/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonHelpDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 shelp_param_blk ( shelp_params, _T("SkeletonSimpleObjParams"),  0, &SkeletonHelpCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_HELPER, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	shelp_simple_param,	_T("HelperParameter"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		0.0f,
		p_range, 		0.0f, 65535.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_SIMPLE_EDIT, IDC_SIMPLE_SPIN, 0.1f,
		end,
	end
	);




/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletontHelpObject::SkeletontHelpObject()
	{
	SkeletonHelpCD.MakeAutoParamBlocks(this);
	assert(pblock2);
	suspendSnap = FALSE;
	}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletontHelpObject::InvalidateUI()
{
	shelp_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}



/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletontHelpObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;

	SkeletonHelpCD.BeginEditParams(ip, this, flags, prev);
	shelp_param_blk.SetUserDlgProc(new SkeletonHelpDlgProc(this));

}
		
void SkeletontHelpObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	SkeletonHelpCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}



/*===========================================================================*\
 |	Clone the helper
\*===========================================================================*/

RefTargetHandle SkeletontHelpObject::Clone(RemapDir& remap) {
	SkeletontHelpObject* newob = new SkeletontHelpObject();	
	newob->ReplaceReference(0,pblock2->Clone(remap));
	return(newob);
	}


/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletontHelpObject::SubAnim(int i) 	{
	switch (i) {
		case 0: return pblock2;
		default: return NULL;
		}
	}
TSTR SkeletontHelpObject::SubAnimName(int i) {
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
	}

RefTargetHandle SkeletontHelpObject::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock2;
		default: return NULL;
		}
	}
void SkeletontHelpObject::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock2 = (IParamBlock2*)rtarg; break;
		}
	}
RefResult SkeletontHelpObject::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_CHANGE:
			shelp_param_blk.InvalidateUI();
			break;
		}
	return REF_SUCCEED;
	}


/*===========================================================================*\
 |	The validity of our parameters
 |	Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/

Interval SkeletontHelpObject::ObjectValidity(TimeValue t)
	{
	float f;	
	Interval valid = FOREVER;
	pblock2->GetValue(shelp_simple_param, t, f, valid);
	return valid;
	}
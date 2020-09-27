/*===========================================================================*\
 | 
 |  FILE:	SimpMod.cpp
 |			Skeleton project and code for a Simple Modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "SimpleMod.h"


IObjParam* SkeletonSimpMod::ip = NULL;


/*===========================================================================*\
 |	Class Descriptor OSM
\*===========================================================================*/

class SkeletonSModClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonSimpMod; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return OSM_CLASS_ID; }
	Class_ID 		ClassID()					{ return SIMPM_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonSimpleMod"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonSModClassDesc SkeletonSMCD;
ClassDesc* GetSimpleModDesc() {return &SkeletonSMCD;}


/*===========================================================================*\
 |	Class Descriptor WSM
\*===========================================================================*/

class SkeletonSWSMClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create( BOOL loading )
	{
		if (loading) return new SkeletonSimpWSM; 
		else return new SkeletonSimpWSM(new SkeletonSimpMod);
	}

	const TCHAR *	ClassName() { return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID 		ClassID() { return SMWSM_CLASSID; }
	const TCHAR* 	Category() { return _T("");  }

	const TCHAR*	InternalName() { return _T("SkeletonSimpleModWSM"); }
	HINSTANCE		HInstance() { return hInstance; }
};

static SkeletonSWSMClassDesc SkeletonSWSMCD;
ClassDesc* GetSimpleWSMDesc() {return &SkeletonSWSMCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonModDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 skeleton_param_blk ( simpm_params, _T("SkeletonSimpleModParams"),  0, &SkeletonSMCD, P_AUTO_CONSTRUCT + P_AUTO_UI, SIMPMOD_PBLOCKREF, 
	//rollout
	IDD_SKELETON_SMOD, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	simpm_simple_param,	_T("simpmParam1"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLE,
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

SkeletonSimpMod::SkeletonSimpMod()
	{
	SkeletonSMCD.MakeAutoParamBlocks(this);
	assert(pblock2);
	}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletonSimpMod::InvalidateUI()
{
	skeleton_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}



/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletonSimpMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	this->ip = ip;

	SimpleMod::BeginEditParams(ip,flags,prev);
	SkeletonSMCD.BeginEditParams(ip, this, flags, prev);

	skeleton_param_blk.SetUserDlgProc(new SkeletonModDlgProc(this));

	}
		
void SkeletonSimpMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleMod::EndEditParams(ip,flags,next);
	SkeletonSMCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
	}



/*===========================================================================*\
 |	Standard Load and clone
\*===========================================================================*/

IOResult SkeletonSimpMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	return IO_OK;
	}

RefTargetHandle SkeletonSimpMod::Clone(RemapDir& remap) {	
	SkeletonSimpMod* newmod = new SkeletonSimpMod();	
	newmod->ReplaceReference(SIMPMOD_PBLOCKREF,pblock2->Clone(remap));
	newmod->SimpleModClone(this);
	return(newmod);
	}



/*===========================================================================*\
 |	The validity of our parameters
 |	Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/

Interval SkeletonSimpMod::GetValidity(TimeValue t)
	{
	float f;	
	Interval valid = FOREVER;
	pblock2->GetValue(simpm_simple_param, t, f, valid);
	return valid;
	}



/*===========================================================================*\
 |	Get the actual deformer to modify the object
\*===========================================================================*/

Deformer& SkeletonSimpMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	float sparam = 0.0f; Interval valid = FOREVER;
	pblock2->GetValue(simpm_simple_param, t, sparam, valid);

	static SkeletonDeformer deformer;
	deformer = SkeletonDeformer(t,mc,mat,invmat,sparam);
	return deformer;
	}



/*===========================================================================*\
 |	SimpleMod2 overloads
\*===========================================================================*/

RefTargetHandle SimpleMod2::GetReference(int i) 
	{ 
	switch (i) {
		case 0: return tmControl;
		case 1: return posControl;
		case 2: return pblock2;
		default: return NULL;
		}
	}

void SimpleMod2::SetReference(int i, RefTargetHandle rtarg) 
	{ 
	switch (i) {
		case 0: tmControl = (Control*)rtarg; break;
		case 1: posControl = (Control*)rtarg; break;
		case 2: pblock2 = (IParamBlock2*)rtarg; break;
		}
	}

Animatable* SimpleMod2::SubAnim(int i) 
	{ 
	switch (i) {
		case 0: return posControl;
		case 1: return tmControl;
		case 2: return pblock2;		
		default: return NULL;
		}
	}

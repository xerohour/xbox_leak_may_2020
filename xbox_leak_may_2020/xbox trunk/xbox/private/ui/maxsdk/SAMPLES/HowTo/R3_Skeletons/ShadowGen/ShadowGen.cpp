/*===========================================================================*\
 | 
 |  FILE:	ShadowGen.cpp
 |			Skeleton project and code for a Shadow Generator
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 10-3-99
 | 
\*===========================================================================*/

#include "ShadowGen.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonShadowClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create(BOOL loading)		{ return new SkeletonShadow; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return SHADOW_TYPE_CLASS_ID; }
	Class_ID 		ClassID()					{ return SHADOW_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonShadowGen"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};

static SkeletonShadowClassDesc SkelShadCD;

ClassDesc* GetSkeletonShdwGenDesc() { return &SkelShadCD; }




/*===========================================================================*\
 |	Paramblock2 Descriptor
\*===========================================================================*/

static ParamBlockDesc2 skshadow_param_blk ( shd_params, _T("SkeletonShadow parameters"),  0, &SkelShadCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_SHADOW, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	shd_simple_param,	_T("shd_simple_param"),	TYPE_BOOL,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_SIMPLE, 
		end,
	end
	);


/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonShadow::SkeletonShadow()
	{
	theParam = NULL;
	SkelShadCD.MakeAutoParamBlocks(this);
	assert(pblock);
	}


/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletonShadow::SubAnim(int i) 	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
TSTR SkeletonShadow::SubAnimName(int i) {
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
	}

RefTargetHandle SkeletonShadow::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
void SkeletonShadow::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock = (IParamBlock2*)rtarg; break;
		}
	}
RefResult SkeletonShadow::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_CHANGE:
			skshadow_param_blk.InvalidateUI();
			break;
		}
	return REF_SUCCEED;
	}


/*===========================================================================*\
 |	Support the Parammap UI
\*===========================================================================*/

SkeletonParam::SkeletonParam(SkeletonShadow *shad, Interface *intface) {
	theShad = shad;
	ip = intface;

	pmap = CreateCPParamMap2(
		theShad->pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_SKELETON_SHADOW),
		GetString(IDS_PARAMETERS),
		0);
	}

SkeletonParam::~SkeletonParam(){
	if(theShad) theShad->theParam = NULL;
	DestroyCPParamMap2(pmap);
	}


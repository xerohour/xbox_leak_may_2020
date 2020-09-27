/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple WSM modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 17-3-99
 | 
\*===========================================================================*/

#include "SimpleWSM.h"


IObjParam* SkeletonWSMMod::ip = NULL;


/*===========================================================================*\
 |	Class Descriptor OSM
\*===========================================================================*/

class SkeletonWSMClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()							{ return FALSE; }
	void *			Create(BOOL loading = FALSE)		{ return new SkeletonWSMMod; }
	const TCHAR *	ClassName()							{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()						{ return WSM_CLASS_ID; }
	Class_ID		ClassID()							{ return WSMMOD_CLASSID; }
	const TCHAR* 	Category()							{ return _T("");}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()						{ return _T("SkeletonSimpleWSM"); }
	HINSTANCE		HInstance()							{ return hInstance; }
};

static SkeletonWSMClassDesc SkeletonWSMCD;
ClassDesc* GetSkeletonSimpleWSMDesc() { return &SkeletonWSMCD; }



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonWSMModDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 skeleton_param_blk ( simpwsm_params, _T("SkeletonSimpleWSMParams"),  0, &SkeletonWSMCD, P_AUTO_CONSTRUCT + P_AUTO_UI, SIMPWSMMOD_PBLOCKREF, 
	//rollout
	IDD_SKELETON_SWSM, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	simpwsm_simple_param,	_T("simpwsm_simple_param"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		1.0f,
		p_range, 		-10.0f, 10.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIMPLE_EDIT, IDC_SIMPLE_SPIN, 0.1f,
		end,
	end
	);




/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonWSMMod::SkeletonWSMMod(INode *node,SkeletonWSMObject *obj)
{	
	SkeletonWSMCD.MakeAutoParamBlocks(this);
	assert(pblock2);

	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);
	
	obRef = NULL;
}


/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletonWSMMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;

	SimpleWSMMod::BeginEditParams(ip,flags,prev);
	SkeletonWSMCD.BeginEditParams(ip, this, flags, prev);

	skeleton_param_blk.SetUserDlgProc(new SkeletonWSMModDlgProc(this));
}
		
void SkeletonWSMMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	SimpleWSMMod::EndEditParams(ip,flags,next);
	SkeletonWSMCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;	
}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletonWSMMod::InvalidateUI()
{
	skeleton_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}



/*===========================================================================*\
 |	Standard Load and clone
\*===========================================================================*/

IOResult SkeletonWSMMod::Load(ILoad *iload)
{
	Modifier::Load(iload);
	return IO_OK;
}

RefTargetHandle SkeletonWSMMod::Clone(RemapDir& remap) 
{
	SkeletonWSMMod *newob = new SkeletonWSMMod(nodeRef,(SkeletonWSMObject*)obRef);
	newob->ReplaceReference(SIMPWSMMOD_PBLOCKREF,pblock2->Clone(remap));
	newob->ReplaceReference(SIMPWSMMOD_OBREF,this->obRef);
	newob->ReplaceReference(SIMPWSMMOD_NODEREF,this->nodeRef);
	return newob;
}



/*===========================================================================*\
 |	The validity of our parameters
 |	Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/

Interval SkeletonWSMMod::GetValidity(TimeValue t) 
{
	if (nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;

		SkeletonWSMObject *obj = (SkeletonWSMObject*)GetWSMObject(t);
		pblock2->GetValue(simpwsm_simple_param, t, f, valid);
		obj->pblock2->GetValue(simpwsmobj_simple_param, t, f, valid);

		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
}




/*===========================================================================*\
 |	Get the actual deformer to modify the object
\*===========================================================================*/

Deformer& SkeletonWSMMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
{
	Interval valid;
	static SkeletonWSMDeformer wsmd;

	pblock2->GetValue(simpwsm_simple_param, t, wsmd.sparam_wsm, valid);

	SkeletonWSMObject *obj = (SkeletonWSMObject*)GetWSMObject(t);
	obj->pblock2->GetValue(simpwsmobj_simple_param, t, wsmd.sparam_obj, valid);

	wsmd.time = t;
	wsmd.itm  = nodeRef->GetObjectTM(t,&valid);
	wsmd.tm   = Inverse( wsmd.itm );
	return wsmd;
}



/*===========================================================================*\
 |	SimpleWSMMod2 overloads
\*===========================================================================*/

RefTargetHandle SimpleWSMMod2::GetReference(int i) 
{ 
	switch (i) {
		case SIMPWSMMOD_OBREF: return obRef;
		case SIMPWSMMOD_NODEREF: return nodeRef;
		case SIMPWSMMOD_PBLOCKREF: return pblock;
		default: return NULL;
		}
}

void SimpleWSMMod2::SetReference(int i, RefTargetHandle rtarg) 
{ 
	switch (i) {
		case SIMPWSMMOD_OBREF: obRef = (WSMObject*)rtarg; break;
		case SIMPWSMMOD_NODEREF: nodeRef = (INode*)rtarg; break;
		case SIMPWSMMOD_PBLOCKREF: pblock2 = (IParamBlock2*)rtarg; SimpleWSMMod::SetReference(i, rtarg);break;
		}
}

Animatable* SimpleWSMMod2::SubAnim(int i) 
{ 
	switch (i) {
		case 0: return pblock2;		
		default: return NULL;
		}
}


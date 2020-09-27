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



/*===========================================================================*\
 |	Class Descriptor OSM
\*===========================================================================*/

class SkeletonWSMObjClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()							{ return TRUE; }
	void *			Create(BOOL loading = FALSE)		{ return new SkeletonWSMObject; }
	const TCHAR *	ClassName()							{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()						{ return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID()							{ return WSMOBJ_CLASSID; }
	const TCHAR* 	Category()							{ return _T("");}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()						{ return _T("SkeletonSimpleWSMObject"); }
	HINSTANCE		HInstance()							{ return hInstance; }
};

static SkeletonWSMObjClassDesc SkeletonWSMObjCD;
ClassDesc* GetSkeletonWSMObjectDesc() { return &SkeletonWSMObjCD; }



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonWSMObjDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 skeletonobj_param_blk ( simpwsmobj_params, _T("SkeletonSimpleWSMObjParams"),  0, &SkeletonWSMObjCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_SWSMOBJ, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	simpwsmobj_simple_param,	_T("simpwsmobj_simple_param"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLEOBJ,
		p_default,		0.0f,
		p_range, 		0.0f, 10.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIMPLEOBJ_EDIT, IDC_SIMPLEOBJ_SPIN, 0.1f,
		end,
	simpwsmobj_radius_param,	_T("simpwsmobj_radius_param"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_RADIUS,
		p_default,		0.0f,
		p_range, 		0.0f, 65535.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_RADIUS_EDIT, IDC_RADIUS_SPIN, 0.1f,
		end,
	end
	);


/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonWSMObject::SkeletonWSMObject()
	{
	SkeletonWSMObjCD.MakeAutoParamBlocks(this);
	assert(pblock2);
	}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletonWSMObject::InvalidateUI()
{
	skeletonobj_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}




/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletonWSMObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	this->ip = ip;

	SimpleWSMObject::BeginEditParams(ip,flags,prev);
	SkeletonWSMObjCD.BeginEditParams(ip, this, flags, prev);

	skeletonobj_param_blk.SetUserDlgProc(new SkeletonWSMObjDlgProc(this));

	}
		
void SkeletonWSMObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleWSMObject::EndEditParams(ip,flags,next);
	SkeletonWSMObjCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
	}




/*===========================================================================*\
 |	Standard Load and clone
\*===========================================================================*/

IOResult SkeletonWSMObject::Load(ILoad *iload)
	{
	SimpleWSMObject::Load(iload);
	return IO_OK;
	}

RefTargetHandle SkeletonWSMObject::Clone(RemapDir& remap) {	
	SkeletonWSMObject* newob = new SkeletonWSMObject();	
	newob->ReplaceReference(0,pblock2->Clone(remap));
	return(newob);
	}



/*===========================================================================*\
 |	Create the WSM modifier
\*===========================================================================*/

Modifier *SkeletonWSMObject::CreateWSMMod(INode *node)
	{
	return new SkeletonWSMMod(node,this);
	}



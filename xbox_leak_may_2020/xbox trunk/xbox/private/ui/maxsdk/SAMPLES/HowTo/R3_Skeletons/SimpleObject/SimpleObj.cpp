/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple Object
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 8-3-99
 | 
\*===========================================================================*/

#include "SimpleObj.h"


IObjParam* SkeletonSimpObj::ip = NULL;


/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonSObjClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonSimpObj; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return GEOMOBJECT_CLASS_ID; }
	Class_ID 		ClassID()					{ return SIMPO_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonSimpleObj"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonSObjClassDesc SkeletonSOCD;
ClassDesc* GetSimpleObjDesc() {return &SkeletonSOCD;}




/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonObjDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 skeleton_param_blk ( simpo_params, _T("SkeletonSimpleObjParams"),  0, &SkeletonSOCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_SOBJ, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	simpo_simple_param,	_T("simpoParam1"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		0.0f,
		p_range, 		0.0f, 65535.0f, 
		p_ui,			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_SIMPLE_EDIT, IDC_SIMPLE_SPIN, 0.1f,
		end,
	simpo_genuv,	 	_T("GenUV"), 	TYPE_BOOL, 		0,		IDS_MAPPING,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_MAPPING, 
		end, 
	end
	);




/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonSimpObj::SkeletonSimpObj()
	{
	SkeletonSOCD.MakeAutoParamBlocks(this);
	assert(pblock2);
	}



/*===========================================================================*\
 |	Invalidate our UI (or the recently changed parameter)
\*===========================================================================*/

void SkeletonSimpObj::InvalidateUI()
{
	skeleton_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}



/*===========================================================================*\
 |	Open and Close dialog UIs
 |	We ask the ClassDesc2 to handle Beginning and Ending EditParams for us
\*===========================================================================*/

void SkeletonSimpObj::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	this->ip = ip;

	SimpleObject::BeginEditParams(ip,flags,prev);
	SkeletonSOCD.BeginEditParams(ip, this, flags, prev);

	skeleton_param_blk.SetUserDlgProc(new SkeletonObjDlgProc(this));

	}
		
void SkeletonSimpObj::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleObject::EndEditParams(ip,flags,next);
	SkeletonSOCD.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
	}




/*===========================================================================*\
 |	Standard Load and clone
\*===========================================================================*/

IOResult SkeletonSimpObj::Load(ILoad *iload)
	{
	Object::Load(iload);
	return IO_OK;
	}

RefTargetHandle SkeletonSimpObj::Clone(RemapDir& remap) 
{
	SkeletonSimpObj* newob = new SkeletonSimpObj();	
	newob->ReplaceReference(0, pblock2->Clone(remap));
	newob->ivalid.SetEmpty();	
	return(newob);
}



/*===========================================================================*\
 |	Standard object implimentations
\*===========================================================================*/

BOOL SkeletonSimpObj::OKtoDisplay(TimeValue t) 
{
		float size;
		pblock2->GetValue(simpo_simple_param, t, size, FOREVER);
		if (size==0.0f) return FALSE;
		else return TRUE;
}


int SkeletonSimpObj::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	// pass to SimpleObject to do this
	return SimpleObject::IntersectRay(t, ray, at, norm);
}

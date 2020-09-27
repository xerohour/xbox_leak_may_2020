/*===========================================================================*\
 | 
 |  FILE:	Atmospheric.cpp
 |			Skeleton project and code for a Atmospheric effect
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 31-3-99
 | 
\*===========================================================================*/

#include "Atmospheric.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonAtmosClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()				{ return TRUE; }
	void *			Create( BOOL loading )	{ return new SkeletonAtmospheric; }
	const TCHAR *	ClassName()				{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()			{ return ATMOSPHERIC_CLASS_ID; }
	Class_ID 		ClassID()				{ return ATMOS_CLASSID; }
	const TCHAR* 	Category()				{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()			{ return _T("SkeletonAtmospheric"); }
	HINSTANCE		HInstance()				{ return hInstance; }
};

static SkeletonAtmosClassDesc SkeletonAtmosCD;
ClassDesc* GetSkeletonAtmosDesc() {return &SkeletonAtmosCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonAtmosDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
 |	Gizmo object validator
 |	This routine works with ParamBlock2 to check each incoming gizmo
 |	to check it is okay to use
\*===========================================================================*/

// All the code is already in OKGizmo, so pass to that
BOOL GizmoValidatorClass::Validate(PB2Value &v) 
{
	INode *node = (INode*) v.r;

	if (node->TestForLoop(FOREVER,(ReferenceMaker *) atp)!=REF_SUCCEED) return FALSE;

	return atp->OKGizmo(node);
}



/*===========================================================================*\
 |	Paramblock2 Descriptor
 |	In an atmospheric, which stores references to gizmos, we can use the INODE_TAB
 |	parameter type of PB2, which will automate the entire UI and reference management
 |	for us. Groovy!
\*===========================================================================*/

static ParamBlockDesc2 skatmos_param_blk ( skatmos_params, _T("SkeletonAtmosParameters"),  0, &SkeletonAtmosCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_ATMOS, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	skatmos_gizmos,    _T("Gizmos"),  TYPE_INODE_TAB, 0, P_AUTO_UI,	IDS_GIZMOS,
		p_ui,	TYPE_NODELISTBOX, IDC_GIZMOLIST,	IDC_ADDGIZMO,	0,	IDC_DELGIZMO,
		end,
	skatmos_simple_param, _T("Color"),	TYPE_RGBA, P_ANIMATABLE, IDS_SIMPLE,
		p_default, Color(0,0,0),
		p_ui, TYPE_COLORSWATCH, IDC_SIMPLE,
		end,
	end
	);



/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonAtmospheric::SkeletonAtmospheric()
{
	SkeletonAtmosCD.MakeAutoParamBlocks(this);
	assert(pblock);

	validator.atp = this;
}



/*===========================================================================*\
 |	Ask the ClassDesc2 to make the AUTO_UI SkeletonAtmosDlgProc
\*===========================================================================*/

EffectParamDlg *SkeletonAtmospheric::CreateParamDialog(IRendParams *ip)
{	
	skatmos_param_blk.ParamOption(skatmos_gizmos,p_validator,&validator);
	return new SkelAtmosParamDlg(ip,this);
}



/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletonAtmospheric::SubAnim(int i) 	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
TSTR SkeletonAtmospheric::SubAnimName(int i) {
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
	}

RefTargetHandle SkeletonAtmospheric::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
void SkeletonAtmospheric::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock = (IParamBlock2*)rtarg; break;
		}
	}
RefResult SkeletonAtmospheric::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_TARGET_DELETED:
			{
				for(int i=0;i<NumGizmos();i++)
				{
					if((INode*)hTarget==GetGizmo(i)) DeleteGizmo(i);
				}
			}
			break;

		case REFMSG_CHANGE:
			skatmos_param_blk.InvalidateUI();
			break;
		}
	return REF_SUCCEED;
	}


/*===========================================================================*\
 |	Support for getting/setting gizmos  
\*===========================================================================*/

int SkeletonAtmospheric::NumGizmos() 
{
	return pblock->Count(skatmos_gizmos);
}

INode *SkeletonAtmospheric::GetGizmo(int i) 
{
	INode *node = NULL;
	Interval iv;
	pblock->GetValue(skatmos_gizmos,0,node,iv, i);
	return node;
}

void SkeletonAtmospheric::DeleteGizmo(int i)
{
	pblock->Delete(skatmos_gizmos, i,1); 
	skatmos_param_blk.InvalidateUI();

	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void SkeletonAtmospheric::InsertGizmo(int i, INode *node)
{
	pblock->SetValue(skatmos_gizmos, 0, node, i);
}

void SkeletonAtmospheric::AppendGizmo(INode *node)
{
	pblock->Append(skatmos_gizmos, 1, &node);
}

BOOL SkeletonAtmospheric::OKGizmo(INode *node)
{
	// check for duplicates in the gizmo list
	for(int i=0;i<NumGizmos();i++) { if(node==GetGizmo(i)) return FALSE; }

	ObjectState os = node->EvalWorldState(GetCOREInterface()->GetTime());
	if (os.obj->ClassID()==SPHEREGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==CYLGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==BOXGIZMO_CLASSID) return TRUE;
	return FALSE;
}

void SkeletonAtmospheric::EditGizmo(INode *node)
{
}



/*===========================================================================*\
 |	Parammap handler implimentation
\*===========================================================================*/

SkelAtmosParamDlg::SkelAtmosParamDlg(IRendParams *i,SkeletonAtmospheric *a) 
{
	atmos = a;
	ip    = i;	

	pmap = CreateRParamMap2(
		atmos->pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_SKELETON_ATMOS),
		GetString(IDS_PARAMETERS),
		0);

	pmap->SetUserDlgProc(new SkeletonAtmosDlgProc(atmos));
}

void SkelAtmosParamDlg::SetThing(ReferenceTarget *m)
{
	assert(m->ClassID()==atmos->ClassID());
	atmos = (SkeletonAtmospheric*)m;
	pmap->SetParamBlock(atmos->pblock);	
}

void SkelAtmosParamDlg::DeleteThis()
{
	ip->EndPickMode();
	DestroyRParamMap2(pmap);
	delete this;
}


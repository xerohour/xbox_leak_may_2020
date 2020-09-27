/*===========================================================================*\
 | 
 |  FILE:	Material.cpp
 |			Skeleton project and code for a Material
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 11-3-99
 | 
\*===========================================================================*/

#include "Material.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonMaterialClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonMaterial(loading); }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return MATERIAL_CLASS_ID; }
	Class_ID 		ClassID()					{ return MTL_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonMaterial"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonMaterialClassDesc SkelMtlCD;
ClassDesc* GetSkeletonMtlDesc() {return &SkelMtlCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkelMtlDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 smtl_param_blk ( mtl_params, _T("SkeletonMaterial parameters"),  0, &SkelMtlCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 1, 
	//rollout
	IDD_SKELETON_MATERIAL, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	mtl_mat1,			_T("mtl_mat1"),			TYPE_MTL,	P_OWNERS_REF,	IDS_MTL1,
		p_refno,		0,
		p_submtlno,		0,		
		p_ui,			TYPE_MTLBUTTON, IDC_MTL1,
		end,
	mtl_mat1_on,		_T("mtl_mat1_on"),		TYPE_BOOL,		0,				IDS_MTL1ON,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MTLON1,
		end,
	mtl_simple_param,	_T("mtl_simple_param"),	TYPE_FLOAT,		P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		1.0f,
		p_range,		0.0f, 1.0f,
		p_ui,			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIMPLE_EDIT, IDC_SIMPLE_SPIN, 0.1f,
		end,

	end
	);



/*===========================================================================*\
 |	Constructor and Reset systems
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

void SkeletonMaterial::Reset() 
{
	ivalid.SetEmpty();
	for (int i=0; i<NSUBMTL; i++) {
		DeleteReference(i);
		mapOn[i] = 1;
		}

	SkelMtlCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
}

SkeletonMaterial::SkeletonMaterial(BOOL loading) 
{
	for (int i=0; i<NSUBMTL; i++) submtl[i] = NULL;
	pblock = NULL;

	if (!loading) 
		Reset();
}

ParamDlg* SkeletonMaterial::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = SkelMtlCD.CreateParamDlgs(hwMtlEdit, imp, this);
	smtl_param_blk.SetUserDlgProc(new SkelMtlDlgProc(this));

	return masterDlg;
}

BOOL SkeletonMaterial::SetDlgThing(ParamDlg* dlg)
{
	return FALSE;
}

Interval SkeletonMaterial::Validity(TimeValue t)
{
	Interval valid = FOREVER;		

	for (int i=0; i<NSUBMTL; i++) 
	{
		if (submtl[i]) 
			valid &= submtl[i]->Validity(t);
	}
	
	float u;
	pblock->GetValue(mtl_simple_param,t,u,valid);
	return valid;
}


/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

RefTargetHandle SkeletonMaterial::GetReference(int i) 
{
	if (i < NSUBMTL )
		return submtl[i];
	else return pblock;
}

void SkeletonMaterial::SetReference(int i, RefTargetHandle rtarg) 
{
	if (i < NSUBMTL)
		submtl[i] = (Mtl *)rtarg; 
	else pblock = (IParamBlock2 *)rtarg; 
}

TSTR SkeletonMaterial::SubAnimName(int i) 
{
	if (i < NSUBMTL)
		return GetSubMtlTVName(i);
	else return TSTR(_T(""));
}

Animatable* SkeletonMaterial::SubAnim(int i) {
	if (i < NSUBMTL)
		return submtl[i]; 
	else return pblock;
	}

RefResult SkeletonMaterial::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) 
{
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget == pblock)
				{
				ParamID changing_param = pblock->LastNotifyParamID();
				smtl_param_blk.InvalidateUI(changing_param);
				}
			break;

		}
	return(REF_SUCCEED);
}



/*===========================================================================*\
 |	SubMtl get and set
\*===========================================================================*/

Mtl* SkeletonMaterial::GetSubMtl(int i)
{
	if (i < NSUBMTL )
		return submtl[i];
	return NULL;
}

void SkeletonMaterial::SetSubMtl(int i, Mtl *m)
{
	ReplaceReference(i,m);
	if (i==0)
	{
		smtl_param_blk.InvalidateUI(mtl_mat1);
		ivalid.SetEmpty();
	}	
}

TSTR SkeletonMaterial::GetSubMtlSlotName(int i)
{
	switch(i) {
		case 0:  return TSTR(GetString(IDS_MTL1)); 
		default: return TSTR(_T(""));
	}
}

TSTR SkeletonMaterial::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}


/*===========================================================================*\
 |	Texmap get and set
 |  By default, we support none
\*===========================================================================*/

Texmap* SkeletonMaterial::GetSubTexmap(int i)
{
	return NULL;
}

void SkeletonMaterial::SetSubTexmap(int i, Texmap *m)
{
}

TSTR SkeletonMaterial::GetSubTexmapSlotName(int i)
{
	return _T("");
}

TSTR SkeletonMaterial::GetSubTexmapTVName(int i)
{
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000

IOResult SkeletonMaterial::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	return IO_OK;
	}	

IOResult SkeletonMaterial::Load(ILoad *iload) { 
	IOResult res;
	int id;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	return IO_OK;
	}




/*===========================================================================*\
 |	Updating and cloning
\*===========================================================================*/


RefTargetHandle SkeletonMaterial::Clone(RemapDir &remap) {
	SkeletonMaterial *mnew = new SkeletonMaterial(FALSE);
	*((MtlBase*)mnew) = *((MtlBase*)this); 
	mnew->ReplaceReference(NSUBMTL,remap.CloneRef(pblock));

	mnew->ivalid.SetEmpty();	
	for (int i = 0; i<NSUBMTL; i++) {
		mnew->submtl[i] = NULL;
		if (submtl[i])
			mnew->ReplaceReference(i,remap.CloneRef(submtl[i]));
		mnew->mapOn[i] = mapOn[i];
		}
	return (RefTargetHandle)mnew;
	}

void SkeletonMaterial::NotifyChanged() 
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void SkeletonMaterial::Update(TimeValue t, Interval& valid) 
{		
	if (!ivalid.InInterval(t)) {

		ivalid.SetInfinite();
		pblock->GetValue( mtl_mat1_on, t, mapOn[0], ivalid);
		pblock->GetValue( mtl_simple_param, t, simple, ivalid);

		for (int i=0; i<NSUBMTL; i++) {
			if (submtl[i]) 
				submtl[i]->Update(t,ivalid);
			}
		}
	valid &= ivalid;
}



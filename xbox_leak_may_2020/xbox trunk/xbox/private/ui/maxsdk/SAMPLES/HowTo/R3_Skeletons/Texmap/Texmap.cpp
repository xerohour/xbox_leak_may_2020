/*===========================================================================*\
 | 
 |  FILE:	Texmap.cpp
 |			Skeleton project and code for a Texture Map
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 11-3-99
 | 
\*===========================================================================*/

#include "Texmap.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonTexmapClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonTexmap; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID()					{ return TEX_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonTexmap"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

static SkeletonTexmapClassDesc SkelTexmapCD;
ClassDesc* GetSkeletonTexmapDesc() {return &SkelTexmapCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkelTexDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

static ParamBlockDesc2 stex_param_blk ( tex_params, _T("SkeletonTexmap parameters"),  0, &SkelTexmapCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 1, 
	//rollout
	IDD_SKELETON_TEXMAP, IDS_PARAMETERS, 0, 0, NULL, 
	// params
	tex_map1,			_T("tex_map1"),			TYPE_TEXMAP,	P_OWNERS_REF,	IDS_MAP1,
		p_refno,		0,
		p_subtexno,		0,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_MAP1,
		end,
	tex_map1_on,		_T("tex_map1_on"),		TYPE_BOOL,		0,				IDS_MAP1ON,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON1,
		end,
	tex_simple_param,	_T("tex_simple_param"),	TYPE_FLOAT,		P_ANIMATABLE,	IDS_SIMPLE,
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

void SkeletonTexmap::Reset() 
{
	ivalid.SetEmpty();
	for (int i=0; i<NSUBTEX; i++) {
		DeleteReference(i);
		mapOn[i] = 1;
		}

	SkelTexmapCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
}

SkeletonTexmap::SkeletonTexmap() 
{
	for (int i=0; i<NSUBTEX; i++) subTex[i] = NULL;
	pblock = NULL;
	Reset();
}

ParamDlg* SkeletonTexmap::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = SkelTexmapCD.CreateParamDlgs(hwMtlEdit, imp, this);
	stex_param_blk.SetUserDlgProc(new SkelTexDlgProc(this));

	return masterDlg;
}



/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

RefTargetHandle SkeletonTexmap::GetReference(int i) 
{
	if (i <NSUBTEX )
		return subTex[i];
	else return pblock;
}

void SkeletonTexmap::SetReference(int i, RefTargetHandle rtarg) 
{
	if (i < NSUBTEX)
		subTex[i] = (Texmap *)rtarg; 
	else pblock = (IParamBlock2 *)rtarg; 
}

TSTR SkeletonTexmap::SubAnimName(int i) 
{
	if (i< NSUBTEX)
		return GetSubTexmapTVName(i);
	else return TSTR(_T(""));
}

Animatable* SkeletonTexmap::SubAnim(int i) {
	if (i < NSUBTEX)
		return subTex[i]; 
	else return pblock;
	}

RefResult SkeletonTexmap::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) 
{
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget == pblock)
				{
				ParamID changing_param = pblock->LastNotifyParamID();
				stex_param_blk.InvalidateUI(changing_param);
				}
			break;

		}
	return(REF_SUCCEED);
}



/*===========================================================================*\
 |	Texmap get and set
\*===========================================================================*/

void SkeletonTexmap::SetSubTexmap(int i, Texmap *m) 
{
	ReplaceReference(i,m);
	if (i==0)
	{
		stex_param_blk.InvalidateUI(tex_map1);
		ivalid.SetEmpty();
	}	
}

TSTR SkeletonTexmap::GetSubTexmapSlotName(int i) {
	switch(i) {
		case 0:  return TSTR(GetString(IDS_MAP1)); 
		default: return TSTR(_T(""));
		}
	}
	 

/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000

IOResult SkeletonTexmap::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	return IO_OK;
	}	

IOResult SkeletonTexmap::Load(ILoad *iload) { 
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


RefTargetHandle SkeletonTexmap::Clone(RemapDir &remap) {
	SkeletonTexmap *mnew = new SkeletonTexmap();
	*((MtlBase*)mnew) = *((MtlBase*)this);  // copy superclass stuff
	mnew->ReplaceReference(NSUBTEX,remap.CloneRef(pblock));

	mnew->ivalid.SetEmpty();	
	for (int i = 0; i<NSUBTEX; i++) {
		mnew->subTex[i] = NULL;
		if (subTex[i])
			mnew->ReplaceReference(i,remap.CloneRef(subTex[i]));
		mnew->mapOn[i] = mapOn[i];
		}
	return (RefTargetHandle)mnew;
	}

void SkeletonTexmap::NotifyChanged() 
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void SkeletonTexmap::Update(TimeValue t, Interval& valid) 
{		
	if (!ivalid.InInterval(t)) {

		ivalid.SetInfinite();
		pblock->GetValue( tex_map1_on, t, mapOn[0], ivalid);
		pblock->GetValue( tex_simple_param, t, simple, ivalid);

		for (int i=0; i<NSUBTEX; i++) {
			if (subTex[i]) 
				subTex[i]->Update(t,ivalid);
			}
		}
	valid &= ivalid;
}




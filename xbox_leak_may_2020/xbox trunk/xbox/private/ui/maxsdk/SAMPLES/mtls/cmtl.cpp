/**********************************************************************
 *<
	FILE: cmtl.cpp

	DESCRIPTION:  Composite material (Top-Bottom material)

	CREATED BY: Dan Silva

	HISTORY: 12/2/98 Updated to Param Block 2 Peter Watje

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mtlhdr.h"		   
#include "mtlres.h"
#include "stdmat.h"
#include "iparamm2.h"
#include "macrorec.h"

extern HINSTANCE hInstance;

#define NSUBMTLS 2

static Class_ID cmtlClassID(CMTL_CLASS_ID,0);
// Parameter block indices


class CMtl: public Mtl {
	// Animatable parameters
	Interval ivalid;
	Mtl* subMtl[NSUBMTLS];
	Mtl* useSubMtl[NSUBMTLS];

	IMtlParams *ip;

	public:
		BOOL Param1;
		BOOL mtlOn[NSUBMTLS];
		BOOL useLocalCoords;
		float thresh;
		IParamBlock2 *pblock;
		float width;

		void SetAmbient(Color c, TimeValue t) {}		
		void SetDiffuse(Color c, TimeValue t) {}		
		void SetSpecular(Color c, TimeValue t) {}
		void SetShininess(float v, TimeValue t) {}		
		void SetThresh(float v, TimeValue t);
		void SetWidth(float v, TimeValue t);
		float GetThresh() { return thresh; }
		float GetWidth() { return width; }

		Mtl *UseMtl();
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);

		CMtl();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& valid);
		void SwapInputs();
		void Init();
		void Reset();
		void CMtl::UpdateUseMtls();
		void SetMtlOn(int i, BOOL onOff);
		Interval Validity(TimeValue t);
		void NotifyChanged();

		Class_ID ClassID() {	return cmtlClassID; }
		SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
		void GetClassName(TSTR& s) { s= GetString(IDS_DS_TOPBOTTOM); }  
		void CMtl::Fixup();  //TEMPORARY
			

		void DeleteThis() { delete this; }	

		// Methods to access sub-materials of meta-materials
	   	int NumSubMtls() { return NSUBMTLS; }
		Mtl* GetSubMtl(int i) { return subMtl[i]; }
		void SetSubMtl(int i, Mtl *m);
		TSTR GetSubMtlSlotName(int i);

		int NumSubs() { return NSUBMTLS+1; }  
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) { return subNum; }

		// From ref
 		int NumRefs() { return NSUBMTLS+1;  }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock


	};


int numCMtls = 0;
class CMtlClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new CMtl; }
	const TCHAR *	ClassName() { return  GetString(IDS_DS_TOPBOTTOM_CDESC); } // mjm - 2.3.99
	SClass_ID		SuperClassID() { return MATERIAL_CLASS_ID; }
	Class_ID 		ClassID() { return cmtlClassID; }
	const TCHAR* 	Category() { return _T("");  }
// PW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("topBottomMat"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

static CMtlClassDesc cMtlCD;

ClassDesc* GetCMtlDesc() { 
	return &cMtlCD;  
	}


//dialog stuff to get the Set Ref button
class CMtlDlgProc : public ParamMap2UserDlgProc {
//public ParamMapUserDlgProc {
	public:
		CMtl *cmtl;		
		CMtlDlgProc(CMtl *m) {cmtl = m;}		
		BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
	};



BOOL CMtlDlgProc::DlgProc(
		TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
				{
				case IDC_CMTL_SWAP:
					{
					cmtl = (CMtl*)map->GetParamBlock()->GetOwner(); 
					cmtl->SwapInputs();
					cmtl->UpdateUseMtls();
					}
				break;
				}
			break;
		}
	return FALSE;
	}

//-----------------------------------------------------------------------------
//  CMtl
//-----------------------------------------------------------------------------

enum { topbottom_params, };  // pblock ID
// topbottom_params param IDs
enum 
{ 
	topbottom_map1, topbottom_map2,		
	topbottom_map1_on, topbottom_map2_on, // main grad params 
	topbottom_blend,
	topbottom_position,
	topbottom_space,

};

// per instance gradient block
static ParamBlockDesc2 topbottom_param_blk ( topbottom_params, _T("parameters"),  0, &cMtlCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_CMTL, IDS_DS_TB_PARAMS, 0, 0, NULL, 
	// params

	topbottom_map1,		_T("topMaterial"),		TYPE_MTL,			P_OWNERS_REF,	IDS_DS_TOP,
		p_refno,		1,
		p_submtlno,		0,		
		p_ui,			TYPE_MTLBUTTON, IDC_TOP_MTL,
		end,
	topbottom_map2,		_T("bottomMaterial"),		TYPE_MTL,			P_OWNERS_REF,	IDS_DS_BOTTOM,
		p_refno,		2,
		p_submtlno,		1,		
		p_ui,			TYPE_MTLBUTTON, IDC_BOT_MTL,
		end,

	topbottom_map1_on,	_T("map1Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MTL_ON1,
		end,
	topbottom_map2_on,	_T("map2Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP2ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MTL_ON2,
		end,

	topbottom_blend,	_T("blend"), TYPE_FLOAT,			P_ANIMATABLE,				IDS_DS_BLEND,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_INT, IDC_CMTL_W_EDIT, IDC_CMTL_W_SPIN, 1.0f, 
		end,
	topbottom_position,	_T("position"), TYPE_FLOAT,			P_ANIMATABLE,				IDS_PW_POSITION,
		p_default,		50.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_INT, IDC_CMTL_Y_EDIT, IDC_CMTL_Y_SPIN, 1.0f, 
		end,
	topbottom_space, _T("coordinates"), TYPE_INT,				0,				IDS_PW_COORDINATES,
		p_default,		0,
		p_range,		0,	1,
		p_ui,			TYPE_RADIO, 2, IDC_CMTL_WORLD, IDC_CMTL_LOCAL,
		end,

	end
);



static ParamBlockDescID pbdesc[] = {
	{ TYPE_FLOAT, NULL, TRUE,topbottom_position },   // thresh
	{ TYPE_FLOAT, NULL, TRUE,topbottom_blend } };   // blend

static ParamVersionDesc versions[] = {
	ParamVersionDesc(pbdesc,2,0),	
	};

void CMtl::Init() {
	ivalid.SetEmpty();
	ReplaceReference(1,NewDefaultStdMat());
	ReplaceReference(2,NewDefaultStdMat());
	GetCOREInterface()->AssignNewName(subMtl[0]);
	GetCOREInterface()->AssignNewName(subMtl[1]);

	thresh = 0.0f;
	width = .05f;
	useLocalCoords = FALSE;
	mtlOn[0] = mtlOn[1] = 1;
	}

void CMtl::Reset() {
	cMtlCD.Reset(this, TRUE);	// reset all pb2's
	Init();
	}

void CMtl::NotifyChanged() {
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}

CMtl::CMtl() {
	Param1 = FALSE;
	subMtl[0] = subMtl[1] = NULL;
	useSubMtl[0] = useSubMtl[1] = NULL;
	mtlOn[0] = mtlOn[1] = FALSE;
	pblock = NULL;
	cMtlCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
	Init();
	}

void CMtl::SwapInputs() {
	Mtl *tmp = subMtl[0];
	subMtl[0] = subMtl[1];
	subMtl[1] = tmp;

//	pblock->SwapControllers(topbottom_map1,0,topbottom_map2,0);
	topbottom_param_blk.InvalidateUI(topbottom_map1);
	topbottom_param_blk.InvalidateUI(topbottom_map2);
	macroRecorder->FunctionCall(_T("swap"), 2, 0, mr_prop, _T("topMaterial"), mr_reftarg, this, mr_prop, _T("bottomMaterial"), mr_reftarg, this);
	ip->MtlChanged();
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	}

Mtl *CMtl::UseMtl() {
	Mtl* m = NULL;
	for (int i=0; i<NSUBMTLS; i++) if (subMtl[i]) { m = subMtl[i]; break; }
	return m;
	}

// These allow the real-time renderer to display a material appearance.
// Just use the "Top material"
Color CMtl::GetAmbient(int mtlNum, BOOL backFace) { 
	return UseMtl()?UseMtl()->GetAmbient(mtlNum,backFace):Color(0,0,0);
	}		
Color CMtl::GetDiffuse(int mtlNum, BOOL backFace){ 
	return UseMtl()?UseMtl()->GetDiffuse(mtlNum,backFace):Color(0,0,0);
	}				
Color CMtl::GetSpecular(int mtlNum, BOOL backFace){
	return UseMtl()?UseMtl()->GetSpecular(mtlNum,backFace):Color(0,0,0);
	}		
float CMtl::GetXParency(int mtlNum, BOOL backFace) {
	return UseMtl()?UseMtl()->GetXParency(mtlNum,backFace):0.0f;
	}
float CMtl::GetShininess(int mtlNum, BOOL backFace) {
	return UseMtl()?UseMtl()->GetXParency(mtlNum,backFace):0.0f;
	}		
float CMtl::GetShinStr(int mtlNum, BOOL backFace) {
	return UseMtl()?UseMtl()->GetXParency(mtlNum,backFace):0.0f;
	}
float CMtl::WireSize(int mtlNum, BOOL backFace) {
	return UseMtl()?UseMtl()->WireSize(mtlNum,backFace):0.0f;
	}


RefTargetHandle CMtl::Clone(RemapDir &remap) {
	CMtl *mnew = new CMtl();
	*((MtlBase*)mnew) = *((MtlBase*)this);  // copy superclass stuff
	mnew->ReplaceReference(0,remap.CloneRef(pblock));
	mnew->thresh = thresh;
	mnew->width = width;
	mnew->ivalid.SetEmpty();	
	mnew->useLocalCoords = useLocalCoords;
	for (int i = 0; i<NSUBMTLS; i++) {
		if (subMtl[i])
			mnew->ReplaceReference(i+1,remap.CloneRef(subMtl[i]));
		mnew->mtlOn[i] = mtlOn[i];
		}
	return (RefTargetHandle)mnew;
	}

	
ParamDlg* CMtl::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	ip = imp;
	IAutoMParamDlg* masterDlg = cMtlCD.CreateParamDlgs(hwMtlEdit, imp, this);
	topbottom_param_blk.SetUserDlgProc(new CMtlDlgProc(this));
	return masterDlg;
	}

void CMtl::UpdateUseMtls() {
	for (int i=0; i<NSUBMTLS; i++) 
		useSubMtl[i] = mtlOn[i]?subMtl[i]:NULL;
	}

void CMtl::SetMtlOn(int i, BOOL onOff) {
	mtlOn[i] = onOff;
	useSubMtl[i] = mtlOn[i]?subMtl[i]:NULL;
	}

void CMtl::Update(TimeValue t, Interval& valid) {		
	if (Param1)
		{
		pblock->SetValue(topbottom_map1_on,t,mtlOn[0]);
		pblock->SetValue(topbottom_map2_on,t,mtlOn[1]);
		pblock->SetValue(topbottom_space,t,useLocalCoords);
		pblock->RescaleParam(topbottom_position, 0, 50.0f);
		pblock->RescaleParam(topbottom_blend, 0, 100.0f);

		float th;
		pblock->GetValue( topbottom_position, t, th, ivalid );
		th += 50;
		pblock->SetValue(topbottom_position,t,th);



		topbottom_param_blk.InvalidateUI(topbottom_position);
		topbottom_param_blk.InvalidateUI(topbottom_blend);

		Param1 = FALSE;
		}

	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();


		pblock->GetValue( topbottom_position, t, thresh, ivalid );
		thresh = (thresh)/50.0f-1.0f;
		pblock->GetValue(topbottom_space,t,useLocalCoords,ivalid);

		pblock->GetValue( topbottom_blend, t, width, ivalid );
		width = width * 0.01f;

		pblock->GetValue(topbottom_map1_on,t,mtlOn[0],ivalid);
		pblock->GetValue(topbottom_map2_on,t,mtlOn[1],ivalid);
		for (int i=0; i<NSUBMTLS; i++) {
			if (subMtl[i]) 
				subMtl[i]->Update(t,ivalid);
			useSubMtl[i] = mtlOn[i]?subMtl[i]:NULL;
			}
		}
	valid &=ivalid;
	}

Interval CMtl::Validity(TimeValue t) {
	Interval valid;
	Update(t,valid);
	return ivalid;
	}

void CMtl::SetThresh(float v, TimeValue t) {
	thresh = v;
	pblock->SetValue( topbottom_blend, t, v);
	NotifyChanged();
	}

void CMtl::SetWidth(float v, TimeValue t) {
	width = v;
	pblock->SetValue( topbottom_position, t, v);
	NotifyChanged();
	}

RefTargetHandle CMtl::GetReference(int i) {
	if (i==0)
		return pblock ;
	else 
		return subMtl[i-1];
	}

void CMtl::SetReference(int i, RefTargetHandle rtarg) {
	if (i==0)
		pblock = (IParamBlock2 *)rtarg;
	else 
		subMtl[i-1] = (Mtl *)rtarg;
	}

void CMtl::SetSubMtl(int i, Mtl *m) {
	ReplaceReference(i+1,m);				  
	UpdateUseMtls();

	if (i==0)
		{
		topbottom_param_blk.InvalidateUI(topbottom_map1);
		ivalid.SetEmpty();
		}
	else if (i==1)
		{
		topbottom_param_blk.InvalidateUI(topbottom_map2);
		ivalid.SetEmpty();
		}

	}

TSTR CMtl::GetSubMtlSlotName(int i) {
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DS_TOP)); 
		case 1:  return TSTR(GetString(IDS_DS_BOTTOM)); 
		default: return TSTR(_T(""));
		}
	}
	 
Animatable* CMtl::SubAnim(int i) {
	if (i==0) return pblock;
	else  return subMtl[i-1]; 
	}

TSTR CMtl::SubAnimName(int i) {
	if (i==0) return TSTR(GetString(IDS_DS_PARAMETERS));		
	else 
		return GetSubMtlTVName(i-1);
	}

RefResult CMtl::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget == pblock)
				{
			// see if this message came from a changing parameter in the pblock,
			// if so, limit rollout update to the changing item and update any active viewport texture
				ParamID changing_param = pblock->LastNotifyParamID();
				topbottom_param_blk.InvalidateUI(changing_param);
				}

			break;
		}
	return(REF_SUCCEED);
	}


inline void Clamp(Color &c) {
	if (c.r > 1.0f) c.r = 1.0f;
	if (c.g > 1.0f) c.g = 1.0f;
	if (c.b > 1.0f) c.b = 1.0f;
	}
static Color black(0,0,0);

void CMtl::Shade(ShadeContext& sc) {
	Point3 N = sc.Normal();
	if (gbufID) sc.SetGBufferID(gbufID);
	if (useLocalCoords) 
		N = sc.VectorTo(N, REF_OBJECT);
	else 
		N = sc.VectorTo(N, REF_WORLD);
	if (N.z>thresh+width) {
		if (useSubMtl[0]) useSubMtl[0]->Shade(sc);
		}
	else if (N.z<thresh-width) {
		if (useSubMtl[1]) useSubMtl[1]->Shade(sc);
		}
	else {
		float b = (N.z-thresh+width)/(2.0f*width);
		ShadeOutput out0;
		if (useSubMtl[0]) {
			useSubMtl[0]->Shade(sc);
			out0 = sc.out;
			}
		else out0.Reset();
		sc.ResetOutput();
		if (useSubMtl[1]) {
			useSubMtl[1]->Shade(sc);
			}
		sc.out.MixIn(out0,1.0f-b);
		}
	}

void CMtl::Fixup() {	
	// Old style: have to fix up the references since they changed.
	SetReference(2,GetReference(1));
	SetReference(1,GetReference(0));
	SetReference(0,NULL);
//	ReplaceReference( 0, CreateParameterBlock( pbdesc, 2 ) );	
	SetThresh(thresh,0);
	SetWidth(width,0);
	}

#define CMTL_YTHRESH_CHUNK 0x1111
#define CMTL_YWIDTH_CHUNK 0x1112
#define MTL_HDR_CHUNK 0x4000
#define CMTL_USE_LOCAL 0x4120
#define MTLOFF_CHUNK 0x1000
#define PARAM2_CHUNK 0x1020

IOResult CMtl::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	isave->BeginChunk(PARAM2_CHUNK);
	isave->EndChunk();
	return IO_OK;
	}	
	  
class CMtlCB: public PostLoadCallback {
	CMtl *mtl;
	public:
		CMtlCB(CMtl *m) { mtl = m; }
		void proc(ILoad *iload) {
			mtl->Fixup();
			delete this;
			}
	};



//watje
class TopBottomPostLoadCallback:public  PostLoadCallback
{
public:
	CMtl      *s;
	int Param1;
	TopBottomPostLoadCallback(CMtl *r, BOOL b) {s=r;Param1 = b;}
	void proc(ILoad *iload);
};

void TopBottomPostLoadCallback::proc(ILoad *iload)
{
	if (Param1)
		{
		TimeValue t  = 0;
		s->pblock->SetValue(topbottom_map1_on,t,s->mtlOn[0]);
		s->pblock->SetValue(topbottom_map2_on,t,s->mtlOn[1]);
		s->pblock->SetValue(topbottom_space,t,s->useLocalCoords);

		}
	delete this;
}


IOResult CMtl::Load(ILoad *iload) { 
	ULONG nb;
	IOResult res;
	int id;
	Param1 = TRUE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id=iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			case CMTL_YTHRESH_CHUNK:
				res = iload->Read(&thresh,sizeof(float), &nb);
				iload->SetObsolete();
				iload->RegisterPostLoadCallback(new CMtlCB(this));
				break;
			case CMTL_YWIDTH_CHUNK:
				res = iload->Read(&width,sizeof(float), &nb);
				break;
			case CMTL_USE_LOCAL:
				useLocalCoords = TRUE;
				break;
			case MTLOFF_CHUNK+0:
			case MTLOFF_CHUNK+1:
				mtlOn[id-MTLOFF_CHUNK] = 0; 
				break;
			case PARAM2_CHUNK:
				Param1 = FALSE;; 
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	// JBW: register old version ParamBlock to ParamBlock2 converter
	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, 1, &topbottom_param_blk, this, 0);
	iload->RegisterPostLoadCallback(plcb);

//	iload->RegisterPostLoadCallback(new TopBottomPostLoadCallback(this,Param1));

	return IO_OK;
                                                      
	}

float CMtl::EvalDisplacement(ShadeContext& sc) {
	Point3 N = sc.Normal();
	N = (useLocalCoords)?sc.VectorTo(N, REF_OBJECT):sc.VectorTo(N, REF_WORLD);
	if (N.z>thresh+width) {
		return useSubMtl[0]? useSubMtl[0]->EvalDisplacement(sc):0.0f;
		}
	else if (N.z<thresh-width) {
		return useSubMtl[1]? useSubMtl[1]->EvalDisplacement(sc):0.0f;
		}
	else {
		float b = (N.z-thresh+width)/(2.0f*width);
		float d0 = 0.0f;
		float d1 = 0.0f;
		if (useSubMtl[0]) {
			d0 = useSubMtl[0]->EvalDisplacement(sc);
			}
		if (useSubMtl[1]) {
			d1 = useSubMtl[1]->EvalDisplacement(sc);
			}
		return b*d0 + (1.0f-b)*d1;
//		sc.out.MixIn(out0,1.0f-b);
		}
	}


Interval CMtl::DisplacementValidity(TimeValue t) {
	Interval iv;
	iv.SetInfinite();
	if (useSubMtl[0]) 	
		iv &= useSubMtl[0]->DisplacementValidity(t);		
	if (useSubMtl[1]) 	
		iv &= useSubMtl[1]->DisplacementValidity(t);		
	return iv;	
	}

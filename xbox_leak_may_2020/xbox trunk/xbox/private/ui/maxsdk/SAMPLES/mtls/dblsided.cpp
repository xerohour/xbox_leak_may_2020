/**********************************************************************
 *<
	FILE: dblsided.cpp

	DESCRIPTION:  A double sided material

	CREATED BY: Rolf Berteig
				
	HISTORY:	Updated to Param Block2 12/1/1998 Peter Watje


 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mtlhdr.h"
#include "mtlres.h"
#include "stdmat.h"
#include "iparamm2.h"

extern HINSTANCE hInstance;

static Class_ID dblsidedClassID(DOUBLESIDED_CLASS_ID,0);
#define  NSUBMTL 2

#define PB_REF		0
#define SUB1_REF	1
#define SUB2_REF	2

enum { doublesided_params, };  // pblock ID
// doublesided_params param IDs
enum 
{ 
	doublesided_map1, doublesided_map2,		
	doublesided_map1_on, doublesided_map2_on, // main grad params 
	doublesided_transluency
};


class DoubleSided : public Mtl {	
	public:
		BOOL Param1;
		IParamBlock2 *pblock; 	// ref #0
		Mtl *sub[2];		// ref #1, 2		
		BOOL mtlOn[2];
		float trans;
		Interval ivalid;

		DoubleSided(BOOL loading);
		void NotifyChanged() {NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);}
		Mtl *UseMtl() {return sub[0]?sub[0]:sub[1];}

		// From MtlBase and Mtl
		void SetAmbient(Color c, TimeValue t) {}		
		void SetDiffuse(Color c, TimeValue t) {}		
		void SetSpecular(Color c, TimeValue t) {}
		void SetShininess(float v, TimeValue t) {}				
		
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);
				
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& valid);
		void Init();
		void Reset();
		Interval Validity(TimeValue t);
		
		Class_ID ClassID() {return dblsidedClassID; }
		SClass_ID SuperClassID() {return MATERIAL_CLASS_ID;}
		void GetClassName(TSTR& s) {s = GetString(IDS_RB_DOUBLESIDED);}  

		void DeleteThis() {delete this;}	

		// Methods to access sub-materials of meta-materials
	   	int NumSubMtls() {return 2;}
		void SetSubMtl(int i, Mtl *m);

		Mtl* GetSubMtl(int i) {return sub[i];}
		TSTR GetSubMtlSlotName(int i) {return i?GetString(IDS_RB_BACK):GetString(IDS_RB_FACING);}

		ULONG LocalRequirements(int subMtlNum) {  return  MTLREQ_2SIDE; }

		int NumSubs() {return 3;} 
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		// From ref
 		int NumRefs() {return 3;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		// IO
		IOResult Save(ISave *isave); 
		IOResult Load(ILoad *iload); 
// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	};

class DoubleSidedClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new DoubleSided(loading);}
	const TCHAR *	ClassName() {return GetString(IDS_RB_DOUBLESIDED_CDESC); } // mjm - 2.3.99
	SClass_ID		SuperClassID() {return MATERIAL_CLASS_ID;}
	Class_ID 		ClassID() {return dblsidedClassID;}
	const TCHAR* 	Category() {return _T("");}
// PW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("doubleSidedMat"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle

	};
static DoubleSidedClassDesc dblsidedCD;
ClassDesc* GetDoubleSidedDesc() {return &dblsidedCD;}





// per instance gradient block
static ParamBlockDesc2 doublesided_param_blk ( doublesided_params, _T("parameters"),  0, &dblsidedCD, P_AUTO_CONSTRUCT + P_AUTO_UI, PB_REF, 
	//rollout
	IDD_DOUBLESIDED, IDS_DS_DBLSIDED_PARAMS, 0, 0, NULL, 
	// params
	doublesided_map1,		_T("material1"),		TYPE_MTL,			P_OWNERS_REF,	IDS_RB_MATERIALONE,
		p_refno,		1,
		p_submtlno,		0,		
		p_ui,			TYPE_MTLBUTTON, IDC_2SIDE_MAT1,
		end,
	doublesided_map2,		_T("material2"),		TYPE_MTL,			P_OWNERS_REF,	IDS_RB_MATERIALTWO,
		p_refno,		2,
		p_submtlno,		1,		
		p_ui,			TYPE_MTLBUTTON, IDC_2SIDE_MAT2,
		end,
	doublesided_map1_on,	_T("map1Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON1,
		end,
	doublesided_map2_on,	_T("map2Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP2ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON2,
		end,
	doublesided_transluency,	_T("translucency"), TYPE_FLOAT,		P_ANIMATABLE,	IDS_RB_TRANSLUECENCY,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_2SIDE_TRANSLUCENCY, IDC_2SIDE_TRANSLUCENCYSPIN, 0.1f, 
		end,
	end
);


//--- DoubleSided Material -------------------------------------------------

static ParamBlockDescID pbdesc[1] = {
	{TYPE_FLOAT, NULL, TRUE,doublesided_transluency }
	};   // Translucency
static ParamVersionDesc versions[1] = {
	ParamVersionDesc(pbdesc,1,0),	
	};

DoubleSided::DoubleSided(BOOL loading)
	{	
	Param1 = FALSE;
	pblock = NULL;
	sub[0] = sub[1] = NULL;	
	mtlOn[0] = mtlOn[1] = 1;
	ivalid.SetEmpty();
	if (!loading) {
		dblsidedCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
		Init();
		}
	}

void DoubleSided::Init()
	{
	ReplaceReference(SUB1_REF, NewDefaultStdMat());
	ReplaceReference(SUB2_REF, NewDefaultStdMat());
	GetCOREInterface()->AssignNewName(sub[0]);
	GetCOREInterface()->AssignNewName(sub[1]);
	mtlOn[0] = mtlOn[1] = 1;
	}

void DoubleSided::Reset()
	{
	dblsidedCD.Reset(this, TRUE);	// reset all pb2's
	Init();
	}

Color DoubleSided::GetAmbient(int mtlNum, BOOL backFace) { 
	if (backFace&&sub[1]) return sub[1]->GetAmbient(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetAmbient(mtlNum);
	return UseMtl()?UseMtl()->GetAmbient():Color(0,0,0);
	}		
Color DoubleSided::GetDiffuse(int mtlNum, BOOL backFace){ 
	if (backFace&&sub[1]) return sub[1]->GetDiffuse(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetDiffuse(mtlNum);
	return UseMtl()?UseMtl()->GetDiffuse(mtlNum,backFace):Color(0,0,0);
	}				
Color DoubleSided::GetSpecular(int mtlNum, BOOL backFace){
	if (backFace&&sub[1]) return sub[1]->GetSpecular(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetSpecular(mtlNum);
	return UseMtl()?UseMtl()->GetSpecular(mtlNum,backFace):Color(0,0,0);
	}		
float DoubleSided::GetXParency(int mtlNum, BOOL backFace) {
	if (backFace&&sub[1]) return sub[1]->GetXParency(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetXParency(mtlNum);
	return UseMtl()?UseMtl()->GetXParency(mtlNum,backFace):0.0f;
	}
float DoubleSided::GetShininess(int mtlNum, BOOL backFace) {
	if (backFace&&sub[1]) return sub[1]->GetShininess(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetShininess(mtlNum);
	return UseMtl()?UseMtl()->GetShininess(mtlNum,backFace):0.0f;
	}		
float DoubleSided::GetShinStr(int mtlNum, BOOL backFace) {
	if (backFace&&sub[1]) return sub[1]->GetShinStr(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->GetShinStr(mtlNum);
	return UseMtl()?UseMtl()->GetShinStr(mtlNum,backFace):0.0f;
	}

float DoubleSided::WireSize(int mtlNum, BOOL backFace) {
	if (backFace&&sub[1]) return sub[1]->WireSize(mtlNum);
	if (!backFace&&sub[0]) return sub[0]->WireSize(mtlNum);
	return UseMtl()?UseMtl()->WireSize(mtlNum,backFace):0.0f;
	}

		
ParamDlg* DoubleSided::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
	{
	IAutoMParamDlg* masterDlg = dblsidedCD.CreateParamDlgs(hwMtlEdit, imp, this);
	return masterDlg;
	}

static Color black(0,0,0);

void DoubleSided::Shade(ShadeContext& sc)
	{
	if (gbufID) sc.SetGBufferID(gbufID);
	Mtl *m1, *m2, *m = UseMtl();
	if (!m) return;
	if (!sc.backFace) {	
		m1 = mtlOn[0]?sub[0]:NULL;
		m2 = mtlOn[1]?sub[1]:NULL;
	} else {
		m1 = mtlOn[1]?sub[1]:NULL;
		m2 = mtlOn[0]?sub[0]:NULL;
		}
	if (trans==0.0f) {
		if (m1) m1->Shade(sc);
	} else {		
		BOOL bf = sc.backFace;
		ShadeOutput out1;
		if(m1) {
			m1->Shade(sc);
			out1 = sc.out;
			}
		else out1.Reset();
		sc.backFace = !sc.backFace;
		if (m2) {
			m2->Shade(sc);
			}
		else sc.out.Reset();
		sc.backFace = bf;
		sc.out.MixIn(out1,trans);
		}
	}


void DoubleSided::SetSubMtl(int i, Mtl *m)
	{ 
	ReplaceReference(i+1,m); 
	if (i==0)
		{
		doublesided_param_blk.InvalidateUI(doublesided_map1);
		ivalid.SetEmpty();
		}
	else if (i==1)
		{
		doublesided_param_blk.InvalidateUI(doublesided_map2);
		ivalid.SetEmpty();
		}	
	}


void DoubleSided::Update(TimeValue t, Interval& valid)
	{	

/*
	if (Param1) // 2.5 load hack fix
		{
		pblock->SetValue(doublesided_map1_on,t,mtlOn[0]);
		pblock->SetValue(doublesided_map2_on,t,mtlOn[1]);
		Param1 = FALSE;
		}
*/

	ivalid = FOREVER;
	for (int i=0; i<2; i++) 
		if (sub[i]) sub[i]->Update(t,valid);
	pblock->GetValue(doublesided_transluency,t,trans,ivalid);
	trans = trans * 0.01f;
	pblock->GetValue(doublesided_map1_on,t,mtlOn[0],ivalid);
	pblock->GetValue(doublesided_map2_on,t,mtlOn[1],ivalid);
	valid &= ivalid;
	}

Interval DoubleSided::Validity(TimeValue t)
	{
	Interval valid = FOREVER;	
	float f;
	for (int i=0; i<2; i++) 
		if (sub[i]) valid &= sub[i]->Validity(t);
	pblock->GetValue(doublesided_transluency,t,f,valid);
	return valid;
	}

Animatable* DoubleSided::SubAnim(int i)
	{
	switch (i) {
		case 0: return pblock;
		case 1: return sub[0];
		case 2: return sub[1];
		default: return NULL;
		}
	}

TSTR DoubleSided::SubAnimName(int i)
	{
	switch (i) {
		case 0: return GetString(IDS_DS_PARAMETERS);
		case 1: return GetString(IDS_RB_FACINGMAT);
		case 2: return GetString(IDS_RB_BACKMATERIAL);
		default: return _T("");
		}
	}

RefTargetHandle DoubleSided::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock;
		case 1: return sub[0];
		case 2: return sub[1];
		default: return NULL;
		}
	}

void DoubleSided::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock = (IParamBlock2*)rtarg; break;
		case 1: sub[0] = (Mtl*)rtarg; break;
		case 2: sub[1] = (Mtl*)rtarg; break;
		}
	}

RefTargetHandle DoubleSided::Clone(RemapDir &remap)
	{
	DoubleSided *mtl = new DoubleSided(FALSE);
	*((MtlBase*)mtl) = *((MtlBase*)this);  // copy superclass stuff
	mtl->ReplaceReference(PB_REF,remap.CloneRef(pblock));
	if (sub[0]) mtl->ReplaceReference(SUB1_REF,remap.CloneRef(sub[0]));
	if (sub[1]) mtl->ReplaceReference(SUB2_REF,remap.CloneRef(sub[1]));
	for (int i=0; i<NSUBMTL; i++) 
		mtl->mtlOn[i] = mtlOn[i];
	return mtl;
	}

RefResult DoubleSided::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget == pblock)
				{
				ParamID changing_param = pblock->LastNotifyParamID();
				doublesided_param_blk.InvalidateUI(changing_param);
				}

			break;
		}
	return REF_SUCCEED;
	}

//
// Note: ALL Materials and texmaps must have a Save and Load to save and load
// the MtlBase info.
#define MTL_HDR_CHUNK 0x4000
#define MTLOFF_CHUNK 0x1000
#define PARAM2_CHUNK 0x1010

IOResult DoubleSided::Save(ISave *isave) { 
	IOResult res;
	// Save common stuff
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	isave->BeginChunk(PARAM2_CHUNK);
	isave->EndChunk();


	return IO_OK;
	}

//watje
class DoubleSidedPostLoadCallback:public  PostLoadCallback
{
public:
	DoubleSided      *s;
	int Param1;
	DoubleSidedPostLoadCallback(DoubleSided *r, BOOL b) {s=r;Param1 = b;}
	void proc(ILoad *iload);
};

void DoubleSidedPostLoadCallback::proc(ILoad *iload)
{
	if (Param1)
		{
		TimeValue t  = 0;
		s->pblock->SetValue(doublesided_map1_on,t,s->mtlOn[0]);
		s->pblock->SetValue(doublesided_map2_on,t,s->mtlOn[1]);

		Interval iv;
		s->pblock->GetValue(doublesided_transluency,t,s->trans,iv);
		s->trans *= 100.0f;
		s->pblock->SetValue(doublesided_transluency,t,s->trans);

		}
	delete this;
}
	
	  

IOResult DoubleSided::Load(ILoad *iload) { 
//	ULONG nb;
	IOResult res;
	int id;
	Param1 = TRUE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			case MTLOFF_CHUNK+0:
			case MTLOFF_CHUNK+1:
				mtlOn[id-MTLOFF_CHUNK] = 0; 
				break;
			case PARAM2_CHUNK:
				Param1 = FALSE;
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	if (Param1)
	{
		ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, 1, &doublesided_param_blk, this, PB_REF);
		iload->RegisterPostLoadCallback(plcb);

		DoubleSidedPostLoadCallback* doubleSidedplcb = new DoubleSidedPostLoadCallback(this,Param1);
		iload->RegisterPostLoadCallback(doubleSidedplcb);
	}

	return IO_OK;
	}


float DoubleSided::EvalDisplacement(ShadeContext& sc) {
	Mtl *m1, *m2, *m = UseMtl();
	if (!m) return 0.0f;
	if (!sc.backFace) {	
		m1 = mtlOn[0]?sub[0]:NULL;
		m2 = mtlOn[1]?sub[1]:NULL;
	} else {
		m1 = mtlOn[1]?sub[1]:NULL;
		m2 = mtlOn[0]?sub[0]:NULL;
		}
	if (trans==0.0f) {
		return (m1)? m1->EvalDisplacement(sc):0.0f;
	} else {		
		BOOL bf = sc.backFace;
		float d1 = (m1)?m1->EvalDisplacement(sc):0.0f;
		sc.backFace = !sc.backFace;
		float d2 = (m2)?m2->EvalDisplacement(sc):0.0f;
		sc.backFace = bf;
		return (1.0f-trans)*d1 + trans*d2;
		}
	}

Interval DoubleSided::DisplacementValidity(TimeValue t) {
	Interval iv;
	iv.SetInfinite();
	if (mtlOn[0]&&sub[0]) 	
		iv &= sub[0]->DisplacementValidity(t);		
	if (mtlOn[1]&&sub[1]) 	
		iv &= sub[1]->DisplacementValidity(t);		
	return iv;	
	} 

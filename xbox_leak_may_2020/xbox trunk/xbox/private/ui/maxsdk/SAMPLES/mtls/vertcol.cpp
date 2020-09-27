/**********************************************************************
 *<
	FILE: VERTCOL.CPP

	DESCRIPTION: Return interpolated vertex color

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mtlhdr.h"
#include "mtlres.h"
#include "stdmat.h"

extern HINSTANCE hInstance;

static LRESULT CALLBACK CurveWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );



static Class_ID vcolClassID(VCOL_CLASS_ID,0);


class VCol;

class VColDlg: public ParamDlg {
	public:
		HWND hwmedit;	 // window handle of the materials editor dialog
		IMtlParams *ip;
		VCol *theTex;	 // current VCol being edited.
		HWND hPanel; // Rollup pane
		TimeValue curTime; 
		int isActive;
		BOOL valid;

		//-----------------------------
		VColDlg(HWND hwMtlEdit, IMtlParams *imp, VCol *m); 
		~VColDlg();
		BOOL PanelProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

		void LoadDialog(BOOL draw);  // stuff params into dialog
		void ReloadDialog();
		void UpdateMtlDisplay() { ip->MtlChanged(); }
		void ActivateDlg(BOOL onOff);
		void Invalidate() { valid = FALSE;	InvalidateRect(hPanel,NULL,0); }

		// methods inherited from ParamDlg:
		Class_ID ClassID() {return vcolClassID;  }
		void SetThing(ReferenceTarget *m);
		ReferenceTarget* GetThing() { return (ReferenceTarget *)theTex; }
		void DeleteThis() { delete this;  }	
		void SetTime(TimeValue t);
	};



//--------------------------------------------------------------
// VCol: A Composite texture map
//--------------------------------------------------------------
class VCol: public Texmap { 
	friend class VColDlg;
	VColDlg *paramDlg;
	Interval ivalid;
	BOOL useUVW;
	public:
		VCol();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		ULONG Requirements(int subMtlNum) { return useUVW?MTLREQ_UV:MTLREQ_UV2; }
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }
		void NotifyChanged();

		// Evaluate the color of map for the context.
		AColor EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);

		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		Point3 EvalNormalPerturb(ShadeContext& sc);

		Class_ID ClassID() {	return vcolClassID; }
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) { s= GetString(IDS_DS_VCOL); }  
		void DeleteThis() { delete this; }	

		int NumSubs() { return 0; }  

		// From ref
 		int NumRefs() { return 0; }

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

	};

class VColClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new VCol; }
	const TCHAR *	ClassName() { return GetString(IDS_DS_VCOL_CDESC); } // mjm - 2.3.99
	SClass_ID		SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID() { return vcolClassID; }
	const TCHAR* 	Category() { return TEXMAP_CAT_COLMOD;  }
	};

static VColClassDesc vcolCD;

ClassDesc* GetVColDesc() { return &vcolCD;  }

static BOOL CALLBACK  PanelDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	VColDlg *theDlg;
	if (msg==WM_INITDIALOG) {
		theDlg = (VColDlg*)lParam;
		theDlg->hPanel = hwndDlg;
		SetWindowLong(hwndDlg, GWL_USERDATA,lParam);
		}
	else {
	    if ( (theDlg = (VColDlg *)GetWindowLong(hwndDlg, GWL_USERDATA) ) == NULL )
			return FALSE; 
		}
	theDlg->isActive = 1;
	int	res = theDlg->PanelProc(hwndDlg,msg,wParam,lParam);
	theDlg->isActive = 0;
	return res;
	}

VColDlg::VColDlg(HWND hwMtlEdit, IMtlParams *imp, VCol *m) { 
	hwmedit = hwMtlEdit;
	ip = imp;
	hPanel = NULL;
	theTex = m; 
	isActive = 0;
	valid = FALSE;
	hPanel = ip->AddRollupPage( 
		hInstance,
		MAKEINTRESOURCE(IDD_VCOL1),
		PanelDlgProc, 
		GetString(IDS_DS_VCOLPARAMS), 
		(LPARAM)this );		
	curTime = imp->GetTime();
	}

void VColDlg::ReloadDialog() {
	Interval valid;
	theTex->Update(curTime, valid);
	LoadDialog(FALSE);
	}

void VColDlg::SetTime(TimeValue t) {
	Interval valid;
	if (t!=curTime) {
		curTime = t;
		theTex->Update(curTime, valid);
		LoadDialog(FALSE);
		InvalidateRect(hPanel,NULL,0);
		}
	}

VColDlg::~VColDlg() {
	theTex->paramDlg = NULL;
	SetWindowLong(hPanel, GWL_USERDATA, NULL);
	}


BOOL VColDlg::PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ) {
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
    switch (msg)    {
		case WM_INITDIALOG:
			{
			ShowWindow(GetDlgItem(hwndDlg,IDC_VC_VC),SW_HIDE);
			ShowWindow(GetDlgItem(hwndDlg,IDC_VC_UVW),SW_HIDE);
//			CheckRadioButton( hwndDlg, IDC_VC_VC, IDC_VC_UVW, IDC_VC_VC+theTex->useUVW);
			return TRUE;
			}
			break;
		case WM_COMMAND:  

//		    switch (id) {
//				case IDC_VC_VC:
//				case IDC_VC_UVW:
//					CheckRadioButton( hwndDlg, IDC_VC_VC, IDC_VC_UVW, id);
//					theTex->useUVW = id-IDC_VC_VC;
//					theTex->NotifyChanged();
//					break;
//				}
			break;
		case WM_PAINT: 	
			if (!valid) {
				valid = TRUE;
				ReloadDialog();
				}
			break;
		case WM_CLOSE: 	break;       
		case WM_DESTROY:  	break;
		case CC_SPINNER_CHANGE: 
			break;
		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP: 
			theTex->NotifyChanged();
		    UpdateMtlDisplay();
			break;

    	}
	return FALSE;
	}


void VColDlg::LoadDialog(BOOL draw) {
	if (theTex) {
		Interval valid;
		theTex->Update(curTime,valid);
//		CheckRadioButton( hPanel, IDC_VC_VC, IDC_VC_UVW, IDC_VC_VC+theTex->useUVW);
		}
	}

void VColDlg::SetThing(ReferenceTarget *m) {
	assert (m->ClassID()==vcolClassID);
	assert (m->SuperClassID()==TEXMAP_CLASS_ID);
	if (theTex) theTex->paramDlg = NULL;
	theTex = (VCol *)m;
	if (theTex) theTex->paramDlg = this;
	LoadDialog(TRUE);
	}

void VColDlg::ActivateDlg(BOOL onOff) {
	}


//-----------------------------------------------------------------------------
//  VCol
//-----------------------------------------------------------------------------

#define VCOL_VERSION 1


void VCol::Reset() {
	useUVW = FALSE;
	ivalid.SetEmpty();
	}

void VCol::NotifyChanged() {
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}

VCol::VCol() {
	paramDlg = NULL;
	Reset();
	}

AColor VCol::EvalColor(ShadeContext& sc) {
	if (gbufID) sc.SetGBufferID(gbufID);
//	Point3 p = sc.UVW(1-useUVW);
	Point3 p = sc.UVW(0);
	return AColor(p.x,p.y,p.z,1.0f);
	}

float VCol::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
	}

Point3 VCol::EvalNormalPerturb(ShadeContext& sc) {
	return Point3(0,0,0);
	}

RefTargetHandle VCol::Clone(RemapDir &remap) {
	VCol *mnew = new VCol();
	*((MtlBase*)mnew) = *((MtlBase*)this);  // copy superclass stuff
	return (RefTargetHandle)mnew;
	}

ParamDlg* VCol::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	VColDlg *dm = new VColDlg(hwMtlEdit, imp, this);
	dm->LoadDialog(TRUE);	
	paramDlg = dm;
	return dm;	
	}


void VCol::Update(TimeValue t, Interval& valid) {		
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();
		}
	valid &= ivalid;
	}


RefResult VCol::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
//			if (paramDlg&&!paramDlg->isActive) 
			if (paramDlg) 
					paramDlg->Invalidate();
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
//			gpn->name= TSTR(GetString(name_id[gpn->index]));
			return REF_STOP; 
			}
		}
	return(REF_SUCCEED);
	}


#define MTL_HDR_CHUNK 0x4000
#define USE_UVW_CHUNK 0x5000

IOResult VCol::Save(ISave *isave) { 
	IOResult res;
	// Save common stuff
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();
	if (useUVW) {
		isave->BeginChunk(USE_UVW_CHUNK);
		isave->EndChunk();
		}
	return IO_OK;
	}	
	  

IOResult VCol::Load(ILoad *iload) { 
//	ULONG nb;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
//			case USE_UVW_CHUNK:
//				useUVW = TRUE;
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}



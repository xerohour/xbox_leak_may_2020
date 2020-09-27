/**********************************************************************
 *<
	FILE: multi.cpp

	DESCRIPTION:  Composite material

	CREATED BY: Dan Silva

	HISTORY: UPdated to Param2 1/11/98 Peter Watje

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mtlhdr.h"
#include "mtlres.h"
#include "stdmat.h"
#include "iparamm2.h"
#include "macrorec.h"

extern HINSTANCE hInstance;

#define NSUBMTLS 10
static Class_ID multiClassID(MULTI_CLASS_ID,0);

class Multi;
class MultiDlg;



#define PBLOCK_REF	0
#define MTL_REF		1

class MultiDlg: public ParamDlg {
	public:		
		HWND hwmedit;	 // window handle of the materials editor dialog
		IMtlParams *ip;
		Multi *theMtl;	 // current mtl being edited.
		HWND hPanelBasic; // Rollup pane		
		HWND hScroll;
		TimeValue curTime; 
		int isActive;
		BOOL valid;
		ICustButton *iBut[NSUBMTLS];
		ICustEdit *iName[NSUBMTLS];
		MtlDADMgr dadMgr;
				
		MultiDlg(HWND hwMtlEdit, IMtlParams *imp, Multi *m); 
		~MultiDlg();
		BOOL BasicPanelProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
		void VScroll(int code, short int cpos );
		void UpdateSubMtlNames();
		void UpdateColorSwatches();
		void LoadDialog(BOOL draw);  // stuff params into dialog
		void Invalidate() {
			valid = FALSE;
			Rect rect;
			rect.left = rect.top = 0;
			rect.right = rect.bottom = 10;
			InvalidateRect(hPanelBasic,&rect,FALSE);
			}
		void ReloadDialog();
		void UpdateMtlDisplay() { ip->MtlChanged(); }
		void ActivateDlg(BOOL onOff) {}
		void SetNumMats(HWND hWnd);
		void DragAndDrop(int ifrom, int ito);
		int SubMtlNumFromNameID(int id);

		// methods inherited from ParamDlg:
		Class_ID ClassID() {return multiClassID;  }
		void SetThing(ReferenceTarget *m);
		ReferenceTarget* GetThing() { return (ReferenceTarget *)theMtl; }
		void DeleteThis() { delete this;  }	
		void SetTime(TimeValue t);
		int FindSubMtlFromHWND(HWND hw);
	};


// Parameter block indices
#define PB_THRESH		0
#define PB_WIDTH		1

class Multi: public MultiMtl {
	friend class MultiDlg;
	friend class SetNumMtlsRestore;
	// Animatable parameters
	Interval ivalid;
	MultiDlg *paramDlg;
	int offset;
	public:
		NameTab subNames;
		Tab<Mtl *>subMtl;
//		Tab<BOOL>mapOn;

		BOOL Param1;
		IParamBlock2 *pblock;   // ref #0		

		BOOL loadingOld;
		void SetNumSubMtls(int n);
		void ClampOffset();

		void SetAmbient(Color c, TimeValue t) {}		
		void SetDiffuse(Color c, TimeValue t) {}		
		void SetSpecular(Color c, TimeValue t) {}
		void SetShininess(float v, TimeValue t) {}		
		void SetThresh(float v, TimeValue t);
		void SetWidth(float v, TimeValue t);

		Mtl *UseMtl();
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);

		Multi(BOOL loading, BOOL createDefaultSubMtls = TRUE); // mjm - 10.11.99 - added createDefaultSubMtls parameter
		void SetParamDlg( MultiDlg *pd) { paramDlg = pd; }
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& valid);
		void Init();
		void Reset();
		Interval Validity(TimeValue t);
		void NotifyChanged();

		Class_ID ClassID() {	return multiClassID; }
		SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_MULTISUBOBJECT); }  

		void DeleteThis() { delete this; }	

		// Methods to access sub-materials of meta-materials
	   	int NumSubMtls() { return subMtl.Count(); }
		Mtl* GetSubMtl(int i) { return subMtl[i]; }
		void SetSubMtl(int i, Mtl *m);
		TSTR GetSubMtlSlotName(int i);
		BOOL IsMultiMtl() { return TRUE; }

		int NumSubs() { return subMtl.Count(); }  
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) { return subNum+1; }

		// From ref
		int NumRefs() { return loadingOld?subMtl.Count()+1: subMtl.Count()+1;  }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		int RemapRefOnLoad(int iref) ;

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


int numMultis = 0;
class MultiClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new Multi(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_RB_MULTISUBOBJECT_CDESC); } // mjm - 2.3.99
	SClass_ID		SuperClassID() { return MATERIAL_CLASS_ID; }
	Class_ID 		ClassID() { return multiClassID; }
	const TCHAR* 	Category() { return _T("");  }
// PW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("multiSubMaterial"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle

	};

static MultiClassDesc multiCD;

ClassDesc* GetMultiDesc() { 
	return &multiCD;  
	}


//-----------------------------------------------------------------------------
//  Multi
//-----------------------------------------------------------------------------
// JBW: IDs for ParamBlock2 blocks and parameters
// Parameter and ParamBlock IDs
enum { multi_params, };  // pblock ID
// multi_params param IDs
enum 
{ 
	multi_mtls,
	multi_ons,
	multi_names,

};

// per instance gradient block
static ParamBlockDesc2 multi_param_blk ( multi_params, _T("parameters"),  0, &multiCD, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_MULTI, IDS_DS_MULTI_PARAMS, 0, 0, NULL, 
	// params
	multi_mtls,	_T("materialList"),	TYPE_MTL_TAB,	10,		P_OWNERS_REF + P_VARIABLE_SIZE,	IDS_RB_MATERIAL2,	
		p_refno,		MTL_REF, 
		end,
	multi_ons,	_T("mapEnabled"), TYPE_BOOL_TAB,	10,		P_VARIABLE_SIZE,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		end,
	multi_names, _T("names"), TYPE_STRING_TAB,		10,		P_VARIABLE_SIZE,				IDS_DS_MAP,
		end,


	end
);


static BOOL CALLBACK  PanelDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	MultiDlg *theDlg;
	if (msg==WM_INITDIALOG) {
		theDlg = (MultiDlg*)lParam;
		theDlg->hPanelBasic = hwndDlg;
		SetWindowLong(hwndDlg, GWL_USERDATA,lParam);
		}
	else {
	    if ( (theDlg = (MultiDlg *)GetWindowLong(hwndDlg, GWL_USERDATA) ) == NULL )
			return FALSE; 
		}
	theDlg->isActive = 1;
	int	res = theDlg->BasicPanelProc(hwndDlg,msg,wParam,lParam);
	theDlg->isActive = 0;
	return res;
	}


int MultiDlg::FindSubMtlFromHWND(HWND hw) {
	for (int i=0; i<NSUBMTLS; i++) {
		if (hw == iBut[i]->GetHwnd()) return i+theMtl->offset;
		}	
	return -1;
	}

void MultiDlg::DragAndDrop(int ifrom, int ito) {
	theMtl->CopySubMtl(hPanelBasic,ifrom+theMtl->offset, ito+theMtl->offset);
	theMtl->NotifyChanged();
	UpdateMtlDisplay();
	}

//-------------------------------------------------------------------

MultiDlg::MultiDlg(HWND hwMtlEdit, IMtlParams *imp, Multi *m) { 
	dadMgr.Init(this);
	hwmedit = hwMtlEdit;
	ip = imp;
	hPanelBasic = NULL;
	theMtl = m; 
	isActive = 0;
	valid = FALSE;
	theMtl->ClampOffset();
	for (int i=0; i<NSUBMTLS; i++) iBut[i] = NULL;
	for (i=0; i<NSUBMTLS; i++) iName[i] = NULL;
	hPanelBasic = ip->AddRollupPage( 
		hInstance,
		MAKEINTRESOURCE(IDD_MULTI),
		PanelDlgProc, 
		GetString(IDS_DS_MULTI_PARAMS), 
		(LPARAM)this );		
	curTime = imp->GetTime();
	}

void MultiDlg::ReloadDialog() {
	Interval valid;
	theMtl->Update(curTime,valid);
	LoadDialog(FALSE);
	}

void MultiDlg::SetTime(TimeValue t) {
	if (t!=curTime) {
		Interval valid;
		curTime = t;
		theMtl->Update(curTime,valid);
		// Since nothing is time varying, can skip this
		//InvalidateRect(hPanelBasic,NULL,0);
		}
	}

MultiDlg::~MultiDlg() {
	theMtl->SetParamDlg(NULL);	
	SetWindowLong(hPanelBasic, GWL_USERDATA, NULL);
	hPanelBasic =  NULL;
	for (int i=0; i<NSUBMTLS; i++) {
		ReleaseICustButton(iBut[i]);
		ReleaseICustEdit(iName[i]);
		iBut[i] = NULL; 
		iName[i] = NULL;
		}
	}


static int subMtlId[NSUBMTLS] = {
	IDC_MULTI_MTL0,
	IDC_MULTI_MTL1,
	IDC_MULTI_MTL2,
	IDC_MULTI_MTL3,
	IDC_MULTI_MTL4,
	IDC_MULTI_MTL5,
	IDC_MULTI_MTL6,
	IDC_MULTI_MTL7,
	IDC_MULTI_MTL8,
	IDC_MULTI_MTL9
	};


static int subNameId[NSUBMTLS] = {
	IDC_MTL_NAME0,
	IDC_MTL_NAME1,
	IDC_MTL_NAME2,
	IDC_MTL_NAME3,
	IDC_MTL_NAME4,
	IDC_MTL_NAME5,
	IDC_MTL_NAME6,
	IDC_MTL_NAME7,
	IDC_MTL_NAME8,
	IDC_MTL_NAME9
	};

static int mapOnIDs[] = {
	IDC_MAPON1,
	IDC_MAPON2,
	IDC_MAPON3,
	IDC_MAPON4,
	IDC_MAPON5,
	IDC_MAPON6,
	IDC_MAPON7,
	IDC_MAPON8,
	IDC_MAPON9,
	IDC_MAPON10,
	};
static int numIDs[] = {
	IDC_MULT_NUM1,
	IDC_MULT_NUM2,
	IDC_MULT_NUM3,
	IDC_MULT_NUM4,
	IDC_MULT_NUM5,
	IDC_MULT_NUM6,
	IDC_MULT_NUM7,
	IDC_MULT_NUM8,
	IDC_MULT_NUM9,
	IDC_MULT_NUM10,
	};	

int MultiDlg::SubMtlNumFromNameID(int id) {
	for (int i=0; i<NSUBMTLS; i++) {
		if (subNameId[i]==id) return i;
		}
	return 0;
	}

void MultiDlg::VScroll(int code, short int cpos ) {
	for (int i=0; i<NSUBMTLS; i++) {
		if (iName[i]->HasFocus()) 
			SetFocus(NULL);
		}
	switch (code) {
		case SB_LINEUP: 	theMtl->offset--;		break;
		case SB_LINEDOWN:	theMtl->offset++;		break;
		case SB_PAGEUP:		theMtl->offset -= NSUBMTLS;	break;
		case SB_PAGEDOWN:	theMtl->offset += NSUBMTLS;	break;
		
		case SB_THUMBPOSITION: 
		case SB_THUMBTRACK:
			theMtl->offset = cpos;
			break;
		}

	theMtl->ClampOffset();
	UpdateSubMtlNames();						
	}

BOOL MultiDlg::BasicPanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ) {
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
    switch (msg)    {
		case WM_INITDIALOG:	 {		
			hScroll	= GetDlgItem(hwndDlg,IDC_MULTI_SCROLL);
			SetScrollRange(hScroll,SB_CTL,0,theMtl->NumSubMtls()-NSUBMTLS,FALSE);
			SetScrollPos(hScroll,SB_CTL,theMtl->offset,TRUE);
			EnableWindow(hScroll,theMtl->subMtl.Count()>NSUBMTLS);
			for (int i=0; i<NSUBMTLS; i++) {
				iBut[i] = GetICustButton(GetDlgItem(hwndDlg,subMtlId[i]));
				iBut[i]->SetDADMgr(&dadMgr);
				iName[i] = GetICustEdit( GetDlgItem(hwndDlg,subNameId[i]));
				iName[i]->SetLeading(2); //??
				if (i+theMtl->offset<theMtl->NumSubMtls()) {
					TCHAR *name;
					Interval iv;
					theMtl->pblock->GetValue(multi_names,0,name,iv,i+theMtl->offset);
					iName[i]->SetText(name);
//					iName[i]->SetText(theMtl->subNames[i+theMtl->offset]);
					}
				TSTR buf;
				buf.printf("%d:",i+theMtl->offset+1);
				SetDlgItemText(hwndDlg,numIDs[i],buf);
				int onCount = theMtl->pblock->Count(multi_ons);
				if (i-theMtl->offset<onCount)
					{
					int on;
					Interval iv;
					theMtl->pblock->GetValue(multi_ons,0,on,iv,i+theMtl->offset);
					SetCheckBox(hwndDlg, mapOnIDs[i], on);
					}
//				if (i-theMtl->offset<theMtl->mapOn.Count())
//					SetCheckBox(hwndDlg, mapOnIDs[i], theMtl->mapOn[i+theMtl->offset]);
				}
			}
			return TRUE;
		case WM_VSCROLL:
			VScroll(LOWORD(wParam),(short int)HIWORD(wParam));
			break;
			
		case WM_COMMAND:  
		    switch (id) {
				case IDC_MULTI_MTL0: 
				case IDC_MULTI_MTL1: 
				case IDC_MULTI_MTL2: 
				case IDC_MULTI_MTL3: 
				case IDC_MULTI_MTL4: 
				case IDC_MULTI_MTL5: 
				case IDC_MULTI_MTL6: 
				case IDC_MULTI_MTL7: 
				case IDC_MULTI_MTL8: 
				case IDC_MULTI_MTL9: 
					if (id-IDC_MULTI_MTL0 + theMtl->offset < theMtl->subMtl.Count()) {
						PostMessage(hwmedit,WM_SUB_MTL_BUTTON, 
							id-IDC_MULTI_MTL0 + theMtl->offset ,(LPARAM)theMtl);
						}
					break;
				
				case IDC_MAPON1:							
				case IDC_MAPON2:							
				case IDC_MAPON3:							
				case IDC_MAPON4:							
				case IDC_MAPON5:							
				case IDC_MAPON6:
				case IDC_MAPON7:
				case IDC_MAPON8:
				case IDC_MAPON9:
				case IDC_MAPON10:
//					theMtl->mapOn[id-IDC_MAPON1+theMtl->offset] = GetCheckBox(hwndDlg, id);
					{
					int on = GetCheckBox(hwndDlg, id);
					Interval iv;
					theMtl->pblock->SetValue(multi_ons,0,on,id-IDC_MAPON1+theMtl->offset);

					theMtl->NotifyChanged();
					break;
					}
				case IDC_MULTI_SETNUM:
					SetNumMats(hwndDlg);
					break;
				case IDC_MTL_NAME0: 
				case IDC_MTL_NAME1: 
				case IDC_MTL_NAME2: 
				case IDC_MTL_NAME3: 
				case IDC_MTL_NAME4: 
				case IDC_MTL_NAME5: 
				case IDC_MTL_NAME6: 
				case IDC_MTL_NAME7: 
				case IDC_MTL_NAME8: 
				case IDC_MTL_NAME9: 
					if (HIWORD(wParam)==EN_CHANGE) {
						TCHAR buf[200];
						int n = SubMtlNumFromNameID(id);
						iName[n]->GetText(buf,199);
//						theMtl->subNames.SetName(n+theMtl->offset, buf);
						theMtl->pblock->SetValue(multi_names,0,buf,n+theMtl->offset);

						}
					break;
				}
			break;
		case WM_PAINT:
			if (!valid) {
				valid = TRUE;
				ReloadDialog();
				}
			return FALSE;
		case WM_CLOSE:
			break;       
		case WM_DESTROY: 
			break;		
		case CC_COLOR_BUTTONDOWN:
			theHold.Begin();
			break;
		case CC_COLOR_BUTTONUP:
			if (HIWORD(wParam)) theHold.Accept(GetString(IDS_DS_PARAMCHG));
			else theHold.Cancel();
			break;
		case CC_COLOR_CHANGE: {
			if (HIWORD(wParam)) theHold.Begin();
			int i = LOWORD(wParam)-IDC_MULTI_COLOR1+theMtl->offset;
			IColorSwatch *cs = (IColorSwatch*)lParam;
			if (i>=0 && i<theMtl->subMtl.Count()&&theMtl->subMtl[i]) {
				theMtl->subMtl[i]->SetDiffuse(
					cs->GetColor(),curTime);
				}
			UpdateColorSwatches();
			if (HIWORD(wParam)) {
				theHold.Accept(GetString(IDS_DS_PARAMCHG));
			    UpdateMtlDisplay();
				}
			break;
			}

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP: 
		    UpdateMtlDisplay();
			break;

    	}
	return FALSE;
	}

static BOOL CALLBACK  BasicPanelDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	MultiDlg *theDlg;
	if (msg==WM_INITDIALOG) {
		theDlg = (MultiDlg*)lParam;
		theDlg->hPanelBasic = hwndDlg;
		SetWindowLong(hwndDlg, GWL_USERDATA,lParam);
		}
	else {
	    if ( (theDlg = (MultiDlg *)GetWindowLong(hwndDlg, GWL_USERDATA) ) == NULL )
			return FALSE; 
		}
	theDlg->isActive = 1;
	int	res = theDlg->BasicPanelProc(hwndDlg,msg,wParam,lParam);
	theDlg->isActive = 0;
	return res;
	}

void MultiDlg::UpdateColorSwatches() {
	for (int i=0; i<theMtl->subMtl.Count()-theMtl->offset && i<NSUBMTLS; i++) {
		Mtl *m = theMtl->subMtl[i+theMtl->offset];
			
		TSTR nm, label;
			if (m) 	nm = m->GetFullName();
			else 	nm = GetString(IDS_DS_NONE);
		if (m) {
			IColorSwatch *cs = GetIColorSwatch(GetDlgItem(hPanelBasic,IDC_MULTI_COLOR1+i),
				m->GetDiffuse(),nm);
			cs->SetColor(m->GetDiffuse());
			ReleaseIColorSwatch(cs);
			}
		}
	}


void MultiDlg::UpdateSubMtlNames() {
	IColorSwatch *cs;	
	int ct = theMtl->pblock->Count(multi_names);

	for (int i=0; i<theMtl->subMtl.Count()-theMtl->offset && i<NSUBMTLS; i++) {
		Mtl *m = theMtl->subMtl[i+theMtl->offset];
		TSTR nm, label;
		if (m) 
			nm = m->GetFullName();
		else 
			nm = GetString(IDS_DS_NONE);
		
		ShowWindow(GetDlgItem(hPanelBasic, IDC_MULTI_MTL0+i), SW_SHOW);
		ShowWindow(GetDlgItem(hPanelBasic, IDC_MULTI_COLOR1+i), SW_SHOW);
		ShowWindow(GetDlgItem(hPanelBasic, subNameId[i]), SW_SHOW);
		ShowWindow(GetDlgItem(hPanelBasic,mapOnIDs[i]),SW_SHOW);

		int on;
		Interval iv;

		if ((i+theMtl->offset)<ct)
			{
			theMtl->pblock->GetValue(multi_ons,0,on,iv,i+theMtl->offset);

			SetCheckBox(hPanelBasic, mapOnIDs[i], on);
			}
//		SetCheckBox(hPanelBasic, mapOnIDs[i], theMtl->mapOn[i+theMtl->offset]);

//		SetDlgItemText(hPanelBasic, IDC_MULTI_MTL0+i, nm.data());
		iBut[i]->SetText(nm.data());

		if (m) {
			cs = GetIColorSwatch(GetDlgItem(hPanelBasic,IDC_MULTI_COLOR1+i),
				m->GetDiffuse(),nm);
			cs->SetColor(m->GetDiffuse());
			ReleaseIColorSwatch(cs);
			}

		TCHAR *name;
		Interval niv;
		if ((i+theMtl->offset)<ct)
			{
			theMtl->pblock->GetValue(multi_names,0,name,niv,i+theMtl->offset);
			if (name) {
				TCHAR buf[256];
				iName[i]->GetText(buf,255);
				if (_tcscmp(name,buf))
					iName[i]->SetText(name);
				}
			else iName[i]->SetText(_T(""));
			}
//		iName[i]->SetText(theMtl->subNames[i+theMtl->offset]);
		TSTR buf;
		buf.printf("%d:",i+theMtl->offset+1);
		SetDlgItemText(hPanelBasic,numIDs[i],buf);
		}
	for ( ; i<NSUBMTLS; i++) {
		ShowWindow(GetDlgItem(hPanelBasic, IDC_MULTI_MTL0+i), SW_HIDE);
		ShowWindow(GetDlgItem(hPanelBasic, IDC_MULTI_COLOR1+i), SW_HIDE);
		ShowWindow(GetDlgItem(hPanelBasic, subNameId[i]), SW_HIDE);
		ShowWindow(GetDlgItem(hPanelBasic,mapOnIDs[i]),SW_HIDE);
		TSTR buf;
		SetDlgItemText(hPanelBasic,numIDs[i],buf);
		}
	TSTR buf;
	buf.printf(_T("%d"),theMtl->subMtl.Count());
	SetDlgItemText(hPanelBasic,IDC_MULTI_NUMMATS,buf);
	SetScrollRange(hScroll,SB_CTL,0,theMtl->NumSubMtls()-NSUBMTLS,FALSE);
	SetScrollPos(hScroll,SB_CTL,theMtl->offset,TRUE);
	EnableWindow(hScroll,theMtl->subMtl.Count()>NSUBMTLS);
	}

void MultiDlg::LoadDialog(BOOL draw) {
	if (theMtl) {
		Interval valid;
		theMtl->Update(curTime,valid);
		UpdateSubMtlNames();		
		}
	}

void MultiDlg::SetThing(ReferenceTarget *m) {
	assert (m->ClassID()==multiClassID);
	assert (m->SuperClassID()==MATERIAL_CLASS_ID);
	if (theMtl) theMtl->paramDlg = NULL;
	theMtl = (Multi *)m;
	if (theMtl) theMtl->paramDlg = this;
	LoadDialog(TRUE);
	}


static BOOL CALLBACK NumMatsDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG: {
			ISpinnerControl *spin = 
				SetupIntSpinner(
					hWnd,IDC_MULTI_NUMMATSSPIN,IDC_MULTI_NUMMATS,
					1,1000,(int)lParam);
			ReleaseISpinner(spin);
			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin = 
						GetISpinner(GetDlgItem(hWnd,IDC_MULTI_NUMMATSSPIN));
					EndDialog(hWnd,spin->GetIVal());
					ReleaseISpinner(spin);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,-1);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void MultiDlg::SetNumMats(HWND hWnd)
	{
	int res = DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_MULTI_SETNUM),
		hPanelBasic,
		NumMatsDlgProc,
		(LPARAM)theMtl->subMtl.Count());
	if (res>=0) {
		if (res<=0) res = 1;
		if (res>1000) res = 1000;
		HCURSOR c = SetCursor(LoadCursor(NULL,IDC_WAIT));
		theMtl->SetNumSubMtls(res);
		SetCursor(c);
		theMtl->ClampOffset();
		UpdateSubMtlNames();
	
		GetCOREInterface()->FlushUndoBuffer();  // DS 6/7/99
		}
	}

//-----------------------------------------------------------------------------
//  Multi
//-----------------------------------------------------------------------------

static ParamBlockDesc pbdesc[] = {
	{ TYPE_FLOAT, NULL, TRUE } };   // blend

void Multi::Init() {
	ivalid.SetEmpty();
	offset = 0;
	}

void Multi::Reset() {
	Init();
	multiCD.Reset(this, TRUE);	// reset all pb2's
//	for (int i=0; i<subMtl.Count(); i++)
//		DeleteReference(i+1);
	SetNumSubMtls(NSUBMTLS);
	for (int i=0; i<subMtl.Count(); i++) {
		ReplaceReference(i+1,(ReferenceTarget*)GetStdMtl2Desc()->Create());
		GetCOREInterface()->AssignNewName(subMtl[i]);
		}
	}

void Multi::NotifyChanged() {
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}

Multi::Multi(BOOL loading, BOOL createDefaultSubMtls) // mjm - 10.11.99 - added createDefaultSubMtls parameter
{
	paramDlg = NULL;
	Param1 = FALSE;
	/*
	subMtl.SetCount(NSUBMTLS);
	for (int i=0; i<NSUBMTLS; i++)  subMtl[i] = NULL;
	*/
	pblock = NULL;

	loadingOld = FALSE;
	multiCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
	Init();

	if (!loading && createDefaultSubMtls) // mjm - 10.11.99
		SetNumSubMtls(NSUBMTLS);

	pblock->DefineParamAlias(_T("material1"), multi_mtls, 0);  // JBW 5/24/99, add alias for base material to support macroRecording
}

Mtl *Multi::UseMtl() {
	Mtl* m = NULL;
	for (int i=0; i<subMtl.Count(); i++) if (subMtl[i]) { m = subMtl[i]; break; }
	return m;
	}

void Multi::ClampOffset()
	{
	if (offset>subMtl.Count()-NSUBMTLS) {
		offset=subMtl.Count()-NSUBMTLS;
		}
	if (offset<0) offset = 0;
	}

// These allow the real-time renderer to display a material appearance.
Color Multi::GetAmbient(int mtlNum, BOOL backFace) { 
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetAmbient(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetAmbient(mtlNum,backFace):Color(0,0,0);
	}		
Color Multi::GetDiffuse(int mtlNum, BOOL backFace){ 
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetDiffuse(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetDiffuse():Color(0,0,0);
	}				
Color Multi::GetSpecular(int mtlNum, BOOL backFace){
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetSpecular(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetSpecular():Color(0,0,0);
	}		

float Multi::GetXParency(int mtlNum, BOOL backFace) {
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetXParency(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetXParency():0.0f;
	}

float Multi::GetShininess(int mtlNum, BOOL backFace) {
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetShininess(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetShininess():0.0f;
	}		
float Multi::GetShinStr(int mtlNum, BOOL backFace) {
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->GetShinStr(mtlNum,backFace);
	return UseMtl()?UseMtl()->GetShinStr():0.0f;
	}

float Multi::WireSize(int mtlNum, BOOL backFace) {
	mtlNum = mtlNum%subMtl.Count();
	if (subMtl[mtlNum]) return subMtl[mtlNum]->WireSize(mtlNum,backFace);
	return UseMtl()?UseMtl()->WireSize():0.0f;
	}

RefTargetHandle Multi::Clone(RemapDir &remap) {
	Multi *mnew = new Multi(FALSE, FALSE); // mjm - 10.11.99 - don't create default subMtls
	*((MtlBase*)mnew) = *((MtlBase*)this);  // copy superclass stuff
	mnew->ivalid.SetEmpty();
	int nsub = subMtl.Count();
	mnew->subMtl.SetCount(nsub);
//	mnew->mapOn.SetCount(nsub);
//	mnew->subNames.SetSize(nsub);
	mnew->offset = offset;
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	mnew->pblock->SetCount(multi_mtls, nsub);
	mnew->pblock->SetCount(multi_ons, nsub);
	mnew->pblock->SetCount(multi_names, nsub);

	for (int i = 0; i<nsub; i++)
	{
		mnew->subMtl[i] = NULL;
		if (subMtl[i])
			mnew->ReplaceReference(i+1,remap.CloneRef(subMtl[i]));
//		mnew->mapOn[i] = mapOn[i];
	}
	return (RefTargetHandle)mnew;
}


ParamDlg* Multi::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	MultiDlg *dm = new MultiDlg(hwMtlEdit, imp, this);
	dm->LoadDialog(TRUE);	
	SetParamDlg(dm);
	return dm;	
	}

void Multi::Update(TimeValue t, Interval& valid) {		
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();
		TCHAR *buf;
		buf = new TCHAR[200];


		for (int i=0; i<subMtl.Count(); i++) {
			if (subMtl[i]) 
				subMtl[i]->Update(t,ivalid);

			}
		delete buf;
		}
	valid &=ivalid;
	}

Interval Multi::Validity(TimeValue t) {
	Interval valid;
	Update(t,valid);
	return ivalid;
	}

class SetNumMtlsRestore : public RestoreObj {
	public:
		Multi *multi;
		Tab<Mtl*> undo, redo;
//		Tab<BOOL> undoMO, redoMO;
		SetNumMtlsRestore(Multi *m) {
			multi  = m;
			undo   = multi->subMtl;
//			undoMO = multi->mapOn;
			}
   		
		void Restore(int isUndo) {
			if (isUndo) {
				redo   = multi->subMtl;
//				redoMO = multi->mapOn;
				}
			multi->subMtl = undo;
//			multi->mapOn  = undoMO;
			}
		void Redo() {
			multi->subMtl = redo;
//			multi->mapOn  = redoMO;
			}
	};

void Multi::SetNumSubMtls(int n)
	{
	int ct = subMtl.Count();
	if (n!=ct) {
		/*
		if (theHold.Holding()) {
			theHold.Put(new SetNumMtlsRestore(this));
			}
		*/
		if (n<ct) {
			for (int i=n; i<ct; i++) {
				if (subMtl[i])
					subMtl[i]->DeactivateMapsInTree();
				ReplaceReference(i+1,NULL);
				}
			}
		subMtl.SetCount(n);
//		subNames.SetSize(n);
//		mapOn.SetCount(n);
//		pblock->SetCount(multi_mtls,n);
		if (n>ct) {
			for (int i=ct; i<subMtl.Count(); i++) {
				subMtl[i] = NULL;
				ReplaceReference(i+1,(ReferenceTarget*)GetStdMtl2Desc()->Create());
				GetCOREInterface()->AssignNewName(subMtl[i]);

//				pblock->SetValue(multi_ons,0,TRUE,i);
//				mapOn[i] = TRUE;
				}
//have to do this sepperate because setvalue causes an update and all the references are not in place yet
			macroRec->Disable();	// JBW 4/21/99, just record on count change
			pblock->SetCount(multi_ons,n);
			pblock->SetCount(multi_names,n);
			macroRec->Enable();
			pblock->SetCount(multi_mtls,n);
			for (i=ct; i<subMtl.Count(); i++) {
				pblock->SetValue(multi_ons,0,TRUE,i);
//				mapOn[i] = TRUE;
				}

			}		
		else 
			{
			macroRec->Disable();	// JBW 4/21/99, just record on count change
			pblock->SetCount(multi_ons,n);
			pblock->SetCount(multi_names,n);
			macroRec->Enable();
			pblock->SetCount(multi_mtls,n);

			}

		ClampOffset();
		NotifyChanged();
		if (paramDlg&&!paramDlg->isActive) {
			paramDlg->ReloadDialog();
			paramDlg->UpdateMtlDisplay();	  // DS 9/2/99
			}
		}
	}
//need to remap references since we added a paramblock
int Multi::RemapRefOnLoad(int iref) 
{
if (Param1) iref += 1;
return iref;
}

RefTargetHandle Multi::GetReference(int i) {
	if (loadingOld) {
		if (i==0) return NULL;
		else return subMtl[i-1];
		}
	else 
		{
		if (i==PBLOCK_REF) return pblock;
		else return subMtl[i-1];
		}
	}

void Multi::SetReference(int i, RefTargetHandle rtarg) {
	if ((i-1)>=subMtl.Count()) {
		int n = subMtl.Count();
		SetNumSubMtls(i+1);
//		subMtl.SetCount(i+1);
//		for (int j=n; j<=i; j++) // mjm - 10,11.99
//			subMtl[j] = NULL;    // mjm - 10,11.99 - default subMtl created in SetNumSubMtls()
	}
	if (loadingOld) {
		if (i==0|| (rtarg&&!IsMtl(rtarg)))  
			{ } //pblock = (IParamBlock *)rtarg;
		else 
			subMtl[i-1] = (Mtl *)rtarg;
		}
	else 
		{
		if (i==PBLOCK_REF) pblock = (IParamBlock2 *)rtarg;
 		else subMtl[i-1] = (Mtl *)rtarg;
		}
	}

void Multi::SetSubMtl(int i, Mtl *m) {
	if (i>=subMtl.Count()) {
		int n = subMtl.Count();
		SetNumSubMtls(i+1);
	    //	subMtl.SetCount(i+1);
//		for (int j=n; j<=i; j++) // mjm - 10,11.99
//			subMtl[j] = NULL;    // mjm - 10,11.99 - default subMtl created in SetNumSubMtls()
		}
	ReplaceReference(i+1,m);
	if (paramDlg)	  
		paramDlg->UpdateSubMtlNames();
	}

TSTR Multi::GetSubMtlSlotName(int i) {
	TSTR s;
	TCHAR *name;
	Interval iv;
	pblock->GetValue(multi_names,0,name,iv,i);

	if (name) 
		s.printf("(%d) %s",i+1,name);
	else 
		s.printf("(%d)",i+1);
	return s;
	}

Animatable* Multi::SubAnim(int i) {
//	if (i==PBLOCK_REF) return pblock;
//	else return subMtl[i-1]; 
	return subMtl[i]; 
	}

TSTR Multi::SubAnimName(int i) {
	return GetSubMtlTVName(i);
	}

RefResult Multi::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (paramDlg) 
				paramDlg->Invalidate();		
//			if (paramDlg&&!paramDlg->isActive) 
//					paramDlg->ReloadDialog();
			// following by JBW 45/21/99 to allow scripter-setting of submtl counts (any count change updates others)
			if (pblock &&
                    pblock->LastNotifyParamID() == multi_ons &&
                    pblock->Count(multi_ons) != subMtl.Count())
				SetNumSubMtls(pblock->Count(multi_ons));
			else if (pblock &&
                    pblock->LastNotifyParamID() == multi_names &&
                    pblock->Count(multi_names) != subMtl.Count())
				SetNumSubMtls(pblock->Count(multi_names));
			else if (pblock &&
                    pblock->LastNotifyParamID() == multi_mtls &&
                    pblock->Count(multi_mtls) != subMtl.Count())
				SetNumSubMtls(pblock->Count(multi_mtls));
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			return REF_STOP; 
			}
		}
	return(REF_SUCCEED);
	}


inline void Clamp(Color &c) {
	if (c.r > 1.0f) c.r = 1.0f;
	if (c.g > 1.0f) c.g = 1.0f;
	if (c.b > 1.0f) c.b = 1.0f;
	}

static Color black(0,0,0);

void Multi::Shade(ShadeContext& sc) {
	if (gbufID) sc.SetGBufferID(gbufID);
	int mtlnum = sc.mtlNum, ct = subMtl.Count();
	if (ct) {
		if (mtlnum < 0) mtlnum = 0;
		if (mtlnum >= ct) mtlnum = mtlnum % ct;		
		Mtl* subm = subMtl[mtlnum];
		int on;
		Interval iv;
		pblock->GetValue(multi_ons,0,on,iv,mtlnum);
//		if (subm&&mapOn[mtlnum]) 
		if (subm&&on) 
			subm->Shade(sc);		
		}
	}

float Multi::EvalDisplacement(ShadeContext& sc) {
	int mtlnum = sc.mtlNum, ct = subMtl.Count();
	if (ct) {
		if (mtlnum < 0) mtlnum = 0;
		if (mtlnum >= ct) mtlnum = mtlnum % ct;		
		Mtl* subm = subMtl[mtlnum];
		int on;
		Interval iv;
		pblock->GetValue(multi_ons,0,on,iv,mtlnum);
//		if (subm&&mapOn[mtlnum]) 
		if (subm&&on) 
			return subm->EvalDisplacement(sc);		
		}
	return 0.0f;
	}

Interval Multi::DisplacementValidity(TimeValue t){
	int ct = subMtl.Count();
	Interval iv;
	iv.SetInfinite();
	for (int i=0; i<ct; i++) {
		Mtl* subm = subMtl[i];
		int on;
		Interval iv;
		pblock->GetValue(multi_ons,0,on,iv,i);
//		if (subm&&mapOn[i]) 
		if (subm&&on) 
			iv &= subm->DisplacementValidity(t);		
		}
	return iv;
	} 

#define MTL_HDR_CHUNK 0x4000
#define MULTI_NUM_OLD 0x4001
#define MULTI_NUM 0x4002
#define MULTI_NAMES 0x4010
#define MAPOFF_CHUNK 0x1000
#define PARAM2_CHUNK 0x4003

IOResult Multi::Save(ISave *isave) { 
	IOResult res;
	ULONG nb;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();


	isave->BeginChunk(PARAM2_CHUNK);
	isave->EndChunk();

	//int numSubs = NSUBMTLS;
	int numSubs = subMtl.Count();
	isave->BeginChunk(MULTI_NUM);
	isave->Write(&numSubs,sizeof(numSubs),&nb);			
	isave->EndChunk();


//	isave->BeginChunk(MULTI_NAMES);
//	subNames.Save(isave);
//	isave->EndChunk();

/*
	for (int i=0; i<subMtl.Count(); i++) {
		if (mapOn[i]==0) {
			isave->BeginChunk(MAPOFF_CHUNK+i);
			isave->EndChunk();
			}
		}
*/

	return IO_OK;
	}	

//2-18-96
class MultiPostLoad : public PostLoadCallback {
	public:
		Multi *m;
		MultiPostLoad(Multi *b) {m=b;}
		void proc(ILoad *iload) {  m->loadingOld = FALSE; delete this; } 
	};

//watje
class Multi2PostLoadCallback:public  PostLoadCallback
{
public:
	Multi      *s;
	Tab<BOOL> ons;
//	NameTab subNames;

	int Param1;
	Multi2PostLoadCallback(Multi *r, BOOL b, Tab<BOOL> bl/*,	NameTab sNames*/) {s=r;Param1 = b;ons = bl;/*subNames = sNames;*/}
	void proc(ILoad *iload);
};

void Multi2PostLoadCallback::proc(ILoad *iload)
{
	if (Param1)
		{
		s->pblock->SetCount(multi_ons,ons.Count());
		s->pblock->SetCount(multi_names,ons.Count());
		s->pblock->SetCount(multi_mtls,ons.Count());
		for (int i=0; i<s->subMtl.Count(); i++) {
			s->pblock->SetValue(multi_ons,0,ons[i],i);
			if (s->subNames[i])
				s->pblock->SetValue(multi_names,0,s->subNames[i],i);

			}
		}
	delete this;
}
	  
IOResult Multi::Load(ILoad *iload) { 
	ULONG nb;
	IOResult res;
	Param1 = TRUE;

	Tab<BOOL>mapOn;
//	NameTab subNames;


	while (IO_OK==(res=iload->OpenChunk())) {
		int id = iload->CurChunkID();

		if (id>=MAPOFF_CHUNK&&id<=MAPOFF_CHUNK+0x1000) {
			mapOn[id-MAPOFF_CHUNK] = FALSE; 
			}
		else 

		switch(id)  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			case MULTI_NUM_OLD: 
				iload->SetObsolete();
				iload->RegisterPostLoadCallback(new MultiPostLoad(this));
				loadingOld = TRUE;
			case MULTI_NUM: {
				int numSubs;
				iload->Read(&numSubs,sizeof(numSubs),&nb);			
				subMtl.SetCount(numSubs);
				mapOn.SetCount(numSubs);
				subNames.SetSize(numSubs);

				for (int i=0; i<numSubs; i++) {	
					subMtl[i] = NULL;
					mapOn[i] = TRUE;
					}
				}
				break;
			case MULTI_NAMES:
				res = subNames.Load(iload);	
				subNames.SetSize(subMtl.Count());
				break;
			case PARAM2_CHUNK:
				Param1 = FALSE;
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	Multi2PostLoadCallback* multiplcb = new Multi2PostLoadCallback(this,Param1,mapOn);
	iload->RegisterPostLoadCallback(multiplcb);
	return IO_OK;
                                                      
	}

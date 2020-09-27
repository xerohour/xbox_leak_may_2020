/**********************************************************************
 *<
	FILE: indepos.cpp

	DESCRIPTION: An independent X, Y, Z position controller

	CREATED BY: Rolf Berteig

	HISTORY: created 13 June 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "ctrl.h"

#define IPOS_CONTROL_CLASS_ID	Class_ID(0x118f7e02,0xffee238a)
#define IPOS_CONTROL_CNAME		GetString(IDS_RB_IPOS)

#define IPOINT3_CONTROL_CLASS_ID	Class_ID(0x118f7e02,0xfeee238b)
#define IPOINT3_CONTROL_CNAME		GetString(IDS_RB_IPOINT3)

#define ICOLOR_CONTROL_CLASS_ID		Class_ID(0x118f7c01,0xfeee238a)
#define ICOLOR_CONTROL_CNAME		GetString(IDS_RB_ICOLOR)

#define IPOS_X_REF		0
#define IPOS_Y_REF		1
#define IPOS_Z_REF		2

class IPosDlg;

static DWORD subColor[] = {PAINTCURVE_XCOLOR, PAINTCURVE_YCOLOR, PAINTCURVE_ZCOLOR};

class IndePosition : public Control {
	public:
		Control *posX;
		Control *posY;
		Control *posZ;
		Point3 curval;
		Interval ivalid;
		BOOL blockUpdate;

		static IPosDlg *dlg;
		static IObjParam *ip;
		static ULONG beginFlags;
		static IndePosition *editControl; // The one being edited.
		
		IndePosition(BOOL loading=FALSE);
		IndePosition(const IndePosition &ctrl);
		~IndePosition();
		void Update(TimeValue t);

		// Animatable methods
		Class_ID ClassID() { return IPOS_CONTROL_CLASS_ID;} 
		SClass_ID SuperClassID() {return CTRL_POSITION_CLASS_ID;} 
		
		void GetClassName(TSTR& s) {s = IPOS_CONTROL_CNAME;}
		void DeleteThis() {delete this;}		
		int IsKeyable() {return 1;}		

		int NumSubs()  {return 3;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		DWORD GetSubAnimCurveColor(int subNum) {return subColor[subNum];}

		ParamDimension* GetParamDimension(int i) {return stdWorldDim;}
		BOOL AssignController(Animatable *control,int subAnim);
		void AddNewKey(TimeValue t,DWORD flags);
		int NumKeys();
		TimeValue GetKeyTime(int index);
		void CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags);
		BOOL IsKeyAtTime(TimeValue t,DWORD flags);
		BOOL GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt);
		void DeleteKeyAtTime(TimeValue t);

		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next );
		
		// Reference methods
		int NumRefs() { return 3; };	
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		
		// Control methods
		Control *GetXController() {return posX;}
		Control *GetYController() {return posY;}
		Control *GetZController() {return posZ;}
		void Copy(Control *from);
		RefTargetHandle Clone(RemapDir& remap);
		BOOL IsLeaf() {return FALSE;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);	
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);
		void CommitValue(TimeValue t);
		void RestoreValue(TimeValue t);		
	};

IPosDlg       *IndePosition::dlg         = NULL;
IObjParam     *IndePosition::ip          = NULL;
ULONG          IndePosition::beginFlags  = 0;
IndePosition  *IndePosition::editControl = NULL;

class IndePoint3 : public IndePosition {
	public:
		IndePoint3(BOOL loading=FALSE) : IndePosition(loading) {}
		SClass_ID SuperClassID() {return CTRL_POINT3_CLASS_ID;} 
		Class_ID ClassID() { return IPOINT3_CONTROL_CLASS_ID;} 
		void GetClassName(TSTR& s) {s = IPOINT3_CONTROL_CNAME;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
		RefTargetHandle Clone(RemapDir& remap);
	};

class IndeColor : public IndePoint3 {
	public:
		IndeColor(BOOL loading=FALSE) : IndePoint3(loading) {}
		SClass_ID SuperClassID() {return CTRL_POINT3_CLASS_ID;} 
		Class_ID ClassID() { return ICOLOR_CONTROL_CLASS_ID;} 
		void GetClassName(TSTR& s) {s = ICOLOR_CONTROL_CNAME;}		
		ParamDimension* GetParamDimension(int i) {return stdColor255Dim;}
		RefTargetHandle Clone(RemapDir& remap);
	};

class IPosClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new IndePosition(loading);}
	const TCHAR *	ClassName() {return IPOS_CONTROL_CNAME;}
	SClass_ID		SuperClassID() {return CTRL_POSITION_CLASS_ID;}
	Class_ID		ClassID() {return IPOS_CONTROL_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};
static IPosClassDesc iposCD;
ClassDesc* GetIPosCtrlDesc() {return &iposCD;}

class IPoint3ClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new IndePoint3(loading);}
	const TCHAR *	ClassName() {return IPOINT3_CONTROL_CNAME;}
	SClass_ID		SuperClassID() {return CTRL_POINT3_CLASS_ID;}
	Class_ID		ClassID() {return IPOINT3_CONTROL_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};
static IPoint3ClassDesc ipoint3CD;
ClassDesc* GetIPoint3CtrlDesc() {return &ipoint3CD;}

class IColorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new IndeColor(loading);}
	const TCHAR *	ClassName() {return ICOLOR_CONTROL_CNAME;}
	SClass_ID		SuperClassID() {return CTRL_POINT3_CLASS_ID;}
	Class_ID		ClassID() {return ICOLOR_CONTROL_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};
static IColorClassDesc icolorCD;
ClassDesc* GetIColorCtrlDesc() {return &icolorCD;}


static BOOL CALLBACK IPosParamDialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

static const int editButs[] = {IDC_IPOS_X,IDC_IPOS_Y,IDC_IPOS_Z};

#define EDIT_X	0
#define EDIT_Y	1
#define EDIT_Z	2

#define IPOS_BEGIN		1
#define IPOS_MIDDLE		2
#define IPOS_END		3

class IPosDlg {
	public:
		IndePosition *cont;
		HWND hWnd;
		IObjParam *ip;
		ICustButton *iEdit[3];
		static int cur;
		
		IPosDlg(IndePosition *cont,IObjParam *ip);
		~IPosDlg();

		void Init();
		void EndingEdit(IndePosition *next);
		void BeginingEdit(IndePosition *cont,IObjParam *ip,IndePosition *prev);
		void SetCur(int c,int code=IPOS_MIDDLE);
		void WMCommand(int id, int notify, HWND hCtrl);
	};

int IPosDlg::cur = EDIT_X;

IPosDlg::IPosDlg(IndePosition *cont,IObjParam *ip)
	{
	this->ip   = ip;
	this->cont = cont;
	for (int i=0; i<3; i++) {
		iEdit[i] = NULL;
		}
	
	TCHAR *name;
	if (cont->ClassID()==IPOS_CONTROL_CLASS_ID) 
		 name = GetString(IDS_RB_IPOSPARAMS);
	else 
	if (cont->ClassID()==IPOINT3_CONTROL_CLASS_ID) 
		 name = GetString(IDS_RB_IPOINT3PARAMS);
	else name = GetString(IDS_RB_ICOLORPARAMS);

	hWnd = ip->AddRollupPage( 
		hInstance,
		MAKEINTRESOURCE(IDD_IPOS_PARAMS),
		IPosParamDialogProc,
		name, 
		(LPARAM)this);
	ip->RegisterDlgWnd(hWnd);	
	
	SetCur(cur,IPOS_BEGIN);	
	UpdateWindow(hWnd);
	}

IPosDlg::~IPosDlg()
	{
	SetCur(cur,IPOS_END);
	for (int i=0; i<3; i++) {
		ReleaseICustButton(iEdit[i]);		
		}
	ip->UnRegisterDlgWnd(hWnd);
	ip->DeleteRollupPage(hWnd);
	hWnd = NULL;
	}

void IPosDlg::EndingEdit(IndePosition *next)
	{
	switch (cur) {
		case EDIT_X:
			cont->posX->EndEditParams(ip,0,next->posX);
			break;
		case EDIT_Y:
			cont->posY->EndEditParams(ip,0,next->posY);
			break;
		case EDIT_Z:
			cont->posZ->EndEditParams(ip,0,next->posZ);
			break;
		}
	cont = NULL;
	ip   = NULL;
	}

void IPosDlg::BeginingEdit(IndePosition *cont,IObjParam *ip,IndePosition *prev)
	{
	this->ip   = ip;
	this->cont = cont;
	switch (cur) {
		case EDIT_X:
			cont->posX->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->posX);
			break;
		case EDIT_Y:
			cont->posY->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->posY);
			break;
		case EDIT_Z:
			cont->posZ->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->posZ);
			break;
		}	
	UpdateWindow(hWnd);
	}

void IPosDlg::Init()
	{	
	for (int i=0; i<3; i++) {
		iEdit[i] = GetICustButton(GetDlgItem(hWnd,editButs[i]));		
		iEdit[i]->SetType(CBT_CHECK);
		}
	iEdit[cur]->SetCheck(TRUE);	
	}

void IPosDlg::SetCur(int c,int code)
	{
	if (c==cur && code==IPOS_MIDDLE) return;
	Control *prev = NULL, *next = NULL;

	if (code!=IPOS_END) {
		switch (c) {
			case EDIT_X:
				next = cont->posX;
				break;
			case EDIT_Y:
				next = cont->posY;
				break;
			case EDIT_Z:
				next = cont->posZ;
				break;
			}
		}

	if (code!=IPOS_BEGIN) {
		switch (cur) {
			case EDIT_X:
				cont->posX->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->posX;
				break;
			case EDIT_Y:
				cont->posY->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->posY;
				break;
			case EDIT_Z:
				cont->posZ->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->posZ;
				break;
			}
		}

	cur = c;

	if (code!=IPOS_END) {
		switch (cur) {
			case EDIT_X:
				cont->posX->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			case EDIT_Y:
				cont->posY->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			case EDIT_Z:
				cont->posZ->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			}
		}
	}

void IPosDlg::WMCommand(int id, int notify, HWND hCtrl)
	{
	switch (id) {
		case IDC_IPOS_X:
			SetCur(0);
			iEdit[0]->SetCheck(TRUE);
			iEdit[1]->SetCheck(FALSE);
			iEdit[2]->SetCheck(FALSE);
			break;
		case IDC_IPOS_Y:
			SetCur(1);
			iEdit[0]->SetCheck(FALSE);
			iEdit[1]->SetCheck(TRUE);
			iEdit[2]->SetCheck(FALSE);
			break;
		case IDC_IPOS_Z:
			SetCur(2);
			iEdit[0]->SetCheck(FALSE);
			iEdit[1]->SetCheck(FALSE);
			iEdit[2]->SetCheck(TRUE);
			break;
		}
	}

static BOOL CALLBACK IPosParamDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	IPosDlg *dlg = (IPosDlg*)GetWindowLong(hDlg,GWL_USERDATA);

	switch (message) {
		case WM_INITDIALOG:
			dlg = (IPosDlg*)lParam;			
			SetWindowLong(hDlg,GWL_USERDATA,lParam);
			dlg->hWnd = hDlg;
			dlg->Init();
			break;
		
		case WM_COMMAND:
			dlg->WMCommand(LOWORD(wParam),HIWORD(wParam),(HWND)lParam);
			break;

		case WM_LBUTTONDOWN:case WM_LBUTTONUP:	case WM_MOUSEMOVE:
			dlg->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			break;
				
		default:
			return FALSE;
		}
	return TRUE;
	}



IndePosition::IndePosition(const IndePosition &ctrl)
	{
	blockUpdate = FALSE;

	posX = NULL;
	posY = NULL;
	posZ = NULL;

	if (ctrl.posX) {
		ReplaceReference(IPOS_X_REF,ctrl.posX);
	} else {
		ReplaceReference(IPOS_X_REF,NewDefaultFloatController());
		}
	if (ctrl.posY) {
		ReplaceReference(IPOS_Y_REF,ctrl.posY);
	} else {
		ReplaceReference(IPOS_Y_REF,NewDefaultFloatController());
		}
	if (ctrl.posZ) {
		ReplaceReference(IPOS_Z_REF,ctrl.posZ);
	} else {
		ReplaceReference(IPOS_Z_REF,NewDefaultFloatController());
		}
	
	curval = ctrl.curval;
	ivalid = ctrl.ivalid;
	}

IndePosition::IndePosition(BOOL loading) 
	{
	blockUpdate = FALSE;

	posX = NULL;
	posY = NULL;
	posZ = NULL;
	if (!loading) {
		ReplaceReference(IPOS_X_REF,NewDefaultFloatController());
		ReplaceReference(IPOS_Y_REF,NewDefaultFloatController());
		ReplaceReference(IPOS_Z_REF,NewDefaultFloatController());
		ivalid = FOREVER;
		curval = Point3(0,0,0);
	} else {
		ivalid.SetEmpty();
		}	
	}

RefTargetHandle IndePoint3::Clone(RemapDir& remap) 
	{
	IndePoint3 *pos = new IndePoint3(TRUE);	
	pos->ReplaceReference(IPOS_X_REF, remap.CloneRef(posX));
	pos->ReplaceReference(IPOS_Y_REF, remap.CloneRef(posY));
	pos->ReplaceReference(IPOS_Z_REF, remap.CloneRef(posZ));
	return pos;
	}

RefTargetHandle IndeColor::Clone(RemapDir& remap) 
	{
	IndeColor *pos = new IndeColor(TRUE);	
	pos->ReplaceReference(IPOS_X_REF, remap.CloneRef(posX));
	pos->ReplaceReference(IPOS_Y_REF, remap.CloneRef(posY));
	pos->ReplaceReference(IPOS_Z_REF, remap.CloneRef(posZ));
	return pos;
	}

RefTargetHandle IndePosition::Clone(RemapDir& remap) 
	{
	IndePosition *pos = new IndePosition(TRUE);	
	pos->ReplaceReference(IPOS_X_REF, remap.CloneRef(posX));
	pos->ReplaceReference(IPOS_Y_REF, remap.CloneRef(posY));
	pos->ReplaceReference(IPOS_Z_REF, remap.CloneRef(posZ));
	return pos;
	}


IndePosition::~IndePosition()
	{
	DeleteAllRefsFromMe();
	}

void IndePosition::Copy(Control *from)
	{
	if (from->ClassID()==ClassID()) {
		IndePosition *ctrl = (IndePosition*)from;
		ReplaceReference(IPOS_X_REF,ctrl->posX);
		ReplaceReference(IPOS_Y_REF,ctrl->posY);
		ReplaceReference(IPOS_Z_REF,ctrl->posZ);
		curval = ctrl->curval;
		ivalid = ctrl->ivalid;
	} else {		
		Point3 v;
		Interval iv;
		int num;		
		if ((num=from->NumKeys())!=NOT_KEYFRAMEABLE && num>0) {
			SuspendAnimate();
			AnimateOn();
			for (int i=0; i<num; i++) {
				TimeValue t = from->GetKeyTime(i);
				from->GetValue(t,&v,iv);
				SetValue(t,&v,TRUE,CTRL_ABSOLUTE);	
				}
			ResumeAnimate();
			// RB 2/10/99: A key at frame 0 may have been created
			if (num>0 && from->GetKeyTime(0)!=0) {
				posX->DeleteKeyAtTime(0);
				posY->DeleteKeyAtTime(0);
				posZ->DeleteKeyAtTime(0);
				}
		} else {
			from->GetValue(0,&v,ivalid);
			SetValue(0,&v,TRUE,CTRL_ABSOLUTE);
			}
		}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void IndePosition::Update(TimeValue t)
	{
	if (!ivalid.InInterval(t)) {
		ivalid = FOREVER;		
		if (posX) posX->GetValue(t,&curval.x,ivalid);
		if (posY) posY->GetValue(t,&curval.y,ivalid);
		if (posZ) posZ->GetValue(t,&curval.z,ivalid);		
		}
	}

void IndePosition::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
	{
	Point3 *v = (Point3*)val;
	
	// RB 5/5/99: SetValue() calls NotifyDependents() which ultimately calls GetValue().
	// This isn't supposed to happen since reference makers are supposed to only invalidate
	// on a call to NotifyRefChanged(), not re-evaluate. However this is what happens
	// on around line 644 of stdShaders.cpp. 
	curval = *v;
	blockUpdate = TRUE;
	if (posX) posX->SetValue(t,&v->x,commit,method);
	if (posY) posY->SetValue(t,&v->y,commit,method);
	if (posZ) posZ->SetValue(t,&v->z,commit,method);
	blockUpdate = FALSE;

	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void IndePosition::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
	{		
	Update(t);
	valid &= ivalid;			 
	if (method==CTRL_RELATIVE) {
  		Matrix3 *mat = (Matrix3*)val;		
		mat->PreTranslate(curval);
	} else {
		*((Point3*)val) = curval;
		}
	}

void IndePoint3::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
	{		
	if (!blockUpdate) Update(t);
	valid &= ivalid;			 
	if (method==CTRL_RELATIVE) {
  		*((Point3*)val) += curval;
	} else {			    
		*((Point3*)val)  = curval;
		}
	}

void IndePosition::CommitValue(TimeValue t)
	{
	if (posX) posX->CommitValue(t);
	if (posY) posY->CommitValue(t);
	if (posZ) posZ->CommitValue(t);
	}

void IndePosition::RestoreValue(TimeValue t)
	{
	if (posX) posX->RestoreValue(t);
	if (posY) posY->RestoreValue(t);
	if (posZ) posZ->RestoreValue(t);
	}

RefTargetHandle IndePosition::GetReference(int i)
	{
	switch (i) {
		case IPOS_X_REF: return posX;
		case IPOS_Y_REF: return posY;
		case IPOS_Z_REF: return posZ;
		default: return NULL;
		}
	}

void IndePosition::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case IPOS_X_REF: posX = (Control*)rtarg; break;
		case IPOS_Y_REF: posY = (Control*)rtarg; break;
		case IPOS_Z_REF: posZ = (Control*)rtarg; break;
		}
	}

Animatable* IndePosition::SubAnim(int i)
	{
	return GetReference(i);
	}

TSTR IndePosition::SubAnimName(int i)
	{
	if (ClassID()==IPOS_CONTROL_CLASS_ID) {
		switch (i) {
			case IPOS_X_REF: return GetString(IDS_RB_XPOSITION);
			case IPOS_Y_REF: return GetString(IDS_RB_YPOSITION);
			case IPOS_Z_REF: return GetString(IDS_RB_ZPOSITION);
			default: return _T("");
			}
	} else
	if (ClassID()==IPOINT3_CONTROL_CLASS_ID) {
		switch (i) {
			case IPOS_X_REF: return _T("X");
			case IPOS_Y_REF: return _T("Y");
			case IPOS_Z_REF: return _T("Z");
			default: return _T("");
			}
	} else {
		switch (i) {
			case IPOS_X_REF: return _T("R");
			case IPOS_Y_REF: return _T("G");
			case IPOS_Z_REF: return _T("B");
			default: return _T("");
			}
		}
	}

RefResult IndePosition::NotifyRefChanged(
		Interval iv, 
		RefTargetHandle hTarg, 
		PartID& partID, 
		RefMessage msg) 
	{
	switch (msg) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			break;
		case REFMSG_TARGET_DELETED:
			if (posX == hTarg) posX = NULL;
			if (posY == hTarg) posY = NULL;
			if (posZ == hTarg) posZ = NULL; 
			break;
		case REFMSG_GET_CONTROL_DIM: {
			ParamDimension **dim = (ParamDimension **)partID;
			assert(dim);
			*dim = stdWorldDim;
			}
		}
	return REF_SUCCEED;
	}

BOOL IndePosition::AssignController(Animatable *control,int subAnim)
	{	
	switch (subAnim) {
		case IPOS_X_REF:
			ReplaceReference(IPOS_X_REF,(RefTargetHandle)control);
			break;
		case IPOS_Y_REF:
			ReplaceReference(IPOS_Y_REF,(RefTargetHandle)control);
			break;
		case IPOS_Z_REF:
			ReplaceReference(IPOS_Z_REF,(RefTargetHandle)control);
			break;
		}

	// mjm 9.28.98
	// bugfix -- validity interval needs to be invalidated so curval
	// can be properly updated in next call to IndePosition::Update()
	ivalid.SetEmpty();

	NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE,TREE_VIEW_CLASS_ID,FALSE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	return TRUE;
	}

void IndePosition::AddNewKey(TimeValue t,DWORD flags)
	{
	if (posX) posX->AddNewKey(t,flags);
	if (posY) posY->AddNewKey(t,flags);
	if (posZ) posZ->AddNewKey(t,flags);
	}

int IndePosition::NumKeys()
	{
	int num = 0;
	if (posX) num += posX->NumKeys(); 
	if (posY) num += posY->NumKeys();
	if (posZ) num += posZ->NumKeys();
	return num;
	}

TimeValue IndePosition::GetKeyTime(int index)
	{
	int onum,num = 0;
	if (posX) num += posX->NumKeys(); 
	if (index < num) return posX->GetKeyTime(index);
	onum = num;
	if (posY) num += posY->NumKeys(); 
	if (index < num) return posY->GetKeyTime(index-onum);
	onum = num;
	if (posZ) num += posZ->NumKeys(); 
	if (index < num) return posZ->GetKeyTime(index-onum);
	return 0;
	}

void IndePosition::CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags)
	{
	if (posX) posX->CopyKeysFromTime(src,dst,flags);
	if (posY) posY->CopyKeysFromTime(src,dst,flags);
	if (posZ) posZ->CopyKeysFromTime(src,dst,flags);
	}

BOOL IndePosition::IsKeyAtTime(TimeValue t,DWORD flags)
	{
	if (posX && posX->IsKeyAtTime(t,flags)) return TRUE;
	if (posY && posY->IsKeyAtTime(t,flags)) return TRUE;
	if (posZ && posZ->IsKeyAtTime(t,flags)) return TRUE;
	return FALSE;
	}

void IndePosition::DeleteKeyAtTime(TimeValue t)
	{
	if (posX) posX->DeleteKeyAtTime(t);
	if (posY) posY->DeleteKeyAtTime(t);
	if (posZ) posZ->DeleteKeyAtTime(t);
	}

BOOL IndePosition::GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt)
	{
	TimeValue at,tnear = 0;
	BOOL tnearInit = FALSE;
	
	if (posX && posX->GetNextKeyTime(t,flags,at)) {
		if (!tnearInit) {
			tnear = at;
			tnearInit = TRUE;
		} else 
		if (ABS(at-t) < ABS(tnear-t)) tnear = at;
		}

	if (posY && posY->GetNextKeyTime(t,flags,at)) {
		if (!tnearInit) {
			tnear = at;
			tnearInit = TRUE;
		} else 
		if (ABS(at-t) < ABS(tnear-t)) tnear = at;
		}

	if (posZ && posZ->GetNextKeyTime(t,flags,at)) {
		if (!tnearInit) {
			tnear = at;
			tnearInit = TRUE;
		} else 
		if (ABS(at-t) < ABS(tnear-t)) tnear = at;
		}
	
	if (tnearInit) {
		nt = tnear;
		return TRUE;
	} else {
		return FALSE;
		}
	}
		

void IndePosition::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{	
	this->ip = ip;

	if (dlg) {
		dlg->BeginingEdit(this,ip,(IndePosition*)prev);
	} else {
		dlg = new IPosDlg(this,ip);
		}
	}

void IndePosition::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
	{	
	IndePosition *cont=NULL;
	if (next && next->ClassID()==ClassID()) {
		cont = (IndePosition*)next;
		}

	if (dlg) {
		if (cont) {
			dlg->EndingEdit(cont);
		} else {
			delete dlg;
			dlg = NULL;
			}
		}
	}

/**********************************************************************
 *<
	FILE: eulrctrl.cpp

	DESCRIPTION: An Euler angle rotation controller

	CREATED BY: Rolf Berteig

	HISTORY: created 13 June 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "ctrl.h"
#include "interpik.h"
#include "euler.h"

#define EULER_CONTROL_CNAME		GetString(IDS_RB_EULERXYZ)

#define EULER_X_REF		0
#define EULER_Y_REF		1
#define EULER_Z_REF		2

#define THRESHHOLD		1.0f

class EulerDlg;

static DWORD subColor[] = {PAINTCURVE_XCOLOR, PAINTCURVE_YCOLOR, PAINTCURVE_ZCOLOR};

class EulerRotation : public Control {
	public:
		Control *rotX;
		Control *rotY;
		Control *rotZ;
		int order;
		Quat curval;
		Interval ivalid;

		static EulerDlg *dlg;
		static IObjParam *ip;
		static ULONG beginFlags;
		static EulerRotation *editControl; // The one being edited.

		EulerRotation(const EulerRotation &ctrl);
		EulerRotation(BOOL loading=FALSE);
		~EulerRotation();
		void Update(TimeValue t);

		// Animatable methods
		Class_ID ClassID() { return Class_ID(EULER_CONTROL_CLASS_ID,0); }  
		SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; }  		
		
		void GetClassName(TSTR& s);
		void DeleteThis() {delete this;}		
		int IsKeyable() {return 1;}		

		int NumSubs()  {return 3;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		DWORD GetSubAnimCurveColor(int subNum) {return subColor[subNum];}

		ParamDimension* GetParamDimension(int i) {return stdAngleDim;}
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

		int SetProperty(ULONG id, void *data);
		void *GetProperty(ULONG id);

		// Reference methods
		int NumRefs() { return 3; };	
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		void RescaleWorldUnits(float f) {}

		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		// Control methods
		Control *GetXController() {return rotX;}
		Control *GetYController() {return rotY;}
		Control *GetZController() {return rotZ;}
		void Copy(Control *from);
		RefTargetHandle Clone(RemapDir& remap);
		BOOL IsLeaf() {return FALSE;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);	
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);
		void CommitValue(TimeValue t);
		void RestoreValue(TimeValue t);
		void EnumIKParams(IKEnumCallback &callback);
		BOOL CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags);
		float IncIKParam(TimeValue t,int index,float delta);
		void ClearIKParam(Interval iv,int index);
		void EnableORTs(BOOL enable);
		void MirrorIKConstraints(int axis,int which);		
		BOOL CanCopyIKParams(int which);
		IKClipObject *CopyIKParams(int which);
		BOOL CanPasteIKParams(IKClipObject *co,int which);
		void PasteIKParams(IKClipObject *co,int which);

		void ChangeOrdering(int newOrder);
	};

EulerDlg *EulerRotation::dlg = NULL;
IObjParam *EulerRotation::ip = NULL;
ULONG EulerRotation::beginFlags = 0;
EulerRotation *EulerRotation::editControl = NULL;

class JointParamsEuler : public JointParams {
	public:			 	
		JointParamsEuler() : JointParams((DWORD)JNT_ROT,3) {flags |= JNT_LIMITEXACT;}
		void SpinnerChange(InterpCtrlUI *ui,WORD id,ISpinnerControl *spin,BOOL interactive);
	};

static BOOL CALLBACK EulerParamDialogProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

static const int editButs[] = {IDC_EULER_X,IDC_EULER_Y,IDC_EULER_Z};

static int eulerIDs[] = {
	IDS_RB_EULERTYPE0,IDS_RB_EULERTYPE1,IDS_RB_EULERTYPE2,
	IDS_RB_EULERTYPE3,IDS_RB_EULERTYPE4,IDS_RB_EULERTYPE5,
	IDS_RB_EULERTYPE6,IDS_RB_EULERTYPE7,IDS_RB_EULERTYPE8};

typedef int EAOrdering[3];
static EAOrdering orderings[] = {
	{0,1,2},
	{0,2,1},
	{1,2,0},
	{1,0,2},
	{2,0,1},
	{2,1,0},
	{0,1,0},
	{1,2,1},
	{2,0,2},
	};

static int xyzIDs[] = {IDS_RB_X,IDS_RB_Y,IDS_RB_Z};
static int xyzRotIDs[] = {IDS_RB_XROTATION,IDS_RB_YROTATION,IDS_RB_ZROTATION};
static int xyzAxisIDs[] = {IDS_RB_XAXIS,IDS_RB_YAXIS,IDS_RB_ZAXIS};

#define EDIT_X	0
#define EDIT_Y	1
#define EDIT_Z	2

#define EULER_BEGIN		1
#define EULER_MIDDLE	2
#define EULER_END		3

class EulerDlg {
	public:
		EulerRotation *cont;
		HWND hWnd;
		IObjParam *ip;
		ICustButton *iEdit[3];
		static int cur;
		
		EulerDlg(EulerRotation *cont,IObjParam *ip);
		~EulerDlg();

		void Init();
		void SetButtonText();
		void EndingEdit(EulerRotation *next);
		void BeginingEdit(EulerRotation *cont,IObjParam *ip,EulerRotation *prev);
		void SetCur(int c,int code=EULER_MIDDLE);
		void WMCommand(int id, int notify, HWND hCtrl);
	};

int EulerDlg::cur = EDIT_X;

EulerDlg::EulerDlg(EulerRotation *cont,IObjParam *ip)
	{
	this->ip   = ip;
	this->cont = cont;
	for (int i=0; i<3; i++) {
		iEdit[i] = NULL;
		}
	
	hWnd = ip->AddRollupPage( 
		hInstance,
		MAKEINTRESOURCE(IDD_EULER_PARAMS),
		EulerParamDialogProc,
		GetString(IDS_RB_EULERPARAMS), 
		(LPARAM)this);
	ip->RegisterDlgWnd(hWnd);	
	
	SetCur(cur,EULER_BEGIN);	
	UpdateWindow(hWnd);
	}

EulerDlg::~EulerDlg()
	{
	SetCur(cur,EULER_END);
	for (int i=0; i<3; i++) {
		ReleaseICustButton(iEdit[i]);		
		}
	ip->UnRegisterDlgWnd(hWnd);
	ip->DeleteRollupPage(hWnd);
	hWnd = NULL;
	}

void EulerDlg::EndingEdit(EulerRotation *next)
	{
	switch (cur) {
		case EDIT_X:
			cont->rotX->EndEditParams(ip,0,next->rotX);
			break;
		case EDIT_Y:
			cont->rotY->EndEditParams(ip,0,next->rotY);
			break;
		case EDIT_Z:
			cont->rotZ->EndEditParams(ip,0,next->rotZ);
			break;
		}
	cont = NULL;
	ip   = NULL;
	}

void EulerDlg::BeginingEdit(EulerRotation *cont,IObjParam *ip,EulerRotation *prev)
	{
	this->ip   = ip;
	this->cont = cont;
	switch (cur) {
		case EDIT_X:
			cont->rotX->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->rotX);
			break;
		case EDIT_Y:
			cont->rotY->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->rotY);
			break;
		case EDIT_Z:
			cont->rotZ->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev->rotZ);
			break;
		}	
	UpdateWindow(hWnd);
	}

void EulerDlg::SetButtonText()
	{
	for (int i=0; i<3; i++) {
		iEdit[i]->SetText(GetString(
			xyzIDs[orderings[cont->order][i]]));
		}
	}

void EulerDlg::Init()
	{	
	for (int i=0; i<3; i++) {
		iEdit[i] = GetICustButton(GetDlgItem(hWnd,editButs[i]));		
		iEdit[i]->SetType(CBT_CHECK);		
		}
	iEdit[cur]->SetCheck(TRUE);	
	SetButtonText();

	SendDlgItemMessage(hWnd,IDC_EULER_ORDER,CB_RESETCONTENT,0,0);
	for (i=0; i<9; i++) {
		SendDlgItemMessage(hWnd,IDC_EULER_ORDER,CB_ADDSTRING,0,
			(LPARAM)GetString(eulerIDs[i]));
		}
	SendDlgItemMessage(hWnd,IDC_EULER_ORDER,CB_SETCURSEL,cont->order,0);
	}

void EulerDlg::SetCur(int c,int code)
	{
	if (c==cur && code==EULER_MIDDLE) return;
	Control *prev = NULL, *next = NULL;

	if (code!=EULER_END) {
		switch (c) {
			case EDIT_X:
				next = cont->rotX;
				break;
			case EDIT_Y:
				next = cont->rotY;
				break;
			case EDIT_Z:
				next = cont->rotZ;
				break;
			}
		}

	if (code!=EULER_BEGIN) {
		switch (cur) {
			case EDIT_X:
				cont->rotX->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->rotX;
				break;
			case EDIT_Y:
				cont->rotY->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->rotY;
				break;
			case EDIT_Z:
				cont->rotZ->EndEditParams(ip,END_EDIT_REMOVEUI,next);
				prev = cont->rotZ;
				break;
			}
		}

	cur = c;

	if (code!=EULER_END) {
		switch (cur) {
			case EDIT_X:
				cont->rotX->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			case EDIT_Y:
				cont->rotY->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			case EDIT_Z:
				cont->rotZ->BeginEditParams(ip,BEGIN_EDIT_MOTION,prev);
				break;
			}
		}
	}

void EulerDlg::WMCommand(int id, int notify, HWND hCtrl)
	{
	switch (id) {
		case IDC_EULER_X:
			SetCur(0);
			iEdit[0]->SetCheck(TRUE);
			iEdit[1]->SetCheck(FALSE);
			iEdit[2]->SetCheck(FALSE);
			break;
		case IDC_EULER_Y:
			SetCur(1);
			iEdit[0]->SetCheck(FALSE);
			iEdit[1]->SetCheck(TRUE);
			iEdit[2]->SetCheck(FALSE);
			break;
		case IDC_EULER_Z:
			SetCur(2);
			iEdit[0]->SetCheck(FALSE);
			iEdit[1]->SetCheck(FALSE);
			iEdit[2]->SetCheck(TRUE);
			break;

		case IDC_EULER_ORDER:
			if (notify==CBN_SELCHANGE) {
				int res = SendDlgItemMessage(hWnd,IDC_EULER_ORDER,CB_GETCURSEL,0,0);
				if (res!=CB_ERR) {
					cont->ChangeOrdering(res);
					SetButtonText();
					}
				}
			break;			
		}
	}

static BOOL CALLBACK EulerParamDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	EulerDlg *dlg = (EulerDlg*)GetWindowLong(hDlg,GWL_USERDATA);

	switch (message) {
		case WM_INITDIALOG:
			dlg = (EulerDlg*)lParam;			
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


//********************************************************
// EULER CONTROL
//********************************************************
static Class_ID eulerControlClassID(EULER_CONTROL_CLASS_ID,0); 
class EulerClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new EulerRotation(loading); }
	const TCHAR *	ClassName() { return EULER_CONTROL_CNAME; }
	SClass_ID		SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	Class_ID		ClassID() { return eulerControlClassID; }
	const TCHAR* 	Category() { return _T("");  }
	};
static EulerClassDesc eulerCD;
ClassDesc* GetEulerCtrlDesc() {return &eulerCD;}

EulerRotation::EulerRotation(const EulerRotation &ctrl)
	{
	order = EULERTYPE_XYZ;
	rotX = NULL;
	rotY = NULL;
	rotZ = NULL;

	if (ctrl.rotX) {
		ReplaceReference(EULER_X_REF,ctrl.rotX);
	} else {
		ReplaceReference(EULER_X_REF,NewDefaultFloatController());
		}
	if (ctrl.rotY) {
		ReplaceReference(EULER_Y_REF,ctrl.rotY);
	} else {
		ReplaceReference(EULER_Y_REF,NewDefaultFloatController());
		}
	if (ctrl.rotZ) {
		ReplaceReference(EULER_Z_REF,ctrl.rotZ);
	} else {
		ReplaceReference(EULER_Z_REF,NewDefaultFloatController());
		}
	curval = ctrl.curval;
	ivalid = ctrl.ivalid;
	}

EulerRotation::EulerRotation(BOOL loading) 
	{
	order = EULERTYPE_XYZ;
	rotX = NULL;
	rotY = NULL;
	rotZ = NULL;
	if (!loading) {
		ReplaceReference(EULER_X_REF,NewDefaultFloatController());
		ReplaceReference(EULER_Y_REF,NewDefaultFloatController());
		ReplaceReference(EULER_Z_REF,NewDefaultFloatController());
		ivalid = FOREVER;
		curval.Identity();
	} else {
		ivalid.SetEmpty();
		}	
	}

RefTargetHandle EulerRotation::Clone(RemapDir& remap) 
	{
	EulerRotation *euler = new EulerRotation(TRUE);	
	euler->ReplaceReference(EULER_X_REF, remap.CloneRef(rotX));
	euler->ReplaceReference(EULER_Y_REF, remap.CloneRef(rotY));
	euler->ReplaceReference(EULER_Z_REF, remap.CloneRef(rotZ));
	euler->order = order;

	JointParams *jp = (JointParams*)GetProperty(PROPID_JOINTPARAMS);
    if (jp) {
		JointParams *jp2 = new JointParams(*jp);
		euler->SetProperty(PROPID_JOINTPARAMS,jp2);
		}
	return euler;
	}



EulerRotation::~EulerRotation()
	{
	DeleteAllRefsFromMe();
	}

void EulerRotation::GetClassName(TSTR& s)
	{		
	TSTR format(GetString(IDS_RB_EULERNAME));
	s.printf(format,GetString(eulerIDs[order]));
	}

// This copy method will sample the from controller and smooth out all flips
// Nikolai 1-15-99
void EulerRotation::Copy(Control *from)
	{
	if (from->ClassID()==ClassID()) {
		EulerRotation *ctrl = (EulerRotation*)from;
		ReplaceReference(EULER_X_REF,ctrl->rotX);
		ReplaceReference(EULER_Y_REF,ctrl->rotY);
		ReplaceReference(EULER_Z_REF,ctrl->rotZ);
		curval = ctrl->curval;
		ivalid = ctrl->ivalid;
		order  = ctrl->order;
	} else {		
		Quat qPrev;
		Quat qCurr;
		Interval iv;
		int num;		
		if ((num=from->NumKeys())!=NOT_KEYFRAMEABLE && num>0) {
			SuspendAnimate();
			AnimateOn();
			Interval anim;

			anim.SetStart(from->GetKeyTime(0));

			float eaCurr[3];
			float eaPrev[3];
			float EulerAng[3] = {0,0,0};

			from->GetValue(anim.Start(),&qPrev,iv);

			Matrix3 tm;
			qPrev.MakeMatrix(tm);
			MatrixToEuler(tm,EulerAng, order);
				
			rotX->SetValue(anim.Start(),&EulerAng[0],TRUE, CTRL_ABSOLUTE);
			rotY->SetValue(anim.Start(),&EulerAng[1],TRUE, CTRL_ABSOLUTE);
			rotZ->SetValue(anim.Start(),&EulerAng[2],TRUE, CTRL_ABSOLUTE);
			
			if(num>1)
			{
				float dEuler[3],f;	
				Matrix3 tmPrev, tmCurr;

				anim.SetEnd(from->GetKeyTime(num-1));
				
				// Here we sample over the time range, to detect flips
				for(TimeValue time = anim.Start()+1; time <= anim.End() ; time++  )
				{
					from->GetValue(time,&qCurr,iv);
					
					qPrev.MakeMatrix(tmPrev);
					qCurr.MakeMatrix(tmCurr);

					// The Euler/Quat ratio is the relation of the angle difference in Euler space to 
					// the angle difference in Quat space. If this ration is bigger than PI the rotation 
					// between the two time steps contains a flip

					f = GetEulerMatAngleRatio(tmPrev,tmCurr,eaPrev,eaCurr,order);	
										
					if(  f > PI)
					{
						// We found a flip here
						for(int j=0 ; j < 3 ; j++)
						{				
							// find the sign flip :
							if(fabs((eaCurr[j]-eaPrev[j])) < 2*PI-THRESHHOLD )
								dEuler[j] = eaCurr[j]-eaPrev[j];
							else
								// unflip the flip
								dEuler[j] = (2*PI - (float) (fabs(eaCurr[j]) + fabs(eaPrev[j]))) * (eaPrev[j] > 0 ? 1 : -1);
							
							EulerAng[j] += dEuler[j];
						}
					}
					else
					{
						// Add up the angle difference
						for(int j=0 ; j < 3 ; j++)
						{
							dEuler[j] = eaCurr[j]-eaPrev[j];
							EulerAng[j] += dEuler[j];
						}
					}
					if(from->IsKeyAtTime(time,KEYAT_ROTATION))
					{
						// Create the keys
						rotX->SetValue(time,&EulerAng[0],TRUE, CTRL_ABSOLUTE);
						rotY->SetValue(time,&EulerAng[1],TRUE, CTRL_ABSOLUTE);
						rotZ->SetValue(time,&EulerAng[2],TRUE, CTRL_ABSOLUTE);
					}
					qPrev = qCurr;
				}
			}
			// RB 2/10/99: A key at frame 0 may have been created
			if (num>0 && from->GetKeyTime(0)!=0) {
				rotX->DeleteKeyAtTime(0);
				rotY->DeleteKeyAtTime(0);
				rotZ->DeleteKeyAtTime(0);
			}
			ResumeAnimate();
		} else {
			from->GetValue(0,&qCurr,ivalid);
			SetValue(0,&qCurr,TRUE,CTRL_ABSOLUTE);
			}
		}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

  /*
void EulerRotation::Copy(Control *from)
	{
	if (from->ClassID()==ClassID()) {
		EulerRotation *ctrl = (EulerRotation*)from;
		ReplaceReference(EULER_X_REF,ctrl->rotX);
		ReplaceReference(EULER_Y_REF,ctrl->rotY);
		ReplaceReference(EULER_Z_REF,ctrl->rotZ);
		curval = ctrl->curval;
		ivalid = ctrl->ivalid;
		order  = ctrl->order;
	} else {		
		Quat v;
		Interval iv;
		int num;		
		if (num=from->NumKeys()) {
			SuspendAnimate();
			AnimateOn();
			for (int i=0; i<num; i++) {
				TimeValue t = from->GetKeyTime(i);
				from->GetValue(t,&v,iv);
				SetValue(t,&v,TRUE,CTRL_ABSOLUTE);	
				}
			ResumeAnimate();
		} else {
			from->GetValue(0,&v,ivalid);
			SetValue(0,&v,TRUE,CTRL_ABSOLUTE);
			}
		}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}
*/

void EulerRotation::Update(TimeValue t)
	{
	if (!ivalid.InInterval(t)) {
		ivalid = FOREVER;
		Point3 ang(0,0,0);
		if (rotX) rotX->GetValue(t,&ang.x,ivalid);
		if (rotY) rotY->GetValue(t,&ang.y,ivalid);
		if (rotZ) rotZ->GetValue(t,&ang.z,ivalid);
		
		// This could be optimized.
		//Matrix3 tm(1);
		//tm.RotateX(x);
		//tm.RotateY(y);
		//tm.RotateZ(z);
		//curval = Quat(tm);		
		Matrix3 tm(1);
		for (int i=0; i<3; i++) {
			switch (orderings[order][i]) {
				case 0: tm.RotateX(ang[i]); break;
				case 1: tm.RotateY(ang[i]); break;
				case 2: tm.RotateZ(ang[i]); break;
				}
			}
		curval = Quat(tm);
		//EulerToQuat(ang, curval, order);
		}
	}

void EulerRotation::ChangeOrdering(int newOrder)
	{
	order = newOrder;
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	ip->RedrawViews(ip->GetTime());
	}

void EulerRotation::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
	{
	Quat v;
	Update(t);
	if (method==CTRL_RELATIVE) {
		v = curval * Quat(*((AngAxis*)val));
	} else {
		v = *((Quat*)val);
		}
		
	float ang[3];
	//QuatToEuler(v,ang);
	Matrix3 tm;
	v.MakeMatrix(tm);
	MatrixToEuler(tm,ang, order);
	
	// RB: this gives the incorrect sign sometimes...
	//QuatToEuler(v, ang, order);		

	if (rotX) rotX->SetValue(t,&ang[0]);
	if (rotY) rotY->SetValue(t,&ang[1]);
	if (rotZ) rotZ->SetValue(t,&ang[2]);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void EulerRotation::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
	{		
	Update(t);
	valid &= ivalid;			 
	if (method==CTRL_RELATIVE) {
  		Matrix3 *mat = (Matrix3*)val;		
		PreRotateMatrix(*mat,curval);		
	} else {
		*((Quat*)val) = curval;
		}
	}

void EulerRotation::CommitValue(TimeValue t)
	{
	if (rotX) rotX->CommitValue(t);
	if (rotY) rotY->CommitValue(t);
	if (rotZ) rotZ->CommitValue(t);
	}

void EulerRotation::RestoreValue(TimeValue t)
	{
	if (rotX) rotX->RestoreValue(t);
	if (rotY) rotY->RestoreValue(t);
	if (rotZ) rotZ->RestoreValue(t);
	}

RefTargetHandle EulerRotation::GetReference(int i)
	{
	switch (i) {
		case EULER_X_REF: return rotX;
		case EULER_Y_REF: return rotY;
		case EULER_Z_REF: return rotZ;
		default: return NULL;
		}
	}

void EulerRotation::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case EULER_X_REF: rotX = (Control*)rtarg; break;
		case EULER_Y_REF: rotY = (Control*)rtarg; break;
		case EULER_Z_REF: rotZ = (Control*)rtarg; break;
		}
	}

Animatable* EulerRotation::SubAnim(int i)
	{
	return GetReference(i);
	}

TSTR EulerRotation::SubAnimName(int i)
	{	
	switch (i) {
		case EULER_X_REF: return GetString(xyzRotIDs[orderings[order][0]]);
		case EULER_Y_REF: return GetString(xyzRotIDs[orderings[order][1]]);
		case EULER_Z_REF: return GetString(xyzRotIDs[orderings[order][2]]);
		default: return _T("");
		}
	}

RefResult EulerRotation::NotifyRefChanged(
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
			if (rotX == hTarg) rotX = NULL;
			if (rotY == hTarg) rotY = NULL;
			if (rotZ == hTarg) rotZ = NULL; 
			break;
		case REFMSG_GET_CONTROL_DIM: {
			ParamDimension **dim = (ParamDimension **)partID;
			assert(dim);
			*dim = stdAngleDim;
			}
		}
	return REF_SUCCEED;
	}

BOOL EulerRotation::AssignController(Animatable *control,int subAnim)
	{	
	switch (subAnim) {
		case EULER_X_REF:
			ReplaceReference(EULER_X_REF,(RefTargetHandle)control);
			break;
		case EULER_Y_REF:
			ReplaceReference(EULER_Y_REF,(RefTargetHandle)control);
			break;
		case EULER_Z_REF:
			ReplaceReference(EULER_Z_REF,(RefTargetHandle)control);
			break;
		}
	NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE,TREE_VIEW_CLASS_ID,FALSE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	return TRUE;
	}

void EulerRotation::AddNewKey(TimeValue t,DWORD flags)
	{
	if (rotX) rotX->AddNewKey(t,flags);
	if (rotY) rotY->AddNewKey(t,flags);
	if (rotZ) rotZ->AddNewKey(t,flags);
	}

int EulerRotation::NumKeys()
	{
	int num = 0;
	if (rotX) num += rotX->NumKeys(); 
	if (rotY) num += rotY->NumKeys();
	if (rotZ) num += rotZ->NumKeys();
	return num;
	}

TimeValue EulerRotation::GetKeyTime(int index)
	{
	int onum,num = 0;
	if (rotX) num += rotX->NumKeys(); 
	if (index < num) return rotX->GetKeyTime(index);
	onum = num;
	if (rotY) num += rotY->NumKeys(); 
	if (index < num) return rotY->GetKeyTime(index-onum);
	onum = num;
	if (rotZ) num += rotZ->NumKeys(); 
	if (index < num) return rotZ->GetKeyTime(index-onum);
	return 0;
	}

void EulerRotation::CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags)
	{
	if (rotX) rotX->CopyKeysFromTime(src,dst,flags);
	if (rotY) rotY->CopyKeysFromTime(src,dst,flags);
	if (rotZ) rotZ->CopyKeysFromTime(src,dst,flags);
	}

BOOL EulerRotation::IsKeyAtTime(TimeValue t,DWORD flags)
	{
	if (rotX && rotX->IsKeyAtTime(t,flags)) return TRUE;
	if (rotY && rotY->IsKeyAtTime(t,flags)) return TRUE;
	if (rotZ && rotZ->IsKeyAtTime(t,flags)) return TRUE;
	return FALSE;
	}

void EulerRotation::DeleteKeyAtTime(TimeValue t)
	{
	if (rotX) rotX->DeleteKeyAtTime(t);
	if (rotY) rotY->DeleteKeyAtTime(t);
	if (rotZ) rotZ->DeleteKeyAtTime(t);
	}

BOOL EulerRotation::GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt)
	{
	TimeValue at,tnear = 0;
	BOOL tnearInit = FALSE;
	
	if (rotX && rotX->GetNextKeyTime(t,flags,at)) {
		if (!tnearInit) {
			tnear = at;
			tnearInit = TRUE;
		} else 
		if (ABS(at-t) < ABS(tnear-t)) tnear = at;
		}

	if (rotY && rotY->GetNextKeyTime(t,flags,at)) {
		if (!tnearInit) {
			tnear = at;
			tnearInit = TRUE;
		} else 
		if (ABS(at-t) < ABS(tnear-t)) tnear = at;
		}

	if (rotZ && rotZ->GetNextKeyTime(t,flags,at)) {
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
		

void EulerRotation::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	if (flags&BEGIN_EDIT_HIERARCHY) {
		JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
		InterpCtrlUI *ui;	

		if (!jp) {
			jp = new JointParamsEuler();
			SetProperty(PROPID_JOINTPARAMS,jp);
			}

		if (prev &&
			prev->ClassID()==ClassID() && 
		    (ui = (InterpCtrlUI*)prev->GetProperty(PROPID_INTERPUI))) {
			JointParams *prevjp = (JointParams*)prev->GetProperty(PROPID_JOINTPARAMS);
			prevjp->EndDialog(ui);
			ui->cont = this;
			ui->ip   = ip;
			prev->SetProperty(PROPID_INTERPUI,NULL);
			JointDlgData *jd = (JointDlgData*)GetWindowLong(ui->hParams,GWL_USERDATA);
			jd->jp = jp;
			jp->InitDialog(ui);
		} else {
			ui = new InterpCtrlUI(NULL,ip,this);
			DWORD f=0;
			if (jp && !jp->RollupOpen()) f = APPENDROLL_CLOSED;	

			ui->hParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_STDJOINTPARAMS),
				JointParamDlgProc,
				GetString(IDS_RB_ROTJOINTPARAMS), 
				(LPARAM)new JointDlgData(ui,jp),f);	
			}
	
		SetDlgItemText(ui->hParams,IDC_XAXIS_LABEL,
			GetString(xyzAxisIDs[orderings[order][0]]));
		SetDlgItemText(ui->hParams,IDC_YAXIS_LABEL,
			GetString(xyzAxisIDs[orderings[order][1]]));
		SetDlgItemText(ui->hParams,IDC_ZAXIS_LABEL,
			GetString(xyzAxisIDs[orderings[order][2]]));

		SetProperty(PROPID_INTERPUI,ui);
		editControl = this;
		beginFlags = flags;
	} else 
	if (flags&BEGIN_EDIT_MOTION) {
		this->ip = ip;

		if (dlg) {
			dlg->BeginingEdit(this,ip,(EulerRotation*)prev);
			dlg->Init();
		} else {
			dlg = new EulerDlg(this,ip);	
			}
		}
	}

void EulerRotation::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{	
	EulerRotation *cont=NULL;
	if (next && next->ClassID()==ClassID()) {
		cont = (EulerRotation*)next;
		}

	if (dlg) {
		if (cont) {
			dlg->EndingEdit(cont);
		} else {
			delete dlg;
			dlg = NULL;
			}
	} else {
		if (cont) return;
		
		editControl = NULL;
		beginFlags = 0;

		int index = aprops.FindProperty(PROPID_INTERPUI);
		if (index>=0) {
			InterpCtrlUI *ui = (InterpCtrlUI*)aprops[index];
			if (ui->hParams) {
				ip->UnRegisterDlgWnd(ui->hParams);
				ip->DeleteRollupPage(ui->hParams);			
				}
			index = aprops.FindProperty(PROPID_INTERPUI);
			if (index>=0) {
				delete aprops[index];
				aprops.Delete(index,1);
				}
			}
		}
	}

int EulerRotation::SetProperty(ULONG id, void *data)
	{
	if (id==PROPID_JOINTPARAMS) {		
		if (!data) {
			int index = aprops.FindProperty(id);
			if (index>=0) {
				aprops.Delete(index,1);
				}
		} else {
			JointParamsEuler *jp = (JointParamsEuler*)GetProperty(id);
			if (jp) {
				*jp = *((JointParamsEuler*)data);
				delete (JointParamsEuler*)data;
			} else {
				aprops.Append(1,(AnimProperty**)&data);
				}					
			}
		return 1;
	} else
	if (id==PROPID_INTERPUI) {		
		if (!data) {
			int index = aprops.FindProperty(id);
			if (index>=0) {				
				aprops.Delete(index,1);
				}
		} else {
			InterpCtrlUI *ui = (InterpCtrlUI*)GetProperty(id);
			if (ui) {
				*ui = *((InterpCtrlUI*)data);
			} else {
				aprops.Append(1,(AnimProperty**)&data);
				}					
			}
		return 1;
	} else {
		return Animatable::SetProperty(id,data);
		}
	}

void* EulerRotation::GetProperty(ULONG id)
	{
	if (id==PROPID_INTERPUI || id==PROPID_JOINTPARAMS) {
		int index = aprops.FindProperty(id);
		if (index>=0) {
			return aprops[index];
		} else {
			return NULL;
			}
	} else {
		return Animatable::GetProperty(id);
		}
	}


#define JOINTPARAMEULER_CHUNK	0x1002
#define ORDER_CHUNK				0x1003

IOResult EulerRotation::Save(ISave *isave)
	{	
	ULONG nb;
	JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
	if (jp) {
		isave->BeginChunk(JOINTPARAMEULER_CHUNK);
		jp->Save(isave);
		isave->EndChunk();
		}

	isave->BeginChunk(ORDER_CHUNK);
	isave->Write(&order,sizeof(order),&nb);
	isave->EndChunk();

	return IO_OK;
	}

IOResult EulerRotation::Load(ILoad *iload)
	{
	ULONG nb;
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case ORDER_CHUNK:
				res=iload->Read(&order,sizeof(order),&nb);
				break;

			case JOINTPARAMEULER_CHUNK: {
				JointParamsEuler *jp = new JointParamsEuler;
				jp->Load(iload);
				jp->flags |= JNT_LIMITEXACT;
				SetProperty(PROPID_JOINTPARAMS,jp);
				break;
				}
			}		
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}
	return IO_OK;
	}


void EulerRotation::EnumIKParams(IKEnumCallback &callback)
	{
	JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
	for (int i=2; i>=0; i--) {
		if (!jp || jp->Active(i)) {
			callback.proc(this,i);
			}
		}
	}

BOOL EulerRotation::CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags)
	{
	JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
	Quat q;
	Interval valid;
	Point3 a(0,0,0);

	if (rotX) rotX->GetValue(t,&a[0],valid);
	if (rotY) rotY->GetValue(t,&a[1],valid);
	if (rotZ) rotZ->GetValue(t,&a[2],valid);

	for (int i=2; i>=0; i--) {
		if (!jp || jp->Active(i)) {
			for (int j=0; j<derivs.NumEndEffectors(); j++) {
				Point3 r = derivs.EndEffectorPos(j) - ptm.GetTrans();	
		
				Point3 axis = ptm.GetRow(orderings[order][i]);
				if (!(ptm.GetIdentFlags()&SCL_IDENT)) {
					axis = Normalize(axis);
					if (ptm.Parity()) axis = -axis;
					}

				if (flags&POSITION_DERIV) {
					derivs.DP(CrossProd(axis,r),j);
					}
				if (flags&ROTATION_DERIV) {
					derivs.DR(axis,j);
					}
				}
			derivs.NextDOF();			
			}
		switch (orderings[order][i]) {
			case 0: ptm.PreRotateX(a[i]); break;
			case 1: ptm.PreRotateY(a[i]); break;
			case 2: ptm.PreRotateZ(a[i]); break;
			}
		}	
	return TRUE;
	}

#define MAX_IKROT	DegToRad(4.0f)
#define SGN(a)	(a<0?-1:1)

float EulerRotation::IncIKParam(TimeValue t,int index,float delta)
	{
	JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
	if ((float)fabs(delta)>MAX_IKROT) delta = MAX_IKROT * SGN(delta);
	
	if (jp) {
		float v=0.0f;		
		if (jp->Limited(index) || jp->Spring(index)) {
			Interval valid;
			switch (index) {
				case 0: if (rotX) rotX->GetValue(t,&v,valid); break;
				case 1: if (rotY) rotY->GetValue(t,&v,valid); break;
				case 2: if (rotZ) rotZ->GetValue(t,&v,valid); break;
				}
			}
		delta = jp->ConstrainInc(index,v,delta);
		}
	switch (index) {
		case 0: if (rotX) rotX->SetValue(t,&delta,FALSE,CTRL_RELATIVE); break;
		case 1: if (rotY) rotY->SetValue(t,&delta,FALSE,CTRL_RELATIVE); break;
		case 2: if (rotZ) rotZ->SetValue(t,&delta,FALSE,CTRL_RELATIVE); break;
		}	
	return delta;	
	}

void EulerRotation::ClearIKParam(Interval iv,int index) 
	{
	switch (index) {
		case 0: if (rotX) rotX->DeleteTime(iv,TIME_INCRIGHT|TIME_NOSLIDE); break;
		case 1: if (rotY) rotY->DeleteTime(iv,TIME_INCRIGHT|TIME_NOSLIDE); break;
		case 2: if (rotZ) rotZ->DeleteTime(iv,TIME_INCRIGHT|TIME_NOSLIDE); break;
		}
	}

void EulerRotation::MirrorIKConstraints(int axis,int which)
	{
	JointParamsEuler *jp = (JointParamsEuler*)GetProperty(PROPID_JOINTPARAMS);
	if (jp) jp->MirrorConstraints(axis);
	}

void EulerRotation::EnableORTs(BOOL enable)
	{
	if (rotX) rotX->EnableORTs(enable);
	if (rotY) rotY->EnableORTs(enable);
	if (rotZ) rotZ->EnableORTs(enable);
	}

BOOL EulerRotation::CanCopyIKParams(int which)
	{
	return ::CanCopyIKParams(this,which);
	}

IKClipObject *EulerRotation::CopyIKParams(int which)
	{
	return ::CopyIKParams(this,which);
	}

BOOL EulerRotation::CanPasteIKParams(IKClipObject *co,int which)
	{
	return ::CanPasteIKParams(this,co,which);
	}

void EulerRotation::PasteIKParams(IKClipObject *co,int which)
	{
	::PasteIKParams(this,co,which);
	}

void JointParamsEuler::SpinnerChange(
		InterpCtrlUI *ui,WORD id,ISpinnerControl *spin,BOOL interactive)
	{
	EulerRotation *c = (EulerRotation*)ui->cont;
	Point3 a(0,0,0);
	BOOL set = FALSE;
	Interval valid;

	if (c->rotX) c->rotX->GetValue(ui->ip->GetTime(),&a[0],valid);
	if (c->rotY) c->rotY->GetValue(ui->ip->GetTime(),&a[1],valid);
	if (c->rotZ) c->rotZ->GetValue(ui->ip->GetTime(),&a[2],valid);

	switch (id) {
		case IDC_XFROMSPIN:
			a[0] = min[0] = DegToRad(spin->GetFVal()); 
			set = TRUE;
			break;
		case IDC_XTOSPIN:
			a[0] = max[0] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		case IDC_XSPRINGSPIN:
			a[0] = spring[0] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		
		case IDC_YFROMSPIN:
			a[1] = min[1] = DegToRad(spin->GetFVal()); 
			set = TRUE;
			break;
		case IDC_YTOSPIN:
			a[1] = max[1] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		case IDC_YSPRINGSPIN:
			a[0] = spring[1] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		
		case IDC_ZFROMSPIN:
			a[2] = min[2] = DegToRad(spin->GetFVal()); 
			set = TRUE;
			break;
		case IDC_ZTOSPIN:
			a[2] = max[2] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		case IDC_ZSPRINGSPIN:
			a[2] = spring[0] = DegToRad(spin->GetFVal());
			set = TRUE;
			break;
		
		case IDC_XDAMPINGSPIN:
			damping[0] = spin->GetFVal(); break;		
		case IDC_YDAMPINGSPIN:
			damping[1] = spin->GetFVal(); break;		
		case IDC_ZDAMPINGSPIN:
			damping[2] = spin->GetFVal(); break;

		case IDC_XSPRINGTENSSPIN:
			stens[0] = spin->GetFVal()/SPRINGTENS_UI; break;
		case IDC_YSPRINGTENSSPIN:
			stens[1] = spin->GetFVal()/SPRINGTENS_UI; break;
		case IDC_ZSPRINGTENSSPIN:
			stens[2] = spin->GetFVal()/SPRINGTENS_UI; break;
		}
	
	if (set && interactive) {		 		
 		if (c->rotX) c->rotX->SetValue(ui->ip->GetTime(),&a[0],TRUE,CTRL_ABSOLUTE);
		if (c->rotY) c->rotY->SetValue(ui->ip->GetTime(),&a[1],TRUE,CTRL_ABSOLUTE);
		if (c->rotZ) c->rotZ->SetValue(ui->ip->GetTime(),&a[2],TRUE,CTRL_ABSOLUTE);
		ui->ip->RedrawViews(ui->ip->GetTime(),REDRAW_INTERACTIVE);
		}
	}




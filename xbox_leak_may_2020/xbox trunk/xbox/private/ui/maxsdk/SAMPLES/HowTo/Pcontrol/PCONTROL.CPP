/*===========================================================================*\
  How To Create a Controller Plug-In

  FILE: PControl.cpp
 
  DESCRIPTION: A controller that moves an object along a spline path

  CREATED BY: Rolf Berteig

  HISTORY: created 13 June 1995

  Copyright (c) 1994, All Rights Reserved.
\*===========================================================================*/
#include "max.h"
#include "resource.h"
#include "units.h"
#include "interpik.h"
#include "spline3d.h"
#include "istdplug.h"

// This is the DLL instance handle passed in when the plug-in is 
// loaded at startup.
HINSTANCE hInstance;

// This function returns a pointer to a string in the string table of
// the resource library.  Note that this function maintains the buffer
// and that only one string is loaded at a time.  Therefore if you intend
// to use this string, you must copy to another buffer since it will 
// be overwritten on the next GetString() call.
TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

// Unique class ID value
#define PCONTROL_CLASS_ID Class_ID(0x50f65c4c, 0x59863a96)

#define InterpType() ((ConstVel())?SPLINE_INTERP_NORMALIZED:SPLINE_INTERP_SIMPLE)

class PathPosition;

class PickPathMode : public PickModeCallback, public PickNodeCallback {
	public:		
		PathPosition *cont;
		
		PickPathMode(PathPosition *c) {cont=c;}
		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{ return TRUE; }

		BOOL Filter(INode *node);
		
		PickNodeCallback *GetFilter() {return this;}
	};

// flag values
#define PATHFLAG_FOLLOW		(1<<0)
#define PATHFLAG_BANK		(1<<1)
#define PATHFLAG_CLOSED		(1<<2)
#define PATHFLAG_ALLOWFLIP	(1<<3)
#define PATHFLAG_CONSTVEL	(1<<4)
#define PATHFLAG_FLIP		(1<<5)
#define PATHFLAG_YAXIS		(1<<6)
#define PATHFLAG_ZAXIS		(1<<7)

#define PATHPOS_PERCENT_REF	0

class PathPosition : public	IPathPosition, public TimeChangeCallback {
	public:
		Control *percent;
		INode *path;
		DWORD flags;
		float bankAmount, tracking;
		Point3 curval;
		Quat curRot;
		Interval ivalid;
		
		static HWND hPathParams;
		static IObjParam *ip;
		static ICustButton *iPick;
		static ISpinnerControl *iBank, *iTrack, *iPerc;
		static PickPathMode *pickMode;
		static PathPosition *editCont;

		PathPosition(const PathPosition &ctrl);
		PathPosition(BOOL loading=FALSE);
		~PathPosition();

		BOOL SetPathNode(INode *node);
		void SetBankAmount(float a);
		void SetTracking(float t);
		void SetFollow(BOOL f);
		void SetBank(BOOL b);
		void SetAllowFlip(BOOL f);
		void SetConstVel(BOOL cv);
		void SetFlip(BOOL onOff);
		void SetAxis(int axis);
		
		BOOL GetFollow() {return Follow();}
		float GetBankAmount();
		BOOL GetBank() {return Bank();}
		float GetTracking();
		BOOL GetAllowFlip() {return AllowFlip();}
		BOOL GetConstVel() {return ConstVel();}		

		BOOL Follow() {return flags&PATHFLAG_FOLLOW?TRUE:FALSE;}
		BOOL Bank() {return flags&PATHFLAG_BANK?TRUE:FALSE;}
		BOOL AllowFlip() {return flags&PATHFLAG_ALLOWFLIP?TRUE:FALSE;}
		BOOL ConstVel() {return flags&PATHFLAG_CONSTVEL?TRUE:FALSE;}
		BOOL GetFlip() {return flags&PATHFLAG_FLIP?TRUE:FALSE;}
		int GetAxis();		
		
		void Update(TimeValue t);
		float GetPercent(TimeValue t,Interval &valid,BOOL noClip=FALSE);
		Point3 PointOnPath(TimeValue t, ShapeObject *pathOb, Interval &valid);
		Matrix3 CalcRefFrame(TimeValue t,ShapeObject *pathOb);

		// Animatable methods
		Class_ID ClassID() { return PCONTROL_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }  		
		
		void GetClassName(TSTR& s) {s = GetString(IDS_RB_PATH);}
		void DeleteThis() {delete this;}		
		int IsKeyable() {return 0;}		

		int NumSubs()  {return 1;}
		Animatable* SubAnim(int i) {return percent;}
		TSTR SubAnimName(int i) {return GetString(IDS_RB_PERCENT);}
		int SubNumToRefNum(int subNum) {if (subNum==0) return 0; else return -1;}

		ParamDimension* GetParamDimension(int i) {return stdPercentDim;}
		BOOL AssignController(Animatable *control,int subAnim);

		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev ); 
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next ); 

		int SetProperty(ULONG id, void *data);
		void *GetProperty(ULONG id);

		// Reference methods
		int NumRefs() { return 2; };	
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);

		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		// Control methods
		void Copy(Control *from);
		RefTargetHandle Clone(RemapDir& remap);
		BOOL IsLeaf() {return FALSE;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);	
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method) {}
		void CommitValue(TimeValue t) {percent->CommitValue(t);}
		void RestoreValue(TimeValue t) {percent->RestoreValue(t);}
		void EnumIKParams(IKEnumCallback &callback);
		BOOL CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags);
		float IncIKParam(TimeValue t,int index,float delta);
		void ClearIKParam(Interval iv,int index);

		void TimeChanged(TimeValue t);
	};

class JointParamsPath : public JointParams {
	public:			 	
		JointParamsPath() : JointParams((DWORD)JNT_POS,1,100.0f) {}
		void SpinnerChange(InterpCtrlUI *ui,WORD id,ISpinnerControl *spin,BOOL interactive);
	};


//********************************************************
// PATH CONTROL
//********************************************************
PathPosition::PathPosition(const PathPosition &ctrl)
	{
	percent = NULL;
	path    = NULL;
	if (ctrl.percent) {
		ReplaceReference(PATHPOS_PERCENT_REF,ctrl.percent);
	} else {
		ReplaceReference(PATHPOS_PERCENT_REF,NewDefaultFloatController());		
		
		percent->AddNewKey(GetAnimStart(),ADDKEY_INTERP);		
		percent->AddNewKey(GetAnimEnd(),ADDKEY_INTERP);
		SuspendAnimate();
		AnimateOn();
		float val=0.0f;
		percent->SetValue(GetAnimStart(),&val);
		val =  1.0f;
		percent->SetValue(GetAnimEnd(),&val); 
		ResumeAnimate();
		}
	if (ctrl.path) {
		ReplaceReference(PATHPOS_PATH_REF,ctrl.path);
	} else {
		path = NULL;
		}
	flags      = 0;
	bankAmount = FromBankUI(0.5f);
	tracking   = FromTrackUI(0.5f);	
	curval = ctrl.curval;
	curRot = ctrl.curRot;
	ivalid = ctrl.ivalid;
	}

PathPosition::PathPosition(BOOL loading) 
	{
	percent  = NULL;
	path     = NULL;	
	curval   = Point3(0,0,0);
	curRot.Identity();
	flags      = 0;
	bankAmount = FromBankUI(0.5f);
	tracking   = FromTrackUI(0.5f);

	if (!loading) {
		ReplaceReference(PATHPOS_PERCENT_REF,NewDefaultFloatController());		
		
		percent->AddNewKey(GetAnimStart(),ADDKEY_INTERP);		
		percent->AddNewKey(GetAnimEnd(),ADDKEY_INTERP);
		SuspendAnimate();
		AnimateOn();
		float val=0.0f;
		percent->SetValue(GetAnimStart(),&val);
		val =  1.0f;
		percent->SetValue(GetAnimEnd(),&val); 
		ResumeAnimate();		
		}
	ivalid.SetEmpty();	
	}

PathPosition::~PathPosition()
	{
	DeleteAllRefsFromMe();
	}

RefTargetHandle PathPosition::Clone(RemapDir& remap)
	{
	PathPosition *p = new PathPosition(TRUE);

	if (path) {
		p->ReplaceReference(PATHPOS_PATH_REF,path);
	} else {
		p->path = NULL;
		}

	if (percent) {
		p->ReplaceReference(PATHPOS_PERCENT_REF,remap.CloneRef(percent));
	} else {
		p->ReplaceReference(PATHPOS_PERCENT_REF,NewDefaultFloatController());
		}

	p->flags      = flags;
	p->bankAmount = bankAmount;
	p->tracking   = tracking;
	p->curval     = curval;
	p->curRot     = curRot;	
	p->ivalid.SetEmpty();
	return p;
	}


void PathPosition::Copy(Control *from)
	{
	if (from->ClassID()==ClassID()) {
		PathPosition *ctrl = (PathPosition*)from;
		ReplaceReference(PATHPOS_PERCENT_REF,ctrl->percent);
		ReplaceReference(PATHPOS_PATH_REF,ctrl->path);
		curval   = ctrl->curval;
		curRot   = ctrl->curRot;
		ivalid   = ctrl->ivalid;		
	} else {		
		from->GetValue(0,&curval,ivalid);
		}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}


float PathPosition::GetPercent(TimeValue t,Interval &valid,BOOL noClip)
	{	
	float per;
	percent->GetValue(t,&per,valid,CTRL_ABSOLUTE);	
	if (noClip) return per;
	if (per < 0.0f || per > 1.0f || (flags&PATHFLAG_CLOSED)) {
		per = (float)fmod(per,1.0f);
		}
	if (per<0.0f) per = 1.0f + per;	
	return per;
	}

Point3 PathPosition::PointOnPath(TimeValue t, ShapeObject *pathOb, Interval &valid)
	{	
	return pathOb->InterpCurve3D(t, 0, GetPercent(t,valid), InterpType()) * 
		path->GetObjTMAfterWSM(t,&valid);	
	}

// Reference frame is calculated as the following:
//
// X axis = tangent(t)
// Y axis = WorldZ cross X
// Z axis = X cross Y
//
// or if the 'Allow Upside Down' option us selected:
//
// X axis = tangent(t)
// Z axis = path normal cross X
// Y axis = Z cross X
//
// If roll is on then the reference frame is rotated about
// the tangent by rollamount * curviture.

#define DU			0.001f
#define BDU			0.01f
#define NUMSTEPS	5

Matrix3 PathPosition::CalcRefFrame(TimeValue t,ShapeObject *pathOb)
	{
	Interval valid;
	Matrix3 tm(1), otm = path->GetObjTMAfterWSM(t);
	float u = GetPercent(t,valid);	
	
	Point3 pt0, pt1;
	pt0 = pathOb->InterpCurve3D(t, 0, u-DU, InterpType()) * otm;
	pt1 = pathOb->InterpCurve3D(t, 0, u+DU, InterpType()) * otm;
	
	Point3 pathNorm(0,0,0);
	if (AllowFlip()) {
		// Compute the normal to the plane of the path by sampling points on the path
#define NUM_SAMPLES 20
		Point3 v[NUM_SAMPLES], cent(0,0,0);
		for (int i=0; i<NUM_SAMPLES; i++) {
			v[i] = pathOb->InterpCurve3D(t, 0, float(i)/float(NUM_SAMPLES), InterpType()) * otm;
			cent += v[i];
			}		
		cent /= float(NUM_SAMPLES);
		for (i=1; i<NUM_SAMPLES; i++) {
			pathNorm += Normalize((v[i]-cent)^(v[i-1]-cent));
			}
		pathNorm = Normalize(pathNorm);
		}

	// X
	tm.SetRow(0,Normalize(pt1-pt0));

	if (GetFlip()) tm.SetRow(0,-tm.GetRow(0));

	if (AllowFlip()) {
		// Choose Z in the plane of the path		
		// Z
		tm.SetRow(2,Normalize(tm.GetRow(0)^pathNorm));

		// Y
		tm.SetRow(1,tm.GetRow(2)^tm.GetRow(0));
	} else {
		// Choose Y in the world XY plane
		// Y
		tm.SetRow(1,Normalize(Point3(0,0,1)^tm.GetRow(0)));

		// Z
		tm.SetRow(2,tm.GetRow(0)^tm.GetRow(1));
		}

	// swap axis around	
	Point3 tmp;
	switch (GetAxis()) {
		case 1: 
			tmp = tm.GetRow(0);
			tm.SetRow(0,-tm.GetRow(1));
			tm.SetRow(1,tmp);
			break;
		case 2: 
			tmp = tm.GetRow(0);
			tm.SetRow(0,-tm.GetRow(2));
			tm.SetRow(2,tmp);
			break;
		}
		
	
	if (Bank()) {
		// Average over NUMSTEPS samples
		Point3 pt0, pt1, pt2, v0, v1;
		float cv = 0.0f;
		u -= float(NUMSTEPS/2+1)*tracking;
				
		if (!pathOb->CurveClosed(t,0)) {
			if (u+(NUMSTEPS+2)*tracking > 1.0f) u = 1.0f - (NUMSTEPS+2)*tracking;
			if (u<0.0f) u=0.0f;
			}

		pt1 = pathOb->InterpCurve3D(t, 0, u, InterpType()) * otm;
		u += tracking;		
		pt2 = pathOb->InterpCurve3D(t, 0, u, InterpType()) * otm;
		u += tracking;		
		for (int i=0; i<NUMSTEPS; i++) {			
			pt0 = pt1;
			pt1 = pt2;
			if (!pathOb->CurveClosed(t,0) && u>1.0f) {
				//pt2 += pt2 - pt1;
				break;
			} else {
				u   = (float)fmod(u,1.0f);
				pt2 = pathOb->InterpCurve3D(t, 0, u, InterpType()) * otm;
				}
			v0 = Normalize(pt2-pt1);
			v1 = Normalize(pt1-pt0);			
			v0.z = v1.z = 0.0f; // remove Z component.
			cv += (v0^v1).z * bankAmount / Length(pt1-pt0);
			u  += tracking;			
			}	
		if (i) {
			if (GetFlip()) cv = -cv;
			switch (GetAxis()) {
				case 0: tm.PreRotateX(cv/float(i)); break;
				case 1: tm.PreRotateY(cv/float(i)); break;
				case 2: tm.PreRotateZ(cv/float(i)); break;
				}
			}
		}

	return tm;
	}

void PathPosition::Update(TimeValue t)
	{
	ShapeObject *pathOb = NULL;
	ivalid = FOREVER;
	
	if (path) {
		ObjectState os = path->EvalWorldState(t);
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID) {
			pathOb = (ShapeObject*)os.obj;
			if (!pathOb->NumberOfCurves()) {
				pathOb = NULL;
				}
			
			}
		ivalid &= os.Validity(t);
		}
	if (!pathOb) return;
	curval = PointOnPath(t,pathOb,ivalid);

	if (Follow()) {
		curRot = Quat(CalcRefFrame(t,pathOb));
	} else {
		curRot.Identity();
		}

	if (pathOb->CurveClosed(t,0)) {
		flags |= PATHFLAG_CLOSED;
	} else {
		flags &= ~PATHFLAG_CLOSED;
		}

	if (ivalid.Empty()) ivalid.SetInstant(t);
	}

void PathPosition::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
	{
	if (!ivalid.InInterval(t)) {
		Update(t);
		}
	valid &= ivalid;
			 
	if (method==CTRL_RELATIVE) {
  		Matrix3 *mat = (Matrix3*)val;
		if (Follow()) {
			curRot.MakeMatrix(*mat);
			}
		mat->SetTrans(curval);		
	} else {
		*((Point3*)val) = curval;
		}
	}


RefTargetHandle PathPosition::GetReference(int i)
	{
	if (i==PATHPOS_PERCENT_REF) {
		return percent;
	} else {
		return path;
		}
	}

void PathPosition::SetReference(int i, RefTargetHandle rtarg)
	{
	if (i==PATHPOS_PERCENT_REF) {
		percent = (Control*)rtarg;
	} else {
		path = (INode*)rtarg;
		}
	}

RefResult PathPosition::NotifyRefChanged(
		Interval iv, 
		RefTargetHandle hTarg, 
		PartID& partID, 
		RefMessage msg) 
	{
	switch (msg) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hPathParams && 
				GetWindowLong(hPathParams,GWL_USERDATA)==(LONG)this &&
				ip) {
				TimeChanged(ip->GetTime());
				}
			break;
		case REFMSG_TARGET_DELETED:
			if (hTarg == path) {
				path = NULL;
				if (hPathParams && GetWindowLong(hPathParams,GWL_USERDATA)==(LONG)this) {
					SetWindowText(GetDlgItem(hPathParams,IDC_PATHNAME),
						GetString(IDS_RB_NONE));
					}
			} else 
			if (hTarg == percent) {
				percent = NULL;
				} 
			break;
		}
	return REF_SUCCEED;
	}

BOOL PathPosition::AssignController(Animatable *control,int subAnim)
	{
	ReplaceReference(PATHPOS_PERCENT_REF,(RefTargetHandle)control);
	NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE,TREE_VIEW_CLASS_ID,FALSE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	return TRUE;
	}


class PickPathRestore : public RestoreObj {
	public:
		PathPosition *cont;
		PickPathRestore(PathPosition *c) {cont=c;}
		void Restore(int isUndo) {
			if (cont->editCont == cont && cont->hPathParams) {
				SetWindowText(GetDlgItem(cont->hPathParams,IDC_PATHNAME),
					GetString(IDS_RB_NONE));
				}									
			}
		void Redo() {
			if (cont->editCont == cont && cont->hPathParams && cont->path) {
				SetWindowText(GetDlgItem(cont->hPathParams,IDC_PATHNAME),
					cont->path->GetName());
				}
			}
		TSTR Description() { return TSTR(_T("Pick Path")); }
	};

BOOL PathPosition::SetPathNode(INode *node)
	{
	if (node->TestForLoop(FOREVER,this)==REF_SUCCEED) {
		theHold.Begin();
		ReplaceReference(PATHPOS_PATH_REF,(RefTargetHandle)node);
		theHold.Put(new PickPathRestore(this));
		theHold.Accept(GetString(IDS_RB_PICKPATH));
		ivalid.SetEmpty();
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		return TRUE;
	} else {
		return FALSE;
		}
	}

void PathPosition::SetTracking(float t)
	{
	tracking = t;
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

float PathPosition::GetTracking()
	{
	return tracking;
	}

void PathPosition::SetBankAmount(float a)
	{
	bankAmount = a;
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);		
	}

float PathPosition::GetBankAmount()
	{
	return bankAmount;
	}

void PathPosition::SetFollow(BOOL f)
	{
	if (f) {
		flags |= PATHFLAG_FOLLOW;
	} else {
		flags &= ~PATHFLAG_FOLLOW;
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetAllowFlip(BOOL f)
	{
	if (f) {
		flags |= PATHFLAG_ALLOWFLIP;
	} else {
		flags &= ~PATHFLAG_ALLOWFLIP;
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetConstVel(BOOL cv)
	{
	if (cv) {
		flags |= PATHFLAG_CONSTVEL;
	} else {
		flags &= ~PATHFLAG_CONSTVEL;
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetBank(BOOL b)
	{
	if (b) {
		flags |= PATHFLAG_BANK;
	} else {
		flags &= ~PATHFLAG_BANK;
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	}

void PathPosition::SetFlip(BOOL onOff)
	{
	if (onOff) flags |= PATHFLAG_FLIP;
	else flags &= ~PATHFLAG_FLIP;
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	}

void PathPosition::SetAxis(int axis)
	{
	switch (axis) {
		case 0:
			flags &= ~PATHFLAG_YAXIS;
			flags &= ~PATHFLAG_ZAXIS;
			break;
		case 1:
			flags &= ~PATHFLAG_ZAXIS;
			flags |= PATHFLAG_YAXIS;
			break;
		case 2:
			flags |= PATHFLAG_ZAXIS;
			flags &= ~PATHFLAG_YAXIS;
			break;
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

int PathPosition::GetAxis()
	{
	if (flags&PATHFLAG_YAXIS) return 1;
	if (flags&PATHFLAG_ZAXIS) return 2;
	return 0;
	}


void PathPosition::TimeChanged(TimeValue t)
	{
	if (iPerc) {
		iPerc->SetValue(100.0f*GetPercent(t,FOREVER,TRUE),FALSE);
		if (percent && percent->IsKeyAtTime(t,0)) {
			iPerc->SetKeyBrackets(TRUE);
		} else {
			iPerc->SetKeyBrackets(FALSE);
			}
		}
	}


/*--------------------------------------------------------------------*/
// PathPosition UI

HWND PathPosition::hPathParams          = NULL;
IObjParam *PathPosition::ip             = NULL;
ICustButton *PathPosition::iPick        = NULL;
PickPathMode *PathPosition::pickMode    = NULL;
ISpinnerControl *PathPosition::iBank    = NULL;
ISpinnerControl *PathPosition::iTrack   = NULL;
ISpinnerControl *PathPosition::iPerc    = NULL;
PathPosition *PathPosition::editCont    = NULL;

BOOL PickPathMode::Filter(INode *node)
	{
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID) {
			SetCursor(cont->ip->GetSysCursor(SYSCUR_SELECT));
			return TRUE;
			}
		}
	return FALSE;
	}

BOOL PickPathMode::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{
	INode *node = cont->ip->PickNode(hWnd,m);
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID) {
			SetCursor(cont->ip->GetSysCursor(SYSCUR_SELECT));
			return TRUE;
			}
		}
	return FALSE;
	}

BOOL PickPathMode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID) {
			if (cont->SetPathNode(node)) {
				SetWindowText(GetDlgItem(cont->hPathParams,IDC_PATHNAME),
					node->GetName());
				
				cont->ip->SetStdCommandMode(CID_OBJMOVE);
				cont->ip->RedrawViews(cont->ip->GetTime());						
			} else {
				TSTR buf1 = GetString(IDS_RB_PATHCONTROLLER);
				TSTR buf2 = GetString(IDS_RB_ILLEGALPATH);
				MessageBox(ip->GetMAXHWnd(),buf2,buf1,MB_OK|MB_ICONEXCLAMATION);
				}
			}
		}
	return TRUE;
	}

void PickPathMode::EnterMode(IObjParam *ip)
	{cont->iPick->SetCheck(TRUE);}

void PickPathMode::ExitMode(IObjParam *ip)
	{cont->iPick->SetCheck(FALSE);}

static void SetControlStates(HWND hDlg)
	{
	BOOL follow = IsDlgButtonChecked(hDlg,IDC_PATH_FOLLOW);
	BOOL bank   = IsDlgButtonChecked(hDlg,IDC_PATH_BANK);
	
	if (follow) {
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_BANK),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_ALLOWFLIP),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_X),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_Y),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_Z),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_FLIP),TRUE);
	} else {
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_BANK),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_ALLOWFLIP),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_X),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_Y),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_Z),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_PATH_FLIP),FALSE);
		}

	if (bank && follow) {
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_BANKLABEL),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_BANKSPIN),TRUE);		
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_TRACKLABEL),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_TRACKSPIN),TRUE);
	} else {
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_BANKLABEL),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_BANKSPIN),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_TRACKLABEL),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_FOLLOW_TRACKSPIN),FALSE);
		}
	}

BOOL CALLBACK PathParamDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	PathPosition *co = (PathPosition*)GetWindowLong(hDlg,GWL_USERDATA);
	if ( !co && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) {             
		case WM_INITDIALOG:
			co = (PathPosition*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)co );
			if (co->path) {
				SetWindowText(GetDlgItem(hDlg,IDC_PATHNAME),
					co->path->GetName());
			} else {
				SetWindowText(GetDlgItem(hDlg,IDC_PATHNAME),
					GetString(IDS_RB_NONE));
				}
			co->iPick = GetICustButton(GetDlgItem(hDlg,IDC_PICKPATH));
			co->iPick->SetType(CBT_CHECK);
			co->iPick->SetHighlightColor(GREEN_WASH);

			co->iBank = GetISpinner(GetDlgItem(hDlg,IDC_FOLLOW_BANKSPIN));			
			co->iBank->SetLimits(-999999,999999,FALSE);			
			co->iBank->SetScale(0.01f);
			co->iBank->LinkToEdit(GetDlgItem(hDlg,IDC_FOLLOW_BANK),EDITTYPE_FLOAT);
			co->iBank->SetValue(ToBankUI(co->bankAmount),FALSE);

			co->iTrack = GetISpinner(GetDlgItem(hDlg,IDC_FOLLOW_TRACKSPIN));			
			co->iTrack->SetLimits(0.01f,10.0f,FALSE);			
			co->iTrack->SetScale(0.01f);
			co->iTrack->LinkToEdit(GetDlgItem(hDlg,IDC_FOLLOW_TRACK),EDITTYPE_FLOAT);
			co->iTrack->SetValue(ToTrackUI(co->tracking),FALSE);
			
			co->iPerc = GetISpinner(GetDlgItem(hDlg,IDC_PATHPERCENTSPIN));
			co->iPerc->SetLimits(float(-999999999),float(999999999),FALSE);
			co->iPerc->SetScale(0.1f);
			co->iPerc->LinkToEdit(GetDlgItem(hDlg,IDC_PATHPERCENT),EDITTYPE_FLOAT);
			co->iPerc->SetValue(100.0f*co->GetPercent(co->ip->GetTime(),FOREVER,TRUE),FALSE);

			if (co->Follow()) {
				CheckDlgButton(hDlg,IDC_PATH_FOLLOW,TRUE);
			} else {
				CheckDlgButton(hDlg,IDC_PATH_FOLLOW,FALSE);
				}

			if (co->Bank()) {
				CheckDlgButton(hDlg,IDC_PATH_BANK,TRUE);
			} else {
				CheckDlgButton(hDlg,IDC_PATH_BANK,FALSE);
				}

			if (co->AllowFlip()) {
				CheckDlgButton(hDlg,IDC_PATH_ALLOWFLIP,TRUE);
			} else {
				CheckDlgButton(hDlg,IDC_PATH_ALLOWFLIP,FALSE);
				}

			CheckDlgButton(hDlg,IDC_PATH_CONSTVEL,co->ConstVel());
			
			CheckDlgButton(hDlg,IDC_PATH_FLIP,co->GetFlip());
			CheckDlgButton(hDlg,IDC_PATH_X,co->GetAxis()==0);
			CheckDlgButton(hDlg,IDC_PATH_Y,co->GetAxis()==1);
			CheckDlgButton(hDlg,IDC_PATH_Z,co->GetAxis()==2);
			
			SetControlStates(hDlg);
			return TRUE;

		case WM_LBUTTONDOWN:case WM_LBUTTONUP:	case WM_MOUSEMOVE:
			co->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;

		case CC_SPINNER_BUTTONDOWN:
			if (LOWORD(wParam)==IDC_PATHPERCENTSPIN) {
				theHold.Begin();
				}
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP:
			if (LOWORD(wParam)==IDC_PATHPERCENTSPIN) {
				if (HIWORD(wParam)) {
					theHold.Accept(GetString(IDS_RB_SETPERCENT));
				} else {
					theHold.Cancel();
					}
				}
			break;

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_PATHPERCENTSPIN: {
					float v = co->iPerc->GetFVal()/100.0f;
					co->percent->SetValue(
						co->ip->GetTime(),&v);
					co->ip->RedrawViews(co->ip->GetTime());
					break;
					}

				case IDC_FOLLOW_BANKSPIN:
					co->SetBankAmount(FromBankUI(co->iBank->GetFVal()));
					co->ip->RedrawViews(co->ip->GetTime());
					break;

				case IDC_FOLLOW_TRACKSPIN:
					co->SetTracking(FromTrackUI(co->iTrack->GetFVal()));
					co->ip->RedrawViews(co->ip->GetTime());
					break;
				}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_PICKPATH:
					co->ip->SetPickMode(co->pickMode);
					break;
				
				case IDC_PATH_X:
					co->SetAxis(0);
					co->ip->RedrawViews(co->ip->GetTime());
					break;
				case IDC_PATH_Y:
					co->SetAxis(1);
					co->ip->RedrawViews(co->ip->GetTime());
					break;
				case IDC_PATH_Z:
					co->SetAxis(2);
					co->ip->RedrawViews(co->ip->GetTime());
					break;

				case IDC_PATH_FLIP:
					co->SetFlip(IsDlgButtonChecked(hDlg,IDC_PATH_FLIP));
					co->ip->RedrawViews(co->ip->GetTime());
					break;
				
				case IDC_PATH_CONSTVEL:
					co->SetConstVel(IsDlgButtonChecked(hDlg,IDC_PATH_CONSTVEL));
					co->ip->RedrawViews(co->ip->GetTime());
					break;

				case IDC_PATH_FOLLOW:
					co->SetFollow(IsDlgButtonChecked(hDlg,IDC_PATH_FOLLOW));					
					SetControlStates(hDlg);
					co->ip->RedrawViews(co->ip->GetTime());
					break;

				case IDC_PATH_ALLOWFLIP:
					co->SetAllowFlip(IsDlgButtonChecked(hDlg,IDC_PATH_ALLOWFLIP));	
					SetControlStates(hDlg);
					co->ip->RedrawViews(co->ip->GetTime());
					break;

				case IDC_PATH_BANK:
					co->SetBank(IsDlgButtonChecked(hDlg,IDC_PATH_BANK));
					SetControlStates(hDlg);
					co->ip->RedrawViews(co->ip->GetTime());
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void PathPosition::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{	
	this->ip = ip;
	editCont = this;
	if (flags&BEGIN_EDIT_HIERARCHY) {
		
		// No IK if follow is on
		if (Follow()) return;

		JointParamsPath *jp = (JointParamsPath*)GetProperty(PROPID_JOINTPARAMS);
		InterpCtrlUI *ui;	

		if (!jp) {
			jp = new JointParamsPath();
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
			if (!jp || !jp->RollupOpen()) f = APPENDROLL_CLOSED;	

			ui->hParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PATHJOINTPARAMS),
				JointParamDlgProc,
				GetString(IDS_RB_PATHJOINTPARAMS), 
				(LPARAM)new JointDlgData(ui,jp),f);	
			}
	
		SetProperty(PROPID_INTERPUI,ui);		
	} else {	
		pickMode    = new PickPathMode(this);
		hPathParams = ip->AddRollupPage( 
			hInstance, 
			MAKEINTRESOURCE(IDD_PATHPARAMS),
			PathParamDialogProc,
			GetString(IDS_RB_PATHPARAMS), 
			(LPARAM)this );		
		ip->RegisterDlgWnd(hPathParams);		
		ip->RegisterTimeChangeCallback(this);
		}
	SetWindowLong( hPathParams, GWL_USERDATA, (LONG)this );		
	}

void PathPosition::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	editCont = NULL;
	if (next && next->ClassID()==ClassID() && !hPathParams) return;

	if (hPathParams) {		
		ip->UnRegisterTimeChangeCallback(this);
		ip->ClearPickMode();
		delete pickMode;
		pickMode = NULL;
		ip->UnRegisterDlgWnd(hPathParams);
		ip->DeleteRollupPage(hPathParams);
		hPathParams = NULL;
	} else {
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

int PathPosition::SetProperty(ULONG id, void *data)
	{
	if (id==PROPID_JOINTPARAMS) {		
		if (!data) {
			int index = aprops.FindProperty(id);
			if (index>=0) {
				aprops.Delete(index,1);
				}
		} else {
			JointParamsPath *jp = (JointParamsPath*)GetProperty(id);
			if (jp) {
				*jp = *((JointParamsPath*)data);
				delete (JointParamsPath*)data;
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

void* PathPosition::GetProperty(ULONG id)
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


#define JOINTPARAMPATH_CHUNK	0x1001
#define FOLLOW_CHUNK			0x1002
#define BANK_CHUNK				0x1003
#define BANKAMOUNT_CHUNK		0x1004
#define	TRACKING_CHUNK			0x1005
#define ALLOWFLIP_CHUNK			0x1006
#define CONSTVEL_CHUNK			0x1007
#define AXIS_CHUNK				0x1008
#define FLIP_CHUNK				0x1009

IOResult PathPosition::Save(ISave *isave)
	{	
	JointParamsPath *jp = (JointParamsPath*)GetProperty(PROPID_JOINTPARAMS);
	ULONG nb;

	if (jp) {
		isave->BeginChunk(JOINTPARAMPATH_CHUNK);
		jp->Save(isave);
		isave->EndChunk();
		}
	
	if (Follow()) {
		isave->BeginChunk(FOLLOW_CHUNK);		
		isave->EndChunk();
		}
	if (Bank()) {
		isave->BeginChunk(BANK_CHUNK);		
		isave->EndChunk();
		}
	if (AllowFlip()) {
		isave->BeginChunk(ALLOWFLIP_CHUNK);		
		isave->EndChunk();
		}
	if (ConstVel()) {
		isave->BeginChunk(CONSTVEL_CHUNK);		
		isave->EndChunk();
		}
	if (GetFlip()) {
		isave->BeginChunk(FLIP_CHUNK);		
		isave->EndChunk();
		}

	int axis = GetAxis();
	isave->BeginChunk(AXIS_CHUNK);		
	isave->Write(&axis,sizeof(axis),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(BANKAMOUNT_CHUNK);		
	isave->Write(&bankAmount,sizeof(bankAmount),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(TRACKING_CHUNK);		
	isave->Write(&tracking,sizeof(tracking),&nb);
	isave->EndChunk();
	
	return IO_OK;
	}

IOResult PathPosition::Load(ILoad *iload)
	{
	ULONG nb;
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case FLIP_CHUNK:
				flags |= PATHFLAG_FLIP;
				break;

			case AXIS_CHUNK: {
				int axis;
				res=iload->Read(&axis,sizeof(axis),&nb);
				SetAxis(axis);
				break;
				}				

			case CONSTVEL_CHUNK:
				flags |= PATHFLAG_CONSTVEL;
				break;

			case FOLLOW_CHUNK:
				flags |= PATHFLAG_FOLLOW;
				break;

			case BANK_CHUNK:
				flags |= PATHFLAG_BANK;
				break;

			case ALLOWFLIP_CHUNK:
				flags |= PATHFLAG_ALLOWFLIP;
				break;

			case BANKAMOUNT_CHUNK:
				res=iload->Read(&bankAmount,sizeof(bankAmount),&nb);
				break;

			case TRACKING_CHUNK:
				res=iload->Read(&tracking,sizeof(tracking),&nb);
				break;

			case JOINTPARAMPATH_CHUNK: {
				JointParamsPath *jp = new JointParamsPath;
				jp->Load(iload);
				SetProperty(PROPID_JOINTPARAMS,jp);
				break;
				}
			}		
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}
	return IO_OK;
	}

//-------------------------------------------------------------
//
// IK
//


void PathPosition::EnumIKParams(IKEnumCallback &callback)
	{
	JointParamsPath *jp = (JointParamsPath*)GetProperty(PROPID_JOINTPARAMS);
	if (jp && jp->Active(0) && !Follow()) {
		callback.proc(this,0);
		}
	}

BOOL PathPosition::CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags)
	{
	JointParamsPath *jp = (JointParamsPath*)GetProperty(PROPID_JOINTPARAMS);
	if (!jp || !jp->Active(0) || Follow()) return FALSE;
	Point3 zero(0,0,0);
	if (path) {
		ObjectState os = path->EvalWorldState(t);
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID) {
			ShapeObject *pathOb = (ShapeObject*)os.obj;
			if (pathOb->NumberOfCurves()) {
				Point3 p0,p1,d;
				float per,dt=0.01f;
				percent->GetValue(t,&per,ivalid,CTRL_ABSOLUTE);
				per = (float)fmod(per,1.0f);
				if (per<0.0f) per = 1.0f + per;
				if (dt+per>1.0f) dt = -dt;
				p0 = pathOb->InterpCurve3D(t,0,per, InterpType());
				p1 = pathOb->InterpCurve3D(t,0,per+dt, InterpType());
				if (os.GetTM()) {
					p0 = p0 * (*os.GetTM());
					p1 = p1 * (*os.GetTM());
					}
				d = (p1-p0)/dt;
				for (int j=0; j<derivs.NumEndEffectors(); j++) {
					if (flags&POSITION_DERIV) {
						derivs.DP(d,j);
						}
					if (flags&ROTATION_DERIV) {
						derivs.DR(zero,j);
						}
					}
				
				derivs.NextDOF();
				ptm.SetTrans(p0);
				return TRUE;
				}
			}	
		}
	
	for (int j=0; j<derivs.NumEndEffectors(); j++) {
		if (flags&POSITION_DERIV) {
			derivs.DP(zero,j);
			}
		if (flags&ROTATION_DERIV) {
			derivs.DR(zero,j);
			}
		}
	derivs.NextDOF();
	return FALSE;
	}

float PathPosition::IncIKParam(TimeValue t,int index,float delta)
	{
	JointParamsPath *jp = (JointParamsPath*)GetProperty(PROPID_JOINTPARAMS);
	float v=0.0f;
	BOOL gotV=FALSE;
	if (fabs(delta) > 0.01f) {
		if (delta<0) delta = -0.01f;
		else delta = 0.01f;
		}
	if (jp) {		
		if (jp->Limited(0)) {			
			Interval valid;
			percent->GetValue(t,&v,valid,CTRL_ABSOLUTE);
			gotV = TRUE;
			}
		delta = jp->ConstrainInc(0,v,delta);
		}	
	// If the path is not closed, do not let it go off the end.
	if (!(flags&PATHFLAG_CLOSED)) {
		if (!gotV) {
			Interval valid;
			percent->GetValue(t,&v,valid,CTRL_ABSOLUTE);
			}
		if (v + delta < 0.0f) delta = -v;
		if (v + delta > 1.0f) delta = 1.0f-v;
		}
	percent->SetValue(t,&delta,FALSE,CTRL_RELATIVE);
	return delta;
	}

void PathPosition::ClearIKParam(Interval iv,int index) 
	{
	if (percent) percent->DeleteTime(iv,TIME_INCRIGHT|TIME_NOSLIDE);	
	}

void JointParamsPath::SpinnerChange(
		InterpCtrlUI *ui,WORD id,ISpinnerControl *spin,BOOL interactive)
	{
	float val;
	BOOL set = FALSE;

	switch (id) {
		case IDC_XFROMSPIN:
			val = min[0] = spin->GetFVal()/scale; 
			set = TRUE;
			break;
		case IDC_XTOSPIN:
			val = max[0] = spin->GetFVal()/scale;
			set = TRUE;
			break;
		
		case IDC_XDAMPINGSPIN:
			damping[0] = spin->GetFVal(); break;
		}
	
	if (set && interactive) {
		PathPosition *c = (PathPosition*)ui->cont;
 		c->percent->SetValue(ui->ip->GetTime(),&val,TRUE,CTRL_ABSOLUTE);
		ui->ip->RedrawViews(ui->ip->GetTime(),REDRAW_INTERACTIVE);
		}
	}

class PathClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new PathPosition(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_RB_PATH); }
	SClass_ID		SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	Class_ID		ClassID() { return PCONTROL_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	};
static PathClassDesc pathCD;

// The following five functions are used by every plug-in DLL.
/*===========================================================================*\
 | The DLL and Library Functions
\*===========================================================================*/
// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.
int controlsInit = FALSE;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

	if (! controlsInit) {
		controlsInit = TRUE;
		
		// Initialize MAX's custom controls
		InitCustomControls(hInstance);
		
		// Initialize Win95 controls
		InitCommonControls();
	}
	
	return(TRUE);
	}

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int LibNumberClasses() {return 1;}

// This function return the ith class descriptor
__declspec( dllexport ) ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case 0: &pathCD;
		default: return 0;
		}
	}

// This function returns a string that describes the DLL.  This string appears in 
// the File / Summary Info / Plug-In Info dialog box.
__declspec( dllexport ) const TCHAR *LibDescription() { 
	return GetString(IDS_LIB_DESC);
	}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }



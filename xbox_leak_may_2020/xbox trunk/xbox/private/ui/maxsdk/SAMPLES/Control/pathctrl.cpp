/**********************************************************************
 *<
	FILE: pathctrl.cpp

	DESCRIPTION: A controller that moves an object along a spline path
				 Ed. 2 re-coded using ParamBlock2/ParamMap2

	CREATED BY: Rolf Berteig
	            Ed. 2 John Wainwright

	HISTORY: created 13 June 1995
	         Ed. 2 re-coded 9/22/98

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "ctrl.h"
#include "units.h"
#include "interpik.h"
#include "spline3d.h"
#include "istdplug.h"
#include "iparamm2.h"

#define PATH_CONTROL_CNAME		GetString(IDS_RB_PATH)

#define InterpType() ((ConstVel())?SPLINE_INTERP_NORMALIZED:SPLINE_INTERP_SIMPLE)

// flag values
#define PATHFLAG_FOLLOW		(1<<0)
#define PATHFLAG_BANK		(1<<1)
#define PATHFLAG_CLOSED		(1<<2)
#define PATHFLAG_ALLOWFLIP	(1<<3)
#define PATHFLAG_CONSTVEL	(1<<4)
#define PATHFLAG_FLIP		(1<<5)
#define PATHFLAG_YAXIS		(1<<6)
#define PATHFLAG_ZAXIS		(1<<7)

class PathPosition : public	IPathPosition {
	public:
		IParamBlock2* pblock;
		INode *path;
		DWORD flags;
		float bankAmount, tracking;
		Point3 curval;
		Quat curRot;
		Control* old_percent;  // holds pre-PB2 percent controller during loading
		Interval ivalid;
		
		static PathPosition *editCont;
		static IObjParam *ip;

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
		Class_ID ClassID() { return Class_ID(PATH_CONTROL_CLASS_ID,0); }  
		SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }  		
		
		void GetClassName(TSTR& s) {s = PATH_CONTROL_CNAME;}
		void DeleteThis() {delete this;}		
		int IsKeyable() {return 0;}		

		int NumSubs()  {return 1;}
		Animatable* SubAnim(int i) {return pblock;}
		TSTR SubAnimName(int i) { return GetString(IDS_RB_PATHPARAMS); }
		int SubNumToRefNum(int subNum) {if (subNum==0) return PATHPOS_PBLOCK_REF; else return -1;}

		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev ); 
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next ); 

		int SetProperty(ULONG id, void *data);
		void *GetProperty(ULONG id);

// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		// Reference methods
		int NumRefs() { return 3; };	
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);

		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		void RescaleWorldUnits(float f) {}

		// Control methods
		void Copy(Control *from);
		RefTargetHandle Clone(RemapDir& remap);
		BOOL IsLeaf() {return FALSE;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);	
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method) {}
		void CommitValue(TimeValue t) { }
		void RestoreValue(TimeValue t) { }
		void EnumIKParams(IKEnumCallback &callback);
		BOOL CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags);
		float IncIKParam(TimeValue t,int index,float delta);
		void ClearIKParam(Interval iv,int index);
	};

class JointParamsPath : public JointParams {
	public:			 	
		JointParamsPath() : JointParams((DWORD)JNT_POS,1,100.0f) {}
		void SpinnerChange(InterpCtrlUI *ui,WORD id,ISpinnerControl *spin,BOOL interactive);
	};

IObjParam *PathPosition::ip             = NULL;
PathPosition *PathPosition::editCont    = NULL;

//********************************************************
// PATH CONTROL
//********************************************************
static Class_ID pathControlClassID(PATH_CONTROL_CLASS_ID,0); 
class PathClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new PathPosition(loading); }
	const TCHAR *	ClassName() { return PATH_CONTROL_CNAME; }
	SClass_ID		SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	Class_ID		ClassID() { return pathControlClassID; }
	const TCHAR* 	Category() { return _T("");  }
// JBW: new descriptor data accessors added.
	const TCHAR*	InternalName() { return _T("Path"); }			// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
	};

static PathClassDesc pathCD;
ClassDesc* GetPathCtrlDesc() {return &pathCD;}

// ParamBlock IDs
// main block
enum { path_params, path_joint_params };
// path_params param IDs
enum { path_percent, path_path, path_follow, path_bank, path_bank_amount, path_smoothness, 
       path_allow_upsidedown, path_constant_vel, path_axis, path_axis_flip, };

// parameter setter callback, reflect any ParamBlock-mediated param setting in instance data members.
// JBW: since the old path controller kept all parameters as instance data members, this setter callback
// is implemented to to reduce changes to existing code 
class PathPBAccessor : public PBAccessor
{ 
public:
	void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)    // set from v
	{
		PathPosition* p = (PathPosition*)owner;
		switch (id)
		{
			case path_path:
			{
				IParamMap2* pmap = p->pblock->GetMap();
				if (pmap)
					if (v.r == NULL)
						SetWindowText(GetDlgItem(pmap->GetHWnd(), IDC_PATHNAME), GetString(IDS_RB_NONE)); 
					else
						SetWindowText(GetDlgItem(pmap->GetHWnd(), IDC_PATHNAME), ((INode*)(v.r))->GetName()); 
				break;
			}
			case path_follow:
				if (v.i) p->flags |= PATHFLAG_FOLLOW; else p->flags &= ~PATHFLAG_FOLLOW; break;
			case path_bank:
				if (v.i) p->flags |= PATHFLAG_BANK; else p->flags &= ~PATHFLAG_BANK; break;
			case path_bank_amount:
				p->bankAmount = FromBankUI(v.f); break;
			case path_smoothness:
				p->tracking = FromTrackUI(v.f); break;
			case path_allow_upsidedown:
				if (v.i) p->flags |= PATHFLAG_ALLOWFLIP; else p->flags &= ~PATHFLAG_ALLOWFLIP; break;
			case path_constant_vel:
				if (v.i) p->flags |= PATHFLAG_CONSTVEL; else p->flags &= ~PATHFLAG_CONSTVEL; break;
			case path_axis:
				switch (v.i)
				{
					case 0: p->flags &= ~(PATHFLAG_YAXIS | PATHFLAG_ZAXIS); break;			// X
					case 1: p->flags |= PATHFLAG_YAXIS; p->flags &= ~PATHFLAG_ZAXIS; break;	// Y
					case 2: p->flags |= PATHFLAG_ZAXIS; p->flags &= ~PATHFLAG_YAXIS; break;	// Y
				}
				break;
			case path_axis_flip:
				if (v.i) p->flags |= PATHFLAG_FLIP; else p->flags &= ~PATHFLAG_FLIP; break;
		}
	}
};

static PathPBAccessor path_accessor;

class PathDlgProc : public ParamMap2UserDlgProc 
{
	public:
		void UpdatePathName(PathPosition* p)
		{
			IParamMap2* pmap = p->pblock->GetMap();
			if (pmap)
			{
				HWND hWnd = pmap->GetHWnd();
				if (p->path == NULL)
					SetWindowText(GetDlgItem(hWnd, IDC_PATHNAME), GetString(IDS_RB_NONE)); 
				else
					SetWindowText(GetDlgItem(hWnd, IDC_PATHNAME), p->path->GetName()); 
			}
		}

		BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch (msg) 
			{
				case WM_INITDIALOG:
					PathPosition* p = (PathPosition*)map->GetParamBlock()->GetOwner();
					UpdatePathName(p);
					return TRUE;
			}
			return FALSE;
		}

		void SetParamBlock(IParamBlock2* pb) 
		{ 
			UpdatePathName((PathPosition*)pb->GetOwner());
		}

		void DeleteThis() { }
};

static PathDlgProc pathDlgProc;

// per instance path controller block
static ParamBlockDesc2 path_paramblk (path_params, _T("PathParameters"),  0, &pathCD, P_AUTO_CONSTRUCT + P_AUTO_UI, PATHPOS_PBLOCK_REF, 
	//rollout
	IDD_PATHPARAMS, IDS_RB_PATHPARAMS, BEGIN_EDIT_MOTION, 0, &pathDlgProc,
	// params
	path_percent,	_T("percent"),		TYPE_PCNT_FRAC, P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_PERCENT, 
		p_default, 		0.0,	
		p_range, 		float(-999999999), float(999999999), 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_PATHPERCENT, IDC_PATHPERCENTSPIN, 0.1f, 
		end, 
	path_path, 		_T("path"), 		TYPE_INODE, 	P_OWNERS_REF + P_NO_AUTO_LABELS,	IDS_RB_PATH,
		p_ui, 			TYPE_PICKNODEBUTTON, IDC_PICKPATH, 
		p_sclassID,		SHAPE_CLASS_ID, 
		p_refno,		PATHPOS_PATH_REF,
		p_accessor,		&path_accessor,
		end, 
	path_follow,	_T("follow"),		TYPE_BOOL, 		0,									IDS_JW_FOLLOW,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_PATH_FOLLOW, 
		p_enable_ctrls,	4, path_bank, path_allow_upsidedown, path_axis, path_axis_flip,
		p_accessor,		&path_accessor,
		end, 
	path_bank,		_T("bank"),			TYPE_BOOL, 		0,									IDS_JW_BANK,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_PATH_BANK, 
		p_enabled,		FALSE,
		p_enable_ctrls,	2, path_bank_amount, path_smoothness,
		p_accessor,		&path_accessor,
		end, 
	path_bank_amount, _T("bankAmount"), TYPE_FLOAT, 	P_ANIMATABLE, 						IDS_JW_BANKAMOUNT, 
		p_default, 		0.5, 
		p_range, 		-999999.0, 999999.0, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FOLLOW_BANK, IDC_FOLLOW_BANKSPIN, 0.01f,
		p_accessor,		&path_accessor,
		p_enabled,		FALSE,
		end, 
	path_smoothness,  _T("smoothness"), TYPE_FLOAT, 	P_ANIMATABLE, 						IDS_JW_SMOOTHNESS, 
		p_default, 		0.5, 
		p_range, 		0.01f, 10.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FOLLOW_TRACK, IDC_FOLLOW_TRACKSPIN, 0.01f,
		p_enabled,		FALSE,
		p_accessor,		&path_accessor,
		end, 
	path_allow_upsidedown, _T("allowUpsideDown"), TYPE_BOOL, 0,								IDS_JW_ALLOWUPSIDEDOWN,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_PATH_ALLOWFLIP, 
		p_enabled,		FALSE,
		p_accessor,		&path_accessor,
		end, 
	path_constant_vel, _T("constantVel"), TYPE_BOOL,	0,									IDS_JW_CONSTANTVEL,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_PATH_CONSTVEL, 
		p_accessor,		&path_accessor,
		end, 
	path_axis, 		_T("axis"),			TYPE_INT, 		0,									IDS_JW_AXIS,
		p_default, 		0, 
		p_range, 		0, 2, 
		p_ui, 			TYPE_RADIO, 	3, IDC_PATH_X, IDC_PATH_Y, IDC_PATH_Z, 
		p_enabled,		FALSE,
		p_accessor,		&path_accessor,
		end, 
	path_axis_flip, _T("axisFlip"),		TYPE_BOOL,		0,									IDS_JW_AXISFLIP,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_PATH_FLIP, 
		p_enabled,		FALSE,
		p_accessor,		&path_accessor,
		end, 
	end
	);

PathPosition::PathPosition(BOOL loading) 
	{
	path     = NULL;
	old_percent = NULL;
	curval   = Point3(0,0,0);
	curRot.Identity();
	flags      = 0;
	bankAmount = FromBankUI(0.5f);
	tracking   = FromTrackUI(0.5f);

	// make the paramblock
	pathCD.MakeAutoParamBlocks(this);
	// pre-animate percent param
	SuspendAnimate();
	AnimateOn();
	float val =  1.0f;
	pblock->SetValue(path_percent, GetAnimEnd(), val); 
	ResumeAnimate();
	pblock->CallSets();
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

    p->ReplaceReference(PATHPOS_PBLOCK_REF, pblock->Clone(remap));

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
		ReplaceReference(PATHPOS_PATH_REF,ctrl->path);
		// a copy will construct its own pblock to keep the pblock-to-owner 1-to-1.
		RemapDir *remap = NewRemapDir(); 
		ReplaceReference(PATHPOS_PBLOCK_REF, ctrl->pblock->Clone(*remap));
		remap->DeleteThis();
		curval   = ctrl->curval;
		curRot   = ctrl->curRot;
		flags    = ctrl->flags;
		bankAmount = ctrl->bankAmount;
		tracking = ctrl->tracking;
//		IParamMap2* pmap = ctrl->pblock->GetMap();
//		if (pmap != NULL)
//			pmap->SetParamBlock(pblock);
	} else {
		from->GetValue(0,&curval,ivalid);
		}
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

float PathPosition::GetPercent(TimeValue t,Interval &valid,BOOL noClip)
	{	
	float per;
	pblock->GetValue(path_percent, t, per, valid);	
	if (noClip) return per;
	if (per < 0.0f || per > 1.0f || (flags & PATHFLAG_CLOSED)) {
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
	bankAmount = FromBankUI(pblock->GetFloat(path_bank_amount, t));
	tracking = FromTrackUI(pblock->GetFloat(path_smoothness, t));
	
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
		if (os.obj->IsShapeObject()) {
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
		switch (i)
		{
			case 0:
				return old_percent;   // ref 0 is now obsolete, percent stored in ParamBlock.
			case PATHPOS_PATH_REF:
				return path;
			case PATHPOS_PBLOCK_REF:
				return pblock;
		}
		return NULL;
	}

void PathPosition::SetReference(int i, RefTargetHandle rtarg)
	{
		switch (i)
		{
			case 0:
				old_percent = (Control*)rtarg;  // reference set by pre-PB2 versions loading, fix up in PLCB
				break;
			case PATHPOS_PATH_REF:
				path = (INode*)rtarg; break;
			case PATHPOS_PBLOCK_REF:
				pblock = (IParamBlock2*)rtarg; break;
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
			path_paramblk.InvalidateUI();
			break;
		case REFMSG_TARGET_DELETED:
			if (hTarg == path) 
			{
				path = NULL;
				pblock->RefDeleted(path_path);
			}
			break;
		}
	return REF_SUCCEED;
	}

class PickPathRestore : public RestoreObj {
	public:
		PathPosition *cont;
		PickPathRestore(PathPosition *c) {cont=c;}
		void Restore(int isUndo) {
			if (cont->editCont == cont) {
				path_paramblk.InvalidateUI();
				}									
			}
		void Redo() {
			if (cont->editCont == cont && cont->path) {
				path_paramblk.InvalidateUI();
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
	float ut = ToTrackUI(t);
	pblock->SetValue(path_smoothness, GetCOREInterface()->GetTime(), ut);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

float PathPosition::GetTracking()
	{
	return FromTrackUI(pblock->GetFloat(path_smoothness, GetCOREInterface()->GetTime()));
	}

void PathPosition::SetBankAmount(float a)
	{
	float ua = ToBankUI(a);
	pblock->SetValue(path_bank_amount, GetCOREInterface()->GetTime(), ua);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);		
	}

float PathPosition::GetBankAmount()
	{
	return FromBankUI(pblock->GetFloat(path_bank_amount, GetCOREInterface()->GetTime()));
	}

void PathPosition::SetFollow(BOOL f)
	{
	pblock->SetValue(path_follow, GetCOREInterface()->GetTime(), f);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetAllowFlip(BOOL f)
	{
	pblock->SetValue(path_allow_upsidedown, GetCOREInterface()->GetTime(), f);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetConstVel(BOOL cv)
	{
	pblock->SetValue(path_constant_vel, GetCOREInterface()->GetTime(), cv);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void PathPosition::SetBank(BOOL b)
	{
	pblock->SetValue(path_bank, GetCOREInterface()->GetTime(), b);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	}

void PathPosition::SetFlip(BOOL onOff)
	{
	pblock->SetValue(path_axis_flip, GetCOREInterface()->GetTime(), onOff);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	}

void PathPosition::SetAxis(int axis)
	{
	pblock->SetValue(path_axis, GetCOREInterface()->GetTime(), axis);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

int PathPosition::GetAxis()
	{
	if (flags&PATHFLAG_YAXIS) return 1;
	if (flags&PATHFLAG_ZAXIS) return 2;
	return 0;
	}

/*--------------------------------------------------------------------*/
// PathPosition UI

void PathPosition::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{	
	this->ip = ip;
	editCont = this;
	if (flags & BEGIN_EDIT_HIERARCHY) {
// JBW: JointParam stuff not handled by ParamBlock2 yet		
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
		pathCD.BeginEditParams(ip, this, flags, prev);
		}
	}

void PathPosition::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	editCont = NULL;
	IParamMap2* pmap = pblock->GetMap();
	if (pmap != NULL)
	{
		if (next && next->ClassID() == ClassID() && ((PathPosition*)next)->pblock)
		{
			pmap->SetParamBlock(((PathPosition*)next)->pblock);
			ip->ClearPickMode();
		}
		else
			pathCD.EndEditParams(ip, this, flags | END_EDIT_REMOVEUI, next);
	}
	else
	{
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

	if (jp) {
		isave->BeginChunk(JOINTPARAMPATH_CHUNK);
		jp->Save(isave);
		isave->EndChunk();
		}
	
	return IO_OK;
	}


// provide a post-load callback so old-version path controller data can be loaded into the ParamBlock2
class PathPLCB : public PostLoadCallback 
{
public:
	PathPosition*	p;
	BOOL			old_version;
	BOOL			flip;
	int				axis;
	BOOL			constvel;
	BOOL			follow;	
	BOOL			bank;	
	BOOL			allowFlip;
	float			bankAmt;	
	float			tracking;

	PathPLCB(PathPosition* pth)
	{ 
		p = pth;
		old_version = FALSE; flip = FALSE; axis = 0; constvel = FALSE;
	    follow = FALSE; bank = FALSE; allowFlip = FALSE; bankAmt = 0.5; tracking = 0.5;
	}
	void proc(ILoad *iload)
	{
		if (old_version)
		{
			// loading an old version, set the param values
			p->pblock->SetValue(path_axis_flip, 0, flip);
			p->pblock->SetValue(path_axis, 0, axis);
			p->pblock->SetValue(path_constant_vel, 0, constvel);
			p->pblock->SetValue(path_follow, 0, follow);
			p->pblock->SetValue(path_bank, 0, bank);
			p->pblock->SetValue(path_allow_upsidedown, 0, allowFlip);
			bankAmt = ToBankUI(bankAmt);
			p->pblock->SetValue(path_bank_amount, 0, bankAmt);
			tracking = ToTrackUI(tracking);
			p->pblock->SetValue(path_smoothness, 0, tracking);
			// copy across old percent controller
			if (p->old_percent != NULL)
				p->pblock->SetController(path_percent, 0, p->old_percent, FALSE);
		}
		else
			// make all params call their PBAccessor::Set() fns to set up flags from just-loaded pblock values
			p->pblock->CallSets();
	}
};

IOResult PathPosition::Load(ILoad *iload)
	{
	ULONG nb;
	IOResult res = IO_OK;
	PathPLCB* plcb = new PathPLCB(this);
	iload->RegisterPostLoadCallback(plcb);

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case FLIP_CHUNK:
				plcb->old_version = TRUE;
				plcb->flip = TRUE;
				break;

			case AXIS_CHUNK:
				plcb->old_version = TRUE;
				res=iload->Read(&plcb->axis,sizeof(int),&nb);
				break;

			case CONSTVEL_CHUNK:
				plcb->old_version = TRUE;
				plcb->constvel = TRUE;
				break;

			case FOLLOW_CHUNK:
				plcb->old_version = TRUE;
				plcb->follow = TRUE;
				break;

			case BANK_CHUNK:
				plcb->old_version = TRUE;
				plcb->bank = TRUE;
				break;

			case ALLOWFLIP_CHUNK:
				plcb->old_version = TRUE;
				plcb->allowFlip = TRUE;
				break;

			case BANKAMOUNT_CHUNK:
				plcb->old_version = TRUE;
				res=iload->Read(&plcb->bankAmt,sizeof(bankAmount),&nb);
				break;

			case TRACKING_CHUNK:
				plcb->old_version = TRUE;
				res=iload->Read(&plcb->tracking,sizeof(tracking),&nb);
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
		if (os.obj->IsShapeObject()) {
			ShapeObject *pathOb = (ShapeObject*)os.obj;
			if (pathOb->NumberOfCurves()) {
				Point3 p0,p1,d;
				float per,dt=0.01f;
				pblock->GetValue(path_percent, t, per, ivalid);
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
			pblock->GetValue(path_percent, t, v, valid);
			gotV = TRUE;
			}
		delta = jp->ConstrainInc(0,v,delta);
		}	
	// If the path is not closed, do not let it go off the end.
	if (!(flags&PATHFLAG_CLOSED)) {
		if (!gotV) {
			Interval valid;
			pblock->GetValue(path_percent, t, v, valid);
			}
		if (v + delta < 0.0f) delta = -v;
		if (v + delta > 1.0f) delta = 1.0f-v;
		}
	pblock->GetController(path_percent)->SetValue(t,&delta,FALSE,CTRL_RELATIVE);
	return delta;
	}

void PathPosition::ClearIKParam(Interval iv,int index) 
	{
	pblock->GetController(path_percent)->DeleteTime(iv,TIME_INCRIGHT|TIME_NOSLIDE);	
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
 		c->pblock->GetController(path_percent)->SetValue(ui->ip->GetTime(),&val,TRUE,CTRL_ABSOLUTE);
		ui->ip->RedrawViews(ui->ip->GetTime(),REDRAW_INTERACTIVE);
		}
	}



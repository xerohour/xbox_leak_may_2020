/**********************************************************************
 *<
	FILE: linkctrl.cpp

	DESCRIPTION: An animated link controller

	CREATED BY: Rolf Berteig

	HISTORY: 1/25/97

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/
#include "ctrl.h"
#include "units.h"
#include "istdplug.h"

#define LINKCTRL_NAME		GetString(IDS_RB_LINKCTRL)

class LinkCtrl;

class PickLinkMode : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		LinkCtrl *cont;
		
		PickLinkMode(LinkCtrl *c) {cont=c;}
		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}
		BOOL Filter(INode *node);		
		PickNodeCallback *GetFilter() {return this;}
	};

class LinkCtrl : public ILinkCtrl {
	public:
		Control *tmControl;		// ref 0
		Tab<INode*> nodes;		// ref 1-n
		Tab<TimeValue> times;

		static HWND hWnd;
		static IObjParam *ip;
		static LinkCtrl *editCont;
		static ISpinnerControl *iTime;
		static ICustButton *iPickOb, *iDelOb;
		static PickLinkMode *pickLinkMode;
		static BOOL valid;

		LinkCtrl(BOOL loading=FALSE);

		// Animatable methods
		void DeleteThis() {delete this;}		
		Class_ID ClassID() {return LINKCTRL_CLASSID;}
		SClass_ID SuperClassID() {return CTRL_MATRIX3_CLASS_ID;}
		void GetClassName(TSTR& s) {s = LINKCTRL_NAME;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev); 
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next); 
		int NumSubs()  {return 1;}
		Animatable* SubAnim(int i) {return tmControl;}
		TSTR SubAnimName(int i) {return GetString(IDS_RB_TRANSFORM);}		

		// Reference methods
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		int NumRefs() {return 1+nodes.Count();}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		BOOL AssignController(Animatable *control,int subAnim);

		// Control methods				
		void Copy(Control *from);
		BOOL IsLeaf() {return FALSE;}
		void CommitValue(TimeValue t) {}
		void RestoreValue(TimeValue t) {}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
		void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);
		BOOL InheritsParentTransform() {return FALSE;}
		BOOL CanInstanceController() {return FALSE;}

		// Animatable methods		
		void CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags) {tmControl->CopyKeysFromTime(src,dst,flags);}
		BOOL IsKeyAtTime(TimeValue t,DWORD flags) {return tmControl->IsKeyAtTime(t,flags);}
		BOOL GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt) {return tmControl->GetNextKeyTime(t,flags,nt);}
		int GetKeyTimes(Tab<TimeValue> &times,Interval range,DWORD flags) {return tmControl->GetKeyTimes(times,range,flags);}
		int GetKeySelState(BitArray &sel,Interval range,DWORD flags) {return tmControl->GetKeySelState(sel,range,flags);}
		Control *GetPositionController() {return tmControl->GetPositionController();}
		Control *GetRotationController() {return tmControl->GetRotationController();}
		Control *GetScaleController() {return tmControl->GetScaleController();}
		BOOL SetPositionController(Control *c) {return tmControl->SetPositionController(c);}
		BOOL SetRotationController(Control *c) {return tmControl->SetRotationController(c);}
		BOOL SetScaleController(Control *c) {return tmControl->SetScaleController(c);}
		void MapKeys(TimeMap *map,DWORD flags);

		// From ILinkCtrl
		int GetParentCount() {return nodes.Count();}
		TimeValue GetLinkTime(int i) {return times[i];}
		void SetLinkTime(int i,TimeValue t) {SetTime(t,i);}
		void LinkTimeChanged() {SortNodes(-1);}

		// Local methods
		Matrix3 GetParentTM(TimeValue t,Interval *valid=NULL);
		Matrix3 CompTM(TimeValue t, int i);
		void AddNewLink(INode *node,TimeValue t);
		void DeleteLink(int i);
		void SetTime(TimeValue t,int i);
		void SortNodes(int sel);

		void SetupDialog(HWND hWnd);
		void DestroyDialog();
		void SetupList(int sel=-1);
		void ListSelChanged();
		void Invalidate();
		void Update();
	};


class LinksRestore : public RestoreObj {
	public:
		LinkCtrl *cont;
		Tab<INode*> unodes;
		Tab<INode*> rnodes;
		Tab<TimeValue> utimes;
		Tab<TimeValue> rtimes;

		LinksRestore(LinkCtrl *c) {
			cont = c;
			unodes = cont->nodes;
			utimes = cont->times;
			}
		void Restore(int isUndo) {
			if (isUndo) {
				rnodes = cont->nodes;
				rtimes = cont->times;
				}
			cont->nodes = unodes;
			cont->times = utimes;
			cont->NotifyDependents(FOREVER,0,REFMSG_CHANGE);
			cont->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
			cont->Invalidate();
			}
		void Redo() {
			cont->nodes = rnodes;
			cont->times = rtimes;
			cont->NotifyDependents(FOREVER,0,REFMSG_CHANGE);
			cont->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
			cont->Invalidate();
			}	
		void EndHold() {cont->ClearAFlag(A_HELD);}
	};



//--- Class Descriptor ----------------------------------------------

HWND			 LinkCtrl::hWnd = NULL;
IObjParam		*LinkCtrl::ip = NULL;
LinkCtrl		*LinkCtrl::editCont = NULL;
ISpinnerControl *LinkCtrl::iTime = NULL;
ICustButton		*LinkCtrl::iPickOb = NULL;
ICustButton		*LinkCtrl::iDelOb = NULL;
PickLinkMode    *LinkCtrl::pickLinkMode = NULL;
BOOL             LinkCtrl::valid = FALSE;
  
class LinkCtrlClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new LinkCtrl(loading);}
	const TCHAR *	ClassName() {return LINKCTRL_NAME;}
	SClass_ID		SuperClassID() {return CTRL_MATRIX3_CLASS_ID; }
	Class_ID		ClassID() {return LINKCTRL_CLASSID;}
	const TCHAR* 	Category() {return _T("");}
	};
static LinkCtrlClassDesc linkCtrlCD;
ClassDesc* GetLinkCtrlDesc() {return &linkCtrlCD;}

BOOL CALLBACK LinkParamDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//--- LinkCtrl methods -----------------------------------------------

LinkCtrl::LinkCtrl(BOOL loading)
	{
	tmControl = NULL;
	if (!loading) {
		Control *cont;
		ClassDesc *desc = GetDefaultController(CTRL_MATRIX3_CLASS_ID);
		if (desc && desc->ClassID()==ClassID()) {
			cont = (Control*)CreateInstance(CTRL_MATRIX3_CLASS_ID, Class_ID(PRS_CONTROL_CLASS_ID,0));			
		} else {
			cont = NewDefaultMatrix3Controller();
			}
		ReplaceReference(0,cont);
		}
	}

void LinkCtrl::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{
	if (flags&BEGIN_EDIT_MOTION) {
		this->ip = ip;
		editCont = this;	
		hWnd = ip->AddRollupPage( 
			hInstance, 
			MAKEINTRESOURCE(IDD_LINK_PARAMS),
			LinkParamDialogProc,
			GetString(IDS_RB_LINKPARAMS), 
			(LPARAM)this);	
		pickLinkMode = new PickLinkMode(this);
		tmControl->BeginEditParams(ip,flags,NULL);
		}
	}

void LinkCtrl::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
	{
	if (editCont) {
		ip->ClearPickMode();
		tmControl->EndEditParams(ip,flags,NULL);
		ip->DeleteRollupPage(hWnd);	
		hWnd     = NULL;
		ip       = NULL;
		editCont = NULL;	
		delete pickLinkMode;
		pickLinkMode = NULL;
		}
	}


RefResult LinkCtrl::NotifyRefChanged(
		Interval iv, RefTargetHandle hTarg, 
		PartID& partID, RefMessage msg)
	{
	switch (msg) {
		case REFMSG_TARGET_DELETED: {
			for (int i=nodes.Count()-1; i>=0; i--) {
				if (nodes[i]==hTarg) DeleteLink(i);
				}
			break;
			}
		}
	return REF_SUCCEED;
	}

#define LINKCOUNT_CHUNK	0x0100
#define TIMES_CHUNK		0x0110

IOResult LinkCtrl::Save(ISave *isave)
	{
	ULONG nb;
	int ct = nodes.Count();
	
	isave->BeginChunk(LINKCOUNT_CHUNK);
	isave->Write(&ct,sizeof(ct),&nb);
	isave->EndChunk();

	if (ct) {
		isave->BeginChunk(TIMES_CHUNK);
		isave->Write(times.Addr(0),sizeof(TimeValue)*ct,&nb);
		isave->EndChunk();
		}

	return IO_OK;
	}

IOResult LinkCtrl::Load(ILoad *iload)
	{
	ULONG nb;
	int ct;	
	IOResult res;
		
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case LINKCOUNT_CHUNK: {
				iload->Read(&ct,sizeof(ct),&nb);
				nodes.SetCount(ct);
				times.SetCount(ct);
				for (int i=0; i<ct; i++) nodes[i] = NULL;
				break;
				}

			case TIMES_CHUNK:
				iload->Read(times.Addr(0),sizeof(TimeValue)*ct,&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}

	return IO_OK;
	}


RefTargetHandle LinkCtrl::Clone(RemapDir &remap)
	{
	LinkCtrl *ctrl = new LinkCtrl();
	ctrl->ReplaceReference(0,remap.CloneRef(tmControl));
	ctrl->nodes.SetCount(nodes.Count());
	ctrl->times.SetCount(times.Count());
	for (int i=0; i<nodes.Count(); i++) {
		ctrl->nodes[i] = NULL;
		ctrl->ReplaceReference(i+1,nodes[i]);
		ctrl->times[i] = times[i];
		}
	return ctrl;
	}

RefTargetHandle LinkCtrl::GetReference(int i)
	{
	if (i==0) return tmControl;
	return nodes[i-1];
	}

void LinkCtrl::SetReference(int i, RefTargetHandle rtarg)
	{
	if (i==0) tmControl = (Control*)rtarg;
	else nodes[i-1] = (INode*)rtarg;
	}

BOOL LinkCtrl::AssignController(Animatable *control,int subAnim)
	{
	if (control->ClassID()==ClassID()) return FALSE;
	ReplaceReference(0,(ReferenceTarget*)control);
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	return TRUE;
	}

void LinkCtrl::Copy(Control *from)
	{
	if (from->ClassID()==ClassID()) {
		LinkCtrl *lc = (LinkCtrl*)from;
		ReplaceReference(0,(ReferenceTarget*)lc->tmControl->Clone());
	} else {
		ReplaceReference(0,(ReferenceTarget*)from->Clone());
		}
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	}

void LinkCtrl::GetValue(
		TimeValue t, void *val, Interval &valid, 
		GetSetMethod method)
	{
	Matrix3 *ptm = (Matrix3*)val;
	*ptm = GetParentTM(t,&valid);
	tmControl->GetValue(t,val,valid,method);
	}

void LinkCtrl::SetValue(
		TimeValue t, void *val, int commit, 
		GetSetMethod method)
	{
	SetXFormPacket *pckt = (SetXFormPacket*)val;
	pckt->tmParent = GetParentTM(t);
	tmControl->SetValue(t,val,commit,method);
	}


Matrix3 LinkCtrl::CompTM(TimeValue t, int i)
	{
	Matrix3 rtm(1);
	if (i) {
		// Evaluate the tm of the previous node the instant of the switch
		Matrix3 pptm = CompTM(times[i],i-1);
		 
		// Evaluate the tm of this node the instant of the switch
		Matrix3 ptm = nodes[i]->GetNodeTM(times[i]);
		
		rtm = pptm*Inverse(ptm);
		}
	return rtm*nodes[i]->GetNodeTM(t);
	}

Matrix3 LinkCtrl::GetParentTM(TimeValue t,Interval *valid)
	{
	if (!nodes.Count()) return Matrix3(1);	
	for (int i=0; i<nodes.Count(); i++) {
		if (times[i]>t) {
			if (i) i--;
			break;
			}
		}
	if (i>nodes.Count()-1) i = nodes.Count()-1;

	// Compute a matrix that adjusts the transitions between links so
	// as to maintain continuity
	Matrix3 rtm(1);
	if (i) {
		// Evaluate (recursively) the tm of the previous node the instant of the switch
		//Matrix3 pptm = nodes[i-1]->GetNodeTM(times[i]);
		Matrix3 pptm = CompTM(times[i],i-1);
			
		// Evaluate the tm of this node the instant of the switch
		Matrix3 ptm = nodes[i]->GetNodeTM(times[i]);
		
		rtm = pptm*Inverse(ptm);
		}
	
	// Validity is limited to the duration of this link
	if (valid) {
		Interval iv = FOREVER;
		if (i) iv.SetStart(times[i]);
		if (i<times.Count()-1) iv.SetEnd(times[i+1]);
		*valid &= iv;
		}

	return rtm*nodes[i]->GetNodeTM(t,valid);
	}


void LinkCtrl::AddNewLink(INode *node,TimeValue t)
	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) 
		theHold.Put(new LinksRestore(this));	
	
	if (!nodes.Count()) {		
		Matrix3 tm(1), ntm = node->GetNodeTM(t), itm(1);
		tmControl->GetValue(t,&tm,FOREVER,CTRL_RELATIVE);
		tmControl->ChangeParents(t,itm,ntm,tm);
		}
	
	for (int i=0; i<nodes.Count(); i++) {
		if (times[i]>t) break;
		}
	nodes.Insert(i,1,&node);
	times.Insert(i,1,&t);
	MakeRefByID(FOREVER,i+1,node);
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	SetupList(i);
	}

void LinkCtrl::DeleteLink(int i)
	{
	if (nodes.Count()==1) {
		TimeValue t = GetCOREInterface()->GetTime();
		Matrix3 ptm = GetParentTM(t);
		Matrix3 itm(1), tm = ptm;
		tmControl->GetValue(t,&tm,FOREVER,CTRL_RELATIVE);
		tmControl->ChangeParents(t,ptm,itm,tm);
		}

	DeleteReference(i+1);
	if (theHold.Holding() && !TestAFlag(A_HELD)) 
		theHold.Put(new LinksRestore(this));
	nodes.Delete(i,1);
	times.Delete(i,1);
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	if (hWnd && editCont==this) SetupList();
	}

void LinkCtrl::SetTime(TimeValue t,int i)
	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {
		theHold.Put(new LinksRestore(this));
		SetAFlag(A_HELD);
		}
	times[i] = t;
	SortNodes(i);
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	}

void LinkCtrl::MapKeys(TimeMap *map,DWORD flags)
	{	
	if (theHold.Holding() && !TestAFlag(A_HELD)) {
		theHold.Put(new LinksRestore(this));
		SetAFlag(A_HELD);
		}

	for (int i=0; i<times.Count(); i++) {
		times[i] = map->map(times[i]);
		}
	
	if (hWnd && editCont==this) SetupList();
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	}

static LinkCtrl *theSortCtrl = NULL;
static int __cdecl CompareNodes(const void *a, const void *b)
	{
	int *aa = (int*)a;
	int *bb = (int*)b;
	assert(theSortCtrl);
	return theSortCtrl->times[*aa] - theSortCtrl->times[*bb];
	}

void LinkCtrl::SortNodes(int sel)
	{
	// Setup a map for a shell sort
	Tab<int> map;
	map.SetCount(nodes.Count());	
	for (int i=0; i<map.Count(); i++) map[i] = i;

	// Sort
	theSortCtrl = this;
	map.Sort(CompareNodes);

	// Put everything in the right place
	Tab<INode*> onodes = nodes;
	Tab<TimeValue> otimes = times;
	for (i=0; i<nodes.Count(); i++) {
		nodes[i] = onodes[map[i]];
		times[i] = otimes[map[i]];
		}

	// Redo the list selecting the node in its new place
	if (sel>=0) {
		for (i=0; i<map.Count(); i++) {
			if (map[i]==sel) {
				SetupList(i);
				break;
				}
			}
		}
	}


//--- UI ------------------------------------------------------------


static BOOL CALLBACK LinkParamDialogProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	LinkCtrl *cont = (LinkCtrl*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_INITDIALOG:
			cont = (LinkCtrl*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			cont->SetupDialog(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_LINK_PICK:
					cont->ip->SetPickMode(cont->pickLinkMode);
					break;

				case IDC_LINK_DELETE: {
					int sel = SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_GETCURSEL,0,0);
					if (sel!=LB_ERR) {
						theHold.Begin();
						cont->DeleteLink(sel);
						theHold.Accept(GetString(IDS_RB_DELETELINK));
						cont->ip->RedrawViews(cont->ip->GetTime());
						}
					break;
					}

				case IDC_LINK_LIST:
					if (HIWORD(wParam)==LBN_SELCHANGE) {
						cont->ListSelChanged();
						}
					break;
				}
			break;

		case CC_SPINNER_BUTTONDOWN:
			theHold.Begin();
			break;

		case CC_SPINNER_CHANGE: {			
			ISpinnerControl *spin = (ISpinnerControl*)lParam;
			int sel = SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_GETCURSEL,0,0);
			if (sel!=LB_ERR) cont->SetTime(spin->GetIVal(),sel);
			cont->ip->RedrawViews(cont->ip->GetTime());
			break;
			}

		case CC_SPINNER_BUTTONUP:
			if (HIWORD(wParam)) {
				theHold.Accept(GetString(IDS_RB_SETLINKTIME));
			} else {
				theHold.Cancel();
				}
			cont->ip->RedrawViews(cont->ip->GetTime());
			break;

		case WM_PAINT:
			cont->Update();
			return FALSE;

		case WM_DESTROY:
			cont->DestroyDialog();
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void LinkCtrl::SetupDialog(HWND hWnd)
	{
	this->hWnd = hWnd;

	iTime = GetISpinner(GetDlgItem(hWnd,IDC_LINK_TIMESPIN));
	iTime->SetLimits(TIME_NegInfinity,TIME_PosInfinity,FALSE);
	iTime->SetScale(10.0f);
	iTime->LinkToEdit(GetDlgItem(hWnd,IDC_LINK_TIME),EDITTYPE_TIME);

	iPickOb = GetICustButton(GetDlgItem(hWnd,IDC_LINK_PICK));
	iDelOb  = GetICustButton(GetDlgItem(hWnd,IDC_LINK_DELETE));

	iPickOb->SetType(CBT_CHECK);
	iPickOb->SetHighlightColor(GREEN_WASH);

	SetupList();	
	}

void LinkCtrl::DestroyDialog()
	{
	ReleaseISpinner(iTime);
	ReleaseICustButton(iPickOb);
	ReleaseICustButton(iDelOb);
	iTime = NULL;
	iPickOb = NULL;
	iDelOb = NULL;
	}

void LinkCtrl::SetupList(int sel)
	{
	if (!hWnd || editCont!=this) return;
	int csel = SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_GETCURSEL,0,0);
	if (sel>=0) csel = sel;
	SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_RESETCONTENT,0,0);
	for (int i=0; i<nodes.Count(); i++) {
		SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_ADDSTRING,0,
			(LPARAM)nodes[i]->GetName());
		}
	if (csel!=LB_ERR && csel<nodes.Count()) {
		SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_SETCURSEL,csel,0);
		}
	ListSelChanged();
	}

void LinkCtrl::ListSelChanged()
	{
	int sel = SendDlgItemMessage(hWnd,IDC_LINK_LIST,LB_GETCURSEL,0,0);
	if (sel!=LB_ERR) {
		iDelOb->Enable();
		iTime->Enable();
		iTime->SetValue(times[sel],FALSE);
		EnableWindow(GetDlgItem(hWnd,IDC_LINK_TIMELABEL),TRUE);
	} else {
		iDelOb->Disable();
		iTime->Disable();		
		EnableWindow(GetDlgItem(hWnd,IDC_LINK_TIMELABEL),FALSE);
		}
	}

void LinkCtrl::Invalidate()
	{	
	if (hWnd && editCont==this) {
		valid = FALSE;
		InvalidateRect(hWnd,NULL,FALSE);		
		}
	}

void LinkCtrl::Update()
	{
	if (!valid) {
		valid = TRUE;
		SetupList();
		}
	}


//--- PickLinkMode ------------------------------------------------

BOOL PickLinkMode::Filter(INode *node)
	{
	if (node) {
		return (node->TestForLoop(FOREVER,cont)==REF_SUCCEED);
		}
	return FALSE;
	}

BOOL PickLinkMode::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{
	INode *node = cont->ip->PickNode(hWnd,m);	
	return node?TRUE:FALSE;
	}

BOOL PickLinkMode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	if (node) {
		if (node->TestForLoop(FOREVER,cont)==REF_SUCCEED) {
			theHold.Begin();
			cont->AddNewLink(node,ip->GetTime());			
			theHold.Accept(GetString(IDS_RB_ADDLINK));
			cont->ip->RedrawViews(cont->ip->GetTime());
			}		
		}
	return FALSE;
	}

void PickLinkMode::EnterMode(IObjParam *ip)
	{cont->iPickOb->SetCheck(TRUE);}

void PickLinkMode::ExitMode(IObjParam *ip)
	{cont->iPickOb->SetCheck(FALSE);}


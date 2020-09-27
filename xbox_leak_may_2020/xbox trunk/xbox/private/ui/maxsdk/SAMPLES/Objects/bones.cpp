/**********************************************************************
 *<
	FILE: bones.cpp

	DESCRIPTION:  Bone implementation

	HISTORY: created November 11 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "prim.h"
#include <props.h>
#include "ikctrl.h"

//------------------------------------------------------

class BoneClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return 0; }
	const TCHAR *	ClassName() { return GetString(IDS_DB_BONES_CLASS); }
	int 			BeginCreate(Interface *i);
	int 			EndCreate(Interface *i);
	SClass_ID		SuperClassID() { return SYSTEM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(0x001,0); }
	const TCHAR* 	Category() { return _T("");  }
	void			ResetClassParams(BOOL fileReset);
	};

static BoneClassDesc boneDesc;

ClassDesc* GetBonesDesc() { return &boneDesc; }

class BonesCreationManager : public MouseCallBack, ReferenceMaker {
	friend static BOOL CALLBACK BoneParamDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	friend class PickAutoBone;
	friend class BoneClassDesc;
	private:
		CreateMouseCallBack *createCB;		
		INode *curNode, *lastNode;
		IObjCreate *createInterface;
		ClassDesc *cDesc;
		Matrix3 mat;  // the nodes TM relative to the CP
		IPoint2 lastpt;
		int ignoreSelectionChange;
		BOOL assignIK, assignIKRoot, assignEE, autoLink, copyJP, matchAlign;
		HWND hWnd;

		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		// StdNotifyRefChanged calls this, which can change the partID to new value 
		// If it doesnt depend on the particular message& partID, it should return
		// REF_DONTCARE
	    RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message);

	public:
		void Begin( IObjCreate *ioc, ClassDesc *desc );
		void End();
		
		void AutoBone(INode *cnode,INode *pnode);
		void DoAutoBone(INode *node);
		void MakeEndEffector();
		void AssignIKControllers(INode *cnode,INode *pnode,BOOL newParent,Matrix3 constTM);
		void SetButtonStates(HWND hWnd);

		BonesCreationManager()
			{
			ignoreSelectionChange = FALSE;
			hWnd         = NULL;
			assignIK     = TRUE;
			assignIKRoot = FALSE;
			assignEE     = TRUE; 
			autoLink     = TRUE;
			copyJP       = TRUE;
			matchAlign   = TRUE;
			}
		int proc( HWND hwnd, int msg, int point, int flag, IPoint2 m );
	};


#define CID_BONECREATE	CID_USER + 1

class BonesCreateMode : public CommandMode {		
	public:
		BonesCreationManager proc;

		void Begin( IObjCreate *ioc, ClassDesc *desc ) { proc.Begin( ioc, desc ); }
		void End() { proc.End(); }
		int Class() { return CREATE_COMMAND; }
		int ID() { return CID_BONECREATE; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints = 1000000; return &proc; }
		ChangeForegroundCallback *ChangeFGProc() { return CHANGE_FG_SELECTED; }
		BOOL ChangeFG( CommandMode *oldMode ) { return (oldMode->ChangeFGProc() != CHANGE_FG_SELECTED); }
		void EnterMode() {}
		void ExitMode() {}
		BOOL IsSticky() { return FALSE; }
	};

static BonesCreateMode theBonesCreateMode;

void BoneClassDesc::ResetClassParams(BOOL fileReset)
	{
	theBonesCreateMode.proc.assignIK     = TRUE;
	theBonesCreateMode.proc.assignIKRoot = FALSE;
	theBonesCreateMode.proc.assignEE     = TRUE; 
	theBonesCreateMode.proc.autoLink     = TRUE;
	theBonesCreateMode.proc.copyJP       = TRUE;
	theBonesCreateMode.proc.matchAlign   = TRUE;
	}

class PickAutoBone : 
		public PickModeCallback,
		public PickNodeCallback {
	public:				
		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);
		BOOL Filter(INode *node) {return TRUE;}
		PickNodeCallback *GetFilter() {return this;}
	};
static PickAutoBone thePickAutoBone;

BOOL PickAutoBone::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	INode *node = ip->PickNode(hWnd,m,this);
	return node ? TRUE : FALSE;
	}

BOOL PickAutoBone::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	theBonesCreateMode.proc.DoAutoBone(node);
	ip->SetCommandMode(&theBonesCreateMode);
	ip->RedrawViews(ip->GetTime());
	return FALSE;
	}

void PickAutoBone::EnterMode(IObjParam *ip)
	{
	ICustButton *but = GetICustButton(GetDlgItem(
		theBonesCreateMode.proc.hWnd,IDC_BONES_AUTOBONE));
	but->SetCheck(TRUE);
	ReleaseICustButton(but);
	}

void PickAutoBone::ExitMode(IObjParam *ip)
	{
	ICustButton *but = GetICustButton(GetDlgItem(
		theBonesCreateMode.proc.hWnd,IDC_BONES_AUTOBONE));
	but->SetCheck(FALSE);
	ReleaseICustButton(but);
	}



static BOOL CALLBACK BoneParamDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	BonesCreationManager *mgr = (BonesCreationManager*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG: {
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mgr = (BonesCreationManager*)lParam;
			CheckDlgButton(hWnd,IDC_BONES_ASSIGNIK,mgr->assignIK);
			CheckDlgButton(hWnd,IDC_BONES_ASSIGNIKROOT,mgr->assignIKRoot);
			CheckDlgButton(hWnd,IDC_BONES_CREATEENDEFFECTOR,mgr->assignEE);
			CheckDlgButton(hWnd,IDC_BONE_AUTOLINK,mgr->autoLink);
			CheckDlgButton(hWnd,IDC_BONE_COPYIKPARAMS,mgr->copyJP);
			CheckDlgButton(hWnd,IDC_BONE_MATCHALIGNMENT,mgr->matchAlign);			

			mgr->SetButtonStates(hWnd);
			ICustButton *but = GetICustButton(GetDlgItem(hWnd,IDC_BONES_AUTOBONE));
			but->SetType(CBT_CHECK);
			but->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(but);
			break;
			}
		
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BONE_AUTOLINK:
					mgr->autoLink = 
						IsDlgButtonChecked(hWnd,LOWORD(wParam));
					break;
				case IDC_BONE_COPYIKPARAMS:
					mgr->copyJP =
						IsDlgButtonChecked(hWnd,LOWORD(wParam));
					break;
				case IDC_BONE_MATCHALIGNMENT:
					mgr->matchAlign =
						IsDlgButtonChecked(hWnd,LOWORD(wParam));
					break;

				case IDC_BONES_AUTOBONE:
					if (mgr->createInterface->GetCommandMode()->ID() 
						== CID_STDPICK) {
						mgr->createInterface->SetCommandMode(
							&theBonesCreateMode);
					} else {
						mgr->createInterface->SetPickMode(
							&thePickAutoBone);
						}
					break;

				case IDC_BONES_ASSIGNIK:
					mgr->assignIK = 
						IsDlgButtonChecked(hWnd,IDC_BONES_ASSIGNIK);
					mgr->SetButtonStates(hWnd);
					break;
				case IDC_BONES_ASSIGNIKROOT:
					mgr->assignIKRoot = 
						IsDlgButtonChecked(hWnd,IDC_BONES_ASSIGNIKROOT);
					mgr->SetButtonStates(hWnd);
					break;
				case IDC_BONES_CREATEENDEFFECTOR:
					mgr->assignEE = 
						IsDlgButtonChecked(hWnd,IDC_BONES_CREATEENDEFFECTOR);
					mgr->SetButtonStates(hWnd);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void BonesCreationManager::SetButtonStates(HWND hWnd)
	{
	if (assignIK) {
		EnableWindow(GetDlgItem(hWnd,IDC_BONES_ASSIGNIKROOT),TRUE);
		CheckDlgButton(hWnd,IDC_BONES_ASSIGNIKROOT,assignIKRoot);
		EnableWindow(GetDlgItem(hWnd,IDC_BONES_CREATEENDEFFECTOR),TRUE);
		CheckDlgButton(hWnd,IDC_BONES_CREATEENDEFFECTOR,assignEE);
	} else {
		EnableWindow(GetDlgItem(hWnd,IDC_BONES_ASSIGNIKROOT),FALSE);
		CheckDlgButton(hWnd,IDC_BONES_ASSIGNIKROOT,FALSE);
		EnableWindow(GetDlgItem(hWnd,IDC_BONES_CREATEENDEFFECTOR),FALSE);
		CheckDlgButton(hWnd,IDC_BONES_CREATEENDEFFECTOR,FALSE);
		}
	}

void BonesCreationManager::Begin( IObjCreate *ioc, ClassDesc *desc )
	{
	createInterface = ioc;
	cDesc           = desc;
	createCB        = NULL;
	curNode			= NULL;
	lastNode        = NULL;
	hWnd = createInterface->AddRollupPage(
		hInstance, 
		MAKEINTRESOURCE(IDD_BONEPARAMS), 
		BoneParamDlgProc,
		GetString(IDS_RB_BONEPARAMS),
		(LPARAM)this);
	}

void BonesCreationManager::End()
	{
	createInterface->ClearPickMode();
	createInterface->DeleteRollupPage(hWnd);
	if (curNode) 
		DeleteReference(0);  // sets curNode = NULL
	}

void BonesCreationManager::AutoBone(INode *cnode,INode *pnode)
	{	
	// Create child node
	Object *ob = (Object *)createInterface->
		CreateInstance(HELPER_CLASS_ID,Class_ID(BONE_CLASS_ID,0)); 			
	INode *node = createInterface->CreateObjectNode(ob);
	node->ShowBone(1);		
	Matrix3 ntm = cnode->GetNodeTM(0);
	
	// Only match position if so desired
	if (!matchAlign) {
		Point3 pos = ntm.GetTrans();
		ntm = Matrix3(1);
		ntm.SetTrans(pos);
		}

	// Assign IK controllers
	if (assignIK && (pnode || assignIKRoot)) {
		IKMasterControl *master;
		IKSlaveControl *slave;

		// Either use existing master or make a new one
		Control *cont = NULL;
		if (pnode && (cont=pnode->GetTMController()) &&
			cont->ClassID()==IKSLAVE_CLASSID) {
			// Use its master
			slave  = (IKSlaveControl*)cont;
			master = slave->GetMaster();
		} else {
			// Make a new master
			master = CreateIKMasterControl();
			}

		// Create a slave and assign it to the new node
		slave = CreateIKSlaveControl(master,node);
		node->SetTMController(slave);
		}	
	
	// Set the nod TM and link to parent bone
	node->SetNodeTM(0,ntm);
	if (pnode) pnode->AttachChild(node);	

	// Recurse
	int nc = cnode->NumberOfChildren();
	for (int i=nc-1; i>=0; i--) {
		AutoBone(cnode->GetChildNode(i),node);
		}

	// Attach to bone
	if (autoLink) node->AttachChild(cnode);	

	// Copy IK params
	if (copyJP) {
		IKClipObject *clip = NULL;
		Control *src = cnode->GetTMController();
		Control *dst = node->GetTMController();

		// Do position
		if (src->CanCopyIKParams(COPYPASTE_IKPOS)) 
			clip = src->CopyIKParams(COPYPASTE_IKPOS);
		if (clip && dst->CanPasteIKParams(clip,COPYPASTE_IKPOS)) {
			dst->PasteIKParams(clip,COPYPASTE_IKPOS);
			}
		if (clip) clip->DeleteThis();
		clip = NULL;

		// Do Rotation
		if (src->CanCopyIKParams(COPYPASTE_IKROT)) 
			clip = src->CopyIKParams(COPYPASTE_IKROT);
		if (clip && dst->CanPasteIKParams(clip,COPYPASTE_IKROT)) {
			dst->PasteIKParams(clip,COPYPASTE_IKROT);
			}
		if (clip) clip->DeleteThis();
		clip = NULL;
		}

	// Last nodes in the chain become end effectors
	if (!nc) {
		lastNode = node;
		MakeEndEffector();
		lastNode = NULL;
		}
	}

void BonesCreationManager::DoAutoBone(INode *node)
	{
	theHold.Begin();
	AutoBone(node,NULL);
	createInterface->RedrawViews(createInterface->GetTime());
	theHold.Accept(IDS_DS_CREATE);	 
	}

void BonesCreationManager::MakeEndEffector()
	{
	if (!lastNode || !assignEE) return;
	IKSlaveControl *slave;
	Control *cont = lastNode->GetTMController();
	if (cont && cont->ClassID()==IKSLAVE_CLASSID) {			
		slave  = (IKSlaveControl*)cont;
	} else return;

	Matrix3 tm = lastNode->GetNodeTM(createInterface->GetTime());
	slave->MakeEE(TRUE,1,tm.GetTrans(),Quat());
	}

void BonesCreationManager::AssignIKControllers(
		INode *cnode,INode *pnode,BOOL newParent,Matrix3 constTM)
	{
	Point3 pos;
	IKMasterControl *master;
	IKSlaveControl *slave;
	int axis = 0;
	float projX, projY, projZ, best;

	// Find the world axis that's closest to the construction plane's Z axis
	projX = (float)fabs(DotProd(constTM.GetRow(2),Point3(1,0,0)));
	projY = (float)fabs(DotProd(constTM.GetRow(2),Point3(0,1,0)));
	projZ = (float)fabs(DotProd(constTM.GetRow(2),Point3(0,0,1)));
	best  = projX;
	if (projY>best) {
		axis = 1;
		best = projY;
		}
	if (projZ>best) axis = 2;

	if (newParent && assignIKRoot) {
		// Need to create a new master controller
		master = CreateIKMasterControl();
		slave  = CreateIKSlaveControl(master,pnode);
		pos    = pnode->GetNodeTM(0).GetTrans();
		slave->SetInitPos(pos);
		slave->SetDOF(axis+3,TRUE);
		pnode->SetTMController(slave);
	} else {
		// See if the parent has an IK controller
		Control *cont = pnode->GetTMController();
		if (cont->ClassID()==IKSLAVE_CLASSID) {
			// Use its master
			slave  = (IKSlaveControl*)cont;
			master = slave->GetMaster();
		} else {
			// Make a new master
			master = CreateIKMasterControl();
			}
		}

	// Create a slave for the child node
	slave = CreateIKSlaveControl(master,cnode);
	pos   = cnode->GetNodeTM(0).GetTrans();
	slave->SetInitPos(pos);
	slave->SetDOF(axis+3,TRUE);
	cnode->SetTMController(slave);
	}


void BonesCreationManager::SetReference(int i, RefTargetHandle rtarg) { 
	switch(i) {
		case 0: curNode = (INode *)rtarg; break;
		default: assert(0); 
		}
	}

RefTargetHandle BonesCreationManager::GetReference(int i) { 
	switch(i) {
		case 0: return (RefTargetHandle)curNode;
		default: assert(0); 
		}
	return NULL;
	}


RefResult BonesCreationManager::NotifyRefChanged(
	Interval changeInt, 
	RefTargetHandle hTarget, 
	PartID& partID,  
	RefMessage message) 
	{
	switch (message) {
		case REFMSG_TARGET_SELECTIONCHANGE:
		 	if ( ignoreSelectionChange ) {
				break;
				}
		 	if ( curNode==hTarget ) {
				// this will set curNode== NULL;
				DeleteReference(0);
				goto endEdit;
				}
			// fall through

		case REFMSG_TARGET_DELETED:		
			if ( curNode==hTarget ) {				
				endEdit:
				curNode    = NULL;
				}
			break;		
		}
	return REF_SUCCEED;
	}

static int DSQ(IPoint2 p, IPoint2 q) {
	return (p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y);
	}


class BonesPicker: public PickNodeCallback {
	BOOL Filter(INode *node) {
		Object* obj = node->GetObjectRef();
		if (obj && obj->SuperClassID()==HELPER_CLASS_ID && obj->ClassID()==Class_ID(BONE_CLASS_ID,0))
			return 1;
		return 0;
		}
	};
			
int BonesCreationManager::proc( 
				HWND hwnd,
				int msg,
				int point,
				int flag,
				IPoint2 m )
	{	
	int res;
	INode *newNode,*parNode;	
	BonesPicker bonePick;
	ViewExp *vpx = createInterface->GetViewport(hwnd); 
	assert( vpx );
	Matrix3 constTM;
	vpx->GetConstructionTM(constTM);


	switch ( msg ) {

		case MOUSE_POINT:
				{
				Object *ob;
				if (point==0) {
					mat.IdentityMatrix();
					if ( createInterface->SetActiveViewport(hwnd) ) {
						return FALSE;
						}
					}
				else  {
					if (DSQ(m,lastpt)<10) {
						res = TRUE;
						goto done;
						}
					
					theHold.SuperAccept(IDS_DS_CREATE);	 
					}

				if (createInterface->IsCPEdgeOnInView()) { 
					res = FALSE;
					goto done;
					}

				
			   	theHold.SuperBegin();	 // begin hold for undo
				theHold.Begin();

				mat.IdentityMatrix();
#ifdef _3D_CREATE
				mat.SetTrans(constTM*vpx->SnapPoint(m,m,NULL,SNAP_IN_3D));
#else
				//mat.SetTrans(vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
				mat.SetTrans(constTM*vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
#endif

				BOOL newParent = FALSE;
				if (curNode==NULL) 	{
					INode* overBone = createInterface->PickNode(hwnd,m,&bonePick);
					if (overBone) {
						parNode = overBone;
						}
					else {
						// Make first node 
						ob = (Object *)createInterface->
							CreateInstance(HELPER_CLASS_ID,Class_ID(BONE_CLASS_ID,0)); 			
						parNode = createInterface->CreateObjectNode(ob);
						//createInterface->SetNodeTMRelConstPlane(parNode, mat);
						parNode->SetNodeTM(0,mat);
						newParent = TRUE;
						}
					parNode->ShowBone(1);
					}
				else {
					lastNode = parNode = curNode;
					DeleteReference(0);
					}

				// Make new node 
				ob = (Object *)createInterface->
					CreateInstance(HELPER_CLASS_ID,Class_ID(BONE_CLASS_ID,0)); 			
				newNode = createInterface->CreateObjectNode(ob);
				newNode->ShowBone(1);
				
				//createInterface->SetNodeTMRelConstPlane(newNode, mat);
				newNode->SetNodeTM(0,mat);				

				parNode->AttachChild(newNode); // make node a child of prev 
				curNode = newNode;

				// Reference the new node so we'll get notifications.
				MakeRefByID( FOREVER, 0, curNode);
	
				// Assign IK controllers
				if (assignIK) 
					AssignIKControllers(curNode,parNode,newParent,constTM);

			   	ignoreSelectionChange = TRUE;
			   	createInterface->SelectNode( curNode);
			   	ignoreSelectionChange = FALSE;
	
 				createInterface->RedrawViews(createInterface->GetTime());  
				theHold.Accept(IDS_DS_CREATE);	 
				lastpt = m;
				res = TRUE; 
				}
				break;
		case MOUSE_MOVE:
			// The user can loose capture by switching to another app.
			if (!GetCapture()) {				
				theHold.SuperAccept(IDS_DS_CREATE);
				return FALSE;
				}
			
			if (curNode) {
#ifdef _3D_CREATE
				mat.SetTrans(constTM*vpx->SnapPoint(m,m,NULL,SNAP_IN_3D));
#else
				//mat.SetTrans(vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
				mat.SetTrans(constTM*vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
#endif
				//createInterface->SetNodeTMRelConstPlane(curNode, mat);
				curNode->SetNodeTM(0,mat);
				createInterface->RedrawViews(createInterface->GetTime());
				}
			res = TRUE;
			break;

		case MOUSE_FREEMOVE: {
			INode* overNode = createInterface->PickNode(hwnd,m,&bonePick);
			if (overNode) {
				SetCursor(LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CROSS_HAIR)));
				}
			else {
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			}
#ifdef _OSNAP
		#ifdef _3D_CREATE
				vpx->SnapPreview(m,m,NULL, SNAP_IN_3D);
		#else
				vpx->SnapPreview(m,m,NULL, SNAP_IN_PLANE);
		#endif
#endif
			break;

// mjm - 3.1.99
		case MOUSE_PROPCLICK:
			// right click while between creations
			createInterface->RemoveMode(NULL);
			break;
// mjm - end

		case MOUSE_ABORT:
			if (curNode) {				
				theHold.SuperCancel(); // this deletes curNode and everything
				curNode = NULL;	 
				MakeEndEffector();
				createInterface->RedrawViews(createInterface->GetTime()); 
				}
			res = FALSE;
			break;
		}
	
	done:
	createInterface->ReleaseViewport(vpx); 
	return res;
	}

int BoneClassDesc::BeginCreate(Interface *i)
	{
	IObjCreate *iob = i->GetIObjCreate();
	
	theBonesCreateMode.Begin( iob, this );
	iob->PushCommandMode( &theBonesCreateMode );
	
	return TRUE;
	}

int BoneClassDesc::EndCreate(Interface *i)
	{
	
	theBonesCreateMode.End();
	i->RemoveMode( &theBonesCreateMode );

	return TRUE;
	}


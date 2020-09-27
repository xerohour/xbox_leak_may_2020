/**********************************************************************
 *<
	FILE: surfwrap.cpp

	DESCRIPTION:  A Procedural Geometry Projection Tool

	CREATED BY: Audrey Peterson

	HISTORY: created January 1997

 *>	Copyright (c) 1997 All Rights Reserved.
 **********************************************************************/
#include "proscatr.h"

#define BIGFLOAT	float(999999)
static Class_ID SURFWRAPOBJ_CLASS_ID(0x1ab13757, 0x12365b98);

#define CID_CREATESURFWRAPMODE CID_USER + 30		
#define REF_OP1		0
#define REF_OP2		1
#define REF_CONT1	2
#define REF_CONT2	3
#define PBLOCK		4
#define MBASE		5

// Flag bits
#define SWRAP_OB1SEL			(1<<0)
#define SWRAP_OB2SEL			(1<<1)
#define SWRAP_ANYSEL			(SWRAP_OB1SEL|SWRAP_OB2SEL)
#define SWRAP_INRENDER			(1<<2)
#define SWRAP_UPDATEALWAYS	(1<<3)
#define SWRAP_UPDATERENDER	(1<<4)
#define SWRAP_UPDATEMANUAL	(1<<5)
#define SWRAP_DISPRESULT    (1<<6)

//--- Parameter map/block descriptors -------------------------------
#define PB_DOWITHDIST		0
#define PB_KIDEFAULT		1
#define PB_STANDOFF			2
#define PB_USESELVERTS		3
#define PB_HIDEDISTOBJECT	4
#define PB_UPDATETYPE		5
#define PB_GETNORM			6
#define PB_SHOWTYPE			7


static int createDistDoIDs[] = {IDC_AP_DISTREF,IDC_AP_DISTCOPY,IDC_AP_DISTMOVE,IDC_AP_DISTINST};
static int UpdateIDs[] = {IDC_SWRAP_UPDATEALWAYS,IDC_SWRAP_UPDATERENDER,IDC_SWRAP_UPDATEMANUAL};
static int NormIDs[] = {IDC_AP_VIEWDIR,IDC_AP_OBJECTDIR,IDC_AP_VERTEXDIR,IDC_AP_WRAPPERCTRDIR,IDC_AP_WRAP2CTRDIR,IDC_AP_WRAPPERPIVDIR,IDC_AP_WRAP2PIVDIR};
static int ShowIDs[] = {IDC_SW_DISPRESULT,IDC_SW_DISPOPS};

static ParamUIDesc descObjects[] = {
	// Get Normal BY
	ParamUIDesc(PB_GETNORM,TYPE_RADIO,NormIDs,7),			

	// Default Projection Distance Ki
	ParamUIDesc(
		PB_KIDEFAULT,
		EDITTYPE_FLOAT,
		IDC_AP_KIDEFAULT,IDC_AP_KIDEFAULTSPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),

	// Standoff Distance
	ParamUIDesc(
		PB_STANDOFF,
		EDITTYPE_FLOAT,
		IDC_AP_STANDOFF2,IDC_AP_STANDOFFSPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),

	// Use Selected Vertices ONLY
	ParamUIDesc(PB_USESELVERTS,TYPE_SINGLECHEKBOX,IDC_AP_USESELVERTS),

	// Start with Perpendicular Instances
	ParamUIDesc(PB_HIDEDISTOBJECT,TYPE_SINGLECHEKBOX,IDC_AP_HIDEWRAPTO),
	
	// Update Types
	ParamUIDesc(PB_UPDATETYPE,TYPE_RADIO,UpdateIDs,3),
	
	// Show Types
	ParamUIDesc(PB_SHOWTYPE,TYPE_RADIO,ShowIDs,2),			
	};

#define OBJECTSDESC_LENGTH 7

static ParamUIDesc descPickDist[] = {
	// What to do with dist object
	ParamUIDesc(PB_DOWITHDIST,TYPE_RADIO,createDistDoIDs,4),
	};

#define PICKDISTDESC_LENGTH 1

// variable type, NULL, animatable, number
ParamBlockDescID SWdescVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_INT, NULL, FALSE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 }
	};
ParamBlockDescID SWdescVer1[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_INT, NULL, FALSE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_INT, NULL, FALSE, 7 }
	};

#define PBLOCK_LENGTH	8

#define NUM_OLDVERSIONS	1

#define CURRENT_VERSION	1
static ParamVersionDesc swversions[] = {
	ParamVersionDesc(SWdescVer0,7,0),
	};
static ParamVersionDesc curVersion(SWdescVer1,PBLOCK_LENGTH,CURRENT_VERSION);

class VNormal {
	public:
		Point3 norm;
		DWORD smooth;
		VNormal *next;
		BOOL init;

		VNormal() {smooth=0;next=NULL;init=FALSE;norm=Point3(0,0,0);}
		VNormal(Point3 &n,DWORD s) {next=NULL;init=TRUE;norm=n;smooth=s;}
		~VNormal() {delete next;}
		void AddNormal(Point3 &n,DWORD s);
		Point3 &GetNormal(DWORD s);
		void Normalize();
	};

void VNormal::AddNormal(Point3 &n,DWORD s)
	{
	if (!(s&smooth) && init) {
		if (next) next->AddNormal(n,s);
		else {
			next = new VNormal(n,s);
			}
	} else {
		norm   += n;
		smooth |= s;
		init    = TRUE;
		}
	}

Point3 &VNormal::GetNormal(DWORD s)
	{
	if (smooth&s || !next) return norm;
	else return next->GetNormal(s);	
	}

void VNormal::Normalize()
	{
	VNormal *ptr = next, *prev = this;
	while (ptr) {
		if (ptr->smooth&smooth) {
			norm += ptr->norm;			
			prev->next = ptr->next;
			delete ptr;
			ptr = prev->next;
		} else {
			prev = ptr;
			ptr  = ptr->next;
			}
		}
	norm = ::Normalize(norm);
	if (next) next->Normalize();
	}

class SWPickOperand;
class SWPickNorm;

class SurfWrapObject: public GeomObject, public MeshOpProgress {
		
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );
				
	public:
		TSTR opaName, opbName, normname;
		Object *ob1, *ob2;
		Control *tm1,*tm2;
		INode *mbase,*mainnode;
		static IParamMap *pmapObjects;
		static IParamMap *pmapDist;
		IParamBlock *pblock;
		static Matrix3 createTM;
		static int dlgNormby;
		static HWND thwnd;
		Point3 wrappivot,topivot;
		Matrix3 wrapoff,wrapoff2;
		Interval ivalid;
		Mesh mesh;	
		DWORD flags;
		int ob1verts,ob1faces,firstmtls,secondmtls;

		Point3 viewPt;
		static IObjParam *ip;
		static SWPickOperand pickCB;
		static SWPickNorm pickNorm;
		static BOOL creating;
		static SurfWrapObject *editOb;

		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;
		static SelectModBoxCMode *selectMode;

		SurfWrapObject();
		~SurfWrapObject();
		
		void SetFlag(DWORD mask) { flags|=mask; }
		void ClearFlag(DWORD mask) { flags &= ~mask; }
		int TestFlag(DWORD mask) { return(flags&mask?1:0); }
		DWORD BoolOp(int &order);

		void SetOperand(INode *node,Matrix3& boolTm,Matrix3& oppTm,int which,INode *orignode,bool *canUndo=NULL);
		BOOL UpdateMesh(TimeValue t,BOOL force=FALSE,BOOL sel=FALSE);
		Object *GetPipeObj(TimeValue t,int which);
		Matrix3 GetOpTM(TimeValue t,int which,Interval *iv=NULL);
		void Invalidate() {ivalid.SetEmpty();}

		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		TCHAR *GetObjectName() { return GetString(IDS_AP_SURFWRAPNAME); }
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);

		// For sub-object selection
		void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
		void Rotate( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE);
		void Scale( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext* mc);
		
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert);
		void ClearSelection(int selLevel);

		int SubObjectIndex(HitRecord *hitRec);
		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);

		void ActivateSubobjSel(int level, XFormModes& modes);
		void ShowOpList(BOOL isobj);
		void ShowName(BOOL ob2);
		void ShowNormName();
		// From Object		
		void InitNodeName(TSTR& s) {s = GetString(IDS_AP_SURFWRAPNAME);}
		Interval ObjectValidity(TimeValue t);
		int CanConvertToType(Class_ID obtype);
		Object* ConvertToType(TimeValue t, Class_ID obtype);
		ObjectState Eval(TimeValue time);
		int NumPipeBranches();
		Object *GetPipeBranch(int i);
		INode *GetBranchINode(TimeValue t,INode *node,int i);
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int aflags);
		void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel);
		void GetLocalBoundBox(TimeValue t, INode *inode,ViewExp* vpt, Box3& box);
		void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);

		// From GeomObject		
		ObjectHandle CreateTriObjRep(TimeValue t) {return NULL;}
		int IntersectRay(TimeValue t, Ray& r, float& at, Point3& norm);
		Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

		// Animatable methods
		Class_ID ClassID() {return SURFWRAPOBJ_CLASS_ID;}  
		void GetClassName(TSTR& s) {s = GetString(IDS_AP_SURFWRAPOBJECT);}
		void DeleteThis() {delete this;}				
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		int RenderBegin(TimeValue t, ULONG flags);
		int RenderEnd(TimeValue t);

		int NumSubs();
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum);

		// From ref
		RefTargetHandle Clone(RemapDir& remap);
		int NumRefs() {return 6;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		// From MeshOpProgress
		BOOL Progress(int p);
		void Init(int total);
		void InvalidateUI();
	};				

class SWPickOperand : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		SurfWrapObject *bo;
		
		SWPickOperand() {bo=NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);

		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);

		BOOL Filter(INode *node);
		
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}

		PickNodeCallback *GetFilter() {return this;}
	};
class SWPickNorm : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		SurfWrapObject *bo;
		
		SWPickNorm() {bo=NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);

		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);

		BOOL Filter(INode *node);
		
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}

		PickNodeCallback *GetFilter() {return this;}
	};

IParamMap *SurfWrapObject::pmapObjects = NULL;
IParamMap *SurfWrapObject::pmapDist = NULL;
IObjParam *SurfWrapObject::ip                   = NULL;
BOOL SurfWrapObject::creating                   = FALSE;
SurfWrapObject *SurfWrapObject::editOb              = NULL;
MoveModBoxCMode*    SurfWrapObject::moveMode    = NULL;
RotateModBoxCMode*  SurfWrapObject::rotMode 	= NULL;
UScaleModBoxCMode*  SurfWrapObject::uscaleMode  = NULL;
NUScaleModBoxCMode* SurfWrapObject::nuscaleMode = NULL;
SquashModBoxCMode*  SurfWrapObject::squashMode  = NULL;
SelectModBoxCMode*  SurfWrapObject::selectMode  = NULL;
SWPickOperand SurfWrapObject::pickCB;
SWPickNorm SurfWrapObject::pickNorm;
Matrix3 SurfWrapObject::createTM;
HWND SurfWrapObject::thwnd;
int SurfWrapObject::dlgNormby=0;

class SWrapObjClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new SurfWrapObject;}
	const TCHAR *	ClassName() { return GetString(IDS_AP_SURFWRAPOBJECT); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return SURFWRAPOBJ_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_AP_COMPOUNDOBJECTS);}
	BOOL			OkToCreate(Interface *i);
	int 			BeginCreate(Interface *i);
	int 			EndCreate(Interface *i);
	void			ResetClassParams(BOOL fileReset);
	};

BOOL SurfWrapObject::HasUVW() { 
	BOOL genUV1=FALSE,genUV2=FALSE;
    if (ob1) genUV1=ob1->HasUVW();
	if (ob2) genUV2=ob2->HasUVW();
	return (genUV1&& genUV2); 
	}

void SurfWrapObject::SetGenUVW(BOOL sw) {  
	if (sw==HasUVW()) return;
	if (ob1) ob1->SetGenUVW(sw);
	if (ob2) ob2->SetGenUVW(sw);			
	}

void SWrapObjClassDesc::ResetClassParams(BOOL fileReset)
	{SurfWrapObject::dlgNormby=0;
	}

static TriObject *GetTriObject(TimeValue t,Object *obj,Interval &valid,BOOL &needsDel)
	{	
	needsDel = FALSE;
	if (!obj) return NULL;
	ObjectState os = obj->Eval(t);
	valid &= os.Validity(t);
	if (os.obj->IsSubClassOf(triObjectClassID)) {
		return (TriObject*)os.obj;
	} else {
		if (os.obj->CanConvertToType(triObjectClassID)) {
			Object *oldObj = os.obj;
			TriObject *tobj = (TriObject*)os.obj->ConvertToType(t,triObjectClassID);			
			needsDel = (tobj != oldObj);			
			return tobj;
			}
		}
	return NULL;
	}

BOOL SWrapObjClassDesc::OkToCreate(Interface *i)
	{
	if (i->GetSelNodeCount()!=1) return FALSE;
	Interval intvalid;
	BOOL needsDel;
	Object *tob=i->GetSelNode(0)->GetObjectRef();
	TriObject *tri=GetTriObject(i->GetTime(),tob,intvalid,needsDel);
	if (tri==NULL) return FALSE;
	if (needsDel) tri->DeleteThis();
	return TRUE;	
	}

static SWrapObjClassDesc SWrapObjDesc;

ClassDesc* GetSWrapObjDesc() { return &SWrapObjDesc; }

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

//----------------------------------------------------------------------

class CreateSWrapProc : public MouseCallBack {
	public:
		IObjParam *ip;
		void Init(IObjParam *i) {ip=i;}
		int proc( 
			HWND hWnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
	};

int CreateSWrapProc::proc( 
		HWND hWnd, 
		int msg, 
		int point, 
		int flags, 
		IPoint2 m ) 
	{
	switch (msg) {
		case MOUSE_POINT:
			ip->SetActiveViewport(hWnd);
			break;
		case MOUSE_FREEMOVE:
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			break;
		case MOUSE_PROPCLICK:
			// right click while between creations
			ip->RemoveMode(NULL);
			break;
		}	
	return TRUE;
	}

class CreateSWrapMode : public CommandMode, ReferenceMaker {		
	public:		
		CreateSWrapProc proc;
		INode *node, *svNode;
		IObjParam *ip;
		SurfWrapObject *obj;

		void Begin(INode *n,IObjParam *i);
		void End(IObjParam *i);
		void JumpStart(IObjParam *i,SurfWrapObject *o);

		int Class() {return CREATE_COMMAND;}
		int ID() { return CID_CREATESURFWRAPMODE; }
		MouseCallBack *MouseProc(int *numPoints) {*numPoints = 1; return &proc;}
		ChangeForegroundCallback *ChangeFGProc() {return CHANGE_FG_SELECTED;}
		BOOL ChangeFG(CommandMode *oldMode) {return TRUE;}
		void EnterMode() {/*MakeRefByID(FOREVER,0,svNode);*/}
		void ExitMode() {/*DeleteAllRefsFromMe();*/}
		
		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return node;}
		void SetReference(int i, RefTargetHandle rtarg) {node = (INode*)rtarg;}
	    RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message);		
	};
static CreateSWrapMode theCreateSWrapMode;


void GenerateViewRay(Point3 *viewPt,Interface *intf)
{  float xpos=1.0f,ypos=1.0f;
   Ray ray;
	ViewExp *view = intf->GetActiveViewport();
	GraphicsWindow *gw=view->getGW();
	if (gw->isPerspectiveView())
	{ xpos+=gw->getWinSizeX()/2;
	  ypos+=gw->getWinSizeY()/2;
	}
	view->MapScreenToWorldRay(xpos, ypos,ray);
	*viewPt=ray.dir;
	intf->ReleaseViewport(view);
}

RefResult CreateSWrapMode::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID,
		RefMessage message)
	{
	switch (message) {
		case REFMSG_TARGET_SELECTIONCHANGE:		
		case REFMSG_TARGET_DELETED:			
			if (ip) ip->StopCreating();
			break;

		}
	return REF_SUCCEED;
	}

class CreateSurfWrapRestore : public RestoreObj {
	public:   		
		void Restore(int isUndo) {
			if (theCreateSWrapMode.ip) {
				// Jump out of boolean create mode.
				theCreateSWrapMode.ip->SetStdCommandMode(CID_OBJMOVE);
				}
			}	
		void Redo() {}
		TSTR Description() {return GetString(IDS_AP_CREATESCAT);}
	};

// Sending the REFMSG_NOTIFY_PASTE message notifies the modify
// panel that the Node's object reference has changed when
// undoing or redoing.

class CreateSurfWrapTM : public RestoreObj {
	public:   		
		SurfWrapObject *obj;
		INode *oldn;
		Control *oldc;
		CreateSurfWrapTM(SurfWrapObject *o, INode *n) {
			obj = o; oldn=n; 
			}
		void Restore(int isUndo) {
			if (obj->tm1) {
				oldn->SetTMController(obj->tm1);
			    obj->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
				}
			}	
		void Redo() { }
		TSTR Description() {return GetString(IDS_AP_CREATESCATNOTIFY);}
	};

class CreateWraptoref : public RestoreObj {
	public:   		
		SurfWrapObject *obj;
		TSTR name,name2;
		CreateWraptoref(SurfWrapObject *o, TSTR n, TSTR n1) {
			obj = o; name=TSTR(n);name2=TSTR(n1); 
			}
		void Restore(int isUndo) {
			BOOL isname=_tcslen(name)>0;
			obj->opbName=(isname?TSTR(GetString(IDS_AP_D)) + name:name);
			obj->ShowOpList(isname);
			}	
		void Redo() 
		{   BOOL isname=_tcslen(name2)>0;
		obj->opbName=(isname?TSTR(GetString(IDS_AP_D)) + name2:name2);
			obj->ShowOpList(isname);
			}
		TSTR Description() {return GetString(IDS_AP_CREATESCATNOTIFY);}
	};

class CreateSurfWrapNotify : public RestoreObj {
	public:   		
		SurfWrapObject *obj;
		BOOL which;
		CreateSurfWrapNotify(SurfWrapObject *o, BOOL w) {
			obj = o; which = w;
			}
		void Restore(int isUndo) {
			if (which) {
				obj->NotifyDependents(FOREVER,0,REFMSG_NOTIFY_PASTE);
				}
			}	
		void Redo() {
			if (!which) {
				obj->NotifyDependents(FOREVER,0,REFMSG_NOTIFY_PASTE);
				}
			}
		TSTR Description() {return GetString(IDS_AP_CREATESCATNOTIFY);}
	};

void CreateSWrapMode::Begin(INode *n,IObjParam *i) 
	{
	MakeRefByID(FOREVER,0,n);
	svNode = node;
	assert(node);
	ip = i;
	proc.Init(ip);

	theHold.Begin();
	theHold.Put(new CreateSurfWrapRestore);

	obj = new SurfWrapObject;

	theHold.Put(new CreateSurfWrapNotify(obj,1));

	obj->createTM = node->GetObjectTM(0);
	GenerateViewRay(&obj->viewPt,ip);

	Matrix3 oldtm=node->GetObjectTM(0);
	obj->mainnode=node;
		theHold.Begin();

	obj->SetOperand(
		node,
		obj->createTM,
		oldtm,
		0,NULL);
	obj->createTM=node->GetObjectTM(0);

	node->SetObjectRef(obj);
	
	theHold.Put(new CreateSurfWrapNotify(obj,0));

	theHold.Accept(GetString(IDS_AP_CREATE));

	obj->BeginEditParams(i,BEGIN_EDIT_CREATE,NULL);
	}

void CreateSWrapMode::End(IObjParam *i)
	{
	svNode = node;
	if (obj) obj->EndEditParams(i,END_EDIT_REMOVEUI,NULL);
	DeleteAllRefsFromMe();
	ip  = NULL;
	obj = NULL;
	}

void CreateSWrapMode::JumpStart(IObjParam *i,SurfWrapObject *o)
	{
	ip  = i;
	obj = o;
	//MakeRefByID(FOREVER,0,svNode);
	obj->BeginEditParams(i,BEGIN_EDIT_CREATE,NULL);
	}


int SWrapObjClassDesc::BeginCreate(Interface *i)
	{	
	assert(i->GetSelNodeCount()==1);

	theCreateSWrapMode.Begin(i->GetSelNode(0),(IObjParam*)i);
	i->PushCommandMode(&theCreateSWrapMode);
	return TRUE;
	}

int SWrapObjClassDesc::EndCreate(Interface *i)
	{
	theCreateSWrapMode.End((IObjParam*)i);
	i->RemoveMode(&theCreateSWrapMode);
	return TRUE;
	}

//----------------------------------------------------------------------


SurfWrapObject::SurfWrapObject()
	{	MakeRefByID(FOREVER, PBLOCK, CreateParameterBlock(SWdescVer1, PBLOCK_LENGTH, CURRENT_VERSION));	
	ob1 = ob2 = NULL;
	tm1 = tm2 = NULL;
	mbase = NULL;
	ivalid.SetEmpty();
	pblock->SetValue(PB_DOWITHDIST,0,3);
	pblock->SetValue(PB_KIDEFAULT,0,1.0f);
	pblock->SetValue(PB_STANDOFF,0,1.0f);
	pblock->SetValue(PB_USESELVERTS,0,0);
	pblock->SetValue(PB_HIDEDISTOBJECT,0,0);
	pblock->SetValue(PB_GETNORM,0,dlgNormby);
	pblock->SetValue(PB_SHOWTYPE,0,0);
	flags = 0;
	SetFlag(SWRAP_UPDATEALWAYS|SWRAP_DISPRESULT);
	ob1verts=0;ob1faces=0;
	firstmtls=0;secondmtls=0;
	opaName=TSTR(_T(""));
	opbName=TSTR(_T(""));
	}

SurfWrapObject::~SurfWrapObject()
	{
	DeleteAllRefsFromMe();
	}

const Point3 Zero=Point3(0.0f,0.0f,0.0f);

Matrix3 GetObjOffMtx(INode *node)
{ ScaleValue scale=node->GetObjOffsetScale();
  Quat rot=node->GetObjOffsetRot();
  Point3 pos=node->GetObjOffsetPos();
  Matrix3 pp(TRUE),ppr(TRUE);
  ApplyScaling(pp,scale);
  rot.MakeMatrix(ppr);
  pp=pp*ppr;
  pp.SetTrans(pos);
  pp=Inverse(pp);
  return pp;
}

void SurfWrapObject::SetOperand(INode *node,Matrix3& boolTm,Matrix3& oppTm,int which,INode *orignode,bool *canundo)
{ if (which) { opbName = TSTR(GetString(IDS_AP_D)) + TSTR(node->GetName()); }
  else { opaName= TSTR(GetString(IDS_AP_S)) + TSTR(node->GetName()); }

  BOOL delnode = FALSE;
  Object *obj = node->GetObjectRef();

  if (which)
  {	int	dmethod;
	pblock->GetValue(PB_DOWITHDIST,0,dmethod,FOREVER);
	if (dmethod==0)	obj = MakeObjectDerivedObject(obj);
	else if (dmethod==1) obj = (Object*)obj->Clone();
    else if (dmethod==2) delnode = TRUE;
  }
	
  // Plug in the object and a new controller.	
  if (which?ob1:ob2) ReplaceReference(which,obj,TRUE);
  else MakeRefByID(FOREVER,which,obj);
  ShowOpList(ob2!=NULL);
  if (which)
  { RemapDir *remap = NewRemapDir(); 
    ReplaceReference(which+2,remap->CloneRef(node->GetTMController()));
    remap->DeleteThis();
    wrapoff2=GetObjOffMtx(node);
    int c1=0,c2=0;
    if (orignode)
	{ if (orignode->GetMtl() && node->GetMtl()) 
	  { if (orignode->GetMtl()!=node->GetMtl()) 
		{ Mtl *multi = CMaterials(orignode->GetMtl(), node->GetMtl(), c1, c2);
		  orignode->SetMtl(multi);
		  (*canundo)=FALSE;
		}
	  }
	  else if (node->GetMtl()) orignode->SetMtl(node->GetMtl());
	}
	firstmtls=c1;secondmtls=c2;
  }
  else
  { Control *c1=NewDefaultMatrix3Controller();
    wrapoff=GetObjOffMtx(node);
    ReplaceReference(which+2,node->GetTMController());
    theHold.Put(new CreateSurfWrapTM(this,node));
    node->SetTMController(c1);
    boolTm.NoRot();boolTm.NoScale();
    node->SetNodeTM(0,boolTm);
    Point3 spt=Point3(1.0f,1.0f,1.0f);
    ScaleValue s(spt);
    node->SetObjOffsetScale(s);
    Quat q;q.Identity();
    node->SetObjOffsetRot(q);
    node->SetObjOffsetPos(Zero);
  }
  SuspendAnimate();
  AnimateOff();
  SetXFormPacket pckt(oppTm,boolTm);
  if (which)
  { tm2->SetValue(0,&pckt);}
  else { tm1->SetValue(0,&pckt);}
  ResumeAnimate();
  if (delnode) ip->DeleteNode(node);
  NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
  theHold.Accept(GetString(IDS_AP_PICKWTOOBJ));
  ivalid.SetEmpty();
}		

Object *SurfWrapObject::GetPipeObj(TimeValue t,int which)
	{
	ObjectState os;
	if (which==0) {
		if (ob1) {
			os = ob1->Eval(t);
			return os.obj;
		} else {
			return NULL;
			}
	} else {
		if (ob2) {
			os = ob2->Eval(t);
			return os.obj;
		} else {
			return NULL;
			}
		}
	return os.obj;
	}

Matrix3 SurfWrapObject::GetOpTM(TimeValue t,int which,Interval *iv)
	{
	Matrix3 tm(1);
	Interval valid, *v;
	if (iv) v = iv;
	else v = &valid;
	
	if (which==0)
	{ if (tm1) 
	{tm1->GetValue(t,&tm,*v,CTRL_RELATIVE);
	  wrappivot=wrapoff.GetTrans();
	 } else wrappivot=Zero;
	}
	else if (which==1)
	{ if (tm2) 
	{tm2->GetValue(t,&tm,*v,CTRL_RELATIVE);
	  topivot=wrapoff2.GetTrans();
	 } else topivot=Zero;
	}
	return tm;
	}	   

int SurfWrapObject::RenderBegin(TimeValue t, ULONG flags)
	{
	SetFlag(SWRAP_INRENDER);
	if (TestFlag(SWRAP_UPDATERENDER)) {
		ivalid.SetEmpty();
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		}
	return 0;
	}

int SurfWrapObject::RenderEnd(TimeValue t)
	{
	ClearFlag(SWRAP_INRENDER);	
	return 0;
	}

void SurfWrapObject::Init(int total)
	{
	}

BOOL SurfWrapObject::Progress(int p)
	{
	SHORT res = GetAsyncKeyState(VK_ESCAPE);
	if (res&1) {
		return FALSE;
		}
	else return TRUE;
	}

 #define EPSILON	0.0001f

int RayIntersect(Ray& ray, float& at, Point3& norm,Mesh *amesh,Tab<VNormal> vnorms,Tab<Point3> fnorms)
	{
	DWORD fi;
	Point3 bary;
	Face *face = amesh->faces;	
	Point3 v0, v1, v2;
	Point3 n, sum, p, bry;
	float d, rn, a;
	Matrix3 vTM(1);
	BOOL first = FALSE;
	fi = 0xFFFFFFFF;

	for (int i=0; i<amesh->getNumFaces(); i++,face++) {
		n = fnorms[i];
		
		// See if the ray intersects the plane (backfaced)
		rn = DotProd(ray.dir,n);
		if (rn > -EPSILON) continue;
		
		// Use a point on the plane to find d
		d = DotProd(amesh->verts[face->v[0]],n);

		// Find the point on the ray that intersects the plane
		a = (d - DotProd(ray.p,n)) / rn;

		// Must be positive...
		if (a < 0.0f) continue;

		// Must be closer than the closest at so far
		if (first) {
			if (a > at) continue;
			}

		// The point on the ray and in the plane.
		p = ray.p + a*ray.dir;

		// Compute barycentric coords.
		bry = amesh->BaryCoords(i,p);

		// barycentric coordinates must sum to 1 and each component must
		// be in the range 0-1
		if (bry.x<0.0f || bry.x>1.0f || bry.y<0.0f || bry.y>1.0f || bry.z<0.0f || bry.z>1.0f) continue;
		if (fabs(bry.x + bry.y + bry.z - 1.0f) > EPSILON) continue;

		// Hit!
		first = TRUE;		
		at    = a;
		fi    = (DWORD)i;		
//		bary.x  = bry.z;
//		bary.y  = bry.x;
//		bary.z  = bry.y;
		bary  = bry;	// DS 3/8/97
		
		// Use interpolated normal instead.
		if (!face->smGroup) {
			norm  = n;
		} else {
			norm = 
				vnorms[face->v[0]].GetNormal(face->smGroup) * bary.x +
				vnorms[face->v[1]].GetNormal(face->smGroup) * bary.y +
				vnorms[face->v[2]].GetNormal(face->smGroup) * bary.z;
			norm = Normalize(norm);
			}
		}

	return first;
	}

void GetVFLst(Mesh* dmesh,Tab<VNormal>* vnorms,Tab<Point3>* fnorms)	 
{ int nv=dmesh->getNumVerts();	
  int nf=dmesh->getNumFaces();	
  (*fnorms).Resize(nf);
  (*fnorms).SetCount(nf);
  (*vnorms).Resize(nv);
  (*vnorms).SetCount(nv);
  Face *face = dmesh->faces;
  for (int i=0; i<nv; i++) 
    (*vnorms)[i] = VNormal();
  Point3 v0, v1, v2;
  for (i=0; i<dmesh->getNumFaces(); i++,face++) 
  {	// Calculate the surface normal
	v0 = dmesh->verts[face->v[0]];
	v1 = dmesh->verts[face->v[1]];
	v2 = dmesh->verts[face->v[2]];
	(*fnorms)[i] = (v1-v0)^(v2-v1);
	for (int j=0; j<3; j++) 
	   (*vnorms)[face->v[j]].AddNormal((*fnorms)[i],face->smGroup);
    (*fnorms)[i] = Normalize((*fnorms)[i]);
  }
  for (i=0; i<nv; i++) 
	(*vnorms)[i].Normalize();
}

BOOL GetV(Mesh* dmesh,DWORD vnum,Point3 *localpt)	 
{ int nv=dmesh->getNumVerts();	
  int nf=dmesh->getNumFaces();
  Point3 vnorms=Point3(0.0f,0.0f,0.0f);
  Face *face = dmesh->faces;
  Point3 fnorm;int vfound=0;
  Point3 v0, v1, v2;
  for (int i=0; i<dmesh->getNumFaces(); i++,face++) 
  if ((face->v[0]==vnum)||(face->v[1]==vnum)||(face->v[2]==vnum))
  {	// Calculate the surface normal
	v0 = dmesh->verts[face->v[0]];
	v1 = dmesh->verts[face->v[1]];
	v2 = dmesh->verts[face->v[2]];
	fnorm = (v1-v0)^(v2-v1);
	vnorms+=fnorm;
	vfound++;
  }
  if (!vfound) return FALSE;
  *localpt=vnorms/(float)vfound;
  return TRUE;
}

#define LOTSOFACES	2000

BOOL SurfWrapObject::UpdateMesh(TimeValue t,BOOL force,BOOL sel)
{	//Point3 Zero=Point3(0.0f,0.0f,0.0f);
	int vertexnum=0,face=0,dverts=0,dfaces=0,dtv,inrend=TestFlag(SWRAP_INRENDER); 
	if (!ivalid.InInterval(t)&&(TestFlag(SWRAP_UPDATEALWAYS) || 
		(TestFlag(SWRAP_UPDATERENDER)&&TestFlag(SWRAP_INRENDER)) ||	force)) 
	{	Point3 localpt;
		BOOL ismirrored = FALSE;
		int normtype;
		pblock->GetValue(PB_GETNORM,t,normtype,ivalid);
		if ((normtype!=1)||(mbase))
		{	ivalid = FOREVER;
			BOOL needsDel1, needsDel2;

			TriObject *towrapOb, *tob2;
			towrapOb = GetTriObject(t,ob1,ivalid,needsDel1);
			tob2 = GetTriObject(t,ob2,ivalid,needsDel2);
			Matrix3 towtm;
			towtm = GetOpTM(t,0,&ivalid);

			if (DotProd((towtm.GetRow(0)^towtm.GetRow(1)),towtm.GetRow(2)) < 0.0f)
				ismirrored = TRUE;

			if (towrapOb)
			{	ob1verts=towrapOb->GetMesh().getNumVerts();
				ob1faces=towrapOb->GetMesh().getNumFaces();
			} 
			else 
			{	ob1verts=0;
				ob1faces=0;
			}

			int infaces, intvs;
			int holdv0, holdv1, holdv2, holdvis0, holdvis1, holdvis2;
			DWORD onetvert, holdtverts[3];

			if (!(towrapOb && tob2))
			{	mesh.DeepCopy(&towrapOb->GetMesh(), OBJ_CHANNELS);//GEOM_CHANNEL|TOPO_CHANNEL|TEXMAP_CHANNEL|DISP_ATTRIB_CHANNEL|SELECT_CHANNEL|SUBSEL_TYPE_CHANNEL);
				for (int i=0;i<ob1verts;i++)
					mesh.verts[i]=mesh.verts[i]*towtm;
				if (needsDel1) 
					towrapOb->DeleteThis();
				if (ismirrored)	
				{	infaces = mesh.getNumFaces();
					for (int revface=0 ; revface < infaces ; revface++)
					{	holdv0 = mesh.faces[revface].v[0];
						holdv1 = mesh.faces[revface].v[1];
						holdv2 = mesh.faces[revface].v[2];
						holdvis0 = mesh.faces[revface].getEdgeVis(0);
						holdvis1 = mesh.faces[revface].getEdgeVis(2);
						holdvis2 = mesh.faces[revface].getEdgeVis(3);
						mesh.faces[revface].setEdgeVis(0,holdvis1);
						mesh.faces[revface].setEdgeVis(1,holdvis0);
						mesh.faces[revface].setVerts(holdv2, holdv1, holdv0);
						intvs = mesh.getNumTVerts();
						if (intvs)
						{	holdtverts[0] = mesh.tvFace[revface].getTVert(0);
							holdtverts[1] = mesh.tvFace[revface].getTVert(1);
							holdtverts[2] = mesh.tvFace[revface].getTVert(2);
							onetvert = holdtverts[0];
							holdtverts[0] = holdtverts[2];
							holdtverts[2] = onetvert;
							mesh.tvFace[revface].setTVerts(holdtverts);
						}
					}
				}
				mesh.InvalidateGeomCache(); 	
//				mesh.InvalidateTopologyCache();
//				mesh.BuildStripsAndEdges();
				return ivalid.InInterval(t);
			}
			int inverts,hdist;
			int totfaces,tottvs;
			Matrix3 tm;tm.IdentityMatrix();
			HCURSOR hCur;
			tm = GetOpTM(t,1,&ivalid);
			int origvs;
			origvs = dverts=tob2->GetMesh().getNumVerts();
			dfaces = tob2->GetMesh().getNumFaces();
			intvs = towrapOb->GetMesh().getNumTVerts();
			dtv = tob2->GetMesh().getNumTVerts();
			inverts = towrapOb->GetMesh().getNumVerts();
			infaces = towrapOb->GetMesh().getNumFaces();
			intvs = towrapOb->GetMesh().getNumTVerts();
			pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
			if (hdist)
			{	dverts=0;
				dfaces=0;
				dtv=-1;
			}
			mesh.setNumVerts(inverts+dverts);
			int inmap,dmap,totalmaps=nummaps(tob2,towrapOb,hdist,inmap,dmap);
			mesh.setNumMaps(totalmaps);
			if (infaces+dfaces > LOTSOFACES) 
				hCur = SetCursor(LoadCursor(NULL,IDC_WAIT));
			GetAsyncKeyState(VK_ESCAPE);  
			Matrix3 invtm=Inverse(tm);
			Ray ray;
			if (!normtype) 
				localpt=viewPt; 
			else if (normtype==1)
			{	Matrix3 ntm = mbase->GetObjectTM(t,&ivalid);
				localpt=ntm.GetRow(2);
				Matrix3 nooff=mainnode->GetObjectTM(t);
				nooff.NoTrans();localpt=localpt*Inverse(nooff);
			}
			int selverts;				
			float kdef,standoff;
			pblock->GetValue(PB_USESELVERTS,t,selverts,ivalid);
			pblock->GetValue(PB_KIDEFAULT,t,kdef,ivalid);
			pblock->GetValue(PB_STANDOFF,t,standoff,ivalid);
			Mesh *tmpmesh=new Mesh;
			tmpmesh->DeepCopy(&tob2->GetMesh(), GEOM_CHANNEL|TOPO_CHANNEL);
/*			mesh.selLevel=tob2->GetMesh().selLevel;
			mesh.dispFlags=tob2->GetMesh().dispFlags;
			mesh.faceSel=tob2->GetMesh().faceSel;
			mesh.faceSel=tob2->GetMesh().faceSel;
			mesh.edgeSel=tob2->GetMesh().edgeSel;
			mesh.vertHide=tob2->GetMesh().vertHide;*/

			for (int j=0;j<origvs;j++)
			{	tmpmesh->verts[j] = tmpmesh->verts[j]*tm;
			}
			Point3 wrapc;
			if ((normtype==3)||(normtype==5))
			{	wrapc=(normtype==3?towrapOb->GetMesh().getBoundingBox().Center():wrappivot);
				wrapc=wrapc*towtm;
			}
			else if (normtype>3)
			{	wrapc=(normtype==4?tob2->GetMesh().getBoundingBox().Center():topivot);
				wrapc=wrapc*tm;
			}
			Tab<VNormal> vnorms;
			Tab<Point3> fnorms;
			GetVFLst(tmpmesh,&vnorms,&fnorms);
			BOOL inface=TRUE;
			for (int i=0;i<inverts;i++)
			{	Point3 vert=towrapOb->GetMesh().verts[i]*towtm;
				if (normtype==2)
				{	inface=GetV(&towrapOb->GetMesh(),i,&localpt);
					localpt = Normalize(-localpt*towtm);
				}
				else if (normtype>2)
				{	localpt = Normalize(wrapc-vert);	
				}
				ray.dir=localpt;
				float *vssel = NULL;
				if (selverts) vssel = towrapOb->GetMesh().getVSelectionWeights ();
				if (inface&&((!selverts)||(towrapOb->GetMesh().vertSel[i])||(vssel&&vssel[i])))
				{	ray.p=vert;
					float at;Point3 norm;
					int kfound=RayIntersect(ray,at,norm,tmpmesh,vnorms,fnorms);
//					int kfound=tmpmesh->IntersectRay(ray,at,norm);
					if (!kfound) 
						vert+=kdef*localpt;
					else 
					{ 	Point3 intersect=ray.p+ray.dir*at;
						intersect=vert-intersect;
						vert=vert+localpt*(Length(intersect)-standoff);
					}
				}			
				mesh.verts[i]=vert;
			}
			vnorms.SetCount(0);vnorms.Shrink();
			fnorms.SetCount(0);fnorms.Shrink();
			mesh.setNumFaces(totfaces=(infaces+dfaces));
			tottvs=intvs+(dtv>0?dtv:0);
			BOOL tvs=((intvs>0)||(dtv>0));
			if ((tvs)&&((intvs==0)||(dtv==0))) 
				tottvs++;
			mesh.setNumTVerts(tottvs);
			mesh.setNumTVFaces(tvs?totfaces:0);
			if (totalmaps>2) mapstuff(&mesh,tob2,towrapOb,inmap,dmap);
			for (i=0;i<infaces;i++)
			{	mesh.faces[i]=towrapOb->GetMesh().faces[i];
				if (firstmtls==1) 
					mesh.faces[i].setMatID(0);
				if (tvs) 
				{	if (intvs>0) 
						mesh.tvFace[i] = towrapOb->GetMesh().tvFace[i];
					else 
						mesh.tvFace[i].setTVerts(dtv,dtv,dtv);
				}
			}
			face = infaces;
			int addvert=inverts;
			for (i=0;i<intvs;i++)
				mesh.tVerts[i]=towrapOb->GetMesh().tVerts[i];
			if (tvs)
			{	if (intvs==0) 
					mesh.tVerts[dtv]=Zero;
				else if (dtv==0) 
					mesh.tVerts[intvs]=Zero;
			} 
			for (i=0;i<dtv;i++)
				mesh.tVerts[i+intvs]=tob2->GetMesh().tVerts[i];
			vertexnum=inverts;
			if (tob2 &&(!hdist))
			{	for (i=0;i<dverts;i++)
					for (i=0;i<dverts;i++)
						mesh.verts[vertexnum++]=tmpmesh->verts[i];
						for (i=0;i<dfaces;i++)
						{	memcpy(&mesh.faces[face],&tob2->GetMesh().faces[i],sizeof(Face));
							mesh.faces[face].v[0]+=addvert;
							mesh.faces[face].v[1]+=addvert;
							mesh.faces[face].v[2]+=addvert;
							mesh.faces[face].setMatID(firstmtls+(secondmtls==1?0:tob2->GetMesh().faces[i].getMatID()));		
							if (tvs) 
							{	if (dtv>0) 
									mesh.tvFace[face].setTVerts(tob2->GetMesh().tvFace[i].t[0]+intvs,tob2->GetMesh().tvFace[i].t[1]+intvs,tob2->GetMesh().tvFace[i].t[2]+intvs);
								else 
									mesh.tvFace[face].setTVerts(intvs,intvs,intvs);
							}
							face++;
						}
			}
			if (tmpmesh) 
				delete tmpmesh;
			if (needsDel1)
				towrapOb->DeleteThis();
			if (needsDel2)
				tob2->DeleteThis();
// here -->
			if (ismirrored)	
			{	infaces = mesh.getNumFaces();
				for (int revface=0 ; revface < infaces ; revface++)
				{	holdv0 = mesh.faces[revface].v[0];
					holdv1 = mesh.faces[revface].v[1];
					holdv2 = mesh.faces[revface].v[2];
					holdvis0 = mesh.faces[revface].getEdgeVis(0);
					holdvis1 = mesh.faces[revface].getEdgeVis(2);
					holdvis2 = mesh.faces[revface].getEdgeVis(3);
					mesh.faces[revface].setEdgeVis(0,holdvis1);
					mesh.faces[revface].setEdgeVis(1,holdvis0);
					mesh.faces[revface].setVerts(holdv2, holdv1, holdv0);
					intvs = mesh.getNumTVerts();
					if (intvs)
					{	holdtverts[0] = mesh.tvFace[revface].getTVert(0);
						holdtverts[1] = mesh.tvFace[revface].getTVert(1);
						holdtverts[2] = mesh.tvFace[revface].getTVert(2);
						onetvert = holdtverts[0];
						holdtverts[0] = holdtverts[2];
						holdtverts[2] = onetvert;
						mesh.tvFace[revface].setTVerts(holdtverts);
					}
				}
			}

			mesh.InvalidateGeomCache(); 	
//			mesh.InvalidateTopologyCache();
//			mesh.BuildStripsAndEdges();
		} 
	}
	else 
	{	if (!ivalid.InInterval(t)) 
		{	ivalid.SetInstant(t); 
		}
	}
	return ivalid.InInterval(t);
}

Interval SurfWrapObject::ObjectValidity(TimeValue t)
	{ 
	UpdateMesh(t);
	if (ivalid.Empty()) return Interval(t,t);
	else return ivalid;
	}

int SurfWrapObject::CanConvertToType(Class_ID obtype)
	{
	if (obtype==defObjectClassID||obtype==triObjectClassID||obtype==mapObjectClassID) {
		if (ob1 || ob2) return 1;
		else return 0;
	}
	return Object::CanConvertToType(obtype);
/*	if (obtype==defObjectClassID||obtype==triObjectClassID||obtype==mapObjectClassID) {
		if (ob1 && ob2) return 1;
		else if (ob1) return ob1->CanConvertToType(obtype);
		else if (ob2) return ob2->CanConvertToType(obtype);
		else return 0;
		}*/
	return Object::CanConvertToType(obtype);
	}

Object* SurfWrapObject::ConvertToType(TimeValue t, Class_ID obtype)
	{
	if (obtype==defObjectClassID||obtype==triObjectClassID||obtype==mapObjectClassID) {
		// Since PrepForBoolean changes the mesh, we should use UpdateMesh to produce new
		// output even when there is only 1 operand.
		TriObject *triob;
		UpdateMesh(t);
		triob = CreateNewTriObject();
		triob->GetMesh() = mesh;
		triob->SetChannelValidity(TOPO_CHAN_NUM,ObjectValidity(t));
		triob->SetChannelValidity(GEOM_CHAN_NUM,ObjectValidity(t));
		return triob;
	} else {
		return Object::ConvertToType(t,obtype);
	}
}

ObjectState SurfWrapObject::Eval(TimeValue time)
	{
	return ObjectState(this);
	}



BOOL SWPickOperand::Filter(INode *node)
	{
	if (node) {
		ObjectState os = node->GetObjectRef()->Eval(bo->ip->GetTime());
		if (os.obj->IsParticleSystem() || 
			os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
			node = NULL;
			return FALSE;
			}

		node->BeginDependencyTest();
		bo->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
		if(node->EndDependencyTest()) {
			node = NULL;
			return FALSE;
			}		
		}

	return node ? TRUE : FALSE;
	}

BOOL SWPickOperand::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	INode *node = ip->PickNode(hWnd,m,this);
	
	if (node) {
	  Interval intvalid;
	  BOOL needsDel;
	  Object *tob=node->GetObjectRef();
	  TriObject *tri=GetTriObject(ip->GetTime(),tob,intvalid,needsDel);
	  if (tri==NULL) { node = NULL; return FALSE;	}
	    if (needsDel) tri->DeleteThis();
		node->BeginDependencyTest();
		bo->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
		if(node->EndDependencyTest()) {
			node = NULL;
			return FALSE;
			}		
		}

	return node ? TRUE : FALSE;
	}

BOOL SWPickOperand::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	assert(node);
	int stkclr=IDYES;
	if ((bo->mainnode)&&((bo->mainnode->GetMtl() && node->GetMtl())&& (bo->mainnode->GetMtl()!=node->GetMtl())) )
	{ TSTR buf(GetString(IDS_AP_MTLMSG));
	  TSTR buf1(GetString(IDS_AP_MTLTITLE));
	  stkclr=MessageBox(NULL,buf,buf1,MB_ICONQUESTION|MB_YESNO);
	}
	if (stkclr==IDNO) return FALSE;
	INodeTab nodes;
	nodes.SetCount(1);nodes[0]=node;
	ip->FlashNodes(&nodes);
	Matrix3 ourTM;
	Matrix3 ntm = node->GetObjectTM(ip->GetTime());	
	theHold.Begin();
	theHold.Put(new CreateWraptoref(bo,bo->opbName,node->GetName()));
	BOOL matRet = TRUE;

	if (bo->creating) {
		ourTM = bo->createTM;
	} else {
		if (bo->mainnode) {
			ourTM = bo->mainnode->GetObjectTM(ip->GetTime());
		} else {
			MaxAssert(0);
			ourTM.IdentityMatrix();
			}
		}

/*	if (bo->mainnode) {
		if (bo->mainnode->GetMtl() && node->GetMtl() && (bo->mainnode->GetMtl()!=node->GetMtl())) {
			matRet = bo->DoAttachMatOptionDialog (bo->pmapDist->GetHWnd());
			bo->inheritMat = FALSE;
		}
		if (!bo->mainnode->GetMtl() && node->GetMtl()) {
			matRet = bo->DoInheritMatOptionDialog (bo->pmapDist->GetHWnd());
		}
	}

	if (!matRet) return FALSE;

	theHold.Begin();
	bo->SetOperandB (ip->GetTime(), node, bo->mainnode, bo->addOppMethod,
		bo->inheritMat ? BOOL_MAT_DISCARD_ORIG : bo->attachMat, &canUndo);
	bo->ReleaseMyNode();
	theHold.Accept(GetString(IDS_PICK_OPERAND));
	if (!canUndo) GetSystemSetting (SYSSET_CLEAR_UNDO);
*/	
	bool canUndo = TRUE;
	bo->SetOperand(node,ourTM,ntm,1,bo->mainnode,&canUndo);	
	if (!canUndo) GetSystemSetting (SYSSET_CLEAR_UNDO);
	GenerateViewRay(&bo->viewPt,bo->ip);
	bo->SetFlag(SWRAP_DISPRESULT);
	CheckRadioButton(bo->pmapObjects->GetHWnd(),IDC_SW_DISPRESULT,IDC_SW_DISPOPS,IDC_SW_DISPRESULT);
	bo->ivalid.SetEmpty();
	bo->UpdateMesh(ip->GetTime(),TRUE);
	
	if (bo->creating) {
		theCreateSWrapMode.JumpStart(ip,bo);
		ip->SetCommandMode(&theCreateSWrapMode);
		ip->RedrawViews(ip->GetTime());
		return FALSE;
	} else {
		return TRUE;
		}
	}

void SWPickOperand::EnterMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapDist->GetHWnd(),IDC_AP_SURFWRAP_OBJECTPICK));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
	}

void SWPickOperand::ExitMode(IObjParam *ip)
	{
	if (bo->pmapDist)
	{ ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapDist->GetHWnd(),IDC_AP_SURFWRAP_OBJECTPICK));
	  if (iBut) iBut->SetCheck(FALSE);
	  ReleaseICustButton(iBut);
	}
	}

BOOL SWPickNorm::Filter(INode *node)
	{
	return node ? TRUE : FALSE;
	}

BOOL SWPickNorm::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	INode *node = ip->PickNode(hWnd,m,this);
	
	return node ? TRUE : FALSE;
	}

BOOL SWPickNorm::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	assert(node);
	INodeTab nodes;
	nodes.SetCount(1);nodes[0]=node;
	ip->FlashNodes(&nodes);
	if (bo->mbase) bo->ReplaceReference(MBASE,node,TRUE);
	else bo->MakeRefByID(FOREVER,MBASE,node);
	bo->normname = TSTR(node->GetName());
	bo->ShowNormName();
	bo->ivalid.SetEmpty();
	bo->UpdateMesh(ip->GetTime(),TRUE);
	
	if (bo->creating) {
		theCreateSWrapMode.JumpStart(ip,bo);
		ip->SetCommandMode(&theCreateSWrapMode);
		ip->RedrawViews(ip->GetTime());
		return FALSE;
	} else {
		return TRUE;
		}
	}

void SWPickNorm::EnterMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapObjects->GetHWnd(),IDC_AP_SURFWRAP_OBJECTDPICK3));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
	}

void SWPickNorm::ExitMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapObjects->GetHWnd(),IDC_AP_SURFWRAP_OBJECTDPICK3));
	if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);
	}

class SurfWrapPickObj : public ParamMapUserDlgProc {
	public:
		SurfWrapObject *po;

		SurfWrapPickObj(SurfWrapObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {delete this;}
		void Update(TimeValue t);
	};
void SurfWrapPickObj::Update(TimeValue t)
{	if (!po->editOb) return;
    po->ShowName(po->ob2!=NULL);
}

BOOL SurfWrapPickObj::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	switch (msg) {
		case WM_INITDIALOG: {
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_SURFWRAP_OBJECTPICK));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
		    ReleaseICustButton(iBut);
			po->ShowName(po->ob2!=NULL);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ case IDC_AP_SURFWRAP_OBJECTPICK:
				   { if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
					{ if (po->creating) 
						{  theCreateSWrapMode.JumpStart(po->ip,po);
							po->ip->SetCommandMode(&theCreateSWrapMode);
						} else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
					} else 
						{ po->pickCB.bo = po;						
						  po->ip->SetPickMode(&po->pickCB);
						}
					break;
				}
			}
			break;	
		}
	return TRUE;
}

class SurfWrapObjList : public ParamMapUserDlgProc {
	public:
		SurfWrapObject *po;

		SurfWrapObjList(SurfWrapObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void Update(TimeValue t);
		void DeleteThis() {delete this;}
	};

void CalcButton(HWND hWnd,BOOL ON)
{	ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_SWRAP_RECALC));
	if (ON)  iBut->Enable();
	 else iBut->Disable();
	ReleaseICustButton(iBut);
}
void ViewNormButton(HWND hWnd,int whichon)
{	ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_SURFWRAP_OBJECTDPICK2));
	ICustButton *iBut2 = GetICustButton(GetDlgItem(hWnd,IDC_AP_SURFWRAP_OBJECTDPICK3));
	if (!whichon)  
	{ iBut->Enable();  
	  iBut2->Disable();  
	  EnableWindow(GetDlgItem(hWnd,IDC_AP_SURFWRAP_NORMOBJ),FALSE);
	}
	else if (whichon==1)
	{ iBut->Disable();
	  iBut2->Enable();  
	  EnableWindow(GetDlgItem(hWnd,IDC_AP_SURFWRAP_NORMOBJ),TRUE);
	}
	else
	{ iBut->Disable();
	  iBut2->Disable();
	  EnableWindow(GetDlgItem(hWnd,IDC_AP_SURFWRAP_NORMOBJ),FALSE);
	}
	ReleaseICustButton(iBut);
	ReleaseICustButton(iBut2);
}
void SurfWrapObjList::Update(TimeValue t)
{	if (!po->editOb) return;
	HWND hWnd=po->pmapObjects->GetHWnd();
	po->ShowOpList(po->ob2!=NULL);
	CheckDlgButton(hWnd,IDC_SW_DISPRESULT,po->TestFlag(SWRAP_DISPRESULT));
	CheckDlgButton(hWnd,IDC_SW_DISPOPS,!po->TestFlag(SWRAP_DISPRESULT));	
	if (po->TestFlag(SWRAP_UPDATERENDER)) CheckDlgButton(hWnd,IDC_SWRAP_UPDATERENDER,TRUE);
	else CheckDlgButton(hWnd,IDC_SWRAP_UPDATERENDER,FALSE);
	if (po->TestFlag(SWRAP_UPDATEMANUAL)) CheckDlgButton(hWnd,IDC_SWRAP_UPDATEMANUAL,TRUE);
	else CheckDlgButton(hWnd,IDC_SWRAP_UPDATEMANUAL,FALSE);
	if (po->TestFlag(SWRAP_UPDATEALWAYS)) CheckDlgButton(hWnd,IDC_SWRAP_UPDATEALWAYS,TRUE);
	else CheckDlgButton(hWnd,IDC_SWRAP_UPDATEALWAYS,FALSE);
//	if (po->TestFlag(SWRAP_UPDATESELECT)) CheckDlgButton(hWnd,IDC_SWRAP_UPDATESELECT,TRUE);
//	else CheckDlgButton(hWnd,IDC_SWRAP_UPDATESELECT,FALSE);	
	ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_SWRAP_RECALC));
	CalcButton(hWnd,po->TestFlag(SWRAP_UPDATEMANUAL));
	ReleaseICustButton(iBut);		
	iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_SURFWRAP_OBJECTDPICK3));
	iBut->SetType(CBT_CHECK);
	iBut->SetHighlightColor(GREEN_WASH);
    ReleaseICustButton(iBut);
	po->ShowNormName();
	int tview;
	po->pblock->GetValue(PB_GETNORM,0,tview,FOREVER);
	ViewNormButton(hWnd,(po->dlgNormby=tview));
}

BOOL SurfWrapObjList::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	switch (msg) {
		case WM_INITDIALOG: {
			Update(t);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_CUSTEDIT_ENTER: {
			ICustEdit *edit;
			TCHAR buf[256];
			
			switch (LOWORD(wParam)) {
				case IDC_SWRAP_ANAME:
					edit = GetICustEdit(GetDlgItem(hWnd,IDC_SWRAP_ANAME));					
					edit->GetText(buf,256);
					po->opaName = TSTR(buf);					
					if (po->ob1) po->ob1->NotifyDependents(FOREVER,PART_ALL,REFMSG_NODE_NAMECHANGE,TREE_VIEW_CLASS_ID);
					break;
					
				case IDC_SWRAP_BNAME:
					edit = GetICustEdit(GetDlgItem(hWnd,IDC_SWRAP_BNAME));					
					edit->GetText(buf,256);
					po->opbName = TSTR(buf);
					if (po->ob2) po->ob2->NotifyDependents(FOREVER,PART_ALL,REFMSG_NODE_NAMECHANGE,TREE_VIEW_CLASS_ID);
					break;					
				}
			
			po->ShowOpList(po->ob2!=NULL);
			break;
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ case IDC_SWRAP_OPERANDS:
					if (HIWORD(wParam)==LBN_SELCHANGE) 
					{	po->flags &= ~SWRAP_ANYSEL;
						
						if (SendMessage((HWND)lParam,LB_GETSEL,0,0)) 
						{ po->flags |= SWRAP_OB1SEL;	}
							
						if (SendMessage((HWND)lParam,LB_GETSEL,1,0))
						{ po->flags |= SWRAP_OB2SEL;	}
						po->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
						po->NotifyDependents(FOREVER,0,REFMSG_BRANCHED_HISTORY_CHANGED);
						po->ip->RedrawViews(po->ip->GetTime());
					}
					break;
				case IDC_SW_DISPOPS:
				case IDC_SW_DISPRESULT:
					if (IsDlgButtonChecked(hWnd,IDC_SW_DISPRESULT)) {						
						po->SetFlag(SWRAP_DISPRESULT);
					} else {
						po->ClearFlag(SWRAP_DISPRESULT);
						}
					po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					po->ip->RedrawViews(po->ip->GetTime());
					break;
			  case IDC_AP_VIEWDIR:
				{ ViewNormButton(hWnd,(po->dlgNormby=0));
				  break;
				}
			  case IDC_AP_OBJECTDIR:
				{ ViewNormButton(hWnd,(po->dlgNormby=1));
				  break;
				}
			  case IDC_AP_VERTEXDIR:
				{ ViewNormButton(hWnd,(po->dlgNormby=2));
				  break;
				}
			  case IDC_AP_WRAPPERCTRDIR:
				{ ViewNormButton(hWnd,(po->dlgNormby=3));
				  break;
				}
			  case IDC_AP_WRAP2CTRDIR:
				{ ViewNormButton(hWnd,(po->dlgNormby=4));
				  break;
				}
			  case IDC_AP_SURFWRAP_OBJECTDPICK2:
				  {	Point3 oldpt=po->viewPt;
					GenerateViewRay(&po->viewPt,po->ip);
					if (!(oldpt==po->viewPt))
					{ po->ivalid.SetEmpty();
					  po->UpdateMesh(t);
					  po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					  po->ip->RedrawViews(t);
					}
				  }
					break;
			  case IDC_AP_SURFWRAP_OBJECTDPICK3:
				    if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
					{ if (po->creating) 
						{  theCreateSWrapMode.JumpStart(po->ip,po);
							po->ip->SetCommandMode(&theCreateSWrapMode);
						} else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
					} else 
						{ po->pickNorm.bo = po;						
						  po->ip->SetPickMode(&po->pickNorm);
						}
					break;

				case IDC_SWRAP_RECALC:
					po->ivalid.SetEmpty();
					po->UpdateMesh(t,TRUE);
					po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					po->ip->RedrawViews(t);
					break;

				case IDC_SWRAP_UPDATEALWAYS:
					po->ClearFlag(SWRAP_UPDATEALWAYS|SWRAP_UPDATERENDER|SWRAP_UPDATEMANUAL);
					po->SetFlag(SWRAP_UPDATEALWAYS);					
					po->ip->RedrawViews(po->ip->GetTime());
					CalcButton(hWnd,FALSE);
					break;

/*				case IDC_SWRAP_UPDATESELECT:
					po->ClearFlag(SWRAP_UPDATEALWAYS|SWRAP_UPDATERENDER|SWRAP_UPDATEMANUAL|SWRAP_UPDATESELECT);
					po->SetFlag(SWRAP_UPDATESELECT);					
					po->ip->RedrawViews(po->ip->GetTime());
					CalcButton(hWnd,FALSE);
					break;	*/			

				case IDC_SWRAP_UPDATERENDER:
					po->ClearFlag(SWRAP_UPDATEALWAYS|SWRAP_UPDATERENDER|SWRAP_UPDATEMANUAL);
					po->SetFlag(SWRAP_UPDATERENDER);					
					CalcButton(hWnd,FALSE);
					break;

				case IDC_SWRAP_UPDATEMANUAL:
					po->ClearFlag(SWRAP_UPDATEALWAYS|SWRAP_UPDATERENDER|SWRAP_UPDATEMANUAL);
					po->SetFlag(SWRAP_UPDATEMANUAL);					
					CalcButton(hWnd,TRUE);
					break;
			}
			break;	
		default:
			return FALSE;
		}
	return TRUE;
}

void SurfWrapObject::InvalidateUI()
	{
	if (pmapDist) pmapDist->Invalidate();
	if (pmapObjects) pmapObjects->Invalidate();
	}

void SurfWrapObject::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{	
	this->ip = ip;	
	editOb   = this;

	if (flags&BEGIN_EDIT_CREATE) {
		creating = TRUE;
	} else {
		creating = FALSE;
		// Create sub object editing modes.
		moveMode       = new MoveModBoxCMode(this,ip);
		rotMode        = new RotateModBoxCMode(this,ip);
		uscaleMode     = new UScaleModBoxCMode(this,ip);
		nuscaleMode    = new NUScaleModBoxCMode(this,ip);
		squashMode     = new SquashModBoxCMode(this,ip);
		selectMode     = new SelectModBoxCMode(this,ip);
		
		// Add our sub object type
		TSTR type(GetString(IDS_AP_OPERANDS));
		const TCHAR *ptype[] = {type};
		ip->RegisterSubObjectTypes(ptype, 1);
		}
	if (pmapObjects && pmapDist)
	{	pmapObjects->SetParamBlock(pblock);
		pmapDist->SetParamBlock(pblock);
	}
	else 
	{  pmapDist = CreateCPParamMap(
				descPickDist,PICKDISTDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_SURFWRAP_PICKWRAPTO),
				GetString(IDS_AP_PICKWRAPTOOBJ),
				(creating?0:APPENDROLL_CLOSED));
		pmapObjects = CreateCPParamMap(
				descObjects,OBJECTSDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_SURFWRAP_OBJECTS),
				GetString(IDS_AP_PARAMETERS),
				0);
  if (pmapObjects) pmapObjects->SetUserDlgProc(new SurfWrapObjList(this));
  if (pmapDist) pmapDist->SetUserDlgProc(new SurfWrapPickObj(this));
	} 
}

void SurfWrapObject::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
	{	
	editOb = NULL;

	if (flags&END_EDIT_REMOVEUI ) {
		if (pmapObjects) DestroyCPParamMap(pmapObjects);
		pmapObjects  = NULL;
		if (pmapDist) DestroyCPParamMap(pmapDist);
		pmapDist  = NULL;
		}

	if (!creating) {
		ip->DeleteMode(moveMode);
		ip->DeleteMode(rotMode);
		ip->DeleteMode(uscaleMode);
		ip->DeleteMode(nuscaleMode);
		ip->DeleteMode(squashMode);
		ip->DeleteMode(selectMode);
		if ( moveMode ) delete moveMode;
		moveMode = NULL;
		if ( rotMode ) delete rotMode;
		rotMode = NULL;
		if ( uscaleMode ) delete uscaleMode;
		uscaleMode = NULL;
		if ( nuscaleMode ) delete nuscaleMode;
		nuscaleMode = NULL;
		if ( squashMode ) delete squashMode;
		squashMode = NULL;
		if ( selectMode ) delete selectMode;
		selectMode = NULL;	
	}
	ip->ClearPickMode();
	ip = NULL;
	creating = FALSE;
	}

int SurfWrapObject::NumPipeBranches() 
	{
	int num=0;
	if (TestFlag(SWRAP_OB1SEL) && ob1) num++;
	if (TestFlag(SWRAP_OB2SEL) && ob2) num++;
	return num;
	}

Object *SurfWrapObject::GetPipeBranch(int i) 
	{
	if (i) return ob2;	
	if (TestFlag(SWRAP_OB1SEL)) return ob1;
	return ob2;
	}

INode *SurfWrapObject::GetBranchINode(TimeValue t,INode *node,int i)
	{
	assert(i<2);
	int index = 0;
	if (i) index = 1;
	else if (TestFlag(SWRAP_OB1SEL)) index = 0;
	else index = 1;
	return CreateINodeTransformed(node,GetOpTM(t,index));
	}

int SurfWrapObject::NumSubs()
{ 	return 6; 	}

Animatable* SurfWrapObject::SubAnim(int i)
{ 	switch (i) {
		case 0:  return ob1;
		case 1:  return tm1;
		case 2:	 return ob2;
		case 3:	 return tm2;
		case 4:	 return pblock;
		case 5:  return mbase;
		default: return NULL;
		}	
}

TSTR SurfWrapObject::SubAnimName(int i)
	{	
	switch (i) {
		case 0: return opaName;//return GetString(IDS_AP_OPERANDA);
		case 1: return GetString(IDS_AP_OPERANDATRANSFORM);
		case 2: return opbName;//return GetString(IDS_AP_OPERANDB);
		case 3: return GetString(IDS_AP_OPERANDBTRANSFORM);
		case 4: return GetString(IDS_AP_PARAMETERS);
		case 5: return GetString(IDS_AP_MBASE);
		}
	return _T("Error");
	}

int SurfWrapObject::SubNumToRefNum(int subNum)
	{
	switch (subNum) {
		case 0:  return REF_OP1;
		case 1:  return REF_CONT1;
		case 2:	 return REF_OP2;
		case 3:	 return REF_CONT2;
		case 4:	 return PBLOCK;
		case 5:	 return MBASE;
		default: return -1;
		}	
	}

RefTargetHandle SurfWrapObject::GetReference(int i)
	{
	switch (i) {
		case REF_OP1: 	return ob1;
		case REF_OP2: 	return ob2;
		case REF_CONT1:	return tm1;
		case REF_CONT2:	return tm2;
		case PBLOCK:	return pblock;
		case MBASE:		return mbase;
		default:        return NULL;
		}
	}
void SurfWrapObject::ShowNormName()
{
TSTR name=TSTR(GetString(IDS_AP_OBJECTSTR)) + (mbase ? normname : TSTR(GetString(IDS_AP_NONE)));
SetWindowText(GetDlgItem(pmapObjects->GetHWnd(), IDC_AP_SURFWRAP_NORMOBJ), name);
}
void SurfWrapObject::ShowName(BOOL ob2)
{if (pmapDist)
{TSTR name=TSTR(GetString(IDS_AP_OBJECTSTR)) + (ob2 ? opbName : TSTR(GetString(IDS_AP_NONE)));
SetWindowText(GetDlgItem(pmapDist->GetHWnd(), IDC_AP_SURFWRAP_WRAPTOOBJ), name);}
}
void SurfWrapObject::ShowOpList(BOOL isobj)
{	if (pmapObjects)
	{ShowName(isobj); 
	HWND hList = GetDlgItem(pmapObjects->GetHWnd(),IDC_SWRAP_OPERANDS);
	SendMessage(hList,LB_RESETCONTENT,0,0);
	TSTR name = TSTR(GetString(IDS_AP_SWRAPTITLE))+ (ob1 ? opaName : TSTR(_T("")));
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name);
	name = TSTR(GetString(IDS_AP_SWRAPTOTITLE))+ (isobj ? opbName : TSTR(_T("")));
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name);
	if (flags&SWRAP_OB1SEL) {SendMessage(hList,LB_SETSEL,TRUE,0); }
	if (flags&SWRAP_OB2SEL) {SendMessage(hList,LB_SETSEL,TRUE,1); }
	ICustEdit *edit = GetICustEdit(GetDlgItem(pmapObjects->GetHWnd(),IDC_SWRAP_ANAME));
	edit->SetText(opaName);
	ReleaseICustEdit(edit);

	edit = GetICustEdit(GetDlgItem(pmapObjects->GetHWnd(),IDC_SWRAP_BNAME));
	edit->SetText(opbName);
	ReleaseICustEdit(edit);
}
}

void SurfWrapObject::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case REF_OP1: 	 
			ob1 = (Object*)rtarg;
/*			if (rtarg==NULL) {				
				if (editOb==this) {
					ShowOpList(ob2!=NULL);
					}
			}*/
			break;

		case REF_OP2: 	 
			ob2 = (Object*)rtarg;  
/*			if (rtarg==NULL) {
				if (editOb==this) {
					ShowOpList(ob2!=NULL);
					}
				}*/
			break;

		case REF_CONT1:	 tm1 = (Control*)rtarg; break;
		case REF_CONT2:	 tm2 = (Control*)rtarg; break;
		case PBLOCK:	 pblock=(IParamBlock*)rtarg; break;
		case MBASE:		 mbase=(INode *)rtarg; break;
		}
	}

RefTargetHandle SurfWrapObject::Clone(RemapDir& remap)
	{
	SurfWrapObject *obj = new SurfWrapObject;
	if (pblock) obj->ReplaceReference(PBLOCK,pblock->Clone(remap)); 
	if (ob1) obj->ReplaceReference(REF_OP1,remap.CloneRef(ob1));
	if (ob2) obj->ReplaceReference(REF_OP2,remap.CloneRef(ob2));
	if (tm1) obj->ReplaceReference(REF_CONT1,remap.CloneRef(tm1));
	if (tm2) obj->ReplaceReference(REF_CONT2,remap.CloneRef(tm2));
	if (mbase) obj->ReplaceReference(MBASE,mbase); 
	obj->flags = flags;
	obj->opaName = opaName;
	obj->opbName = opbName;
	obj->ivalid.SetEmpty();
	return obj;
	}

int SurfWrapObject::IntersectRay(
		TimeValue t, Ray& r, float& at, Point3& norm)
	{
	if (TestFlag(SWRAP_DISPRESULT)) {
		UpdateMesh(t);
		return mesh.IntersectRay(r,at,norm);
	} else {
		return 0;
		}
	}

Mesh* SurfWrapObject::GetRenderMesh(
		TimeValue t, INode *inode, View& view, BOOL& needDelete)
	{	
	UpdateMesh(t);
	needDelete = FALSE;
	return &mesh;	
	}

int SurfWrapObject::HitTest(
		TimeValue t, INode* inode, int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt)
	{
	int res = 0;
	BOOL issel=inode->Selected();
	if (ob1 && ob2&& TestFlag(SWRAP_DISPRESULT)) {
		UpdateMesh(t,FALSE,inode->Selected());
		HitRegion hitRegion;
		GraphicsWindow *gw = vpt->getGW();	
		Material *mtl = gw->getMaterial();		
		gw->setTransform(inode->GetObjectTM(t));
		MakeHitRegion(hitRegion, type, crossing, 4, p);
		res = mesh.select(gw, mtl, &hitRegion, flags & HIT_ABORTONHIT);
		if (res) return res;
	} else {
		Object *ob;
		if (ob=GetPipeObj(t,0)) {
			INodeTransformed n(inode,GetOpTM(t,0));
			res = ob->HitTest(t,&n,type,crossing,flags,p,vpt);
			if (res) return res;
			}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	INodeTransformed n(inode,GetOpTM(t,1));
			res = ob->HitTest(t,&n,type,crossing,flags,p,vpt);
			if (res) return res;
			}
		}
	return res;
	}

void SurfWrapObject::Snap(
		TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
	{
	if (TestFlag(SWRAP_DISPRESULT)&&ob1 && ob2) {
		UpdateMesh(t,FALSE,inode->Selected());
		Matrix3 tm = inode->GetObjectTM(t);	
		GraphicsWindow *gw = vpt->getGW();		
		gw->setTransform(tm);
		mesh.snap( gw, snap, p, tm );
	} else {	
		Object *ob;
		if (ob=GetPipeObj(t,0)) {
			INodeTransformed n(inode,GetOpTM(t,0));
			ob->Snap(t,&n,snap,p,vpt);
			}	
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	INodeTransformed n(inode,GetOpTM(t,1));
			ob->Snap(t,&n,snap,p,vpt);
			}
		}
	}

#define DRAW_A (1<<1)
#define DRAW_B (1<<2)

int SurfWrapObject::
		Display(TimeValue t, INode* inode, ViewExp *vpt, int aflags)
{	GraphicsWindow *gw = vpt->getGW();

	Object *ob;
		
	DWORD rlim = gw->getRndLimits();

/*	Interval intvalid;
	BOOL needsDel;
	TriObject *tri;*/
	if (ob=GetPipeObj(t,0)) {
	  if (TestFlag(SWRAP_DISPRESULT))
	  { UpdateMesh(t,FALSE,inode->Selected());
		Matrix3 mat;
		mat=inode->GetObjectTM(t);
		gw->setTransform(mat);
		mesh.render(gw, inode->Mtls(),(flags&USE_DAMAGE_RECT)?&vpt->GetDammageRect():NULL, COMP_ALL, inode->NumMtls());
/*		 if (inode->Selected()) {
				 if (TestFlag(SWRAP_OB1SEL)) {
					vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
				 } else {
					Point3 selClr = GetUIColor(COLOR_SELECTION); 
					vpt->getGW()->setColor( LINE_COLOR, selClr.x, selClr.y, selClr.z);
				 	}
				}
		TriObject t1;
		t1.GetMesh().DeepCopy(&mesh,PART_GEOM|PART_TOPO);//|SUBSEL_TYPE_CHANNEL|DISP_ATTRIB_CHANNEL);
		t1.GetMesh().setNumVerts(ob1verts,TRUE);t1.GetMesh().setNumFaces(ob1faces,TRUE);
//		t1.mesh.DeepCopy(&tri->mesh,SUBSEL_TYPE_CHANNEL|DISP_ATTRIB_CHANNEL);
//		t1.mesh.dispFlags=tri->mesh.dispFlags;COMP_ALL| ((aflags&DISP_SHOWSUBOBJECT)?COMP_OBJSELECTED:0)
		t1.GetMesh().render(gw, inode->Mtls(),(aflags&USE_DAMAGE_RECT)?&vpt->GetDammageRect():NULL, COMP_ALL, inode->NumMtls());				 */
	  }
	  else
		{	INodeTransformed n(inode,GetOpTM(t,0));
			if (inode->Selected()) 
			{ if (TestFlag(SWRAP_OB1SEL)) 
				vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
			  else vpt->getGW()->setColor( LINE_COLOR, GetSelColor());
			}
			ob->Display(t,&n,vpt,aflags);
//	    if (needsDel) tri->DeleteThis(); 
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	INodeTransformed n(inode,GetOpTM(t,1));
			if (inode->Selected()) 
			{ if (TestFlag(SWRAP_OB2SEL)) 
				vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
			  else vpt->getGW()->setColor( LINE_COLOR, GetSelColor());
			}
			ob->Display(t,&n,vpt,flags);
		}
		}
	}
		
	gw->setRndLimits(rlim);
	return 0;
}

void SurfWrapObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel)
{
	Box3 abox;
	abox.Init();
	box.Init();
	if (TestFlag(SWRAP_DISPRESULT) && ob1 && ob2) {
		UpdateMesh(t);
		box = mesh.getBoundingBox(tm);
	} else {
		Object *ob;
		if (ob=GetPipeObj(t,0)) {				
			if (tm) {
				Matrix3 mat = GetOpTM(t,0) * *tm;
				ob->GetDeformBBox(t,abox,&mat,useSel);
			} else ob->GetDeformBBox(t,abox,NULL,useSel);
			box += abox;
			}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	if (tm) {
				Matrix3 mat = GetOpTM(t,1) * *tm;
				ob->GetDeformBBox(t,abox,&mat,useSel);
			} else ob->GetDeformBBox(t,abox,NULL,useSel);
			box += abox;
			}
		}
	}

void SurfWrapObject::GetLocalBoundBox(TimeValue t, INode *inode,ViewExp* vpt, Box3& box) 
	{
	Box3 abox;
	abox.Init();
	box.Init();
	if (TestFlag(SWRAP_DISPRESULT) && ob1 && ob2) {
		UpdateMesh(t,FALSE,inode->Selected());
		box = mesh.getBoundingBox();	
	} else {
		Object *ob;
		if (ob=GetPipeObj(t,0)) {
			INodeTransformed n(inode,GetOpTM(t,0));
			ob->GetLocalBoundBox(t,&n,vpt,abox);
			if (!abox.IsEmpty()) abox = abox * GetOpTM(t,0);
			box += abox;
			}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	INodeTransformed n(inode,GetOpTM(t,1));
			ob->GetLocalBoundBox(t,&n,vpt,abox);
			if (!abox.IsEmpty()) abox = abox * GetOpTM(t,1);
			box += abox;
			}
		}
	}

void SurfWrapObject::GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box)
{	Box3 abox;
	int disp = 0;
	abox.Init();
	box.Init();

	if (TestFlag(SWRAP_DISPRESULT) && ob1 && ob2) 
	{	UpdateMesh(t,FALSE,inode->Selected());
		Matrix3 mat = inode->GetObjectTM(t);	
		box = mesh.getBoundingBox();
		if (!box.IsEmpty()) box = box * mat;
	} 
	else 
	{ Object *ob;
	  if (ob=GetPipeObj(t,0)) 
	  {		INodeTransformed n(inode,GetOpTM(t,0));
			ob->GetWorldBoundBox(t,&n,vpt,abox);
			box += abox;
	  }
		
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
	  {	INodeTransformed n(inode,GetOpTM(t,1));
		ob->GetWorldBoundBox(t,&n,vpt,abox);
		box += abox;
	  }
	}
}

RefResult SurfWrapObject::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message ) 
	{
	switch (message) {
		case REFMSG_SELECT_BRANCH:
			if (hTarget==ob1 || hTarget==ob2) {
				ClearFlag(SWRAP_OB1SEL|SWRAP_OB2SEL);
				if (hTarget==ob1) SetFlag(SWRAP_OB1SEL);
				if (hTarget==ob2) SetFlag(SWRAP_OB2SEL);
				NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
				NotifyDependents(FOREVER,0,REFMSG_BRANCHED_HISTORY_CHANGED);
				}
			break;
		case REFMSG_CHANGE:
			 ivalid.SetEmpty();
			if (editOb==this) InvalidateUI();
			break;
		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
			ivalid.SetEmpty();
			if (editOb==this) InvalidateUI();
			break;
		case REFMSG_TARGET_DELETED:	
			{ if (hTarget==mbase) mbase=NULL;
			}
			break;
		case REFMSG_NODE_NAMECHANGE:
			{ if (hTarget==mbase) 
			  { normname = TSTR(mbase->GetName());
			    ShowNormName();
				}
			  break;
			}
		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			gpd->dim = GetParameterDim(gpd->index);			
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			gpn->name = GetParameterName(gpn->index);			
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}




//--Subobject Selection-------------------------------------------------------------


void SurfWrapObject::Move(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin)
	{
#ifdef DESIGN_VER
	t=0;
#endif
	if (TestFlag(SWRAP_OB1SEL) && tm1) {
		SetXFormPacket pckt(val,partm,tmAxis);
		tm1->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	if (TestFlag(SWRAP_OB2SEL) && tm2) {
		SetXFormPacket pckt(val,partm,tmAxis);
		tm2->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	}

void SurfWrapObject::Rotate(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Quat& val, BOOL localOrigin)
	{
#ifdef DESIGN_VER
	t=0;
#endif
	if (TestFlag(SWRAP_OB1SEL) && tm1) {
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		tm1->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	if (TestFlag(SWRAP_OB2SEL) && tm2) {
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		tm2->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	}

void SurfWrapObject::Scale(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin)
	{
#ifdef DESIGN_VER
	t=0;
#endif
	if (TestFlag(SWRAP_OB1SEL) && tm1) {
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		tm1->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	if (TestFlag(SWRAP_OB2SEL) && tm2) {
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		tm2->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	}

/*int SurfWrapObject::HitTest(TimeValue t, INode* inode, int type, int crossing,int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{	
	int savedLimits, res = 0;
	
	UpdateMesh(t,FALSE,inode->Selected());
	HitRegion hitRegion;
	GraphicsWindow *gw = vpt->getGW();	
	MakeHitRegion(hitRegion, type, crossing, 4, p);
	gw->setHitRegion(&hitRegion);
	gw->setTransform(inode->GetObjectTM(t));
	SubObjHitList hitList;
	MeshSubHitRec *rec;	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	gw->setRndLimits(gw->getRndLimits() & ~GW_BACKCULL);
	gw->clearHitCode();
	res = mesh.SubObjectHitTest(gw, gw->getMaterial(), &hitRegion,
				flags|SUBHIT_ABORTONHIT|SUBHIT_VERTS, hitList);
	rec = hitList.First();
	if (rec)
	  vpt->LogHit(inode,mc,0,(rec->index<ob1verts?0:1),NULL);
	gw->setRndLimits(savedLimits);	
	return res;
	}*/
int SurfWrapObject::HitTest(TimeValue t, INode* inode, int type, int crossing,int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc)
{	int  res = 0;
	Object *ob;
	Interval ivalid=FOREVER;
	if ((ob=GetPipeObj(t,0)) &&
		!(flags&HIT_SELONLY && !TestFlag(SWRAP_OB1SEL)) &&
		!(flags&HIT_UNSELONLY && TestFlag(SWRAP_OB1SEL)) ) {
		if (TestFlag(SWRAP_DISPRESULT))
		{TriObject t1;
		t1.GetMesh().DeepCopy(&mesh,PART_GEOM|PART_TOPO);
		t1.GetMesh().setNumVerts(ob1verts,TRUE);t1.GetMesh().setNumFaces(ob1faces,TRUE);
		if (t1.HitTest(t,inode,type,crossing,flags,p,vpt)) {
			vpt->LogHit(inode,mc,0,0,NULL);
			res = TRUE;
			if (flags & HIT_ABORTONHIT) 
			{ return TRUE;}
			}
		}
		else
		{ INodeTransformed n(inode,GetOpTM(t,0));
		  if (ob->HitTest(t,&n,type,crossing,flags,p,vpt)) {
			vpt->LogHit(inode,mc,0,0,NULL);
			res = TRUE;			
			}		
		}
	}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)) &&
		!(flags&HIT_SELONLY && !TestFlag(SWRAP_OB2SEL)) &&
		!(flags&HIT_UNSELONLY && TestFlag(SWRAP_OB2SEL)) ) {
		
		INodeTransformed n(inode,GetOpTM(t,1));
		
		if (ob->HitTest(t,&n,type,crossing,flags,p,vpt)) {
			vpt->LogHit(inode,mc,0,1,NULL);
			res = TRUE;			
			}		
		}
	return res;
}

int SurfWrapObject::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flags, ModContext* mc)
	{
	return 0;
	}

void SurfWrapObject::SelectSubComponent(
		HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
	{
	while (hitRec) {
		if (hitRec->hitInfo) {
			if (selected) SetFlag(SWRAP_OB2SEL);
			else ClearFlag(SWRAP_OB2SEL);
		} else {
			if (selected) SetFlag(SWRAP_OB1SEL);
			else ClearFlag(SWRAP_OB1SEL);
			}
		if (all) hitRec = hitRec->Next();
		else break;
		}
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_BRANCHED_HISTORY_CHANGED);
	if (ip) ShowOpList(ob2!=NULL);
	}

void SurfWrapObject::ClearSelection(int selLevel)
	{
	ClearFlag(SWRAP_OB1SEL|SWRAP_OB2SEL);
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	if (ip) ShowOpList(ob2!=NULL);
	}

int SurfWrapObject::SubObjectIndex(HitRecord *hitRec)
	{
	return hitRec->hitInfo;
	}

void SurfWrapObject::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	Matrix3 tm;
	if (TestFlag(SWRAP_OB1SEL)) {
		tm = GetOpTM(t,0) * node->GetObjectTM(t);
		cb->Center(tm.GetTrans(),0);
		}
	if (TestFlag(SWRAP_OB2SEL)) {
		tm = GetOpTM(t,1) * node->GetObjectTM(t);
		cb->Center(tm.GetTrans(),1);
		}
	}

void SurfWrapObject::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	Matrix3 tm;
	if (TestFlag(SWRAP_OB1SEL)) {
		tm = GetOpTM(t,0) * node->GetObjectTM(t);
		cb->TM(tm,0);
		}
	if (TestFlag(SWRAP_OB2SEL)) {
		tm = GetOpTM(t,1) * node->GetObjectTM(t);
		cb->TM(tm,1);
		}
	}

void SurfWrapObject::ActivateSubobjSel(int level, XFormModes& modes)
	{
	if (level) {
		modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
		NotifyDependents(
			FOREVER, 
			PART_SUBSEL_TYPE|PART_DISPLAY, 
			REFMSG_CHANGE);		
		ip->PipeSelLevelChanged();
		HWND hList = GetDlgItem(pmapObjects->GetHWnd(),IDC_SWRAP_OPERANDS);
		if (SendMessage(hList,LB_GETSEL,0,0)) flags |= SWRAP_OB1SEL;							
		if (SendMessage(hList,LB_GETSEL,1,0)) flags |= SWRAP_OB2SEL;	
	} 
}

class SurfWrapPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		SurfWrapPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload) {
			DWORD oldVer = ((SurfWrapObject*)(cb->targ))->pblock->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<1) {	
 				((SurfWrapObject*)targ)->pblock->SetValue(PB_SHOWTYPE,0,0);
				((SurfWrapObject*)targ)->SetFlag(SWRAP_DISPRESULT);
				}
			delete this;
			}
	};

#define WRAP_FLAGS_CHUNK	 0x0100
#define WRAP_OPANAME_CHUNK	 0x0110
#define WRAP_OPBNAME_CHUNK	 0x0120
#define WRAP_TRANS_CHUNK	 0x0130
#define WRAP_NORMNAME_CHUNK	 0x0140
#define WRAP_FIRSTMTLS_CHUNK 0x0150
#define WRAP_MAINNODE_CHUNK  0x0160
#define	WRAP_OFFMTX_CHUNK	 0x0170
#define WRAP_SECONDMTLS_CHUNK 0x0180
#define	WRAP_OFFMTX2_CHUNK	 0x0190


IOResult SurfWrapObject::Save(ISave *isave)
	{
	ULONG nb;
	int refid;

	isave->BeginChunk(WRAP_FLAGS_CHUNK);		
	isave->Write(&flags,sizeof(flags),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(WRAP_OPANAME_CHUNK);		
	isave->WriteWString(opaName);
	isave->EndChunk();
	
	isave->BeginChunk(WRAP_OPBNAME_CHUNK);		
	isave->WriteWString(opbName);
	isave->EndChunk();
	
	isave->BeginChunk(WRAP_TRANS_CHUNK);		
	isave->Write(&viewPt,sizeof(viewPt),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(WRAP_NORMNAME_CHUNK);		
	isave->WriteWString(normname);
	isave->EndChunk();

	isave->BeginChunk(WRAP_FIRSTMTLS_CHUNK);		
	isave->Write(&firstmtls,sizeof(firstmtls),&nb);
	isave->EndChunk();

	isave->BeginChunk(WRAP_SECONDMTLS_CHUNK);		
	isave->Write(&secondmtls,sizeof(secondmtls),&nb);
	isave->EndChunk();

	refid=isave->GetRefID(mainnode);
	isave->BeginChunk(WRAP_MAINNODE_CHUNK);		
	isave->Write(&refid,sizeof(int),&nb);
	isave->EndChunk();

	isave->BeginChunk(WRAP_OFFMTX_CHUNK);		
	isave->Write(&wrapoff,sizeof(wrapoff),&nb);
	isave->EndChunk();

	isave->BeginChunk(WRAP_OFFMTX2_CHUNK);		
	isave->Write(&wrapoff2,sizeof(wrapoff2),&nb);
	isave->EndChunk();

	return IO_OK;
	}


IOResult SurfWrapObject::Load(ILoad *iload)
	{
		iload->RegisterPostLoadCallback(
			new SurfWrapPostLoadCallback(
				new ParamBlockPLCB(swversions,NUM_OLDVERSIONS,&curVersion,this,PBLOCK)));
	ULONG nb;
	IOResult res = IO_OK;
	int refid;
	
	// Default names
	opaName = TSTR(GetString(IDS_AP_OPERAND));
	opbName = TSTR(GetString(IDS_AP_OPERAND));

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case WRAP_FLAGS_CHUNK:
				res=iload->Read(&flags,sizeof(flags),&nb);
				break;
			
			case WRAP_OPANAME_CHUNK: {
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				opaName = TSTR(buf);
				break;
				}

			case WRAP_OPBNAME_CHUNK: {
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				opbName = TSTR(buf);
				break;
				}

			case WRAP_TRANS_CHUNK:
				res=iload->Read(&viewPt,sizeof(viewPt),&nb);
				break;

			case WRAP_NORMNAME_CHUNK: {
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				normname = TSTR(buf);
				break;
				}
			case WRAP_FIRSTMTLS_CHUNK:
				res=iload->Read(&firstmtls,sizeof(firstmtls),&nb);
				break;
			case WRAP_SECONDMTLS_CHUNK:
				res=iload->Read(&secondmtls,sizeof(secondmtls),&nb);
				break;
			case WRAP_MAINNODE_CHUNK: 
				{res=iload->Read(&refid,sizeof(int),&nb);
			     iload->RecordBackpatch(refid,(void**)&mainnode);
				 break;
			case WRAP_OFFMTX_CHUNK:
				res=iload->Read(&wrapoff,sizeof(wrapoff),&nb);
				break;
			case WRAP_OFFMTX2_CHUNK:
				res=iload->Read(&wrapoff2,sizeof(wrapoff2),&nb);
				break;
				}
			}
		
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}

	Invalidate();
	return IO_OK;
	}

ParamDimension *SurfWrapObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_KIDEFAULT: return stdWorldDim;
		case PB_STANDOFF: return stdWorldDim;
		default: return defaultDim;
		}
	}

TSTR SurfWrapObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_KIDEFAULT: return GetString(IDS_AP_KIDEFAULT);
		case PB_STANDOFF: return GetString(IDS_AP_STANDOFF);
		default: return TSTR(_T(""));
		}
	}

/*BOOL SurfWrapObject::DoAttachMatOptionDialog (HWND hWnd) {
	return DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_ATTACH_MATS),
		hWnd, AttachMatsDlgProc, (LPARAM)this);
}

BOOL SurfWrapObject::DoInheritMatOptionDialog (HWND hWnd) {
	TSTR buf1 = GetString (IDS_ATTACH_INHERIT);
	TSTR buf2 = GetString (IDS_AP_SURFWRAPOBJECT);
	int ret = MessageBox (hWnd, buf1, buf2, MB_ICONQUESTION|MB_YESNOCANCEL);
	inheritMat = (ret==IDYES);
	return (ret != IDCANCEL);
}*/
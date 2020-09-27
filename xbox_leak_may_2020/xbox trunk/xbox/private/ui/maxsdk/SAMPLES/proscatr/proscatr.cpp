/**********************************************************************
 *<
	FILE: ProScatr.cpp

	DESCRIPTION:  A Procedural Scatter Object

	CREATED BY: Audrey Peterson

	HISTORY: created 24 December 1996

 *>	Copyright (c) 1996,1997 All Rights Reserved.
 **********************************************************************/
#include <io.h>
#include "proscatr.h"

static Class_ID PROSCATTEROBJ_CLASS_ID(0x5c2417bd, 0x44050964);

#define CID_CREATEPROSCATTERMODE CID_USER + 26		
#define REF_OP1		0
#define REF_OP2		1
#define REF_CONT1	2
#define REF_CONT2	3
#define PBLOCK		4
#define EPSILON 0.0001f
#define TESTLIMIT 100

const int isize=sizeof(int);
const int fsize=sizeof(float);
const int NLEN=16;
const int HLEN=4*isize;
typedef TCHAR AName[NLEN];

typedef struct{
  DWORD vfrom,vto;
  Point3 normal;
}fEdge;

// Flag bits
#define PSCAT_OB1SEL			(1<<0)
#define PSCAT_OB2SEL			(1<<1)
#define PSCAT_ANYSEL			(PSCAT_OB1SEL|PSCAT_OB2SEL)
#define PSCAT_INRENDER			(1<<2)
#define PSCAT_ROTX				(1<<4)
#define PSCAT_ROTY				(1<<5)
#define PSCAT_ROTZ				(1<<6)
#define PSCAT_TRANX				(1<<7)
#define PSCAT_TRANY				(1<<8)
#define PSCAT_TRANZ				(1<<9)
#define PSCAT_TRANFX			(1<<10)
#define PSCAT_TRANFY			(1<<11)
#define PSCAT_TRANFZ			(1<<12)
#define PSCAT_SCALEX			(1<<13)
#define PSCAT_SCALEY			(1<<14)
#define PSCAT_SCALEZ			(1<<15)
#define forceredraw				(1<<16)
#define PSCAT_DISPRESULT		(1<<3)

//--- Parameter map/block descriptors -------------------------------
#define PB_DISTMETHOD		0
#define PB_NUMBER			1
#define PB_BASESCALE		2
#define PB_VERTEXCHAOS		3
#define PB_MAKEPERP			4
#define PB_DOONSELFACES		5
#define PB_DISTMETHOD2		6
#define PB_EVERYNFACES		7

#define PB_DOWITHDIST		8

#define PB_ROTX				9
#define PB_ROTY				10
#define PB_ROTZ				11
#define PB_ROTLOCK			12
#define PB_TRANX			13
#define PB_TRANY			14
#define PB_TRANZ			15
#define PB_TRANLOCK			16
#define PB_TRANFX			17
#define PB_TRANFY			18
#define PB_TRANFZ			19
#define PB_TRANFLOCK		20
#define PB_SCALEX			21
#define PB_SCALEY			22
#define PB_SCALEZ			23
#define PB_SCALELOCK		24
#define PB_ASPECTLOCK		25

#define PB_DISPLAYMETHOD	26
#define PB_DISPLAYPORTION	27
#define PB_HIDEDISTOBJECT	28
#define PB_RANDOMSEED		29
#define PB_SHOWTYPE			30

#define PB_EXTRACTTYPE		31
#define PB_ANIOFF			32


static int createMethodIDs[] = {IDC_AP_VOLARRAY,IDC_AP_DISTOBJECT};
static int extractMethodIDs[] = {IDC_SCATR_EXTRACT_INSTANCE,IDC_SCATR_EXTRACT_COPY};
static int createDistDoIDs[] = {IDC_AP_DISTREF,IDC_AP_DISTCOPY,IDC_AP_DISTMOVE,IDC_AP_DISTINST};
static int createDistributeWayIDs[] = {IDC_AP_DISTCENTERS,IDC_AP_DISTVERTICES,IDC_AP_DISTAREA,
									   IDC_AP_DISTEVEN,IDC_AP_DISTNFACES,IDC_AP_DISTRANDOM,IDC_AP_DISTEDGES,IDC_AP_DISTECENTER,IDC_AP_DISTVOLUME};
static int ShowIDs[] = {IDC_PS_DISPRESULT,IDC_PS_DISPOPS};
#define FCENTER 0
#define VERTICES 1
#define UNIFORM 2
#define EVEN 3
#define NFACES 4
#define RANDOM 5
#define EDGES 6
#define ECENTER 7
#define VOLUME 8

static int createDisplayIDs[] = {IDC_AP_DISPPROXY,IDC_AP_DISPMESH};


static ParamUIDesc descObjects[] = {
	// Distribution Method
	ParamUIDesc(PB_DISTMETHOD,TYPE_RADIO,createMethodIDs,2),

	// Extraction Method
	ParamUIDesc(PB_EXTRACTTYPE,TYPE_RADIO,extractMethodIDs,2),

	// Number of instances in ScatterObject
	ParamUIDesc(
		PB_NUMBER,
		EDITTYPE_INT,
		IDC_AP_NUMBER,IDC_AP_NUMBERSPIN,
		0.0f,65000.0f,
		1.0f),

	// Base Scale
	ParamUIDesc(
		PB_BASESCALE,
		EDITTYPE_FLOAT,
		IDC_AP_BASESCALE,IDC_AP_BASESCALESPIN,
		0.0f,100.0f,
		0.1f,stdPercentDim),
	
	// Vertex Chaos
	ParamUIDesc(
		PB_VERTEXCHAOS,
		EDITTYPE_FLOAT,
		IDC_AP_BASEVERTS,IDC_AP_BASEVERTSSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	// Animation offset
	ParamUIDesc(
		PB_ANIOFF,
		EDITTYPE_TIME,
		IDC_AP_FRAMESDIFF,IDC_AP_FRAMESDIFFSPIN,
		1.0f,999999999.0f,
		10.0f),
	
	// Start with Perpendicular Instances
	ParamUIDesc(PB_MAKEPERP,TYPE_SINGLECHEKBOX,IDC_AP_GOPERP),			

	// Use Selected Faces
	ParamUIDesc(PB_DOONSELFACES,TYPE_SINGLECHEKBOX,IDC_AP_GOSELECT),			

	// Distribute instances using
	ParamUIDesc(PB_DISTMETHOD2,TYPE_RADIO,createDistributeWayIDs,9),

	// Skip N faces
	ParamUIDesc(
		PB_EVERYNFACES,
		EDITTYPE_INT,
		IDC_AP_DISTNFACESN,IDC_AP_DISTNFACESNSPIN,
		0.0f,65000.0f,
		1.0f),

	// Show Types
	ParamUIDesc(PB_SHOWTYPE,TYPE_RADIO,ShowIDs,2),			
	};

#define OBJECTSDESC_LENGTH 11

static ParamUIDesc descPickDist[] = {
	// What to do with dist object
	ParamUIDesc(PB_DOWITHDIST,TYPE_RADIO,createDistDoIDs,4),
	};

#define PICKDISTDESC_LENGTH 1

static ParamUIDesc descTransforms[] = {
	// Rotation Controls
	ParamUIDesc(
		PB_ROTX,
		EDITTYPE_FLOAT,
		IDC_AP_ROTX,IDC_AP_ROTXSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_ROTY,
		EDITTYPE_FLOAT,
		IDC_AP_ROTY,IDC_AP_ROTYSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_ROTZ,
		EDITTYPE_FLOAT,
		IDC_AP_ROTZ,IDC_AP_ROTZSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	// Lock maximum rotation
	ParamUIDesc(PB_ROTLOCK,TYPE_SINGLECHEKBOX,IDC_AP_ROTLOCK),			

	// Translation controls in local frame
	ParamUIDesc(
		PB_TRANX,
		EDITTYPE_FLOAT,
		IDC_AP_TRANX,IDC_AP_TRANXSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_TRANY,
		EDITTYPE_FLOAT,
		IDC_AP_TRANY,IDC_AP_TRANYSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_TRANZ,
		EDITTYPE_FLOAT,
		IDC_AP_TRANZ,IDC_AP_TRANZSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	// Lock maximum translation
	ParamUIDesc(PB_TRANLOCK,TYPE_SINGLECHEKBOX,IDC_AP_TRANLOCK),			

	// Translation controls in FACE frame
	ParamUIDesc(
		PB_TRANFX,
		EDITTYPE_FLOAT,
		IDC_AP_TRANXF,IDC_AP_TRANXFSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_TRANFY,
		EDITTYPE_FLOAT,
		IDC_AP_TRANYF,IDC_AP_TRANYFSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	ParamUIDesc(
		PB_TRANFZ,
		EDITTYPE_FLOAT,
		IDC_AP_TRANZF,IDC_AP_TRANZFSPIN,
		-9999999.0f,9999999.0f,
		SPIN_AUTOSCALE),

	// Lock maximum translation
	ParamUIDesc(PB_TRANFLOCK,TYPE_SINGLECHEKBOX,IDC_AP_TRANFLOCK),			

	// Scale Controls
	ParamUIDesc(
		PB_SCALEX,
		EDITTYPE_FLOAT,
		IDC_AP_SCALEX,IDC_AP_SCALEXSPIN,
		0.0f,1000.0f,
		0.1f),
	
	ParamUIDesc(
		PB_SCALEY,
		EDITTYPE_FLOAT,
		IDC_AP_SCALEY,IDC_AP_SCALEYSPIN,
		0.0f,1000.0f,
		0.1f),
	
	ParamUIDesc(
		PB_SCALEZ,
		EDITTYPE_FLOAT,
		IDC_AP_SCALEZ,IDC_AP_SCALEZSPIN,
		0.0f,1000.0f,
		0.1f),
	
	// Lock maximum scale transform
	ParamUIDesc(PB_SCALELOCK,TYPE_SINGLECHEKBOX,IDC_AP_SCALELOCK),			

	// Lock aspect ratios
	ParamUIDesc(PB_ASPECTLOCK,TYPE_SINGLECHEKBOX,IDC_AP_ASPECTLOCK)

	};

#define TRANSFORMSDESC_LENGTH 17

static ParamUIDesc descDisplay[] = {
	// Distribute instances using
	ParamUIDesc(PB_DISPLAYMETHOD,TYPE_RADIO,createDisplayIDs,2),

	// Display Portion
	ParamUIDesc(
		PB_DISPLAYPORTION,
		EDITTYPE_FLOAT,
		IDC_AP_DISPPORTION,IDC_AP_DISPPORTIONSPIN,
		1.0f,100.0f,
		1.0f,stdPercentDim),

	// Start with Perpendicular Instances
	ParamUIDesc(PB_HIDEDISTOBJECT,TYPE_SINGLECHEKBOX,IDC_AP_HIDEDIST),			
	
	// Random Number Seed
	ParamUIDesc(
		PB_RANDOMSEED,
		EDITTYPE_INT,
		IDC_AP_RANDOMSEED,IDC_AP_RANDOMSEEDSPIN,
		0.0f,32000.0f,
		1.0f)
	};

#define DISPLAYDESC_LENGTH 4

// variable type, NULL, animatable, number
ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_INT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_INT, NULL, FALSE, 7 },

	{ TYPE_INT, NULL, FALSE, 8 },

	{ TYPE_FLOAT, NULL, TRUE, 9 },
	{ TYPE_FLOAT, NULL, TRUE, 10 },
	{ TYPE_FLOAT, NULL, TRUE, 11 },
	{ TYPE_INT, NULL, FALSE, 12 },
	{ TYPE_FLOAT, NULL, TRUE, 13 },
	{ TYPE_FLOAT, NULL, TRUE, 14 },
	{ TYPE_FLOAT, NULL, TRUE, 15 },
	{ TYPE_INT, NULL, FALSE, 16 },
	{ TYPE_FLOAT, NULL, TRUE, 17 },
	{ TYPE_FLOAT, NULL, TRUE, 18 },
	{ TYPE_FLOAT, NULL, TRUE, 19 },
	{ TYPE_INT, NULL, FALSE, 20 },
	{ TYPE_FLOAT, NULL, TRUE, 21 },
	{ TYPE_FLOAT, NULL, TRUE, 22 },
	{ TYPE_FLOAT, NULL, TRUE, 23 },
	{ TYPE_INT, NULL, FALSE, 24 },
	{ TYPE_INT, NULL, FALSE, 25 },

	{ TYPE_INT, NULL, FALSE, 26 },
	{ TYPE_FLOAT, NULL, FALSE, 27 },
	{ TYPE_INT, NULL, FALSE, 28 },
	{ TYPE_INT, NULL, FALSE, 29 }
	};
ParamBlockDescID descVer1[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_INT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_INT, NULL, FALSE, 7 },

	{ TYPE_INT, NULL, FALSE, 8 },

	{ TYPE_FLOAT, NULL, TRUE, 9 },
	{ TYPE_FLOAT, NULL, TRUE, 10 },
	{ TYPE_FLOAT, NULL, TRUE, 11 },
	{ TYPE_INT, NULL, FALSE, 12 },
	{ TYPE_FLOAT, NULL, TRUE, 13 },
	{ TYPE_FLOAT, NULL, TRUE, 14 },
	{ TYPE_FLOAT, NULL, TRUE, 15 },
	{ TYPE_INT, NULL, FALSE, 16 },
	{ TYPE_FLOAT, NULL, TRUE, 17 },
	{ TYPE_FLOAT, NULL, TRUE, 18 },
	{ TYPE_FLOAT, NULL, TRUE, 19 },
	{ TYPE_INT, NULL, FALSE, 20 },
	{ TYPE_FLOAT, NULL, TRUE, 21 },
	{ TYPE_FLOAT, NULL, TRUE, 22 },
	{ TYPE_FLOAT, NULL, TRUE, 23 },
	{ TYPE_INT, NULL, FALSE, 24 },
	{ TYPE_INT, NULL, FALSE, 25 },

	{ TYPE_INT, NULL, FALSE, 26 },
	{ TYPE_FLOAT, NULL, FALSE, 27 },
	{ TYPE_INT, NULL, FALSE, 28 },
	{ TYPE_INT, NULL, FALSE, 29 },
	{ TYPE_INT, NULL, FALSE, 30 }
	};
ParamBlockDescID descVer2[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_INT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_INT, NULL, FALSE, 7 },

	{ TYPE_INT, NULL, FALSE, 8 },

	{ TYPE_FLOAT, NULL, TRUE, 9 },
	{ TYPE_FLOAT, NULL, TRUE, 10 },
	{ TYPE_FLOAT, NULL, TRUE, 11 },
	{ TYPE_INT, NULL, FALSE, 12 },
	{ TYPE_FLOAT, NULL, TRUE, 13 },
	{ TYPE_FLOAT, NULL, TRUE, 14 },
	{ TYPE_FLOAT, NULL, TRUE, 15 },
	{ TYPE_INT, NULL, FALSE, 16 },
	{ TYPE_FLOAT, NULL, TRUE, 17 },
	{ TYPE_FLOAT, NULL, TRUE, 18 },
	{ TYPE_FLOAT, NULL, TRUE, 19 },
	{ TYPE_INT, NULL, FALSE, 20 },
	{ TYPE_FLOAT, NULL, TRUE, 21 },
	{ TYPE_FLOAT, NULL, TRUE, 22 },
	{ TYPE_FLOAT, NULL, TRUE, 23 },
	{ TYPE_INT, NULL, FALSE, 24 },
	{ TYPE_INT, NULL, FALSE, 25 },

	{ TYPE_INT, NULL, FALSE, 26 },
	{ TYPE_FLOAT, NULL, FALSE, 27 },
	{ TYPE_INT, NULL, FALSE, 28 },
	{ TYPE_INT, NULL, FALSE, 29 },
	{ TYPE_INT, NULL, FALSE, 30 },
	{ TYPE_INT, NULL, FALSE, 31 },
	{ TYPE_INT, NULL, TRUE, 32 }
	};

#define PBLOCK_LENGTH	33

#define NUM_OLDVERSIONS	2

#define CURRENT_VERSION	2
static ParamVersionDesc psversions[] = {
	ParamVersionDesc(descVer0,30,0),
	ParamVersionDesc(descVer1,31,1),
	};
static ParamVersionDesc curVersion(descVer2,PBLOCK_LENGTH,CURRENT_VERSION);
typedef struct {
 Point3	pt,norm,x,V02;
} SavePt;
static Point3 ZNorm(0.0f,0.0f,1.0f);
static Point3 XNorm(1.0f,0.0f,0.0f);
class PickOperand;

class PScatterObject: public GeomObject, public MeshOpProgress {
		
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );
				
	public:
		TSTR opaName, opbName;
		Object *ob1, *ob2;
		Control *tm1,*tm2;
		INode *mainnode;
		static IParamMap *pmapObjects;
		static IParamMap *pmapDist;
		static IParamMap *pmapTransforms;
		static IParamMap *pmapDisplay;
		static custsettings;
		IParamBlock *pblock;
		static Matrix3 createTM;
		static HWND thwnd,distwnd;
		DWORD flags;
		int ob1verts,ob1faces,firstmtls,secondmtls,size;
		Interval ivalid;
		Mesh mesh;		

		static IObjParam *ip;
		static PickOperand pickCB;
		static BOOL creating;
		static PScatterObject *editOb;
		static int extractCopy;

		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;
		static SelectModBoxCMode *selectMode;

		PScatterObject();
		~PScatterObject();
		
		void SetFlag(DWORD mask) { flags|=mask; }
		void ClearFlag(DWORD mask) { flags &= ~mask; }
		int TestFlag(DWORD mask) { return(flags&mask?1:0); }
		DWORD BoolOp(int &order);

		void SetOperand(INode *node,Matrix3& boolTm,Matrix3& oppTm,int which,INode *orignode,bool *canUndo=NULL);
		BOOL UpdateMesh(TimeValue t,BOOL force=FALSE,BOOL sel=FALSE);
		Object *GetPipeObj(TimeValue t,int which);
		Matrix3 GetOpTM(TimeValue t,int which,Interval *iv=NULL);
		void Invalidate() {ivalid.SetEmpty();}
		void ExtractOperand(int which);
//		void MapKeys(TimeMap *map,DWORD flags);

		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		TCHAR *GetObjectName() { return GetString(IDS_AP_SCATTERNAME); }
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);

		// For sub-object selection
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext* mc);
		
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert);
		void ClearSelection(int selLevel);

		int SubObjectIndex(HitRecord *hitRec);

		void ActivateSubobjSel(int level, XFormModes& modes);
		void ShowOpList(BOOL isobj);
		void ShowName(BOOL ob2);
		static AName *NameLst;
		static HWND hParams2,hObject;
		static ICustEdit *custCtrlEdit;
		// From Object		
		void InitNodeName(TSTR& s) {s = GetString(IDS_AP_SCATTERNAME);}
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
		void SetExtractButtonState();

		// From GeomObject		
		ObjectHandle CreateTriObjRep(TimeValue t) {return NULL;}
		int IntersectRay(TimeValue t, Ray& r, float& at, Point3& norm);
		Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

		// Animatable methods
		Class_ID ClassID() {return PROSCATTEROBJ_CLASS_ID;}  
		void GetClassName(TSTR& s) {s = GetString(IDS_AP_SCATTEROBJECT);}
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
		int NumRefs() {return 5;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		void GetFilename(TCHAR *filename);
		void SetupTargetList();
		int SaveSettings(int overwrite,TCHAR *newname);
		int GetSettings(int setnum,TCHAR *newname);
		int RemSettings(int setnum,TCHAR *newname);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		// From MeshOpProgress
		void Init(int total);
		BOOL Progress(int p);
		void GetPosInfo(int x,int y,int z,int sx,int sy,int sz,float *rotx,float *roty,float *rotz,TimeValue t);
		void InvalidateUI();
	};				

class PickOperand : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		PScatterObject *bo;
		
		PickOperand() {bo=NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);

		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);

		BOOL Filter(INode *node);
		
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}

		PickNodeCallback *GetFilter() {return this;}
	};

IParamMap *PScatterObject::pmapObjects = NULL;
IParamMap *PScatterObject::pmapDist = NULL;
IParamMap *PScatterObject::pmapTransforms = NULL;
IParamMap *PScatterObject::pmapDisplay = NULL;
IObjParam *PScatterObject::ip                   = NULL;
BOOL PScatterObject::creating                   = FALSE;
PScatterObject *PScatterObject::editOb              = NULL;
MoveModBoxCMode*    PScatterObject::moveMode    = NULL;
RotateModBoxCMode*  PScatterObject::rotMode 	= NULL;
UScaleModBoxCMode*  PScatterObject::uscaleMode  = NULL;
NUScaleModBoxCMode* PScatterObject::nuscaleMode = NULL;
SquashModBoxCMode*  PScatterObject::squashMode  = NULL;
SelectModBoxCMode*  PScatterObject::selectMode  = NULL;
ICustEdit *PScatterObject::custCtrlEdit=NULL;
int PScatterObject::custsettings=0;
AName *PScatterObject::NameLst=NULL;
PickOperand PScatterObject::pickCB;
Matrix3 PScatterObject::createTM;
HWND PScatterObject::thwnd;
HWND PScatterObject::distwnd;
HWND PScatterObject::hParams2;
HWND PScatterObject::hObject;
int PScatterObject::extractCopy = FALSE;

class PScatObjClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new PScatterObject;}
	const TCHAR *	ClassName() { return GetString(IDS_AP_SCATTEROBJECT); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return PROSCATTEROBJ_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_AP_COMPOUNDOBJECTS);}
	BOOL			OkToCreate(Interface *i);
	int 			BeginCreate(Interface *i);
	int 			EndCreate(Interface *i);
	void			ResetClassParams(BOOL fileReset);
	};

class CreateScatrref : public RestoreObj {
	public:   		
		PScatterObject *obj;
		TSTR name,name2;
		CreateScatrref(PScatterObject *o, TSTR n, TSTR n1) {
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

void PScatObjClassDesc::ResetClassParams(BOOL fileReset)
	{
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

BOOL PScatObjClassDesc::OkToCreate(Interface *i)
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

static PScatObjClassDesc PScatObjDesc;

ClassDesc* GetPScatObjDesc() { return &PScatObjDesc; }

BOOL PScatterObject::HasUVW() { 
	BOOL genUV1=FALSE,genUV2=FALSE;
    if (ob1) genUV1=ob1->HasUVW();
	if (ob2) genUV2=ob2->HasUVW();
	return (genUV1&& genUV2); 
	}

void PScatterObject::SetGenUVW(BOOL sw) {  
	if (sw==HasUVW()) return;
	if (ob1) ob1->SetGenUVW(sw);
	if (ob2) ob2->SetGenUVW(sw);			
	}


// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

//----------------------------------------------------------------------

class CreatePScatProc : public MouseCallBack {
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

int CreatePScatProc::proc( 
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

class CreatePScatMode : public CommandMode, ReferenceMaker {		
	public:		
		CreatePScatProc proc;
		INode *node, *svNode;
		IObjParam *ip;
		PScatterObject *obj;

		void Begin(INode *n,IObjParam *i);
		void End(IObjParam *i);
		void JumpStart(IObjParam *i,PScatterObject *o);

		int Class() {return CREATE_COMMAND;}
		int ID() { return CID_CREATEPROSCATTERMODE; }
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
static CreatePScatMode theCreatePScatMode;

RefResult CreatePScatMode::NotifyRefChanged(
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

class CreateScatterRestore : public RestoreObj {
	public:   		
		void Restore(int isUndo) {
			if (theCreatePScatMode.ip) {
				// Jump out of boolean create mode.
				theCreatePScatMode.ip->SetStdCommandMode(CID_OBJMOVE);
				}
			}	
		void Redo() {}
		TSTR Description() {return GetString(IDS_AP_CREATESCAT);}
	};

// Sending the REFMSG_NOTIFY_PASTE message notifies the modify
// panel that the Node's object reference has changed when
// undoing or redoing.
class CreateScatterNotify : public RestoreObj {
	public:   		
		PScatterObject *obj;
		BOOL which;
		CreateScatterNotify(PScatterObject *o, BOOL w) {
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

void CreatePScatMode::Begin(INode *n,IObjParam *i) 
	{
	MakeRefByID(FOREVER,0,n);
	svNode = node;
	assert(node);
	ip = i;
	proc.Init(ip);

	theHold.Begin();
	theHold.Put(new CreateScatterRestore);

	obj = new PScatterObject;

	theHold.Put(new CreateScatterNotify(obj,1));

	obj->createTM = node->GetObjectTM(0);
	obj->mainnode=node;
	theHold.Begin();

	obj->SetOperand(
		node,
		obj->createTM,
		obj->createTM,
		0,NULL);
	node->SetObjectRef(obj);
	
	theHold.Put(new CreateScatterNotify(obj,0));

	theHold.Accept(GetString(IDS_AP_CREATE));

	obj->BeginEditParams(i,BEGIN_EDIT_CREATE,NULL);
	}

void CreatePScatMode::End(IObjParam *i)
	{
	svNode = node;
	if (obj) obj->EndEditParams(i,END_EDIT_REMOVEUI,NULL);
	DeleteAllRefsFromMe();
	ip  = NULL;
	obj = NULL;
	}

void CreatePScatMode::JumpStart(IObjParam *i,PScatterObject *o)
	{
	ip  = i;
	obj = o;
	//MakeRefByID(FOREVER,0,svNode);
	obj->BeginEditParams(i,BEGIN_EDIT_CREATE,NULL);
	}


int PScatObjClassDesc::BeginCreate(Interface *i)
	{	
	assert(i->GetSelNodeCount()==1);
	theCreatePScatMode.Begin(i->GetSelNode(0),(IObjParam*)i);
	i->PushCommandMode(&theCreatePScatMode);
	return TRUE;
	}

int PScatObjClassDesc::EndCreate(Interface *i)
	{
	theCreatePScatMode.End((IObjParam*)i);
	i->RemoveMode(&theCreatePScatMode);
	return TRUE;
	}

//----------------------------------------------------------------------
/*void PScatterObject::MapKeys(TimeMap *map,DWORD flags)
{//	Animatable::MapKeys(map,flags);
//	TimeValue TempTime;
// remap values
//	pblock->GetValue(PB_ANIOFF,0,TempTime,FOREVER);
//	TempTime=map->map(TempTime);
//	pblock->SetValue(PB_ANIOFF,0,TempTime);
}  */


PScatterObject::PScatterObject()
	{	MakeRefByID(FOREVER, PBLOCK, CreateParameterBlock(descVer2, PBLOCK_LENGTH, CURRENT_VERSION));	
	ob1 = ob2 = NULL;
	tm1 = tm2 = NULL;
	flags = (PSCAT_ROTX | PSCAT_ROTY | PSCAT_ROTZ | PSCAT_TRANX | PSCAT_TRANY | PSCAT_TRANZ);
	flags |= (PSCAT_SCALEX | PSCAT_SCALEY | PSCAT_SCALEZ | PSCAT_TRANFX | PSCAT_TRANFY | PSCAT_TRANFZ);
	flags |= PSCAT_DISPRESULT;
	ivalid.SetEmpty();
	pblock->SetValue(PB_DOWITHDIST,0,3);
	pblock->SetValue(PB_DISTMETHOD,0,1);
	pblock->SetValue(PB_DISTMETHOD2,0,3);
	pblock->SetValue(PB_BASESCALE,0,1.0f);
	pblock->SetValue(PB_MAKEPERP,0,1);
	pblock->SetValue(PB_RANDOMSEED,0,12345);
	pblock->SetValue(PB_NUMBER,0,1);
	pblock->SetValue(PB_SCALEX,0,0.0f);
	pblock->SetValue(PB_SCALEY,0,0.0f);
	pblock->SetValue(PB_SCALEZ,0,0.0f);
	pblock->SetValue(PB_DISPLAYPORTION,0,1.0f);
	pblock->SetValue(PB_DISPLAYMETHOD,0,1);
	pblock->SetValue(PB_SHOWTYPE,0,0);
	pblock->SetValue(PB_ANIOFF,0,TimeValue(0));
	ob1verts=0;ob1faces=0;
	firstmtls=0;secondmtls=0;
	size=16*isize+fsize*15;
	}

PScatterObject::~PScatterObject()
	{
	DeleteAllRefsFromMe();
	}

static int Progress(int percent,void *data)
	{
	if (data) {
		MeshOpProgress *p = (MeshOpProgress*)data;
		return p->Progress(percent);
	} else {
		return 1;
		}
	}

void PScatterObject::ExtractOperand(int which)
{	if (creating) return;

	// Compute a node TM for the new object
	assert(ip);
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	Matrix3 tm = nodes[0]->GetObjectTM(ip->GetTime());
	Matrix3 tmOp = GetOpTM(ip->GetTime(),which);
	Object *obj = which ? ob2 : ob1;
	if (!obj) return;
	tm = tmOp * tm;

	// Clone the object if specified
	if (extractCopy) obj = (Object*)obj->Clone();

	// Create the new node
	INode *node = ip->CreateObjectNode(obj);

	// Set the node TM.
	SuspendAnimate();
	AnimateOff();
	node->SetNodeTM(0,tm);
	ResumeAnimate();

	nodes.DisposeTemporary();
}

Point3 GetNormalizedNormal(ulong *curface,Point3 *v)
{return(Normalize((v[curface[0]]-v[curface[1]])^(v[curface[0]]-v[curface[2]])));
}
void SpreadOutParts(float *arealst,float TotalArea,int maxfaces,int block,int total,int *lst)
{int i,newcount;

  newcount=0;
  for (i=0;i<block;i++)
  {	newcount+=(lst[i]=(int)floor(total*(arealst[i]/TotalArea)));
  }
  for (i=newcount;i<total;i++)
   lst[RND0x(maxfaces)]+=1;
}

float GetFaceArea(DWORD *curface,Point3 *v)
{ Point3 V1,V2;

  V1=v[curface[1]]-v[curface[0]];
  V2=v[curface[2]]-v[curface[0]];
/* Point3 D,N;
  float h,b;
  N=V1^V2;
	D=Normalize(N^V1);
  h=(float)fabs(DotProd(D,v[curface[2]])-DotProd(D,v[curface[0]]));
  b=Length(V1);
  return(0.5f*b*h);	 */
  return(Length(V1^V2)*0.5f);
}

void FillInUniform(int infaces,int c,Face *faces,Point3 *verts,SavePt *center)
{ int i,*lst,vertices=0;
  float *arealst,TotalArea,rx,ry;
  Point3 Norm,X;

  arealst=new float[infaces];
  lst=new int[infaces];
  assert(arealst && lst);
  TotalArea=0.0f;
  for (i=0;i<infaces;i++)
    TotalArea+=(arealst[i]=GetFaceArea(&faces[i].v[0],verts));
  float incr=TotalArea/(float)c,cpos=0.0f,tarea=arealst[0];
  i=0;
  for (int cnt=0;cnt<c;cnt++)
  {	cpos+=incr;
    while ((cpos>tarea)&&(i+1<infaces)) tarea+=arealst[++i];
    Point3 V0,V1,V2,V01,V02,nV02;
    V01=(V1=verts[faces[i].v[1]])-(V0=verts[faces[i].v[0]]);
    V02=(V2=verts[faces[i].v[2]])-V0;
    Norm=GetNormalizedNormal(&faces[i].v[0],verts);
	X=Normalize(V01);
	nV02=Normalize(V02);
    center[vertices].norm=Norm;
	center[vertices].x=X;
	center[vertices].V02=nV02;
    rx=RND01();ry=RND01();
    if (rx+ry>1) {rx=1-rx;ry=1-ry;}
	center[vertices].pt=V0+V01*rx+V02*ry;
	vertices++;
  }
  delete[] arealst;
  delete[] lst;
}
Point3 AveNormal(Point3 newnorm,Point3 oldnorm)
{ Point3 aveN;

  aveN=(newnorm+oldnorm)/2.0f;
  if (Length(aveN)==0.0f)
  { aveN=Point3(RND11(),RND11(),RND11());}
  return(Normalize(aveN));
}
float Checkfordup(DWORD *curface,int P0,int P1,float *arealst,fEdge *edgelst,int *edges,int face,Point3 *vlst,Face *f)
{ int i,found;
  float area;
  Point3 norm;

  found=i=0;area=0.0f;
  while ((!found)&&(i<*edges))
   { found=((edgelst[i].vto==curface[P0])&&(edgelst[i].vfrom==curface[P1]))||
            ((edgelst[i].vfrom==curface[P0])&&(edgelst[i].vto==curface[P1]));
     if (!found) i++;
   }
   if (found)
   { norm=GetNormalizedNormal(curface,vlst);
     edgelst[i].normal=AveNormal(norm,edgelst[i].normal);
   }
   else
   {if (arealst) area=(arealst[i]=Length(vlst[curface[P0]]-vlst[curface[P1]]));
    edgelst[i].vfrom=curface[P0];edgelst[i].vto=curface[P1];
    edgelst[i].normal=GetNormalizedNormal(curface,vlst);
    (*edges)++;
   }
 return(area);
}

float GetEdgeArea(Face *faces,Point3 *verts,float *arealst,fEdge *edgelst,int *edges,int face)
{ float area;
  DWORD *curface=&(faces[face]).v[0],flags=faces[face].flags;
  area=0.0f;
  if ((flags & EDGE_A)>0)
    area+=Checkfordup(curface,0,1,arealst,edgelst,edges,face,verts,faces);
  if ((flags & EDGE_B)>0)
    area+=Checkfordup(curface,1,2,arealst,edgelst,edges,face,verts,faces);
  if ((flags & EDGE_C)>0)
    area+=Checkfordup(curface,2,0,arealst,edgelst,edges,face,verts,faces);
  return(area);
}

void FillByEdges(int infaces,int c,Face *faces,Point3 *verts,SavePt *center)
{ int i,edges,numedges=3*infaces,*lst,vertices=0;
  float *arealst,TotalArea,r;
  fEdge *edgelst;
  Point3 lastpt;

  edges=0;
  assert((arealst=new float[numedges])!=NULL);
  assert((edgelst=new fEdge[numedges])!=NULL);
  lst=new int[numedges];assert(lst);
  TotalArea=0.0f;
  for (i=0;i<infaces;i++)
    TotalArea+=GetEdgeArea(faces,verts,arealst,edgelst,&edges,i);
  SpreadOutParts(arealst,TotalArea,edges-1,edges,c,lst);
  for (i=0;i<edges;i++)
  { Point3 pt=verts[edgelst[i].vfrom],pdist=verts[edgelst[i].vto]-pt;
    Point3 X=Normalize(pdist);
	for (int j=0;j<lst[i];j++)
	{ r=RND01();
      center[vertices].pt=pt+pdist*r;
      center[vertices].norm=edgelst[i].normal;
      center[vertices].x=X;
      vertices++;
    }
  }
  if (lst) delete[] lst;
  delete[] arealst;
  delete[] edgelst;
} 
void FillEdgeCenter(int edges,fEdge *edgelst,Point3 *verts,SavePt *center)
{ int i;

  for (i=0;i<edges;i++)
  { Point3 pt=verts[edgelst[i].vfrom],pdist=verts[edgelst[i].vto]-pt;
    Point3 X=Normalize(pdist);
    center[i].pt=pt+pdist*0.5f;
    center[i].norm=edgelst[i].normal;
    center[i].x=X;
  }
  if (edgelst) delete[] edgelst;
} 

int amatch(DWORD *ptr,DWORD nxt)
{ return((ptr[0]==nxt)||(ptr[1]==nxt)||(ptr[2]==nxt));
}
BOOL VertexinWhichFace(int v,Face *flst,int infaces,int *aface)
{ BOOL found;

  found=0;
  while ((*aface<infaces)&&(!found))
  { found=amatch(flst[*aface].v,v);
    if (!found) (*aface)++;
  }
  return(found);
}

void FillInFaces(int nfaces,int vertices,Face *faces,Point3 *verts,SavePt *center)
{ int i;

  for (i=0;i<nfaces;i++)
  { center[i].pt=(verts[faces[i].v[0]]+verts[faces[i].v[1]]+verts[faces[i].v[2]])/3.0f;
	center[i].norm=GetNormalizedNormal(&faces[i].v[0],verts);
	center[i].x=Normalize(verts[faces[i].v[1]]-verts[faces[i].v[0]]);
	center[i].V02=Normalize(verts[faces[i].v[2]]-verts[faces[i].v[0]]);
  }
}

void FillInVertex(int inverts,int infaces,Face *faces,Point3 *verts,SavePt *center)
{ int i,face,ncounter,firstface,icount=0;
  Point3 newNorm,zero=Point3(0.0f,0.0f,0.0f);

  for (i=0;i<inverts;i++)
  {	face=firstface=ncounter=0;
	  newNorm=zero;
      while (VertexinWhichFace(i,faces,infaces,&face))
	  { newNorm+=GetNormalizedNormal(&faces[face].v[0],verts);
	    if (ncounter==0) firstface=face;
	    ncounter++;face++;
	   }
	if (ncounter>0)
	{
	int firstv=(faces[firstface].v[0]==(DWORD)i?faces[firstface].v[1]:faces[firstface].v[0]);
	center[icount].pt=verts[i];
	center[icount].norm=Normalize(ncounter>0?newNorm/(float)ncounter:Point3(RND11(),RND11(),RND11()));
	center[icount].x=Normalize((verts[firstv]-verts[i])^center[icount].norm);
	icount++; }
  }
}

void FillByXFaces(int infaces,int c,Face *faces,Point3 *verts,SavePt *center,int incr,BOOL rnd)
{ int i=0,maxrnd=infaces-1;
  float rx,ry;
  Point3 Norm;

  Point3 V0,V1,V2,V01,V02;
  incr++;
  for (int cnt=0;cnt<c;cnt++)
  { if (rnd) i=RND0x(maxrnd);
	V01=(V1=verts[faces[i].v[1]])-(V0=verts[faces[i].v[0]]);
    V02=(V2=verts[faces[i].v[2]])-V0;
	Norm=GetNormalizedNormal(&faces[i].v[0],verts);
    center[cnt].norm=Norm;
	center[cnt].x=Normalize(V01);
	center[cnt].V02=Normalize(V02);
    rx=RND01();ry=RND01();
    if (rx+ry>1) {rx=1-rx;ry=1-ry;}
	center[cnt].pt=V0+V01*rx+V02*ry;
	if (!rnd) {i+=incr;if (i>=infaces) i%=infaces;}
  }
}

BOOL InsideMesh(Ray& ray, Mesh *amesh,Point3 *fnorms)
{	Face *face;	
	Point3 n,p, bry;
	float d, rn, a,at;
	BOOL notfirst = FALSE,inside=FALSE,savein=FALSE;
	int raycount=0;

   while ((!savein)&&(raycount<3))
   { notfirst=FALSE;
     if (raycount==0) ray.dir=Point3(1.0f,0.0f,0.0f);
	 else if (raycount==1) ray.dir=Point3(0.0f,1.0f,0.0f);
	 else ray.dir=Point3(0.0f,0.0f,1.0f);
	face=amesh->faces;
	for (int i=0; i<amesh->getNumFaces(); i++,face++)
	{	n = fnorms[i];
		// See if the ray intersects the plane (backfaced)
		rn = DotProd(ray.dir,n);
		if (fabs(rn) < EPSILON) continue; //is parallel, so bail
		// Use a point on the plane to find d
		d = DotProd(amesh->verts[face->v[0]],n);
		// Find the point on the ray that intersects the plane
		a = (d - DotProd(ray.p,n)) / rn;
		// The point on the ray and in the plane.
		p = ray.p + a*ray.dir;
		inside=(((a > 0.0f)&&(rn>0.0f))||((a < 0.0f)&&(rn<0.0f)));
		// Compute barycentric coords.
		bry = amesh->BaryCoords(i,p);
		// barycentric coordinates must sum to 1 and each component must
		// be in the range 0-1
		if (bry.x<0.0f || bry.x>1.0f || bry.y<0.0f || bry.y>1.0f || bry.z<0.0f || bry.z>1.0f) continue;
		if (fabs(bry.x + bry.y + bry.z - 1.0f) > EPSILON) continue;
		// Must be closer than the closest at so far
		if (notfirst)
			if (fabs(a) > at) continue;

		// Hit!
		notfirst = TRUE;		
		at    = (float)fabs(a);
		savein=inside;
	  } raycount++;
   }
	return savein;

	}

void FillByNoVolume(Mesh *amesh,int num,SavePt *center,Box3 bbox,Point3 box)
{ Ray ray;
  for (int i=0;i<num;i++)
  { center[i].pt=bbox.pmin+Point3(RND01(),RND01(),RND01())*box;
  }
}
void FillByVolume(Mesh *amesh,int num,SavePt *center,Box3 bbox,Point3 box,Point3 *fnorms)
{ Ray ray;
  for (int i=0;i<num;i++)
  {	int testcount=0;
	ray.p=bbox.pmin+Point3(RND01(),RND01(),RND01())*box;
	while ((!InsideMesh(ray,amesh,fnorms))&&(testcount<TESTLIMIT))
	{ testcount++;
	  ray.p=bbox.pmin+Point3(RND01(),RND01(),RND01())*box;
	}
	center[i].pt=ray.p;
  }
}


void PScatterObject::SetOperand(
		INode *node,Matrix3& boolTm,Matrix3& oppTm,int which,INode *orignode,bool *canUndo)
{ if (which)
	{ opbName = TSTR(GetString(IDS_AP_D)) + TSTR(node->GetName()); }
  else { opaName= TSTR(GetString(IDS_AP_S)) + TSTR(node->GetName()); }

  BOOL delnode = FALSE;
  Object *obj = node->GetObjectRef();

  if (which)
  {	int	dmethod;
	pblock->GetValue(PB_DOWITHDIST,0,dmethod,FOREVER);
	if (dmethod==0)	obj = MakeObjectDerivedObject(obj);
	else if (dmethod==1) obj = (Object*)obj->Clone();
    else if (dmethod==2) delnode = TRUE;
	else flags |= forceredraw;
  }
	
//	theHold.Begin();

	// Plug in the object and a new controller.	
	if (which?ob1:ob2) ReplaceReference(which,obj,TRUE);
	else MakeRefByID(FOREVER,which,obj);
	ShowOpList(ob2!=NULL);
//	ReplaceReference(2,NewDefaultMatrix3Controller());
	SuspendAnimate();
	AnimateOff();
	 RemapDir *remap = NewRemapDir(); 
	 Control *oldtm=node->GetTMController(),*nxttm;
	 nxttm = (Control *)oldtm->Clone(*remap);
	 remap->DeleteThis();
	ReplaceReference(which+2,nxttm);
	if (which)
	{ //theHold.Put(new CreateDistref(bo,node));
	  int c1=0,c2=0;
	  if (orignode)
	  {	if (orignode->GetMtl() && node->GetMtl()) 
	    { if (orignode->GetMtl()!=node->GetMtl()) 
		  {	Mtl *multi = CMaterials(orignode->GetMtl(), node->GetMtl(), c1, c2);
			orignode->SetMtl(multi);
		   (*canUndo)=FALSE;
		  }
	    }
		else if (node->GetMtl()) orignode->SetMtl(node->GetMtl());
	  }
	  firstmtls=c1;secondmtls=c2;
	}
	SetXFormPacket pckt(oppTm,boolTm);
    if (which) tm2->SetValue(0,&pckt);
	else tm1->SetValue(0,&pckt);
	ResumeAnimate();
	if (delnode) ip->DeleteNode(node);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	theHold.Accept(GetString(IDS_AP_PICKDISTOBJ));
	ivalid.SetEmpty();
}		

Object *PScatterObject::GetPipeObj(TimeValue t,int which)
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

Matrix3 PScatterObject::GetOpTM(TimeValue t,int which,Interval *iv)
	{
	Matrix3 tm(1);
	Interval valid, *v;
	if (iv) v = iv;
	else v = &valid;

	if (which==0) {
		if (tm1) {
			tm1->GetValue(t,&tm,*v,CTRL_RELATIVE);
			}
	} else {
		if (tm2) {
			tm2->GetValue(t,&tm,*v,CTRL_RELATIVE);
			}
		}
	return tm;
	}

int PScatterObject::RenderBegin(TimeValue t, ULONG flags)
	{
	SetFlag(PSCAT_INRENDER);
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	return 0;
	}

int PScatterObject::RenderEnd(TimeValue t)
	{
	ivalid.SetEmpty();
	ClearFlag(PSCAT_INRENDER);	
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	return 0;
	}

void PScatterObject::Init(int total)
	{
	}

BOOL PScatterObject::Progress(int p)
	{
	SHORT res = GetAsyncKeyState(VK_ESCAPE);
	if (res&1) {
		return FALSE;
		}
	else return TRUE;
	}
#define LOTSOFACES	2000
void MakeProxy(Point3 *invlst,Face *inflst,Box3 inbox)
{	 float Dy=inbox.pmax.y-inbox.pmin.y,Dy125=0.125f*Dy,Ay=(inbox.pmax.y+inbox.pmin.y)/2.0f;
	 float Dx=inbox.pmax.x-inbox.pmin.x,Dx25=0.25f*Dx,Ax=(inbox.pmax.x+inbox.pmin.x)/2.0f;
	 invlst[0]=Point3(inbox.pmax.x,inbox.pmin.y,inbox.pmin.z);
	 invlst[1]=Point3(inbox.pmax.x,inbox.pmax.y,inbox.pmin.z);
	 invlst[2]=Point3(inbox.pmin.x,inbox.pmax.y,inbox.pmin.z);
	 invlst[3]=Point3(inbox.pmin.x,inbox.pmin.y,inbox.pmin.z);
	 invlst[4]=Point3(Ax+Dx25,Ay-Dy125,inbox.pmax.z);
	 invlst[5]=Point3(Ax+Dx25,Ay+Dy125,inbox.pmax.z);
	 invlst[6]=Point3(Ax-Dx25,Ay+Dy125,inbox.pmax.z);
	 invlst[7]=Point3(Ax-Dx25,Ay-Dy125,inbox.pmax.z);
	 inflst[0].setVerts(0,1,5);
	 inflst[0].setEdgeVisFlags(1,1,0);
	 inflst[0].setSmGroup(2);
	 inflst[1].setVerts(0,5,4);
	 inflst[1].setEdgeVisFlags(0,1,1);
	 inflst[1].setSmGroup(2);
	 inflst[2].setVerts(1,2,6);
	 inflst[2].setEdgeVisFlags(1,1,0);
	 inflst[2].setSmGroup(4);
	 inflst[3].setVerts(1,6,5);
	 inflst[3].setEdgeVisFlags(0,1,1);
	 inflst[3].setSmGroup(4);
	 inflst[4].setVerts(2,3,7);
	 inflst[4].setEdgeVisFlags(1,1,0);
	 inflst[4].setSmGroup(2);
	 inflst[5].setVerts(2,7,6);
	 inflst[5].setEdgeVisFlags(0,1,1);
	 inflst[5].setSmGroup(2);
	 inflst[6].setVerts(3,0,4);
	 inflst[6].setEdgeVisFlags(1,1,0);
	 inflst[6].setSmGroup(4);
	 inflst[7].setVerts(3,4,7);
	 inflst[7].setEdgeVisFlags(0,1,1);
	 inflst[7].setSmGroup(4);
	 inflst[8].setVerts(0,3,2);
	 inflst[8].setEdgeVisFlags(1,1,0);
	 inflst[8].setSmGroup(1);
	 inflst[9].setVerts(0,2,1);
	 inflst[9].setEdgeVisFlags(0,1,1);
	 inflst[9].setSmGroup(1);
	 inflst[10].setVerts(4,5,6);
	 inflst[10].setEdgeVisFlags(1,1,0);
	 inflst[10].setSmGroup(1);
	 inflst[11].setVerts(4,6,7);
	 inflst[11].setEdgeVisFlags(0,1,1);
	 inflst[11].setSmGroup(1);
}
void PScatterObject::GetPosInfo(int x,int y,int z,int sx,int sy,int sz,float *rotx,float *roty,float *rotz,TimeValue t)
{   pblock->GetValue(x,t,*rotx,ivalid);
	pblock->GetValue(y,t,*roty,ivalid);
	pblock->GetValue(z,t,*rotz,ivalid);
	if (!(flags&sx))
	  *rotx=(flags&sy?*roty:*rotz);
	if (!(flags&sy))
	  *roty=(flags&sx?*rotx:*rotz);
	if (!(flags&sz))
	  *rotz=(flags&sx?*rotx:*roty);
	}
typedef Mesh* mp;
typedef struct
{ mp MeshPtr;
  int vertsoff,prevs;
  int *pmv;
}MeshPtrinfo;
typedef struct
{ int meshnum,count;
  TimeValue t;
} Meshinfo;

int __cdecl timesort( const void *arg1, const void *arg2 )
{  TimeValue v1=((Meshinfo *)arg1)->t,v2=((Meshinfo *)arg2)->t;
	if (v1<v2) return 1;
	if (v1==v2) return 0;
	return -1;
}

BOOL PScatterObject::UpdateMesh(TimeValue t,BOOL force,BOOL sel)
{
  Point3 Zero=Point3(0.0f,0.0f,0.0f);
  SavePt *center;
  int seed,vertexnum=0,face=0,dverts,dfaces,dtv,inrend=TestFlag(PSCAT_INRENDER); 
  int cnum=0;
  if ((!ivalid.InInterval(t))||force)
  { ivalid = FOREVER;
	BOOL needsDel1, needsDel2;

    pblock->GetValue(PB_RANDOMSEED,0,seed,FOREVER);
	TriObject *tob1, *tob2;
	tob1 = GetTriObject(t,ob1,ivalid,needsDel1);
	tob2 = GetTriObject(t,ob2,ivalid,needsDel2);
	Matrix3 mat1;mat1.IdentityMatrix();
	int number,dmeth,mperp,numcopies=0;
	int inverts,infaces,hdist;
	TimeValue anioff=0;
	pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
    srand(seed);
	float bscale;
	pblock->GetValue(PB_DISTMETHOD,0,dmeth,FOREVER);
	pblock->GetValue(PB_NUMBER,t,number,ivalid); 
	if (number < 0) number = 0;
	pblock->GetValue(PB_BASESCALE,t,bscale,ivalid);
	pblock->GetValue(PB_MAKEPERP,t,mperp,ivalid);
	numcopies=number;
	center=NULL;
	BOOL dnotV=dmeth;
	if (tob1)
	{ ob1verts=tob1->GetMesh().getNumVerts();
	  ob1faces=tob1->GetMesh().getNumFaces();
	} else {ob1verts=0;ob1faces=0;}
 	pblock->GetValue(PB_ANIOFF,0,anioff,ivalid);
	if (dmeth)
	{ if (tob1 && tob2) 
	  {/*		HCURSOR hCur;
		if (faces > LOTSOFACES) hCur = SetCursor(LoadCursor(NULL,IDC_WAIT));
		GetAsyncKeyState(VK_ESCAPE);  */
		mat1 = GetOpTM(t,1,&ivalid);
		int scatby,nfaces,dsfaces;
	    dverts=inverts=tob2->GetMesh().getNumVerts();
	    dfaces=infaces=tob2->GetMesh().getNumFaces();
		if (hdist){ dverts=0;dfaces=0;dtv=-1;}
		dtv=tob2->GetMesh().getNumTVerts();
		Face *flst;
		pblock->GetValue(PB_DOONSELFACES,t,dsfaces,ivalid);
		pblock->GetValue(PB_DISTMETHOD2,0,scatby,FOREVER);
		if (scatby==VOLUME)
		{ if (infaces==0) numcopies=0;
		  else
		  { Point3 *fnorms=NULL;
		    int nf=tob2->GetMesh().getNumFaces();
			fnorms=new Point3[nf];
			Point3 v1;
			Face *face=tob2->GetMesh().faces;
			for (int fi=0; fi<nf; fi++) 
			{ v1 = tob2->GetMesh().verts[face[fi].v[1]];
			  fnorms[fi] = Normalize((v1-tob2->GetMesh().verts[face[fi].v[0]])^(tob2->GetMesh().verts[face[fi].v[2]]-v1));
			}
			center=new SavePt[numcopies];
		    Box3 bbox=tob2->GetMesh().getBoundingBox();
		    Point3 box=bbox.pmax-bbox.pmin;
		    if ((box.x<EPSILON)||(box.y<EPSILON)||(box.z<EPSILON)) //emitter volume isn't
			  FillByNoVolume(&(tob2->GetMesh()),numcopies,center,bbox,box);
		    else //assume we have a logically arranged mesh
			  FillByVolume(&(tob2->GetMesh()),numcopies,center,bbox,box,fnorms);
			if (fnorms) delete[] fnorms;
			for (int i=0;i<numcopies;i++)
			{ center[i].pt=center[i].pt*mat1;
			  center[i].norm=ZNorm;
			  center[i].x=XNorm;
			}
			dnotV=0;
		  }
//		  dverts=0;dfaces=0;dtv=-1;
		}
		else
		{ BOOL makevlst=(scatby==VERTICES);
		  int svlst=inverts;
		  if (dsfaces)
		  { flst=new Face[infaces];
		    assert(flst);
		    int sface=infaces;infaces=0;
		    for (int fcnt=0;fcnt<sface;fcnt++)
			{ if (tob2->GetMesh().faceSel[fcnt])
		        flst[infaces++]=tob2->GetMesh().faces[fcnt];
			}
		    if (infaces==0) {svlst=0;numcopies=0;}
		    else if (makevlst)
			{ int face=0;svlst=0;
		      for (int vcnt=0;vcnt<inverts;vcnt++)
			  { face=0;
			    if (VertexinWhichFace(vcnt,flst,infaces,&face))
			      svlst++;
			  }
			}
		  }
		 else
		  flst=tob2->GetMesh().faces;
		  Point3 *vlst;
		  vlst=new Point3[inverts];
		  assert(vlst);
		  for (int iv=0;iv<inverts;iv++)
		    vlst[iv]=tob2->GetMesh().verts[iv]*mat1;
		  pblock->GetValue(PB_EVERYNFACES,t,nfaces,ivalid);
		  fEdge *edgelst;
		  if (scatby==VERTICES) numcopies=svlst;
		  else if (scatby==FCENTER) numcopies=infaces;
		  else if (scatby==ECENTER)
		  { if ((infaces==0)||(svlst==0)) numcopies=0;
		    else
			{ int i,numedges=3*infaces;
		      numcopies=0;
		      assert((edgelst=new fEdge[numedges])!=NULL);
	          for (i=0;i<infaces;i++)
			    GetEdgeArea(flst,vlst,NULL,edgelst,&numcopies,i);
		      if ((numcopies==0)&&(edgelst)) delete[] edgelst;
			}
		  }
		  else if ((infaces==0)||(inverts==0)) numcopies=0;
		  if (numcopies>0)
		  {	center=new SavePt[numcopies];
			if (scatby==VERTICES) FillInVertex(inverts,infaces,flst,vlst,&center[0]);
			else if (scatby==FCENTER) FillInFaces(infaces,inverts,flst,vlst,&center[0]);
			else if (scatby==EDGES) FillByEdges(infaces,number,flst,vlst,&center[0]);
			else if (scatby==ECENTER) FillEdgeCenter(numcopies,edgelst,vlst,&center[0]);
			else if (scatby==UNIFORM) FillInUniform(infaces,number,flst,vlst,&center[0]);
			else if (scatby==NFACES)  FillByXFaces(infaces,number,flst,vlst,&center[0],nfaces,0);
			else FillByXFaces(infaces,number,flst,vlst,center,(int)((float)infaces/numcopies),scatby!=EVEN);
//			if (faces > LOTSOFACES) SetCursor(hCur);
		  }
		  if (dsfaces) { if (flst) delete[] flst; }
		  if (vlst) delete[] vlst;
		}
		}
	  else 
	  { mesh.DeepCopy(&tob1->GetMesh(), GEOM_CHANNEL|TOPO_CHANNEL|TEXMAP_CHANNEL|DISP_ATTRIB_CHANNEL);
	    mesh.InvalidateGeomCache(); 
	    if (needsDel1) tob1->DeleteThis();
	    return ivalid.InInterval(t);
	  }
	}
	else
	{ center=new SavePt[numcopies];
	  for (int i=0;i<numcopies;i++)
	    center[i].pt=Zero;
	  dverts=0;dfaces=0;dtv=-1;
	}
	float anirange=0;
	int oneframe=GetTicksPerFrame();
	if (anioff>0)
	{ 
	  anirange=(float)anioff/oneframe;
	}
	BOOL Proxy,showless;	
	int intvs=0, tvcount=0,dmesh,allcopies=numcopies;
	float dpercent;
	pblock->GetValue(PB_DISPLAYMETHOD,t,dmesh,ivalid);
	pblock->GetValue(PB_DISPLAYPORTION,t,dpercent,ivalid);
	showless=(!inrend)&&(dpercent<1.0f);
	Proxy=((dmesh==0)&&(!inrend));
	float offby=0.0f,ob;
	int skipby=1;
	if ((numcopies>0)&&(showless) )
	{ if (dpercent==0.0f) numcopies=0;
	  else {
	  skipby=(int)(ob=1.0f/dpercent)+1;
	  ob=ob+1-skipby;
	  numcopies=1+(int)(numcopies-1)/skipby;
	  offby=(dpercent*allcopies-numcopies)/(float)numcopies;
	  }
	}
	int tverts=0,tottvs=0,totfaces=0;
	MeshPtrinfo *meshlst=NULL;
	Meshinfo *anistuff=NULL;
	int *mnum=NULL,vertsoff=0;
    int tob1map=0,tob2map=0,totalmaps=0;
	totalmaps=nummaps(tob1,tob2,hdist,tob1map,tob2map,Proxy);
	int *maplst=(tob1map?new int[tob1map]:NULL);
	int *mtob1=(tob1map?new int[tob1map]:NULL);
	int *mtob2=(tob2map?new int[tob2map]:NULL);
	int *mapvcnt=(tob1map?new int[tob1map]:NULL);
	int mcnt1=2,mcnt2=2;
	for (int mx=2;((mcnt1<tob1map)||(mcnt2<tob2map))&&(mx<mmap);mx++)
	{ if (tob1->GetMesh().mapSupport(mx)) 
			{mtob1[mcnt1]=mx;mcnt1++;}
	  if (tob2->GetMesh().mapSupport(mx)) 
			{mtob2[mcnt2]=mx;mcnt2++;}
	}
	for (int ml=0;ml<tob1map;ml++) maplst[ml]=0;
	if (numcopies>0)
	{ int holdrnd=rand();
	  mnum=new int[numcopies];assert(mnum);memset(mnum,0,numcopies*sizeof(int));
	  srand(seed);
	  if (anioff==0) 
	  { if (Proxy)
		{ vertsoff=tob1->GetMesh().getNumVerts()-8;inverts=8;infaces=12;intvs=0;}
	    else
		{ inverts=tob1->GetMesh().getNumVerts();
	      infaces=tob1->GetMesh().getNumFaces();
	      intvs=tob1->GetMesh().getNumTVerts();
	      vertsoff=8-tob1->GetMesh().getNumVerts();
		  for (ml=2;ml<tob1map;ml++) maplst[ml]=tob1->GetMesh().getNumMapVerts(mtob1[ml]);
		}
	    tverts=(numcopies*inverts);
	    totfaces=(numcopies*infaces);
	  }
	  else
	  { anistuff=new Meshinfo[allcopies];assert(anistuff);
	    int next=0,numcount=0;
	    for (int i=0;i<allcopies;i++)
		{ TimeValue newt=t+((TimeValue)(anirange*RND11())*oneframe);
		  if (i==next)
		  { BOOL found=FALSE;
		  	for (int j=0;(!found)&&(j<cnum);j++) 
			{ if (newt==anistuff[j].t) {found=TRUE;mnum[numcount]=j;anistuff[j].count++;}}
			if (!found) {anistuff[cnum].meshnum=cnum;anistuff[cnum].t=newt;anistuff[j].count=1;mnum[numcount]=cnum;cnum++;}
		    next+=skipby;numcount++;
		  }
		}
		qsort(anistuff,cnum,sizeof(Meshinfo),timesort);
	    meshlst=new MeshPtrinfo[cnum];assert(meshlst);BOOL needsDel;int last=0,texverts=0;
		for (i=0;i<cnum;i++)
		{ int numvs=8,numfs=12,numtvs=0;
/*		  iszero=-1;
		  if (anistuff[i].t==t) 
		  { iszero=anistuff[i].meshnum;
		    meshlst[anistuff[i].meshnum].MeshPtr=&tob1->GetMesh();
			if (!Proxy) 
			{ numvs=tob1->GetMesh().getNumVerts();numfs=tob1->GetMesh().getNumFaces();
			  numtvs=tob1->GetMesh().getNumTVerts();
			}
		  }
		  else*/
		  for (ml=2;ml<tob1map;ml++)
		  {	if (ml==2) meshlst[anistuff[i].meshnum].pmv[ml]=maplst[ml];
		    else meshlst[anistuff[i].meshnum].pmv[ml]+=maplst[ml];
		  }
		  { TriObject *tri=GetTriObject(anistuff[i].t,ob1,ivalid,needsDel);
		    meshlst[anistuff[i].meshnum].MeshPtr=new Mesh;
			if (Proxy)
			{ Box3 inbox=tri->GetMesh().getBoundingBox();
			  meshlst[anistuff[i].meshnum].vertsoff=8-tri->GetMesh().getNumVerts();
			  meshlst[anistuff[i].meshnum].MeshPtr->setNumVerts(8);
			  meshlst[anistuff[i].meshnum].MeshPtr->setNumFaces(12);
			  MakeProxy(meshlst[anistuff[i].meshnum].MeshPtr->verts,meshlst[anistuff[i].meshnum].MeshPtr->faces,inbox);
	          if (needsDel) tri->DeleteThis();
			}
			else
			{ meshlst[anistuff[i].meshnum].MeshPtr->DeepCopy(&tri->GetMesh(), GEOM_CHANNEL|TOPO_CHANNEL|TEXMAP_CHANNEL|DISP_ATTRIB_CHANNEL);
			  numvs=meshlst[anistuff[i].meshnum].MeshPtr->getNumVerts();
			  numfs=meshlst[anistuff[i].meshnum].MeshPtr->getNumFaces();
			  meshlst[anistuff[i].meshnum].vertsoff=numvs-8;
			  Point3 *vlst=meshlst[anistuff[i].meshnum].MeshPtr->verts;
			  for (int k=0;k<numvs;k++) vlst[k]=vlst[k]*bscale;
	          if (needsDel) tri->DeleteThis();
		      intvs+=(numtvs=meshlst[anistuff[i].meshnum].MeshPtr->getNumTVerts());
		      for (ml=2;ml<tob1map;ml++) maplst[ml]+=meshlst[anistuff[i].meshnum].MeshPtr->getNumMapVerts(mtob1[ml]);
			}
		  }
		  tverts+=numvs*anistuff[i].count;
		  totfaces+=numfs*anistuff[i].count;
		  meshlst[anistuff[i].meshnum].prevs=texverts;
		  texverts+=numtvs;
		}
	   tob1 = GetTriObject(t,ob1,ivalid,needsDel1);
	  }
	  srand(holdrnd);
	}
	tottvs=intvs+(dtv>0?dtv:0);
	ob1verts=tverts;ob1faces=totfaces;
	totfaces+=dfaces;
	mesh.setNumVerts(tverts+dverts);
	mesh.setNumFaces(totfaces);
	BOOL tvs=(((numcopies>0)&&(intvs>0))||(dtv>0));
	if ((tvs)&&((intvs==0)||(dtv==0))) tottvs++;
	mesh.setNumTVerts(tottvs);
	mesh.setNumTVFaces(tvs?totfaces:0);
	mesh.setNumMaps(totalmaps);
	int chnum=2,lastm=2;
	int t1f=totfaces-dfaces;
    for (ml=2;ml<tob1map;ml++)
	{ mesh.setMapSupport (ml, TRUE);
	  mesh.setNumMapFaces(ml, totfaces);
	  mesh.setNumMapVerts(ml,maplst[ml]+1);
	  mesh.setMapVert(ml,maplst[ml],Zero);
	  TVFace *nmapf = mesh.mapFaces(ml);	 
	  TVFace defface(maplst[ml],maplst[ml],maplst[ml]);
	  for (int mv=t1f;mv<totfaces;mv++) nmapf[mv]=defface;
	} if (chnum<tob1map) chnum=tob1map;
    for (ml=chnum;ml<totalmaps;ml++)
	{ mesh.setMapSupport (ml, TRUE);
	  mesh.setNumMapFaces(ml, totfaces);
	  Mesh *m1=&(tob2->GetMesh());int c=mtob2[lastm];
	  UVVert *mapv = m1->mapVerts(c);
	  int vnum=m1->getNumMapVerts(c);
	  mesh.setNumMapVerts(ml,vnum+1);
	  mesh.setMapVert(ml,vnum,Zero);
	  for (int mv=0;mv<vnum;mv++)
		  mesh.setMapVert(ml,mv,mapv[mv]);
	  TVFace *mapf = m1->mapFaces(c);TVFace *nmapf = mesh.mapFaces(ml);
	  int t1f=totfaces-dfaces;
	  TVFace defface(vnum,vnum,vnum);
	  for (mv=0;mv<t1f;mv++) nmapf[mv]=defface;
	  int old=0;
	  for (mv=t1f;mv<totfaces;mv++)
	  {	  nmapf[mv]=mapf[old];old++;}
	  lastm++;
	}
	face=0;
	int addvert=0,i;
	if (numcopies>0)
	{ Point3 *tmpvs=NULL,*invlst=NULL;
	  Face *inflst=NULL;
  	  int inob1=inverts;
	  if (anioff==0)
	  { tmpvs=new Point3[inverts];
	    if (Proxy) 
		{ invlst=new Point3[inverts];
	      inflst=new Face[infaces];
	      Box3 inbox=tob1->GetMesh().getBoundingBox();
	      MakeProxy(invlst,inflst,inbox);
		}
	    else {invlst=tob1->GetMesh().verts;inflst=tob1->GetMesh().faces;}
		for (int i=0;i<inob1;i++)
		  tmpvs[i]=invlst[i]*bscale;
	  }
	float scalex,scaley,scalez;
	int slockon;
	pblock->GetValue(PB_ASPECTLOCK,0,slockon,FOREVER);
   	float vchaos;
	pblock->GetValue(PB_VERTEXCHAOS,t,vchaos,ivalid);
   	float rotx,rotz,roty,tranx,trany,tranz,tranfx,tranfy,tranfz,bscalex,bscaley,bscalez;
	GetPosInfo(PB_ROTX,PB_ROTY,PB_ROTZ,PSCAT_ROTX,PSCAT_ROTY,PSCAT_ROTZ,&rotx,&roty,&rotz,t);
	rotx*=PIOver180;roty*=PIOver180;rotz*=PIOver180;
	GetPosInfo(PB_TRANX,PB_TRANY,PB_TRANZ,PSCAT_TRANX,PSCAT_TRANY,PSCAT_TRANZ,&tranx,&trany,&tranz,t);
	GetPosInfo(PB_TRANFX,PB_TRANFY,PB_TRANFZ,PSCAT_TRANFX,PSCAT_TRANFY,PSCAT_TRANFZ,&tranfx,&tranfy,&tranfz,t);
	GetPosInfo(PB_SCALEX,PB_SCALEY,PB_SCALEZ,PSCAT_SCALEX,PSCAT_SCALEY,PSCAT_SCALEZ,&bscalex,&bscaley,&bscalez,t);
	bscalex*=0.01f;
	bscaley*=0.01f;
	bscalez*=0.01f;
	int curcopy=0;
	float cumerr=0.0f;
	Point3 *curverts=tmpvs;int ncount=0;
	int evenuprnd=vertsoff;
	while (curcopy<allcopies)
	{ float tmpsave=RND11();
	  if (cnum>0)
	  { curverts=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr->verts;
	    inob1=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr->getNumVerts();
	  }
	  else {curverts=tmpvs;inob1=inverts;}
	  if (anioff>0) evenuprnd=meshlst[anistuff[mnum[ncount]].meshnum].vertsoff;
	  ncount++;
	  scalex=1.0f+bscalex*tmpsave;
	  if (slockon) 
	  { scaley=(scalez=scalex);RND11();RND11();}
	  else
	  { scaley=1.0f+bscaley*RND11();
	    scalez=1.0f+bscalez*RND11();
	  }
	  float alpha,beta,gamma,calpha,cbeta,cgamma,salpha,sbeta,sgamma;
	  calpha=(float)cos(alpha=RND11()*rotx);salpha=(float)sin(alpha);
	  cbeta=(float)cos(beta=RND11()*roty); sbeta=(float)sin(beta);
	  cgamma=(float)cos(gamma=RND11()*rotz);sgamma=(float)sin(gamma);
	  Matrix4By4 rotmat;
	  rotmat[0][0]=cbeta*cgamma;
	  rotmat[0][1]=salpha*sbeta*cgamma-calpha*sgamma;
	  rotmat[0][2]=calpha*sbeta*cgamma+salpha*sgamma;
	  rotmat[0][3]=RND11()*tranx;
	  rotmat[1][3]=RND11()*trany;
	  rotmat[2][3]=RND11()*tranz;
	  rotmat[3][0]=rotmat[3][1]=rotmat[3][2]=0.0f;
	  rotmat[3][3]=1.0f;
	  rotmat[1][0]=cbeta*sgamma;
	  rotmat[1][1]=salpha*sbeta*sgamma+calpha*cgamma;
	  rotmat[1][2]=calpha*sbeta*sgamma-salpha*cgamma;
	  rotmat[2][0]=-sbeta;
	  rotmat[2][1]=salpha*cbeta;
	  rotmat[2][2]=calpha*cbeta;
	  Point3 y;
	  Matrix4By4 omat;
	  if (dmeth) y=Normalize(center[curcopy].norm^center[curcopy].x);
	  if (dmeth && mperp)
	  {	omat[0][0]=center[curcopy].x[0];
	    omat[1][0]=center[curcopy].x[1];
		omat[2][0]=center[curcopy].x[2];
		omat[0][1]=y[0];omat[1][1]=y[1];omat[2][1]=y[2];
		omat[0][2]=center[curcopy].norm[0];
		omat[1][2]=center[curcopy].norm[1];
		omat[2][2]=center[curcopy].norm[2];
		omat[0][3]=center[curcopy].pt.x;
		omat[1][3]=center[curcopy].pt.y;
		omat[2][3]=center[curcopy].pt.z;
		omat[3][0]=omat[3][1]=omat[3][2]=0.0f;omat[3][3]=1.0f;
	  }
	  Point3 offset;
	  if (dmeth) offset=center[curcopy].x*RND11()*tranfx+y*RND11()*tranfy+center[curcopy].norm*RND11()*tranfz;
	  for (int i=0;i<inob1;i++)
	  { mesh.verts[vertexnum].x=curverts[i].x*scalex+RND11()*vchaos;
	    mesh.verts[vertexnum].y=curverts[i].y*scaley+RND11()*vchaos;
	    mesh.verts[vertexnum].z=curverts[i].z*scalez+RND11()*vchaos;
	    Mult4XPoint(&mesh.verts[vertexnum],rotmat);
	    if (dmeth && mperp)
	    { Mult4XPoint(&mesh.verts[vertexnum],omat);
	    } else {mesh.verts[vertexnum]+=center[curcopy].pt;}
	    if (dmeth)
	      mesh.verts[vertexnum]+=offset;
	    vertexnum++;
	  }
	  for (int evn=0;evn<evenuprnd;evn++)
	  { rand();rand();rand(); }
	  curcopy+=skipby;cumerr=cumerr+offby;
	}
	if (tmpvs) delete[] tmpvs;
	int j=0,cvs=0;
	if (center) delete[] center;
	ncount=0;
	while (j<allcopies)
	{ int curf;Face *flst;
	  if (cnum>0)
	  { flst=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr->faces;
	    curf=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr->getNumFaces();
		cvs=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr->getNumVerts();
	  }
	  else {flst=inflst;curf=infaces;cvs=inverts;}
	  for (i=0;i<curf;i++)
	  { memcpy(&mesh.faces[face],&flst[i],sizeof(Face));
	    if (firstmtls==1) mesh.faces[face].setMatID(0);
		mesh.faces[face].v[0]+=addvert;
		mesh.faces[face].v[1]+=addvert;
		mesh.faces[face].v[2]+=addvert;
	    if (tvs) 
		{ if (intvs>0)
		  { if (cnum>0)
			{ Mesh *m=meshlst[anistuff[mnum[ncount]].meshnum].MeshPtr;
			  TVFace tvf=m->tvFace[i];
			  int prevs=meshlst[anistuff[mnum[ncount]].meshnum].prevs;
			  mesh.tvFace[face].setTVerts(tvf.t[0]+prevs,tvf.t[1]+prevs,tvf.t[2]+prevs);		  
			  for (int mv=2;mv<tob1map;mv++)
			  { TVFace mtvf=m->mapFaces(mtob1[mv])[i];
			    prevs=meshlst[anistuff[mnum[ncount]].meshnum].pmv[mv];
			    mesh.mapFaces(mv)[face]=TVFace(mtvf.t[1]+prevs,mtvf.t[1]+prevs,mtvf.t[2]+prevs);
			  }
			}
			else 
			{ for (int mv=2;mv<tob1map;mv++)
			    mesh.mapFaces(mv)[face]=tob1->GetMesh().mapFaces(mtob1[mv])[i];
			  mesh.tvFace[face]=tob1->GetMesh().tvFace[i];
			}
		  }	
		  else mesh.tvFace[face].setTVerts(dtv,dtv,dtv);
		}
		face++;
	  }
	  addvert+=cvs;
	  j+=skipby;ncount++;
	}
	if (Proxy) 
	{ if (inflst) delete[] inflst;
	  if (invlst) delete[] invlst;
	}
	tvcount=intvs;
	if (intvs>0)
	{ if (cnum==0)
	  { for (i=0;i<intvs;i++)
		  mesh.tVerts[i]=tob1->GetMesh().tVerts[i];
	    for (j=2;j<tob1map;j++)
		{ int c=mtob1[j];UVVert *mapv = tob1->GetMesh().mapVerts(c);
		  for (i=0;i<tob1->GetMesh().getNumMapVerts(c);i++)
			mesh.setMapVert(j,i,mapv[i]);
		}
	  }
	  else
	  { tvcount=0;for (j=2;j<tob1map;j++) mapvcnt[j]=0;
		for (i=0;i<cnum;i++)
		{ for (j=0;j<meshlst[anistuff[i].meshnum].MeshPtr->getNumTVerts();j++)
		  { mesh.tVerts[tvcount]=meshlst[anistuff[i].meshnum].MeshPtr->tVerts[j];
		    tvcount++;
		  }
		  for (j=2;j<tob1map;j++)
		  { Mesh *m=meshlst[anistuff[i].meshnum].MeshPtr;int c=mtob1[j];
			UVVert *mapv = m->mapVerts(c);
		    for (i=0;i<m->getNumMapVerts(c);i++)
			{ mesh.setMapVert(j,mapvcnt[j],mapv[i]);(mapvcnt[j])++;}
		  }
		}
	  }
	}
	if (tvs)
	{ if (intvs==0) {mesh.tVerts[dtv]=Zero;}
	  else if (dtv==0) mesh.tVerts[tvcount]=Zero;}
	} else tvcount=0;
	for (i=0;i<dtv;i++)
	  mesh.tVerts[i+tvcount]=tob2->GetMesh().tVerts[i];
	if ((tob2)&&(!hdist))
	{ for (i=0;i<dverts;i++)
	    mesh.verts[vertexnum++]=tob2->GetMesh().verts[i]*mat1;
	  for (i=0;i<dfaces;i++)
	  {	memcpy(&mesh.faces[face],&tob2->GetMesh().faces[i],sizeof(Face));
		mesh.faces[face].v[0]+=addvert;
		mesh.faces[face].v[1]+=addvert;
		mesh.faces[face].v[2]+=addvert;
		mesh.faces[face].setMatID(firstmtls+(secondmtls==1?0:tob2->GetMesh().faces[i].getMatID()));		
	    if (tvs) 
		{ if (dtv>0) mesh.tvFace[face].setTVerts(tob2->GetMesh().tvFace[i].t[0]+tvcount,tob2->GetMesh().tvFace[i].t[1]+tvcount,tob2->GetMesh().tvFace[i].t[2]+tvcount);
		  else mesh.tvFace[face].setTVerts(tvcount,tvcount,tvcount);
		}
		face++;
	  }
	}
	if (maplst) delete[] maplst;if (mapvcnt) delete[] mapvcnt;
	if (mtob1) delete[] mtob1;if (mtob2) delete[] mtob2;
	for (i=0;i<cnum;i++) delete meshlst[i].MeshPtr;
	if (meshlst) delete[] meshlst;
	if (anistuff) delete[] anistuff;
	if (mnum) delete[] mnum;
	if (needsDel1) tob1->DeleteThis();
	if (needsDel2) tob2->DeleteThis();
	mesh.InvalidateGeomCache(); 
  } 
  else 
  { if (!ivalid.InInterval(t)) 
    {	ivalid.SetInstant(t); }
  }
  return ivalid.InInterval(t);
}

Interval PScatterObject::ObjectValidity(TimeValue t)
	{ 
	UpdateMesh(t);
	if (ivalid.Empty()) return Interval(t,t);
	else return ivalid;
	}

int PScatterObject::CanConvertToType(Class_ID obtype)
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

Object* PScatterObject::ConvertToType(TimeValue t, Class_ID obtype)
{/*	int dmeth;pblock->GetValue(PB_DISTMETHOD,0,dmeth,FOREVER);
	if (obtype==defObjectClassID||obtype==triObjectClassID||obtype==mapObjectClassID) {
		if (ob1 && (ob2 ||(!dmeth))) {
			TriObject *triob;
			UpdateMesh(t);
			triob = CreateNewTriObject();
			triob->GetMesh() = mesh;
			triob->SetChannelValidity(TOPO_CHAN_NUM,ObjectValidity(t));
			triob->SetChannelValidity(GEOM_CHAN_NUM,ObjectValidity(t));
			return triob;
		} else {
			// RB 4-11-96:
			// There was a bug where if one of the operands was a tri-object
			// it would convert itself to a tri-object by just returning itself (as it should)
			// The problem is that there are places in the system where the
			// the system would think it needed to delete the tri-object
			// becuase it was not equal to the boolean object. In other words,
			// it thinks that the boolean convert itself to a tri-object and
			// therefore the tri-object was a temporary object.
			// So what this code does is clone the tri-object in this case
			// so that the boolean object will always return a temporary
			// object.
			Object *obj = NULL;
			if (ob1) {
				obj = ob1->ConvertToType(t,obtype);
				if (obj && obj==ob1) {
					return (Object*)obj->Clone();
				} else {
					return obj;
					}
				}
			if (ob2) {
				obj = ob2->ConvertToType(t,obtype);
				if (obj && obj==ob2) {
					return (Object*)obj->Clone();
				} else {
					return obj;
					}
				}			
			return NULL;
			}
	} else {
		return Object::ConvertToType(t,obtype);
		}*/
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

ObjectState PScatterObject::Eval(TimeValue time)
	{
	return ObjectState(this);
	}

void PScatterObject::SetExtractButtonState()
{	ICustButton *iBut = GetICustButton(GetDlgItem(hObject,IDC_SCATR_EXTRACTOP));
	if (!creating && (flags & (PSCAT_OB1SEL|PSCAT_OB2SEL))) {
		iBut->Enable();
		EnableWindow(GetDlgItem(hObject,IDC_SCATR_EXTRACT_INSTANCE),TRUE);
		EnableWindow(GetDlgItem(hObject,IDC_SCATR_EXTRACT_COPY),TRUE);
	} else {
		iBut->Disable();
		EnableWindow(GetDlgItem(hObject,IDC_SCATR_EXTRACT_INSTANCE),FALSE);
		EnableWindow(GetDlgItem(hObject,IDC_SCATR_EXTRACT_COPY),FALSE);
		}
	ReleaseICustButton(iBut);	

	CheckDlgButton(hObject,IDC_SCATR_EXTRACT_INSTANCE,!extractCopy);
	CheckDlgButton(hObject,IDC_SCATR_EXTRACT_COPY,extractCopy);
}


BOOL PickOperand::Filter(INode *node)
	{
	if (node) {
		ObjectState os = node->GetObjectRef()->Eval(bo->ip->GetTime());
		if (os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
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

BOOL PickOperand::HitTest(
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

BOOL PickOperand::Pick(IObjParam *ip,ViewExp *vpt)
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
	Matrix3 ourTM;
	Matrix3 ntm = node->GetObjectTM(ip->GetTime());	
	INode *mainnode;
	theHold.Begin();
	theHold.Put(new CreateScatrref(bo,bo->opbName,node->GetName()));

	if (bo->creating) {
		ourTM = bo->createTM;
		mainnode=bo->mainnode;
	} else {
		if (bo->mainnode) {
			ourTM = bo->mainnode->GetObjectTM(ip->GetTime());
		} else {
			MaxAssert(0);
			ourTM.IdentityMatrix();
			}
		}

	bool canUndo = TRUE;
	bo->SetOperand(node,ourTM,ntm,1,bo->mainnode,&canUndo);	
	if (!canUndo) GetSystemSetting (SYSSET_CLEAR_UNDO);
	bo->SetFlag(PSCAT_DISPRESULT);
	CheckRadioButton(bo->pmapObjects->GetHWnd(),IDC_PS_DISPRESULT,IDC_PS_DISPOPS,IDC_PS_DISPRESULT);
		
	bo->UpdateMesh(ip->GetTime(),TRUE);
//	bo->ShowOpList();
	
	if (bo->creating) {
		theCreatePScatMode.JumpStart(ip,bo);
		ip->SetCommandMode(&theCreatePScatMode);
		ip->RedrawViews(ip->GetTime());
		return FALSE;
	} else {
		return TRUE;
		}
	}

void PickOperand::EnterMode(IObjParam *ip)
{	ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapDist->GetHWnd(),IDC_AP_PROSCATTER_OBJECTDPICK));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
}

void PickOperand::ExitMode(IObjParam *ip)
{ if (bo->pmapDist)
	{ ICustButton *iBut = GetICustButton(GetDlgItem(bo->pmapDist->GetHWnd(),IDC_AP_PROSCATTER_OBJECTDPICK));
	  if (iBut) iBut->SetCheck(FALSE);
	  ReleaseICustButton(iBut);
	}
}

static BOOL ReadInt(int *buf,FILE *f)
{ return(fread(buf,isize,1,f)==1);
}
static BOOL WriteInt(int *buf,FILE *f)
{ return(fwrite(buf,isize,1,f)==1);
}
BOOL GenNewSaveFile(int osize,int size,int custsettings,FILE *f,TCHAR *filename,int vers)
{ int future=0;BOOL ok=TRUE;
  fclose(f);
  if ((f = _tfopen(filename, _T("r+b"))) == NULL) return FALSE;
  ok=(WriteInt(&custsettings,f)&&WriteInt(&vers,f)&&WriteInt(&size,f)&&WriteInt(&future,f));
 if (custsettings>0)
 { int cset=(custsettings-1),odata=osize+NLEN;
   AName Name;int ofs=(size-osize),tofs=cset*ofs;
   BYTE *data=new BYTE[size];for (int x=0;x<size;x++) data[x]=0;
   cset*=odata;cset+=HLEN;
   while (cset>=HLEN)
   { fseek(f,cset,SEEK_SET);
      if (fread(Name,NLEN,1,f)==1)
	 { if (fread(data,osize,1,f)!=1) goto badend;
       fseek(f,cset+tofs,SEEK_SET);
		if (fwrite(Name,1,NLEN,f)!=NLEN) goto badend;
	   if (fwrite(data,size,1,f)!=1) goto badend;
	 } else goto badend;
	 cset-=odata;tofs-=ofs;
   }
   delete[] data;
   rewind(f);
 }
 return ok;
badend: fclose(f);return FALSE;
 }

void PScatterObject::GetFilename(TCHAR *filename)
{   _tcscpy(filename,ip->GetDir(APP_PLUGCFG_DIR));
  int len= _tcslen(filename);
  if (len)
  {  if (filename[len-1]!=_T('\\'))
		  _tcscat(filename,_T("\\"));
  }
  _tcscat(filename,GetString(IDS_AP_PASCATCST));
}
void PScatterObject::SetupTargetList()		
	{TCHAR filename[MAX_PATH];
     FILE *f;
	 int vers,i,future,osize;

	custsettings=0;
    GetFilename(filename);
	BOOL fileok=TRUE;
	if ((f = _tfopen(filename, _T("rb"))) == NULL) return;
	if((ReadInt(&custsettings,f))&&(ReadInt(&vers,f))&&(ReadInt(&osize,f))&&(ReadInt(&future,f)))
	{ if ((vers!=CURRENT_VERSION)&&(fileok=GenNewSaveFile(osize,size,custsettings,f,filename,CURRENT_VERSION)))
		{fileok=((ReadInt(&custsettings,f))&&(ReadInt(&vers,f))&&(ReadInt(&osize,f))&&(ReadInt(&future,f)));
		}
	if (fileok&&(vers==CURRENT_VERSION))
	{ NameLst=new AName[custsettings]; 
	  for (i=0;i<custsettings;i++)
	  if (fread(NameLst[i],NLEN,1,f)==1)
	    fseek(f,size,SEEK_CUR);
	  else goto onerr;
	}}
	onerr:if (i<custsettings) 
	{ custsettings=0;
	MessageBox (NULL,GetString(IDS_RB_BADFILE),
            "", MB_ICONINFORMATION);
	}
	if (fileok) fclose(f);
	SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),LB_RESETCONTENT,0,0);
	for (i=0; i<custsettings; i++) 
	{ SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),
			LB_ADDSTRING,0,(LPARAM)(TCHAR*)NameLst[i]);
	}
}

int PScatterObject::RemSettings(int overwrite,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  long startpt;
  int vers,newsets,future=0,baselen=size+NLEN;
  GetFilename(filename);
  vers=CURRENT_VERSION;
  newsets=custsettings-1;
  if ((f = _tfopen(filename,_T("r+b"))) == NULL) return 0;
  if (custsettings==1) 
  {fclose(f);remove(filename);custsettings=0;delete[] NameLst;NameLst=NULL;
    SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),LB_RESETCONTENT,0,0);
   return (1);}
 if (!WriteInt(&newsets,f)) {fclose(f);return 0;}
  startpt=overwrite*(baselen)+HLEN;
  fseek(f,startpt,SEEK_SET); 
  int i;
  BYTE *buf;
  buf=new BYTE[baselen];
  assert(buf);
  long cpos=startpt;
  for (i=overwrite+1;i<custsettings;i++)
  {	fseek(f,cpos+baselen,SEEK_SET);
	if (fread(buf,baselen,1,f)!=1) {delete[] buf;fclose(f);return 0;}
	fseek(f,cpos,SEEK_SET);
	if (fwrite(buf,baselen,1,f)!=1) {delete[] buf;fclose(f);return 0;}
	cpos+=baselen;
  }
  delete[] buf;
  _chsize(_fileno(f),ftell(f));
  fclose(f);								  
  AName *Tmp=new AName[custsettings-1];
  int newi=0;
  for (i=0;i<custsettings;i++)
  { if (i!=overwrite) 
      _tcscpy(Tmp[newi++],NameLst[i]);}
  delete []NameLst;NameLst=Tmp;
  custsettings--;
  SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),LB_RESETCONTENT,0,0);
  for (i=0; i<custsettings; i++) 
  {	SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),
			LB_ADDSTRING,0,(LPARAM)(TCHAR*)NameLst[i]);	}
  return(1);
}
int PScatterObject::SaveSettings(int overwrite,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  int vers,newsets,future=0;

  if ((overwrite>-1)&&(MessageBox (NULL,GetString(IDS_AP_SETEXISTS),GetString(IDS_AP_WARNING), MB_ICONQUESTION | MB_YESNO ) == IDNO))
	 return 0;
  GetFilename(filename);
  vers=CURRENT_VERSION;
  newsets=custsettings+1;
  if ((f = _tfopen(filename,(custsettings==0?_T("wb"):_T("r+b")))) == NULL) return 0;
  // longest one
 int plength=PBLOCK_LENGTH;
  if (custsettings==0)
  { if (!(WriteInt(&newsets,f)&&WriteInt(&vers,f)&&WriteInt(&size,f)&&WriteInt(&future,f))) goto errend;
  } 
  else 
  { if (overwrite>=0) 
    { overwrite=overwrite*(size+NLEN)+HLEN;
      fseek(f,overwrite,SEEK_SET); }
    else 
	{ if (!WriteInt(&newsets,f)) goto errend;
	  fseek(f,0,SEEK_END);
	}
  }	 
  int ival,i;
  float fval;
  if (fwrite(newname,1,NLEN,f)!=NLEN) goto errend;
  for (i=0;i<plength;i++)
  {	if (descVer2[i].type==TYPE_INT) 
    { pblock->GetValue(i,0,ival,FOREVER);
      if (fwrite(&ival,isize,1,f)!=1) goto errend;
	}
    else
    { pblock->GetValue(i,0,fval,FOREVER);
      if (fwrite(&fval,fsize,1,f)!=1) goto errend;
	}
  }
  fclose(f);
  if (overwrite<0) 
  { AName *Tmp=new AName[custsettings+1];
    memcpy(Tmp,NameLst,sizeof(AName)*custsettings);
	delete []NameLst;NameLst=Tmp;
    _tcscpy(NameLst[custsettings],newname);
	custsettings++;
	SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),LB_RESETCONTENT,0,0);
	for (int i=0; i<custsettings; i++) {
		SendMessage(GetDlgItem(hParams2,IDC_PS_SETLIST),
			LB_ADDSTRING,0,(LPARAM)(TCHAR*)NameLst[i]);
		}
  }  
  return(1);
errend: fclose(f);return(0);
}
int PScatterObject::GetSettings(int setnum,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  GetFilename(filename);
  if ((f = _tfopen(filename,_T("rb"))) == NULL) return 0;
  { setnum=setnum*(size+NLEN)+HLEN;
    fseek(f,setnum,SEEK_SET); 
  }	 
  int ival,i;
  float fval;
  // longest one
  int plength=PBLOCK_LENGTH;
  if (fread(newname,1,NLEN,f)!=NLEN) goto errend;
  for (i=0;i<plength;i++)
  {	if (descVer2[i].type==TYPE_INT) 
    { if (fread(&ival,isize,1,f)!=1) goto errend;
	  pblock->SetValue(i,0,ival);
	}
    else
    { if (fread(&fval,fsize,1,f)!=1) goto errend;
//	  if (i!=PB_EMITRWID) 
		  pblock->SetValue(i,0,fval);
	}
  }
  fclose(f);
  pmapDisplay->Invalidate();
  pmapObjects->Invalidate();
  pmapTransforms->Invalidate();
  return(1);
  errend: fclose(f);return(0);
}
void SpinnerOff(HWND hWnd,int SpinNum)
{ ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
  spin2->Disable();
  ReleaseISpinner(spin2);
}
void SpinnerOn(HWND hWnd,int SpinNum)
{ ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
  spin2->Enable();
  ReleaseISpinner(spin2);
}

void LockOuts(HWND hWnd,int flags)
{ if (!(flags &PSCAT_ROTX)) SpinnerOff(hWnd,IDC_AP_ROTXSPIN);
  if (!(flags &PSCAT_ROTY)) SpinnerOff(hWnd,IDC_AP_ROTYSPIN);
  if (!(flags &PSCAT_ROTZ)) SpinnerOff(hWnd,IDC_AP_ROTZSPIN);
  if (!(flags &PSCAT_TRANX)) SpinnerOff(hWnd,IDC_AP_TRANXSPIN);
  if (!(flags &PSCAT_TRANY)) SpinnerOff(hWnd,IDC_AP_TRANYSPIN);
  if (!(flags &PSCAT_TRANZ)) SpinnerOff(hWnd,IDC_AP_TRANZSPIN);
  if (!(flags &PSCAT_TRANFX)) SpinnerOff(hWnd,IDC_AP_TRANXFSPIN);
  if (!(flags &PSCAT_TRANFY)) SpinnerOff(hWnd,IDC_AP_TRANYFSPIN);
  if (!(flags &PSCAT_TRANFZ)) SpinnerOff(hWnd,IDC_AP_TRANZFSPIN);
  if (!(flags &PSCAT_SCALEX))	SpinnerOff(hWnd,IDC_AP_SCALEXSPIN);
  if (!(flags &PSCAT_SCALEY))	SpinnerOff(hWnd,IDC_AP_SCALEYSPIN);
  if (!(flags &PSCAT_SCALEZ))	SpinnerOff(hWnd,IDC_AP_SCALEZSPIN);
}

void DistObjectOff(PScatterObject *po,HWND hWnd)
{ EnableWindow(GetDlgItem(po->pmapDisplay->GetHWnd(),IDC_AP_HIDEDIST),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_GOPERP),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_GOSELECT),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTVOLUME),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTCENTERS),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTVERTICES),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTAREA),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTEVEN),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTNFACES),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTRANDOM),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTEDGES),FALSE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTECENTER),FALSE);
  HWND thWnd=po->pmapTransforms->GetHWnd();
  EnableWindow(GetDlgItem(thWnd,IDC_AP_TRANFLOCK),FALSE);
  SpinnerOff(thWnd,IDC_AP_TRANXFSPIN);
  SpinnerOff(thWnd,IDC_AP_TRANYFSPIN);
  SpinnerOff(thWnd,IDC_AP_TRANZFSPIN);
  HWND dhWnd=po->pmapDist->GetHWnd();
  ICustButton *iBut = GetICustButton(GetDlgItem(dhWnd,IDC_AP_PROSCATTER_OBJECTDPICK));
  if (iBut) iBut->Disable();
  ReleaseICustButton(iBut);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTREF),FALSE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTCOPY),FALSE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTMOVE),FALSE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTINST),FALSE);
}
void DistObjectOn(PScatterObject *po,HWND hWnd)
{ EnableWindow(GetDlgItem(po->pmapDisplay->GetHWnd(),IDC_AP_HIDEDIST),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_GOPERP),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_GOSELECT),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTVOLUME),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTCENTERS),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTVERTICES),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTAREA),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTEVEN),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTNFACES),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTRANDOM),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTEDGES),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_DISTECENTER),TRUE);
  HWND thWnd=po->pmapTransforms->GetHWnd();
  EnableWindow(GetDlgItem(thWnd,IDC_AP_TRANFLOCK),TRUE);
  if (po->flags &PSCAT_TRANFX) SpinnerOn(po->thwnd,IDC_AP_TRANXFSPIN);else SpinnerOff(po->thwnd,IDC_AP_TRANXFSPIN);
  if (po->flags &PSCAT_TRANFY) SpinnerOn(po->thwnd,IDC_AP_TRANYFSPIN);else SpinnerOff(po->thwnd,IDC_AP_TRANYFSPIN);
  if (po->flags &PSCAT_TRANFZ) SpinnerOn(po->thwnd,IDC_AP_TRANZFSPIN);else SpinnerOff(po->thwnd,IDC_AP_TRANZFSPIN);
  HWND dhWnd=po->pmapDist->GetHWnd();
  ICustButton *iBut = GetICustButton(GetDlgItem(dhWnd,IDC_AP_PROSCATTER_OBJECTDPICK));
  if (iBut) iBut->Enable();
  ReleaseICustButton(iBut);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTREF),TRUE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTCOPY),TRUE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTMOVE),TRUE);
  EnableWindow(GetDlgItem(dhWnd,IDC_AP_DISTINST),TRUE);
}

void DistGreyOuts(PScatterObject *po,HWND hWnd,TimeValue t)
{ int dist;
  CheckDlgButton(hWnd,IDC_PS_DISPRESULT,po->TestFlag(PSCAT_DISPRESULT));
  CheckDlgButton(hWnd,IDC_PS_DISPOPS,!po->TestFlag(PSCAT_DISPRESULT));	
  po->pblock->GetValue(PB_DISTMETHOD,t,dist,FOREVER);
  if (dist) DistObjectOn(po,hWnd);
  else DistObjectOff(po,hWnd);
  po->pblock->GetValue(PB_DISTMETHOD2,t,dist,FOREVER);
  if ((dist<UNIFORM)||(dist==ECENTER)) 
	SpinnerOff(hWnd,IDC_AP_NUMBERSPIN);
  else SpinnerOn(hWnd,IDC_AP_NUMBERSPIN);
}

static BOOL CALLBACK CustomSettingParamDlgProc( 
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ TCHAR newname[NLEN];
  int i,save=0;
  PScatterObject *po = (PScatterObject*)GetWindowLong(hWnd,GWL_USERDATA);
  if (!po && message!=WM_INITDIALOG) return FALSE;

  switch (message) {
		case WM_INITDIALOG: {
			po = (PScatterObject*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			po->custCtrlEdit = GetICustEdit(GetDlgItem(hWnd,IDC_PS_SETEDIT));
			po->custCtrlEdit->SetText(_T(""));
			po->hParams2 = hWnd;
			po->SetupTargetList();
			break;
			}
		case WM_COMMAND:
			{ switch(LOWORD(wParam)) {
				case IDC_PS_SETLIST:  
					if (HIWORD(wParam)==LBN_DBLCLK)
					  goto doload;
					break;
				case IDC_PS_DELETE:  
					i = SendMessage(GetDlgItem(po->hParams2,IDC_PS_SETLIST),
							LB_GETCURSEL,0,0);
					   if (po->custsettings>0)
					   { po->custCtrlEdit->GetText(newname,NLEN);
						 if (_tcscmp(newname,po->NameLst[i])==0)
						   po->custCtrlEdit->SetText(_T(""));
					     po->RemSettings(i,po->NameLst[i]);
					   }
					break;
				case IDC_PS_SAVE:
				   save=1;
				case IDC_PS_LOAD:
					doload:
					if (!save)
					{ i = SendMessage(GetDlgItem(po->hParams2,IDC_PS_SETLIST),
							LB_GETCURSEL,0,0);
					   if ((i>-1)&&(po->custsettings>0))
					     po->custCtrlEdit->SetText(po->NameLst[i]);
					}
				    po->custCtrlEdit->GetText(newname,NLEN);
					if (save) 
					{int tstblk=0,tstlen=_tcslen(newname);
					  while ((tstblk<tstlen)&&(newname[tstblk]==' ')) tstblk++;
					  if (tstblk>=tstlen) 
						MessageBox (NULL,GetString(IDS_AP_NONAME),
            "", MB_ICONINFORMATION);
					  else 
					  { i=0;
					    while ((i<po->custsettings)&&(_tcscmp(newname,po->NameLst[i])))
					     i++;
						if (i>=po->custsettings) i=-1;
					    po->SaveSettings(i,newname);
					  }
					}
					else if ((i>-1)&&(i<po->custsettings))
					{ po->GetSettings(i,newname);
					  LockOuts(po->thwnd,po->flags);
					  DistGreyOuts(po,po->distwnd,po->ip->GetTime());
					  po->ip->RedrawViews(po->ip->GetTime()); 
					}
					else MessageBox (NULL,GetString(IDS_AP_BADNAME),
            "", MB_ICONINFORMATION);
					break;
				}
			break;
			}

		case WM_DESTROY:
			// Release all our Custom Controls
			ReleaseICustEdit(po->custCtrlEdit);
			if (po->NameLst) {delete []po->NameLst;po->NameLst=NULL;}
			return FALSE;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			po->ip->RollupMouseMessage(hWnd,message,wParam,lParam);
			return FALSE;
		
		default:	return FALSE;		
		}
	return TRUE;
	}	

class ProScatPickObj : public ParamMapUserDlgProc {
	public:
		PScatterObject *po;

		ProScatPickObj(PScatterObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {delete this;}
	};

BOOL ProScatPickObj::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	switch (msg) {
		case WM_INITDIALOG: {
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_PROSCATTER_OBJECTDPICK));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
		    ReleaseICustButton(iBut);
			po->ShowName(po->ob2!=NULL);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ case IDC_AP_PROSCATTER_OBJECTDPICK:
				   { if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
					{ if (po->creating) 
						{  theCreatePScatMode.JumpStart(po->ip,po);
							po->ip->SetCommandMode(&theCreatePScatMode);
						} else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
					} else 
						{ po->pickCB.bo = po;						
						  po->ip->SetPickMode(&po->pickCB);
						}
					break;
				}
		default:
			return FALSE;
			}
			break;	
		}
	return TRUE;
}
class ProScatDispProc : public ParamMapUserDlgProc {
	public:
		PScatterObject *po;

		ProScatDispProc(PScatterObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {delete this;}
	};

BOOL ProScatDispProc::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	switch (msg) {
		case WM_INITDIALOG: {
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ 	case IDC_AP_NEWSEED:
					{ srand( (unsigned)time( NULL ) );
					  int newseed=rand() % 25001;
					  po->pblock->SetValue(PB_RANDOMSEED,0,newseed);
					  po->pmapDisplay->Invalidate();
					}
			        return TRUE;
		default:
			return FALSE;
			}
			break;	
		}
	return TRUE;
}

class ProScatLockProc : public ParamMapUserDlgProc {
	public:
		PScatterObject *po;

		ProScatLockProc(PScatterObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void Update(TimeValue t);
		void DeleteThis() {delete this;}
	};

void  ProScatLockProc::Update(TimeValue t)
{ if (!po->editOb) return;
   LockOuts(po->thwnd,po->flags);
}

BOOL ProScatLockProc::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{	Point3 rot;
	    int maxv;
		switch (msg) {

		case WM_INITDIALOG: {
			po->thwnd=hWnd;
			Update(t);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
			case IDC_AP_ROTLOCK:
			{	int rlock;
			    po->pblock->GetValue(PB_ROTLOCK,t,rlock,FOREVER);
				if (rlock)
				{po->pblock->GetValue(PB_ROTX,t,rot.x,FOREVER);
			    po->pblock->GetValue(PB_ROTY,t,rot.y,FOREVER);
			    po->pblock->GetValue(PB_ROTZ,t,rot.z,FOREVER);
				maxv=MaxComponent(rot);
				if (maxv==0) 
				{ po->pblock->SetValue(PB_ROTY,t,rot.x);
				  po->pblock->SetValue(PB_ROTZ,t,rot.x);
				  SpinnerOff(hWnd,IDC_AP_ROTYSPIN);
				  SpinnerOff(hWnd,IDC_AP_ROTZSPIN);
				  po->flags &=~(PSCAT_ROTY|PSCAT_ROTZ);
				}
				else if (maxv==1) 
				{ po->pblock->SetValue(PB_ROTX,t,rot.y);
				  po->pblock->SetValue(PB_ROTZ,t,rot.y);
				  SpinnerOff(hWnd,IDC_AP_ROTXSPIN);
				  SpinnerOff(hWnd,IDC_AP_ROTZSPIN);
				  po->flags &=~(PSCAT_ROTX|PSCAT_ROTZ);
				}
				else  
				{ po->pblock->SetValue(PB_ROTX,t,rot.z);
				  po->pblock->SetValue(PB_ROTY,t,rot.z);
				  SpinnerOff(hWnd,IDC_AP_ROTXSPIN);
				  SpinnerOff(hWnd,IDC_AP_ROTYSPIN);
				  po->flags &=~(PSCAT_ROTX|PSCAT_ROTY);
				}
				} else
				{ SpinnerOn(hWnd,IDC_AP_ROTXSPIN);
				  SpinnerOn(hWnd,IDC_AP_ROTYSPIN);
				  SpinnerOn(hWnd,IDC_AP_ROTZSPIN);
				  po->flags |=(PSCAT_ROTX|PSCAT_ROTY|PSCAT_ROTZ);
				}
				po->pmapTransforms->Invalidate();
 				break;
			  }
			  case IDC_AP_TRANLOCK:
			  {	int rlock;
			    po->pblock->GetValue(PB_TRANLOCK,t,rlock,FOREVER);
				if (rlock)
				{po->pblock->GetValue(PB_TRANX,t,rot.x,FOREVER);
			    po->pblock->GetValue(PB_TRANY,t,rot.y,FOREVER);
			    po->pblock->GetValue(PB_TRANZ,t,rot.z,FOREVER);
				maxv=MaxComponent(rot);
				if (maxv==0) 
				{ po->pblock->SetValue(PB_TRANY,t,rot.x);
				  po->pblock->SetValue(PB_TRANZ,t,rot.x);
				  SpinnerOff(hWnd,IDC_AP_TRANYSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANZSPIN);
				  po->flags &=~(PSCAT_TRANY|PSCAT_TRANZ);
				}
				else if (maxv==1) 
				{ po->pblock->SetValue(PB_TRANX,t,rot.y);
				  po->pblock->SetValue(PB_TRANZ,t,rot.y);
				  SpinnerOff(hWnd,IDC_AP_TRANXSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANZSPIN);
				  po->flags &=~(PSCAT_TRANX|PSCAT_TRANZ);
				}
				else  
				{ po->pblock->SetValue(PB_TRANX,t,rot.z);
				  po->pblock->SetValue(PB_TRANY,t,rot.z);
				  SpinnerOff(hWnd,IDC_AP_TRANXSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANYSPIN);
				  po->flags &=~(PSCAT_TRANX|PSCAT_TRANY);
				}
				}  else
				{ SpinnerOn(hWnd,IDC_AP_TRANXSPIN);
				  SpinnerOn(hWnd,IDC_AP_TRANYSPIN);
				  SpinnerOn(hWnd,IDC_AP_TRANZSPIN);
				  po->flags |=(PSCAT_TRANX|PSCAT_TRANY|PSCAT_TRANZ);
				}
				  po->pmapTransforms->Invalidate();
				   break;
			  }
			  case IDC_AP_TRANFLOCK:
			  {	int rlock;
			    po->pblock->GetValue(PB_TRANFLOCK,t,rlock,FOREVER);
				if (rlock)
				{po->pblock->GetValue(PB_TRANFX,t,rot.x,FOREVER);
			    po->pblock->GetValue(PB_TRANFY,t,rot.y,FOREVER);
			    po->pblock->GetValue(PB_TRANFZ,t,rot.z,FOREVER);
				maxv=MaxComponent(rot);
				if (maxv==0) 
				{ po->pblock->SetValue(PB_TRANFY,t,rot.x);
				  po->pblock->SetValue(PB_TRANFZ,t,rot.x);
				  SpinnerOff(hWnd,IDC_AP_TRANYFSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANZFSPIN);
				  po->flags &=~(PSCAT_TRANFY|PSCAT_TRANFZ);
				}
				else if (maxv==1) 
				{ po->pblock->SetValue(PB_TRANFX,t,rot.y);
				  po->pblock->SetValue(PB_TRANFZ,t,rot.y);
				  SpinnerOff(hWnd,IDC_AP_TRANXFSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANZFSPIN);
				  po->flags &=~(PSCAT_TRANFX|PSCAT_TRANFZ);
				}
				else  
				{ po->pblock->SetValue(PB_TRANFX,t,rot.z);
				  po->pblock->SetValue(PB_TRANFY,t,rot.z);
				  SpinnerOff(hWnd,IDC_AP_TRANXFSPIN);
				  SpinnerOff(hWnd,IDC_AP_TRANYFSPIN);
				  po->flags &=~(PSCAT_TRANFX|PSCAT_TRANFY);
				}
				}  else
				{ SpinnerOn(hWnd,IDC_AP_TRANXFSPIN);
				  SpinnerOn(hWnd,IDC_AP_TRANYFSPIN);
				  SpinnerOn(hWnd,IDC_AP_TRANZFSPIN);
				  po->flags |=(PSCAT_TRANFX|PSCAT_TRANFY|PSCAT_TRANFZ);
				}
				  po->pmapTransforms->Invalidate();
				   break;
			  }
			  case IDC_AP_SCALELOCK:
			  {	int rlock;
			    po->pblock->GetValue(PB_SCALELOCK,t,rlock,FOREVER);
				if (rlock)
				{po->pblock->GetValue(PB_SCALEX,t,rot.x,FOREVER);
			    po->pblock->GetValue(PB_SCALEY,t,rot.y,FOREVER);
			    po->pblock->GetValue(PB_SCALEZ,t,rot.z,FOREVER);
				maxv=MaxComponent(rot);
				if (maxv==0) 
				{ po->pblock->SetValue(PB_SCALEY,t,rot.x);
				  po->pblock->SetValue(PB_SCALEZ,t,rot.x);
				  SpinnerOff(hWnd,IDC_AP_SCALEYSPIN);
				  SpinnerOff(hWnd,IDC_AP_SCALEZSPIN);
				  po->flags &=~(PSCAT_SCALEY|PSCAT_SCALEZ);
				}
				else if (maxv==1) 
				{ po->pblock->SetValue(PB_SCALEX,t,rot.y);
				  po->pblock->SetValue(PB_SCALEY,t,rot.y);
				  SpinnerOff(hWnd,IDC_AP_SCALEXSPIN);
				  SpinnerOff(hWnd,IDC_AP_SCALEZSPIN);
				  po->flags &=~(PSCAT_SCALEX|PSCAT_SCALEZ);
				}
				else  
				{ po->pblock->SetValue(PB_SCALEX,t,rot.z);
				  po->pblock->SetValue(PB_SCALEY,t,rot.z);
				  SpinnerOff(hWnd,IDC_AP_SCALEXSPIN);
				  SpinnerOff(hWnd,IDC_AP_SCALEYSPIN);
				  po->flags &=~(PSCAT_SCALEX|PSCAT_SCALEY);
				}
				}  else
				{ SpinnerOn(hWnd,IDC_AP_SCALEXSPIN);
				  SpinnerOn(hWnd,IDC_AP_SCALEYSPIN);
				  SpinnerOn(hWnd,IDC_AP_SCALEZSPIN);
				  po->flags |=(PSCAT_SCALEX|PSCAT_SCALEY|PSCAT_SCALEZ);
				}
				  po->pmapTransforms->Invalidate();
					   break;
			  }
			}
			break;

		default:
			return FALSE;
		}
	return TRUE;
}		

class ProScatObjList : public ParamMapUserDlgProc {
	public:
		PScatterObject *po;

		ProScatObjList(PScatterObject *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void Update(TimeValue t);
		void DeleteThis() {delete this;}
	};

void ProScatObjList::Update(TimeValue t)
{   if (!po->editOb) return;
	po->ShowOpList(po->ob2!=NULL);
	DistGreyOuts(po,po->distwnd,t);
}

BOOL ProScatObjList::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ switch (msg) {
		case WM_INITDIALOG: {
			po->distwnd=hWnd;
			Update(t);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_CUSTEDIT_ENTER: {
			ICustEdit *edit;
			TCHAR buf[256];
			
			switch (LOWORD(wParam)) {
				case IDC_PSCAT_ANAME:
					edit = GetICustEdit(GetDlgItem(hWnd,IDC_PSCAT_ANAME));					
					edit->GetText(buf,256);
					po->opaName = TSTR(buf);					
					if (po->ob1) po->ob1->NotifyDependents(FOREVER,PART_ALL,REFMSG_NODE_NAMECHANGE,TREE_VIEW_CLASS_ID);
					break;
					
				case IDC_PSCAT_BNAME:
					edit = GetICustEdit(GetDlgItem(hWnd,IDC_PSCAT_BNAME));					
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
			{ case IDC_BOOL_OPERANDS:
					if (HIWORD(wParam)==LBN_SELCHANGE) 
					{	po->flags &= ~PSCAT_ANYSEL;
						
						if (SendMessage((HWND)lParam,LB_GETSEL,0,0)) 
						{ po->flags |= PSCAT_OB1SEL;	}
							
						if (SendMessage((HWND)lParam,LB_GETSEL,1,0))
						{ po->flags |= PSCAT_OB2SEL;	}
						po->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
						po->NotifyDependents(FOREVER,0,REFMSG_BRANCHED_HISTORY_CHANGED);
						po->ip->RedrawViews(po->ip->GetTime());
						po->SetExtractButtonState();
					}
					break;
			  case IDC_AP_DISTOBJECT:
				DistObjectOn(po,hWnd);
				break;
			  case IDC_AP_VOLARRAY:
				DistObjectOff(po,hWnd);
				break;
			  case IDC_AP_DISTCENTERS:
			  case IDC_AP_DISTVERTICES:
			  case IDC_AP_DISTECENTER:
				  SpinnerOff(hWnd,IDC_AP_NUMBERSPIN);
				break;
			  case IDC_AP_DISTAREA:
			  case IDC_AP_DISTEVEN:
			  case IDC_AP_DISTNFACES:
			  case IDC_AP_DISTRANDOM:
			  case IDC_AP_DISTEDGES:
			  case IDC_AP_DISTVOLUME:
				  SpinnerOn(hWnd,IDC_AP_NUMBERSPIN);
				break;
				case IDC_PS_DISPOPS:
				case IDC_PS_DISPRESULT:
					if (IsDlgButtonChecked(hWnd,IDC_PS_DISPRESULT)) {						
						po->SetFlag(PSCAT_DISPRESULT);
					} else {
						po->ClearFlag(PSCAT_DISPRESULT);
						}
					po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					po->ip->RedrawViews(po->ip->GetTime());
					break;
				case IDC_SCATR_EXTRACT_INSTANCE:
					po->extractCopy = FALSE;
					break;
				case IDC_SCATR_EXTRACT_COPY:
					po->extractCopy = TRUE;
					break;
				case IDC_SCATR_EXTRACTOP:
					theHold.Begin();
					if (po->flags&PSCAT_OB1SEL) po->ExtractOperand(0);
					if (po->flags&PSCAT_OB2SEL) po->ExtractOperand(1);
					theHold.Accept(GetString(IDS_AP_EXTRACTOP));
					po->ip->RedrawViews(po->ip->GetTime());
					break;
			}
			break;	
		default:
			return FALSE;
		}
	return TRUE;
}
void PScatterObject::InvalidateUI()
	{
	if (pmapDist) pmapDist->Invalidate();
	if (pmapObjects) pmapObjects->Invalidate();
	if (pmapTransforms) pmapTransforms->Invalidate();
	if (pmapDisplay) pmapDisplay->Invalidate();
	}


void PScatterObject::BeginEditParams(
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
	if (pmapObjects && pmapTransforms && pmapDisplay && pmapTransforms)
	{	pmapDist->SetParamBlock(pblock);
		pmapDisplay->SetParamBlock(pblock);
		pmapObjects->SetParamBlock(pblock);
		pmapTransforms->SetParamBlock(pblock);
		SetWindowLong(hParams2,GWL_USERDATA,(LONG)this);
	}
	else 
	{ pmapDist = CreateCPParamMap(
				descPickDist,PICKDISTDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_PROSCATTER_PICKDIST),
				GetString(IDS_AP_PICKDISTOBJ),
				0);

	  pmapObjects = CreateCPParamMap(
				descObjects,OBJECTSDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_PROSCATTER_OBJECTS),
				GetString(IDS_AP_SCATTEROBJECTS),
				0);


		pmapTransforms= CreateCPParamMap(
				descTransforms,TRANSFORMSDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_PROSCATTER_XFORMS),
				GetString(IDS_AP_TRANSFORMS),
				(creating?APPENDROLL_CLOSED:0));			

		pmapDisplay = CreateCPParamMap(
				descDisplay,DISPLAYDESC_LENGTH,
				pblock,
				ip,
				hInstance,
				MAKEINTRESOURCE(IDD_PROSCATTER_DISPLAY),
				GetString(IDS_AP_SCATRDISPLAY),
				(creating?APPENDROLL_CLOSED:0));

		hParams2 = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PROSCATTER_SAVE),
				CustomSettingParamDlgProc, 
				GetString(IDS_AP_LOADSAVE), 
				(LPARAM)this,APPENDROLL_CLOSED);		
		ip->RegisterDlgWnd(hParams2);

	hObject=pmapObjects->GetHWnd();
   thwnd=pmapTransforms->GetHWnd();
if (pmapObjects) pmapObjects->SetUserDlgProc(new ProScatObjList(this));
  if (pmapDist) pmapDist->SetUserDlgProc(new ProScatPickObj(this));
  if (pmapTransforms) pmapTransforms->SetUserDlgProc(new ProScatLockProc(this));
  if (pmapDisplay) pmapDisplay->SetUserDlgProc(new ProScatDispProc(this));
	} 
 }

void PScatterObject::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
	{	
	editOb = NULL;

	if (flags&END_EDIT_REMOVEUI ) {
		if (pmapObjects) DestroyCPParamMap(pmapObjects);
		if (pmapTransforms) DestroyCPParamMap(pmapTransforms);
		if (pmapDisplay) DestroyCPParamMap(pmapDisplay);
		if (pmapDist) DestroyCPParamMap(pmapDist);
		ip->UnRegisterDlgWnd(hParams2);
		ip->DeleteRollupPage(hParams2);
		hParams2 = NULL;
		pmapObjects  = NULL;
		pmapTransforms = NULL;
		pmapDisplay = NULL;
		pmapDist = NULL;
		}else
		SetWindowLong(hParams2,GWL_USERDATA,(LONG)NULL);

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

int PScatterObject::NumPipeBranches() 
	{
	int num=0;
	if (TestFlag(PSCAT_OB1SEL) && ob1) num++;
	if (TestFlag(PSCAT_OB2SEL) && ob2) num++;
	return num;
	}

Object *PScatterObject::GetPipeBranch(int i) 
	{
	if (i) return ob2;	
	if (TestFlag(PSCAT_OB1SEL)) return ob1;
	return ob2;
	}

INode *PScatterObject::GetBranchINode(TimeValue t,INode *node,int i)
	{
	assert(i<2);
	int index = 0;
	if (i) index = 1;
	else if (TestFlag(PSCAT_OB1SEL)) index = 0;
	else index = 1;
	return CreateINodeTransformed(node,GetOpTM(t,index));
	}

int PScatterObject::NumSubs()
{ 	return 5; 	}

Animatable* PScatterObject::SubAnim(int i)
{ 	switch (i) {
		case 0:  return ob1;
		case 1:  return tm1;
		case 2:	 return ob2;
		case 3:	 return tm2;
		case 4:	 return pblock;
		default: return NULL;
		}	
}

TSTR PScatterObject::SubAnimName(int i)
	{	
	switch (i) {
		case 0: return opaName;//return GetString(IDS_AP_OPERANDA);
		case 1: return GetString(IDS_AP_OPERANDATRANSFORM);
		case 2: return opbName;//return GetString(IDS_AP_OPERANDB);
		case 3: return GetString(IDS_AP_OPERANDBTRANSFORM);
		case 4: return GetString(IDS_AP_PARAMETERS);
		}
	return _T("Error");
	}

int PScatterObject::SubNumToRefNum(int subNum)
	{
	switch (subNum) {
		case 0:  return REF_OP1;
		case 1:  return REF_CONT1;
		case 2:	 return REF_OP2;
		case 3:	 return REF_CONT2;
		case 4:	 return PBLOCK;
		default: return -1;
		}	
	}

RefTargetHandle PScatterObject::GetReference(int i)
	{
	switch (i) {
		case REF_OP1: 	return ob1;
		case REF_OP2: 	return ob2;
		case REF_CONT1:	return tm1;
		case REF_CONT2:	return tm2;
		case PBLOCK:	return pblock;
		default:        return NULL;
		}
	}
void PScatterObject::ShowName(BOOL ob2)
{ if (pmapDist)
{ TSTR name=TSTR(GetString(IDS_AP_OBJECTSTR)) + (ob2 ? opbName : TSTR(GetString(IDS_AP_NONE)));
SetWindowText(GetDlgItem(pmapDist->GetHWnd(), IDC_AP_PROSCATTER_DISTOBJ), name);}
}
void PScatterObject::ShowOpList(BOOL isobj)
{ if (pmapObjects)
{	ShowName(isobj); 
	HWND hList = GetDlgItem(pmapObjects->GetHWnd(),IDC_BOOL_OPERANDS);
	SendMessage(hList,LB_RESETCONTENT,0,0);
	TSTR name = TSTR(GetString(IDS_AP_SCATTERTITLE))+ (ob1 ? opaName : TSTR(_T("")));
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name);
	name = TSTR(GetString(IDS_AP_DISTRIBTITLE))+ (isobj ? opbName : TSTR(_T("")));
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name);
	if (flags&PSCAT_OB1SEL) {SendMessage(hList,LB_SETSEL,TRUE,0); }
	if (flags&PSCAT_OB2SEL) {SendMessage(hList,LB_SETSEL,TRUE,1); }
	ICustEdit *edit = GetICustEdit(GetDlgItem(pmapObjects->GetHWnd(),IDC_PSCAT_ANAME));
	edit->SetText(opaName);
	ReleaseICustEdit(edit);

	edit = GetICustEdit(GetDlgItem(pmapObjects->GetHWnd(),IDC_PSCAT_BNAME));
	edit->SetText(opbName);
	ReleaseICustEdit(edit);
	SetExtractButtonState();
}
}

void PScatterObject::SetReference(int i, RefTargetHandle rtarg)
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
					ShowOpList();
					}
				}*/
			break;

		case REF_CONT1:	 tm1 = (Control*)rtarg; break;
		case REF_CONT2:	 tm2 = (Control*)rtarg; break;
		case PBLOCK:	 pblock=(IParamBlock*)rtarg; break;
		}
	}

RefTargetHandle PScatterObject::Clone(RemapDir& remap)
	{
	PScatterObject *obj = new PScatterObject;
	if (ob1) obj->ReplaceReference(REF_OP1,remap.CloneRef(ob1));
	if (ob2) obj->ReplaceReference(REF_OP2,remap.CloneRef(ob2));
	if (tm1) obj->ReplaceReference(REF_CONT1,remap.CloneRef(tm1));
	if (tm2) obj->ReplaceReference(REF_CONT2,remap.CloneRef(tm2));
	if (pblock) obj->ReplaceReference(PBLOCK,pblock->Clone(remap)); 
	obj->flags = flags;
	obj->opaName=opaName;
	obj->opbName=opbName;
	obj->ivalid.SetEmpty();
	return obj;
	}

int PScatterObject::IntersectRay(
		TimeValue t, Ray& r, float& at, Point3& norm)
	{
	if (TestFlag(PSCAT_DISPRESULT)) {
		UpdateMesh(t);
		return mesh.IntersectRay(r,at,norm);
	} else {
		return 0;
		}
	}

Mesh* PScatterObject::GetRenderMesh(
		TimeValue t, INode *inode, View& view, BOOL& needDelete)
	{	
	UpdateMesh(t);
	needDelete = FALSE;
	return &mesh;	
	}

int PScatterObject::HitTest(
		TimeValue t, INode* inode, int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt)
	{
	int res = 0;
	if (ob1&& TestFlag(PSCAT_DISPRESULT)) {
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

void PScatterObject::Snap(
		TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
	{
	if (TestFlag(PSCAT_DISPRESULT)&&ob1) {
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

int PScatterObject::
		Display(TimeValue t, INode* inode, ViewExp *vpt, int aflags)
	{	
	GraphicsWindow *gw = vpt->getGW();

/*	UpdateMesh(t,FALSE,inode->Selected());
	Matrix3 mat;
	mat=inode->GetObjectTM(t);
	gw->setTransform(mat);
	mesh.render(gw, inode->Mtls(),(aflags&USE_DAMAGE_RECT)?&vpt->GetDammageRect():NULL, COMP_ALL, inode->NumMtls());*/

	Object *ob;
		
	DWORD rlim = gw->getRndLimits();

	if (ob=GetPipeObj(t,0)) {
	  if (TestFlag(PSCAT_DISPRESULT))
	  { UpdateMesh(t,FALSE,inode->Selected());
		Matrix3 mat;
		mat=inode->GetObjectTM(t);
		gw->setTransform(mat);
		 if (inode->Selected()) {
				 if (TestFlag(PSCAT_OB1SEL)) {
					vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
				 } else {
					Point3 selClr = GetUIColor(COLOR_SELECTION); 
					vpt->getGW()->setColor( LINE_COLOR, selClr.x, selClr.y, selClr.z);
				 	}
				}
		TriObject t1;
		t1.GetMesh().DeepCopy(&mesh,PART_GEOM|PART_TOPO);
		t1.GetMesh().setNumVerts(ob1verts,TRUE);t1.GetMesh().setNumFaces(ob1faces,TRUE);
		t1.GetMesh().render(gw, inode->Mtls(),(aflags&USE_DAMAGE_RECT)?&vpt->GetDammageRect():NULL, COMP_ALL, inode->NumMtls());				 
	  }
	  else
		{	INodeTransformed n(inode,GetOpTM(t,0));
			if (inode->Selected()) 
			{ if (TestFlag(PSCAT_OB1SEL)) 
				vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
			  else vpt->getGW()->setColor( LINE_COLOR, GetSelColor());
			}
			ob->Display(t,&n,vpt,aflags);
		}
	}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)))
		{	INodeTransformed n(inode,GetOpTM(t,1));
			if (inode->Selected()) 
			{ if (TestFlag(PSCAT_OB2SEL)) 
				vpt->getGW()->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
			  else vpt->getGW()->setColor( LINE_COLOR, GetSelColor());
			}
			ob->Display(t,&n,vpt,flags);
		}
		
	gw->setRndLimits(rlim);
	return 0;
	}

void PScatterObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel)
{
	Box3 abox;
	abox.Init();
	box.Init();
	if (TestFlag(PSCAT_DISPRESULT) && ob1) {
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

void PScatterObject::GetLocalBoundBox(TimeValue t, INode *inode,ViewExp* vpt, Box3& box) 
	{
	Box3 abox;
	abox.Init();
	box.Init();
	if (TestFlag(PSCAT_DISPRESULT) && ob1) {
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

void PScatterObject::GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box)
{	Box3 abox;
	int disp = 0;
	abox.Init();
	box.Init();

	if (TestFlag(PSCAT_DISPRESULT) && ob1) 
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

RefResult PScatterObject::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message ) 
	{
	switch (message) {
		case REFMSG_SELECT_BRANCH:
			if (hTarget==ob1 || hTarget==ob2) {
				ClearFlag(PSCAT_OB1SEL|PSCAT_OB2SEL);
				if (hTarget==ob1) SetFlag(PSCAT_OB1SEL);
				if (hTarget==ob2) SetFlag(PSCAT_OB2SEL);
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


int PScatterObject::HitTest(
		TimeValue t, INode* inode, int type, int crossing, 
		int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{	
	int res = 0;
	Object *ob;
	if ((ob=GetPipeObj(t,0)) &&
		!(flags&HIT_SELONLY && !TestFlag(PSCAT_OB1SEL)) &&
		!(flags&HIT_UNSELONLY && TestFlag(PSCAT_OB1SEL)) )
		if (TestFlag(PSCAT_DISPRESULT))
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
		{INodeTransformed n(inode,GetOpTM(t,0));
		
		if (ob->HitTest(t,&n,type,crossing,flags,p,vpt)) {
			vpt->LogHit(inode,mc,0,0,NULL);
			res = TRUE;
			if (flags & HIT_ABORTONHIT) return TRUE;
			}		
		}
		int hdist;
		pblock->GetValue(PB_HIDEDISTOBJECT,t,hdist,ivalid);
		if ((!hdist)&&(ob=GetPipeObj(t,1)) &&
		!(flags&HIT_SELONLY && !TestFlag(PSCAT_OB2SEL)) &&
		!(flags&HIT_UNSELONLY && TestFlag(PSCAT_OB2SEL)) ) {
		
		INodeTransformed n(inode,GetOpTM(t,1));
		
		if (ob->HitTest(t,&n,type,crossing,flags,p,vpt)) {
			vpt->LogHit(inode,mc,0,1,NULL);
			res = TRUE;			
			}		
		}
	
	return res;
	}

int PScatterObject::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flags, ModContext* mc)
	{
	return 0;
	}

void PScatterObject::SelectSubComponent(
		HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
	{
	while (hitRec) {
		if (hitRec->hitInfo) {
			if (selected) SetFlag(PSCAT_OB2SEL);
			else ClearFlag(PSCAT_OB2SEL);
		} else {
			if (selected) SetFlag(PSCAT_OB1SEL);
			else ClearFlag(PSCAT_OB1SEL);
			}
		if (all) hitRec = hitRec->Next();
		else break;
		}
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_BRANCHED_HISTORY_CHANGED);
	if (ip) ShowOpList(ob2!=NULL);
	}

void PScatterObject::ClearSelection(int selLevel)
	{
	ClearFlag(PSCAT_OB1SEL|PSCAT_OB2SEL);
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	if (ip) ShowOpList(ob2!=NULL);
	}

int PScatterObject::SubObjectIndex(HitRecord *hitRec)
	{
	return hitRec->hitInfo;
	}

void PScatterObject::ActivateSubobjSel(int level, XFormModes& modes)
	{
	if (level) {
		modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
		NotifyDependents(
			FOREVER, 
			PART_SUBSEL_TYPE|PART_DISPLAY, 
			REFMSG_CHANGE);		
		ip->PipeSelLevelChanged();
		HWND hList = GetDlgItem(pmapObjects->GetHWnd(),IDC_SWRAP_OPERANDS);
		if (SendMessage(hList,LB_GETSEL,0,0)) flags |= PSCAT_OB1SEL;							
		if (SendMessage(hList,LB_GETSEL,1,0)) flags |= PSCAT_OB2SEL;	
		}
}
class ProScatPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		ProScatPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload) {
			DWORD oldVer = ((PScatterObject*)(cb->targ))->pblock->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<2) {	
 				((PScatterObject*)targ)->pblock->SetValue(PB_ANIOFF,0,TimeValue(0));
 				((PScatterObject*)targ)->pblock->SetValue(PB_EXTRACTTYPE,0,0);
				}
			if (oldVer<1) {	
 				((PScatterObject*)targ)->pblock->SetValue(PB_SHOWTYPE,0,0);
				((PScatterObject*)targ)->SetFlag(PSCAT_DISPRESULT);
				}
			delete this;
			}
	};


#define BOOL_FLAGS_CHUNK	0x0100
#define BOOL_OPANAME_CHUNK	0x0110
#define BOOL_OPBNAME_CHUNK	0x0120
#define BOOL_FIRSTMTLS_CHUNK 0x0130
#define BOOL_SECONDMTLS_CHUNK 0x0140

IOResult PScatterObject::Save(ISave *isave)
	{
	ULONG nb;

	isave->BeginChunk(BOOL_FLAGS_CHUNK);		
	isave->Write(&flags,sizeof(flags),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(BOOL_OPANAME_CHUNK);		
	isave->WriteWString(opaName);
	isave->EndChunk();
	
	isave->BeginChunk(BOOL_OPBNAME_CHUNK);		
	isave->WriteWString(opbName);
	isave->EndChunk();
	
	isave->BeginChunk(BOOL_FIRSTMTLS_CHUNK);		
	isave->Write(&firstmtls,sizeof(firstmtls),&nb);
	isave->EndChunk();

	isave->BeginChunk(BOOL_SECONDMTLS_CHUNK);		
	isave->Write(&firstmtls,sizeof(secondmtls),&nb);
	isave->EndChunk();

	return IO_OK;
	}


IOResult PScatterObject::Load(ILoad *iload)
	{
		iload->RegisterPostLoadCallback(
			new ProScatPostLoadCallback(
				new ParamBlockPLCB(psversions,NUM_OLDVERSIONS,&curVersion,this,PBLOCK)));
	ULONG nb;
	IOResult res = IO_OK;
	
	// Default names
	opaName = TSTR(GetString(IDS_AP_OPERAND));
	opbName = TSTR(GetString(IDS_AP_OPERAND));

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case BOOL_FLAGS_CHUNK:
				res=iload->Read(&flags,sizeof(flags),&nb);
				break;
			
			case BOOL_OPANAME_CHUNK: {
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				opaName = TSTR(buf);
				break;
				}

			case BOOL_OPBNAME_CHUNK: {
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				opbName = TSTR(buf);
				break;
				}
			case BOOL_FIRSTMTLS_CHUNK:
				res=iload->Read(&firstmtls,sizeof(firstmtls),&nb);
				break;
			case BOOL_SECONDMTLS_CHUNK:
				res=iload->Read(&secondmtls,sizeof(secondmtls),&nb);
				break;
			}
		
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}

	Invalidate();
	return IO_OK;
	}

ParamDimension *PScatterObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_BASESCALE: return stdPercentDim;
		case PB_VERTEXCHAOS: return stdWorldDim;
		case PB_ROTX: return stdWorldDim;
		case PB_ROTY: return stdWorldDim;
		case PB_ROTZ: return stdWorldDim;
		case PB_TRANX: return stdWorldDim;
		case PB_TRANY: return stdWorldDim;
		case PB_TRANZ: return stdWorldDim;
		case PB_TRANFX: return stdWorldDim;
		case PB_TRANFY: return stdWorldDim;
		case PB_TRANFZ: return stdWorldDim;
		case PB_SCALEX: return stdWorldDim;
		case PB_SCALEY: return stdWorldDim;
		case PB_SCALEZ: return stdWorldDim;
		case PB_DISPLAYPORTION: return stdPercentDim;
		case PB_ANIOFF:			return stdTimeDim;
		default: return defaultDim;
		}
	}

TSTR PScatterObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_NUMBER: return GetString(IDS_AP_NUMBER);
		case PB_BASESCALE: return GetString(IDS_AP_BASESCALE);
		case PB_VERTEXCHAOS: return GetString(IDS_AP_VERTICESDANCE);
		case PB_ROTX: return GetString(IDS_AP_ROTX);
		case PB_ROTY: return GetString(IDS_AP_ROTY);
		case PB_ROTZ: return GetString(IDS_AP_ROTZ);
		case PB_TRANX: return GetString(IDS_AP_TRANX);
		case PB_TRANY: return GetString(IDS_AP_TRANY);
		case PB_TRANZ: return GetString(IDS_AP_TRANZ);
		case PB_TRANFX: return GetString(IDS_AP_TRANFX);
		case PB_TRANFY: return GetString(IDS_AP_TRANFY);
		case PB_TRANFZ: return GetString(IDS_AP_TRANFZ);
		case PB_SCALEX: return GetString(IDS_AP_SCALEX);
		case PB_SCALEY: return GetString(IDS_AP_SCALEY);
		case PB_SCALEZ: return GetString(IDS_AP_SCALEZ);
		case PB_EXTRACTTYPE: return GetString(IDS_AP_EXTRACTTYPE);
		case PB_ANIOFF: return GetString(IDS_AP_ANIOFF);
		default: return TSTR(_T(""));
		}
	}


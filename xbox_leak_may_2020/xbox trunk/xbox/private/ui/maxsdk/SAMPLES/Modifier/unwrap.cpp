/********************************************************************** *<
	FILE: unwrap.cpp

	DESCRIPTION: A UVW map modifier unwraps the UVWs onto the image

	HISTORY: 12/31/96
	CREATED BY: Rolf Berteig
	UPDATED Sept. 16, 1998 Peter Watje




 *>	Copyright (c) 1998, All Rights Reserved.
**********************************************************************/

#include "mods.h"
#include "iparamm.h"
#include "meshadj.h"
#include "sctex.h"
#include "decomp.h"

#include "gport.h"
#include "bmmlib.h"

#include "stdmat.h"


#define UNWRAP_NAME		GetString(IDS_RB_UNWRAPMOD)
#define UNWRAP_CLASSID	Class_ID(0x02df2e3a,0x72ba4e1f)

// Flags
#define CONTROL_FIT			(1<<0)
#define CONTROL_CENTER		(1<<1)
#define CONTROL_ASPECT		(1<<2)
#define CONTROL_UNIFORM		(1<<3)
#define CONTROL_HOLD		(1<<4)
#define CONTROL_INIT		(1<<5)
#define CONTROL_OP			(CONTROL_FIT|CONTROL_CENTER|CONTROL_ASPECT|CONTROL_UNIFORM)
#define CONTROL_INITPARAMS	(1<<10)

#define IS_MESH		1
#define IS_PATCH	2
#define IS_NURBS	3


class UnwrapMod;

static HCURSOR selCur   = NULL;
static HCURSOR moveCur  = NULL;
static HCURSOR moveXCur  = NULL;
static HCURSOR moveYCur  = NULL;
static HCURSOR rotCur   = NULL;
static HCURSOR scaleCur = NULL;
static HCURSOR scaleXCur = NULL;
static HCURSOR scaleYCur = NULL;

static HCURSOR zoomCur = NULL;
static HCURSOR zoomRegionCur = NULL;
static HCURSOR panCur = NULL;
static HCURSOR weldCur = NULL;

#define CBS		(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL)


// Rightclick menu UI stuff


void UnwrapMatrixFromNormal(Point3& normal, Matrix3& mat);


class SelectMode : public MouseCallBack {
	public:
		UnwrapMod *mod;
		BOOL region, toggle, subtract;
		IPoint2 om, lm;
		SelectMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		virtual int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)=0;
		virtual HCURSOR GetXFormCur()=0;
	};




class MoveMode : public SelectMode {
	public:				
		UnwrapMod *mod;
		MoveMode(UnwrapMod *m) : SelectMode(m) {mod = m;}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};
class RotateMode : public SelectMode {
	public:				
		RotateMode(UnwrapMod *m) : SelectMode(m) {}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {return rotCur;}
	};
class ScaleMode : public SelectMode {
	public:				
		ScaleMode(UnwrapMod *m) : SelectMode(m) {}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();// {return scaleCur;}
	};
class WeldMode : public SelectMode {
	public:				
		WeldMode(UnwrapMod *m) : SelectMode(m) {}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {return weldCur;}
	};

class PanMode : public MouseCallBack {
	public:
		UnwrapMod *mod;
		IPoint2 om;
		float oxscroll, oyscroll;
		PanMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {return panCur;}

	};
class ZoomMode : public MouseCallBack {
	public:
		UnwrapMod *mod;
		IPoint2 om;
		float ozoom;
		float oxscroll, oyscroll;
		ZoomMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {
			return zoomCur;
			}
	};
class ZoomRegMode : public MouseCallBack {
	public:
		UnwrapMod *mod;
		IPoint2 om, lm;		
		ZoomRegMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {return zoomCur;}
	};



class RightMouseMode : public MouseCallBack {
	public:
		UnwrapMod *mod;		
		RightMouseMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};


class MiddleMouseMode : public MouseCallBack {
	public:
		UnwrapMod *mod;		
		IPoint2 om;
		float ozoom;
		float oxscroll, oyscroll;
		MiddleMouseMode(UnwrapMod *m) {mod=m;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};


#define CID_FACEALIGNMAP 	0x4f298c7c
#define CID_REGIONFIT 		0x4f298c7d





//nee a table of TV faces

class UVW_TVFaceClass
{
public:

int t[4];
int FaceIndex;
int MatID;
int flags;
Point3 pt[4];
};

#define FLAG_DEAD		1
#define FLAG_HIDDEN		2
#define FLAG_FROZEN		4
#define FLAG_QUAD		8
#define FLAG_SELECTED	16

//need a table of TVert pointers
class UVW_TVVertClass
{
public:
Point3 p;
float influence;
BYTE flags;
};

class UVW_ChannelClass
{
public:
int channel;
Tab<UVW_TVVertClass> v;
Tab<UVW_TVFaceClass> f;
Tab<Control*> cont;		
};


class VertexLookUpDataClass
{
public:
int index;
int newindex;
Point3 p;

};
class VertexLookUpListClass
{
public:
BitArray sel;
Tab<VertexLookUpDataClass> d;
void addPoint(int a_index, Point3 a);
};

void VertexLookUpListClass::addPoint(int a_index, Point3 a)
	{	
	BOOL found = FALSE;
	if (sel[a_index]) found = TRUE;
/*	
	for (int i = 0; i < d.Count(); i++)
		{
		if (d[i].index == a_index)
			{
			found = TRUE;
			i = d.Count();
			}
		}
*/
	if (!found)
		{
		VertexLookUpDataClass t;
		t.index = a_index;
		t.newindex = a_index;
		t.p = a;
//		d.Append(1,&t,1);
		d[a_index] = t;
		sel.Set(a_index);
		}
	};



class UnwrapRegionFitMode;
class UnwrapFaceAlignMode;

class UnwrapMod : public Modifier, TimeChangeCallback {	
	public:	
//		Tab<Point3> tvert;
//		Tab<TVFace> tvFace;
//		Tab<Control*> cont;		
		int version;
		BOOL oldDataPresent;
		BOOL firstPass;
		BitArray vsel;
//		BitArray meshsel;
		Tab<int> facehit;
		Control *tmControl;
		Control *offsetControl;
		Control *scaleControl;
		Control *rotateControl;


		Point3 gNormal,gCenter;
		float gXScale,gYScale;


		DWORD flags;
		BOOL instanced;

		Point3 s;

		float zoom, aspect, xscroll, yscroll;
		Mtl *BaseMtl; 
		static int CurrentMap;
		Texmap *map[10];
		void AddMaterial(MtlBase *mtl);
		void LoadMaterials();
		void UpdateListBox();

		UBYTE *image;
		int iw, ih, uvw, move,scale;
		int rendW, rendH;
		int channel;
		int pixelSnap;
		int isBitmap;
		int bitmapWidth, bitmapHeight;
		BOOL useBitmapRes;
		int type;
		int objType;

		int zoomext;
		int lockSelected;
		int mirror;
		int hide;
		int freeze;
		int incSelected;
		int falloff;
		int falloffSpace;
		BOOL showMap;

		BOOL updateCache;

		static COLORREF lineColor, selColor;
		static float weldThreshold;
		static BOOL update;
		static int showVerts;
		static int midPixelSnap;

	
//		int CurrentTVMap;
//		Tab<UVW_ChannelClass*> TVMaps;
		UVW_ChannelClass TVMaps;

//planar gizmo stuff
		Point3 gOffset;
		Point3 gScale;
		float gRotate;
		Matrix3 PlanarTM;
		int GizmoSelected;
		Tab<UVW_TVFaceClass> gfaces;
		VertexLookUpListClass gverts;
		void ApplyGizmo();
		void ComputeSelectedFaceData();
	
//filter stuff
		int filterSelectedFaces;
		int matid;
		int alignDir;
		void BuildMatIDList();
		int IsFaceVisible(int i);
		int IsVertVisible(int i);
		Tab<int> filterMatID;
		BitArray vertMatIDList;
		void SetMatFilters();
	

		static HWND hParams, hWnd, hView;
		static IObjParam  *ip;
		static UnwrapMod *editMod;
		static ICustToolbar *iTool;
		static ICustToolbar *iVertex;
		static ICustToolbar *iView;
		static ICustToolbar *iOption;
		static ICustToolbar *iFilter;
        static ICustButton *iApplyButton;

		HWND hTextures;
		HWND hMatIDs;

		static ICustButton *iMove, *iRot, *iScale,*iMirror, *iPan, *iZoom,*iFalloff ;
		static ICustButton *iZoomReg, *iZoomExt, *iUVW, *iProp, *iShowMap, *iUpdate;
		static ICustButton *iSnap,*iWeld,*iLockSelected,*iFilterSelected;
		static ICustButton *iHide,*iFreeze;
		static ICustButton *iIncSelected;
		static ICustButton *iFalloffSpace;

		static ICustButton *iWeldSelected, *iBreak;


		static ISpinnerControl *iU, *iV, *iW;
		static ISpinnerControl *iStr;
		static ISpinnerControl *iMapID;

		static MouseManager mouseMan;
		static IOffScreenBuf *iBuf;
		static int mode;
		static int oldMode;
		static MoveMode *moveMode;
		static RotateMode *rotMode;
		static ScaleMode *scaleMode;
		static PanMode *panMode;
		static ZoomMode *zoomMode;
		static ZoomRegMode *zoomRegMode;
		static WeldMode *weldMode;



		static RightMouseMode *rightMode;
		static MiddleMouseMode *middleMode;


		static SelectModBoxCMode *selectMode;

/*		static MoveModBoxCMode *gmoveMode;
		static RotateModBoxCMode *grotMode;
		static UScaleModBoxCMode *guscaleMode;
		static NUScaleModBoxCMode *gnuscaleMode;
		static SquashModBoxCMode *gsquashMode;		

		static UnwrapFaceAlignMode *faceAlignMode;
		static UnwrapRegionFitMode *regionFitMode;
*/
		// From MappingMod
		void EnterNormalAlign();
		void ExitNormalAlign();
		void EnterRegionFit();
		void ExitRegionFit();
		int axis;
		int GetAxis();


		static BOOL viewValid, typeInsValid;

		Point2 center;
		int centeron;
		Matrix3 InverseTM;
		Point3 cdebug;


		UnwrapMod();
		~UnwrapMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s=UNWRAP_NAME;}
		virtual Class_ID ClassID() {return UNWRAP_CLASSID;}
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() { return UNWRAP_NAME; }
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
		BOOL AssignController(Animatable *control,int subAnim);

		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE|PART_VERTCOLOR;}
		ChannelMask ChannelsChanged() {return TEXMAP_CHANNEL|PART_VERTCOLOR; }		
//		Class_ID InputType() {return triObjectClassID;}
		Class_ID InputType() {return mapObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);		

		int NumRefs() {
						int ct = 0;
						ct += TVMaps.cont.Count();
						return ct+11;
						}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		int RemapRefOnLoad(int iref) ;

		// From BaseObject
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		Box3 BuildBoundVolume(Object *obj);


		void InitControl(TimeValue t);
/*		void Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
		void Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE);
		void Scale(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
*/
		Matrix3 CompMatrix(TimeValue t,ModContext *mc, Matrix3 *ntm,BOOL applySize=TRUE, BOOL applyAxis=TRUE);
		void DoIcon(PolyLineProc& lp,BOOL sel);
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);
		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		void ActivateSubobjSel(int level, XFormModes& modes);
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
//		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
		void ClearSelection(int selLevel);
//		void SelectAll(int selLevel);
//		void InvertSelection(int selLevel);


		int NumSubs() {
						int ct = 0;
						ct += TVMaps.cont.Count();

						return ct;
//						return cont.Count();
						}

		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		// From TimeChangeCallback
		void TimeChanged(TimeValue t) {InvalidateView();}

		// Local methods
		void EditMapping(HWND hWnd);
//		void SynchWithMesh(Mesh &mesh);

		// Floater win methods
		void SetupDlg(HWND hWnd);
		void SizeDlg();
		void DestroyDlg();
		void PaintView();		
		void RegisterClasses();
		Point2 UVWToScreen(Point3 pt, float xzoom, float yzoom,int w,int h);
		void ComputeZooms(HWND hWnd, float &xzoom, float &yzoom,int &width,int &height);
		void SetMode(int m);
		void InvalidateView();
		BOOL HitTest(Rect rect,Tab<int> &hits,BOOL selOnly);
		void Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all);
		void ClearSelect();
		void HoldPoints();
		void HoldPointsAndFaces();
		void HoldSelection();
		void MovePoints(Point2 pt);
		void MoveGizmo(Point2 pt);
		void RotatePoints(HWND h, float ang);
		void RotateGizmo(HWND h, float ang);
		void ScalePoints(HWND h, float scale, int direction);
		void ScaleGizmo(HWND h, float scale, int direction);
		void MirrorPoints(HWND h, int direction);
		void MirrorGizmo(HWND h, int direction);
		void AlignMap();
		void PickMap();
		void SetupImage();
		void GetUVWIndices(int &i1, int &i2);
		void PropDialog();
		void PlugControllers();
		Point3 GetPoint(TimeValue t,int i);
//		Point3 GetObjectPoint(TimeValue t,int i);

		Tab<Point3> objectPointList;
		Point3 GetObjectPoint(TimeValue t,int i);
		void BuildObjectPoints();

		void ZoomExtents();
		void Reset();
		void SetupChannelButtons();
		void SetupTypeins();
		void InvalidateTypeins();
		void TypeInChanged(int which);

		void SnapPoint( Point3 &p);
		void BreakSelected();
		void WeldSelected();
		BOOL WeldPoints(HWND h, IPoint2 m);

		void HideSelected();
		void UnHideAll();

		void FreezeSelected();
		void UnFreezeAll();

		void ZoomSelected();

		void DeleteSelected();

		Tab<int> ActiveAddFaces;
		void AddFaces(BitArray sel, Mesh *mesh);
		void ExpandFaces(BitArray sel, Mesh *mesh);
		void AddFaces(BitArray sel, PatchMesh *patch);
		void ExpandFaces(BitArray sel, PatchMesh *patch);
		void DeleteVertsFromFace(Tab<UVW_TVFaceClass> f);
		void DeleteVertsFromFace(BitArray f);

		void UpdateFaceSelection(BitArray f);
		int IsSelected(int index);

		void ExpandSelection(int dir);

		void RebuildDistCache();
		void ComputeFalloff(float &u, int ftype);

		void LoadUVW(HWND hWnd);
		void SaveUVW(HWND hWnd);
		void TrackRBMenu(HWND hwnd, int x, int y);


		Tab<TSTR*> namedSel;		
		Tab<DWORD> ids;
		BOOL SupportsNamedSubSels() {return TRUE;}
		void ActivateSubSelSet(TSTR &setName);
		void NewSetFromCurSel(TSTR &setName);
		void RemoveSubSelSet(TSTR &setName);
		void SetupNamedSelDropDown();
		int NumNamedSelSets();
		TSTR GetNamedSelSetName(int i);
		void SetNamedSelSetName(int i,TSTR &newName);
		void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);

	// Local methods for handling named selection sets
		int FindSet(TSTR &setName);		
		DWORD AddSet(TSTR &setName);
		void RemoveSet(TSTR &setName);
		void ClearSetNames();

		void LocalDataChanged();
		IOResult LoadNamedSelChunk(ILoad *iload);
		IOResult SaveLocalData(ISave *isave, LocalModData *ld);
		IOResult LoadLocalData(ILoad *iload, LocalModData **pld);
		void SetNumSelLabel();		
		void BuildInitialMapping(Mesh *msh);
		void BuildInitialMapping(PatchMesh *msh);

		void RemoveDeadVerts(PatchMesh *mesh,int CurrentChannel);
		int IsSelectedSetup();
		BitArray isSelected;

//watje 10-19-99 213458
		BOOL DependOnTopology(ModContext &mc) {return TRUE;}
};

class UnwrapFaceAlignMouseProc : public MouseCallBack {
	public:
		UnwrapMod *mod;
		IObjParam *ip;
		UnwrapFaceAlignMouseProc(UnwrapMod *m,IObjParam *i) {mod=m;ip=i;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		void FaceAlignMap(HWND hWnd,IPoint2 m);
	};
class UnwrapFaceAlignMode : public CommandMode {
	public:
		ChangeFGObject fgProc;
		UnwrapFaceAlignMouseProc proc;
		IObjParam *ip;
		UnwrapMod *mod;

		UnwrapFaceAlignMode(UnwrapMod *m,IObjParam *i) 
			: fgProc(m), proc(m,i) {ip=i;mod=m;}

		int Class() {return MOVE_COMMAND;}		
		int ID() {return CID_FACEALIGNMAP;}
		MouseCallBack *MouseProc(int *numPoints) {*numPoints=2;return &proc;}
		ChangeForegroundCallback *ChangeFGProc() {return &fgProc;}
		BOOL ChangeFG(CommandMode *oldMode) {return oldMode->ChangeFGProc() != &fgProc;}
		void EnterMode();
		void ExitMode();
	};




HCURSOR MoveMode::GetXFormCur()
	{		
	if (mod->move==1)
		return moveXCur;
	else if (mod->move==2) return moveYCur;
	return moveCur;
	}

HCURSOR ScaleMode::GetXFormCur()
	{		
	if (mod->scale==1)
		return scaleXCur;
	else if (mod->scale==2) return scaleYCur;
	return scaleCur;

	}


class UnwrapRightMenu : public RightClickMenu {
	private:
		UnwrapMod *ep;
	public:
		void Init(RightClickMenuManager* manager, HWND hWnd, IPoint2 m);
		void Selected(UINT id);
		void SetMod(UnwrapMod *ep) { this->ep = ep; }
	};



void UnwrapRightMenu::Init(RightClickMenuManager* manager, HWND hWnd, IPoint2 m) {
		
		int flags1,flags2,flags3,flags4,flags5,flags6;

		flags1 = flags2 = flags3 = flags4 = flags5 = flags6 = MF_STRING | MF_UNCHECKED;


		
		manager->AddMenu(this, MF_SEPARATOR, 0, NULL);
		manager->AddMenu(this, flags1, 0, GetString(IDS_PW_FACEMODE));
		manager->AddMenu(this, flags6, 5, GetString(IDS_PW_APPLYPLANAR));

		}
	

void UnwrapRightMenu::Selected(UINT id) {
//  Add Cross Section
	if (id ==  0)
		ep->ip->SetSubObjectLevel(1);	
	else if (id == 5)
		{
		SendMessage(ep->hParams,WM_COMMAND,IDC_UNWRAP_APPLY,0);

		}

	}

UnwrapRightMenu rMenu;



class UnwrapRegionFitMouseProc : public MouseCallBack {
	public:
		UnwrapMod *mod;
		IObjParam *ip;
		IPoint2 om;
		UnwrapRegionFitMouseProc(UnwrapMod *m,IObjParam *i) {mod=m;ip=i;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		void RegionFitMap(HWND hWnd,IPoint2 m);
	};

class UnwrapRegionFitMode : public CommandMode {
	public:
		ChangeFGObject fgProc;
		UnwrapRegionFitMouseProc proc;
		IObjParam *ip;
		UnwrapMod *mod;

		UnwrapRegionFitMode(UnwrapMod *m,IObjParam *i) 
			: fgProc(m), proc(m,i) {ip=i;mod=m;}

		int Class() {return MOVE_COMMAND;}		
		int ID() {return CID_REGIONFIT;}
		MouseCallBack *MouseProc(int *numPoints) {*numPoints=2;return &proc;}
		ChangeForegroundCallback *ChangeFGProc() {return &fgProc;}
		BOOL ChangeFG(CommandMode *oldMode) {return oldMode->ChangeFGProc() != &fgProc;}
		void EnterMode();
		void ExitMode();
	};


class UVWUnwrapDeleteEvent : public EventUser {
public:
	UnwrapMod *m;

	void Notify() {if (m) 
						{
						m->DeleteSelected();
						m->InvalidateView();

						}
					}
	void SetEditMeshMod(UnwrapMod *im) {m=im;}
};

UVWUnwrapDeleteEvent delEvent;

class MeshTopoData : public LocalModData {
public:
	Mesh *mesh;
	PatchMesh *patch;
	BitArray faceSel;
	GenericNamedSelSetList fselSet;

	MeshTopoData(Mesh &mesh);
	MeshTopoData(PatchMesh &patch);
	MeshTopoData() { mesh=NULL; patch = NULL; }
	~MeshTopoData() { FreeCache(); }
	LocalModData *Clone();

	Mesh *GetMesh() {return mesh;}
	PatchMesh *GetPatch() {return patch;}
	void SetCache(Mesh &mesh);
	void SetCache(PatchMesh &patch);
	void FreeCache();

	BitArray &GetFaceSel() { return faceSel; }
	void SetFaceSel(BitArray &set, UnwrapMod *imod, TimeValue t);

};

LocalModData *MeshTopoData::Clone() {
	MeshTopoData *d = new MeshTopoData;
	mesh = NULL;
	patch = NULL;
	return d;
	}

MeshTopoData::MeshTopoData(Mesh &mesh) {
	this->mesh = new Mesh(mesh);
	this->patch = NULL;
}

MeshTopoData::MeshTopoData(PatchMesh &patch) {
	this->mesh = NULL;
	this->patch = new PatchMesh(patch);
}

void MeshTopoData::SetCache(Mesh &mesh)
	{
	if (this->mesh) delete this->mesh;
	this->mesh = new Mesh(mesh);
	}

void MeshTopoData::SetCache(PatchMesh &patch)
	{
	if (this->patch) delete this->patch;
	this->patch = new PatchMesh(patch);
	}

void MeshTopoData::FreeCache() {
	if (mesh) delete mesh;
	mesh = NULL;
	if (patch) delete patch;
	patch = NULL;
}

void MeshTopoData::SetFaceSel(BitArray &set, UnwrapMod *imod, TimeValue t) {
	UnwrapMod *mod = (UnwrapMod *) imod;
	faceSel = set;
	if (mesh) mesh->faceSel = set;
	if (patch) patch->patchSel = set;
}




class UnwrapClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new UnwrapMod;}
	const TCHAR *	ClassName() {return UNWRAP_NAME;}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return UNWRAP_CLASSID;}
	const TCHAR* 	Category() {return GetString(IDS_RB_DEFSURFACE);}
	};

static UnwrapClassDesc unwrapDesc;
ClassDesc* GetUnwrapModDesc() {return &unwrapDesc;}

#define ID_TOOL_MOVE	0x0100
#define ID_TOOL_ROTATE	0x0110
#define ID_TOOL_SCALE	0x0120
#define ID_TOOL_PAN		0x0130
#define ID_TOOL_ZOOM    0x0140
#define ID_TOOL_PICKMAP	0x0160
#define ID_TOOL_ZOOMREG 0x0170
#define ID_TOOL_UVW		0x0200
#define ID_TOOL_PROP	0x0210 
#define ID_TOOL_SHOWMAP	0x0220
#define ID_TOOL_UPDATE	0x0230
#define ID_TOOL_ZOOMEXT	0x0240
#define ID_TOOL_BREAK	0x0250
#define ID_TOOL_WELD	0x0260
#define ID_TOOL_WELD_SEL 0x0270
#define ID_TOOL_HIDE	 0x0280
#define ID_TOOL_UNHIDE	 0x0290
#define ID_TOOL_FREEZE	 0x0300
#define ID_TOOL_UNFREEZE	 0x0310
#define ID_TOOL_TEXTURE_COMBO 0x0320
#define ID_TOOL_SNAP 0x0330
#define ID_TOOL_LOCKSELECTED 0x0340
#define ID_TOOL_MIRROR 0x0350
#define ID_TOOL_FILTER_SELECTEDFACES 0x0360
#define ID_TOOL_FILTER_MATID 0x0370
#define ID_TOOL_INCSELECTED 0x0380
#define ID_TOOL_FALLOFF 0x0390
#define ID_TOOL_FALLOFF_SPACE 0x0400




HWND            UnwrapMod::hParams = NULL;
HWND            UnwrapMod::hWnd = NULL;
HWND            UnwrapMod::hView = NULL;
IObjParam      *UnwrapMod::ip = NULL;
ICustToolbar   *UnwrapMod::iTool = NULL;
ICustToolbar   *UnwrapMod::iVertex = NULL;
ICustToolbar   *UnwrapMod::iView = NULL;
ICustToolbar   *UnwrapMod::iOption = NULL;
ICustToolbar   *UnwrapMod::iFilter = NULL;
ICustButton    *UnwrapMod::iMove = NULL;
ICustButton    *UnwrapMod::iRot = NULL;
ICustButton    *UnwrapMod::iScale = NULL;
ICustButton    *UnwrapMod::iFalloff = NULL;
ICustButton    *UnwrapMod::iFalloffSpace = NULL;

ICustButton    *UnwrapMod::iMirror = NULL;
ICustButton    *UnwrapMod::iWeld = NULL;
ICustButton    *UnwrapMod::iPan = NULL;
ICustButton    *UnwrapMod::iZoom = NULL;
ICustButton    *UnwrapMod::iUpdate = NULL;
ISpinnerControl *UnwrapMod::iU = NULL;
ISpinnerControl *UnwrapMod::iV = NULL;
ISpinnerControl *UnwrapMod::iW = NULL;
ISpinnerControl *UnwrapMod::iStr = NULL;
ISpinnerControl *UnwrapMod::iMapID = NULL;
MouseManager    UnwrapMod::mouseMan;
IOffScreenBuf  *UnwrapMod::iBuf = NULL;
int             UnwrapMod::mode = ID_TOOL_MOVE;
int             UnwrapMod::oldMode = ID_TOOL_MOVE;
 
MoveMode       *UnwrapMod::moveMode = NULL;
RotateMode     *UnwrapMod::rotMode = NULL;
ScaleMode      *UnwrapMod::scaleMode = NULL;
PanMode        *UnwrapMod::panMode = NULL;
ZoomMode       *UnwrapMod::zoomMode = NULL;
ZoomRegMode    *UnwrapMod::zoomRegMode = NULL;
WeldMode       *UnwrapMod::weldMode = NULL;
RightMouseMode *UnwrapMod::rightMode = NULL;
MiddleMouseMode *UnwrapMod::middleMode = NULL;
BOOL            UnwrapMod::viewValid = FALSE;
BOOL            UnwrapMod::typeInsValid = FALSE;
UnwrapMod      *UnwrapMod::editMod = NULL;
ICustButton    *UnwrapMod::iZoomReg = NULL;
ICustButton    *UnwrapMod::iZoomExt = NULL;
ICustButton    *UnwrapMod::iUVW = NULL;
ICustButton    *UnwrapMod::iProp = NULL;
ICustButton    *UnwrapMod::iShowMap = NULL;
ICustButton    *UnwrapMod::iLockSelected = NULL;
ICustButton    *UnwrapMod::iFilterSelected = NULL;
ICustButton    *UnwrapMod::iHide = NULL;
ICustButton    *UnwrapMod::iFreeze = NULL;
ICustButton    *UnwrapMod::iIncSelected = NULL;
ICustButton    *UnwrapMod::iSnap = NULL;

ICustButton    *UnwrapMod::iBreak = NULL;
ICustButton    *UnwrapMod::iWeldSelected = NULL;



SelectModBoxCMode *UnwrapMod::selectMode      = NULL;
/*
MoveModBoxCMode*    UnwrapMod::gmoveMode      = NULL;
RotateModBoxCMode*  UnwrapMod::grotMode       = NULL;
UScaleModBoxCMode*  UnwrapMod::guscaleMode    = NULL;
NUScaleModBoxCMode* UnwrapMod::gnuscaleMode   = NULL;
SquashModBoxCMode*  UnwrapMod::gsquashMode    = NULL;
UnwrapFaceAlignMode*      UnwrapMod::faceAlignMode = NULL;
UnwrapRegionFitMode*      UnwrapMod::regionFitMode = NULL;
*/
//ICustButton*			  UnwrapMod::iAlignButton  = NULL;
ICustButton*			  UnwrapMod::iApplyButton  = NULL;
//ICustButton*			  UnwrapMod::iFitButton  = NULL;
int						  UnwrapMod::CurrentMap = 0;
COLORREF UnwrapMod::lineColor = RGB(255,255,255);
COLORREF UnwrapMod::selColor  = RGB(255,0,0);
float UnwrapMod::weldThreshold = 0.01f;
BOOL UnwrapMod::update = TRUE;
int UnwrapMod::showVerts = 1;
int UnwrapMod::midPixelSnap = 0;

static BOOL CALLBACK UnwrapFloaterDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class TVertRestore : public RestoreObj {
	public:
		UnwrapMod *mod;
		Tab<UVW_TVVertClass> undo, redo;
		BitArray uvsel, rvsel;

		TVertRestore(UnwrapMod *m) {
			mod = m;
//			undo = mod->tvert;
			undo = mod->TVMaps.v;
			uvsel   = mod->vsel;

			}
		void Restore(int isUndo) {
			if (isUndo) {
//				redo = mod->tvert;
				redo = mod->TVMaps.v;
				rvsel   = mod->vsel;

				}
//			mod->tvert = undo;
			mod->TVMaps.v = undo;
			mod->vsel = uvsel;

			mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void Redo() {
			mod->TVMaps.v = redo;
			mod->vsel = rvsel;
			mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PW_UVW_VERT_EDIT)));
		}
		
	};

class TVertAndTFaceRestore : public RestoreObj {
	public:
		UnwrapMod *mod;
		Tab<UVW_TVVertClass> undo, redo;
		Tab<UVW_TVFaceClass> fundo, fredo;
		BitArray uvsel, rvsel;



		TVertAndTFaceRestore(UnwrapMod *m) {
			mod = m;
//			undo = mod->tvert;
			undo = mod->TVMaps.v;
			fundo = mod->TVMaps.f;
			uvsel   = mod->vsel;
			}
		void Restore(int isUndo) {
			if (isUndo) {
//				redo = mod->tvert;
				redo = mod->TVMaps.v;
				fredo = mod->TVMaps.f;
				rvsel   = mod->vsel;
				}
//			mod->tvert = undo;
			mod->TVMaps.v = undo;
			mod->TVMaps.f = fundo;
			mod->vsel = uvsel;
			mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void Redo() {
			mod->TVMaps.v = redo;
			mod->TVMaps.f = fredo;
			mod->vsel = rvsel;

			mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PW_UVW_EDIT)));}
	};


class TSelRestore : public RestoreObj {
	public:
		UnwrapMod *mod;
		BitArray undo, redo;

		TSelRestore(UnwrapMod *m) {
			mod = m;
			undo = mod->vsel;
			}
		void Restore(int isUndo) {
			if (isUndo) {
				redo = mod->vsel;

				}
			mod->vsel = undo;
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void Redo() {
			mod->vsel = redo;
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			}
		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PW_SELECT_UVW)));}
	};	

class ResetRestore : public RestoreObj {
	public:
		UnwrapMod *mod;
/*
		Tab<UVW_TVVertClass> utvert, rtvert;
		Tab<UVW_TVFaceClass> utvFace, rtvFace;
		Tab<Control*> ucont, rcont;
*/
		UVW_ChannelClass undoTVMaps,redoTVMaps;
		BitArray uvsel, rvsel;
		int uchan, rchan;
		

		ResetRestore(UnwrapMod *m) {
			mod     = m;
//			utvert  = mod->tvert;
//			utvFace = mod->tvFace;
//			ucont   = mod->cont;
			undoTVMaps = mod->TVMaps;
			uvsel   = mod->vsel;
			uchan   = mod->channel;
			}
		void Restore(int isUndo) {
			if (isUndo) {
				redoTVMaps = mod->TVMaps;
//				rtvert  = mod->tvert;
//				rtvFace = mod->tvFace;
//				rcont   = mod->cont;
				rvsel   = mod->vsel;
				rchan   = mod->channel;
				}
//			mod->tvert   = utvert;
//			mod->tvFace  = utvFace;
//			mod->cont   = ucont;
			mod->TVMaps = undoTVMaps;
			mod->vsel    = uvsel;
			mod->channel = uchan;
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			mod->SetupChannelButtons();
			}
		void Redo() {
//			mod->tvert   = rtvert;
//			mod->tvFace  = rtvFace;
//			mod->cont   = rcont;
			mod->TVMaps = redoTVMaps;
			mod->vsel    = rvsel;
			mod->channel = rchan;
			if (mod->editMod==mod && mod->hView) mod->InvalidateView();
			mod->SetupChannelButtons();
			}		
		TSTR Description() {return TSTR(_T(GetString(IDS_PW_RESET_UNWRAP)));
		}
	};

// MeshSelRestore --------------------------------------------------
class UnwrapSelRestore : public RestoreObj {
public:
	BitArray usel, rsel;
	BitArray *sel;
	UnwrapMod *mod;
	MeshTopoData *d;
//	int level;

	UnwrapSelRestore(UnwrapMod *m, MeshTopoData *d);
	UnwrapSelRestore(UnwrapMod *m, MeshTopoData *d, int level);
	void Restore(int isUndo);
	void Redo();
	int Size() { return 1; }
	void EndHold() {
//		d->held=FALSE;
	}
	TSTR Description() { return TSTR(_T(GetString(IDS_PW_SELECTRESTORE))); 
	}
};


UnwrapSelRestore::UnwrapSelRestore(UnwrapMod *m, MeshTopoData *data) {
	mod     = m;
	d       = data;
//	d->held = TRUE;
//	usel = d->faceSel;
	usel.SetSize(mod->TVMaps.f.Count());
	usel.ClearAll();
	for (int i =0; i < usel.GetSize();i++)
		{
		if (mod->TVMaps.f[i].flags & FLAG_SELECTED)
			usel.Set(i);
		}

	
}

UnwrapSelRestore::UnwrapSelRestore(UnwrapMod *m, MeshTopoData *data, int sLevel) 
{
	mod     = m;
//	level   = sLevel;
	d       = data;
//	d->held = TRUE;
	usel = d->faceSel;
	
}

void UnwrapSelRestore::Restore(int isUndo) {
	if (isUndo) {
		rsel = d->faceSel;
		}
	
	d->faceSel = usel; 
	mod->UpdateFaceSelection(d->faceSel);
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	mod->InvalidateView();
//	mod->SetNumSelLabel();
}

void UnwrapSelRestore::Redo() {
	d->faceSel = rsel;
	mod->UpdateFaceSelection(d->faceSel);
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	mod->InvalidateView();
	}


//--- UnwrapMod methods -----------------------------------------------

UnwrapMod::UnwrapMod()
	{
	move    = 0;
	zoom    = 0.9f;
	aspect	= 1.0f;
	xscroll = 0.0f;
	yscroll = 0.0f;
	for (int i =0;i<10;i++)
		map[i] = NULL;
	image = NULL;
	iw = ih = 0;
	uvw = 0;
	scale =0;
	zoomext =0;
	showMap = TRUE;	
	rendW = 256;
	rendH = 256;
	channel = 0;
//	CurrentMap = 0;
	isBitmap = 0;
	useBitmapRes = TRUE;
	pixelSnap =0;
	tmControl = NULL;
	offsetControl = NULL;
	scaleControl = NULL;
	rotateControl = NULL;
	TVMaps.v.ZeroCount();
	TVMaps.f.ZeroCount();
	flags = CONTROL_CENTER|CONTROL_FIT|CONTROL_INIT;
	PlanarTM.IdentityMatrix();	
	gScale.x = 0.25f;
	gScale.y = 0.25f;
	gScale.z = 0.0f;
	gOffset.x =0.5f;
	gOffset.y =0.5f;
	gOffset.z =0.5f;
	version = 2;
	lockSelected = 0;
	mirror = 0;
	filterSelectedFaces = 0;
	hide = 0;
	freeze = 0;
	incSelected = 0;
	falloff = 0;
	falloffSpace = 0;
	alignDir = 3;
	oldDataPresent = FALSE;
	firstPass = FALSE;
	type = MAP_PLANAR;

	updateCache = FALSE;
	instanced = FALSE;


//	lineColor = RGB(255,255,255);
//	selColor  = RGB(255,0,0);
//	weldThreshold = 0.01f;
//	update = TRUE;
//	showVerts = 1;
//	midPixelSnap = 0;
	}

UnwrapMod::~UnwrapMod()
	{
	DeleteAllRefsFromMe();
	}

static BOOL CALLBACK UnwrapRollupWndProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	UnwrapMod *mod = (UnwrapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	
	static BOOL inEnter = FALSE;

	switch (msg) {
		case WM_INITDIALOG:
			mod = (UnwrapMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;
			
			mod->iApplyButton = GetICustButton(GetDlgItem(hWnd, IDC_UNWRAP_APPLY));
			mod->iApplyButton->SetType(CBT_PUSH);
			mod->iApplyButton->SetHighlightColor(GREEN_WASH);
			mod->iApplyButton->Enable(TRUE);

			mod->iMapID = GetISpinner(GetDlgItem(hWnd,IDC_MAP_CHAN_SPIN));
			mod->iMapID->LinkToEdit(GetDlgItem(hWnd,IDC_MAP_CHAN),EDITTYPE_INT);
			mod->iMapID->SetLimits(1, 99, FALSE);
			mod->iMapID->SetAutoScale();	

			mod->SetupChannelButtons();
			mod->ip->GetRightClickMenuManager()->Register(&rMenu);


			CheckDlgButton(hWnd,IDC_RADIO4,TRUE);
			mod->alignDir = 3;

			break;


		case WM_CUSTEDIT_ENTER:
				{
				if (!inEnter)
					{
					inEnter = TRUE;
					TSTR buf1 = GetString(IDS_RB_SHOULDRESET);
					TSTR buf2 = GetString(IDS_RB_UNWRAPMOD);
					int tempChannel = mod->iMapID->GetIVal();
					if (tempChannel == 1) tempChannel = 0;
					if (tempChannel != mod->channel)
						{
						int res = MessageBox(mod->ip->GetMAXHWnd(),buf1,buf2,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
						if (res==IDYES)
							{
							theHold.Begin();
							mod->Reset();
							mod->channel = mod->iMapID->GetIVal();
							if (mod->channel == 1) mod->channel = 0;
							theHold.Accept(GetString(IDS_RB_SETCHANNEL));					
							mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
							mod->ip->RedrawViews(mod->ip->GetTime());
							mod->InvalidateView();
							}
						else mod->SetupChannelButtons();
						}
					inEnter = FALSE;

					}

				}

			break;
		case CC_SPINNER_BUTTONUP:
				{
				TSTR buf1 = GetString(IDS_RB_SHOULDRESET);
				TSTR buf2 = GetString(IDS_RB_UNWRAPMOD);
				int tempChannel = mod->iMapID->GetIVal();
				if (tempChannel == 1) tempChannel = 0;
				if (tempChannel != mod->channel)
					{
					int res = MessageBox(mod->ip->GetMAXHWnd(),buf1,buf2,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
					if (res==IDYES)
						{
						theHold.Begin();
						mod->Reset();
						mod->channel = mod->iMapID->GetIVal();
						if (mod->channel == 1) mod->channel = 0;
						theHold.Accept(GetString(IDS_RB_SETCHANNEL));					
						mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
						mod->ip->RedrawViews(mod->ip->GetTime());
						mod->InvalidateView();
						}
					else mod->SetupChannelButtons();
					}

				}

			break;


		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_UNWRAP_SAVE:
					DragAcceptFiles(mod->ip->GetMAXHWnd(), FALSE);
					mod->SaveUVW(hWnd);
					DragAcceptFiles(mod->ip->GetMAXHWnd(), TRUE);
					break;
				case IDC_UNWRAP_LOAD:
					DragAcceptFiles(mod->ip->GetMAXHWnd(), FALSE);
					mod->LoadUVW(hWnd);
					DragAcceptFiles(mod->ip->GetMAXHWnd(), TRUE);
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;

				case IDC_RADIO1: 
					if (IsDlgButtonChecked(hWnd,IDC_RADIO1)) mod->alignDir = 0;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					mod->InvalidateView();
					break;
				case IDC_RADIO2:
					if (IsDlgButtonChecked(hWnd,IDC_RADIO2)) mod->alignDir = 1;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					mod->InvalidateView();
					break;
				case IDC_RADIO3:
					if (IsDlgButtonChecked(hWnd,IDC_RADIO3)) mod->alignDir = 2;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					mod->InvalidateView();
					break;

				case IDC_RADIO4:
					if (IsDlgButtonChecked(hWnd,IDC_RADIO4)) mod->alignDir = 3;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					mod->InvalidateView();

					break;

				case IDC_MAP_CHAN1:
				case IDC_MAP_CHAN2: {
					TSTR buf1 = GetString(IDS_RB_SHOULDRESET);
					TSTR buf2 = GetString(IDS_RB_UNWRAPMOD);
					int res = MessageBox(mod->ip->GetMAXHWnd(),buf1,buf2,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
					if (res==IDYES)
						{
						theHold.Begin();
						mod->Reset();
						mod->channel = IsDlgButtonChecked(hWnd,IDC_MAP_CHAN2);
						if (mod->channel == 1)
							mod->iMapID->Enable(FALSE);
						else 
							{
							int ival = mod->iMapID->GetIVal();
							if (ival == 1) mod->channel = 0;
							  else mod->channel = ival;
							mod->iMapID->Enable(TRUE);
							}
						mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
						mod->ip->RedrawViews(mod->ip->GetTime());
						mod->InvalidateView();

						theHold.Accept(GetString(IDS_RB_SETCHANNEL));					
						}
					else mod->SetupChannelButtons();

					break;
					}

				case IDC_UNWRAP_RESET: {
					TSTR buf1 = GetString(IDS_RB_RESETUNWRAPUVWS);
					TSTR buf2 = GetString(IDS_RB_UNWRAPMOD);
					if (IDYES==MessageBox(mod->ip->GetMAXHWnd(),buf1,buf2,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL)) {
						theHold.Begin();
						mod->Reset();
						theHold.Accept(GetString(IDS_RB_RESETUVWS));
						mod->ip->RedrawViews(mod->ip->GetTime());
						mod->InvalidateView();

						}
					break;
					}

				case IDC_UNWRAP_APPLY:
					{
//align to normals
					mod->AlignMap();
//call fit

					mod->flags |= CONTROL_FIT|CONTROL_CENTER|CONTROL_HOLD;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					mod->ApplyGizmo();					
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());


					break;
					}
				case IDC_UNWRAP_EDIT:
					mod->EditMapping(hWnd);
					break;
				case IDC_UNWRAP_FIT:
//					mod->flags |= CONTROL_FIT|CONTROL_CENTER|CONTROL_HOLD;
//					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
//					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
				case IDC_UNWRAP_ALIGN_TO_NORMAL:
/*
					if (mod->ip->GetCommandMode()->ID()==CID_FACEALIGNMAP) {
						mod->ip->SetStdCommandMode(CID_OBJMOVE);
					} else {
						mod->ip->SetCommandMode(mod->faceAlignMode);
						}
*/
					break;




				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void UnwrapMod::SetupChannelButtons()
	{
	if (hParams && editMod==this) {		
		if (channel == 0)
			{
			iMapID->Enable(TRUE);
			iMapID->SetValue(1,TRUE);
			CheckDlgButton(hParams,IDC_MAP_CHAN1,TRUE);
			CheckDlgButton(hParams,IDC_MAP_CHAN2,FALSE);

			}
		else if (channel == 1)
			{
			CheckDlgButton(hParams,IDC_MAP_CHAN1,FALSE);
			CheckDlgButton(hParams,IDC_MAP_CHAN2,TRUE);
			iMapID->Enable(FALSE);
//			iMapID->SetValue(0,TRUE);
			}
		else
			{
			CheckDlgButton(hParams,IDC_MAP_CHAN1,TRUE);
			CheckDlgButton(hParams,IDC_MAP_CHAN2,FALSE);
			iMapID->Enable(TRUE);
			iMapID->SetValue(channel,TRUE);
			}
		}
	}

void UnwrapMod::Reset()
	{
	if (theHold.Holding()) theHold.Put(new ResetRestore(this));

	for (int i=0; i<TVMaps.cont.Count(); i++) DeleteReference(i+11);
	TVMaps.v.Resize(0);
	TVMaps.f.Resize(0);
	TVMaps.cont.Resize(0);
	vsel.SetSize(0);
	updateCache = TRUE;

	NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE,TREE_VIEW_CLASS_ID,FALSE);	
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

BOOL UnwrapMod::AssignController(Animatable *control,int subAnim)
	{
	ReplaceReference(subAnim+11,(RefTargetHandle)control);	
	NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE,TREE_VIEW_CLASS_ID,FALSE);	
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	return TRUE;
	}

void UnwrapMod::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	selCur   = ip->GetSysCursor(SYSCUR_SELECT);
	moveCur	 = ip->GetSysCursor(SYSCUR_MOVE);
	
	if (moveXCur == NULL)
		moveXCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MOVEX));
	if (moveYCur == NULL)
		moveYCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MOVEY));

	if (scaleXCur == NULL)
		scaleXCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SCALEX));
	if (scaleYCur == NULL)
		scaleYCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SCALEY));

	if (zoomCur == NULL)
		zoomCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ZOOM));
	if (zoomRegionCur == NULL)
		zoomRegionCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ZOOMREG));
	if (panCur == NULL)
		panCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_PANHAND));
	if (weldCur == NULL)
		weldCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_WELDCUR));


	rotCur   = ip->GetSysCursor(SYSCUR_ROTATE);
	scaleCur = ip->GetSysCursor(SYSCUR_USCALE);


	// Add our sub object type
	TSTR type1(GetString(IDS_PW_SELECTFACE));
	TSTR type2 (GetString(IDS_PW_FACEMAP));
	TSTR type3 (GetString(IDS_PW_PLANAR));
	const TCHAR *ptype[] = {type1};
	ip->RegisterSubObjectTypes(ptype, 1);

	selectMode = new SelectModBoxCMode(this,ip);
/*
	gmoveMode      = new MoveModBoxCMode(this,ip);
	grotMode       = new RotateModBoxCMode(this,ip);
	guscaleMode    = new UScaleModBoxCMode(this,ip);
	gnuscaleMode   = new NUScaleModBoxCMode(this,ip);
	gsquashMode    = new SquashModBoxCMode(this,ip);	

	regionFitMode = new UnwrapRegionFitMode(this,ip);
	faceAlignMode = new UnwrapFaceAlignMode(this,ip);
*/

	offsetControl = NewDefaultPoint3Controller();
	Point3 p(0.5f,0.5f,0.0f);
	offsetControl->SetValue(0,&p,CTRL_ABSOLUTE);

	scaleControl = NewDefaultPoint3Controller();
	Point3 sp(0.25f,0.25f,0.0f);
	scaleControl->SetValue(0,&sp,CTRL_ABSOLUTE);

	rotateControl = NewDefaultFloatController();
	float a = 0.0f;
	rotateControl->SetValue(0,&a,CTRL_ABSOLUTE);


	this->ip = ip;
	editMod  = this;
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	hParams  = ip->AddRollupPage( 
		hInstance, 
		MAKEINTRESOURCE(IDD_UNWRAP_PARAMS),
		UnwrapRollupWndProc,
		GetString(IDS_RB_PARAMETERS),
		(LPARAM)this);

	ip->RegisterTimeChangeCallback(this);

	SetNumSelLabel();
	firstPass = TRUE;

	}


void UnwrapMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{	
	ClearAFlag(A_MOD_BEING_EDITED);

	ip->UnRegisterTimeChangeCallback(this);
	if (hParams) ip->DeleteRollupPage(hParams);
	hParams  = NULL;	
	if (hWnd) DestroyWindow(hWnd);

/*
	ip->DeleteMode(gmoveMode);
	ip->DeleteMode(grotMode);
	ip->DeleteMode(guscaleMode);
	ip->DeleteMode(gnuscaleMode);
	ip->DeleteMode(gsquashMode);	
	ip->DeleteMode(regionFitMode);
*/

	ip->DeleteMode(selectMode);

	if (selectMode) delete selectMode;
	selectMode = NULL;
/*
	delete gmoveMode; gmoveMode = NULL;
	delete grotMode; grotMode = NULL;
	delete guscaleMode; guscaleMode = NULL;
	delete gnuscaleMode; gnuscaleMode = NULL;
	delete gsquashMode; gsquashMode = NULL;
	delete faceAlignMode; faceAlignMode = NULL;
	delete regionFitMode; regionFitMode = NULL;
*/

//	ReleaseICustButton(iAlignButton);
//	iAlignButton = NULL;

	ReleaseICustButton(iApplyButton);
	iApplyButton = NULL;

//	ReleaseICustButton(iFitButton);
//	iFitButton = NULL;
	ReleaseISpinner(iMapID); iMapID = NULL;

	ip->GetRightClickMenuManager()->Unregister(&rMenu);


	TimeValue t =ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	this->ip = NULL;

	editMod  = NULL;
	
	ip->EnableSubObjectSelection(TRUE);


	}




class NullView: public View {
	public:
		Point2 ViewToScreen(Point3 p) { return Point2(p.x,p.y); }
		NullView() { worldToView.IdentityMatrix(); screenW=640.0f; screenH = 480.0f; }
	};

static AdjFaceList *BuildAdjFaceList(Mesh &mesh)
	{
	AdjEdgeList ae(mesh);
	return new AdjFaceList(mesh,ae);
	}


Box3 UnwrapMod::BuildBoundVolume(Object *obj)

{
Box3 b;
b.Init();
if (objType == IS_PATCH)
	{
	PatchObject *pobj = (PatchObject*)obj;
	for (int i = 0; i < pobj->patch.patchSel.GetSize(); i++)
		{
		if (pobj->patch.patchSel[i])
			{
			int pcount = 3;
			if (pobj->patch.patches[i].type == PATCH_QUAD) pcount = 4;
			for (int j = 0; j < pcount; j++)
				{
				int index = pobj->patch.patches[i].v[j];

				b+= pobj->patch.verts[index].p;
				}
			}	
		}

	}	
else if (objType == IS_MESH)
	{
	TriObject *tobj = (TriObject*)obj;
	for (int i = 0; i < tobj->GetMesh().faceSel.GetSize(); i++)
		{
		if (tobj->GetMesh().faceSel[i])
			{
			for (int j = 0; j < 3; j++)
				{
				int index = tobj->GetMesh().faces[i].v[j];

				b+= tobj->GetMesh().verts[index];
				}
			}	
		}
	}
return b;
}


void UnwrapMod::InitControl(TimeValue t)
	{
	Box3 box;
	Matrix3 tm;

	if (tmControl==NULL) {
		MakeRefByID(FOREVER,0,NewDefaultMatrix3Controller()); 
		NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE);
		}		
	
	if (flags&CONTROL_INIT) {
		SuspendAnimate();
		AnimateOff();		
		// Rotate the seem to the back
		s.x = 100.0f;
		s.y = 100.0f;
		s.z = 100.0f;	

		SetXFormPacket pckt(Quat(RotateZMatrix(0)),TRUE);
		tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		ResumeAnimate();
		}
/*
	if (flags&CONTROL_INITPARAMS) {
		}

	if (!(flags&CONTROL_OP)) {
		flags = 0;
		return;
		}

	if (flags&CONTROL_HOLD) theHold.Begin();	

	Matrix3 oldtm = tm = Inverse(CompMatrix(t,&mc,NULL,FALSE));	
	AffineParts parts;
	decomp_affine(tm, &parts);
	parts.q.MakeMatrix(tm);
	tm.Translate(parts.t);
	
	Matrix3 mctm(1);
	if (mc.tm) mctm = *mc.tm;
	tm.Scale(Point3(
		Length(mctm.GetRow(0)),
		Length(mctm.GetRow(1)),
		Length(mctm.GetRow(2))
		));

	Point3 tp;
	tp  = s;
	box = BuildBoundVolume(obj);
	box = box * tm;
	box.Scale(1.001f);  // prevent wrap-around on sides of boxes

	int type = MAP_PLANAR;

	if (flags&CONTROL_ASPECT &&
		(type==MAP_PLANAR ||
		 type==MAP_BOX ||
		 type==MAP_CYLINDRICAL)) {

		if (type==MAP_PLANAR || type==MAP_BOX) {			
			float w = aspect*tp.y;
			tp.x *= w / tp.x;			
			tp.z *= tp.y / tp.z;				
		} else
		if (type==MAP_CYLINDRICAL) {
			float w = (tp.x+tp.y)*PI;			
			tp.z *= w/(aspect*tp.z);
			}
		}

	if (flags&CONTROL_UNIFORM) {
		float av = (tp.x + tp.y + tp.z)/3.0f;
		tp.x = tp.y = tp.z = av;
		}

	if (flags&CONTROL_FIT) {
		Point3 w = box.Width();
		if (box.IsEmpty()) w = Point3(10.0f,10.0f,10.0f);

		if (type==MAP_PLANAR) {
			tp.x = w.x==0.0f ? 1.0f : w.x;
			tp.y = w.y==0.0f ? 1.0f : w.y;
			tp.z = w.z==0.0f ? 1.0f : w.z;
		} else
		if (type==MAP_BOX) {
			tp.x = w.x==0.0f ? 1.0f : w.x;
			tp.y = w.y==0.0f ? 1.0f : w.y;
			tp.z = w.z==0.0f ? 1.0f : w.z;
		} else
		if (type==MAP_SPHERICAL || type==MAP_BALL) {
			float max = w.x;
			if (w.y>max) max = w.y;
			if (w.z>max) max = w.z;
			if (max==0.0f) max = 1.0f;
			tp.x = tp.y = tp.z = max;
		} else {
			if (w.x>w.y) tp.x = tp.y = w.x;
			else tp.x = tp.y = w.y;
			s.z = w.z;
			if (tp.x==0.0f) tp.x = 1.0f;
			if (tp.y==0.0f) tp.y = 1.0f;
			if (tp.z==0.0f) tp.z = 1.0f;			
			}

		}

	if (flags&(CONTROL_CENTER|CONTROL_FIT)) {		
		
		Box3 sbox;		
//		obj->GetDeformBBox(t,sbox,&oldtm,TRUE);

//fix this i think this needs to be put in old tm space
		sbox = BuildBoundVolume(obj);


		Point3 pt = sbox.Center();
		// Get the mod context tm
		Matrix3 tm(1);
		if (mc.tm) tm = tm * *mc.tm;
		
		// Transform the point and ray into mod context space
		pt = pt * tm;
		// Construct the target transformation in mod context space
		Matrix3 destTM(1);
		destTM.SetTrans(pt);

		// Our current transformation... gives relative TM
		Matrix3 curTM(1), relTM, id(1);
		Interval valid;
		tmControl->GetValue(t,&curTM,valid,CTRL_RELATIVE);
		relTM = Inverse(curTM) * destTM;
		
		// Here's the modifications we need to make to get there
		tm.IdentityMatrix();
		tm.SetTrans(curTM.GetTrans());
		AffineParts parts;			
		decomp_affine(relTM,&parts);
		Point3 delta = destTM.GetTrans()-curTM.GetTrans();
//		mod->Rotate(t,id,tm,parts.q);
		Move(t,id,id,delta);

		}

	if (flags&(CONTROL_FIT|CONTROL_ASPECT|CONTROL_INITPARAMS|CONTROL_UNIFORM)) {
		SuspendAnimate();
		AnimateOff();

		// Clear out any scale in the transform
		tm = Matrix3(1);	
		tmControl->GetValue(t,&tm,FOREVER,CTRL_RELATIVE);	
		decomp_affine(tm, &parts);
		parts.q.MakeMatrix(tm);
		tm.Translate(parts.t);	
		SetXFormPacket pckt(tm);		
		tmControl->SetValue(t,&pckt,TRUE,CTRL_ABSOLUTE);

		// Set the new dimensions
		s = tp;
		
		ResumeAnimate();
		}

	if (flags&CONTROL_HOLD) theHold.Accept(0);
*/
	flags = 0;
	}


/*
// --- Gizmo transformations ------------------------------------------

void UnwrapMod::Move(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin) 
	{	
	assert(tmControl);	
	SetXFormPacket pckt(val,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
	}


void UnwrapMod::Rotate(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Quat& val, BOOL localOrigin) 
	{
	assert(tmControl);
	SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);		
	}

void UnwrapMod::Scale(
		TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin) 
	{
	assert(tmControl);
	SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);		
	}

*/

Matrix3 UnwrapMod::CompMatrix(
		TimeValue t,ModContext *mc, Matrix3 *ntm,BOOL applySize, BOOL applyAxis)
	{
	Matrix3 tm(1);
	Interval valid;
	
//	int type = GetMapType();

	if (tmControl) {
		tmControl->GetValue(t,&tm,valid,CTRL_RELATIVE);
		}
	
	// Rotate icon	
	if (applyAxis && TestAFlag(A_PLUGIN1)) {
		switch (type) {
			case MAP_BOX:
			case MAP_PLANAR:
				tm.PreRotateZ(PI);
				break;
			
			case MAP_BALL:
			case MAP_SPHERICAL:
			case MAP_CYLINDRICAL:
				tm.PreRotateZ(HALFPI);
				break;
			}
		}
	
	if (applyAxis) {
		switch (GetAxis()) {
			case 0:
				tm.PreRotateY(-HALFPI);
				break;
			case 1:			
				tm.PreRotateX(HALFPI);
				break;
			}
		}

	if (applySize) {
		Point3 ts;
		ts = s;
//		Point3 s;
//		s.x = GetWidth(t);
//		s.y = GetLength(t);
//		s.z = GetHeight(t);
//		s.x = 100.0f;
//		s.y = 100.0f;
//		s.z = 100.0f;
		switch (type) {
			case MAP_CYLINDRICAL:			
			case MAP_PLANAR:
				ts.x *= 0.5f;
				ts.y *= 0.5f;
				break;
			
			case MAP_BALL:
			case MAP_SPHERICAL:
			case MAP_BOX:
				ts *= 0.5f;				
				break;
			}
		tm.PreScale(ts);
		}

	if (mc && mc->tm) {
		tm = tm * Inverse(*mc->tm);
		}
	if (ntm) {
		tm = tm * *ntm;
		}
	return tm;
	}

static int lStart[12] = {0,1,3,2,4,5,7,6,0,1,2,3};
static int lEnd[12]   = {1,3,2,0,5,7,6,4,4,5,6,7};

static void DoBoxIcon(BOOL sel,float length, PolyLineProc& lp)
	{
	Point3 pt[3];
	
	length *= 0.5f;
	Box3 box;
	box.pmin = Point3(-length,-length,-length);
	box.pmax = Point3( length, length, length);

	if (sel) //lp.SetLineColor(1.0f,1.0f,0.0f);
		 lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else //lp.SetLineColor(0.85f,0.5f,0.0f);		
		 lp.SetLineColor(GetUIColor(COLOR_GIZMOS));

	for (int i=0; i<12; i++) {
		pt[0] = box[lStart[i]];
		pt[1] = box[lEnd[i]];
		lp.proc(pt,2);
		}
	}


void UnwrapMod::ComputeSelectedFaceData()
{
//gNormal,gCenter;
//gXSscale,gYScale;
//compute average normal
Point3 pnorm(0.0f,0.0f,0.0f);
int ct = 0;
gCenter.x = 0.0f;
gCenter.y = 0.0f;
gCenter.z = 0.0f;
int gCt = 0;
int dir = alignDir;
if (dir == 0) //x
	{
	gNormal.x = 1.0f; 
	gNormal.y = 0.0f; 
	gNormal.z = 0.0f; 
	}
else if (dir == 1) //y
	{	
	gNormal.y = 1.0f; 
	gNormal.x = 0.0f; 
	gNormal.z = 0.0f; 
	}	
else if (dir == 2) //z
	{
	gNormal.z = 1.0f; 
	gNormal.x = 0.0f; 
	gNormal.y = 0.0f; 
	}
else
	{

	for (int k=0; k<gfaces.Count(); k++) {
		// Grap the three points, xformed
		int pcount = 3;
		if (gfaces[k].flags & FLAG_QUAD) pcount = 4;

		Point3 temp_point[4];
		for (int j=0; j<pcount; j++) {
			int index = gfaces[k].t[j];
//					Point3 temp_point;
			temp_point[j] = gverts.d[index].p;
			gCenter += gverts.d[index].p;
		
			}
		pnorm += Normalize(temp_point[1]-temp_point[0]^temp_point[2]-temp_point[1]);
		ct++;
		}
	gNormal = pnorm/(float)ct;
	}
gCenter.x = 0.0f;
gCenter.y = 0.0f;
gCenter.z = 0.0f;

gCt = 0;
float minx = 99999999999.9f,miny = 99999999999.9f,maxx= -999999999.0f,maxy= -9999999999.0f;
float minz = 99999999999.9f,maxz = -99999999999.9f;

for (int k=0; k<gverts.d.Count(); k++) {
	// Grap the three points, xformed
	if (gverts.sel[k])
		{
		Point3 pd2 = gverts.d[k].p;
		if (pd2.x < minx) minx = pd2.x;
		if (pd2.y < miny) miny = pd2.y;
		if (pd2.z < minz) minz = pd2.z;
		if (pd2.x > maxx) maxx = pd2.x;
		if (pd2.y > maxy) maxy = pd2.y;
		if (pd2.z > maxz) maxz = pd2.z;
		}
	}

gCenter.x = (float) (maxx+minx)/2.0f ;
gCenter.y = (float) (maxy+miny)/2.0f ;
gCenter.z = (float) (maxz+minz)/2.0f ;


Matrix3 tm;
UnwrapMatrixFromNormal(gNormal,tm);
tm.SetTrans(gCenter);
tm = Inverse(tm);
minx = 99999999999.9f;
miny = 99999999999.9f;
maxx= -999999999.0f;
maxy= -9999999999.0f;
minz = 99999999999.9f;
maxz = 99999999999.9f;

for (k=0; k<gfaces.Count(); k++) {
	// Grap the three points, xformed
	int pcount = 3;
	if (gfaces[k].flags & FLAG_QUAD) pcount = 4;

	for (int j=0; j<pcount; j++) {
		int index = gfaces[k].t[j];
		Point3 pd2 = gverts.d[index].p*tm;
		if (pd2.x < minx) minx = pd2.x;
		if (pd2.y < miny) miny = pd2.y;
		if (pd2.x > maxx) maxx = pd2.x;
		if (pd2.y > maxy) maxy = pd2.y;
		
		}
	}
gXScale = (float) fabs(maxx-minx)/2.0f ;
gYScale = (float) fabs(maxy-miny)/2.0f ;

}

void UnwrapMod::DoIcon(PolyLineProc& lp,BOOL sel)
	{
//	int type = GetMapType();	
	type = MAP_PLANAR;
	switch (type) {
		case MAP_BOX: DoBoxIcon(sel,2.0f,lp); break;
		case MAP_PLANAR: DoPlanarMapIcon(sel,2.0f,2.0f,lp); break;
		case MAP_BALL:
		case MAP_SPHERICAL: DoSphericalMapIcon(sel,1.0f,lp); break;
		case MAP_CYLINDRICAL: DoCylindricalMapIcon(sel,1.0f,1.0f,lp); break;
		}
	}
 

int UnwrapMod::Display(
		TimeValue t, INode* inode, ViewExp *vpt, int flags, 
		ModContext *mc) 
	{	

	if ( (ip && (ip->GetSubObjectLevel() == 1) ) && (gfaces.Count() > 0))
		{
		ComputeSelectedFaceData();

		Matrix3 vtm(1);
		Interval iv;
		if (inode) 
			vtm = inode->GetObjectTM(t,&iv);

		Point3 off;
		off = vtm.GetTrans();
		Matrix3 tm;
		UnwrapMatrixFromNormal(gNormal,tm);

		Point3 a(-0.5f,-0.5f,0.0f),b(0.5f,-0.5f,0.0f),c(0.5f,0.5f,0.0f),d(-0.5f,0.5f,0.0f);
		
		Point3 xvec,yvec;
		xvec = tm.GetRow(0)*(gXScale *1.5f);		
		yvec = tm.GetRow(1)*(gYScale *1.5f);		
		a = -xvec+yvec;
		b = xvec+yvec;
		c = xvec-yvec;
		d = -xvec-yvec;

		GraphicsWindow *gw = vpt->getGW();
		Matrix3 modmat, ntm = inode->GetObjectTM(t);

//		DrawLineProc lp(gw);

//		modmat = CompMatrix(t,mc,&ntm);	
		gw->setTransform(vtm);	
		Point3 line[5];
		line[0] = gCenter + a;
		line[1] = gCenter + b;
		line[2] = gCenter + c;
		line[3] = gCenter + d;
		line[4] = line[0];
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
		gw->polyline(5, line, NULL, NULL, 0);

//		DoIcon(lp, ip&&ip->GetSubObjectLevel()==1);
		return 1;
		}

	return 0;	
	}


void UnwrapMod::GetWorldBoundBox(
		TimeValue t,INode* inode, ViewExp *vpt, Box3& box, 
		ModContext *mc) 
	{

	if ( (ip && (ip->GetSubObjectLevel() == 1) )&& (gfaces.Count() > 0))
		{
		ComputeSelectedFaceData();

		Matrix3 tm;
		UnwrapMatrixFromNormal(gNormal,tm);

		Point3 a(-0.5f,-0.5f,0.0f),b(0.5f,-0.5f,0.0f),c(0.5f,0.5f,0.0f),d(-0.5f,0.5f,0.0f);
		
		Point3 xvec,yvec;
		xvec = tm.GetRow(0)*(gXScale * 1.5f);		
		yvec = tm.GetRow(1)*(gYScale * 1.5f);		
		a = -xvec+yvec;
		b = xvec+yvec;
		c = xvec-yvec;
		d = -xvec-yvec;

		Matrix3 modmat = inode->GetObjTMBeforeWSM(t);	

		Point3 line[5];
		line[0] = (gCenter + a)*modmat;
		line[1] = (gCenter + b)*modmat;
		line[2] = (gCenter + c)*modmat;
		line[3] = (gCenter + d)*modmat;
		line[4] = line[0];
		box.Init();
		box += line[0];
		box += line[1];
		box += line[2];
		box += line[3];
		}

	}

void UnwrapMod::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{	
	Matrix3 modmat, ntm = node->GetObjectTM(t);	
	modmat = CompMatrix(t,mc,&ntm);
	cb->Center(modmat.GetTrans(),0);	
	}

void UnwrapMod::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	Matrix3 ntm = node->GetObjectTM(t), modmat;
	modmat = CompMatrix(t,mc,&ntm);
	cb->TM(modmat,0);
	}



int UnwrapMod::HitTest (TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc) {
	Interval valid;
	int savedLimits, res = 0;
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	

// Face slection and Face Map selection

	if (ip && (ip->GetSubObjectLevel() == 1) )
		{
		MakeHitRegion(hr,type, crossing,4,p);
		gw->setHitRegion(&hr);
		Matrix3 mat = inode->GetObjectTM(t);
		gw->setTransform(mat);	
		gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
		if (type == HITTYPE_POINT)
			gw->setRndLimits(gw->getRndLimits() & GW_BACKCULL);
		else gw->setRndLimits(gw->getRndLimits() & ~GW_BACKCULL);
		gw->clearHitCode();

		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;
		BOOL polySelect = !(GetKeyState(VK_SHIFT)<0);

		if (add)
			ip->ReplacePrompt( GetString(IDS_PW_MOUSE_ADD));
		else if (sub)
			ip->ReplacePrompt( GetString(IDS_PW_MOUSE_SUBTRACT));
		else if (!polySelect)
			ip->ReplacePrompt( GetString(IDS_PW_MOUSE_SELECTTRI));
		else ip->ReplacePrompt( GetString(IDS_PW_MOUSE_SELECTFACE));



		if (objType == IS_MESH)
			{
			SubObjHitList hitList;
			MeshSubHitRec *rec;	
		
			if (!((MeshTopoData*)mc->localData)->GetMesh()) return 0;

			Mesh &mesh = *((MeshTopoData*)mc->localData)->GetMesh();
			mesh.faceSel = ((MeshTopoData*)mc->localData)->faceSel;
			res = mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr,
				flags|SUBHIT_FACES|SUBHIT_SELSOLID, hitList);

			rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				}

			}
		else if (objType == IS_PATCH)
			{
			SubPatchHitList hitList;
		
			if (!((MeshTopoData*)mc->localData)->GetPatch()) return 0;

			PatchMesh &patch = *((MeshTopoData*)mc->localData)->GetPatch();
			patch.patchSel = ((MeshTopoData*)mc->localData)->faceSel;

			res = patch.SubObjectHitTest(gw, gw->getMaterial(), &hr,
				flags|SUBHIT_PATCH_PATCHES|SUBHIT_SELSOLID, hitList);

			PatchSubHitRec *rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				}

			}

		}

	gw->setRndLimits(savedLimits);	
	return res;	
	}

void UnwrapMod::DeleteVertsFromFace(Tab<UVW_TVFaceClass> f)

{
Tab<int> fcount;
Tab<int> fcount_original;
fcount.SetCount(TVMaps.v.Count());
fcount_original.SetCount(TVMaps.v.Count());

for (int i = 0; i < fcount.Count(); i++)
	{
	fcount[i] = 0;
	fcount_original[i] = 0;
	}
	
for (i = 0; i < f.Count(); i++)
	{
	if (!(TVMaps.f[f[i].FaceIndex].flags & FLAG_DEAD))
		{
		int pcount =3;
		if (TVMaps.f[f[i].FaceIndex].flags & FLAG_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = TVMaps.f[f[i].FaceIndex].t[j];
			fcount[index] += 1;
			}
		}
	}

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		int pcount =3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = TVMaps.f[i].t[j];
			fcount_original[index] += 1;
			}

		}
	}
for (i = 0; i < f.Count(); i++)
	{
	if (!(TVMaps.f[f[i].FaceIndex].flags & FLAG_DEAD))
		{
		int pcount =3;
		if (TVMaps.f[f[i].FaceIndex].flags & FLAG_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = TVMaps.f[f[i].FaceIndex].t[j];
			if (fcount[index] == fcount_original[index])
				TVMaps.v[index].flags = FLAG_DEAD;

			}
		}
	}


}


void UnwrapMod::DeleteVertsFromFace(BitArray f)

{
Tab<int> fcount;
Tab<int> fcount_original;
fcount.SetCount(TVMaps.v.Count());
fcount_original.SetCount(TVMaps.v.Count());

for (int i = 0; i < fcount.Count(); i++)
	{
	fcount[i] = 0;
	fcount_original[i] = 0;
	}
	
for (i = 0; i < f.GetSize(); i++)
	{
	if (f[i] ==1)
		{
		if (!(TVMaps.f[i].flags & FLAG_DEAD))
			{
			for (int j = 0; j < 3; j++)
				{
				int index = TVMaps.f[i].t[j];
				fcount[index] += 1;
				}
			}
		}
	}

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		for (int j = 0; j < 3; j++)
			{
			int index = TVMaps.f[i].t[j];
			fcount_original[index] += 1;
			}

		}
	}
for (i = 0; i < f.GetSize(); i++)
	{
	if (f[i] ==1)
		{
		if (!(TVMaps.f[i].flags & FLAG_DEAD))
			{
			for (int j = 0; j < 3; j++)
				{
				int index = TVMaps.f[i].t[j];
				if (fcount[index] == fcount_original[index])
					TVMaps.v[index].flags = FLAG_DEAD;

				}
			}
		}
	}


}

void UnwrapMod::ApplyGizmo()
{

theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	


//add vertices to our internal vertex list filling in dead spots where appropriate
int ct = 0;  

//get align normal
//get fit data

PlanarTM.IdentityMatrix();
Interval v;
scaleControl->GetValue(0,&gScale,v);
PlanarTM.SetScale(gScale);
rotateControl->GetValue(0,&gRotate,v);
PlanarTM.RotateZ(gRotate);
offsetControl->GetValue(0,&gOffset,v);
PlanarTM.SetTrans(gOffset);

ComputeSelectedFaceData();

Matrix3 gtm;
UnwrapMatrixFromNormal(gNormal,gtm);

gtm = Inverse(gtm);

//nuke any verts previously held in the faces
//for (int i = 0; i < gfaces.Count(); i++)
//	{
	DeleteVertsFromFace(gfaces);
//	}

//unselect all verts
for (int j=0;j<TVMaps.v.Count();j++)
	{
	if (vsel[j])
		{
		vsel.Clear(j);
		}
	}

//build available list
Tab<int> alist;
alist.ZeroCount();

for (j=0;j<TVMaps.v.Count();j++)
	{
	if (TVMaps.v[j].flags & FLAG_DEAD)
//dead veretx found copy new vert in and note the place
		{
		alist.Append(1,&j,1);
		}
	}


for (int i = 0; i < gverts.d.Count(); i++)
	{
	BOOL found = FALSE;
	if (gverts.sel[i])
		{
		if (ct < alist.Count() )
			{
			j = alist[ct];
			TVMaps.v[j].flags = 0;
			TVMaps.v[j].influence = 0.0f;
			
			Point3 tp = gverts.d[i].p - gCenter;
			tp = tp * gtm;
			tp.x = tp.x/gXScale *.25f +0.5f;
			tp.y = tp.y/gYScale *.25f +0.5f;

			TVMaps.v[j].p = tp;
			int vcount = vsel.GetSize();
			vsel.Set(j);

			if (TVMaps.cont[j]) TVMaps.cont[j]->SetValue(0,&tp,CTRL_ABSOLUTE);
			gverts.d[i].newindex = j;
			ct++;

			}
		else
			{
			UVW_TVVertClass tempv;

			Point3 tp = gverts.d[i].p - gCenter;
			tp = tp * gtm;
			tp.x = tp.x/gXScale *.25f +0.5f;
			tp.y = tp.y/gYScale *.25f +0.5f;
			tempv.p = tp;

			tempv.flags = 0;
			tempv.influence = 0.0f;
			gverts.d[i].newindex = TVMaps.v.Count();
			TVMaps.v.Append(1,&tempv,1);

			vsel.SetSize(TVMaps.v.Count(), 1);
			vsel.Set(TVMaps.v.Count()-1);

			Control* c;
			c = NULL;
			TVMaps.cont.Append(1,&c,1);
			if (TVMaps.cont[TVMaps.v.Count()-1]) 
				TVMaps.cont[TVMaps.v.Count()-1]->SetValue(0,&TVMaps.v[TVMaps.v.Count()-1].p,CTRL_ABSOLUTE);
			}
		}

	}
//now copy our face data over
for (i = 0; i < gfaces.Count(); i++)
	{
//	if (fs[i])
		{
//		ActiveAddFaces.Append(1,&i,1);
		int ct = gfaces[i].FaceIndex;
		TVMaps.f[ct].flags = gfaces[i].flags;
		TVMaps.f[ct].flags |= FLAG_SELECTED;
		int pcount = 3;
		if (gfaces[i].flags & FLAG_QUAD) pcount = 4;
		for (int j = 0; j < pcount; j++)
			{
			int index = gfaces[i].t[j];
//find spot in our list
			TVMaps.f[ct].t[j] = gverts.d[index].newindex;

			}
		}
	}
//make a named selection for new face selection
TSTR namePre(GetString(IDS_PW_PLANARMAP));
TSTR name;
int ctn = 0;
while (1) {				
	char num[10];
	wsprintf(num,"%d",ctn);
	name = namePre+num;
	Tab<TSTR*> &setList = namedSel;

	BOOL unique = TRUE;
	for (int i=0; i<setList.Count(); i++) {
		if (name==*setList[i]) {
			unique = FALSE;
			break;
		}
	}
	ctn++;
	if (unique) break;

	}
NewSetFromCurSel(name); 
SetupNamedSelDropDown();

theHold.Accept(_T(GetString(IDS_PW_PLANARMAP)));
theHold.SuperAccept(_T(GetString(IDS_PW_PLANARMAP)));

//update our views to show new faces
InvalidateView();
}

void UnwrapMod::AddFaces(BitArray fs, Mesh *mesh)
{

theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	

ActiveAddFaces.ZeroCount();

Face *tf = mesh->faces;
Point3 *tp = mesh->verts;
VertexLookUpListClass v;

DeleteVertsFromFace(fs);


//isolate a vertex list of just the selected faces

for (int i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		for (int j = 0; j < 3; j++)
			{
			int index = tf[i].v[j];
			v.addPoint(index, tp[index]);
			}

		}
	}

//center the selection
Point3 center(.0f,0.0f,0.0f);
for (i = 0; i<v.d.Count(); i++)
	{
	center += v.d[i].p;
	}
center = center/(float)v.d.Count();
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p -= center;
	}

//get normal
Point3 zaxis = CrossProd(Normalize(v.d[1].p - v.d[0].p),Normalize(v.d[2].p - v.d[0].p));
zaxis = Normalize(zaxis);
//find first visible edge and use those cords as the xaxis
Point3 xaxis = Normalize(v.d[1].p - v.d[0].p);




Point3 yaxis = CrossProd(zaxis,xaxis);
yaxis = Normalize(yaxis);

Matrix3 tm(1);

tm.SetRow(0,xaxis);
tm.SetRow(1,yaxis);
tm.SetRow(2,zaxis);

for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p * Inverse(tm);
	}


mesh->buildBoundingBox();
Box3 b = mesh->getBoundingBox();


float d = 0.0f;
d = Length(b.pmax-b.pmin);
d = 1.0f/d;
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p*d + 0.5f;
	}

//add vertices to our internal vertex list filling in dead spots where appropriate
int ct = 0;  
for (i = 0; i < v.d.Count(); i++)
	{
	BOOL found = FALSE;
	for (int j=ct;j<TVMaps.v.Count();j++)
		{
		if (TVMaps.v[j].flags & FLAG_DEAD)
//dead veretx found copy new vert in and note the place
			{

			TVMaps.v[j].flags = 0;
			TVMaps.v[j].influence = 0.0f;
			TVMaps.v[j].p = v.d[i].p;
			if (TVMaps.cont[j]) TVMaps.cont[j]->SetValue(0,&TVMaps.v[j].p);
			v.d[i].newindex = j;
			ct = j;
			j = TVMaps.v.Count();
			found  = TRUE;
			}
		}
	if (!found)
		{
		UVW_TVVertClass tempv;

		tempv.p = v.d[i].p;
		tempv.flags = 0;
		tempv.influence = 0.0f;
		v.d[i].newindex = TVMaps.v.Count();
		TVMaps.v.Append(1,&tempv,1);

		vsel.SetSize(TVMaps.v.Count(), 1);
		Control* c;
		c = NULL;
		TVMaps.cont.Append(1,&c,1);
		if (TVMaps.cont[TVMaps.v.Count()-1]) TVMaps.cont[TVMaps.v.Count()-1]->SetValue(0,&TVMaps.v[TVMaps.v.Count()-1].p);
		}

	}

//now copy our face data over
for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		ActiveAddFaces.Append(1,&i,1);
		TVMaps.f[i].flags = 0;
		for (int j = 0; j < 3; j++)
			{
			int index = tf[i].v[j];
//find spot in our list
			for (int k = 0; k < v.d.Count();k++)
				{
				if (v.d[k].index == index)
					{
					TVMaps.f[i].t[j] = v.d[k].newindex;
					k = v.d.Count();
					}
				}
			}
		}
	}
theHold.Accept(_T(GetString(IDS_PW_PLANARMAP)));
theHold.SuperAccept(_T(GetString(IDS_PW_PLANARMAP)));

//update our views to show new faces
InvalidateView();
}

void UnwrapMod::AddFaces(BitArray fs, PatchMesh *patch)
{

theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	

ActiveAddFaces.ZeroCount();

Patch *tf = patch->patches;
PatchVert *tp = patch->verts;
VertexLookUpListClass v;

DeleteVertsFromFace(fs);


//isolate a vertex list of just the selected faces
for (int i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		int pcount =3;
		if (tf[i].type == PATCH_QUAD) pcount = 4;
		for (int j = 0; j < pcount; j++)
			{
			int index = tf[i].v[j];
			v.addPoint(index, tp[index].p);
			}

		}
	}

//center the selection
Point3 center(.0f,0.0f,0.0f);
for (i = 0; i<v.d.Count(); i++)
	{
	center += v.d[i].p;
	}
center = center/(float)v.d.Count();
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p -= center;
	}

//get normal
Point3 zaxis = CrossProd(Normalize(v.d[1].p - v.d[0].p),Normalize(v.d[2].p - v.d[0].p));
zaxis = Normalize(zaxis);
//find first visible edge and use those cords as the xaxis
Point3 xaxis = Normalize(v.d[1].p - v.d[0].p);




Point3 yaxis = CrossProd(zaxis,xaxis);
yaxis = Normalize(yaxis);

Matrix3 tm(1);

tm.SetRow(0,xaxis);
tm.SetRow(1,yaxis);
tm.SetRow(2,zaxis);

for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p * Inverse(tm);
	}


patch->buildBoundingBox();
Box3 b = patch->getBoundingBox();


float d = 0.0f;
d = Length(b.pmax-b.pmin);
d = 1.0f/d;
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p*d + 0.5f;
	}

//add vertices to our internal vertex list filling in dead spots where appropriate
int ct = 0;  
for (i = 0; i < v.d.Count(); i++)
	{
	BOOL found = FALSE;
	for (int j=ct;j<TVMaps.v.Count();j++)
		{
		if (TVMaps.v[j].flags & FLAG_DEAD)
//dead veretx found copy new vert in and note the place
			{

			TVMaps.v[j].flags = 0;
			TVMaps.v[j].influence = 0.0f;
			TVMaps.v[j].p = v.d[i].p;
			if (TVMaps.cont[j]) TVMaps.cont[j]->SetValue(0,&TVMaps.v[j].p);
			v.d[i].newindex = j;
			ct = j;
			j = TVMaps.v.Count();
			found  = TRUE;
			}
		}
	if (!found)
		{
		UVW_TVVertClass tempv;

		tempv.p = v.d[i].p;
		tempv.flags = 0;
		tempv.influence = 0.0f;
		v.d[i].newindex = TVMaps.v.Count();
		TVMaps.v.Append(1,&tempv,1);

		vsel.SetSize(TVMaps.v.Count(), 1);
		Control* c;
		c = NULL;
		TVMaps.cont.Append(1,&c,1);
		if (TVMaps.cont[TVMaps.v.Count()-1]) TVMaps.cont[TVMaps.v.Count()-1]->SetValue(0,&TVMaps.v[TVMaps.v.Count()-1].p);
		}

	}

//now copy our face data over
for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		ActiveAddFaces.Append(1,&i,1);

		if (tf[i].type == PATCH_QUAD)
			TVMaps.f[i].flags = FLAG_QUAD;
		else TVMaps.f[i].flags = 0;


		int pcount =3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = tf[i].v[j];
//find spot in our list
			for (int k = 0; k < v.d.Count();k++)
				{
				if (v.d[k].index == index)
					{
					TVMaps.f[i].t[j] = v.d[k].newindex;
					k = v.d.Count();
					}
				}
			}
		}
	}
theHold.Accept(_T(GetString(IDS_PW_PLANARMAP)));
theHold.SuperAccept(_T(GetString(IDS_PW_PLANARMAP)));

//update our views to show new faces
InvalidateView();
}


void UnwrapMod::ExpandFaces(BitArray fs, Mesh *mesh)
{
theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	


Face *tf = mesh->faces;
Point3 *tp = mesh->verts;
VertexLookUpListClass v;

DeleteVertsFromFace(fs);

//isolate a vertex list of just the selected faces
for (int i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		for (int j = 0; j < 3; j++)
			{
			int index = tf[i].v[j];
			v.addPoint(index, tp[index]);
			}

		}
	}

//center the selection
Point3 center(.0f,0.0f,0.0f);
for (i = 0; i<v.d.Count(); i++)
	{
	center += v.d[i].p;
	}
center = center/(float)v.d.Count();
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p -= center;
	}

//get normal
Point3 zaxis = CrossProd(Normalize(v.d[1].p - v.d[0].p),Normalize(v.d[2].p - v.d[0].p));
zaxis = Normalize(zaxis);
//find first visible edge and use those cords as the xaxis
Point3 xaxis = Normalize(v.d[1].p - v.d[0].p);




Point3 yaxis = CrossProd(zaxis,xaxis);
yaxis = Normalize(yaxis);

Matrix3 tm(1);

tm.SetRow(0,xaxis);
tm.SetRow(1,yaxis);
tm.SetRow(2,zaxis);

for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p * Inverse(tm);
	}


mesh->buildBoundingBox();
Box3 b = mesh->getBoundingBox();


float d = 0.0f;
d = Length(b.pmax-b.pmin);
d = 1.0f/d;
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p*d + 0.5f;
	}


//now we need to line them up
Point3 offset(0.0f,0.0f,0.0f);
float angle;
float scale;

Point3 pa,pb,ta,tb;

for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
//loop through each face edge
		for (int j = 0; j < 3; j++)
			{
			int a,b;
			a = tf[i].v[j];
			if (j==2)
				b = tf[i].v[0];
			else b = tf[i].v[j+1];
			for (int k = 0; k < ActiveAddFaces.Count(); k++)
				{
				int afaceid = ActiveAddFaces[k];
				for (int m = 0; m < 3; m++)
					{
					int target_a, target_b;
					target_b = tf[afaceid].v[m];
					if (m==2)
						target_a = tf[afaceid].v[0];
					else target_a = tf[afaceid].v[m+1];
//share a common mesh edge so share the TV Face edge also 
//move and rotat to align
					if ((a==target_a) && (b==target_b))
						{
						int index,index_target;
						if (m==2)
							index = TVMaps.f[afaceid].t[0];
						else index = TVMaps.f[afaceid].t[m+1];
						index_target = tf[i].v[j];

						for (int n =  0; n < v.d.Count(); n++)
							{
							if (v.d[n].index == index_target)
								{	
//get the transform offfset
								pa = TVMaps.v[index].p;
								ta = v.d[n].p;
								offset = TVMaps.v[index].p-v.d[n].p;
								n = v.d.Count();
								}
							}
						index = TVMaps.f[afaceid].t[m];
						if (j==2)
							index_target = tf[i].v[0];
						else index_target = tf[i].v[j+1];
						for (n =  0; n < v.d.Count(); n++)
							{
							if (v.d[n].index == index_target)
								{	
//get the transform offfset
								pb = TVMaps.v[index].p;
								tb = v.d[n].p;
								n = v.d.Count();
								}
							}
						Point3 pvec,tvec;
						pvec = pb-pa;
						tvec = tb-ta;
//get the scale offset
						scale = Length(pvec)/Length(tvec);
//get the angle offset
						angle = (float)acos(DotProd(Normalize(pvec),Normalize(tvec)));
						Point3 dirvec = CrossProd(Normalize(pvec),Normalize(tvec));
//						if ((pvec.x < 0.0) || (pvec.y < 0.0) )angle = -angle;
						if ( dirvec.z>0.0f )angle = -angle;

						m = 4;
						k = ActiveAddFaces.Count();
						j = 4;
						i = fs.GetSize();
						}

					}
			
				}


			}
		}
	}
//need to scale and rotate our new pieces
//offset *= scale;
Matrix3 rtm(1);
rtm.RotateZ(angle);

for (i = 0; i < v.d.Count(); i++)
	{
//zero out corner
	Point3 ZeroOffset;
	ZeroOffset = -ta;
	v.d[i].p += ZeroOffset;
//scale it 
	v.d[i].p *= scale;

//rotate it
	v.d[i].p = v.d[i].p * rtm;
//move it back
	v.d[i].p += -ZeroOffset;
//offset it
	v.d[i].p += offset;
	}
/*
for (i = 0; i < v.d.Count(); i++)
	{
	ZeroOffset = -pa;
	v.d[i].p += ZeroOffset;

//move it back
	v.d[i].p += -ZeroOffset;
	}
*/
//add vertices to our internal vertex list filling in dead spots where appropriate
int ct = 0;  
for (i = 0; i < v.d.Count(); i++)
	{
	BOOL found = FALSE;
	for (int j=ct;j<TVMaps.v.Count();j++)
		{
		if (TVMaps.v[j].flags & FLAG_DEAD)
//dead veretx found copy new vert in and note the place
			{

			TVMaps.v[j].flags = 0;
			TVMaps.v[j].influence = 0.0f;
			TVMaps.v[j].p = v.d[i].p;
			if (TVMaps.cont[j]) TVMaps.cont[j]->SetValue(0,&TVMaps.v[j].p);
			v.d[i].newindex = j;
			ct = j;
			j = TVMaps.v.Count();
			found  = TRUE;
			}
		}
	if (!found)
		{
		UVW_TVVertClass tempv;

		tempv.p = v.d[i].p;
		tempv.flags = 0;
		tempv.influence = 0.0f;
		v.d[i].newindex = TVMaps.v.Count();
		TVMaps.v.Append(1,&tempv,1);

		vsel.SetSize(TVMaps.v.Count(), 1);
		Control* c;
		c = NULL;
		TVMaps.cont.Append(1,&c,1);
		if (TVMaps.cont[TVMaps.v.Count()-1]) TVMaps.cont[TVMaps.v.Count()-1]->SetValue(0,&TVMaps.v[TVMaps.v.Count()-1].p);
		}

	}

//now copy our face data over
for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		ActiveAddFaces.Append(1,&i,1);
		TVMaps.f[i].flags = 0;
		for (int j = 0; j < 3; j++)
			{
			int index = tf[i].v[j];
//find spot in our list
			for (int k = 0; k < v.d.Count();k++)
				{
				if (v.d[k].index == index)
					{
					TVMaps.f[i].t[j] = v.d[k].newindex;
					k = v.d.Count();
					}
				}
			}
		}
	}

theHold.Accept(_T(GetString(IDS_PW_PLANARMAP)));
theHold.SuperAccept(_T(GetString(IDS_PW_PLANARMAP)));


//update our views to show new faces
InvalidateView();
}

void UnwrapMod::ExpandFaces(BitArray fs, PatchMesh *patch)
{
theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	


Patch *tf = patch->patches;
PatchVert *tp = patch->verts;
VertexLookUpListClass v;

DeleteVertsFromFace(fs);

//isolate a vertex list of just the selected faces
for (int i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		int pcount =3;
//		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		if (tf[i].type == PATCH_QUAD) pcount = 4;
		for (int j = 0; j < pcount; j++)
			{
			int index = tf[i].v[j];
			v.addPoint(index, tp[index].p);
			}

		}
	}

//center the selection
Point3 center(.0f,0.0f,0.0f);
for (i = 0; i<v.d.Count(); i++)
	{
	center += v.d[i].p;
	}
center = center/(float)v.d.Count();
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p -= center;
	}

//get normal
Point3 zaxis = CrossProd(Normalize(v.d[1].p - v.d[0].p),Normalize(v.d[2].p - v.d[0].p));
zaxis = Normalize(zaxis);
//find first visible edge and use those cords as the xaxis
Point3 xaxis = Normalize(v.d[1].p - v.d[0].p);




Point3 yaxis = CrossProd(zaxis,xaxis);
yaxis = Normalize(yaxis);

Matrix3 tm(1);

tm.SetRow(0,xaxis);
tm.SetRow(1,yaxis);
tm.SetRow(2,zaxis);

for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p * Inverse(tm);
	}


patch->buildBoundingBox();
Box3 b = patch->getBoundingBox();


float d = 0.0f;
d = Length(b.pmax-b.pmin);
d = 1.0f/d;
for (i = 0; i<v.d.Count(); i++)
	{
	v.d[i].p = v.d[i].p*d + 0.5f;
	}


//now we need to line them up
Point3 offset(0.0f,0.0f,0.0f);
float angle;
float scale;

Point3 pa,pb,ta,tb;

for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
//loop through each face edge
		int pcount =3;
//		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		if (tf[i].type == PATCH_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int a,b;
			a = tf[i].v[j];
			if (j==(pcount-1))
				b = tf[i].v[0];
			else b = tf[i].v[j+1];
			for (int k = 0; k < ActiveAddFaces.Count(); k++)
				{
				int afaceid = ActiveAddFaces[k];
				int mpcount =3;
//				if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
				if (tf[afaceid].type == PATCH_QUAD) mpcount = 4;

				for (int m = 0; m < mpcount; m++)
					{
					int target_a, target_b;
					target_b = tf[afaceid].v[m];
					if (m==(mpcount-1))
						target_a = tf[afaceid].v[0];
					else target_a = tf[afaceid].v[m+1];
//share a common mesh edge so share the TV Face edge also 
//move and rotat to align
					if ((a==target_a) && (b==target_b))
						{
						int index,index_target;
						if (m==(mpcount-1))
							index = TVMaps.f[afaceid].t[0];
						else index = TVMaps.f[afaceid].t[m+1];
						index_target = tf[i].v[j];

						for (int n =  0; n < v.d.Count(); n++)
							{
							if (v.d[n].index == index_target)
								{	
//get the transform offfset
								pa = TVMaps.v[index].p;
								ta = v.d[n].p;
								offset = TVMaps.v[index].p-v.d[n].p;
								n = v.d.Count();
								}
							}
						index = TVMaps.f[afaceid].t[m];
						if (j==(pcount-1))
							index_target = tf[i].v[0];
						else index_target = tf[i].v[j+1];
						for (n =  0; n < v.d.Count(); n++)
							{
							if (v.d[n].index == index_target)
								{	
//get the transform offfset
								pb = TVMaps.v[index].p;
								tb = v.d[n].p;
								n = v.d.Count();
								}
							}
						Point3 pvec,tvec;
						pvec = pb-pa;
						tvec = tb-ta;
//get the scale offset
						scale = Length(pvec)/Length(tvec);
//get the angle offset
						angle = (float)acos(DotProd(Normalize(pvec),Normalize(tvec)));
						Point3 dirvec = CrossProd(Normalize(pvec),Normalize(tvec));
//						if ((pvec.x < 0.0) || (pvec.y < 0.0) )angle = -angle;
						if ( dirvec.z>0.0f )angle = -angle;

						m = 5;
						k = ActiveAddFaces.Count();
						j = 5;
						i = fs.GetSize();
						}

					}
			
				}


			}
		}
	}
//need to scale and rotate our new pieces
//offset *= scale;
Matrix3 rtm(1);
rtm.RotateZ(angle);

for (i = 0; i < v.d.Count(); i++)
	{
//zero out corner
	Point3 ZeroOffset;
	ZeroOffset = -ta;
	v.d[i].p += ZeroOffset;
//scale it 
	v.d[i].p *= scale;

//rotate it
	v.d[i].p = v.d[i].p * rtm;
//move it back
	v.d[i].p += -ZeroOffset;
//offset it
	v.d[i].p += offset;
	}
/*
for (i = 0; i < v.d.Count(); i++)
	{
	ZeroOffset = -pa;
	v.d[i].p += ZeroOffset;

//move it back
	v.d[i].p += -ZeroOffset;
	}
*/
//add vertices to our internal vertex list filling in dead spots where appropriate
int ct = 0;  
for (i = 0; i < v.d.Count(); i++)
	{
	BOOL found = FALSE;
	for (int j=ct;j<TVMaps.v.Count();j++)
		{
		if (TVMaps.v[j].flags & FLAG_DEAD)
//dead veretx found copy new vert in and note the place
			{

			TVMaps.v[j].flags = 0;
			TVMaps.v[j].influence = 0.0f;
			TVMaps.v[j].p = v.d[i].p;
			if (TVMaps.cont[j]) TVMaps.cont[j]->SetValue(0,&TVMaps.v[j].p);
			v.d[i].newindex = j;
			ct = j;
			j = TVMaps.v.Count();
			found  = TRUE;
			}
		}
	if (!found)
		{
		UVW_TVVertClass tempv;

		tempv.p = v.d[i].p;
		tempv.flags = 0;
		tempv.influence = 0;
		v.d[i].newindex = TVMaps.v.Count();
		TVMaps.v.Append(1,&tempv,1);

		vsel.SetSize(TVMaps.v.Count(), 1);
		Control* c;
		c = NULL;
		TVMaps.cont.Append(1,&c,1);
		if (TVMaps.cont[TVMaps.v.Count()-1]) TVMaps.cont[TVMaps.v.Count()-1]->SetValue(0,&TVMaps.v[TVMaps.v.Count()-1].p);
		}

	}

//now copy our face data over
for (i = 0; i < fs.GetSize(); i++)
	{
	if (fs[i])
		{
		ActiveAddFaces.Append(1,&i,1);

		if (tf[i].type == PATCH_QUAD)
			TVMaps.f[i].flags = FLAG_QUAD;
		else TVMaps.f[i].flags = 0;

//		TVMaps.f[i].flags = 0;

		int pcount =3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = tf[i].v[j];
//find spot in our list
			for (int k = 0; k < v.d.Count();k++)
				{
				if (v.d[k].index == index)
					{
					TVMaps.f[i].t[j] = v.d[k].newindex;
					k = v.d.Count();
					}
				}
			}
		}
	}

theHold.Accept(_T(GetString(IDS_PW_PLANARMAP)));
theHold.SuperAccept(_T(GetString(IDS_PW_PLANARMAP)));


//update our views to show new faces
InvalidateView();
}

void UnwrapMod::UpdateFaceSelection(BitArray f)
{
int ct = 0;
for (int i = 0; i < TVMaps.f.Count();i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		if (f[i] == 1)
			{
			TVMaps.f[i].flags |= FLAG_SELECTED;
			ct++;
			}
		else TVMaps.f[i].flags &= (~FLAG_SELECTED);
		}
	}
//if (ct > 0) AlignMap();

}

int UnwrapMod::IsSelected(int index)
{
if (index < isSelected.GetSize())
	return isSelected[index];
else return 0;
int sel = 0;
for (int i = 0; i < TVMaps.f.Count();i++)
	{
	int pcount = 3;
	if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
	for (int j = 0;  j < pcount;j++)
		{
		if ( (TVMaps.f[i].t[j] == index) && ((TVMaps.f[i].flags & FLAG_SELECTED)))
			{
			sel = 1;
			return sel;
			}
		}
	}
return sel;
}

int UnwrapMod::IsSelectedSetup()
{
int sel = 0;
isSelected.SetSize(TVMaps.v.Count());
isSelected.ClearAll();

for (int i = 0; i < TVMaps.f.Count();i++)
	{
	int pcount = 3;
	if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
	for (int j = 0;  j < pcount;j++)
		{
		if ( (TVMaps.f[i].flags & FLAG_SELECTED))
			{
			isSelected.Set(TVMaps.f[i].t[j]);
			}
		}
	}
return 1;
}


void UnwrapMod::ExpandSelection(int dir)

{
Tab<int>SelectMe;


/*
DebugPrint("vsel sixe %d number set %d\n",vsel.GetSize(),vsel.NumberSet());
for (int i = 0; i < vsel.GetSize();i++)
	{
	DebugPrint("%d",vsel[i]);

	}
DebugPrint("\n");
*/

for (int i = 0; i < TVMaps.v.Count();i++)
	{
	if (vsel[i]==dir)
		{
//if face is selected select me
		for (int j = 0; j < TVMaps.f.Count();j++)
			{
			if (!(TVMaps.f[j].flags & FLAG_DEAD))
				{
				int pcount = 3;
				if (TVMaps.f[j].flags & FLAG_QUAD) pcount = 4;
				int match = 0;
				for (int k = 0; k < pcount; k++)
					{
					int index = TVMaps.f[j].t[k];
					if (index == i)
						match = 1;
					}
				if (match)
					{
					for (k = 0; k < pcount; k++)
						{
						int index = TVMaps.f[j].t[k];
						if (vsel[index]!=dir)
							{
							SelectMe.Append(1,&i,1);
							k = 5;
							j = TVMaps.f.Count();
							}
						}
					}
				}
			}
		}
	}
for (i = 0; i < SelectMe.Count(); i++)
	{
	if (dir == 0)
		vsel.Set(SelectMe[i],1);
	else vsel.Set(SelectMe[i],0);
	}

}

void UnwrapMod::SelectSubComponent (HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert) {

MeshTopoData *d = NULL, *od = NULL;

BitArray set;
//BitArray tempset;
AdjFaceList *al = NULL;
facehit.ZeroCount();
BOOL add = GetKeyState(VK_CONTROL)<0;
BOOL sub = GetKeyState(VK_MENU)<0;
BOOL polySelect = !(GetKeyState(VK_SHIFT)<0);

ip->ClearCurNamedSelSet();

if ( (ip && (ip->GetSubObjectLevel() == 1)  ))
	{
	if (objType == IS_MESH)
		{
		Mesh &mesh = *(((MeshTopoData*)hitRec->modContext->localData)->GetMesh());
		set.SetSize(mesh.getNumFaces());


		AdjFaceList *al = NULL;
		if (polySelect)
			al = BuildAdjFaceList(mesh);

		if (theHold.Holding()) theHold.Put (new UnwrapSelRestore (this,(MeshTopoData*)hitRec->modContext->localData));

		while (hitRec) {					
			d = (MeshTopoData*)hitRec->modContext->localData;					
//			if (set.GetSize()!=d->faceSel.GetSize()) d->faceSel.SetSize(set.GetSize(),TRUE);
			set.ClearAll();
			if (polySelect)
				{
//6-29--99 watje
				if ((hitRec->hitInfo < mesh.numFaces) &&
				    (d->faceSel.GetSize() == set.GetSize()))
					{

					mesh.PolyFromFace (hitRec->hitInfo, set, 45.0, FALSE, al);
					if (invert) d->faceSel ^= set;
					else if (selected) d->faceSel |= set;
					else d->faceSel &= ~set;
					}


				}
			else
				{
				BOOL state = selected;
//6-29--99 watje
				if (hitRec->hitInfo < d->faceSel.GetSize())
					{
					if (invert) state = !d->faceSel[hitRec->hitInfo];
					if (state) d->faceSel.Set(hitRec->hitInfo);
					else       d->faceSel.Clear(hitRec->hitInfo);
					}
				}
			if (!all) break;

			hitRec = hitRec->Next();
		}
		UpdateFaceSelection(d->faceSel);
		if (al) delete al;
		}
	else if (objType == IS_PATCH)
		{
		if (theHold.Holding()) theHold.Put (new UnwrapSelRestore (this,(MeshTopoData*)hitRec->modContext->localData));

		while (hitRec) {
			PatchMesh &patch = *(((MeshTopoData*)hitRec->modContext->localData)->GetPatch());
//			if (patch.numPatches!=d->faceSel.GetSize()) d->faceSel.SetSize(patch.numPatches,TRUE);
			od = d;
			d  = (MeshTopoData*)hitRec->modContext->localData;

	// Build it the first time
			BOOL state = selected;
//6-29--99 watje
			if (hitRec->hitInfo < d->faceSel.GetSize())
				{

				if (invert) state = !d->faceSel[hitRec->hitInfo];
				if (state) d->faceSel.Set(hitRec->hitInfo);
				else       d->faceSel.Clear(hitRec->hitInfo);
				}
			if (!all) break;
			hitRec = hitRec->Next();

			}
		UpdateFaceSelection(d->faceSel);
		}
	if (filterSelectedFaces == 1) InvalidateView();

	}

ComputeSelectedFaceData();
theHold.Accept (GetString (IDS_DS_SELECT));

NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
SetNumSelLabel();
}

void UnwrapMod::ClearSelection(int selLevel)
{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	MeshTopoData *d;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshTopoData*)list[i]->localData;
		if (!d) continue;
		if (theHold.Holding()) theHold.Put (new UnwrapSelRestore (this,d));

		switch (selLevel) {
			case 1:
				d->faceSel.ClearAll();
				UpdateFaceSelection(d->faceSel);
				break;
			}
		}
	theHold.Accept (GetString (IDS_DS_SELECT));

	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	SetNumSelLabel();
//update our views to show new faces
	InvalidateView();


}



void UnwrapMod::ActivateSubobjSel(int level, XFormModes& modes) {
	// Fill in modes with our sub-object modes

	if (level ==0)
		{
//		iAlignButton->Enable(FALSE);
		iApplyButton->Enable(FALSE);
//		iFitButton->Enable(FALSE);
		}
	if (level==1) 
		{
//		AlignMap();
		SetupNamedSelDropDown();
//face select
//		iAlignButton->Enable(FALSE);
		iApplyButton->Enable(TRUE);
//		iFitButton->Enable(FALSE);
		modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
		}
/*
	else if (level==2) 
		{
//face map mode
//		iAlignButton->Enable(FALSE);
		iApplyButton->Enable(FALSE);
//		iFitButton->Enable(FALSE);
		modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
		}
	else if (level==3) 
		{
//face map mode
//		iAlignButton->Enable(TRUE);
		iApplyButton->Enable(TRUE);
//		iFitButton->Enable(TRUE);
		modes = XFormModes(gmoveMode,grotMode,gnuscaleMode,guscaleMode,gsquashMode,NULL);
		type = MAP_PLANAR;
		}
*/
	SetNumSelLabel ();

	InvalidateView();
	NotifyDependents(FOREVER,PART_DISPLAY,REFMSG_CHANGE);

}

class MyEnumProc : public DependentEnumProc 
	{
      public :
      virtual int proc(ReferenceMaker *rmaker); 
      INodeTab Nodes;              
	};

int MyEnumProc::proc(ReferenceMaker *rmaker) 
	{ 
	if (rmaker->SuperClassID()==BASENODE_CLASS_ID)    
			{
            Nodes.Append(1, (INode **)&rmaker);                 
			}
     return 0;              
	}

void UnwrapMod::LoadMaterials()
{
//no entries on our material list lets add some based on the current assigned material in MtlBase
int ct =0;

MyEnumProc dep;              
EnumDependents(&dep);
INode *SelfNode = dep.Nodes[0];
BaseMtl = SelfNode->GetMtl();

//if ((map[0] == NULL) && (BaseMtl != NULL))
if ((BaseMtl != NULL))
	{
//check is standard material
//	if (BaseMtl->ClassID() == DMTL_CLASS_ID)
	if (BaseMtl->ClassID() == Class_ID(DMTL_CLASS_ID,0))
		{
		StdMat *stdmat = (StdMat*) BaseMtl;
		int tex_count = stdmat->NumSubTexmaps();
		for (int i = 0; i < tex_count; i++)
			{
			Texmap *tmap;
			MtlBase *m;
			tmap = stdmat->GetSubTexmap(i);
			m = (MtlBase*) tmap;
//			m = (MtlBase*) stdmat->GetSubMtl(i);
			if (m != NULL)
				{
//add to our list
				AddMaterial(m);
				ct++;
				if (ct ==10) return;
				}
			}
		}
//check multi sub material
	else if (BaseMtl->IsMultiMtl())
		{	
		int mat_count = 0;
		mat_count = BaseMtl->NumSubMtls();
		for (int j=0; j < mat_count; j++)
			{
			
			MtlBase *m;
			m = (MtlBase*) BaseMtl->GetSubMtl(j);
			if (m != NULL)
				{
				int tex_count = m->NumSubTexmaps();
				for (int i = 0; i < tex_count; i++)
					{

					Texmap *tmap;
					MtlBase *mt;
					tmap = m->GetSubTexmap(i);
					mt = (MtlBase*) tmap;
					if (mt != NULL)
						{
//add to our list
						AddMaterial(mt);
						ct++;
						if (ct ==10) return;

						}
					}
				}
			}

		}

	}
CurrentMap = 0;
SetupImage();
SendMessage(hTextures, CB_SETCURSEL, CurrentMap, 0 );

}

void UnwrapMod::BuildMatIDList()
{

filterMatID.ZeroCount();
for (int i = 0; i < TVMaps.f.Count(); i++)
	{
	int found = 0;
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		for (int j = 0; j < filterMatID.Count(); j++)
			{
			if (filterMatID[j] == TVMaps.f[i].MatID) 
				{
				found = 1;
				j = filterMatID.Count();
				}
			}
		}
	else found = 1;
	if (found == 0)
		filterMatID.Append(1,&TVMaps.f[i].MatID,1);
	}

vertMatIDList.SetSize(TVMaps.v.Count());
vertMatIDList.SetAll();
matid = -1;
}


void UnwrapMod::ComputeFalloff(float &u, int ftype)

{
if (u<= 0.0f) u = 0.0f;
else if (u>= 1.0f) u = 1.0f;
else switch (ftype)
	{
	case (3) : u = u*u*u; break;
//	case (BONE_FALLOFF_X2_FLAG) : u = u*u; break;
	case (0) : u = u; break;
	case (1) : u = 1.0f-((float)cos(u*PI) + 1.0f)*0.5f; break;
//	case (BONE_FALLOFF_2X_FLAG) : u = (float) sqrt(u); break;
	case (2) : u = (float) pow(u,0.3); break;

	}

}

void UnwrapMod::RebuildDistCache()
{
float str = iStr->GetFVal();
float sstr = str*str;
if (str == 0.0f)
	{
	for (int i = 0; i<TVMaps.v.Count(); i++)
		TVMaps.v[i].influence = 0.0f;
	return;
	}
Tab<int> Selected;

for (int i = 0; i<TVMaps.v.Count(); i++)
	{
	if (vsel[i])
		Selected.Append(1,&i,1);
	}
if (falloffSpace == 0)
	BuildObjectPoints();
for (i = 0; i<TVMaps.v.Count(); i++)
	{
	if (vsel[i] == 0)
		{
		float closest_dist = BIGFLOAT;
		for (int j= 0; j < Selected.Count();j++)
			{
//use XY	Z space values
			if (falloffSpace == 0)
				{
				Point3 sp = GetObjectPoint(ip->GetTime(),i);
				Point3 rp = GetObjectPoint(ip->GetTime(),Selected[j]);
				float d = LengthSquared(sp-rp);
				if (d < closest_dist) closest_dist = d;

				}
			else
//use UVW space values
				{
				Point3 sp = GetPoint(ip->GetTime(),Selected[j]);
				Point3 rp = GetPoint(ip->GetTime(),i);
				float d = LengthSquared(sp-rp);
				if (d < closest_dist) closest_dist = d;
				}
			}
		if (closest_dist < sstr)
			{
			closest_dist = (float) sqrt(closest_dist);
			TVMaps.v[i].influence = 1.0f - closest_dist/str;
			ComputeFalloff(TVMaps.v[i].influence,falloff);
			}
		else TVMaps.v[i].influence = 0.0f;
		}
	}	
}

void UnwrapMod::BuildInitialMapping(Mesh *msh)
{
//build bounding box
Box3 bbox;
bbox.Init();
//normalize the length width height
for (int i = 0; i < TVMaps.f.Count(); i++)
	{
	int pcount = 3;
	if (TVMaps.f[i].flags & FLAG_QUAD)
		pcount = 4;
	for (int j = 0; j < pcount; j++)
		{
		bbox += TVMaps.f[i].pt[j];
		}

	}
Tab<int> indexList;

indexList.SetCount(TVMaps.f.Count() *4);
BitArray usedIndex;
usedIndex.SetSize(TVMaps.f.Count() *4);
usedIndex.ClearAll();

for (i = 0; i < TVMaps.f.Count()*4; i++)
	indexList[i] = -1;

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD)
			pcount = 4;
		for (int j = 0; j < pcount; j++)
			{
			usedIndex.Set(msh->faces[i].v[j]);
			}
		}

	}

int ct = 0;
for (i = 0; i < usedIndex.GetSize(); i++)
	{
	if (usedIndex[i])
		indexList[i] = ct++;

	}

TVMaps.v.SetCount(usedIndex.NumberSet());
TVMaps.cont.SetCount(usedIndex.NumberSet());
vsel.SetSize(usedIndex.NumberSet());

//watje 10-19-99 bug 213437  to prevent a divide by 0 which gives you a huge u,v, or w value
if (bbox.Width().x == 0.0f) bbox += Point3(0.5f,0.0f,0.0f);
if (bbox.Width().y == 0.0f) bbox += Point3(0.0f,0.5f,0.0f);
if (bbox.Width().z == 0.0f) bbox += Point3(0.0f,0.0f,0.5f);

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD)
			pcount = 4;
		TVMaps.f[i].flags &= ~FLAG_DEAD;
		for (int j = 0; j < pcount; j++)
			{
			int index;
			int a = msh->faces[i].v[j];
			index = indexList[a];
			TVMaps.f[i].t[j] = index;
			TVMaps.v[index].p.x =  TVMaps.f[i].pt[j].x/bbox.Width().x + 0.5f;
			TVMaps.v[index].p.y =  TVMaps.f[i].pt[j].y/bbox.Width().y + 0.5f;
			TVMaps.v[index].p.z =  TVMaps.f[i].pt[j].z/bbox.Width().z + 0.5f;
			TVMaps.v[index].influence =  0.f;
			TVMaps.v[index].flags =  0.f;
			TVMaps.cont[index] = NULL;
		
			}

		}
	}

//PlugControllers();
//NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);

//if (ip) ip->RedrawViews(ip->GetTime());
}



void UnwrapMod::BuildInitialMapping(PatchMesh *msh)
{
//build bounding box
Box3 bbox;
bbox.Init();
//normalize the length width height
for (int i = 0; i < TVMaps.f.Count(); i++)
	{
	int pcount = 3;
	if (TVMaps.f[i].flags & FLAG_QUAD)
		pcount = 4;
	for (int j = 0; j < pcount; j++)
		{
		bbox += TVMaps.f[i].pt[j];
		}

	}
Tab<int> indexList;

indexList.SetCount(TVMaps.f.Count() *4);
BitArray usedIndex;
usedIndex.SetSize(TVMaps.f.Count() *4);
usedIndex.ClearAll();

for (i = 0; i < TVMaps.f.Count()*4; i++)
	indexList[i] = -1;

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD)
			pcount = 4;
		for (int j = 0; j < pcount; j++)
			{
//		usedIndex.Set(TVMaps.f[i].t[j]);
			usedIndex.Set(msh->patches[i].v[j]);
			}
		}

	}

int ct = 0;
for (i = 0; i < usedIndex.GetSize(); i++)
	{
	if (usedIndex[i])
		indexList[i] = ct++;

	}

TVMaps.v.SetCount(usedIndex.NumberSet());
TVMaps.cont.SetCount(usedIndex.NumberSet());
vsel.SetSize(usedIndex.NumberSet());

//watje 10-19-99 bug 213437  to prevent a divide by 0 which gives you a huge u,v, or w value
if (bbox.Width().x == 0.0f) bbox += Point3(0.5f,0.0f,0.0f);
if (bbox.Width().y == 0.0f) bbox += Point3(0.0f,0.5f,0.0f);
if (bbox.Width().z == 0.0f) bbox += Point3(0.0f,0.0f,0.5f);

for (i = 0; i < TVMaps.f.Count(); i++)
	{
	if (!(TVMaps.f[i].flags & FLAG_DEAD))
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD)
			pcount = 4;
		TVMaps.f[i].flags &= ~FLAG_DEAD;
		for (int j = 0; j < pcount; j++)
			{
			int index;
			int a = msh->patches[i].v[j];
			index = indexList[a];
			TVMaps.f[i].t[j] = index;
			TVMaps.v[index].p.x =  TVMaps.f[i].pt[j].x/bbox.Width().x + 0.5f;
			TVMaps.v[index].p.y =  TVMaps.f[i].pt[j].y/bbox.Width().y + 0.5f;
			TVMaps.v[index].p.z =  TVMaps.f[i].pt[j].z/bbox.Width().z + 0.5f;;
			TVMaps.v[index].influence =  0.f;
			TVMaps.v[index].flags =  0.f;
			TVMaps.cont[index] = NULL;
		
			}
		}

	}

//PlugControllers();
//NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
//if (ip) ip->RedrawViews(ip->GetTime());


}

void UnwrapMod::RemoveDeadVerts(PatchMesh *mesh, int channel)
{
	Tab<Point3> vertList;
	Tab<int> idList;
//copy over vertlist

	int ct = mesh->getNumMapVerts(channel);
	vertList.SetCount(ct);
	Point3 *tVerts = mesh->mapVerts(channel);
	for (int i = 0; i < ct; i++)
		vertList[i] = tVerts[i];

	BitArray usedList;
	usedList.SetSize(ct);
	TVPatch *tvFace = NULL;
	if (!mesh->getMapSupport(channel))
		{
		return;
		}

	tvFace = mesh->tvPatches[channel];
	if (tvFace == NULL) return;

	for (i =0; i < mesh->numPatches; i++)
		{
		int pcount = 3;
		if (mesh->patches[i].type == PATCH_QUAD) pcount = 4;

		for (int j = 0; j < pcount; j++)
			{
			int index = tvFace[i].tv[j];
			usedList.Set(index);
			}
		}
	mesh->setNumMapVerts (channel,usedList.NumberSet(),TRUE);

	int current = 0;
	tVerts = mesh->mapVerts(channel);

	for (i = 0; i < ct; i++)
		{
		if (usedList[i])
			{
			tVerts[current] = vertList[i];
//now fix up faces
			for (int j = 0; j < mesh->numPatches; j++)
				{
				int pcount = 3;
				if (mesh->patches[j].type == PATCH_QUAD) pcount = 4;

				for (int k = 0; k < pcount; k++)
					{
					int index = tvFace[j].tv[k];
					if (index == i)
						{
						tvFace[j].tv[k] = current;
						}
					}

				}
			current++;
			}
		}




}



void UnwrapMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{


	MyEnumProc dep;              
	EnumDependents(&dep);
/*
	if ((dep.Nodes.Count() > 1) && (firstPass))
		{

		ip->EnableSubObjectSelection(FALSE);
		iApplyButton->Enable(FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_RADIO1),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_RADIO2),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_RADIO3),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_RADIO4),FALSE);

		EnableWindow(GetDlgItem(hParams,IDC_UNWRAP_EDIT),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_UNWRAP_SAVE),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_UNWRAP_LOAD),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_UNWRAP_RESET),FALSE);
		iMapID->Enable(FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_MAP_CHAN1),FALSE);
		EnableWindow(GetDlgItem(hParams,IDC_MAP_CHAN2),FALSE);


//do unhide here
		firstPass = FALSE;
		instanced = TRUE;
		TSTR buf2 = GetString(IDS_PW_ERROR);
		TSTR buf1 = GetString(IDS_PW_INSTANCEERROR);
		MessageBox(GetCOREInterface()->GetMAXHWnd(),buf1,buf2,MB_OK|MB_TASKMODAL);
		}
	else instanced = FALSE;
	if (dep.Nodes.Count() > 1) 
		{
//		TVMaps.v.Count();
//		TVMaps.f.Count();
		return;
		}
*/

	// Prepare the controller and set up mats
	Object *obj = os->obj;

	if (oldDataPresent)
		{
		oldDataPresent = FALSE;
		Tab<UVW_TVFaceClass> tempStorage;
		tempStorage = TVMaps.f;
		TriObject *tobj = (TriObject*)os->obj;

		Mesh &mesh = tobj->GetMesh();
		if (mesh.selLevel==MESH_FACE) 
			{


			TVMaps.f.SetCount(tobj->GetMesh().getNumFaces());
			int current = 0;
			for (int i = 0; i < TVMaps.f.Count(); i++)
				{
				if (tobj->GetMesh().faceSel[i])
					{
					TVMaps.f[i] = tempStorage[current];
					current++;
					}
				else TVMaps.f[i].flags |= FLAG_DEAD;
				}	
			}
		}


	if ((!os->obj->IsSubClassOf(patchObjectClassID)) && (!os->obj->IsSubClassOf(triObjectClassID)))
		{
//neither patch or NURBS convert to a mesh
		if (os->obj->CanConvertToType(triObjectClassID))
			{
			TriObject *tri = (TriObject *) os->obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
			os->obj = tri;
			os->obj->UnlockObject();
			}
		
		}

//poll for material on mesh
	int CurrentChannel = 0;

	if (channel == 0)
		{
		CurrentChannel = 1;
//should be from scroller;

		}
	else if (channel == 1)
		{
		CurrentChannel = 0;
		}
	else CurrentChannel = channel;


	INode *SelfNode = dep.Nodes[0];
	BaseMtl = SelfNode->GetMtl();

	if ((os->obj->IsSubClassOf(patchObjectClassID)) &  (firstPass))
		{
		PatchObject *pobj = (PatchObject*)os->obj;
		if (ip) 
			{
			firstPass = FALSE;
			if (pobj->patch.selLevel==PATCH_PATCH ) 
				{	
				ip->EnableSubObjectSelection(FALSE);
				iApplyButton->Enable(FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO1),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO2),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO3),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO4),FALSE);
				}
			else
				{
				ip->EnableSubObjectSelection(TRUE);
				iApplyButton->Enable(TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO1),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO2),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO3),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO4),TRUE);

				}


			}

		}
	else if ((os->obj->IsSubClassOf(triObjectClassID)) && (firstPass))
		{
		TriObject *tobj = (TriObject*)os->obj;
		if (ip) 
			{
			firstPass = FALSE;
			if (tobj->GetMesh().selLevel==MESH_FACE) 
				{
				ip->EnableSubObjectSelection(FALSE);
				iApplyButton->Enable(FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO1),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO2),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO3),FALSE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO4),FALSE);

				}
			else
				{
				ip->EnableSubObjectSelection(TRUE);
				iApplyButton->Enable(TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO1),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO2),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO3),TRUE);
				EnableWindow(GetDlgItem(hParams,IDC_RADIO4),TRUE);

				}


			}
		}


	if (os->obj->IsSubClassOf(patchObjectClassID))
		{
		objType = IS_PATCH;
		PatchObject *pobj = (PatchObject*)os->obj;
		MeshTopoData *d  = (MeshTopoData*)mc.localData;
		if (!d) 
			{
			mc.localData = d = new MeshTopoData(pobj->patch);
			d->SetFaceSel(pobj->patch.patchSel, this, t);
			}
		if (((editMod==this) && (!d->GetPatch())) || (updateCache))
			{
			d->SetCache(pobj->patch);
			d->SetFaceSel(pobj->patch.patchSel, this, t);
			updateCache = FALSE;
			}

		
		BitArray faceSel = d->faceSel;
		faceSel.SetSize(pobj->patch.getNumPatches(),TRUE);
		if ( (ip && (ip->GetSubObjectLevel() > 0) ))
			{

			pobj->patch.patchSel = faceSel;
			if (showVerts)
				{
//select verts based on the current tverts;
				BitArray vertSel;
				vertSel.SetSize(pobj->patch.getNumVerts(),TRUE);
				vertSel.ClearAll();
				for(int sv = 0; sv < TVMaps.f.Count();sv++)
					{
					if (!(TVMaps.f[sv].flags & FLAG_DEAD))
						{

						int pcount = 3;
						if (TVMaps.f[sv].flags & FLAG_QUAD) pcount = 4;
						for (int j = 0; j < pcount ; j++)
							{
							int index = TVMaps.f[sv].t[j];
//6-29--99 watje
							if ((index < vsel.GetSize()) && (vsel[index] ==1) && (sv < pobj->patch.numPatches))
//							if (vsel[index] ==1)
								{
								int findex = pobj->patch.patches[sv].v[j];
//6-29--99 watje
								if ((findex < vertSel.GetSize()) && (findex >=0))
									vertSel.Set(findex,1);
								}
							}
						}
					}
				pobj->patch.vertSel = vertSel;

				pobj->patch.SetDispFlag(DISP_SELPATCHES|DISP_LATTICE|DISP_SELVERTS|DISP_VERTTICKS);
				}
			else pobj->patch.SetDispFlag(DISP_SELPATCHES|DISP_LATTICE);
			}

//		pobj->patch.SetDispFlag(DISP_SELPATCHES|DISP_LATTICE);

		
		if (!tmControl || (flags&CONTROL_OP) || (flags&CONTROL_INITPARAMS)) 
			InitControl(t);


//if planar mode build vert and face list
		if ( (ip && (ip->GetSubObjectLevel() == 1) ))
			{
//			Matrix3 tm;	
//			tm = Inverse(CompMatrix(t,&mc,NULL));
//copy verts		
//			gverts.d.ZeroCount();
			gfaces.ZeroCount();
			gverts.d.SetCount(faceSel.GetSize()*4);

			gverts.sel.SetSize(faceSel.GetSize()*4,1);
			gverts.sel.ClearAll();
		
			Patch *tf = pobj->patch.patches;
			PatchVert *tp = pobj->patch.verts;
//			VertexLookUpListClass v;

//isolate a vertex list of just the selected faces
			for (int i = 0; i < faceSel.GetSize(); i++)
				{
				if (faceSel[i])
					{
					int pcount = 3;
					if (tf[i].type == PATCH_QUAD) pcount = 4;
					for (int j = 0; j < pcount; j++)
						{
						int index = tf[i].v[j];
//						gverts.addPoint(index, tp[index].p*tm);
						gverts.addPoint(index, tp[index].p);
						}

					}
				}
//build new tv faces
			gfaces.SetCount(faceSel. NumberSet());
			int ct = 0;
			for (i = 0; i < faceSel.GetSize(); i++)
				{
				if (faceSel[i])
					{
					UVW_TVFaceClass t;
					t.FaceIndex = i;
					int pcount = 3;
					t.flags = 0;
					if (tf[i].type == PATCH_QUAD) 
						{
						pcount = 4;
						t.flags = FLAG_QUAD;
						}

					for (int j = 0; j < pcount; j++)
						{
//find indes in our vert array
						t.t[j] = (int)tf[i].v[j];

//						for (int k = 0; k < gverts.d.Count(); k++)
//							{
//							if (gverts.d[k].index == (int)tf[i].v[j])
//								{
//								t.t[j] = k;
//								}
//							}

						}
					gfaces[ct++] = t;


					}
				}
	
			}
		}
	else if (os->obj->IsSubClassOf(triObjectClassID))
		{
		objType = IS_MESH;
		TriObject *tobj = (TriObject*)os->obj;
		MeshTopoData *d  = (MeshTopoData*)mc.localData;
		if (!d) 
			{
			mc.localData = d = new MeshTopoData(tobj->GetMesh());
			d->SetFaceSel(tobj->GetMesh().faceSel, this, t);
			}
		if ( ((editMod==this) && (!d->GetMesh())) || (updateCache))
			{
			d->SetCache(tobj->GetMesh());
			d->SetFaceSel(tobj->GetMesh().faceSel, this, t);
			updateCache = FALSE;
			}

		BitArray faceSel = d->faceSel;


		faceSel.SetSize(tobj->GetMesh().getNumFaces(),TRUE);

		if ( (ip && (ip->GetSubObjectLevel() > 0) ))
			{

			tobj->GetMesh().faceSel = faceSel;

			if (showVerts)
				{
//select verts based on the current tverts;
				BitArray vertSel;
				vertSel.SetSize(tobj->GetMesh().getNumVerts(),TRUE);
				vertSel.ClearAll();
				for(int sv = 0; sv < TVMaps.f.Count();sv++)
					{
					if (!(TVMaps.f[sv].flags & FLAG_DEAD))
						{
						int pcount = 3;
						if (TVMaps.f[sv].flags & FLAG_QUAD) pcount = 4;
						for (int j = 0; j < pcount ; j++)
							{
							int index = TVMaps.f[sv].t[j];
//6-29--99 watje
							if ((index < vsel.GetSize()) && (vsel[index] ==1) && (sv<tobj->GetMesh().numFaces))
//							if (vsel[index] ==1)
								{
								int findex = tobj->GetMesh().faces[sv].v[j];
//6-29--99 watje
								if ((findex < vertSel.GetSize()) && (findex >=0))
									vertSel.Set(findex,1);
								}
							}
						}
					}
				tobj->GetMesh().vertSel = vertSel;
				tobj->GetMesh().SetDispFlag(DISP_SELFACES|DISP_VERTTICKS|DISP_SELVERTS);
//done++;
				}
			else
				{
				tobj->GetMesh().SetDispFlag(DISP_SELFACES);
//done++;
				}

			}

		
		if (!tmControl || (flags&CONTROL_OP) || (flags&CONTROL_INITPARAMS)) 
			InitControl(t);


//if planar mode build vert and face list

		if ( (ip && (ip->GetSubObjectLevel() == 1) ))
			{
			gfaces.ZeroCount();
			gverts.d.SetCount(faceSel.GetSize()*4);

			gverts.sel.SetSize(faceSel.GetSize()*4,1);
			gverts.sel.ClearAll();

			Face *tf = tobj->GetMesh().faces;
			Point3 *tp = tobj->GetMesh().verts;

//isolate a vertex list of just the selected faces
			for (int i = 0; i < faceSel.GetSize(); i++)
				{
				if (faceSel[i])
					{
					for (int j = 0; j < 3; j++)
						{
						int index = tf[i].v[j];
						gverts.addPoint(index, tp[index]);
						}

					}
				}
//build new tv faces
			gfaces.SetCount(faceSel. NumberSet());
			int ct = 0;
			for (i = 0; i < faceSel.GetSize(); i++)
				{
				if (faceSel[i])
					{
					UVW_TVFaceClass t;
					t.FaceIndex = i;
					t.flags = 0;

					for (int j = 0; j < 3; j++)
						{
//find indes in our vert array
						t.t[j] = (int)tf[i].v[j];


						}

					gfaces[ct++] = t;


					}
				}
	
			}
		}


	BOOL reset = FALSE;
	if (os->obj->IsSubClassOf(patchObjectClassID))
		{
// is whole mesh
		PatchObject *pobj = (PatchObject*)os->obj;
		PatchMesh &patch = pobj->patch;
		if (TVMaps.f.Count() != patch.getNumPatches())
			reset =TRUE;

		}
	else if (os->obj->IsSubClassOf(triObjectClassID)) 
		{
		TriObject *tobj = (TriObject*)os->obj;
				// Apply our mapping
		Mesh &mesh = tobj->GetMesh();
		if (TVMaps.f.Count() != mesh.getNumFaces())
			reset = TRUE;
	}


//check is TVMap == 0 then get data from mesh, patch, or nurbs and copy to our data
	if ((TVMaps.v.Count() == 0) || (reset))
		{
///is patch
		if (os->obj->IsSubClassOf(patchObjectClassID))
			{
// is whole mesh

			PatchObject *pobj = (PatchObject*)os->obj;
			// Apply our mapping
			PatchMesh &patch = pobj->patch;

					
			if ( (patch.selLevel==PATCH_PATCH) && (patch.patchSel.NumberSet() == 0) ) 
				{
				return;
				}
	

//loop through all maps
//get channel from mesh
			TVMaps.channel = CurrentChannel;
					
	
//get from mesh based on cahne
			Point3 *tVerts = NULL;
			TVPatch *tvFace = NULL;
			if (!patch.getMapSupport(CurrentChannel))
				{
				patch.setNumMaps(CurrentChannel+1);
				}

			tVerts = patch.tVerts[CurrentChannel];
			tvFace = patch.tvPatches[CurrentChannel];

			if (patch.selLevel!=PATCH_PATCH ) 
				{
//copy into our structs
				TVMaps.f.SetCount(patch.getNumPatches());
				TVMaps.v.SetCount(patch.getNumMapVerts(CurrentChannel));
				TVMaps.cont.SetCount(patch.getNumMapVerts(CurrentChannel));

				vsel.SetSize(patch.getNumMapVerts (CurrentChannel));


				for (int j=0; j<TVMaps.f.Count(); j++) 
					{
					TVMaps.f[j].flags = 0;
					if (tvFace == NULL)
						{
						TVMaps.f[j].t[0] = 0;
						TVMaps.f[j].t[1] = 0;
						TVMaps.f[j].t[2] = 0;
						TVMaps.f[j].t[3] = 0;
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = patch.getPatchMtlIndex(j);
						TVMaps.f[j].flags = 0;
						int pcount = 3;
						if (patch.patches[j].type == PATCH_QUAD) 
							{
							TVMaps.f[j].flags |= FLAG_QUAD;
							pcount = 4;
							}
								 
						for (int k = 0; k < pcount; k++)
							{
							int index = patch.patches[j].v[k];
							TVMaps.f[j].pt[k] = patch.verts[index].p;
							}

						}
					else
						{
						TVMaps.f[j].t[0] = tvFace[j].tv[0];
						TVMaps.f[j].t[1] = tvFace[j].tv[1];
						TVMaps.f[j].t[2] = tvFace[j].tv[2];
						TVMaps.f[j].t[3] = tvFace[j].tv[3];
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = patch.getPatchMtlIndex(j);

						int pcount = 3;

						if (patch.patches[j].type == PATCH_QUAD) 
							{
							TVMaps.f[j].flags = FLAG_QUAD;
							pcount = 4;
							}
						else TVMaps.f[j].flags = 0;

						if ((patch.selLevel==PATCH_PATCH ) && (patch.patchSel[j] == 0))
							TVMaps.f[j].flags |= FLAG_DEAD;

						for (int k = 0; k < pcount; k++)
							{
							int index = patch.patches[j].v[k];
							TVMaps.f[j].pt[k] = patch.verts[index].p;
							}



						}
					}
				for (    j=0; j<TVMaps.v.Count(); j++) 
					{
					TVMaps.cont[j] = NULL;
					TVMaps.v[j].flags = 0;
					if (tVerts)
						TVMaps.v[j].p  = tVerts[j];
					else TVMaps.v[j].p  = Point3(0.0f,0.0f,0.0f);
					TVMaps.v[j].influence = 0.0f;
					}
				if (tvFace == NULL) BuildInitialMapping(&patch);
				}
			else
				{

//copy into our structs
				TVMaps.f.SetCount(patch.getNumPatches());

				TVMaps.v.SetCount(patch.getNumMapVerts (CurrentChannel));
				TVMaps.cont.SetCount(patch.getNumMapVerts (CurrentChannel));

				vsel.SetSize(patch.getNumMapVerts (CurrentChannel));

				for (int j=0; j<TVMaps.f.Count(); j++) 
					{
					TVMaps.f[j].flags = 0;
					if (tvFace == NULL)
						{
						TVMaps.f[j].t[0] = 0;
						TVMaps.f[j].t[1] = 0;
						TVMaps.f[j].t[2] = 0;
						TVMaps.f[j].t[3] = 0;
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = patch.patches[j].getMatID();
						if (patch.patchSel[j])
							TVMaps.f[j].flags = 0;
						else TVMaps.f[j].flags = FLAG_DEAD;
						int pcount = 3;

						if (patch.patches[j].type == PATCH_QUAD) 
							{
							TVMaps.f[j].flags |= FLAG_QUAD;
							pcount = 4;
							}
						for (int k = 0; k < pcount; k++)
							{
							int index = patch.patches[j].v[k];
							TVMaps.f[j].pt[k] = patch.verts[index].p;
							}

						}
					else
						{
						TVMaps.f[j].t[0] = tvFace[j].tv[0];
						TVMaps.f[j].t[1] = tvFace[j].tv[1];
						TVMaps.f[j].t[2] = tvFace[j].tv[2];
						TVMaps.f[j].t[3] = tvFace[j].tv[3];
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = patch.patches[j].getMatID();
				

						if (patch.patchSel[j])
							TVMaps.f[j].flags = 0;
						else TVMaps.f[j].flags = FLAG_DEAD;

						int pcount = 3;
						if (patch.patches[j].type == PATCH_QUAD) 
							{
							TVMaps.f[j].flags |= FLAG_QUAD;
							pcount = 4;
							}

						for (int k = 0; k < pcount; k++)
							{
							int index = patch.patches[j].v[k];
							TVMaps.f[j].pt[k] = patch.verts[index].p;
							}

						}
					}
				for (j=0; j<TVMaps.v.Count(); j++) 
					{
					TVMaps.cont[j] = NULL;
					TVMaps.v[j].flags = FLAG_DEAD;
					if (tVerts)
						TVMaps.v[j].p  = tVerts[j];
					else TVMaps.v[j].p  = Point3(.0f,0.0f,0.0f);
//check if vertex for this face selected
					TVMaps.v[j].influence = 0.0f;

					}


				if (tvFace == NULL) BuildInitialMapping(&patch);

				for (j=0; j<TVMaps.f.Count(); j++) 
					{
					if (!(TVMaps.f[j].flags & FLAG_DEAD))
						{
						int a;
						a = TVMaps.f[j].t[0];
						TVMaps.v[a].flags = 0;
						a = TVMaps.f[j].t[1];
						TVMaps.v[a].flags = 0;
						a = TVMaps.f[j].t[2];
						TVMaps.v[a].flags = 0;
						a = TVMaps.f[j].t[3];
						TVMaps.v[a].flags = 0;
						}
					}

				}



			}
///else it is something else convert to a mesh
		else if (os->obj->IsSubClassOf(triObjectClassID)) 
	
			{

			TriObject *tobj = (TriObject*)os->obj;
				// Apply our mapping
			Mesh &mesh = tobj->GetMesh();




//get channel from mesh
			TVMaps.channel = CurrentChannel;
					
			if ( (mesh.selLevel==MESH_FACE) && (mesh.faceSel.NumberSet() == 0) ) 
				{
				return;
				}
						
	
//get from mesh based on cahne
			TVFace *tvFace = mesh.mapFaces(CurrentChannel);
			Point3 *tVerts = mesh.mapVerts(CurrentChannel);
			if (mesh.selLevel!=MESH_FACE) 
				{
//copy into our structs
				TVMaps.f.SetCount(mesh.getNumFaces());
				TVMaps.v.SetCount(mesh.getNumMapVerts (CurrentChannel));
				TVMaps.cont.SetCount(mesh.getNumMapVerts (CurrentChannel));

				vsel.SetSize(mesh.getNumMapVerts (CurrentChannel));


				for (int j=0; j<TVMaps.f.Count(); j++) 
					{
					TVMaps.f[j].flags = 0;
					if (tvFace == NULL)
						{
						TVMaps.f[j].t[0] = 0;
						TVMaps.f[j].t[1] = 0;
						TVMaps.f[j].t[2] = 0;
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = mesh.faces[j].getMatID();
						TVMaps.f[j].flags = 0;
						for (int k = 0; k < 3; k++)
							{
							int index = mesh.faces[j].v[k];
							TVMaps.f[j].pt[k] = mesh.verts[index];
							}
						}
					else
						{
						TVMaps.f[j].t[0] = tvFace[j].t[0];
						TVMaps.f[j].t[1] = tvFace[j].t[1];
						TVMaps.f[j].t[2] = tvFace[j].t[2];
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = mesh.faces[j].getMatID();
				

						TVMaps.f[j].flags = 0;
						for (int k = 0; k < 3; k++)
							{
							int index = mesh.faces[j].v[k];
							TVMaps.f[j].pt[k] = mesh.verts[index];
							}

							
						}
					}
				for (    j=0; j<TVMaps.v.Count(); j++) 
					{
					TVMaps.cont[j] = NULL;
					TVMaps.v[j].flags = 0;
					if (tVerts)
						TVMaps.v[j].p  = tVerts[j];
					else TVMaps.v[j].p  = Point3(.0f,0.0f,0.0f);
					TVMaps.v[j].influence = 0.0f;
					}
				if (tvFace == NULL) BuildInitialMapping(&mesh);
				}
			else
				{
//copy into our structs
				TVMaps.f.SetCount(mesh.getNumFaces());

				TVMaps.v.SetCount(mesh.getNumMapVerts (CurrentChannel));
				TVMaps.cont.SetCount(mesh.getNumMapVerts (CurrentChannel));

				vsel.SetSize(mesh.getNumMapVerts (CurrentChannel));


				for (int j=0; j<TVMaps.f.Count(); j++) 
					{
					TVMaps.f[j].flags = 0;
					if (tvFace == NULL)
						{
						TVMaps.f[j].t[0] = 0;
						TVMaps.f[j].t[1] = 0;
						TVMaps.f[j].t[2] = 0;
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = mesh.faces[j].getMatID();
						if (mesh.faceSel[j])
							TVMaps.f[j].flags = 0;
						else TVMaps.f[j].flags = FLAG_DEAD;
						for (int k = 0; k < 3; k++)
							{
							int index = mesh.faces[j].v[k];
							TVMaps.f[j].pt[k] = mesh.verts[index];
							}
						}
					else
						{
						TVMaps.f[j].t[0] = tvFace[j].t[0];
						TVMaps.f[j].t[1] = tvFace[j].t[1];
						TVMaps.f[j].t[2] = tvFace[j].t[2];
						TVMaps.f[j].FaceIndex = j;
						TVMaps.f[j].MatID = mesh.faces[j].getMatID();
				
						if (mesh.faceSel[j])
							TVMaps.f[j].flags = 0;
						else TVMaps.f[j].flags = FLAG_DEAD;
						for (int k = 0; k < 3; k++)
							{
							int index = mesh.faces[j].v[k];
							TVMaps.f[j].pt[k] = mesh.verts[index];
							}
								
						}
					}
				for (j=0; j<TVMaps.v.Count(); j++) 
					{
					TVMaps.cont[j] = NULL;
					TVMaps.v[j].flags = FLAG_DEAD;
					if (tVerts)
						TVMaps.v[j].p  = tVerts[j];
					else TVMaps.v[j].p  = Point3(0.0f,0.0f,0.0f);
//check if vertex for this face selected
					TVMaps.v[j].influence = 0.0f;

					}
				if (tvFace == NULL) BuildInitialMapping(&mesh);
				for (j=0; j<TVMaps.f.Count(); j++) 
					{
					if (TVMaps.f[j].flags != FLAG_DEAD)
						{
						int a;
						a = TVMaps.f[j].t[0];
						TVMaps.v[a].flags = 0;
						a = TVMaps.f[j].t[1];
						TVMaps.v[a].flags = 0;
						a = TVMaps.f[j].t[2];
						TVMaps.v[a].flags = 0;
						}
					}


				}


			}

		BuildMatIDList();

			
		}
//we already have edits so just copy them into the  tv faces/verts
	if (TVMaps.v.Count() != 0)
		{
//else copy our data into the mesh,patch or nurbs tv data
///is patch
			if (os->obj->IsSubClassOf(patchObjectClassID))
				{
// is whole mesh
				PatchObject *pobj = (PatchObject*)os->obj;
				// Apply our mapping
				PatchMesh &patch = pobj->patch;

//fix this get the channels from mesh
//				int NumChannels = 1;

//loop through all maps
//				for (int i =0; i<NumChannels;i++)
					{	
//get channel from mesh
//					int CurrentChannel = 0;
					TVMaps.channel = CurrentChannel;
					
	
//get from mesh 

					if (!patch.getMapSupport(CurrentChannel) )
						{
						patch.setNumMaps (CurrentChannel+1);
						}

					TVPatch *tvFace = patch.tvPatches[CurrentChannel];

					
					int tvFaceCount =  patch.numPatches;

					if (patch.selLevel!=PATCH_PATCH) 
						{
//copy into mesh struct
						if (!tvFace) 
							{
			// Create tvfaces and init to 0
							patch.setNumMapPatches(CurrentChannel,patch.getNumPatches());
							tvFace = patch.tvPatches[CurrentChannel];
							for (int k=0; k<patch.getNumPatches(); k++)
								{	
								for (int j=0; j<4; j++) 
									{
									tvFace[k].tv[j] = 0;			
									}
								}
							}
						for (int k=0; k<tvFaceCount; k++) 
							{
							if (k < TVMaps.f.Count())
								{
								tvFace[k].tv[0] = TVMaps.f[k].t[0];
								tvFace[k].tv[1] = TVMaps.f[k].t[1];
								tvFace[k].tv[2] = TVMaps.f[k].t[2];
								tvFace[k].tv[3] = TVMaps.f[k].t[3];
								}
							else{
								tvFace[k].tv[0] = 0;
								tvFace[k].tv[1] = 0;
								tvFace[k].tv[2] = 0;
								tvFace[k].tv[3] = 0;
								}
							}
//match verts
						patch.setNumMapVerts (CurrentChannel,TVMaps.v.Count());
						Point3 *tVerts = patch.tVerts[CurrentChannel];
						for (    k=0; k<TVMaps.v.Count(); k++) 
							tVerts[k] = GetPoint(t,k);
		
						}
					else
						{
//copy into mesh struct
						if (!tvFace) 
							{
			// Create tvfaces and init to 0
							patch.setNumMapPatches (CurrentChannel,patch.getNumPatches());
							tvFace = patch.tvPatches[CurrentChannel];
							for (int k=0; k<patch.getNumPatches(); k++)
								{	
								for (int j=0; j<4; j++) 
									{
									tvFace[k].tv[j] = 0;			
									}
								}
							}
						int offset = patch.getNumMapVerts (CurrentChannel);
						int current = 0;
						for (int k=0; k<tvFaceCount; k++) 
//						for (int k=0; k<TVMaps.f.Count(); k++) 
							{
//copy if face is selected
							if (patch.patchSel[k])
								{
//								if (current < TVMaps.f.Count())
									{
									tvFace[k].tv[0] = TVMaps.f[k].t[0]+offset;
									tvFace[k].tv[1] = TVMaps.f[k].t[1]+offset;
									tvFace[k].tv[2] = TVMaps.f[k].t[2]+offset;
									tvFace[k].tv[3] = TVMaps.f[k].t[3]+offset;
//									current++;
									}
								}
							}
//match verts
						patch.setNumMapVerts (CurrentChannel,TVMaps.v.Count()+offset,TRUE);
						Point3 *tVerts = patch.tVerts[CurrentChannel];
						for (    k=0; k<TVMaps.v.Count(); k++) 
							tVerts[k+offset] = GetPoint(t,k);
		
						}
					RemoveDeadVerts(&patch,CurrentChannel);

					}
				}


///else convert to a mesh
			else if (os->obj->IsSubClassOf(triObjectClassID)) 
				{
// is whole mesh
				TriObject *tobj = (TriObject*)os->obj;
				// Apply our mapping
				Mesh &mesh = tobj->GetMesh();

				if (!mesh.mapSupport(CurrentChannel)) {
		// allocate texture verts. Setup tv faces into a parallel
		// topology as the regular faces
					if (CurrentChannel >= mesh.getNumMaps ()) mesh.setNumMaps (CurrentChannel+1, TRUE);
					mesh.setMapSupport (CurrentChannel, TRUE);
					} 


	
				TVFace *tvFace = mesh.mapFaces(CurrentChannel);
				
				int tvFaceCount =  mesh.numFaces;

				if (mesh.selLevel!=MESH_FACE) 
					{
//copy into mesh struct

					for (int k=0; k<tvFaceCount; k++) 
						{
						if (k < TVMaps.f.Count())
							{
							tvFace[k].t[0] = TVMaps.f[k].t[0];
							tvFace[k].t[1] = TVMaps.f[k].t[1];
							tvFace[k].t[2] = TVMaps.f[k].t[2];
							}
						else 
							{
							tvFace[k].t[0] = 0;
							tvFace[k].t[1] = 0;
							tvFace[k].t[2] = 0;
							}
						}
//match verts
					mesh.setNumMapVerts (CurrentChannel, TVMaps.v.Count());
					Point3 *tVerts = mesh.mapVerts(CurrentChannel);
					for (    k=0; k<TVMaps.v.Count(); k++) 
						{
						tVerts[k] = GetPoint(t,k);
						}

		
					}
				else
					{
//copy into mesh struct
//check if mesh has existing tv faces
					int offset = mesh.getNumMapVerts (CurrentChannel);
					int current = 0;
					for (int k=0; k<tvFaceCount; k++) 
						{
//copy if face is selected
						if (mesh.faceSel[k]==1)
							{
//							if (current < TVMaps.f.Count())
								{
								tvFace[k].t[0] = TVMaps.f[k].t[0] + offset;
								tvFace[k].t[1] = TVMaps.f[k].t[1] + offset;
								tvFace[k].t[2] = TVMaps.f[k].t[2] + offset;
//								current++;
								}
							}
						}
//add our verts
					mesh.setNumMapVerts (CurrentChannel,TVMaps.v.Count()+offset,TRUE);
					Point3 *tVerts = mesh.mapVerts(CurrentChannel);
					for (    k=0; k<TVMaps.v.Count(); k++) 
						tVerts[k+offset] = GetPoint(t,k);

							

					}
				mesh.DeleteIsoMapVerts(CurrentChannel);

					
				}

		}

	os->obj->UpdateValidity(TEXMAP_CHAN_NUM,LocalValidity(t));	

	}

Interval UnwrapMod::LocalValidity(TimeValue t)
	{
	Interval iv = FOREVER;
	for (int i=0; i<TVMaps.cont.Count(); i++) {
		if (TVMaps.cont[i]) {
			TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,iv);
			}
		}
	return iv;
	}

RefTargetHandle UnwrapMod::GetReference(int i)
	{
		if (i==0) return tmControl;
		if (i<11) return map[i-1];
		return TVMaps.cont[i-11];
	}

void UnwrapMod::SetReference(int i, RefTargetHandle rtarg)
	{
		if (i==0) tmControl = (Control*)rtarg;
		else if (i<11) map[i-1] = (Texmap*)rtarg;
		else TVMaps.cont[i-11] = (Control*)rtarg;
	}

int UnwrapMod::RemapRefOnLoad(int iref) 
{
if (version == 1)
	{
	if (iref == 0)
		return 1;
	else if (iref > 0)
		return iref + 10;

	}	
return iref;
}

Animatable* UnwrapMod::SubAnim(int i)
	{
	return TVMaps.cont[i];
	}

TSTR UnwrapMod::SubAnimName(int i)
	{
	TSTR buf;
//	buf.printf(_T("Point %d"),i+1);
	buf.printf(_T("%s %d"),GetString(IDS_PW_POINT),i+1);
	return buf;
	}


RefTargetHandle UnwrapMod::Clone(RemapDir& remap)
	{
	UnwrapMod *mod = new UnwrapMod;
	mod->TVMaps    = TVMaps;
//	mod->tvert     = tvert;
//	mod->tvFace    = tvFace;
//	mod->cont      = cont;
	mod->vsel      = vsel;
	mod->zoom      = zoom;
	mod->aspect    = aspect;
	mod->xscroll   = xscroll;
	mod->yscroll   = yscroll;
	mod->uvw       = uvw;
	mod->showMap   = showMap;
	mod->update    = update;
	mod->lineColor = lineColor;
	mod->selColor  =	selColor;
	mod->rendW     = rendW;
	mod->rendH     = rendH;
	mod->isBitmap =  isBitmap;
	mod->isBitmap =  pixelSnap;
	mod->useBitmapRes = useBitmapRes;
	mod->channel = channel;
	mod->ReplaceReference(0,remap.CloneRef(tmControl));
	for (int i=0; i<10; i++) 
		mod->ReplaceReference(i+1,map[i]);
	for (i=0; i<TVMaps.cont.Count(); i++) {
		mod->TVMaps.cont[i] = NULL;		
		if (TVMaps.cont[i]) mod->ReplaceReference(i+11,remap.CloneRef(TVMaps.cont[i]));
		}

	if (instanced)
		{
		for (i=0; i<mod->TVMaps.cont.Count(); i++) mod->DeleteReference(i+11);
		mod->TVMaps.v.Resize(0);
		mod->TVMaps.f.Resize(0);
		mod->TVMaps.cont.Resize(0);
		mod->vsel.SetSize(0);
		mod->updateCache = TRUE;
		mod->instanced = FALSE;
		}

	return mod;
	}

#define NAMEDSEL_STRING_CHUNK	0x2809
#define NAMEDSEL_ID_CHUNK		0x2810


RefResult UnwrapMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	for (int i = 0; i < 10; i++)
		if (hTarget==map[i]) return REF_STOP;
	switch (message) {
		case REFMSG_CHANGE:
			if (editMod==this && hView) {
				InvalidateView();
				}
			break;
		case REFMSG_TARGET_DELETED:
			{
			for (int i=0; i<10; i++) 
				{
				if (hTarget==map[i]) {
					map[i] = NULL;
					}
				}
			break;
			}

		}
	return REF_SUCCEED;
	}

#define VERTCOUNT_CHUNK	0x0100
#define VERTS_CHUNK		0x0110
#define VERTSEL_CHUNK	0x0120
#define ZOOM_CHUNK		0x0130
#define ASPECT_CHUNK	0x0140
#define XSCROLL_CHUNK	0x0150
#define YSCROLL_CHUNK	0x0160
#define IWIDTH_CHUNK	0x0170
#define IHEIGHT_CHUNK	0x0180
#define SHOWMAP_CHUNK	0x0190
#define UPDATE_CHUNK	0x0200
#define LINECOLOR_CHUNK	0x0210
#define SELCOLOR_CHUNK	0x0220
#define FACECOUNT_CHUNK	0x0230
#define FACE_CHUNK		0x0240
#define UVW_CHUNK		0x0250
#define CHANNEL_CHUNK	0x0260
#define VERTS2_CHUNK	0x0270
#define FACE2_CHUNK		0x0280
#define PREFS_CHUNK		0x0290
#define USEBITMAPRES_CHUNK		0x0300

IOResult UnwrapMod::Save(ISave *isave)
	{
	ULONG nb;
	Modifier::Save(isave);

	int vct = TVMaps.v.Count(), fct = TVMaps.f.Count();

	isave->BeginChunk(VERTCOUNT_CHUNK);
	isave->Write(&vct, sizeof(vct), &nb);
	isave->EndChunk();

	if (vct) {
		isave->BeginChunk(VERTS2_CHUNK);
		isave->Write(TVMaps.v.Addr(0), sizeof(UVW_TVVertClass)*vct, &nb);
		isave->EndChunk();
		}
	
	isave->BeginChunk(FACECOUNT_CHUNK);
	isave->Write(&fct, sizeof(fct), &nb);
	isave->EndChunk();

	if (fct) {
		isave->BeginChunk(FACE2_CHUNK);
		isave->Write(TVMaps.f.Addr(0), sizeof(UVW_TVFaceClass)*fct, &nb);
		isave->EndChunk();
		}

	isave->BeginChunk(VERTSEL_CHUNK);
	vsel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(ZOOM_CHUNK);
	isave->Write(&zoom, sizeof(zoom), &nb);
	isave->EndChunk();

	isave->BeginChunk(ASPECT_CHUNK);
	isave->Write(&aspect, sizeof(aspect), &nb);
	isave->EndChunk();

	isave->BeginChunk(XSCROLL_CHUNK);
	isave->Write(&xscroll, sizeof(xscroll), &nb);
	isave->EndChunk();

	isave->BeginChunk(YSCROLL_CHUNK);
	isave->Write(&yscroll, sizeof(yscroll), &nb);
	isave->EndChunk();

	isave->BeginChunk(IWIDTH_CHUNK);
	isave->Write(&rendW, sizeof(rendW), &nb);
	isave->EndChunk();

	isave->BeginChunk(IHEIGHT_CHUNK);
	isave->Write(&rendH, sizeof(rendH), &nb);
	isave->EndChunk();

	isave->BeginChunk(UVW_CHUNK);
	isave->Write(&uvw, sizeof(uvw), &nb);
	isave->EndChunk();

	isave->BeginChunk(SHOWMAP_CHUNK);
	isave->Write(&showMap, sizeof(showMap), &nb);
	isave->EndChunk();

	isave->BeginChunk(UPDATE_CHUNK);
	isave->Write(&update, sizeof(update), &nb);
	isave->EndChunk();

	isave->BeginChunk(LINECOLOR_CHUNK);
	isave->Write(&lineColor, sizeof(lineColor), &nb);
	isave->EndChunk();

	isave->BeginChunk(SELCOLOR_CHUNK);
	isave->Write(&selColor, sizeof(selColor), &nb);
	isave->EndChunk();

	isave->BeginChunk(CHANNEL_CHUNK);
	isave->Write(&channel, sizeof(channel), &nb);
	isave->EndChunk();

	isave->BeginChunk(PREFS_CHUNK);
	isave->Write(&lineColor, sizeof(lineColor), &nb);
	isave->Write(&selColor, sizeof(selColor), &nb);
	isave->Write(&weldThreshold, sizeof(weldThreshold), &nb);
	isave->Write(&update, sizeof(update), &nb);
	isave->Write(&showVerts, sizeof(showVerts), &nb);
	isave->Write(&midPixelSnap, sizeof(midPixelSnap), &nb);
	isave->EndChunk();

		if (namedSel.Count()) {
			isave->BeginChunk(0x2806);			
			for (int i=0; i<namedSel.Count(); i++) {
				isave->BeginChunk(NAMEDSEL_STRING_CHUNK);
				isave->WriteWString(*namedSel[i]);
				isave->EndChunk();

				isave->BeginChunk(NAMEDSEL_ID_CHUNK);
				isave->Write(&ids[i],sizeof(DWORD),&nb);
				isave->EndChunk();
				}
			isave->EndChunk();
			}
	if (useBitmapRes)
		{
		isave->BeginChunk(USEBITMAPRES_CHUNK);
		isave->EndChunk();
		}

		
	return IO_OK;
	}


void UnwrapMod::LoadUVW(HWND hWnd)
{
static TCHAR fname[256] = {'\0'};
OPENFILENAME ofn;
memset(&ofn,0,sizeof(ofn));
FilterList fl;
fl.Append( GetString(IDS_PW_UVWFILES));
fl.Append( _T("*.uvw"));		
TSTR title = GetString(IDS_PW_LOADOBJECT);

ofn.lStructSize     = sizeof(OPENFILENAME);
ofn.hwndOwner       = hWnd;
ofn.lpstrFilter     = fl;
ofn.lpstrFile       = fname;
ofn.nMaxFile        = 256;    
//ofn.lpstrInitialDir = ip->GetDir(APP_EXPORT_DIR);
ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
ofn.lpstrDefExt     = _T("uvw");
ofn.lpstrTitle      = title;


if (GetOpenFileName(&ofn)) {
//load stuff here  stuff here
	FILE *file = fopen(fname,_T("rb"));

	for (int i = 0; i < TVMaps.cont.Count(); i++)
		{
		if (TVMaps.cont[i]) TVMaps.cont[i]->DeleteThis();
		}

	int vct = TVMaps.v.Count(), fct = TVMaps.f.Count();

	fread(&vct, sizeof(vct), 1,file);

	TVMaps.v.SetCount(vct);
	vsel.SetSize(vct);

	TVMaps.cont.SetCount(vct);
	for (i = 0; i < vct; i++)
		TVMaps.cont[i] = NULL;

	if (vct) {
		fread(TVMaps.v.Addr(0), sizeof(UVW_TVVertClass)*vct, 1,file);
		}
	
	fread(&fct, sizeof(fct), 1,file);
	TVMaps.f.SetCount(fct);

	if (fct) {
		fread(TVMaps.f.Addr(0), sizeof(UVW_TVFaceClass)*fct, 1,file);
		}

	fclose(file);

	}

}
void UnwrapMod::SaveUVW(HWND hWnd)
{
static TCHAR fname[256] = {'\0'};
OPENFILENAME ofn;
memset(&ofn,0,sizeof(ofn));
FilterList fl;
fl.Append( GetString(IDS_PW_UVWFILES));
fl.Append( _T("*.uvw"));		
TSTR title = GetString(IDS_PW_SAVEOBJECT);

ofn.lStructSize     = sizeof(OPENFILENAME);
ofn.hwndOwner       = hWnd;
ofn.lpstrFilter     = fl;
ofn.lpstrFile       = fname;
ofn.nMaxFile        = 256;    
//ofn.lpstrInitialDir = ip->GetDir(APP_EXPORT_DIR);
ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
ofn.lpstrDefExt     = _T("uvw");
ofn.lpstrTitle      = title;

tryAgain:
if (GetSaveFileName(&ofn)) {
	if (DoesFileExist(fname)) {
		TSTR buf1;
		TSTR buf2 = GetString(IDS_PW_SAVEOBJECT);
		buf1.printf(GetString(IDS_PW_FILEEXISTS),fname);
		if (IDYES!=MessageBox(
			hParams,
			buf1,buf2,MB_YESNO|MB_ICONQUESTION)) {
			goto tryAgain;
			}
		}
//save stuff here
//	theAsciiOut.OutputObject(node,fname);
// this is timed slice so it will not save animation not sure how to save controller info but will neeed to later on in other plugs

	FILE *file = fopen(fname,_T("wb"));
	int vct = TVMaps.v.Count(), fct = TVMaps.f.Count();

//	isave->BeginChunk(VERTCOUNT_CHUNK);
//	isave->Write(&vct, sizeof(vct), &nb);
//	isave->EndChunk();
	fwrite(&vct, sizeof(vct), 1,file);

	if (vct) {
//		isave->BeginChunk(VERTS2_CHUNK);
//		isave->Write(TVMaps.v.Addr(0), sizeof(UVW_TVVertClass)*vct, &nb);
//		isave->EndChunk();
		fwrite(TVMaps.v.Addr(0), sizeof(UVW_TVVertClass)*vct, 1,file);
		}
	
//	isave->BeginChunk(FACECOUNT_CHUNK);
//	isave->Write(&fct, sizeof(fct), &nb);
//	isave->EndChunk();
	fwrite(&fct, sizeof(fct), 1,file);

	if (fct) {
//		isave->BeginChunk(FACE2_CHUNK);
//		isave->Write(TVMaps.f.Addr(0), sizeof(UVW_TVFaceClass)*fct, &nb);
//		isave->EndChunk();
		fwrite(TVMaps.f.Addr(0), sizeof(UVW_TVFaceClass)*fct, 1,file);
		}

	fclose(file);
	}

}


IOResult UnwrapMod::LoadNamedSelChunk(ILoad *iload) {	
	IOResult res;
	DWORD ix=0;
	ULONG nb;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case NAMEDSEL_STRING_CHUNK: {
			TCHAR *name;
			res = iload->ReadWStringChunk(&name);
			//AddSet(TSTR(name),level+1);
			TSTR *newName = new TSTR(name);
			namedSel.Append(1,&newName);				
			ids.Append(1,&ix);
			ix++;
			break;
			}
		case NAMEDSEL_ID_CHUNK:
			iload->Read(&ids[ids.Count()-1],sizeof(DWORD), &nb);
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}


class UnwrapPostLoadCallback:public  PostLoadCallback
{
public:
	UnwrapMod      *s;

	int oldData;
	UnwrapPostLoadCallback(UnwrapMod *r, BOOL b) {s=r;oldData = b;}
	void proc(ILoad *iload);
};

void UnwrapPostLoadCallback::proc(ILoad *iload)
{
	if (!oldData)
		{
		for (int i=0; i<10; i++) 
			s->ReplaceReference(i+1,NULL);
		}
	delete this;
}



IOResult UnwrapMod::Load(ILoad *iload)
	{

	version = 2;
	IOResult res;
	ULONG nb;
	Modifier::Load(iload);
	int ct, i;
//check for backwards compatibility
	useBitmapRes = FALSE;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case 0x2806:
				res = LoadNamedSelChunk(iload);
				break;

			case VERTCOUNT_CHUNK:
				iload->Read(&ct, sizeof(ct), &nb);
				TVMaps.v.SetCount(ct);
				TVMaps.cont.SetCount(ct);
				vsel.SetSize(ct);
				for (i=0; i<ct; i++) TVMaps.cont[i] = NULL;				
				break;
			case FACECOUNT_CHUNK:
				iload->Read(&ct, sizeof(ct), &nb);
				TVMaps.f.SetCount(ct);
				break;
//old way here for legacy reason only
			case FACE_CHUNK:
				{
				version = 1;
				oldDataPresent = TRUE;
				Tab<TVFace> f;
				f.SetCount(TVMaps.f.Count());
				iload->Read(f.Addr(0), sizeof(TVFace)*TVMaps.f.Count(), &nb);
				for (i=0;i<TVMaps.f.Count();i++)
					{
					TVMaps.f[i].t[0] = f[i].t[0];
					TVMaps.f[i].t[1] = f[i].t[1];
					TVMaps.f[i].t[2] = f[i].t[2];
					TVMaps.f[i].flags = 0;
					}

				break;
				}
//old way here for legacy reason only
			case VERTS_CHUNK:
				{
				Tab<Point3> p;
				p.SetCount(TVMaps.v.Count());
				oldDataPresent = TRUE;

				iload->Read(p.Addr(0), sizeof(Point3)*TVMaps.v.Count(), &nb);

				for (i=0;i<TVMaps.v.Count();i++)
					{
					TVMaps.v[i].p = p[i];
					TVMaps.v[i].flags = 0;
					TVMaps.v[i].influence = 0.0f;
					}
				break;
				}
			case FACE2_CHUNK:
				
				iload->Read(TVMaps.f.Addr(0), sizeof(UVW_TVFaceClass)*TVMaps.f.Count(), &nb);

				break;
			case VERTS2_CHUNK:
				iload->Read(TVMaps.v.Addr(0), sizeof(UVW_TVVertClass)*TVMaps.v.Count(), &nb);
				break;
			case VERTSEL_CHUNK:
				vsel.Load(iload);
				break;
			case ZOOM_CHUNK:
				iload->Read(&zoom, sizeof(zoom), &nb);
				break;
			case ASPECT_CHUNK:
				iload->Read(&aspect, sizeof(aspect), &nb);
				break;
			case XSCROLL_CHUNK:
				iload->Read(&xscroll, sizeof(xscroll), &nb);
				break;
			case YSCROLL_CHUNK:
				iload->Read(&yscroll, sizeof(yscroll), &nb);
				break;
			case IWIDTH_CHUNK:
				iload->Read(&rendW, sizeof(rendW), &nb);
				break;
			case IHEIGHT_CHUNK:
				iload->Read(&rendH, sizeof(rendH), &nb);
				break;
			case SHOWMAP_CHUNK:
				iload->Read(&showMap, sizeof(showMap), &nb);
				break;
			case UPDATE_CHUNK:
				iload->Read(&update, sizeof(update), &nb);
				break;
			case LINECOLOR_CHUNK:
				iload->Read(&lineColor, sizeof(lineColor), &nb);
				break;
			case SELCOLOR_CHUNK:
				iload->Read(&selColor, sizeof(selColor), &nb);
				break;			
			case UVW_CHUNK:
				iload->Read(&uvw, sizeof(uvw), &nb);
				break;
			case CHANNEL_CHUNK:
				iload->Read(&channel, sizeof(channel), &nb);
				break;			
			case PREFS_CHUNK:
				iload->Read(&lineColor, sizeof(lineColor), &nb);
				iload->Read(&selColor, sizeof(selColor), &nb);
				iload->Read(&weldThreshold, sizeof(weldThreshold), &nb);
				iload->Read(&update, sizeof(update), &nb);
				iload->Read(&showVerts, sizeof(showVerts), &nb);
				iload->Read(&midPixelSnap, sizeof(midPixelSnap), &nb);
				break;
			case USEBITMAPRES_CHUNK:
				useBitmapRes = TRUE;
				break;			



			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	BuildMatIDList();

	UnwrapPostLoadCallback* unwrapplcb = new UnwrapPostLoadCallback(this,oldDataPresent);
	iload->RegisterPostLoadCallback(unwrapplcb);

	return IO_OK;
	}

#define FACESEL_CHUNKID			0x0210
#define FSELSET_CHUNK			0x2846


IOResult UnwrapMod::SaveLocalData(ISave *isave, LocalModData *ld) {	
	MeshTopoData *d = (MeshTopoData*)ld;


	isave->BeginChunk(FACESEL_CHUNKID);
	d->faceSel.Save(isave);
	isave->EndChunk();

	if (d->fselSet.Count()) {
		isave->BeginChunk(FSELSET_CHUNK);
		d->fselSet.Save(isave);
		isave->EndChunk();
		}

	return IO_OK;
	}

IOResult UnwrapMod::LoadLocalData(ILoad *iload, LocalModData **pld) {
	MeshTopoData *d = new MeshTopoData;
	*pld = d;
	IOResult res;	
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case FACESEL_CHUNKID:
				d->faceSel.Load(iload);
				break;
			case FSELSET_CHUNK:
				res = d->fselSet.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}


void UnwrapMod::EditMapping(HWND hWnd)
	{
	RegisterClasses();
	if (!this->hWnd) {
		CreateDialogParam(
			hInstance,
			MAKEINTRESOURCE(IDD_UNWRAP_FLOATER),
//			hWnd,
			ip->GetMAXHWnd (),
			UnwrapFloaterDlgProc,
			(LPARAM)this);
		LoadMaterials();

	} else {
		SetForegroundWindow(this->hWnd);
		ShowWindow(this->hWnd,SW_RESTORE);
		}
	}

/*
void UnwrapMod::SynchWithMesh(Mesh &mesh)
	{
	int ct=0;
	if (mesh.selLevel==MESH_FACE) {
		for (int i=0; i<mesh.getNumFaces(); i++) {
			if (mesh.faceSel[i]) ct++;
			}
	} else {
		ct = mesh.getNumFaces();
		}
	if (ct != tvFace.Count()) {
		DeleteAllRefsFromMe();
		tvert.Resize(0);
		cont.Resize(0);
		tvFace.SetCount(ct);
		
		TVFace *tvFaceM = mesh.tvFace;
		Point3 *tVertsM = mesh.tVerts;
		int numTV = channel ? mesh.getNumVertCol() : mesh.getNumTVerts();
		if (channel) {
			tvFaceM = mesh.vcFace;
			tVertsM = mesh.vertCol;
			}

		if (mesh.selLevel==MESH_FACE) {
			// Mark tverts that are used by selected faces
			BitArray used;
			if (tvFaceM) used.SetSize(numTV);
			else used.SetSize(mesh.getNumVerts());
			for (int i=0; i<mesh.getNumFaces(); i++) {
				if (mesh.faceSel[i]) {
					if (tvFaceM) {
						for (int j=0; j<3; j++) 
							used.Set(tvFaceM[i].t[j]);
					} else {
						for (int j=0; j<3; j++) 
							used.Set(mesh.faces[i].v[j]);
						}
					}
				}

			// Now build a vmap
			Tab<DWORD> vmap;
			vmap.SetCount(used.GetSize());
			int ix=0;
			for (i=0; i<used.GetSize(); i++) {
				if (used[i]) vmap[i] = ix++;
				else vmap[i] = UNDEFINED;				
				}

			// Copy in tverts
			tvert.SetCount(ix);
			cont.SetCount(ix);
			vsel.SetSize(ix);
			ix = 0;
			Box3 box = mesh.getBoundingBox();
			for (i=0; i<used.GetSize(); i++) {
				if (used[i]) {
					cont[ix] = NULL;
					if (tvFaceM) tvert[ix++] = tVertsM[i];
					else {
						// Do a planar mapping if there are no tverts
						tvert[ix].x = mesh.verts[i].x/box.Width().x + 0.5f;
						tvert[ix].y = mesh.verts[i].y/box.Width().y + 0.5f;
						tvert[ix].z = mesh.verts[i].z/box.Width().z + 0.5f;
						ix++;
						}
					}
				}

			// Copy in face and remap indices		
			ix = 0;
			for (i=0; i<mesh.getNumFaces(); i++) {
				if (mesh.faceSel[i]) {
					if (tvFaceM) tvFace[ix] = tvFaceM[i];
					else {
						for (int j=0; j<3; j++) 
							tvFace[ix].t[j] = mesh.faces[i].v[j];
						}

					for (int j=0; j<3; j++) {
						tvFace[ix].t[j] = vmap[tvFace[ix].t[j]];
						}
					ix++;
					}
				}
		} else {
			// Just copy all the tverts and faces
			if (tvFaceM) {
				tvert.SetCount(numTV);
				cont.SetCount(numTV);
				vsel.SetSize(numTV);
				for (int i=0; i<numTV; i++) {
					tvert[i] = tVertsM[i];
					cont[i]  = NULL;
					}
				for (i=0; i<mesh.getNumFaces(); i++) {
					tvFace[i] = tvFaceM[i];
					}
			} else {
				Box3 box = mesh.getBoundingBox();
				tvert.SetCount(mesh.getNumVerts());
				cont.SetCount(mesh.getNumVerts());
				vsel.SetSize(mesh.getNumVerts());
				for (int i=0; i<mesh.getNumVerts(); i++) {
					// Do a planar mapping if there are no tverts
					tvert[i].x = mesh.verts[i].x/box.Width().x + 0.5f;
					tvert[i].y = mesh.verts[i].y/box.Width().y + 0.5f;
					tvert[i].z = mesh.verts[i].z/box.Width().z + 0.5f;
					cont[i]  = NULL;
					}
				for (i=0; i<mesh.getNumFaces(); i++) {
					for (int j=0; j<3; j++) 
						tvFace[i].t[j] = mesh.faces[i].v[j];
					}
				}
			}
		if (hView && editMod==this) {
			InvalidateView();
			}
		}
	}
*/
void UnwrapMod::GetUVWIndices(int &i1, int &i2)
	{
	switch (uvw) {
		case 0: i1 = 0; i2 = 1; break;
		case 1: i1 = 1; i2 = 2; break;
		case 2: i1 = 0; i2 = 2; break;
		}
	}


//--- Floater Dialog -------------------------------------------------


#define TOOL_HEIGHT		30
#define SPINNER_HEIGHT	30

#define WM_SETUPMOD	WM_USER+0x18de

static HIMAGELIST hToolImages = NULL;
static HIMAGELIST hOptionImages = NULL;
static HIMAGELIST hViewImages = NULL;
static HIMAGELIST hVertexImages = NULL;

class DeleteResources {
	public:
		~DeleteResources() {
			if (hToolImages) ImageList_Destroy(hToolImages);			
			if (hOptionImages) ImageList_Destroy(hOptionImages);			
			if (hViewImages) ImageList_Destroy(hViewImages);			
			if (hVertexImages) ImageList_Destroy(hVertexImages);			
			}
	};
static DeleteResources	theDelete;

void UnwrapMod::TrackRBMenu(HWND hwnd, int x, int y) {
//	hPopMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_POPUP_MENU));
//	HMENU hMenu = getResMgr().getMenu(IDR_UNWRAP_RIGHT_MENU);
	HMENU hMenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_UNWRAP_RIGHT_MENU));
	HMENU subMenu = GetSubMenu(hMenu, 0);
	RECT rect;

/*	CheckMenuItem(subMenu, ID_DRAG_COPY,  dragMode == DRAG_COPY   ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(subMenu, ID_DRAG_ROT, dragMode == DRAG_ROTATE ? MF_CHECKED : MF_UNCHECKED);
	EnableMenuItem(subMenu, ID_RENDERMAP, IsTex(CurMtl(activeSlot))?MF_ENABLED:MF_GRAYED );

	CheckMenuItem(subMenu, ID_MEDIT_ZOOM0,  zoomLevel == 0  ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(subMenu, ID_MEDIT_ZOOM1,  zoomLevel == 1  ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(subMenu, ID_MEDIT_ZOOM2,  zoomLevel == 2  ? MF_CHECKED : MF_UNCHECKED);

	EnableMenuItem(subMenu, ID_MEDIT_MAG, (CanMagnify(activeSlot)&&!isMag)?MF_ENABLED:MF_GRAYED );
*/

	GetWindowRect(hwnd, &rect);
	TrackPopupMenu(subMenu, TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
		rect.left+x-2, rect.top+y, 0, hWnd, NULL);
	DestroyMenu(subMenu);
	DestroyMenu(hMenu);		

	}




static BOOL CALLBACK UnwrapFloaterDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	UnwrapMod *mod = (UnwrapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	//POINTS p = MAKEPOINTS(lParam);	commented out by sca 10/7/98 -- causing warning since unused.
	switch (msg) {
		case WM_INITDIALOG:
			mod = (UnwrapMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, GetClassLong(mod->ip->GetMAXHWnd(), GCL_HICONSM)); // mjm - 3.12.99
			mod->ip->RegisterDlgWnd(hWnd);
			mod->SetupDlg(hWnd);

			delEvent.SetEditMeshMod(mod);
			mod->ip->RegisterDeleteUser(&delEvent);

			mod->UpdateListBox();
			SendMessage( mod->hTextures, CB_SETCURSEL, mod->CurrentMap, 0 );


			break;
		case WM_SIZE:
			mod->SizeDlg();
			break;
//		case WM_RBUTTONDOWN:
//			break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			Rect rect;
			GetClientRect(hWnd,&rect);			
			rect.top += TOOL_HEIGHT-2;
			SelectObject(hdc,GetStockObject(WHITE_BRUSH));			
			WhiteRect3D(hdc,rect,TRUE);
			EndPaint(hWnd,&ps);
			break;
			}
		case CC_SPINNER_BUTTONDOWN:
			if (LOWORD(wParam) != IDC_UNWRAP_STRSPIN) 
				{
				theHold.SuperBegin();
				mod->PlugControllers();			
				theHold.Begin();
				}
			break;

		case CC_SPINNER_CHANGE:
			if (LOWORD(wParam) == IDC_UNWRAP_STRSPIN) 
				{
				mod->RebuildDistCache();
				UpdateWindow(hWnd);
				mod->InvalidateView();
				}
			else
				{
				if (!theHold.Holding()) {
					theHold.SuperBegin();
					mod->PlugControllers();			
					theHold.Begin();
					}
			
				switch (LOWORD(wParam)) {
					case IDC_UNWRAP_USPIN:
						mod->TypeInChanged(0);
						break;
					case IDC_UNWRAP_VSPIN:
						mod->TypeInChanged(1);
						break;
					case IDC_UNWRAP_WSPIN:
						mod->TypeInChanged(2);
						break;
					}
				UpdateWindow(hWnd);
				}
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP:
			if ( (LOWORD(wParam) == IDC_UNWRAP_STR) || (LOWORD(wParam) == IDC_UNWRAP_STRSPIN) )
				{
				mod->RebuildDistCache();
				mod->InvalidateView();
				UpdateWindow(hWnd);
				}
			else
				{
				if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) {
					theHold.Accept(_T(GetString(IDS_PW_MOVE_UVW)));
					theHold.SuperAccept(_T(GetString(IDS_PW_MOVE_UVW)));
				} else {
					theHold.Cancel();
					theHold.SuperCancel();
					mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
					mod->InvalidateView();
					UpdateWindow(hWnd);
					mod->ip->RedrawViews(mod->ip->GetTime());
					}
				}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_TOOL_MOVE:
				case ID_TOOL_ROTATE:
				case ID_TOOL_SCALE:
				case ID_TOOL_WELD:
				case ID_TOOL_PAN:
				case ID_TOOL_ZOOM:
				case ID_TOOL_ZOOMREG:
				case ID_UNWRAP_MOVE:
				case ID_UNWRAP_ROTATE:
				case ID_UNWRAP_SCALE:
				case ID_UNWRAP_PAN:
				case ID_UNWRAP_WELD:
				case ID_UNWRAP_ZOOM:
				case ID_UNWRAP_ZOOMREGION:

					mod->move = mod->iMove->GetCurFlyOff();
					if (mod->move == 0)
						mod->iMove->SetTooltip(TRUE,GetString(IDS_RB_MOVE));
					else if (mod->move == 1)
						mod->iMove->SetTooltip(TRUE,GetString(IDS_PW_MOVEH));
					else if (mod->move == 2)
						mod->iMove->SetTooltip(TRUE,GetString(IDS_PW_MOVEV));
					
					mod->scale = mod->iScale->GetCurFlyOff();
					if (mod->scale == 0)
						mod->iScale->SetTooltip(TRUE,GetString(IDS_RB_SCALE));
					else if (mod->scale == 1)
						mod->iScale->SetTooltip(TRUE,GetString(IDS_PW_SCALEH));
					else if (mod->scale == 2)
						mod->iScale->SetTooltip(TRUE,GetString(IDS_PW_SCALEV));

					mod->SetMode(LOWORD(wParam));
					break;				

				case ID_TOOL_FALLOFF:
					mod->falloff = mod->iFalloff->GetCurFlyOff();
					mod->RebuildDistCache();
					mod->InvalidateView();
					break;
				case ID_TOOL_FALLOFF_SPACE:
					mod->falloffSpace = mod->iFalloffSpace->GetCurFlyOff();
					if (mod->falloffSpace)
						mod->iFalloffSpace->SetTooltip(TRUE,GetString(IDS_PW_FALLOFFSPACEUVW));
					else mod->iFalloffSpace->SetTooltip(TRUE,GetString(IDS_PW_FALLOFFSPACE));

					mod->RebuildDistCache();
					mod->InvalidateView();
					break;
				
				case ID_TOOL_INCSELECTED:
					mod->incSelected = mod->iIncSelected->GetCurFlyOff();
					if (mod->incSelected)
						mod->iIncSelected->SetTooltip(TRUE,GetString(IDS_PW_CONTRACTSELECTION));
					else mod->iIncSelected->SetTooltip(TRUE,GetString(IDS_PW_EXPANDSELECTION)); 
					mod->ExpandSelection(mod->incSelected);
					mod->InvalidateView();
					UpdateWindow(hWnd);
					break;
				case ID_UNWRAP_MIRROR:
				case ID_TOOL_MIRROR:
					mod->mirror = mod->iMirror->GetCurFlyOff();
					if (mod->mirror ==0)
						mod->iMirror->SetTooltip(TRUE,GetString(IDS_PW_MIRRORH));
					else if (mod->mirror ==1)
						mod->iMirror->SetTooltip(TRUE,GetString(IDS_PW_MIRRORV));

					mod->MirrorPoints(hWnd, mod->mirror);

					break;
				case ID_TOOL_LOCKSELECTED:
					mod->lockSelected = mod->iLockSelected->IsChecked();
					break;
				case ID_TOOL_FILTER_SELECTEDFACES:
					mod->filterSelectedFaces = mod->iFilterSelected->IsChecked();
					mod->InvalidateView();
					UpdateWindow(hWnd);
					break;
				case ID_UNWRAP_EXTENT:
				case ID_TOOL_ZOOMEXT:
					mod->zoomext = mod->iZoomExt->GetCurFlyOff();

					if (mod->zoomext ==0)
						mod->ZoomExtents();
					else mod->ZoomSelected();
					break;

				case ID_TOOL_FILTER_MATID:
					if ( HIWORD(wParam) == CBN_SELCHANGE ) {
						//get count
						mod->matid = SendMessage( mod->hMatIDs, CB_GETCURSEL, 0, 0 )-1;
						mod->SetMatFilters();

						mod->InvalidateView();
						}
					break;
				case ID_TOOL_TEXTURE_COMBO:
					if ( HIWORD(wParam) == CBN_SELCHANGE ) {
//get count
						int ct = SendMessage( mod->hTextures, CB_GETCOUNT, 0, 0 );
						int res = SendMessage( mod->hTextures, CB_GETCURSEL, 0, 0 );
//pick a new map
						if (res == (ct -2))
							{
							mod->PickMap();
							SendMessage( mod->hTextures, CB_SETCURSEL, mod->CurrentMap, 0 );
							}
						else if (res < (ct-3))
//select a current
							{
							mod->CurrentMap = res;
							mod->SetupImage();

							}

						}
					break;


				
				case ID_TOOL_UVW:
					mod->uvw = mod->iUVW->GetCurFlyOff();
					mod->InvalidateView();
					break;

				case ID_TOOL_PROP:
					SetFocus(hWnd);
					mod->PropDialog();
					break;

				case ID_TOOL_SHOWMAP:
					mod->showMap = mod->iShowMap->IsChecked();
					mod->InvalidateView();
					break;

				case ID_TOOL_SNAP:
					mod->pixelSnap = mod->iSnap->IsChecked();
//					mod->InvalidateView();
					break;

				case ID_UNWRAP_BREAK:
				case ID_TOOL_BREAK:
					mod->BreakSelected();
					mod->InvalidateView();
					break;
				case ID_UNWRAP_WELDSELECTED:
				case ID_TOOL_WELD_SEL:
					mod->WeldSelected();
					mod->InvalidateView();
					break;
				case ID_TOOL_UPDATE:
					mod->SetupImage();
					mod->InvalidateView();
					break;
				case ID_TOOL_HIDE:
					mod->hide = mod->iHide->GetCurFlyOff();
					if (mod->hide == 0)
						{
						mod->iHide->SetTooltip(TRUE,GetString(IDS_PW_HIDE));
						mod->HideSelected();
						}
					else{
						mod->iHide->SetTooltip(TRUE,GetString(IDS_PW_UNHIDE));
						mod->UnHideAll();
						}
					mod->InvalidateView();
					break;
/*
				case ID_TOOL_UNHIDE:
					mod->UnHideAll();
					mod->InvalidateView();
					break;
*/
				case ID_TOOL_FREEZE:
					mod->freeze = mod->iFreeze->GetCurFlyOff();
					if (mod->freeze == 0)
						{
						mod->iFreeze->SetTooltip(TRUE,GetString(IDS_PW_FREEZE));
						mod->FreezeSelected();
						}
					else
						{
						mod->iFreeze->SetTooltip(TRUE,GetString(IDS_PW_UNFREEZE));
						mod->UnFreezeAll();
						}
					mod->InvalidateView();
					break;
/*
				case ID_TOOL_UNFREEZE:
					mod->UnFreezeAll();
					mod->InvalidateView();
					break;
*/

				}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			mod->DestroyDlg();
			mod->ip->UnRegisterDeleteUser(&delEvent);

			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

static LRESULT CALLBACK UnwrapViewProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	UnwrapMod *mod = (UnwrapMod*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_SIZE:			
			if (mod) {
				mod->iBuf->Resize();
				mod->InvalidateView();
				}
			break;

		case WM_PAINT:
			if (mod) mod->PaintView();
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:		
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:		

		case WM_MOUSEMOVE:

			return mod->mouseMan.MouseWinProc(hWnd,msg,wParam,lParam);

		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
		}	
	return 0;
	}

void UnwrapMod::DestroyDlg()
	{
	ReleaseICustToolbar(iTool);
	iTool = NULL;
	ReleaseICustToolbar(iView);
	iView = NULL;
	ReleaseICustToolbar(iOption);
	iOption = NULL;
	ReleaseICustToolbar(iFilter);
	iFilter = NULL;
	ReleaseICustToolbar(iVertex);
	iVertex = NULL;
	ReleaseICustButton(iMove   ); iMove    = NULL;
	ReleaseICustButton(iRot    ); iRot     = NULL;
	ReleaseICustButton(iScale  ); iScale   = NULL;
	ReleaseICustButton(iFalloff   ); iFalloff    = NULL;
	ReleaseICustButton(iFalloffSpace   ); iFalloffSpace    = NULL;

	ReleaseICustButton(iMirror  ); iMirror   = NULL;
	ReleaseICustButton(iWeld   ); iWeld    = NULL;
	ReleaseICustButton(iPan    ); iPan     = NULL;
	ReleaseICustButton(iZoom   ); iZoom    = NULL;
	ReleaseICustButton(iUpdate ); iUpdate  = NULL;
	ReleaseICustButton(iZoomReg); iZoomReg = NULL;
	ReleaseICustButton(iZoomExt); iZoomExt = NULL;
	ReleaseICustButton(iUVW	   ); iUVW	   = NULL;
	ReleaseICustButton(iProp   ); iProp    = NULL;
	ReleaseICustButton(iShowMap); iShowMap = NULL;
	ReleaseICustButton(iLockSelected); iLockSelected = NULL;
	ReleaseICustButton(iFilterSelected); iFilterSelected = NULL;
	ReleaseICustButton(iHide); iHide = NULL;
	ReleaseICustButton(iFreeze); iFreeze = NULL;
	ReleaseICustButton(iIncSelected); iFreeze = NULL;
	ReleaseICustButton(iSnap); iSnap = NULL;
	ReleaseISpinner(iU); iU = NULL;
	ReleaseISpinner(iV); iV = NULL;
	ReleaseISpinner(iW); iW = NULL;

	ReleaseISpinner(iStr); iStr = NULL;

	ReleaseICustButton(iWeldSelected); iWeldSelected = NULL;
	ReleaseICustButton(iBreak); iBreak = NULL;

	DestroyIOffScreenBuf(iBuf); iBuf   = NULL;
	hWnd = NULL;
	delete moveMode; moveMode = NULL;
	delete rotMode; rotMode = NULL;
	delete scaleMode; scaleMode = NULL;
	delete panMode; panMode = NULL;
	delete zoomMode; zoomMode = NULL;
	delete zoomRegMode; zoomRegMode = NULL;
	delete weldMode; weldMode = NULL;
	delete rightMode; rightMode = NULL;	
	delete middleMode; middleMode = NULL;	
	mouseMan.SetMouseProc(NULL,LEFT_BUTTON,0);
	mouseMan.SetMouseProc(NULL,RIGHT_BUTTON,0);
	mouseMan.SetMouseProc(NULL,MIDDLE_BUTTON,0);
	ip->UnRegisterDlgWnd(hWnd);

	}



void UnwrapMod::SetupDlg(HWND hWnd)
	{
	this->hWnd = hWnd;
	
	hView = GetDlgItem(hWnd,IDC_UNWRAP_VIEW);
	SetWindowLong(hView,GWL_USERDATA,(LONG)this);
	iBuf = CreateIOffScreenBuf(hView);
	iBuf->SetBkColor(RGB(150,150,150));
	viewValid    = FALSE;
	typeInsValid = FALSE;

	moveMode = new MoveMode(this);
	rotMode = new RotateMode(this);
	scaleMode = new ScaleMode(this);
	panMode = new PanMode(this);
	zoomMode = new ZoomMode(this);
	zoomRegMode = new ZoomRegMode(this);
	weldMode = new WeldMode(this);
	rightMode = new RightMouseMode(this);
	middleMode = new MiddleMouseMode(this);

	mouseMan.SetMouseProc(rightMode,RIGHT_BUTTON,1);
	mouseMan.SetMouseProc(middleMode,MIDDLE_BUTTON,2);

	iU = GetISpinner(GetDlgItem(hWnd,IDC_UNWRAP_USPIN));
	iU->LinkToEdit(GetDlgItem(hWnd,IDC_UNWRAP_U),EDITTYPE_FLOAT);
	iU->SetLimits(-9999999, 9999999, FALSE);
	iU->SetAutoScale();
	
	iV = GetISpinner(GetDlgItem(hWnd,IDC_UNWRAP_VSPIN));
	iV->LinkToEdit(GetDlgItem(hWnd,IDC_UNWRAP_V),EDITTYPE_FLOAT);
	iV->SetLimits(-9999999, 9999999, FALSE);
	iV->SetAutoScale();
	
	iW = GetISpinner(GetDlgItem(hWnd,IDC_UNWRAP_WSPIN));
	iW->LinkToEdit(GetDlgItem(hWnd,IDC_UNWRAP_W),EDITTYPE_FLOAT);
	iW->SetLimits(-9999999, 9999999, FALSE);
	iW->SetAutoScale();	

	iStr = GetISpinner(GetDlgItem(hWnd,IDC_UNWRAP_STRSPIN));
	iStr->LinkToEdit(GetDlgItem(hWnd,IDC_UNWRAP_STR),EDITTYPE_FLOAT);
	iStr->SetLimits(0, 9999999, FALSE);
	iStr->SetAutoScale();	


	iTool = GetICustToolbar(GetDlgItem(hWnd,IDC_UNWARP_TOOLBAR));
	iTool->SetBottomBorder(TRUE);	
	iTool->SetImage(hToolImages);
	iTool->AddTool(ToolSeparatorItem(5));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			0, 0, 1, 1, 16, 15, 23, 22, ID_TOOL_MOVE));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			2, 2, 3, 3, 16, 15, 23, 22, ID_TOOL_ROTATE));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			4, 4, 5, 5, 16, 15, 23, 22, ID_TOOL_SCALE));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			14, 14, 15, 15, 16, 15, 23, 22, ID_TOOL_MIRROR));
	iTool->AddTool(ToolSeparatorItem(5));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			18, 18, 19, 19, 16, 15, 23, 22, ID_TOOL_INCSELECTED));
	iTool->AddTool(ToolSeparatorItem(5));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			24, 24, 25, 25, 16, 15, 23, 22, ID_TOOL_FALLOFF));

	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			32, 32, 33, 33, 16, 15, 23, 22, ID_TOOL_FALLOFF_SPACE));
	

	iOption = GetICustToolbar(GetDlgItem(hWnd,IDC_UNWRAP_OPTION_TOOLBAR));
	iOption->SetBottomBorder(TRUE);	
	iOption->SetImage(hOptionImages);
	iOption->AddTool(ToolSeparatorItem(5));
	iOption->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			0, 0, 0, 0, 16, 15, 70, 22, ID_TOOL_UPDATE));
	iOption->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			0, 0, 1, 1, 16, 15, 23, 22, ID_TOOL_SHOWMAP));
	iOption->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			2, 2, 2, 2, 16, 15, 23, 22, ID_TOOL_UVW));
	iOption->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			5, 5, 5, 5, 16, 15, 23, 22, ID_TOOL_PROP));

//	iOption->AddTool(ToolSeparatorItem(5));

// get the objects materail texture list

	iOption->AddTool(ToolOtherItem(_T("combobox"), 110,	230, ID_TOOL_TEXTURE_COMBO,
		                           CBS, 2, NULL, 0));
	hTextures = iOption->GetItemHwnd(ID_TOOL_TEXTURE_COMBO);
//	SendMessage(hTextures, WM_SETFONT, (WPARAM)GetAppHFont(), MAKELONG(0, 0));
	SendMessage(hTextures, CB_ADDSTRING, 0, (LPARAM)_T("---------------------"));	
	SendMessage(hTextures, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PW_PICK));	
	SendMessage(hTextures, CB_ADDSTRING, 0, (LPARAM)_T("---------------------"));
	SendMessage(hTextures,CB_SETCURSEL, (WPARAM)CurrentMap, (LPARAM)0 );


	


	iView = GetICustToolbar(GetDlgItem(hWnd,IDC_UNWARP_VIEW_TOOLBAR));
	iView->SetBottomBorder(FALSE);	
	iView->SetImage(hViewImages);
	iView->AddTool(ToolSeparatorItem(5));

	iView->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			0, 0, 1, 1, 16, 15, 23, 22, ID_TOOL_PAN));
	iView->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			2, 2, 3, 3, 16, 15, 23, 22, ID_TOOL_ZOOM));
	iView->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			 4,  4,  5,  5, 16, 15, 23, 22, ID_TOOL_ZOOMREG));
	iView->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			 6,  6,  7,  7, 16, 15, 23, 22, ID_TOOL_ZOOMEXT));
	iView->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			 10, 10,  10,  10, 16, 15, 23, 22, ID_TOOL_SNAP));

/*View->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			 11, 11,  11,  11, 16, 15, 23, 22, ID_TOOL_LOCKSELECTED));
*/



	iVertex = GetICustToolbar(GetDlgItem(hWnd,IDC_UNWRAP_VERTS_TOOLBAR));
	iVertex->SetBottomBorder(TRUE);	
	iVertex->SetImage(hVertexImages);
	iVertex->AddTool(ToolSeparatorItem(5));

	iVertex->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			0, 1, 1, 1, 16, 15, 23, 22, ID_TOOL_BREAK));
	iVertex->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			2, 2, 3, 3, 16, 15, 23, 22, ID_TOOL_WELD));
	iVertex->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			 4,  4,  5,  5, 16, 15, 23, 22, ID_TOOL_WELD_SEL));
//	iVertex->AddTool(ToolSeparatorItem(10));

	iFilter = GetICustToolbar(GetDlgItem(hWnd,IDC_UNWRAP_FILTER_TOOLBAR));
	iFilter->SetBottomBorder(FALSE);	
	iFilter->SetImage(hVertexImages);
	iFilter->AddTool(ToolSeparatorItem(5));

	iFilter->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			 14, 16,  15,  17, 16, 15, 23, 22, ID_TOOL_LOCKSELECTED));
	iFilter->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			 6,  6,  7,  7, 16, 15, 23, 22, ID_TOOL_HIDE));
	iFilter->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			 10,  10,  11,  11, 16, 15, 23, 22, ID_TOOL_FREEZE));

	iFilter->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			18, 19, 18, 19, 16, 15, 23, 22, ID_TOOL_FILTER_SELECTEDFACES));

	iFilter->AddTool(ToolSeparatorItem(10));

	iFilter->AddTool(ToolOtherItem(_T("combobox"), 70,	280, ID_TOOL_FILTER_MATID,
		                           CBS, 2, NULL, 0));
	hMatIDs = iFilter->GetItemHwnd(ID_TOOL_FILTER_MATID);
//	SendMessage(hTextures, WM_SETFONT, (WPARAM)GetAppHFont(), MAKELONG(0, 0));
//FIX THIS make res id
	SendMessage(hMatIDs, CB_ADDSTRING, 0, (LPARAM)_T(GetString(IDS_PW_ID_ALLID)));	
	SendMessage(hMatIDs,CB_SETCURSEL, (WPARAM)0, (LPARAM)0 );



	iWeld    = iVertex->GetICustButton(ID_TOOL_WELD);
	iLockSelected = iFilter->GetICustButton(ID_TOOL_LOCKSELECTED);
	iHide	 = iFilter->GetICustButton(ID_TOOL_HIDE);
	iFreeze	 = iFilter->GetICustButton(ID_TOOL_FREEZE);
	iFilterSelected = iFilter->GetICustButton(ID_TOOL_FILTER_SELECTEDFACES);

	iMove    = iTool->GetICustButton(ID_TOOL_MOVE);
	iRot     = iTool->GetICustButton(ID_TOOL_ROTATE);
	iScale   = iTool->GetICustButton(ID_TOOL_SCALE);	
	iMirror   = iTool->GetICustButton(ID_TOOL_MIRROR);	
	iIncSelected = iTool->GetICustButton(ID_TOOL_INCSELECTED);	
	iFalloff = iTool->GetICustButton(ID_TOOL_FALLOFF);	
	iFalloffSpace = iTool->GetICustButton(ID_TOOL_FALLOFF_SPACE);

	iPan     = iView->GetICustButton(ID_TOOL_PAN);	
	iZoom    = iView->GetICustButton(ID_TOOL_ZOOM);
	iZoomExt = iView->GetICustButton(ID_TOOL_ZOOMEXT);
	iZoomReg = iView->GetICustButton(ID_TOOL_ZOOMREG);
	iSnap = iView->GetICustButton(ID_TOOL_SNAP);

	iBreak = iVertex->GetICustButton(ID_TOOL_BREAK);
	iWeldSelected = iVertex->GetICustButton(ID_TOOL_WELD_SEL);

	iUpdate  = iOption->GetICustButton(ID_TOOL_UPDATE);
	iUVW	 = iOption->GetICustButton(ID_TOOL_UVW);
	iProp	 = iOption->GetICustButton(ID_TOOL_PROP);
	iShowMap = iOption->GetICustButton(ID_TOOL_SHOWMAP);
	
	iMove->SetTooltip(TRUE,GetString(IDS_RB_MOVE));
	iRot->SetTooltip(TRUE,GetString(IDS_RB_ROTATE));
	iScale->SetTooltip(TRUE,GetString(IDS_RB_SCALE));
	iPan->SetTooltip(TRUE,GetString(IDS_RB_PAN));
	iZoom->SetTooltip(TRUE,GetString(IDS_RB_ZOOM));
	iUpdate->SetTooltip(TRUE,GetString(IDS_RB_UPDATE));
	iZoomExt->SetTooltip(TRUE,GetString(IDS_RB_ZOOMEXT));
	iZoomReg->SetTooltip(TRUE,GetString(IDS_RB_ZOOMREG));
	iUVW->SetTooltip(TRUE,GetString(IDS_RB_UVW));
	iProp->SetTooltip(TRUE,GetString(IDS_RB_PROP));
	iShowMap->SetTooltip(TRUE,GetString(IDS_RB_SHOWMAP));
	iSnap->SetTooltip(TRUE,GetString(IDS_PW_SNAP));
	iWeld->SetTooltip(TRUE,GetString(IDS_PW_WELD));

	iWeldSelected->SetTooltip(TRUE,GetString(IDS_PW_WELDSELECTED));
	iBreak->SetTooltip(TRUE,GetString(IDS_PW_BREAK));



	iMirror->SetTooltip(TRUE,GetString(IDS_PW_MIRRORH));
	iIncSelected->SetTooltip(TRUE,GetString(IDS_PW_EXPANDSELECTION));
	iFalloff->SetTooltip(TRUE,GetString(IDS_PW_FALLOFF)); 
	iFalloffSpace->SetTooltip(TRUE,GetString(IDS_PW_FALLOFFSPACE)); 
//need break tool tip	iBreak->SetTooltip(TRUE,GetString(IDS_PW_BREAK)); 
//need weld selecetd	iBreak->SetTooltip(TRUE,GetString(IDS_PW_BREAK)); 
	iLockSelected->SetTooltip(TRUE,GetString(IDS_PW_LOCKSELECTED));
	iHide->SetTooltip(TRUE,GetString(IDS_PW_HIDE));
	iFreeze->SetTooltip(TRUE,GetString(IDS_PW_FREEZE));
	iFilterSelected->SetTooltip(TRUE,GetString(IDS_PW_FACEFILTER));

	iMove->SetHighlightColor(GREEN_WASH);
	iRot->SetHighlightColor(GREEN_WASH);
	iScale->SetHighlightColor(GREEN_WASH);
	iWeld->SetHighlightColor(GREEN_WASH);
	iPan->SetHighlightColor(GREEN_WASH);
	iZoom->SetHighlightColor(GREEN_WASH);	
	iZoomReg->SetHighlightColor(GREEN_WASH);	
	iMirror->SetHighlightColor(GREEN_WASH);	
	iFalloff->SetHighlightColor(GREEN_WASH);	
	iFalloffSpace->SetHighlightColor(GREEN_WASH);	

	iUpdate->SetImage(NULL,0,0,0,0,0,0);
	iUpdate->SetText(GetString(IDS_RB_UPDATE));

	FlyOffData fdata1[] = {
		{ 2, 2 ,  2,  2},
		{ 3,  3,  3,  3},
		{ 4,  4,  4,  4}};
	iUVW->SetFlyOff(3,fdata1,ip->GetFlyOffTime(),uvw,FLY_DOWN);

	FlyOffData fdata2[] = {
		{ 0,  0,  1,  1},
		{10, 10, 11, 11},
		{12, 12, 13, 13}};
	iMove->SetFlyOff(3,fdata2,ip->GetFlyOffTime(),move,FLY_DOWN);

	FlyOffData fdata2a[] = {
		{24, 24, 25, 25},
		{26, 26, 27, 27},
		{28, 28, 29, 29},
		{30, 30, 31, 31}
		};
	iFalloff->SetFlyOff(4,fdata2a,ip->GetFlyOffTime(),falloff,FLY_DOWN);


	FlyOffData fdata2b[] = {
		{32, 32, 32, 32},
		{33, 33, 33, 33},
		};
	iFalloffSpace->SetFlyOff(2,fdata2b,ip->GetFlyOffTime(),falloffSpace,FLY_DOWN);



	FlyOffData fdata3[] = {
		{ 4,  4,  5,  5},
		{ 6,  6,  7,  7},
		{ 8,  8,  9,  9}};
	iScale->SetFlyOff(3,fdata3,ip->GetFlyOffTime(),scale,FLY_DOWN);

	FlyOffData fdata5[] = {
		{ 14,  14,  15,  15},
		{ 16,  16,  17,  17}
		};
	iMirror->SetFlyOff(2,fdata5,ip->GetFlyOffTime(),mirror,FLY_DOWN);

	FlyOffData fdata4[] = {
		{ 6,  6,  7,  7},
		{ 8,  8,  9,  9}};
	iZoomExt->SetFlyOff(2,fdata4,ip->GetFlyOffTime(),zoomext,FLY_UP);

	FlyOffData fdata6[] = {
		{ 6,  6,  7,  7},
		{ 8,  8,  9,  9}};
	iHide->SetFlyOff(2,fdata6,ip->GetFlyOffTime(),hide,FLY_UP);

	FlyOffData fdata7[] = {
		{ 10,  11,  11,  11},
		{ 12,  13,  13,  13}};
	iFreeze->SetFlyOff(2,fdata7,ip->GetFlyOffTime(),hide,FLY_UP);

	FlyOffData fdata8[] = {
		{ 18,  18,  19,  19},
		{ 20,  20,  21,  21}};
	iIncSelected->SetFlyOff(2,fdata8,ip->GetFlyOffTime(),hide,FLY_DOWN);




	iShowMap->SetCheck(showMap);
	if (image) iShowMap->Enable();
	else iShowMap->Disable();


	iSnap->SetCheck(pixelSnap);

	iFilterSelected->SetCheck(filterSelectedFaces);


	SizeDlg();
	SetMode(mode);
	
	for (int i = 0; i<filterMatID.Count();i++)
		{
		char st[20];
		sprintf(st,"%d",filterMatID[i]+1);
		SendMessage(hMatIDs, CB_ADDSTRING , 0, (LPARAM) (TCHAR*) st);
		}
	matid = -1;
	SendMessage(hMatIDs, CB_SETCURSEL, matid+1, 0 );

	}

static void SetWindowYPos(HWND hWnd,int y)
	{
	Rect rect;
	GetClientRectP(hWnd,&rect);
	SetWindowPos(hWnd,NULL,rect.left,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}

static void SetWindowXPos(HWND hWnd,int x)
	{
	Rect rect;
	GetClientRectP(hWnd,&rect);
	SetWindowPos(hWnd,NULL,x,rect.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}

void UnwrapMod::SizeDlg()
	{
	Rect rect;
	GetClientRect(hWnd,&rect);
	MoveWindow(GetDlgItem(hWnd,IDC_UNWARP_TOOLBAR),
		0, 0, 176, TOOL_HEIGHT, TRUE);

	SetWindowXPos(GetDlgItem(hWnd,IDC_UNWRAP_STR),176);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_STR),6);
	SetWindowXPos(GetDlgItem(hWnd,IDC_UNWRAP_STRSPIN),208);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_STRSPIN),6);

	MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_VERTS_TOOLBAR),
		232, 0, 80, TOOL_HEIGHT, TRUE);


	MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_OPTION_TOOLBAR),
		310, 0, 320, TOOL_HEIGHT, TRUE);


/*
	MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_TEXTURE_COMBO),
		480, 4, 100, TOOL_HEIGHT, TRUE);
	MoveWindow(GetDlgItem(hWnd,IDC_MATID_STRING),
		582, 0, 36, TOOL_HEIGHT-4, TRUE);
	MoveWindow(GetDlgItem(hWnd,IDC_MATID_COMBO),
		620, 4, 50, TOOL_HEIGHT, TRUE);
*/

	MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_VIEW),
		2, TOOL_HEIGHT, rect.w()-5, rect.h()-TOOL_HEIGHT-SPINNER_HEIGHT-3,FALSE);

	
	int ys = rect.h()-TOOL_HEIGHT+3;
	int yl = rect.h()-TOOL_HEIGHT+5;
	
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_ULABEL),yl);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_VLABEL),yl);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_WLABEL),yl);

	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_U),ys);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_V),ys);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_W),ys);
	
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_USPIN),ys);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_VSPIN),ys);
	SetWindowYPos(GetDlgItem(hWnd,IDC_UNWRAP_WSPIN),ys);
	int w = rect.w();
	if (rect.w() < 600)
		{
		MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_FILTER_TOOLBAR),
			220, ys-5, 175, TOOL_HEIGHT, TRUE);
		MoveWindow(GetDlgItem(hWnd,IDC_UNWARP_VIEW_TOOLBAR),
			440, ys-5, 120, TOOL_HEIGHT, TRUE);

		}
	else
		{


		MoveWindow(GetDlgItem(hWnd,IDC_UNWRAP_FILTER_TOOLBAR),
			rect.w()-380, ys-5, 175, TOOL_HEIGHT, TRUE);
		MoveWindow(GetDlgItem(hWnd,IDC_UNWARP_VIEW_TOOLBAR),
			rect.w()-123-32, ys-5, 120, TOOL_HEIGHT, TRUE);

		}

	InvalidateRect(hWnd,NULL,TRUE);
	}

Point2 UnwrapMod::UVWToScreen(Point3 pt,float xzoom, float yzoom,int w,int h)
	{	
	int i1, i2;
	GetUVWIndices(i1,i2);
	int tx = (w-int(xzoom))/2;
	int ty = (h-int(yzoom))/2;
	return Point2(pt[i1]*xzoom+xscroll+tx, (float(h)-pt[i2]*yzoom)+yscroll-ty);
	}

void UnwrapMod::ComputeZooms(
		HWND hWnd, float &xzoom, float &yzoom,int &width, int &height)
	{
 	Rect rect;
	GetClientRect(hWnd,&rect);	
	width = rect.w()-1;
	height = rect.h()-1;

	xzoom = zoom*aspect*float(width);
	yzoom = zoom*float(height);
	}


void UnwrapMod::SetMatFilters()
{
if (vertMatIDList.GetSize() != TVMaps.v.Count())
	vertMatIDList.SetSize(TVMaps.v.Count());
if (matid == -1)
	vertMatIDList.SetAll();
else 
	{
	vertMatIDList.ClearAll();
	for (int j = 0; j < TVMaps.f.Count(); j++)
		{
		int pcount = 3;
		if (TVMaps.f[j].flags & FLAG_QUAD) pcount = 4;
		for (int k = 0; k < pcount; k++)
			{
			int index = TVMaps.f[j].t[k];
//			if (filterMatID[matid] == TVMaps.f[j].MatID)
//6-29-99 watje
			if ((matid < filterMatID.Count()) && (filterMatID[matid] == TVMaps.f[j].MatID) && (index < vertMatIDList.GetSize()))
					vertMatIDList.Set(index);
			}
		}
	}

}


int UnwrapMod::IsFaceVisible(int i)
{
if (!(TVMaps.f[i].flags & FLAG_DEAD) )
	{
	if ((filterSelectedFaces==0) || (TVMaps.f[i].flags & FLAG_SELECTED))
		{
		if ((matid == -1) || (matid >= filterMatID.Count()))
			return 1;
		else if (filterMatID[matid] ==TVMaps.f[i].MatID)
			return 1;

		}
	}
return 0;

}

int UnwrapMod::IsVertVisible(int i)
{
if ( (!(TVMaps.v[i].flags & FLAG_DEAD)) && (!(TVMaps.v[i].flags & FLAG_HIDDEN)) )
	{
	if ((filterSelectedFaces==0) || (IsSelected(i) == 1))
		{
		if ((matid == -1)|| (i >= vertMatIDList.GetSize()))
			return 1;
		else 
			{
			if (vertMatIDList[i]) return 1;
/*
			for (int j = 0; j < TVMaps.f.Count(); j++)
				{
				int pcount = 3;
				if (TVMaps.f[j].flags & FLAG_QUAD) pcount = 4;
				for (int k = 0; k < pcount; k++)
					{
					int index = TVMaps.f[j].t[k];
					if (index==i)
						{
						if (filterMatID[matid] ==TVMaps.f[j].MatID)
							return 1;
						}
					}
				}
*/
			}
		}
	}
return 0;

}


void UnwrapMod::PaintView()
	{	
	PAINTSTRUCT		ps;
	BeginPaint(hView,&ps);
	EndPaint(hView,&ps);
	TimeValue t = ip->GetTime();

	COLORREF frozenColor = RGB(64,64,64);
	COLORREF yellowColor = RGB(255,255,64);
	COLORREF darkyellowColor = RGB(117,117,28);
	COLORREF darkgreenColor = RGB(28,117,28);
	COLORREF greenColor = RGB(0,255,0);
	COLORREF blueColor = RGB(0,0,255);


	SetupTypeins();
	
	if (!viewValid) {
		if (!image && map[CurrentMap]) SetupImage();
		viewValid = TRUE;
		Point2 pt[4];
		float xzoom, yzoom;
		int width,height;
		ComputeZooms(hView,xzoom,yzoom,width,height);
		iBuf->Erase();
		HDC hdc = iBuf->GetDC();
		int i1, i2;
		GetUVWIndices(i1,i2);

		if (image && showMap) {						
			Point3 p(0,0,0);
			p[i2] = 1.0f;
			Point2 sp;
			sp = UVWToScreen(p,xzoom,yzoom,width,height);
			Rect dest;
			dest.left   = (int)sp.x;
			dest.top    = (int)sp.y;
			dest.right  = dest.left + int(xzoom)-1;
			dest.bottom = dest.top + int(yzoom)-1;
			Rect src;
			src.left   = src.top = 0;
			src.right  = iw-1;
			src.bottom = ih-1;
//			GetGPort()->DisplayMap(hdc, dest, src, image, ByteWidth(iw*3));
			GetGPort()->DisplayMap(hdc, dest, src, image, ByteWidth(iw));
		} else {
			HPEN gPen = CreatePen(PS_SOLID,2,RGB(100,100,100));
			Point3 p1(0,0,0),p2(0,0,0);
			Point2 sp1, sp2;
			p2[i1] = 1.0f;
			p2[i2] = 1.0f;
			sp1 = UVWToScreen(p1,xzoom,yzoom,width,height);
			sp2 = UVWToScreen(p2,xzoom,yzoom,width,height);			
			SelectObject(hdc,gPen);
			Rectangle(hdc,(int)sp1.x,(int)sp2.y,(int)sp2.x,(int)sp1.y);
			SelectObject(hdc,GetStockObject(WHITE_PEN));
			DeleteObject(gPen);
			}

		// Paint faces		
		HPEN selPen   = CreatePen(PS_SOLID,2,selColor);
		HPEN unselPen = CreatePen(PS_SOLID,0,lineColor);
		HPEN frozenPen = CreatePen(PS_SOLID,0,frozenColor);
		HPEN yellowPen = CreatePen(PS_SOLID,0,yellowColor);
		HPEN darkyellowPen = CreatePen(PS_SOLID,0,darkyellowColor);
		HPEN darkgreenPen = CreatePen(PS_SOLID,0,darkgreenColor);
		HPEN greenPen = CreatePen(PS_SOLID,0,greenColor);
		HPEN bluePen = CreatePen(PS_SOLID,0,blueColor);
		
		SelectObject(hdc,unselPen);
//		for (int i=0; i<tvFace.Count(); i++) {
		for (int i=0; i<TVMaps.f.Count(); i++) {
			// Grap the three points, xformed
			BOOL hidden = FALSE;
			if (IsFaceVisible(i))
				{
				int pcount = 3;
				if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;

				for (int j=0; j<pcount; j++) {
					int index = TVMaps.f[i].t[j];
					pt[j] = UVWToScreen(GetPoint(t,index),xzoom,yzoom,width,height);
					if (TVMaps.v[index].flags & FLAG_HIDDEN) hidden = TRUE;
					}
			// Now draw the face
				if (!hidden)
					{
					MoveToEx(hdc,(int)pt[0].x, (int)pt[0].y, NULL);
					for (j=0; j<pcount; j++) {
						LineTo(hdc,(int)pt[(j+1)%pcount].x, (int)pt[(j+1)%pcount].y);
						}
					}
					
				}
			}

		// Now paint points		
		Point3 selSoft = GetUIColor(COLOR_SUBSELECTION_SOFT);
		Point3 selMedium = GetUIColor(COLOR_SUBSELECTION_MEDIUM);
		Point3 selHard = GetUIColor(COLOR_SUBSELECTION_HARD);

		IsSelectedSetup();
		for (i=0; i<TVMaps.v.Count(); i++) {
			if (IsVertVisible(i))
/*
			if ( (!(TVMaps.v[i].flags & FLAG_DEAD)) && (!(TVMaps.v[i].flags & FLAG_HIDDEN)) )
				{
				if ((filterSelectedFaces==0) || (IsSelected(i) == 1))
					{
*/
				{
				Point3 selColor(0.0f,0.0f,0.0f);

				pt[0] = UVWToScreen(GetPoint(t,i),xzoom,yzoom,width,height);
/*
				if (TVMaps.v[i].flags & FLAG_FROZEN)
					SelectObject(hdc,frozenPen);
				else if (vsel[i]) SelectObject(hdc,selPen);
				else
*/
				{ 


					if (TVMaps.v[i].influence == 0.0f)
						{
//						delColor = 
//						SelectObject(hdc,unselPen);
						}
					else if (TVMaps.v[i].influence <0.5f)
						selColor = selSoft + ( (selMedium-selSoft) * (TVMaps.v[i].influence/0.5f));
					else if (TVMaps.v[i].influence<=1.0f)
						selColor = selMedium + ( (selHard-selMedium) * ((TVMaps.v[i].influence-0.5f)/0.5f));
/*
					else if (TVMaps.v[i].influence <0.333)
						SelectObject(hdc,bluePen);
					else if (TVMaps.v[i].influence<0.666)
						SelectObject(hdc,greenPen);
					else if (TVMaps.v[i].influence<1.0)
						SelectObject(hdc,yellowPen);
*/

					}
				COLORREF inflColor;
				inflColor = RGB((int)(selColor.x * 255.f), (int)(selColor.y * 255.f),(int)(selColor.z * 255.f));

				HPEN inflPen = CreatePen(PS_SOLID,0,inflColor);
				if (TVMaps.v[i].flags & FLAG_FROZEN)
					SelectObject(hdc,frozenPen);
				else if (vsel[i]) SelectObject(hdc,selPen);
				else if (TVMaps.v[i].influence == 0.0f)
					SelectObject(hdc,unselPen);
				else SelectObject(hdc,inflPen);

				Rectangle(hdc,
					(int)pt[0].x-3,(int)pt[0].y-3,
					(int)pt[0].x+3,(int)pt[0].y+3);					
				SelectObject(hdc,GetStockObject(BLACK_PEN));
				DeleteObject(inflPen);

				}
			}

//check if planar if so draw planar gizmo in dialog
		if ( (ip && (ip->GetSubObjectLevel() == 3) ))
			{
// draw gizmo
			Point3 pt0,pt1,pt2,pt3,pt4,pt5,pt6;
			

			pt0.x = -.5f;
			pt0.y = -.5f;
			pt0.z = 0.0f;

			pt1.x = .5f;
			pt1.y = -.5f;
			pt1.z = 0.0f;

			pt2.x = .5f;
			pt2.y = .5f;
			pt2.z = 0.0f;

			pt3.x = -0.5f;
			pt3.y = 0.5f;
			pt3.z = 0.0f;

			pt4.x = 0.0f;
			pt4.y = 0.0f;
			pt4.z = 0.0f;

			pt5.x = 0.0f;
			pt5.y = 0.5f;
			pt5.z = 0.0f;

			pt6.x = 0.0f;
			pt6.y = 0.75f;
			pt6.z = 0.0f;



			PlanarTM.IdentityMatrix();
			Interval v;
			scaleControl->GetValue(0,&gScale,v);
			PlanarTM.SetScale(gScale);
			rotateControl->GetValue(0,&gRotate,v);
			PlanarTM.RotateZ(gRotate);
			offsetControl->GetValue(0,&gOffset,v);
			PlanarTM.SetTrans(gOffset);
			
			pt1 = pt1 * PlanarTM;

			pt2 = pt2 * PlanarTM;
			pt3 = pt3 * PlanarTM;
			pt4 = pt4 * PlanarTM;
			pt0 = pt0 * PlanarTM;
			pt5 = pt5 * PlanarTM;
			pt6 = pt6 * PlanarTM;


			Point2 spt[7];
			spt[0] = UVWToScreen(pt0,xzoom,yzoom,width,height);
			spt[1] = UVWToScreen(pt1,xzoom,yzoom,width,height);
			spt[2] = UVWToScreen(pt2,xzoom,yzoom,width,height);
			spt[3] = UVWToScreen(pt3,xzoom,yzoom,width,height);
			spt[4] = UVWToScreen(pt4,xzoom,yzoom,width,height);
			spt[5] = UVWToScreen(pt5,xzoom,yzoom,width,height);
			spt[6] = UVWToScreen(pt6,xzoom,yzoom,width,height);

			SelectObject(hdc,yellowPen);

			MoveToEx(hdc,(int)spt[5].x, (int)spt[5].y, NULL);
			LineTo(hdc,(int)spt[6].x, (int)spt[6].y);
			MoveToEx(hdc,(int)spt[0].x, (int)spt[0].y, NULL);
			LineTo(hdc,(int)spt[1].x, (int)spt[1].y);
			SelectObject(hdc,darkgreenPen);
			LineTo(hdc,(int)spt[2].x, (int)spt[2].y);


			SelectObject(hdc,yellowPen);

			LineTo(hdc,(int)spt[3].x, (int)spt[3].y);

			LineTo(hdc,(int)spt[0].x, (int)spt[0].y);

			Rectangle(hdc,
					(int)spt[0].x-3,(int)spt[0].y-3,
					(int)spt[0].x+3,(int)spt[0].y+3);					

			Rectangle(hdc,
					(int)spt[1].x-3,(int)spt[1].y-3,
					(int)spt[1].x+3,(int)spt[1].y+3);					
			Rectangle(hdc,
					(int)spt[2].x-3,(int)spt[2].y-3,
					(int)spt[2].x+3,(int)spt[2].y+3);					
			Rectangle(hdc,
					(int)spt[3].x-3,(int)spt[3].y-3,
					(int)spt[3].x+3,(int)spt[3].y+3);					


			Rectangle(hdc,
					(int)spt[4].x-4,(int)spt[4].y-4,
					(int)spt[4].x+4,(int)spt[4].y+4);					
//do selecetd verts in plane
			SelectObject(hdc,darkyellowPen);
			for (i = 0; i < gverts.d.Count();i++)
				{
				Point2 gv;
				Point3 gv3;
				gv3 = gverts.d[i].p;
//				gv3 .x -=0.5;
//				gv3 .y -=0.5;
				gv3 = gv3*0.5f;
				gv3 = gv3 * PlanarTM;
				gv = UVWToScreen(gv3,xzoom,yzoom,width,height);

				Rectangle(hdc,
					(int)gv.x-2,(int)gv.y-2,
					(int)gv.x+2,(int)gv.y+2);					

				}
			for (i=0; i<gfaces.Count(); i++) {
			// Grap the three points, xformed
				int pcount = 3;
				if (gfaces[i].flags & FLAG_QUAD) pcount = 4;

				for (int j=0; j<pcount; j++) {
					int index = gfaces[i].t[j];
					Point3 temp_point;
					temp_point = gverts.d[index].p;
					temp_point = temp_point * 0.5f;
					temp_point = temp_point *PlanarTM;
					pt[j] = UVWToScreen(temp_point,xzoom,yzoom,width,height);
					}
			// Now draw the face
				MoveToEx(hdc,(int)pt[0].x, (int)pt[0].y, NULL);
				for (j=0; j<pcount; j++) 
					{
					LineTo(hdc,(int)pt[(j+1)%pcount].x, (int)pt[(j+1)%pcount].y);
					}
				}




			}

		SelectObject(hdc,GetStockObject(BLACK_PEN));
		DeleteObject(selPen);
		DeleteObject(yellowPen);		
		DeleteObject(darkyellowPen);		
		DeleteObject(darkgreenPen);		
		DeleteObject(greenPen);		
		DeleteObject(bluePen);		
		DeleteObject(frozenPen);		
		DeleteObject(unselPen);		
		}		
	iBuf->Blit();
	}

void UnwrapMod::InvalidateView()
	{
	InvalidateTypeins();
	viewValid = FALSE;
	if (hView) {
		InvalidateRect(hView,NULL,TRUE);
		}
	}

void UnwrapMod::SetMode(int m)
	{
	switch (mode) {
		case ID_TOOL_MOVE:
		case ID_UNWRAP_MOVE:
			iMove->SetCheck(FALSE);   break;
		case ID_UNWRAP_ROTATE:
		case ID_TOOL_ROTATE:  iRot->SetCheck(FALSE);    break;
		case ID_UNWRAP_SCALE:
		case ID_TOOL_SCALE:   iScale->SetCheck(FALSE);  break;
		case ID_UNWRAP_PAN:
		case ID_TOOL_PAN:     iPan->SetCheck(FALSE);    break;
		case ID_UNWRAP_ZOOM:
		case ID_TOOL_ZOOM:    iZoom->SetCheck(FALSE);   break;
		case ID_UNWRAP_ZOOMREGION:
		case ID_TOOL_ZOOMREG: iZoomReg->SetCheck(FALSE);break;
		case ID_UNWRAP_WELD:
		case ID_TOOL_WELD:	  iWeld->SetCheck(FALSE);break;
		}

	oldMode = mode;
	mode = m;

	switch (mode) {
		case ID_UNWRAP_MOVE:   
		case ID_TOOL_MOVE:   
			iMove->SetCheck(TRUE);  
			mouseMan.SetMouseProc(moveMode, LEFT_BUTTON);
			break;

		case ID_UNWRAP_ROTATE:   
		case ID_TOOL_ROTATE: 
			iRot->SetCheck(TRUE);   
			mouseMan.SetMouseProc(rotMode, LEFT_BUTTON);
			break;

		case ID_UNWRAP_SCALE:   
		case ID_TOOL_SCALE:  
			iScale->SetCheck(TRUE); 
			mouseMan.SetMouseProc(scaleMode, LEFT_BUTTON);
			break;
		case ID_UNWRAP_WELD:
		case ID_TOOL_WELD:   
			iWeld->SetCheck(TRUE);  
			mouseMan.SetMouseProc(weldMode, LEFT_BUTTON);
			break;


		case ID_UNWRAP_PAN:
		case ID_TOOL_PAN:    
			iPan->SetCheck(TRUE);   
			mouseMan.SetMouseProc(panMode, LEFT_BUTTON);
			break;

		case ID_UNWRAP_ZOOM:
		case ID_TOOL_ZOOM:   
			iZoom->SetCheck(TRUE);
			mouseMan.SetMouseProc(zoomMode, LEFT_BUTTON);
			break;



		case ID_UNWRAP_ZOOMREGION:
		case ID_TOOL_ZOOMREG:
			iZoomReg->SetCheck(TRUE);
			mouseMan.SetMouseProc(zoomRegMode, LEFT_BUTTON);
			break;
		}
	}

void UnwrapMod::RegisterClasses()
	{
	if (!hToolImages) {
		HBITMAP hBitmap, hMask;	
		hToolImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_TRANSFORM));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_TRANSFORM_MASK));
		ImageList_Add(hToolImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}

	if (!hOptionImages) {
		HBITMAP hBitmap, hMask;	
		hOptionImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_OPTION));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_OPTION_MASK));
		ImageList_Add(hOptionImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}

	if (!hViewImages) {
		HBITMAP hBitmap, hMask;	
		hViewImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_VIEW));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_VIEW_MASK));
		ImageList_Add(hViewImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}

	if (!hVertexImages) {
		HBITMAP hBitmap, hMask;	
		hVertexImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_VERT));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UNWRAP_VERT_MASK));
		ImageList_Add(hVertexImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}

	static BOOL registered = FALSE;
	if (!registered) {
		registered = TRUE;
		WNDCLASS  wc;
		wc.style         = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL; //(HBRUSH)GetStockObject(WHITE_BRUSH);	
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = UnwrapViewProc;
		wc.lpszClassName = _T(GetString(IDS_PW_UNWRAPVIEW));
		RegisterClass(&wc);
		}
	}

//static int lStart[12] = {0,1,3,2,4,5,7,6,0,1,2,3};
//static int lEnd[12]   = {1,3,2,0,5,7,6,4,4,5,6,7};
/*
static void DoBoxIcon(BOOL sel,float length, PolyLineProc& lp)
	{
	Point3 pt[3];
	
	length *= 0.5f;
	Box3 box;
	box.pmin = Point3(-length,-length,-length);
	box.pmax = Point3( length, length, length);

	if (sel) //lp.SetLineColor(1.0f,1.0f,0.0f);
		 lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else //lp.SetLineColor(0.85f,0.5f,0.0f);		
		 lp.SetLineColor(GetUIColor(COLOR_GIZMOS));

	for (int i=0; i<12; i++) {
		pt[0] = box[lStart[i]];
		pt[1] = box[lEnd[i]];
		lp.proc(pt,2);
		}
	}

*/

BOOL UnwrapMod::HitTest(Rect rect,Tab<int> &hits,BOOL selOnly)
	{
	Point2 pt;
	float xzoom, yzoom;
	int width,height;
	TimeValue t = ip->GetTime();
	ComputeZooms(hView,xzoom,yzoom,width,height);	

	if ( (abs(rect.left-rect.right) <= 4) && (abs(rect.bottom-rect.top) <= 4) )
		{
		rect.left -= 4;
		rect.right += 4;
		rect.top -= 4;
		rect.bottom += 4;
		}

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (selOnly && !vsel[i]) continue;
		if (TVMaps.v[i].flags & FLAG_HIDDEN) continue;
		if (TVMaps.v[i].flags & FLAG_FROZEN) continue;
		if (!IsVertVisible(i)) continue;

		pt = UVWToScreen(GetPoint(t,i),xzoom,yzoom,width,height);
		IPoint2 ipt(int(pt.x),int(pt.y));
		if (rect.Contains(ipt)) {
			hits.Append(1,&i,10);
			}
		}
	return hits.Count();
	}

void UnwrapMod::InvalidateTypeins()
	{
	typeInsValid = FALSE;	
	}

void UnwrapMod::SetupTypeins()
	{
	if (typeInsValid) return;
	typeInsValid = TRUE;

	Point3 uv(0,0,0);
	BOOL found = FALSE;
	BOOL u = TRUE, v = TRUE, w = TRUE;

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (!vsel[i]) continue;
		
		if (found) {
			if (uv.x!=TVMaps.v[i].p.x) {
				u = FALSE;				
				}
			if (uv.y!=TVMaps.v[i].p.y) {
				v = FALSE;				
				}
			if (uv.z!=TVMaps.v[i].p.z) {
				w = FALSE;				
				}			
		} else {
			uv = TVMaps.v[i].p;
			found = TRUE;
			}
		}

	if (!found) {
		iU->Disable();
		iV->Disable();
		iW->Disable();
	} else {
		iU->Enable();
		iV->Enable();
		iW->Enable();
		if (u) {
			iU->SetIndeterminate(FALSE);
			iU->SetValue(uv.x,FALSE);
		} else {
			iU->SetIndeterminate(TRUE);
			}

		if (v) {
			iV->SetIndeterminate(FALSE);
			iV->SetValue(uv.y,FALSE);
		} else {
			iV->SetIndeterminate(TRUE);
			}

		if (w) {
			iW->SetIndeterminate(FALSE);
			iW->SetValue(uv.z,FALSE);
		} else {
			iW->SetIndeterminate(TRUE);
			}
		}
	}

void UnwrapMod::Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all)
	{
	if ( (ip && (ip->GetSubObjectLevel() == 3) ))
		{
		GizmoSelected = FALSE;
		for (int i=0; i<hits.Count(); i++) 
			{
			if (hits[i]!= 0)
				GizmoSelected = TRUE;
			}

		}
	else
		{
		HoldSelection();
		IsSelectedSetup();
		for (int i=0; i<hits.Count(); i++) {
			if ( (IsVertVisible(hits[i])) &&  (!(TVMaps.v[hits[i]].flags & FLAG_FROZEN)) )
				{
/*			if ( (!(TVMaps.v[hits[i]].flags & FLAG_DEAD)) && 
				 (!(TVMaps.v[hits[i]].flags & FLAG_HIDDEN)) && 
				 (!(TVMaps.v[hits[i]].flags & FLAG_FROZEN))   )
				{
				if ((filterSelectedFaces==0) || (IsSelected(hits[i]) == 1))
*/
//					{

					if (toggle) vsel.Set(hits[i],!vsel[hits[i]]);
					else if (subtract) vsel.Set(hits[i],FALSE);
					else vsel.Set(hits[i],TRUE);
					if (!all) break;
//					}
				}
			}	
		RebuildDistCache();
		}
	}

void UnwrapMod::ClearSelect()
	{
	HoldSelection();
	vsel.ClearAll();	
	for (int i=0; i<TVMaps.v.Count(); i++) 
		{
		TVMaps.v[i].influence = 0.0f;
		}

	}

void UnwrapMod::HoldPoints()
	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {
		SetAFlag(A_HELD);
		theHold.Put(new TVertRestore(this));
		}
	}


void UnwrapMod::HoldSelection()
	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {	
		SetAFlag(A_HELD);
		theHold.Put(new TSelRestore(this));
		}
	}

void UnwrapMod::HoldPointsAndFaces()
	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {
		SetAFlag(A_HELD);
		theHold.Put(new TVertAndTFaceRestore(this));
		}
	}


void UnwrapMod::TypeInChanged(int which)
	{
	TimeValue t = ip->GetTime();
	HoldPoints();
	Point3 uvw;
	uvw[0] = iU->GetFVal();
	uvw[1] = iV->GetFVal();
	uvw[2] = iW->GetFVal();

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
			TVMaps.v[i].p[which] = uvw[which];
			if (TVMaps.cont[i]) TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		}

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	ip->RedrawViews(ip->GetTime());
	}

void UnwrapMod::DeleteSelected()
{
if (TVMaps.v.Count() == 0) return;
theHold.SuperBegin();
theHold.Begin();

HoldPointsAndFaces();	
	
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( (vsel[i]) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
		{
		TVMaps.v[i].flags |= FLAG_DEAD;
		vsel.Set(i,FALSE);
		}
	}
for (i=0; i<TVMaps.f.Count(); i++) 
	{
	for (int j=0; j<3; j++) 
		{
		int index = TVMaps.f[i].t[j];
		if (TVMaps.v[index].flags & FLAG_DEAD)
			{
			TVMaps.f[i].flags |= FLAG_DEAD;
			}
		}
	}
// loop through faces all
theHold.Accept(_T(GetString(IDS_PW_DELETE_SELECTED)));
theHold.SuperAccept(_T(GetString(IDS_PW_DELETE_SELECTED)));

}

void UnwrapMod::HideSelected()
{

theHold.SuperBegin();
theHold.Begin();
HoldPoints();	
	
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( (vsel[i]) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
		{
	
		TVMaps.v[i].flags |= FLAG_HIDDEN;
		vsel.Set(i,FALSE);
		}
	}

theHold.Accept(_T(GetString(IDS_PW_HIDE_SELECTED)));
theHold.SuperAccept(_T(GetString(IDS_PW_HIDE_SELECTED)));
}

void UnwrapMod::UnHideAll()
{
theHold.SuperBegin();
theHold.Begin();
HoldPoints();	
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( (TVMaps.v[i].flags & FLAG_HIDDEN) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
		{
		TVMaps.v[i].flags -= FLAG_HIDDEN;
		}
	}

theHold.Accept(_T(GetString(IDS_PW_UNHIDEALL)));
theHold.SuperAccept(_T(GetString(IDS_PW_UNHIDEALL)));


}


void UnwrapMod::FreezeSelected()
{
theHold.SuperBegin();
theHold.Begin();
HoldPoints();	
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( (vsel[i]) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
		{
		TVMaps.v[i].flags |= FLAG_FROZEN;
		vsel.Set(i,FALSE);
		}
	}
theHold.Accept(_T(GetString(IDS_PW_FREEZE_SELECTED)));
theHold.SuperAccept(_T(GetString(IDS_PW_FREEZE_SELECTED)));


}

void UnwrapMod::UnFreezeAll()
{
theHold.SuperBegin();
theHold.Begin();
HoldPoints();	
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( !(TVMaps.v[i].flags & FLAG_DEAD)) 
		{
		if ( (TVMaps.v[i].flags & FLAG_FROZEN)) 
			TVMaps.v[i].flags -= FLAG_FROZEN;
		}
	}
theHold.Accept(_T(GetString(IDS_PW_UNFREEZEALL)));
theHold.SuperAccept(_T(GetString(IDS_PW_UNFREEZEALL)));

}

void UnwrapMod::WeldSelected()
{

theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	
float sweldThreshold;

sweldThreshold = weldThreshold * weldThreshold;

for (int m=0; m<TVMaps.v.Count(); m++) 
	{
	if (vsel[m])
		{
		Point3 p(0.0f,0.0f,0.0f);
		Point3 op(0.0f,0.0f,0.0f);
		p = GetPoint(ip->GetTime(),m);
		op = p;
		int ct = 0;
		int index = -1;
		for (int i=m+1; i<TVMaps.v.Count(); i++) 
			{
			if ( (vsel[i]) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
//	if (vsel[i]) 
				{
				Point3 np;
				
				np= GetPoint(ip->GetTime(),i);
				if (LengthSquared(np-op) < sweldThreshold)
					{
					p+= np;
					ct++;
					if (index == -1)
						index = m;
					TVMaps.v[i].flags |= FLAG_DEAD;
					}
				}
			}

		if ((index == -1) || (ct == 0))
			{
//			theHold.SuperCancel();
//			theHold.Cancel();
//			return;
			}
		else
			{
			ct++;
			p = p /(float)ct;


			if (TVMaps.cont[index]) 
				TVMaps.cont[index]->GetValue(ip->GetTime(),&TVMaps.v[index].p,FOREVER);

			TVMaps.v[index].p = p;
			if (TVMaps.cont[index]) 
				TVMaps.cont[index]->SetValue(ip->GetTime(),&TVMaps.v[index].p);

			for (i=0; i<TVMaps.f.Count(); i++) 
				{
				int pcount = 3;
				if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
				for (int j=0; j<pcount; j++) 
					{
					int tvfIndex = TVMaps.f[i].t[j];
					Point3 np = TVMaps.v[tvfIndex].p;
					 

					if ((vsel[tvfIndex]) && (LengthSquared(np-op) < sweldThreshold)) 
						{
						if (tvfIndex != index)
							{
							TVMaps.f[i].t[j] = index;
//							vsel.Set(tvfIndex,0);
							}

						}
					}
				}
			}
		}
	}
theHold.Accept(_T(GetString(IDS_PW_WELDSELECTED)));
theHold.SuperAccept(_T(GetString(IDS_PW_WELDSELECTED)));

}

BOOL UnwrapMod::WeldPoints(HWND h, IPoint2 m)
{
//theHold.SuperBegin();
//theHold.Begin();
theHold.Put(new TVertAndTFaceRestore(this));
//HoldPointsAndFaces();	

Point3 p(0.0f,0.0f,0.0f);
int ct = 0;
Point2 mp;
mp.x = (float) m.x;
mp.y = (float) m.y;

float xzoom, yzoom;
int width,height;
ComputeZooms(h,xzoom,yzoom,width,height);
int index = -1;
BOOL holdNeeded = FALSE;
for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if (!vsel[i]) 
		{
		Point2 sp;
		p = GetPoint(ip->GetTime(),i);
		sp = UVWToScreen(p,xzoom,yzoom,width,height);
		if (Length(sp-mp) < 4.0f)
			{
			index = i;
			i = TVMaps.v.Count();
			}

		}
	}

BOOL first = TRUE;
if (index != -1)
	{
	for (i=0; i<TVMaps.f.Count(); i++) 
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		for (int j=0; j<pcount; j++) 
			{
			int tvfIndex = TVMaps.f[i].t[j];
			if (vsel[tvfIndex])
				{
				TVMaps.f[i].t[j] = index;
				TVMaps.v[tvfIndex].flags |= FLAG_DEAD;
				holdNeeded = TRUE;
				}
			}
		}
	
	}
theHold.Accept(_T(GetString(IDS_PW_WELD)));

InvalidateView();

return holdNeeded;
}

void UnwrapMod::BreakSelected()
{

theHold.SuperBegin();
theHold.Begin();
HoldPointsAndFaces();	

for (int i=0; i<TVMaps.v.Count(); i++) 
	{
	if ( (vsel[i]) && (!(TVMaps.v[i].flags & FLAG_DEAD)) )
		{
//find all faces attached to this vertex
		Point3 p = GetPoint(ip->GetTime(),i);
		BOOL first = TRUE;
		for (int j=0; j<TVMaps.f.Count(); j++) 
			{
			int pcount = 3;
			if (TVMaps.f[j].flags & FLAG_QUAD) pcount = 4;
			for (int k = 0; k < pcount;k++)
				{
				if ((TVMaps.f[j].t[k] == i) && (!(TVMaps.f[j].flags & FLAG_DEAD)))
					{
					if (first)
						{
						first = FALSE;
						}
					else
						{
//loop through vertex list looking for dead ones else attache to end
						int found = -1;
						for (int m= 0; m <TVMaps.v.Count();m++)
							{
							if (TVMaps.v[m].flags & FLAG_DEAD)
								{
								found =m;
								m = TVMaps.v.Count();
								}
							}
//found dead spot add to it
						if (found != -1)
							{
							TVMaps.v[found].p = p;
							if (TVMaps.cont[found]) 
								TVMaps.cont[found]->SetValue(ip->GetTime(),&TVMaps.v[found].p);

							TVMaps.f[j].t[k] = found;
							TVMaps.v[found].flags -= FLAG_DEAD;
							}
//create a new vert
						else
							{
							UVW_TVVertClass tv;
							tv.p = p;
							tv.flags = 0;
							TVMaps.v.Append(1,&tv,1);
							TVMaps.f[j].t[k] = TVMaps.v.Count()-1;

							vsel.SetSize(TVMaps.v.Count(), 1);
							Control* c;
							c = NULL;
							TVMaps.cont.Append(1,&c,1);
	
							}
						}
					}

				}
			}

		}
	}
theHold.Accept(_T(GetString(IDS_PW_BREAK)));
theHold.SuperAccept(_T(GetString(IDS_PW_BREAK)));


}

void UnwrapMod::SnapPoint(Point3 &p)
	{
	int i1,i2;
	GetUVWIndices(i1,i2);
//	int ix, iy;
	float fx,fy;
	double dx,dy;
//compute in pixel space
//find closest whole pixel
	fx = (float) modf(( (float) p[i1] * (float) (bitmapWidth-1) ),&dx);
	fy = (float) modf(( (float) p[i2] * (float) (bitmapHeight-1) ),&dy);
	if (midPixelSnap)
		{
//		if (fx > 0.5f) dx+=1.0f;
//		if (fy > 0.5f) dy+=1.0f;
		dx += 0.5f;
		dy += 0.5f;
		}
	else
		{
		if (fx > 0.5f) dx+=1.0f;
		if (fy > 0.5f) dy+=1.0f;
		}
//put back in UVW space
	p[i1] = (float)dx/(float)(bitmapWidth-1);
	p[i2] = (float)dy/(float)(bitmapHeight-1);
	}
void UnwrapMod::MovePoints(Point2 pt)
	{
	int i1, i2;
	GetUVWIndices(i1,i2);
	HoldPoints();	
	TimeValue t = ip->GetTime();

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			TVMaps.v[i].p[i1] += pt.x;
			TVMaps.v[i].p[i2] += pt.y;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}
			if (TVMaps.cont[i]) TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		else if(TVMaps.v[i].influence != 0.0f)
			{
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			Point3 NewPoint = TVMaps.v[i].p;
			NewPoint[i1] += pt.x;
			NewPoint[i2] += pt.y;
			Point3 vec;
			vec = (NewPoint - TVMaps.v[i].p) * TVMaps.v[i].influence;
			TVMaps.v[i].p += vec;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}
			if (TVMaps.cont[i]) TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}

		}
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}

void UnwrapMod::MoveGizmo(Point2 pt)
	{
	int i1, i2;
	GetUVWIndices(i1,i2);


	HoldPoints();	
	TimeValue t = ip->GetTime();

	if (offsetControl) 
		offsetControl->GetValue(t,&gOffset,FOREVER);
	gOffset[i1] += pt.x;
	gOffset[i2] += pt.y;
	if (offsetControl) offsetControl->SetValue(t,&gOffset);

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}

void UnwrapMod::RotatePoints(HWND h, float ang)
	{
	HoldPoints();	
	TimeValue t = ip->GetTime();

	Point3 cent(0,0,0);

	if (centeron)
		{
		float xzoom, yzoom;
		int width,height;

		ComputeZooms(h,xzoom,yzoom,width,height);


		int tx = (width-int(xzoom))/2;
		int ty = (height-int(yzoom))/2;
		cent.x = (center.x-tx-xscroll)/xzoom;
		cent.y = (center.y+ty-yscroll - height)/-yzoom;
		cent.z = 0.0f;
//		cent.x = 0.0f(center.x-tx-xscroll)/xzoom;
//		cent.y = 0.0f;//(center.y+ty-yscroll - height)/-yzoom;
		}
	else
		{

		int ct = 0;
		for (int i=0; i<TVMaps.v.Count(); i++) {
			if (vsel[i]) {
				cent += TVMaps.v[i].p;
				ct++;
				}
			}
		if (!ct) return;
		cent /= float(ct);
		}

	Matrix3 mat(1);	
	mat.Translate(-cent);
	ang = ip->SnapAngle(ang,FALSE);
	switch (uvw) {
		case 0: mat.RotateZ(ang); break;
		case 1: mat.RotateX(ang); break;
		case 2: mat.RotateY(ang); break;
		}
	mat.Translate(cent);

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			TVMaps.v[i].p = mat * TVMaps.v[i].p;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		else if(TVMaps.v[i].influence != 0.0f)
			{
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			Point3 NewPoint = TVMaps.v[i].p;
			NewPoint = mat * TVMaps.v[i].p;

			Point3 vec;
			vec = (NewPoint - TVMaps.v[i].p) * TVMaps.v[i].influence;
			TVMaps.v[i].p += vec;

			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}

		}

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}

void UnwrapMod::RotateGizmo(HWND h, float ang)
	{
	HoldPoints();	
	TimeValue t = ip->GetTime();

	Point3 cent(0,0,0);

	if (centeron)
		{
		float xzoom, yzoom;
		int width,height;

		ComputeZooms(h,xzoom,yzoom,width,height);


		int tx = (width-int(xzoom))/2;
		int ty = (height-int(yzoom))/2;
		cent.x = (center.x-tx-xscroll)/xzoom;
		cent.y = (center.y+ty-yscroll - height)/-yzoom;
		cent.z = 0.0f;
//		cent.x = 0.0f(center.x-tx-xscroll)/xzoom;
//		cent.y = 0.0f;//(center.y+ty-yscroll - height)/-yzoom;
		}
	else
		{
		cent = gOffset;
/*
		int ct = 0;
		for (int i=0; i<TVMaps.v.Count(); i++) {
			if (vsel[i]) {
				cent += TVMaps.v[i].p;
				ct++;
				}
			}
		if (!ct) return;
		cent /= float(ct);
*/
		}

	if (rotateControl) 
		rotateControl->GetValue(t,&gRotate,FOREVER);
	gRotate += ang;

	if (rotateControl) 
		rotateControl->SetValue(t,&gRotate);

/*
	Matrix3 mat(1);	
	mat.Translate(-cent);
	switch (uvw) {
		case 0: mat.RotateZ(ang); break;
		case 1: mat.RotateX(ang); break;
		case 2: mat.RotateY(ang); break;
		}
	mat.Translate(cent);

	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			TVMaps.v[i].p = mat * TVMaps.v[i].p;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		}
*/
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}


void UnwrapMod::ScaleGizmo(HWND h, float scale, int direction)
	{
	HoldPoints();	
	TimeValue t = ip->GetTime();

	Point3 cent(0,0,0);
	scale = scale - 1.0f;
	if (centeron)
		{
		float xzoom, yzoom;
		int width,height;

		ComputeZooms(h,xzoom,yzoom,width,height);

		int tx = (width-int(xzoom))/2;
		int ty = (height-int(yzoom))/2;
		cent.x = (center.x-tx-xscroll)/xzoom;
		cent.y = (center.y+ty-yscroll - height)/-yzoom;
		cent.z = 0.0f;
		}
	else
		{
		cent = gOffset;
/*
		int ct = 0;
		for (int i=0; i<TVMaps.v.Count(); i++) {
			if (vsel[i]) {
				cent += TVMaps.v[i].p;
				ct++;
				}
			}
		if (!ct) return;
		cent /= float(ct);
*/
		}

	Matrix3 mat(1);	
	mat.Translate(-cent);
	Point3 sc(0.0f,0.0f,0.0f);
	int i1, i2;
	GetUVWIndices(i1,i2);
	if (direction == 0)
		{
		sc[i1] = scale;
		sc[i2] = scale;
		}
	else if (direction == 1)
		{
		sc[i1] = -scale;
//		sc[i2] = scale;
		}
	else if (direction == 2)
		{
//		sc[i1] = scale;
		sc[i2] = scale;
		}

//	sc[i1] = scale;
//	sc[i2] = scale;

	mat.Scale(sc,TRUE);
	mat.Translate(cent);
	if (scaleControl) 
		scaleControl->GetValue(t,&gScale,FOREVER);
	gScale += sc;

	if (scaleControl) 
		scaleControl->SetValue(t,&gScale);

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}


void UnwrapMod::ScalePoints(HWND h, float scale, int direction)
	{
	HoldPoints();	
	TimeValue t = ip->GetTime();

	Point3 cent(0,0,0);
	int i;
	if (centeron)
		{
		float xzoom, yzoom;
		int width,height;

		ComputeZooms(h,xzoom,yzoom,width,height);
/*
Rect rect;
GetClientRect(hWnd,&rect);	
int w = rect.w()-1;
int h = height = rect.h()-1;
*/

		int tx = (width-int(xzoom))/2;
		int ty = (height-int(yzoom))/2;
		cent.x = (center.x-tx-xscroll)/xzoom;
		cent.y = (center.y+ty-yscroll - height)/-yzoom;
		cent.z = 0.0f;
		}
	else
		{
		int ct = 0;
		for (int i=0; i<TVMaps.v.Count(); i++) {
			if (vsel[i]) {
				cent += TVMaps.v[i].p;
				ct++;
				}
			}
		if (!ct) return;
		cent /= float(ct);
		}

	Matrix3 mat(1);	
	mat.Translate(-cent);
	Point3 sc(1,1,1);
	int i1, i2;
	GetUVWIndices(i1,i2);
	if (direction == 0)
		{
		sc[i1] = scale;
		sc[i2] = scale;
		}
	else if (direction == 1)
		{
		sc[i1] = scale;
//		sc[i2] = scale;
		}
	else if (direction == 2)
		{
//		sc[i1] = scale;
		sc[i2] = scale;
		}

//	sc[i1] = scale;
//	sc[i2] = scale;

	mat.Scale(sc,TRUE);
	mat.Translate(cent);

	for (i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			TVMaps.v[i].p = mat * TVMaps.v[i].p;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}

			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		else if(TVMaps.v[i].influence != 0.0f)
			{
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
//			TVMaps.v[i].p = mat * TVMaps.v[i].p;
			Point3 NewPoint = TVMaps.v[i].p;
			NewPoint = mat * TVMaps.v[i].p;
			Point3 vec;
			vec = (NewPoint - TVMaps.v[i].p) * TVMaps.v[i].influence;
			TVMaps.v[i].p += vec;

			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}

			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}

		



		}

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}


void UnwrapMod::MirrorGizmo(HWND h, int direction)
	{

	HoldPoints();	
	TimeValue t = ip->GetTime();
/*
	Point3 cent(0,0,0);
	int ct = 0;
	for (int i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			cent += TVMaps.v[i].p;
			ct++;
			}
		}
	if (!ct) return;
	cent /= float(ct);
*/
//	Matrix3 mat(1);	
//	mat.Translate(-cent);
	Point3 sc(1.0f,1.0f,1.0f);
	int i1, i2;
	GetUVWIndices(i1,i2);
	if (direction == 0)
		{
		sc[i1] = -1.0f;
		}
	else if (direction == 1)
		{
		sc[i2] = -1.0f;
		}


//flip the scale transform
//	mat.Scale(sc,TRUE);
//	mat.Translate(cent);
	if (scaleControl) 
		scaleControl->GetValue(t,&gScale,FOREVER);
	gScale *= sc;

	if (scaleControl) 
		scaleControl->SetValue(t,&gScale);

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
	}


void UnwrapMod::MirrorPoints(HWND h, int direction)
	{
	TimeValue t = ip->GetTime();

	Point3 cent(0,0,0);
	int i;
	int ct = 0;
	for (i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			cent += TVMaps.v[i].p;
			ct++;
			}
		}
	if (!ct) return;
	cent /= float(ct);

	theHold.SuperBegin();
	theHold.Begin();
	HoldPoints();	


	Matrix3 mat(1);	
	mat.Translate(-cent);
	Point3 sc(1.0f,1.0f,1.0f);
	int i1, i2;
	GetUVWIndices(i1,i2);
	if (direction == 0)
		{
		sc[i1] = -1.0f;
		}
	else if (direction == 1)
		{
		sc[i2] = -1.0f;
		}

//	sc[i1] = scale;
//	sc[i2] = scale;

	mat.Scale(sc,TRUE);
	mat.Translate(cent);

	for (i=0; i<TVMaps.v.Count(); i++) {
		if (vsel[i]) {
			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
//check snap and bitmap
			TVMaps.v[i].p = mat * TVMaps.v[i].p;
			if ((isBitmap) && (pixelSnap))
				{
				SnapPoint(TVMaps.v[i].p);
				}

			if (TVMaps.cont[i]) 
				TVMaps.cont[i]->SetValue(t,&TVMaps.v[i].p);
			}
		}

	theHold.Accept(_T(GetString(IDS_TH_MIRROR)));
	theHold.SuperAccept(_T(GetString(IDS_TH_MIRROR)));

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
	InvalidateView();
	}

void UnwrapMod::UpdateListBox()
{
	int ct = 0;
	for (int i = 0; i < 10; i++)
		{
		if (map[i] != NULL) 
			{
			SendMessage(hTextures, CB_INSERTSTRING , ct++, (LPARAM) (TCHAR*) map[i]->GetName());
			}
		}

}

void UnwrapMod::AddMaterial(MtlBase *mtl)
{
	int found = -1;

	for (int i = 0; i < 10; i++)
		{
		if (map[i] == mtl) 
			{
			return;
			}
		}

	for (i = 0; i < 10; i++)
		{
		if (map[i] == NULL) 
			{
			found = i;
			i = 10;
			}
		}
	if (found == -1)
		{
		for (i = 9; i >0 ;i--)
			ReplaceReference(i+1,map[i-1]);
		ReplaceReference(1,mtl);
		SendMessage(hTextures, CB_DELETESTRING , 0, (LPARAM) (TCHAR*) mtl->GetName());
		SendMessage(hTextures, CB_INSERTSTRING , 0, (LPARAM) (TCHAR*) mtl->GetName());
//		SendMessage(hTextures, CB_INSERTSTRING , found, (LPARAM) (TCHAR*) mtl->GetName());
		found = 0;
		}
	else 
		{
		ReplaceReference(found+1,mtl);
		SendMessage(hTextures, CB_INSERTSTRING , found, (LPARAM) (TCHAR*) mtl->GetName());
		}
	CurrentMap = found;
	SetupImage();

}

void UnwrapMod::PickMap()
	{	
	BOOL newMat=FALSE, cancel=FALSE;
	MtlBase *mtl = ip->DoMaterialBrowseDlg(
		hWnd,
		BROWSE_MAPSONLY|BROWSE_INCNONE|BROWSE_INSTANCEONLY,
		newMat,cancel);
	if (cancel) {
		if (newMat) mtl->DeleteThis();
		return;
		}

	AddMaterial(mtl);
	}

UBYTE *RenderBitmap(Bitmap *bmp,int w, int h)
	{
	float du = 1.0f/float(w);
	float dv = 1.0f/float(h);
	AColor col;
//	SCTex sc;
//	int scanw = ByteWidth(w*3);
	int scanw = ByteWidth(w);
//	UBYTE *image = new UBYTE[ByteWidth(w*3)*h];
	UBYTE *image = new UBYTE[ByteWidth(w)*h];
	UBYTE *p1;
		
//	sc.scale = 1.0f;
//	sc.duvw = Point3(du,dv,0.0f);
//	sc.dpt  = sc.duvw;
//	sc.uvw.y = 1.0f-0.5f*dv;

	BMM_Color_64 color;
	for (int j=0; j<h; j++) {
//		sc.scrPos.y = j;
//		sc.uvw.x = 0.5f*du;				
		p1 = image + (h-j-1)*scanw;
		for (int i=0; i<w; i++) {
			bmp->GetPixels(i,j,1,&color);
			
			*p1++ = (UBYTE)(color.b>>8);
			*p1++ = (UBYTE)(color.g>>8);
			*p1++ = (UBYTE)(color.r>>8);	

			}		
		}
	return image;
	}


void UnwrapMod::SetupImage()
	{
	delete image; image = NULL;
	if (map[CurrentMap]) {		
		iw = rendW;
		ih = rendH;
		aspect = 1.0f;
//		Class_ID bid = Class_ID(BMTEX_CLASS_ID);
		Bitmap *bmp = NULL;
		if (map[CurrentMap]->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
			{
			isBitmap = 1;
			BitmapTex *bmt;
			bmt = (BitmapTex *) map[CurrentMap];
			bmp = bmt->GetBitmap(ip->GetTime());
			if (bmp!= NULL)
				{
				if (useBitmapRes)
					{
					bitmapWidth = bmp->Width();
					bitmapHeight = bmp->Height();
					iw = bitmapWidth;
					ih = bitmapHeight;
					aspect = (float)bitmapWidth/(float)bitmapHeight;
					}
				else	
					{
					bitmapWidth = iw;
					bitmapHeight = ih;

					aspect = (float)iw/(float)ih;
					}


				}
			}
		else
			{
			isBitmap = 0;
			}
		if (iw==0 || ih==0) return;
		map[CurrentMap]->Update(ip->GetTime(), FOREVER);
		map[CurrentMap]->LoadMapFiles(ip->GetTime());
		SetCursor(LoadCursor(NULL,IDC_WAIT));
//		if (isBitmap)
//			image = RenderTexMap(map[CurrentMap],bitmapWidth,bitmapHeight);
//		else 
		if (map[CurrentMap]->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
			{
			if (bmp != NULL)
				{
				if (useBitmapRes)
					image = RenderBitmap(bmp,iw,ih);
				else image = RenderTexMap(map[CurrentMap],iw,ih);
				}
			}
		else image = RenderTexMap(map[CurrentMap],iw,ih);
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		InvalidateView();
		}
	if (image) iShowMap->Enable();
	else iShowMap->Disable();
	}

void UnwrapMod::PlugControllers()
	{
	theHold.Begin();
	SuspendAnimate();
	AnimateOff();
	for (int i=0; i<TVMaps.v.Count(); i++) {
		if ((vsel[i] && !TVMaps.cont[i]) || ((TVMaps.v[i].influence != 0.0f) && !TVMaps.cont[i])){
			ReplaceReference(i+11,NewDefaultPoint3Controller());			
			TVMaps.cont[i]->SetValue(0,&TVMaps.v[i].p);
			}
		}
	ResumeAnimate();
	theHold.Accept(_T(GetString(IDS_PW_ASSIGN_CONTROLLER)));
	}

int UnwrapMod::GetAxis()
{
return 2;
return axis;
}

Point3 UnwrapMod::GetPoint(TimeValue t,int i)
	{
	if (i>=TVMaps.cont.Count() || i>= TVMaps.v.Count()) {
		return Point3(0,0,0);
		}
	if (TVMaps.v[i].flags & FLAG_DEAD) {
		return Point3(0,0,0);
		}

	if (TVMaps.cont[i]) TVMaps.cont[i]->GetValue(t,&TVMaps.v[i].p,FOREVER);
	return TVMaps.v[i].p;
	}


void UnwrapMod::BuildObjectPoints()
{
	objectPointList.SetCount(TVMaps.v.Count());

	Point3 p(0.0f,0.0f,0.0f);
	for (int i = 0; i < TVMaps.f.Count(); i++)
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		for (int j = 0 ; j < pcount; j++)
			{
			if (TVMaps.f[i].t[j]<objectPointList.Count())
				objectPointList[TVMaps.f[i].t[j]] = TVMaps.f[i].pt[j];
//			if (index == TVMaps.f[i].t[j])
//				return TVMaps.f[i].pt[j];
			}
			
		}

}

Point3 UnwrapMod::GetObjectPoint(TimeValue t,int index)
	{

	Point3 p(0.0f,0.0f,0.0f);
	if (index<objectPointList.Count())
		p = objectPointList[index];
/*	for (int i = 0; i < TVMaps.f.Count(); i++)
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		for (int j = 0 ; j < pcount; j++)
			{
			if (index == TVMaps.f[i].t[j])
				return TVMaps.f[i].pt[j];
			}
			
		}
*/
	return p;

	}
/*
Point3 UnwrapMod::GetObjectPoint(TimeValue t,int index)
	{
	Point3 p(0.0f,0.0f,0.0f);
	for (int i = 0; i < TVMaps.f.Count(); i++)
		{
		int pcount = 3;
		if (TVMaps.f[i].flags & FLAG_QUAD) pcount = 4;
		for (int j = 0 ; j < pcount; j++)
			{
			if (index == TVMaps.f[i].t[j])
				return TVMaps.f[i].pt[j];
			}
			
		}
	return p;

	}
*/

void UnwrapMod::ZoomExtents()
	{
	Rect brect;
	Point2 pt;
	float xzoom, yzoom;
	int width,height;
	TimeValue t = ip->GetTime();
	ComputeZooms(hView,xzoom,yzoom,width,height);	
	brect.SetEmpty();
	IsSelectedSetup();
	for (int i=0; i<TVMaps.v.Count(); i++) {		
		if (!(TVMaps.v[i].flags & FLAG_DEAD))
			{
			if (IsVertVisible(i))
				{
				pt = UVWToScreen(GetPoint(t,i),xzoom,yzoom,width,height);
				IPoint2 ipt(int(pt.x),int(pt.y));
				brect += ipt;		
				}
			}
		}
	Rect srect;
	GetClientRect(hView,&srect);		
	float rat1, rat2;
	rat1 = float(srect.w()-1)/float(fabs(double(brect.w()-1)));
	rat2 = float(srect.h()-1)/float(fabs(double(brect.h()-1)));
	float rat = (rat1<rat2?rat1:rat2) * 0.9f;
	zoom *= rat;
	IPoint2 delta = srect.GetCenter() - brect.GetCenter();
	xscroll += delta.x;
	yscroll += delta.y;
	xscroll *= rat;
	yscroll *= rat;	
	InvalidateView();
	}


void UnwrapMod::ZoomSelected()
	{
	Rect brect;
	Point2 pt;
	float xzoom, yzoom;
	int width,height;
	TimeValue t = ip->GetTime();
	ComputeZooms(hView,xzoom,yzoom,width,height);	
	brect.SetEmpty();
	int found = 0;
	for (int i=0; i<TVMaps.v.Count(); i++) {		
		if (!(TVMaps.v[i].flags & FLAG_DEAD))
			{
			if (vsel[i])
				{
				pt = UVWToScreen(GetPoint(t,i),xzoom,yzoom,width,height);
				IPoint2 ipt(int(pt.x),int(pt.y));
				brect += ipt;		
				found++;
				}
			}
		}
	if (found <=1) return;
	Rect srect;
	GetClientRect(hView,&srect);		
	float rat1 = 1.0f, rat2 = 1.0f;
	if (brect.w()>2.0f )
		rat1 = float(srect.w()-1)/float(fabs(double(brect.w()-1)));
	if (brect.h()>2.0f )
		rat2 = float(srect.h()-1)/float(fabs(double(brect.h()-1)));
	float rat = (rat1<rat2?rat1:rat2) * 0.9f;
	zoom *= rat;
	IPoint2 delta = srect.GetCenter() - brect.GetCenter();
	xscroll += delta.x;
	yscroll += delta.y;
	xscroll *= rat;
	yscroll *= rat;	
	InvalidateView();
	}


void UnwrapMod::AlignMap()
	{
	TimeValue t = ip->GetTime();	
	GeomObject *obj;
	Point3 norm, pt;
	Interval valid;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Calculate a ray from the mouse point

	for (int i=0; i<nodeList.Count(); i++) {
		INode *node = nodeList[i];

		// Get the object from the node
		ObjectState os = node->EvalWorldState(t);
		if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
			obj = (GeomObject*)os.obj;
		} else {
			continue;
			}

//compute the average normal or use x/y/z
		// See if we hit the object
//find center of selection
//average normal if needed
		Point3 zeroPoint(.0f,.0f,.0f);
		Ray ray;
		ray.p  = zeroPoint;
		ray.dir = zeroPoint;
		int dir = alignDir;
		if (dir == 0) //x
			{
			ray.dir.x = 1.0f; 
			norm = -ray.dir;
			}
		else if (dir == 1) //y
			{	
			ray.dir.y = 1.0f; 
			norm = -ray.dir;
			}	
		else if (dir == 2) //z
			{
			ray.dir.z = 1.0f; 
			norm = ray.dir;
			}
		else
			{
//compute average normal
			Point3 pnorm(0.0f,0.0f,0.0f);
			for (int k=0; k<gfaces.Count(); k++) {
			// Grap the three points, xformed
				int pcount = 3;
				if (gfaces[k].flags & FLAG_QUAD) pcount = 4;

				Point3 temp_point[4];
				for (int j=0; j<pcount; j++) {
					int index = gfaces[k].t[j];
//					Point3 temp_point;
					temp_point[j] = gverts.d[index].p;
					}
				pnorm += Normalize(temp_point[1]-temp_point[0]^temp_point[2]-temp_point[1]);
				}
			ray.dir = pnorm / (float) gfaces.Count();
			norm = -ray.dir;
			}


//		if (obj->IntersectRay(t,ray,at,norm)) {
		if (1) {
			// Calculate the hit point
			pt = ray.p + ray.dir;
					
			// Get the mod context tm
			Matrix3 tm(1);
			if (mcList[0]->tm) tm = tm * *mcList[0]->tm;
		
			// Transform the point and ray into mod context space
			pt = pt * tm;
			norm = Normalize(VectorTransform(tm,norm));
		
			// Construct the target transformation in mod context space
			Matrix3 destTM;
			UnwrapMatrixFromNormal(norm,destTM);
			destTM.SetTrans(pt);
			destTM.PreRotateZ(0);

			switch (GetAxis()) {
				case 0:
					destTM.PreRotateY(-HALFPI);
					break;
				case 1:
					destTM.PreRotateX(HALFPI);
					break;

				}

			// Our current transformation... gives relative TM
			Matrix3 curTM(1), relTM, id(1);
			tmControl->GetValue(t,&curTM,valid,CTRL_RELATIVE);
			relTM = Inverse(curTM) * destTM;
		
			// Here's the modifications we need to make to get there
			tm.IdentityMatrix();
			tm.SetTrans(curTM.GetTrans());
			AffineParts parts;			
			decomp_affine(relTM,&parts);
			Point3 delta = destTM.GetTrans()-curTM.GetTrans();
			Rotate(t,id,tm,parts.q);
			Move(t,id,id,delta);
			break;
			}
		}

	nodeList.DisposeTemporary();
	}

//--- Mouse procs for modes -----------------------------------------------

int SelectMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				// First click
				
				region   = FALSE;
				toggle   = flags&MOUSE_CTRL;
				subtract = flags&MOUSE_ALT;
				
				// Hit test
				Tab<int> hits;
				Rect rect;
				rect.left = m.x-2;
				rect.right = m.x+2;
				rect.top = m.y-2;
				rect.bottom = m.y+2;
				// First hit test sel only
				mod->centeron = 0;
				if (toggle && subtract)
					{
					mod->centeron = 1;
					return subproc(hWnd,msg,point,flags,m);
					}
				
				// First hit test sel only
				if ((!toggle && !subtract && mod->HitTest(rect,hits,TRUE)) || (mod->lockSelected==1)){
					return subproc(hWnd,msg,point,flags,m);
				} else
				// Next hit test everything
				if (mod->HitTest(rect,hits,subtract)) {
					theHold.Begin();
					if (!toggle && !subtract) mod->ClearSelect();
					mod->Select(hits,toggle,subtract,FALSE);
					mod->InvalidateView();
					theHold.Accept(_T(GetString(IDS_PW_SELECT_UVW)));
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());

					if (toggle || subtract) return FALSE;
					return subproc(hWnd,msg,point,flags,m);
				} else {					
					region = TRUE;
					lm = om = m;
					XORDottedRect(hWnd,om,m);
					}				
			} else {
				// Second click
				if (region) {
					Rect rect;
					rect.left   = om.x;
					rect.top    = om.y;
					rect.right  = m.x;
					rect.bottom = m.y;
					rect.Rectify();					
					Tab<int> hits;
					theHold.Begin();
					if (!toggle && !subtract) mod->ClearSelect();
					if (mod->HitTest(rect,hits,subtract)) {						
						mod->Select(hits,FALSE,subtract,TRUE);											
						}
					theHold.Accept(_T(GetString(IDS_PW_SELECT_UVW)));
					
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());

					mod->InvalidateView();
				} else {
					return subproc(hWnd,msg,point,flags,m);
					}
				}
			break;			

		case MOUSE_MOVE:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (region) {
				XORDottedRect(hWnd,om,lm);
				XORDottedRect(hWnd,om,m);
				lm = m;
			} else {
				SetCursor(GetXFormCur());
				return subproc(hWnd,msg,point,flags,m);
				}
			break;
		case MOUSE_FREEMOVE: {
			Tab<int> hits;
			Rect rect;
			rect.left = m.x-2;
			rect.right = m.x+2;
			rect.top = m.y-2;
			rect.bottom = m.y+2;

			if ((flags&MOUSE_CTRL) &&  (flags&MOUSE_ALT))
				mod->ip->ReplacePrompt( GetString(IDS_PW_MOUSE_CENTER));
			else if (flags&MOUSE_CTRL)
				mod->ip->ReplacePrompt( GetString(IDS_PW_MOUSE_ADD));
			else if (flags&MOUSE_ALT)
				mod->ip->ReplacePrompt( GetString(IDS_PW_MOUSE_SUBTRACT));
			else if (flags&MOUSE_SHIFT)
				mod->ip->ReplacePrompt( GetString(IDS_PW_MOUSE_CONSTRAIN));
			else mod->ip->ReplacePrompt( GetString(IDS_PW_MOUSE_SELECTTV));


			if (mod->HitTest(rect,hits,FALSE)) {
				if (mod->vsel[hits[0]]) {
					SetCursor(GetXFormCur());
				} else {
					SetCursor(selCur);
					}
			} else {
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				}


			return subproc(hWnd,msg,point,flags,m);
			}

		case MOUSE_ABORT:
			if (region) {
				InvalidateRect(hWnd,NULL,FALSE);
			} else {
				return subproc(hWnd,msg,point,flags,m);
				}
			break;
		}
	return 1;
	}

int MoveMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				theHold.SuperBegin();
				mod->PlugControllers();
				theHold.Begin();
				om = m;
			} else {
				theHold.Accept(_T(GetString(IDS_PW_MOVE_UVW)));
				theHold.SuperAccept(_T(GetString(IDS_PW_MOVE_UVW)));
				mod->ip->RedrawViews(mod->ip->GetTime());
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			float xzoom, yzoom;
			int width, height;
			IPoint2 delta = m-om;
			if (flags&MOUSE_SHIFT && mod->move==0) {
				if (abs(delta.x) > abs(delta.y)) delta.y = 0;
				else delta.x = 0;
			} else if (mod->move==1) {
				delta.y = 0;
			} else if (mod->move==2) {
				delta.x = 0;
				}
			mod->ComputeZooms(hWnd,xzoom,yzoom,width,height);
			Point2 mv;
			mv.x = delta.x/xzoom;
			mv.y = -delta.y/yzoom;
//check if moving points or gizmo

			if ( (mod->ip && (mod->ip->GetSubObjectLevel() == 3) ))
				{
//				mv.x += (float)om.x/xzoom;
//				mv.y += (float)om.y/yzoom;

				mod->MoveGizmo(mv);
				}
			else mod->MovePoints(mv);
	
	

			if (mod->update) mod->ip->RedrawViews(mod->ip->GetTime());
			UpdateWindow(hWnd);
			break;		
			}


		case MOUSE_ABORT:
			if ( (mod->ip && (mod->ip->GetSubObjectLevel() == 3) ))
				{
				}
			else
				{
				theHold.Cancel();
				theHold.SuperCancel();
				}
			mod->ip->RedrawViews(mod->ip->GetTime());
			break;
		}
	return 1;
	}

#define ZOOM_FACT	0.01f
#define ROT_FACT	DegToRad(0.5f)

int RotateMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				theHold.SuperBegin();
				mod->PlugControllers();
				theHold.Begin();
				mod->center.x = (float) m.x;
				mod->center.y = (float) m.y;
				om = m;
			} else {
				theHold.Accept(_T(GetString(IDS_PW_ROTATE_UVW)));
				theHold.SuperAccept(_T(GetString(IDS_PW_ROTATE_UVW)));
				mod->ip->RedrawViews(mod->ip->GetTime());
				}
			break;

		case MOUSE_MOVE:
			theHold.Restore();
			if ( (mod->ip && (mod->ip->GetSubObjectLevel() == 3) ))
				{
				mod->RotateGizmo(hWnd,float(m.y-om.y)*ROT_FACT);
				}
			else mod->RotatePoints(hWnd,float(m.y-om.y)*ROT_FACT);
			if (mod->update) mod->ip->RedrawViews(mod->ip->GetTime());
			UpdateWindow(hWnd);
			break;		

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			mod->ip->RedrawViews(mod->ip->GetTime());
			break;
		}
	return 1;
	}

int ScaleMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				theHold.SuperBegin();
				mod->PlugControllers();
				theHold.Begin();
				mod->center.x = (float) m.x;
				mod->center.y = (float) m.y;
				om = m;
			} else {
				theHold.Accept(_T(GetString(IDS_PW_SCALE_UVW)));
				theHold.SuperAccept(_T(GetString(IDS_PW_SCALE_UVW)));
				mod->ip->RedrawViews(mod->ip->GetTime());
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			IPoint2 delta = om-m;
			int direction = 0;
			if (flags&MOUSE_SHIFT ){
				if (abs(delta.x) > abs(delta.y)) 
					{
					delta.y = 0;
					direction = 1;
					}
				else 
					{	
					delta.x = 0;
					direction = 2;
					}
				}
			else if (mod->scale > 0)
				{
				if (mod->scale == 1) 
					{
					delta.y = 0;
					direction = 1;
					}
				else if (mod->scale == 2) 
					{	
					delta.x = 0;
					direction = 2;
					}

				}

			float z;
			if (direction == 0)
				{
				if (delta.y<0)
					 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
				else z = (1.0f+ZOOM_FACT*delta.y);
				}
			else if (direction == 1)
				{
				if (delta.x<0)
					 z = (1.0f/(1.0f-ZOOM_FACT*delta.x));
				else z = (1.0f+ZOOM_FACT*delta.x);

				}
			else if (direction == 2)
				{
				if (delta.y<0)
					 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
				else z = (1.0f+ZOOM_FACT*delta.y);
				}
//Rect rect;
//GetWindowRect(hWnd,&rect);	
			if ( (mod->ip && (mod->ip->GetSubObjectLevel() == 3) ))
				{
				mod->ScaleGizmo(hWnd, z,direction);
				}
			else mod->ScalePoints(hWnd, z,direction);
/*
			if (delta.y<0)
				 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
			else z = (1.0f+ZOOM_FACT*delta.y);
			mod->ScalePoints(z);
*/
			if (mod->update) mod->ip->RedrawViews(mod->ip->GetTime());
			UpdateWindow(hWnd);
			break;
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			mod->ip->RedrawViews(mod->ip->GetTime());
			break;
		}
	return 1;
	}

int WeldMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				theHold.SuperBegin();
		 		mod->PlugControllers();
				theHold.Begin();
				om = m;
			} else {
				if (mod->WeldPoints(hWnd,m))
					{
					theHold.Accept(_T(GetString(IDS_PW_WELD_UVW)));
					theHold.SuperAccept(_T(GetString(IDS_PW_WELD_UVW)));
					}
				else{
					theHold.Accept(_T(GetString(IDS_PW_MOVE_UVW)));
					theHold.SuperAccept(_T(GetString(IDS_PW_MOVE_UVW)));
					}
				mod->ip->RedrawViews(mod->ip->GetTime());
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			float xzoom, yzoom;
			int width, height;
			IPoint2 delta = m-om;
			if (flags&MOUSE_SHIFT && mod->move==0) {
				if (abs(delta.x) > abs(delta.y)) delta.y = 0;
				else delta.x = 0;
			} else if (mod->move==1) {
				delta.y = 0;
			} else if (mod->move==2) {
				delta.x = 0;
				}
			mod->ComputeZooms(hWnd,xzoom,yzoom,width,height);
			Point2 mv;
			mv.x = delta.x/xzoom;
			mv.y = -delta.y/yzoom;
			mod->MovePoints(mv);
			if (mod->update) mod->ip->RedrawViews(mod->ip->GetTime());
			UpdateWindow(hWnd);
			break;		
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			mod->ip->RedrawViews(mod->ip->GetTime());
			break;
		}
	return 1;
	}


int PanMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				oxscroll = mod->xscroll;
				oyscroll = mod->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = m-om;
			mod->xscroll = oxscroll + float(delta.x);
			mod->yscroll = oyscroll + float(delta.y);
			mod->InvalidateView();
			SetCursor(GetPanCursor());
			break;
			}

		case MOUSE_ABORT:
			mod->xscroll = oxscroll;
			mod->yscroll = oyscroll;
			mod->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(GetPanCursor());
			break;		
		}
	return 1;
	}

int ZoomMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				ozoom = mod->zoom;
				oxscroll = mod->xscroll;
				oyscroll = mod->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = om-m;
			float z;
			if (delta.y<0)
				 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
			else z = (1.0f+ZOOM_FACT*delta.y);
			mod->zoom = ozoom * z;
			mod->xscroll = oxscroll*z;
			mod->yscroll = oyscroll*z;
			mod->InvalidateView();
			SetCursor(zoomCur);

//			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
			}

		case MOUSE_ABORT:
			mod->zoom = ozoom;
			mod->xscroll = oxscroll;
			mod->yscroll = oyscroll;
			mod->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(zoomCur);

//			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;		
		}
	return 1;
	}

int ZoomRegMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				lm = om = m;
				XORDottedRect(hWnd,om,lm);
			} else {

				if (om!=m)
				{

					Rect rect;
					GetClientRect(hWnd,&rect);
					IPoint2 mcent = (om+m)/2;
					IPoint2 scent = rect.GetCenter();
					IPoint2 delta = m-om;
					float rat1, rat2;
					if ((delta.x  != 0) && (delta.y != 0))
						{
						rat1 = float(rect.w()-1)/float(fabs((double)delta.x));
						rat2 = float(rect.h()-1)/float(fabs((double)delta.y));
						float rat = rat1<rat2?rat1:rat2;
						mod->zoom *= rat;
						delta = scent - mcent;
						mod->xscroll += delta.x;
						mod->yscroll += delta.y;
						mod->xscroll *= rat;
						mod->yscroll *= rat;
						}
					}
				mod->InvalidateView();
				}
			break;

		case MOUSE_MOVE:
			XORDottedRect(hWnd,om,lm);
			XORDottedRect(hWnd,om,m);
			lm = m;
			SetCursor(zoomRegionCur);
			break;

		case MOUSE_ABORT:
			InvalidateRect(hWnd,NULL,FALSE);
			break;
		
		case MOUSE_FREEMOVE:
			SetCursor(zoomRegionCur);
			break;		
		}
	return 1;
	}

int RightMouseMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
		case MOUSE_PROPCLICK:
//check if in pan zoom or soom region mode
			if ( (mod->mode == ID_UNWRAP_PAN) ||
				 (mod->mode == ID_TOOL_PAN) ||
				 (mod->mode == ID_UNWRAP_ZOOM) ||
				 (mod->mode == ID_TOOL_ZOOM) ||
				 (mod->mode == ID_UNWRAP_ZOOMREGION) ||
				 (mod->mode == ID_TOOL_ZOOMREG) ||
				 (mod->mode == ID_UNWRAP_WELD) ||
				 (mod->mode == ID_TOOL_WELD) 
				)
				{
				if (!( (mod->oldMode == ID_UNWRAP_PAN) ||
					 (mod->oldMode == ID_TOOL_PAN) ||
					 (mod->oldMode == ID_UNWRAP_ZOOM) ||
					 (mod->oldMode == ID_TOOL_ZOOM) ||
					 (mod->oldMode == ID_UNWRAP_ZOOMREGION) ||
					 (mod->oldMode == ID_TOOL_ZOOMREG) ||
					 (mod->oldMode == ID_UNWRAP_WELD) ||
					 (mod->oldMode == ID_TOOL_WELD) 
					))
					{
					mod->SetMode(mod->oldMode);
					}
				else mod->TrackRBMenu(hWnd, m.x, m.y);

				}
			else mod->TrackRBMenu(hWnd, m.x, m.y);
			break;
		}
	return 1;
	}

int MiddleMouseMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	static int modeType = 0;
	switch (msg) {
			
		case MOUSE_POINT:
			if (point==0) {
				BOOL ctrl = flags & MOUSE_CTRL;
				BOOL alt = flags & MOUSE_ALT;

				if (ctrl && alt)
					{
					modeType = ID_TOOL_ZOOM;
					ozoom = mod->zoom;
					}
				else modeType = ID_TOOL_PAN;

				om = m;
				oxscroll = mod->xscroll;
				oyscroll = mod->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			if (modeType == ID_TOOL_PAN)
				{
				IPoint2 delta = m-om;
				mod->xscroll = oxscroll + float(delta.x);
				mod->yscroll = oyscroll + float(delta.y);
				mod->InvalidateView();
				SetCursor(GetPanCursor());
				}
			else if (modeType == ID_TOOL_ZOOM)
				{
				IPoint2 delta = om-m;
				float z;
				if (delta.y<0)
					 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
				else z = (1.0f+ZOOM_FACT*delta.y);
				mod->zoom = ozoom * z;
				mod->xscroll = oxscroll*z;
				mod->yscroll = oyscroll*z;
				mod->InvalidateView();
				SetCursor(zoomCur);
				}
			break;
			}

		case MOUSE_ABORT:
			mod->xscroll = oxscroll;
			mod->yscroll = oyscroll;
			if (modeType == ID_TOOL_ZOOM)
				mod->zoom = ozoom;
			mod->InvalidateView();
			break;



/*		case MOUSE_FREEMOVE:
			SetCursor(GetPanCursor());
			break;		
*/

/*
		case MOUSE_PROPCLICK:
//check if in pan zoom or soom region mode
			if ( (mod->mode == ID_UNWRAP_PAN) ||
				 (mod->mode == ID_TOOL_PAN) ||
				 (mod->mode == ID_UNWRAP_ZOOM) ||
				 (mod->mode == ID_TOOL_ZOOM) ||
				 (mod->mode == ID_UNWRAP_ZOOMREGION) ||
				 (mod->mode == ID_TOOL_ZOOMREG) ||
				 (mod->mode == ID_UNWRAP_WELD) ||
				 (mod->mode == ID_TOOL_WELD) 
				)
				mod->SetMode(mod->oldMode);
			else mod->TrackRBMenu(hWnd, m.x, m.y);
			break;
*/
		}
	return 1;
	}


static BOOL CALLBACK PropDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static IColorSwatch *iSelColor, *iLineColor;
	static ISpinnerControl *spinW, *spinH;
	static ISpinnerControl *spinThreshold ;

	UnwrapMod *mod = (UnwrapMod*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_INITDIALOG: {
			mod = (UnwrapMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			
			iSelColor = GetIColorSwatch(GetDlgItem(hWnd,IDC_UNWRAP_SELCOLOR), 
				mod->selColor, _T(GetString(IDS_PW_LINECOLOR)));			
			iLineColor = GetIColorSwatch(GetDlgItem(hWnd,IDC_UNWRAP_LINECOLOR), 
				mod->lineColor, _T(GetString(IDS_PW_LINECOLOR)));						
			
			CheckDlgButton(hWnd,IDC_UNWRAP_CONSTANTUPDATE,mod->update);


			CheckDlgButton(hWnd,IDC_UNWRAP_SELECT_VERTS,mod->showVerts);
			CheckDlgButton(hWnd,IDC_UNWRAP_MIDPIXEL_SNAP,mod->midPixelSnap);

			CheckDlgButton(hWnd,IDC_UNWRAP_USEBITMAPRES,mod->useBitmapRes);
			if ((mod->map[mod->CurrentMap]) && (mod->map[mod->CurrentMap]->ClassID() != Class_ID(BMTEX_CLASS_ID,0) ) )
				EnableWindow(GetDlgItem(hWnd,IDC_UNWRAP_USEBITMAPRES),FALSE);
			
			spinThreshold = SetupFloatSpinner(
				hWnd,IDC_UNWRAP_WELDTHRESHSPIN,IDC_UNWRAP_WELDTHRESH,
				0.0f,10.0f,mod->weldThreshold);			
			spinW = SetupIntSpinner(
				hWnd,IDC_UNWRAP_WIDTHSPIN,IDC_UNWRAP_WIDTH,
				10,1000,mod->rendW);			
			spinH = SetupIntSpinner(
				hWnd,IDC_UNWRAP_HEIGHTSPIN,IDC_UNWRAP_HEIGHT,
				10,1000,mod->rendH);			
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					{
					mod->lineColor = iLineColor->GetColor();
					mod->selColor  = iSelColor->GetColor();					
					mod->update = IsDlgButtonChecked(hWnd,IDC_UNWRAP_CONSTANTUPDATE);
					mod->showVerts = IsDlgButtonChecked(hWnd,IDC_UNWRAP_SELECT_VERTS);
					mod->midPixelSnap = IsDlgButtonChecked(hWnd,IDC_UNWRAP_MIDPIXEL_SNAP);
//watje 5-3-99
					BOOL oldRes = mod->useBitmapRes;

					mod->useBitmapRes = IsDlgButtonChecked(hWnd,IDC_UNWRAP_USEBITMAPRES);
					mod->weldThreshold = spinThreshold->GetFVal();
					mod->rendW = spinW->GetIVal();
					mod->rendH = spinH->GetIVal();
//watje 5-3-99
					if (mod->rendW!=mod->iw ||
						mod->rendH!=mod->ih || oldRes!=mod->useBitmapRes) {
						mod->SetupImage();
						}
					mod->InvalidateView();
					//fall through
					}
				case IDCANCEL:
					ReleaseIColorSwatch(iSelColor);
					ReleaseIColorSwatch(iLineColor);
					ReleaseISpinner(spinThreshold);
					ReleaseISpinner(spinW);
					ReleaseISpinner(spinH);
					EndDialog(hWnd,0);
					break;

				case IDC_UNWRAP_DEFAULTS:
					iLineColor->SetColor(RGB(255,255,255));
					iSelColor->SetColor(RGB(255,0,0));
					spinW->SetValue(256,FALSE);
					spinH->SetValue(256,FALSE);					
					spinThreshold->SetValue(2,FALSE);					
					CheckDlgButton(hWnd,IDC_UNWRAP_CONSTANTUPDATE,TRUE);
					CheckDlgButton(hWnd,IDC_UNWRAP_SELECT_VERTS,TRUE);
					CheckDlgButton(hWnd,IDC_UNWRAP_MIDPIXEL_SNAP,FALSE);
					CheckDlgButton(hWnd,IDC_UNWRAP_USEBITMAPRES,TRUE);
					break;
				}
			break;

		default:
			return FALSE;
		} 
	return TRUE;
	}

void UnwrapMod::PropDialog() 
	{
	DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_UNWRAP_PROP),
		hWnd,
		PropDlgProc,
		(LONG)this);
	}


static void UnwrapMatrixFromNormal(Point3& normal, Matrix3& mat)
	{
	Point3 vx;
	vx.z = .0f;
	vx.x = -normal.y;
	vx.y = normal.x;	
	if ( vx.x == .0f && vx.y == .0f ) {
		vx.x = 1.0f;
		}
	mat.SetRow(0,vx);
	mat.SetRow(1,normal^vx);
	mat.SetRow(2,normal);
	mat.SetTrans(Point3(0,0,0));
	mat.NoScale();
	}

void UnwrapFaceAlignMouseProc::FaceAlignMap(HWND hWnd,IPoint2 m)
	{
	ViewExp *vpt = ip->GetViewport(hWnd);
	if (!vpt) return;

	Ray ray, wray;
	float at;
	TimeValue t = ip->GetTime();	
	GeomObject *obj;
	Point3 norm, pt;
	Interval valid;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Calculate a ray from the mouse point
	vpt->MapScreenToWorldRay(float(m.x), float(m.y),wray);

	for (int i=0; i<nodeList.Count(); i++) {
		INode *node = nodeList[i];

		// Get the object from the node
		ObjectState os = node->EvalWorldState(t);
		if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
			obj = (GeomObject*)os.obj;
		} else {
			continue;
			}

		// Back transform the ray into object space.		
		Matrix3 obtm  = node->GetObjectTM(t);
		Matrix3 iobtm = Inverse(obtm);
		ray.p   = iobtm * wray.p;
		ray.dir = VectorTransform(iobtm, wray.dir);
	
		// See if we hit the object
		if (obj->IntersectRay(t,ray,at,norm)) {
			// Calculate the hit point
			pt = ray.p + ray.dir * at;
					
			// Get the mod context tm
			Matrix3 tm(1);
			if (mcList[0]->tm) tm = tm * *mcList[0]->tm;
		
			// Transform the point and ray into mod context space
			pt = pt * tm;
			norm = Normalize(VectorTransform(tm,norm));
		
			// Construct the target transformation in mod context space
			Matrix3 destTM;
			UnwrapMatrixFromNormal(norm,destTM);
			destTM.SetTrans(pt);
			destTM.PreRotateZ(0);

			switch (mod->GetAxis()) {
				case 0:
					destTM.PreRotateY(-HALFPI);
					break;
				case 1:
					destTM.PreRotateX(HALFPI);
					break;

				}

			// Our current transformation... gives relative TM
			Matrix3 curTM(1), relTM, id(1);
			mod->tmControl->GetValue(t,&curTM,valid,CTRL_RELATIVE);
			relTM = Inverse(curTM) * destTM;
		
			// Here's the modifications we need to make to get there
			tm.IdentityMatrix();
			tm.SetTrans(curTM.GetTrans());
			AffineParts parts;			
			decomp_affine(relTM,&parts);
			Point3 delta = destTM.GetTrans()-curTM.GetTrans();
			mod->Rotate(t,id,tm,parts.q);
			mod->Move(t,id,id,delta);
			break;
			}
		}

	nodeList.DisposeTemporary();
	ip->ReleaseViewport(vpt);
	}



int UnwrapFaceAlignMouseProc::proc(
		HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {				
				theHold.Begin();
				ip->RedrawViews(ip->GetTime(),REDRAW_BEGIN);
			} else {
				theHold.Accept(0);
				ip->RedrawViews(ip->GetTime(),REDRAW_END);
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			FaceAlignMap(hWnd,m);
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);			
			break;
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			break;

		case MOUSE_FREEMOVE:			
			SetCursor(ip->GetSysCursor(SYSCUR_SELECT));
			break;
		}
	return TRUE;
	}

void UnwrapFaceAlignMode::EnterMode()
	{
	mod->EnterNormalAlign();	
	}

void UnwrapFaceAlignMode::ExitMode()
	{
	mod->ExitNormalAlign();	
	}


void UnwrapRegionFitMouseProc::RegionFitMap(HWND hWnd,IPoint2 m)
	{
	ViewExp *vpt = ip->GetViewport(hWnd);
	if (!vpt) return;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Viewport tm
	Matrix3 vtm;
	vpt->GetAffineTM(vtm);
	vtm = Inverse(vtm);

	// Node tm
	Matrix3 ntm = nodeList[0]->GetObjectTM(ip->GetTime());	
	
	// MC tm
	Matrix3 mctm(1);
	if (mcList[0]->tm) mctm = *mcList[0]->tm;

	// Current val of tmCont.. remove any scale
	Matrix3 ctm(1);
	mod->tmControl->GetValue(ip->GetTime(),&ctm,FOREVER,CTRL_RELATIVE);
	AffineParts parts;
	decomp_affine(ctm, &parts);
	parts.q.MakeMatrix(ctm);
	ctm.Translate(parts.t);
	
	// Compute the inverse world space tm for the gizmo
	Matrix3 iwtm = Inverse(ctm * Inverse(mctm) * ntm);
	
	// Calculate a ray from the two mouse points
	Ray mray, omray;
	float at;
	Point3 p1, p2;
	vpt->MapScreenToWorldRay(float(m.x), float(m.y),mray);
	vpt->MapScreenToWorldRay(float(om.x), float(om.y),omray);
	
	// Back transform the rays into gizmo space
	mray.p    = iwtm * mray.p;
	mray.dir  = VectorTransform(iwtm, mray.dir);
	omray.p   = iwtm * omray.p;
	omray.dir = VectorTransform(iwtm, omray.dir);

	float dir, pnt, odir, opnt;
/*
	switch (mod->GetAxis()) {
		case 0:
			dir = mray.dir.x; odir = omray.dir.x;
			pnt = mray.p.x; opnt = omray.p.x;
			break;
		case 1:
			dir = mray.dir.y; odir = omray.dir.y;
			pnt = mray.p.y; opnt = omray.p.y;
			break;
		case 2:
*/
			dir = mray.dir.z; odir = omray.dir.z;
			pnt = mray.p.z; opnt = omray.p.z;
/*
			break;
		}
*/
#define EPSILON	0.001
	// Make sure we're going to hit
	if (fabs(dir)>EPSILON && fabs(odir)>EPSILON) {
	
		// Compute the point of intersection
		at = -pnt/dir;
		p1 = mray.p + at*mray.dir;
		at = -opnt/odir;
		p2 = omray.p + at*omray.dir;
		
		// Center the map in the region
		ctm.PreTranslate((p1+p2)/2.0f);

		// Compute scale factors and scale
		float sx;
		float sy;
/*
		switch (mod->GetAxis()) {
			case 0:
				sx = (float)fabs(p1.z-p2.z);
				sy = (float)fabs(p1.y-p2.y);
				break;
			case 1:
				sx = (float)fabs(p1.x-p2.x);
				sy = (float)fabs(p1.z-p2.z);
				break;

			case 2:
*/
				sx = (float)fabs(p1.x-p2.x);
				sy = (float)fabs(p1.y-p2.y);
/*
				break;

			}
*/
		
		// Scale params instead of the matrix
		TimeValue t = ip->GetTime();		
/*		mod->SetWidth(t,sx);
		mod->SetLength(t,sy);		
*/
		mod->s.x = sx;
		mod->s.y = sy;
		/*
		if (sx>0.0f && sy>0.0f) {
			ctm.PreScale(Point3(sx,sy,1.0f));
			}
		*/

		// Plug-in the new value		
		SetXFormPacket pckt(ctm);
		mod->tmControl->SetValue(ip->GetTime(), &pckt);		
		}

	nodeList.DisposeTemporary();
	ip->ReleaseViewport(vpt);
	ip->RedrawViews(ip->GetTime());
	}

int UnwrapRegionFitMouseProc::proc(
		HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_PROPCLICK:
			ip->SetStdCommandMode(CID_OBJMOVE);
			break;

		case MOUSE_POINT:
			if (point==0) {				
				om = m;
				theHold.Begin();
				ip->RedrawViews(ip->GetTime(),REDRAW_BEGIN);
			} else {
				theHold.Accept(0);
				ip->RedrawViews(ip->GetTime(),REDRAW_END);
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			RegionFitMap(hWnd,m);
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);			
			break;
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			break;

		case MOUSE_FREEMOVE:			
			SetCursor(ip->GetSysCursor(SYSCUR_SELECT));
			break;
		}
	return TRUE;
	}

void UnwrapRegionFitMode::EnterMode()
	{
	mod->EnterRegionFit();	
	}

void UnwrapRegionFitMode::ExitMode()
	{
	mod->ExitRegionFit();	
	}

void UnwrapMod::EnterRegionFit()
	{
	SendMessage(GetDlgItem(hParams,IDC_UNWRAP_FIT),CC_COMMAND,CC_CMD_SET_STATE,1);
	}

void UnwrapMod::ExitRegionFit()
	{
	SendMessage(GetDlgItem(hParams,IDC_UNWRAP_FIT),CC_COMMAND,CC_CMD_SET_STATE,0);
	}

void UnwrapMod::EnterNormalAlign()
	{	
//	iAlignButton->SetCheck(TRUE );
//	SendMessage(GetDlgItem(hParams,IDC_UNWRAP_ALIGN_TO_NORMAL),CC_COMMAND,CC_CMD_SET_STATE,1);
	}

void UnwrapMod::ExitNormalAlign()
	{	
//	iAlignButton->SetCheck(FALSE );
//	SendMessage(GetDlgItem(hParams,IDC_UNWRAP_ALIGN_TO_NORMAL),CC_COMMAND,CC_CMD_SET_STATE,0);
	}



//--- Named selection sets -----------------------------------------

int UnwrapMod::FindSet(TSTR &setName) {
	for (int i=0; i<namedSel.Count(); i++) {
		if (setName == *namedSel[i]) return i;
	}
	return -1;
}

DWORD UnwrapMod::AddSet(TSTR &setName) {
	DWORD id = 0;
	TSTR *name = new TSTR(setName);
	namedSel.Append(1,&name);
	BOOL found = FALSE;
	while (!found) {
		found = TRUE;
		for (int i=0; i<ids.Count(); i++) {
			if (ids[i]!=id) continue;
			id++;
			found = FALSE;
			break;
		}
	}
	ids.Append(1,&id);
	return id;
}

void UnwrapMod::RemoveSet(TSTR &setName) {
	int i = FindSet(setName);
	if (i<0) return;
	delete namedSel[i];
	namedSel.Delete(i,1);
	ids.Delete(i,1);
}

void UnwrapMod::ClearSetNames() {
	for (int i=0; i<3; i++) {
		for (int j=0; j<namedSel.Count(); j++) {
			delete namedSel[j];
			namedSel[j] = NULL;
		}
	}
}

void UnwrapMod::ActivateSubSelSet(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	int index = FindSet (setName);	
	if (index<0 || !ip) return;

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) 
		{
		MeshTopoData *meshData = (MeshTopoData*)mcList[i]->localData;
		if (!meshData) continue;
//		if (theHold.Holding() && !meshData->held) theHold.Put(new UnwrapRestore(this,meshData));

		BitArray *set = NULL;

		set = meshData->fselSet.GetSet(ids[index]);
		if (set) 
			{
			if (set->GetSize()!=meshData->faceSel.GetSize()) 
				{
				set->SetSize(meshData->faceSel.GetSize(),TRUE);
				}
			meshData->SetFaceSel (*set, this, ip->GetTime());
			UpdateFaceSelection(*set);
			InvalidateView();
			UpdateWindow(hWnd);
			}
		}
	
	nodes.DisposeTemporary();
	LocalDataChanged ();
	ip->RedrawViews(ip->GetTime());
}

void UnwrapMod::NewSetFromCurSel(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	DWORD id = -1;
	int index = FindSet(setName);
	if (index<0) id = AddSet(setName);
	else id = ids[index];

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		MeshTopoData *meshData = (MeshTopoData*)mcList[i]->localData;
		if (!meshData) continue;
		
		BitArray *set = NULL;

			if (index>=0 && (set = meshData->fselSet.GetSet(id))) {
				*set = meshData->faceSel;
			} else meshData->fselSet.AppendSet(meshData->faceSel,id);
		}
		
	nodes.DisposeTemporary();
}

void UnwrapMod::RemoveSubSelSet(TSTR &setName) {
	int index = FindSet (setName);
	if (index<0 || !ip) return;		

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	DWORD id = ids[index];

	for (int i = 0; i < mcList.Count(); i++) {
		MeshTopoData *meshData = (MeshTopoData*)mcList[i]->localData;
		if (!meshData) continue;		

//				if (theHold.Holding()) theHold.Put(new DeleteSetRestore(&meshData->fselSet,id));
				meshData->fselSet.RemoveSet(id);
		}
	
//	if (theHold.Holding()) theHold.Put(new DeleteSetNameRestore(&(namedSel[nsl]),this,&(ids[nsl]),id));
	RemoveSet (setName);
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
}

void UnwrapMod::SetupNamedSelDropDown() {


	ip->ClearSubObjectNamedSelSets();
	for (int i=0; i<namedSel.Count(); i++)
		ip->AppendSubObjectNamedSelSet(*namedSel[i]);
//	if (namedSel.Count() >= 0)
//		ip->SetCurNamedSelSet(*namedSel[namedSel.Count()]);
}

int UnwrapMod::NumNamedSelSets() {
	return namedSel.Count();
}

TSTR UnwrapMod::GetNamedSelSetName(int i) {
	return *namedSel[i];
}


void UnwrapMod::SetNamedSelSetName(int i,TSTR &newName) {
//	if (theHold.Holding()) theHold.Put(new SetNameRestore(&namedSel,this,&ids,ids[i]));
	*namedSel[i] = newName;
}

void UnwrapMod::NewSetByOperator(TSTR &newName,Tab<int> &sets,int op) {
	ModContextList mcList;
	INodeTab nodes;
	
	DWORD id = AddSet(newName);
//	if (theHold.Holding()) theHold.Put(new AppendSetNameRestore(this,&namedSel,&ids));

	BOOL delSet = TRUE;
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		MeshTopoData *meshData = (MeshTopoData*)mcList[i]->localData;
		if (!meshData) continue;
	
		BitArray bits;
		GenericNamedSelSetList *setList;

		setList = &meshData->fselSet; break;			
				

		bits = (*setList)[sets[0]];

		for (int i=1; i<sets.Count(); i++) {
			switch (op) {
			case NEWSET_MERGE:
				bits |= (*setList)[sets[i]];
				break;

			case NEWSET_INTERSECTION:
				bits &= (*setList)[sets[i]];
				break;

			case NEWSET_SUBTRACT:
				bits &= ~((*setList)[sets[i]]);
				break;
			}
		}
		if (bits.NumberSet()) delSet = FALSE;

		setList->AppendSet(bits,id);
//		if (theHold.Holding()) theHold.Put(new AppendSetRestore(setList));
	}
	if (delSet) RemoveSubSelSet(newName);
}

void UnwrapMod::LocalDataChanged() {
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	if (ip && editMod==this) {
	//	SetNumSelLabel();
		ip->ClearCurNamedSelSet();
	}
}

void UnwrapMod::SetNumSelLabel() {	
	TSTR buf;
	int num = 0, which;

	if (!hParams) return;

	ModContextList mcList;
	INodeTab nodes;

	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		MeshTopoData *meshData = (MeshTopoData*)mcList[i]->localData;
		if (!meshData) continue;

		num += meshData->faceSel.NumberSet();
		if (meshData->faceSel.NumberSet() == 1) {
			for (which=0; which<meshData->faceSel.GetSize(); which++) if (meshData->faceSel[which]) break;
			}
	}

/*
	if (num==1) buf.printf (GetString(IDS_EM_WHICHFACESEL), which+1);
	else buf.printf(GetString(IDS_RB_NUMFACESELP),num);
	SetDlgItemText(hParams,IDC_MS_NUMBER_SEL,buf);
*/
}

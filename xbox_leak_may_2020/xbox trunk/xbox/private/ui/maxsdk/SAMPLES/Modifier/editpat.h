
/**********************************************************************
 *<
	FILE: editpat.h

	DESCRIPTION:  Edit Patch OSM

	CREATED BY: Tom Hudson, Dan Silva & Rolf Berteig

	HISTORY: created 23 June 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/


#ifndef __EDITPATCH_H__
#define __EDITPATCH_H__

#include "namesel.h"
#include "nsclip.h"
#include "sbmtlapi.h"

#define Alert(x) MessageBox(GetActiveWindow(),x,_T("Alert"),MB_OK);

#define EDITPAT_CHANNELS (PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|PART_DISPLAY|PART_TOPO|TEXMAP_CHANNEL)

// These are values for selLevel.
#define EP_OBJECT	0
#define EP_VERTEX	1
#define EP_EDGE		2
#define EP_PATCH	3

// Named selection set levels:
#define EP_NS_VERTEX 0
#define EP_NS_EDGE 1
#define EP_NS_PATCH 2
// Conversion from selLevel to named selection level:
static int namedSetLevel[] = { EP_NS_VERTEX, EP_NS_VERTEX, EP_NS_EDGE, EP_NS_PATCH };
static int namedClipLevel[] = { CLIP_P_VERT, CLIP_P_VERT, CLIP_P_EDGE, CLIP_P_PATCH };

#define MAX_MATID	0xffff

#define UNDEFINED	0xffffffff

#define CID_EPM_BIND	CID_USER + 203
#define CID_EPM_EXTRUDE	CID_USER + 204
#define CID_EPM_BEVEL	CID_USER + 205

class EditPatchMod;

class EPM_BindMouseProc : public MouseCallBack {
	private:
		EditPatchMod *pobj;
		IObjParam *ip;
		IPoint2 om;
		BitArray knotList;
		PatchMesh *pMesh;
	
	protected:
		HCURSOR GetTransformCursor();
		BOOL HitAKnot(ViewExp *vpt, IPoint2 *p, int *vert);
		BOOL HitASegment(ViewExp *vpt, IPoint2 *p, int *Seg);

		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags, int subType );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		

	public:
		EPM_BindMouseProc(EditPatchMod* spl, IObjParam *i) { pobj=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
	};



class EPM_BindCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		EPM_BindMouseProc eproc;
		EditPatchMod* pobj;
//		int type; // See above

	public:
		EPM_BindCMode(EditPatchMod* spl, IObjParam *i) :
			fgProc((ReferenceTarget*)spl), eproc(spl,i) {pobj=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_EP_BIND; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=2; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
//		void SetType(int type) { this->type = type; eproc.SetType(type); }
	};

class EPM_ExtrudeMouseProc : public MouseCallBack {
private:
	MoveTransformer moveTrans;
	EditPatchMod *po;
	Interface *ip;
	IPoint2 om;
	Point3 ndir;
public:
	EPM_ExtrudeMouseProc(EditPatchMod* o, IObjParam *i) : moveTrans(i) {po=o;ip=i;}
	int proc(HWND hwnd, int msg, int point, int flags, IPoint2 m);
};


class EPM_ExtrudeSelectionProcessor : public GenModSelectionProcessor {
protected:
	HCURSOR GetTransformCursor();
public:
	EPM_ExtrudeSelectionProcessor(EPM_ExtrudeMouseProc *mc, EditPatchMod *o, IObjParam *i) 
		: GenModSelectionProcessor(mc,(BaseObject*) o,i) {}
};


class EPM_ExtrudeCMode : public CommandMode {
private:
	ChangeFGObject fgProc;
	EPM_ExtrudeSelectionProcessor mouseProc;
	EPM_ExtrudeMouseProc eproc;
	EditPatchMod* po;

public:
	EPM_ExtrudeCMode(EditPatchMod* o, IObjParam *i) :
		fgProc((ReferenceTarget *)o), mouseProc(&eproc,o,i), eproc(o,i) {po=o;}
	int Class() { return MODIFY_COMMAND; }
	int ID() { return CID_EPM_EXTRUDE; }
	MouseCallBack *MouseProc(int *numPoints) { *numPoints=2; return &mouseProc; }
	ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
	BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
	void EnterMode();
	void ExitMode();
};




class EPM_BevelMouseProc : public MouseCallBack {
private:
	MoveTransformer moveTrans;
	EditPatchMod *po;
	Interface *ip;
	IPoint2 om;
	
public:
	EPM_BevelMouseProc(EditPatchMod* o, IObjParam *i) : moveTrans(i) {po=o;ip=i;}
	int proc(HWND hwnd, int msg, int point, int flags, IPoint2 m);
};


class EPM_BevelSelectionProcessor : public GenModSelectionProcessor {
protected:
	HCURSOR GetTransformCursor();
public:
	EPM_BevelSelectionProcessor(EPM_BevelMouseProc *mc, EditPatchMod *o, IObjParam *i) 
		: GenModSelectionProcessor(mc,(BaseObject*) o,i) {}
};


class EPM_BevelCMode : public CommandMode {
private:
	ChangeFGObject fgProc;
	EPM_BevelSelectionProcessor mouseProc;
	EPM_BevelMouseProc eproc;
	EditPatchMod* po;

public:
	EPM_BevelCMode(EditPatchMod* o, IObjParam *i) :
		fgProc((ReferenceTarget *)o), mouseProc(&eproc,o,i), eproc(o,i) {po=o;}
	int Class() { return MODIFY_COMMAND; }
	int ID() { return CID_EPM_BEVEL; }
	MouseCallBack *MouseProc(int *numPoints) { *numPoints=3; return &mouseProc; }
	ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
	BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
	void EnterMode();
	void ExitMode();
};

class VertInsertRecord;
class PickPatchAttach;

class EditPatchMod : public Modifier, IPatchOps, IPatchSelect, ISubMtlAPI, AttachMatDlgUser {
	friend class EPTempData;
	friend class EditPatchData;
	friend class XFormProc;
	friend class PatchRestore;
	friend class PVertexRightMenu;
	friend class PatchRightMenu;
	friend class PickPatchAttach;

	public:
		static HWND hSelectPanel, hOpsPanel, hSurfPanel;
		static BOOL rsSel, rsOps, rsSurf;	// rollup states (FALSE = rolled up)
		static IObjParam *ip;		
		
		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;
		static SelectModBoxCMode *selectMode;
		static ISpinnerControl *weldSpin;
		static ISpinnerControl *stepsSpin;
//3-18-99 to suport render steps and removal of the mental tesselator
		static ISpinnerControl *stepsRenderSpin;
		static PickPatchAttach pickCB;
		static BOOL patchUIValid;

//watje command mode for the extrude and beevl		
		static EPM_ExtrudeCMode *extrudeMode;
		static EPM_BevelCMode *bevelMode;
		static EPM_BindCMode *bindMode;

		// for the tessellation controls
		static BOOL settingViewportTess;  // are we doing viewport or renderer
		static BOOL settingDisp;          // if we're doign renderer is it mesh or displacmenent
		static ISpinnerControl *uSpin;
		static ISpinnerControl *vSpin;
		static ISpinnerControl *edgeSpin;
		static ISpinnerControl *angSpin;
		static ISpinnerControl *distSpin;
		static ISpinnerControl *mergeSpin;
		static ISpinnerControl *matSpin;
		static int attachMat;
		static BOOL condenseMat;

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message ) { return REF_SUCCEED; }
		
		int selLevel;

		// RB:named sel sets
		BOOL namedSelNeedsFixup;	// TRUE for pre-r3 files
		Tab<TSTR*> namedSel[3];
		int FindSet(TSTR &setName,int level);
		void AddSet(TSTR &setName,int level);
		void RemoveSet(TSTR &setName,int level);
		void RemoveAllSets();
		void ClearSetNames();

		// Remembered info
		PatchMesh *rememberedPatch;	// NULL if using all selected patches
		int rememberedIndex;
		int rememberedData;

		BOOL displaySurface;
		BOOL displayLattice;
		int meshSteps;
//3-18-99 to suport render steps and removal of the mental tesselator
		int meshStepsRender;
		BOOL showInterior;

		BOOL meshAdaptive;	// Future use (Not used now)
		TessApprox viewTess; // for GAP tessellation
		TessApprox prodTess;
		TessApprox dispTess;
		BOOL mViewTessNormals;	// use normals from the tesselator
		BOOL mProdTessNormals;	// use normals from the tesselator
		BOOL mViewTessWeld;	// Weld the mesh after tessellation
		BOOL mProdTessWeld;	// Weld the mesh after tessellation
		BOOL propagate;

		BOOL inExtrude;
		BOOL inBevel;


		EditPatchMod();
		~EditPatchMod();

		Interval LocalValidity(TimeValue t);
		ChannelMask ChannelsUsed()  { return EDITPAT_CHANNELS; }
		ChannelMask ChannelsChanged() 	{ return EDITPAT_CHANNELS; }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);
		Class_ID InputType() { return Class_ID(PATCHOBJ_CLASS_ID,0); }
		
		int CompMatrix(TimeValue t, ModContext& mc, Matrix3& tm, Interval& valid);
		
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= TSTR(_T("EditPatchMod")); }
		Class_ID ClassID() { return Class_ID(EDITPATCH_CLASS_ID,0);}
		void* GetInterface(ULONG id);

		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);

		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		int SubObjectIndex(HitRecord *hitRec);

 		BOOL DependOnTopology(ModContext &mc);

		// Generic xform procedure.
		void XFormVerts( XFormProc *xproc, TimeValue t, Matrix3& partm, Matrix3& tmAxis );

		// Specialized xform for bezier handles
		void XFormHandles( XFormProc *xproc, TimeValue t, Matrix3& partm, Matrix3& tmAxis, int object, int handleIndex );

		// Affine transform methods		
		void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		void Rotate( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE );
		void Scale( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );

		// The following is called before the first Move(), Rotate() or Scale() call
		void TransformStart(TimeValue t);

		// The following is called after the user has completed the Move, Rotate or Scale operation and
		// the undo object has been accepted.
		void TransformFinish(TimeValue t);		

		// The following is called when the transform operation is cancelled by a right-click and the undo
		// has been cancelled.
		void TransformCancel(TimeValue t);		

		BOOL SupportsNamedSubSels() {return TRUE;}
		void ActivateSubSelSet(TSTR &setName);
		void NewSetFromCurSel(TSTR &setName);
		void RemoveSubSelSet(TSTR &setName);
		void SetupNamedSelDropDown();
		int NumNamedSelSets();
		TSTR GetNamedSelSetName(int i);
		void SetNamedSelSetName(int i,TSTR &newName);
		void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);
		BOOL GetUniqueSetName(TSTR &name);
		int SelectNamedSet();
		void NSCopy();
		void NSPaste();
		void MaybeFixupNamedSels();

//watje 12-10-98
		void DoHide(int type); 
		void DoUnHide(); 
		void DoPatchHide(); 
		void DoVertHide(); 
		void DoEdgeHide(); 

		void DoAddHook(PatchMesh *pMesh, int vert1, int seg1) ;
		void DoRemoveHook(); 

//watje bevel and extrusion stuff
		void DoExtrude() ;
		void BeginExtrude(TimeValue t); 	
		void EndExtrude (TimeValue t, BOOL accept=TRUE);		
		void Extrude( TimeValue t, float amount, BOOL useLocalNorm );

		
		void DoBevel() ;
		void BeginBevel(TimeValue t); 	
		void EndBevel (TimeValue t, BOOL accept=TRUE);		
		void Bevel( TimeValue t, float amount, BOOL smoothStart, BOOL smoothEnd );




		void DoDeleteSelected();
		void DoVertDelete();
		void DoEdgeDelete();
		void DoPatchDelete();
		void DoPatchAdd(int type);
		void DoSubdivide(int type);
		void DoEdgeSubdivide();
		void DoPatchSubdivide();
		void DoVertWeld();
		void DoPatchDetach(int copy, int reorient);

		void ClearPatchDataFlag(ModContextList& mcList,DWORD f);
		void DeletePatchDataTempData();		
		void CreatePatchDataTempData();

		int NumRefs() { return 0; }
		RefTargetHandle GetReference(int i) { return NULL; }
		void SetReference(int i, RefTargetHandle rtarg) {}

		void ChangeRememberedPatch(int type);
		void ChangeSelPatches(int type);
		int RememberPatchThere(HWND hWnd, IPoint2 m);
		void SetRememberedPatchType(int type);
		void ChangeRememberedVert(int type);
		void ChangeSelVerts(int type);
		int RememberVertThere(HWND hWnd, IPoint2 m);
		void SetRememberedVertType(int type);

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
		IOResult SaveLocalData(ISave *isave, LocalModData *ld);
		IOResult LoadLocalData(ILoad *iload, LocalModData **pld);
		IOResult LoadNamedSelChunk(ILoad *iload,int level);

		CreateMouseCallBack* GetCreateMouseCallBack() { return NULL; } 
		void BeginEditParams( IObjParam  *ip, ULONG flags, Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags, Animatable *next );
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() { return GetString(IDS_TH_EDITPATCH); }
		void ActivateSubobjSel(int level, XFormModes& modes );
		int NeedUseSubselButton() { return 0; }
		void SelectSubComponent( HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert );
		void ClearSelection(int selLevel);
		void SelectAll(int selLevel);
		void InvertSelection(int selLevel);
		
		void SetDisplaySurface(BOOL sw);
		void SetDisplayLattice(BOOL sw);
		void SetPropagate(BOOL sw);		
		BOOL GetPropagate() {return propagate;}		
		void SetMeshSteps(int count);
		int GetMeshSteps() { return meshSteps; }
//3-18-99 to suport render steps and removal of the mental tesselator
		void SetMeshStepsRender(int count);
		int GetMeshStepsRender() { return meshStepsRender; }
		void SetShowInterior(BOOL si);
		BOOL GetShowInterior() { return showInterior; }

// Future use (Not used now)
//		void SetMeshAdaptive(BOOL sw);
		void SetViewTess(TessApprox &tess);
		TessApprox GetViewTess() { return viewTess; }
		void SetProdTess(TessApprox &tess);
		TessApprox GetProdTess() { return prodTess; }
		void SetDispTess(TessApprox &tess);
		TessApprox GetDispTess() { return dispTess; }
		void SetTessUI(HWND hDlg, TessApprox *tess);
		BOOL GetViewTessNormals() { return mViewTessNormals; }
		void SetViewTessNormals(BOOL use);
		BOOL GetProdTessNormals() { return mProdTessNormals; }
		void SetProdTessNormals(BOOL use);
		BOOL GetViewTessWeld() { return mViewTessWeld; }
		void SetViewTessWeld(BOOL weld);
		BOOL GetProdTessWeld() { return mProdTessWeld; }
		void SetProdTessWeld(BOOL weld);

		// Get the commonality of material index for the selection (-1 indicates no commonality)
		int GetSelMatIndex();
		void SetSelMatIndex(int index);
		void SelectByMat(int index,BOOL clear);

		// Smoothing
		DWORD GetSelSmoothBits(DWORD &invalid);
		DWORD GetUsedSmoothBits();
		void SelectBySmoothGroup(DWORD bits,BOOL clear);
		void SetSelSmoothBits(DWORD bits,DWORD which);

		void PatchSelChanged();

		// from AttachMatDlgUser
		int GetAttachMat() { return attachMat; }
		void SetAttachMat(int value) { attachMat = value; }
		BOOL GetCondenseMat() { return condenseMat; }
		void SetCondenseMat(BOOL sw) { condenseMat = sw; }

		int DoAttach(INode *node, PatchMesh *attPatch, bool & canUndo);

		// Store current topology in the PatchObject
		void RecordTopologyTags();

		// Re-match named selection sets, etc. with changed topology (Call RecordTopologyTags
		// before making the changes to the shape, then call this)
		void ResolveTopoChanges();

		void RescaleWorldUnits(float f);

		int GetSubobjectLevel();
		void SetSubobjectLevel(int level);
		void RefreshSelType();
		void UpdateSelectDisplay();
		void SetSelDlgEnables();
		void SetOpsDlgEnables();
		void SetSurfDlgEnables();
		void SelectionChanged();
		void InvalidateSurfaceUI();
		BitArray *GetLevelSelectionSet(PatchMesh *patch);

		// patch select and operations interfaces, JBW 2/2/99
		void StartCommandMode(patchCommandMode mode);
		void ButtonOp(patchButtonOp opcode);

		DWORD GetSelLevel();
		void SetSelLevel(DWORD level);
		void LocalDataChanged();
	
		// ISubMtlAPI methods:
		MtlID	GetNextAvailMtlID(ModContext* mc);
		BOOL	HasFaceSelection(ModContext* mc);
		void	SetSelFaceMtlID(ModContext* mc, MtlID id, BOOL bResetUnsel = FALSE);
		int		GetSelFaceUniqueMtlID(ModContext* mc);
		int		GetSelFaceAnyMtlID(ModContext* mc);
		int		GetMaxMtlID(ModContext* mc);
	};

class PickPatchAttach : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		EditPatchMod *ep;
		
		PickPatchAttach() {ep=NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);

		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);

		HCURSOR GetHitCursor(IObjParam *ip);

		BOOL Filter(INode *node);
		
		PickNodeCallback *GetFilter() {return this;}

		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}
	};

// Table to convert selLevel values to patch selLevel flags.
const int patchLevel[] = {PATCH_OBJECT,PATCH_VERTEX,PATCH_EDGE,PATCH_PATCH};

// Get display flags based on selLevel.
const DWORD patchLevelDispFlags[] = {0,DISP_VERTTICKS|DISP_SELVERTS,DISP_SELEDGES,DISP_SELPATCHES};

// For hit testing...
static int patchHitLevel[] = {0,SUBHIT_PATCH_VERTS | SUBHIT_PATCH_VECS,SUBHIT_PATCH_EDGES,SUBHIT_PATCH_PATCHES};

class EditPatchClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE ) { return new EditPatchMod; }
	const TCHAR *	ClassName() { return GetString(IDS_TH_EDITPATCH_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(EDITPATCH_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFEDIT);}
	void			ResetClassParams(BOOL fileReset);
	};

typedef Tab<Point3> Point3Tab;

class XFormProc {
	public:
		virtual Point3 proc(Point3& p, Matrix3 &mat, Matrix3 &imat)=0;
		virtual void SetMat( Matrix3& mat ) {}
	};

class MoveXForm : public XFormProc {
	private:
		Point3 delta, tdelta;		
	public:
		Point3 proc(Point3& p, Matrix3 &mat, Matrix3 &imat) 
			{ return p + tdelta; }
		void SetMat( Matrix3& mat ) 
			{ tdelta = VectorTransform(Inverse(mat),delta); }
		MoveXForm(Point3 d) { delta = d; }
	};

class RotateXForm : public XFormProc {
	private:
		Matrix3 rot, trot;
	public:
		Point3 proc(Point3& p, Matrix3 &mat, Matrix3 &imat) 
			{ return (trot*p)*imat; }
		void SetMat( Matrix3& mat ) 
			{ trot = mat * rot; }
		RotateXForm(Quat q) { q.MakeMatrix(rot); }
	};

class ScaleXForm : public XFormProc {
	private:
		Matrix3 scale, tscale;
	public:
		Point3 proc(Point3& p, Matrix3 &mat, Matrix3 &imat) 
			{ return (p*tscale)*imat; }
		void SetMat( Matrix3& mat ) 
			{ tscale = mat*scale; }
		ScaleXForm(Point3 s) { scale = ScaleMatrix(s); }
	};

typedef Tab<int> IntTab;

// General-purpose patch point table -- Maintains point table for each of n polygons
class PatchPointTab {
	public:
		Point3Tab ptab;	// Patch mesh points
		Point3Tab vtab;	// Patch mesh vectors
		IntTab pttab;	// Patch point types
		PatchPointTab();
		~PatchPointTab();
		void Empty();
		void Zero();
		void MakeCompatible(PatchMesh& patch, BOOL clear=TRUE);
		PatchPointTab& operator=(PatchPointTab& from);
		BOOL IsCompatible(PatchMesh &patch);
		void RescaleWorldUnits(float f);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

class PatchVertexDelta {
	public:
		PatchPointTab dtab;

		void SetSize(PatchMesh &patch, BOOL load=TRUE);
		void Empty() { dtab.Empty(); }
		void Zero() { dtab.Zero(); }
		void SetVert(int i, const Point3& p) { dtab.ptab[i] = p; }
		void SetVertType(int i, int k) { dtab.pttab[i] = k; }
		void SetVec(int i, const Point3& p) { dtab.vtab[i] = p; }
		void MoveVert(int i, const Point3& p) { dtab.ptab[i] += p; }
		void MoveVec(int i, const Point3& p) { dtab.vtab[i] += p; }
		void Apply(PatchMesh& patch);
		void UnApply(PatchMesh& patch);
		PatchVertexDelta& operator=(PatchVertexDelta& from) { dtab = from.dtab; return *this; }
		void ApplyHandlesAndZero(PatchMesh &patch, int handleVert);
		BOOL IsCompatible(PatchMesh &patch) { return dtab.IsCompatible(patch); }
		void RescaleWorldUnits(float f) { dtab.RescaleWorldUnits(f); }
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

class AdjEdgeList;
class EPTempData;

/*-------------------------------------------------------------------*/

// Class for recording changes -- This is used to reconstruct an object from the original whenever
// the modifier is re-entered or whenever the system needs to reconstruct an object's cache.  This may be
// slow if a lot of changes have been recorded, but it's about the only way to properly reconstruct an object
// because the output of one operation becomes the input of the next.

// These are used as follows:
// When a user makes a modification to an object, a StartChangeGroup call needs to be made to the EditPatchData
// object.  Then a change record needs to be added for each sub-operation that makes up the modification.  These
// records are owned by the EditPatchData object, but they should also be referenced by the undo object for that
// operation.  If an undo is done, ownership of the modification record transfers to the undo/redo object and the
// record is REMOVED (NOT DELETED) from the EditPatchData object.  This keeps the record around for a redo operation
// but removes it from the list of records for the modifier.  If the undo is redone, ownership transfers back to
// the modifier, when it is re-added to the modification record list.

// Note that this class contains load and save methods, necessary because the modifier needs to be able to save
// and load them.  When you subclass off of this, be sure your load and save methods call the base class's first!

class PatchRestore;

class PModRecord {
	public:
		virtual BOOL Redo(PatchMesh *patch,int reRecord)=0;
		virtual IOResult Load(ILoad *iload)=0;
	};

typedef PModRecord* PPModRecord;
typedef Tab<PPModRecord> ModRecordTab;

/*-------------------------------------------------------------------*/

// Here are the types of modification records we use!

#define CLEARVERTSELRECORD_CHUNK	0x2000
#define SETVERTSELRECORD_CHUNK		0x2001
#define INVERTVERTSELRECORD_CHUNK	0x2002
#define CLEAREDGESELRECORD_CHUNK	0x2005
#define SETEDGESELRECORD_CHUNK		0x2006
#define INVERTEDGESELRECORD_CHUNK	0x2007
#define CLEARPATCHSELRECORD_CHUNK	0x2010
#define SETPATCHSELRECORD_CHUNK		0x2011
#define INVERTPATCHSELRECORD_CHUNK	0x2012
#define VERTSELRECORD_CHUNK			0x2020
#define EDGESELRECORD_CHUNK			0x2025
#define PATCHSELRECORD_CHUNK		0x2030
#define VERTMOVERECORD_CHUNK		0x2040
#define PATCHDELETERECORD_CHUNK		0x2050
#define VERTDELETERECORD_CHUNK		0x2060
#define PATCHCHANGERECORD_CHUNK		0x2070
#define VERTCHANGERECORD_CHUNK		0x2080
#define PATCHADDRECORD_CHUNK		0x2090
#define EDGESUBDIVIDERECORD_CHUNK	0x20A0
#define PATCHSUBDIVIDERECORD_CHUNK	0x20B0
#define VERTWELDRECORD_CHUNK		0x20C0
#define PATTACHRECORD_CHUNK			0x20D0
#define PATCHDETACHRECORD_CHUNK		0x20E0
#define PATCHMTLRECORD_CHUNK		0x20F0
										 
class ClearPVertSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class SetPVertSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class InvertPVertSelRecord : public PModRecord {
	public:
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class ClearPEdgeSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class SetPEdgeSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class InvertPEdgeSelRecord : public PModRecord {
	public:
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class ClearPatchSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class SetPatchSelRecord : public PModRecord {
	public:
		BitArray sel;	// Old state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class InvertPatchSelRecord : public PModRecord {
	public:
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PVertSelRecord : public PModRecord {
	public:
		BitArray oldSel;	// Old state
		BitArray newSel;	// New state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PEdgeSelRecord : public PModRecord {
	public:
		BitArray oldSel;	// Old state
		BitArray newSel;	// New state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchSelRecord : public PModRecord {
	public:
		BitArray oldSel;	// Old state
		BitArray newSel;	// New state
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PVertMoveRecord : public PModRecord {
	public:
		PatchVertexDelta delta;	// Position changes for each vertex (Wasteful!  Change later?)
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchDeleteRecord : public PModRecord {
	public:
		PatchMesh oldPatch;		// How the spline looked before the delete
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PVertDeleteRecord : public PModRecord {
	public:
		PatchMesh oldPatch;		// How the patch looked before the delete
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchChangeRecord : public PModRecord {
	public:
		PatchMesh oldPatch;		// How the patch mesh looked before the change
		int index;
		int type;
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PVertChangeRecord : public PModRecord {
	public:
		PatchMesh oldPatch;		// How the patch mesh looked before the change
		int index;
		int type;
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchAddRecord : public PModRecord {
	public:
		BOOL postWeld;			// Present in MAX 2.0 and up
		int type;				// 3 or 4 sides!
		PatchMesh oldPatch;		// How the patch looked before the addition
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class EdgeSubdivideRecord : public PModRecord {
	public:
		BOOL propagate;			// Carry around entire patch mesh?
		PatchMesh oldPatch;		// How the patch looked before the addition
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchSubdivideRecord : public PModRecord {
	public:
		BOOL propagate;			// Carry around entire patch mesh?
		PatchMesh oldPatch;		// How the patch looked before the addition
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PVertWeldRecord : public PModRecord {
	public:
		float thresh;			// Weld threshold
		BOOL propagate;			// Carry around entire patch mesh?
		PatchMesh oldPatch;		// How the patch looked before the addition
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PAttachRecord : public PModRecord {
	public:
		PatchMesh attPatch;			// The patch we're attaching
		int oldPatchCount;		// The number of splines present before attaching
		int mtlOffset;
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

class PatchDetachRecord : public PModRecord {
	public:
		int copy;
		PatchMesh oldPatch;
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

typedef Tab<MtlID> MtlIDTab;

class PatchMtlRecord : public PModRecord {
	public:
		MtlIDTab materials;		// Materials from selected patches
		MtlID index;				// New material index assigned
		BOOL Redo(PatchMesh *patch,int reRecord);
		IOResult Load(ILoad *iload);
	};

/*-------------------------------------------------------------------*/

// Vertex Mapping class -- Gives mapping from vert in original patch to
// vert in modified patch

class EPMapVert {
	public:
		BOOL originalStored;
		int vert;
		Point3 original;	// Original point location
		Point3 delta;		// The delta we've applied
		EPMapVert() { originalStored = FALSE; vert = 0; original = Point3(0,0,0); delta = Point3(0,0,0); }
		EPMapVert(int v, Point3 &o, Point3 &d) { vert = v; original = o; delta = d; originalStored = TRUE; }
	};

class EPVertMapper {
	public:
		int verts;
		EPMapVert *vertMap;
		int vecs;
		EPMapVert *vecMap;
		EPVertMapper() { verts = vecs = 0; vertMap = vecMap = NULL; }
		~EPVertMapper();
		// Set up remap data structures.
		void Build(PatchMesh &patch);
		// Update the deltas we have stored, if necessary and apply to output patch mesh.
		// This is in response to the original shape changing
		void UpdateAndApplyDeltas(PatchMesh &inPatch, PatchMesh &outPatch);
		// Recompute the deltas we have stored
		// This is done after the modifier's user interaction changes the shape
		void RecomputeDeltas(PatchMesh &patch);
		// Record the topology tags in the specified shape
		void RecordTopologyTags(PatchMesh &patch);
		// Update the topology tag mapping
		void UpdateMapping(PatchMesh &patch);
		EPVertMapper& operator=(EPVertMapper &from);
		void RescaleWorldUnits(float f);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

/*-------------------------------------------------------------------*/

// EditPatchData flags
#define EPD_BEENDONE			(1<<0)
#define EPD_UPDATING_CACHE		(1<<1)
#define EPD_HASDATA				(1<<2)
#define EMD_HELD				(1<<3) // equivalent to A_HELD

// This is the data that each mod app will have.
class EditPatchData : public LocalModData {
	public:
		BOOL handleFlag;
		int handleVert;

		// Stuff we need to have for the patch's mesh conversion -- These are
		// Here because they're kind of a global change -- not undoable.
		int meshSteps;
//3-18-99 to suport render steps and removal of the mental tesselator
		int meshStepsRender;
		BOOL showInterior;

		BOOL meshAdaptive;	// Future use (Not used now)
		TessApprox viewTess;
		TessApprox prodTess;
		TessApprox dispTess;
		BOOL mViewTessNormals;	// use normals from the tesselator
		BOOL mProdTessNormals;	// use normals from the tesselator
		BOOL mViewTessWeld;	// Weld the mesh after tessellation
		BOOL mProdTessWeld;	// Weld the mesh after tessellation
		BOOL displaySurface;
		BOOL displayLattice;

		DWORD flags;

		// This records the changes to the incoming object.
		ModRecordTab changes;

		// A pointer to the change record's vertex delta object
		PatchVertexDelta vdelta;

		// RB: Named selection set lists
		GenericNamedSelSetList vselSet;  // Vertex
		GenericNamedSelSetList eselSet;  // Edge
		GenericNamedSelSetList pselSet;  // Patch

		// While an object is being edited, this exists.
		EPTempData *tempData;

		// The knot mapping for the edited patch
		EPVertMapper vertMap; 

		// The final edited patch
		PatchMesh finalPatch;

		EditPatchData(EditPatchMod *mod);
		EditPatchData(EditPatchData& emc);
		
		// Applies modifications to a patchObject
		void Apply(TimeValue t,PatchObject *patchOb,int selLevel);

		// Invalidates any caches affected by the change.
		void Invalidate(PartID part,BOOL meshValid=TRUE);
		
		// If this is the first edit, then the delta arrays will be allocated
		void BeginEdit(TimeValue t);

		LocalModData *Clone() { return new EditPatchData(*this); }
		
		void SetFlag(DWORD f,BOOL on) 
			{ 
			if ( on ) {
				flags|=f;
			} else {
				flags&=~f; 
				}
			}
		DWORD GetFlag(DWORD f) { return flags&f; }

		EPTempData *TempData(EditPatchMod *mod);

		// Change recording functions:
		void ClearHandleFlag() { handleFlag = FALSE; }
		void SetHandleFlag(int vert) { handleVert = vert; handleFlag = TRUE; }
		BOOL DoingHandles() { return handleFlag; }
		void ApplyHandlesAndZero(PatchMesh &patch) { vdelta.ApplyHandlesAndZero(patch, handleVert); }
		void RescaleWorldUnits(float f);

		// MAXr3: New recording system
		void RecordTopologyTags(PatchMesh *patch);
		void UpdateChanges(PatchMesh *patch, BOOL checkTopology=TRUE);

		// Named selection set access
		GenericNamedSelSetList &GetSelSet(EditPatchMod *mod);	// Get the one for the current subobject selection level
		GenericNamedSelSetList &GetSelSet(int level);	// Get the one for the specified subobject selection level

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

// My generic restore class

class PatchRestore : public RestoreObj {
	public:
		PatchMesh oldPatch, newPatch;
		BOOL gotRedo;
		TimeValue t;
		EditPatchData *epd;
		EditPatchMod *mod;
		TSTR where;
		
		PatchRestore(EditPatchData* pd, EditPatchMod* mod, PatchMesh *patch, TCHAR *id=_T(""));

		void Restore(int isUndo);
		void Redo();
		int Size() { return 1; }
		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() {
			TSTR string;
			string.printf(_T("Generic patch restore [%s]"),where);
			return string;
			}
	};

// Patch selection restore class

class PatchSelRestore : public RestoreObj {
	public:
		BitArray oldVSel, newVSel;
		BitArray oldESel, newESel;
		BitArray oldPSel, newPSel;
		BOOL gotRedo;
		TimeValue t;
		EditPatchData *epd;
		EditPatchMod *mod;

		PatchSelRestore(EditPatchData* pd, EditPatchMod* mod, PatchMesh *patch);

		void Restore(int isUndo);
		void Redo();
		int Size() { return 1; }
		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() { return TSTR(_T("Patch Select restore")); }
	};

/*-------------------------------------------------------------------*/

class EPTempData {
	private:
		PatchMesh		*patch;
		Interval 		patchValid;
		
		EditPatchMod 	*mod;
		EditPatchData 	*patchData;

	public:		
		
		~EPTempData();
		EPTempData(EditPatchMod *m,EditPatchData *md);
		void Invalidate(PartID part,BOOL meshValid=TRUE);
		
		PatchMesh		*GetPatch(TimeValue t);
		
		BOOL PatchCached(TimeValue t);
		void UpdateCache(PatchObject *patchOb);
		EditPatchMod	*GetMod() { return mod; }
	};


// Patch hit override functions

extern void SetPatchHitOverride(int value);
extern void ClearPatchHitOverride();

#endif // __EDITPATCH_H__

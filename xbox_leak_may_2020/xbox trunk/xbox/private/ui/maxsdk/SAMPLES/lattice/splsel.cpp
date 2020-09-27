/**********************************************************************
 *<
	FILE: splsel.cpp

	DESCRIPTION:  A selection modifier for splines

	CREATED BY: Audrey Peterson	

	HISTORY: 1/14/97

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "mods2.h"
#include "iparamm.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "nsclip.h"
#include "selname.h"

#define WM_UPDATE_CACHE		(WM_USER+0x287)

// This is a special override value which allows us to hit-test on
// any sub-part of a shape


NVertSelSetList::~NVertSelSetList()
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	}

void NVertSelSetList::AppendSet(ShapeVSel &nset)
	{
	ShapeVSel *n = new ShapeVSel();
	*n = nset;
	sets.Append(1,&n);
	}

void NVertSelSetList::DeleteSet(int i)
	{
	delete sets[i];
	sets.Delete(i,1);
	}

void NVertSelSetList::SetSize(BezierShape& shape)
	{
	for (int i=0; i<sets.Count(); i++) {
		sets[i]->SetSize(shape);
		}
	}

NVertSelSetList& NVertSelSetList::operator=(NVertSelSetList& from)
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	sets.SetCount(0);
	return *this;
	}

static void BitArrayDeleteSet(BitArray &ba,BitArray &set,int m)
	{
	int j = 0, sz = set.GetSize();
	if (ba.GetSize()<sz) sz = ba.GetSize();

	for (int i=0; i<sz; i++) {
		if (set[i]) continue;
		
		for (int k=0; k<m; k++) ba.Set(j*m+k,ba[i*m+k]);
		j++;
		}
	if (j*m!=ba.GetSize()) ba.SetSize(j*m,TRUE);
	}

void NVertSelSetList::DeleteSetElements(ShapeVSel &set,int m)
	{
	for (int i=0; i<sets.Count(); i++) {
		if (!(*this)[i].IsCompatible(set)) {
			(*this)[i].SetSize(set);
			}
		for(int j = 0; j < set.polys; ++j)
			BitArrayDeleteSet((*this)[i][j],set[j],m);
		}
	}

#define SELSET_SET_CHUNK		0x1000

IOResult NVertSelSetList::Load(ILoad *iload)
	{
	IOResult res;
	ShapeVSel set;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SELSET_SET_CHUNK:				
				set = ShapeVSel();
				res = set.Load(iload);				
				AppendSet(set);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult NVertSelSetList::Save(ISave *isave)
	{
	for (int i=0; i<sets.Count(); i++) {
		isave->BeginChunk(SELSET_SET_CHUNK);
		sets[i]->Save(isave);
		isave->EndChunk();
		}
	return IO_OK;
	}

NSegSelSetList::~NSegSelSetList()
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	}

void NSegSelSetList::AppendSet(ShapeSSel &nset)
	{
	ShapeSSel *n = new ShapeSSel();
	*n = nset;
	sets.Append(1,&n);
	}

void NSegSelSetList::DeleteSet(int i)
	{
	delete sets[i];
	sets.Delete(i,1);
	}

void NSegSelSetList::SetSize(BezierShape& shape)
	{
	for (int i=0; i<sets.Count(); i++) {
		sets[i]->SetSize(shape);
		}
	}

NSegSelSetList& NSegSelSetList::operator=(NSegSelSetList& from)
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	sets.SetCount(0);
	return *this;
	}

void NSegSelSetList::DeleteSetElements(ShapeSSel &set,int m)
	{
	for (int i=0; i<sets.Count(); i++) {
		if (!(*this)[i].IsCompatible(set)) {
			(*this)[i].SetSize(set);
			}
		for(int j = 0; j < set.polys; ++j)
			BitArrayDeleteSet((*this)[i][j],set[j],m);
		}
	}

#define SELSET_SET_CHUNK		0x1000
#define SELSET_NAME_CHUNK		0x1010

IOResult NSegSelSetList::Load(ILoad *iload)
	{
	IOResult res;
	ShapeSSel set;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SELSET_SET_CHUNK:				
				set = ShapeSSel();
				res = set.Load(iload);				
				AppendSet(set);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult NSegSelSetList::Save(ISave *isave)
	{
	for (int i=0; i<sets.Count(); i++) {
		isave->BeginChunk(SELSET_SET_CHUNK);
		sets[i]->Save(isave);
		isave->EndChunk();
		}
	return IO_OK;
	}

//--- Named spline selection sets --------------------------------

NPolySelSetList::~NPolySelSetList()
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	}

void NPolySelSetList::AppendSet(ShapePSel &nset)
	{
	ShapePSel *n = new ShapePSel();
	*n = nset;
	sets.Append(1,&n);
	}

void NPolySelSetList::DeleteSet(int i)
	{
	delete sets[i];
	sets.Delete(i,1);
	}

void NPolySelSetList::SetSize(BezierShape& shape)
	{
	for (int i=0; i<sets.Count(); i++) {
		sets[i]->SetSize(shape);
		}
	}

NPolySelSetList& NPolySelSetList::operator=(NPolySelSetList& from)
	{
	for (int i=0; i<sets.Count(); i++) {
		delete sets[i];
		}
	sets.SetCount(0);
	return *this;
	}

void NPolySelSetList::DeleteSetElements(ShapePSel &set,int m)
	{
	for (int i=0; i<sets.Count(); i++) {
		if (!(*this)[i].IsCompatible(set)) {
			(*this)[i].SetSize(set);
			}
		BitArrayDeleteSet((*this)[i].sel,set.sel,m);
		}
	}

#define SELSET_SET_CHUNK		0x1000
#define SELSET_NAME_CHUNK		0x1010

IOResult NPolySelSetList::Load(ILoad *iload)
	{
	IOResult res;
	ShapePSel set;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SELSET_SET_CHUNK:				
				set = ShapePSel();
				res = set.Load(iload);				
				AppendSet(set);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult NPolySelSetList::Save(ISave *isave)
	{
	for (int i=0; i<sets.Count(); i++) {
		isave->BeginChunk(SELSET_SET_CHUNK);
		sets[i]->Save(isave);
		isave->EndChunk();
		}
	return IO_OK;
	}
int splineHitOverride = 0;	// If zero, no override is done

void SetSplineHitOverride(int value) {
	splineHitOverride = value;
	}

void ClearSplineHitOverride() {
	splineHitOverride = 0;
	}
static Class_ID SPLSELECT_CLASS_ID(0x4dbe3132, 0x614357b9);

class SplineSelMod : public Modifier {	
	public:				
		DWORD selLevel;
		Tab<TSTR*> namedSel[3];		

		static IObjParam *ip;
		static IParamMap *pmapParam;
		static SplineSelMod *editMod;
		static BOOL selByVert;
		static HWND hParams;
		static SelectModBoxCMode *selectMode;
		static BOOL updateCachePosted;

		SplineSelMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s = GetString(IDS_AP_SPLINESELMOD);}  
		virtual Class_ID ClassID() { return SPLSELECT_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_AP_SPLINESELMOD);}

		// From modifier
		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE;}
		ChannelMask ChannelsChanged() {return PART_SELECT;}
		Class_ID InputType() { return Class_ID(SPLINESHAPE_CLASS_ID,0); }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t) {return FOREVER;}

		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		void ActivateSubobjSel(int level, XFormModes& modes);
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
		void ClearSelection(int selLevel);
		void SelectAll(int selLevel);
		void InvertSelection(int selLevel);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

		BOOL SupportsNamedSubSels() {return TRUE;}
		void ActivateSubSelSet(TSTR &setName);
		void NewSetFromCurSel(TSTR &setName);
		void RemoveSubSelSet(TSTR &setName);
		void SetupNamedSelDropDown();
		int NumNamedSelSets();
		TSTR GetNamedSelSetName(int i);
		void SetNamedSelSetName(int i,TSTR &newName);
		void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);


		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
		IOResult LoadNamedSelChunk(ILoad *iload,int level);
		IOResult SaveLocalData(ISave *isave, LocalModData *ld);
		IOResult LoadLocalData(ILoad *iload, LocalModData **pld);

		int NumRefs() {return 0;}
		RefTargetHandle GetReference(int i) {return NULL;}
		void SetReference(int i, RefTargetHandle rtarg) {}

		int NumSubs() {return 0;}
		Animatable* SubAnim(int i) {return NULL;}
		TSTR SubAnimName(int i) {return _T("");}

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message) {return REF_SUCCEED;}

		void RemoveRollupPages();
		void SetRollupPages();
		void SelectFrom(int from);
		void UpdateCache(TimeValue t);

		// Local methods for handling named selection sets
		int FindSet(TSTR &setName,int level);
		void AddSet(TSTR &setName,int level);
		void RemoveSet(TSTR &setName,int level);
		void ClearSetNames();
		void NSCopy();
		void NSPaste();
		BOOL GetUniqueSetName(TSTR &name);
		int SelectNamedSet();

	};


class SplineSelData : public LocalModData {
	public:
		// Selection sets
		ShapeVSel vertSel;
		ShapeSSel SegSel;
		ShapePSel SplineSel;

		// Lists of named selection sets
		NVertSelSetList NvselSet;
		NSegSelSetList NsegselSet;
		NPolySelSetList NsplselSet;

		BOOL held;
		BezierShape *shape;

		SplineSelData(BezierShape &shape);
		SplineSelData() {held=0;shape=NULL;}
		~SplineSelData() {FreeCache();}
		LocalModData *Clone();
		BezierShape *GetShape() {return shape;}
		void SetCache(BezierShape &Shape);
		void FreeCache();

		void SelVertBySeg();
		void SelVertBySpline();
		void SelSegByVert();
		void SelSegBySpline();
		void SelSplineByVert();
		void SelSplineBySeg();
	};


class SplSelRestore : public RestoreObj {
	public:
		ShapeVSel uvsel,rvsel;
		ShapeSSel ussel,rssel;
		ShapePSel upsel,rpsel;
		BitArray *sel;
		SplineSelMod *mod;
		SplineSelData *d;
		int level;

		SplSelRestore(SplineSelMod *m, SplineSelData *d);
		void Restore(int isUndo);
		void Redo();
		int Size() { return 1; }
		void EndHold() {d->held=FALSE;}
		TSTR Description() { return TSTR(_T("SelectRestore")); }
	};

class SetNameRestore : public RestoreObj {
	public:
		TSTR undo, redo;
		int index;
		Tab<TSTR*> *sets;
		SplineSelMod *ss;
		SetNameRestore(int i,Tab<TSTR*> *s1,SplineSelMod *s) {
			index = i; sets = s1; ss = s;
			undo = *(*sets)[index];
			}

		void Restore(int isUndo) {			
			redo = *(*sets)[index];
			*(*sets)[index] = undo;
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
		void Redo() {
			*(*sets)[index] = redo;
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
				
		TSTR Description() {return TSTR(_T("Set Name"));}
	};

class AppendVertSetRestore : public RestoreObj {
	public:
		ShapeVSel set;
		NVertSelSetList *setList;

		AppendVertSetRestore(NVertSelSetList *sl) {	setList = sl;}   		
		void Restore(int isUndo) {
			set  = *setList->sets[setList->Count()-1];
			setList->DeleteSet(setList->Count()-1);
			}
		void Redo() {
			setList->AppendSet(set);
			}
				
		TSTR Description() {return TSTR(_T("Append Vert Set"));}
	};
class NAppendSetNameRestore : public RestoreObj {
	public:
		TSTR name;
		SplineSelMod *ss;
		Tab<TSTR*> *sets;

		NAppendSetNameRestore(SplineSelMod *sl,Tab<TSTR*> *s) {
			ss = sl; 
			sets = s;
			}   		
		void Restore(int isUndo) {
			name = *(*sets)[sets->Count()-1];
			delete (*sets)[sets->Count()-1];
			sets->Delete(sets->Count()-1,1);			
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
		void Redo() {
			TSTR *nm = new TSTR(name);
			sets->Append(1,&nm);
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
				
		TSTR Description() {return TSTR(_T("Append Set Name"));}
	};
class AppendSegSetRestore : public RestoreObj {
	public:
		ShapeSSel set;
		NSegSelSetList *setList;

		AppendSegSetRestore(NSegSelSetList *sl) {	setList = sl;}   		
		void Restore(int isUndo) {
			set  = *setList->sets[setList->Count()-1];
			setList->DeleteSet(setList->Count()-1);
			}
		void Redo() {
			setList->AppendSet(set);
			}
				
		TSTR Description() {return TSTR(_T("Append Seg Set"));}
	};
class AppendPolySetRestore : public RestoreObj {
	public:
		ShapePSel set;
		NPolySelSetList *setList;

		AppendPolySetRestore(NPolySelSetList *sl) {	setList = sl;}   		
		void Restore(int isUndo) {
			set  = *setList->sets[setList->Count()-1];
			setList->DeleteSet(setList->Count()-1);
			}
		void Redo() {
			setList->AppendSet(set);
			}
				
		TSTR Description() {return TSTR(_T("Append Poly Set"));}
	};

class DeleteVertSetRestore : public RestoreObj {
	public:
		ShapeVSel set;
		int index;
		NVertSelSetList *setList;

		DeleteVertSetRestore(int i,NVertSelSetList *sl) {
			setList = sl; index = i;
			set  = *setList->sets[index];
			}   		
		void Restore(int isUndo) {
			ShapeVSel *n = new ShapeVSel(set);
			setList->sets.Insert(index,1,&n);
			}
		void Redo() {
			setList->DeleteSet(index);
			}
				
		TSTR Description() {return TSTR(_T("Delete Vert Set"));}
	};

class DeleteSegSetRestore : public RestoreObj {
	public:
		ShapeSSel set;
		int index;
		NSegSelSetList *setList;

		DeleteSegSetRestore(int i,NSegSelSetList *sl) {
			setList = sl; index = i;
			set  = *setList->sets[index];
			}   		
		void Restore(int isUndo) {
			ShapeSSel *n = new ShapeSSel(set);
			setList->sets.Insert(index,1,&n);
			}
		void Redo() {
			setList->DeleteSet(index);
			}
				
		TSTR Description() {return TSTR(_T("Delete Seg Set"));}
	};

class DeletePolySetRestore : public RestoreObj {
	public:
		ShapePSel set;
		int index;
		NPolySelSetList *setList;

		DeletePolySetRestore(int i,NPolySelSetList *sl) {
			setList = sl; index = i;
			set  = *setList->sets[index];
			}   		
		void Restore(int isUndo) {
			ShapePSel *n = new ShapePSel(set);
			setList->sets.Insert(index,1,&n);
			}
		void Redo() {
			setList->DeleteSet(index);
			}
				
		TSTR Description() {return TSTR(_T("Delete Poly Set"));}
	};

class DeleteSetNameRestore : public RestoreObj {
	public:		
		TSTR name;
		int index;		
		SplineSelMod *ss;
		Tab<TSTR*> *sets;

		DeleteSetNameRestore(int i,Tab<TSTR*> *s,SplineSelMod *sm) {
			sets = s; ss = sm; index = i;			
			name = *(*sets)[index];
			}   		
		void Restore(int isUndo) {			
			TSTR *nm = new TSTR(name);			
			sets->Insert(index,1,&nm);
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
		void Redo() {
			sets->Delete(index,1);
			if (ss->ip) ss->ip->NamedSelSetListChanged();
			}
				
		TSTR Description() {return TSTR(_T("Delete Set Name"));}
	};

//--- ClassDescriptor and class vars ---------------------------------

#define SEL_OBJECT	0
#define SEL_VERTEX	1
#define SEL_SEGMENT		2
#define SEL_SPLINE	3


IObjParam             *SplineSelMod::ip              = NULL;
IParamMap             *SplineSelMod::pmapParam       = NULL;
SplineSelMod            *SplineSelMod::editMod         = NULL;
HWND                   SplineSelMod::hParams         = NULL;
BOOL                   SplineSelMod::selByVert       = FALSE;
SelectModBoxCMode     *SplineSelMod::selectMode      = NULL;
BOOL                   SplineSelMod::updateCachePosted = FALSE;

static float lastSegThresh = 1.0f;

class SplSelClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new SplineSelMod; }
	const TCHAR *	ClassName() { return GetString(IDS_AP_SPLINESELMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return SPLSELECT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_AP_DEFDEFORMATIONS);}
	};

static SplSelClassDesc SplSelDesc;
ClassDesc* GetSplineSelModDesc() {return &SplSelDesc;}


static HIMAGELIST hSegImages = NULL;

static BOOL CALLBACK SplineSelObjectProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK SplineSelVertProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK SplineSelSegProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK SplineSelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static DLGPROC windProcs[] = {SplineSelObjectProc,SplineSelVertProc,SplineSelSegProc,SplineSelProc};
static int dlgIDs[] = {IDD_SPLSEL_OBJECT,IDD_SPLSEL_VERTEX,IDD_SPLSEL_SEGMENT,IDD_SPLSEL_SPLINE};
static int dlgTitles[] = {IDS_AP_SELOBJECT,IDS_AP_SELVERTEX,IDS_AP_SELSEG,IDS_AP_SELSPLINE};

// Table to convert selLevel values to shape selLevel flags.
const int shapeLevel[] = {SHAPE_OBJECT,SHAPE_VERTEX,SHAPE_SEGMENT,SHAPE_SPLINE};

// Get display flags based on selLevel.
const DWORD shapeLevelDispFlags[] = {0,DISP_VERTTICKS|DISP_SELVERTS,DISP_VERTTICKS|DISP_SELSEGMENTS,DISP_SELPOLYS};

// For hit testing...
const int shapeHitLevel[] = {0,SUBHIT_SHAPE_VERTS,SUBHIT_SHAPE_SEGMENTS,SUBHIT_SHAPE_POLYS};

//--- SPLSEL mod methods -------------------------------

SplineSelMod::SplineSelMod()
	{	
	selLevel = SEL_VERTEX;
	}

RefTargetHandle SplineSelMod::Clone(RemapDir& remap)
{   SplineSelMod *ssmod= new SplineSelMod();
	ssmod->selLevel = selLevel;
	return ssmod;
}

void SplineSelMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{ if ( os->obj->ClassID() == Class_ID(SPLINESHAPE_CLASS_ID,0))
  { SplineShape *sShape = (SplineShape *)os->obj;
    BezierShape splShape=sShape->GetShape();
    SplineSelData *d  = (SplineSelData*)mc.localData;
    if (!d) {	mc.localData = d = new SplineSelData(splShape);}
	if (editMod==this) 
	{ if (!d->GetShape()) d->SetCache(splShape); }

	d->vertSel.SetSize(splShape,1);
	d->SegSel.SetSize(splShape,1);
	d->SplineSel.SetSize(splShape,1);
	sShape->shape.vertSel = d->vertSel;
	sShape->shape.segSel = d->SegSel;
	sShape->shape.polySel = d->SplineSel;

	if (d->GetShape()) {
	// Keep the cache up to date if it exists.
	d->GetShape()->vertSel = d->vertSel;
	d->GetShape()->segSel = d->SegSel;
	d->GetShape()->polySel = d->SplineSel;
	}

	sShape->shape.dispFlags = 0;
	switch ( selLevel ) {
		case SS_SPLINE:
			sShape->shape.SetDispFlag(DISP_SELPOLYS);
			break;
		case SS_VERTEX:
			sShape->shape.SetDispFlag(DISP_VERTTICKS|DISP_SELVERTS);
			break;
		case SS_SEGMENT:
			sShape->shape.SetDispFlag(DISP_VERTTICKS|DISP_SELSEGMENTS);			
			break;
		}
	sShape->shape.selLevel = shapeLevel[selLevel];
  }
}


void SplineSelMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
	{
	if (mc->localData && ip) {
		((SplineSelData*)mc->localData)->FreeCache();
		TimeValue t = ip->GetTime();
		if (hParams && editMod==this && !updateCachePosted) {
			PostMessage(hParams,WM_UPDATE_CACHE,(WPARAM)t,0);
			updateCachePosted = TRUE;
			}
/*		NotifyDependents(Interval(t,t), 
			PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|
			PART_DISPLAY|PART_TOPO,
		    REFMSG_MOD_EVAL);  */

		}
	}

void SplineSelMod::UpdateCache(TimeValue t)
	{
	NotifyDependents(Interval(t,t), 
		PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|
		PART_DISPLAY|PART_TOPO,
		REFMSG_MOD_EVAL);
	updateCachePosted = FALSE;
	}


void SplineSelMod::RemoveRollupPages()
	{
	if (hParams) ip->DeleteRollupPage(hParams);
	hParams = NULL;	
	}

void SplineSelMod::SetRollupPages()
	{
	RemoveRollupPages();
	hParams = ip->AddRollupPage( 
		hInstance, 
		MAKEINTRESOURCE(dlgIDs[selLevel]),
		windProcs[selLevel],
		GetString(dlgTitles[selLevel]),
		(LPARAM)this);	
	}

void SplineSelMod::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;	
	editMod  = this;

	SetRollupPages();
	
	selectMode = new SelectModBoxCMode(this,ip);

	// Add our sub object type
	TSTR type1(GetString(IDS_AP_VERTEX));
	TSTR type2(GetString(IDS_AP_SEGMENT));
	TSTR type3(GetString(IDS_AP_SPLINE));
	const TCHAR *ptype[] = {type1, type2, type3};
	ip->RegisterSubObjectTypes(ptype, 3);

	// Restore the selection level.
	ip->SetSubObjectLevel(selLevel);

	// Disable show end result.
	ip->EnableShowEndResult(FALSE);
		
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	}

void SplineSelMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	for (int i=0; i<list.Count(); i++) {
		if (list[i]->localData) {
			((SplineSelData*)list[i]->localData)->FreeCache();
			}
		}
	nodes.DisposeTemporary();

	RemoveRollupPages();

	ip->DeleteMode(selectMode);
	if (selectMode) delete selectMode;
	selectMode = NULL;

	// Enable show end result
	ip->EnableShowEndResult(TRUE);

	this->ip = NULL;
	editMod  = NULL;
	hParams  = NULL;
	}

int SplineSelMod::HitTest(
		TimeValue t, INode* inode, int type, int crossing, 
		int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{
	Interval valid;
	int savedLimits,res = 0;
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	Matrix3 mat = inode->GetObjectTM(t);
	gw->setTransform(mat);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	gw->clearHitCode();
	
	if ( mc->localData ) {		
		SplineSelData *shapeData = (SplineSelData*)mc->localData;
		BezierShape *shape = shapeData->GetShape();
		if(!shape) return 0;
		SubShapeHitList hitList;
		ShapeSubHitRec *rec;
		res = shape->SubObjectHitTest( gw, gw->getMaterial(), &hr,
			flags|((splineHitOverride) ? shapeHitLevel[splineHitOverride] : shapeHitLevel[selLevel]), hitList );
	
		rec = hitList.First();
		while( rec ) {
			vpt->LogHit(inode,mc,rec->dist,123456,new ShapeHitData(rec->shape, rec->poly, rec->index));
			rec = rec->Next();
			}
		}

	gw->setRndLimits(savedLimits);	
	return res;
}

void SplineSelMod::ActivateSubobjSel(int level, XFormModes& modes)
	{
	//SetSubobjectLevel(level);
	selLevel = level;

	// Fill in modes with our sub-object modes
	if (level!=SEL_OBJECT) {
		modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
		}

	// Setup rollup pages
	SetRollupPages();
	
	// Setup named selection sets	
	SetupNamedSelDropDown();

	ip->PipeSelLevelChanged();
	NotifyDependents(FOREVER, SELECT_CHANNEL|DISP_ATTRIB_CHANNEL|SUBSEL_TYPE_CHANNEL, REFMSG_CHANGE);
	}

void SplineSelMod::SelectSubComponent(
		HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
	{
	SplineSelData *d = NULL, *od = NULL;	

	ip->ClearCurNamedSelSet();

	switch (selLevel) {
		case SEL_VERTEX:
			while (hitRec) {
				d = (SplineSelData*)hitRec->modContext->localData;
				if (d) {
					if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
					BOOL state = selected;
					int poly=((ShapeHitData *)(hitRec->hitData))->poly;
					int vert=((ShapeHitData *)(hitRec->hitData))->index;
					if (invert) state = !d->vertSel[poly][vert];
					if (state) d->vertSel[poly].Set(vert);
					else       d->vertSel[poly].Clear(vert);
					}
				if (!all) break;
				hitRec = hitRec->Next();
				}			
			break;

		case SEL_SEGMENT: 
			while (hitRec) {					
				d = (SplineSelData*)hitRec->modContext->localData;
				if (d) {
					if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
					BOOL state = selected;
					int poly=((ShapeHitData *)(hitRec->hitData))->poly;
					int vert=((ShapeHitData *)(hitRec->hitData))->index;
					if (invert) state = !d->SegSel[poly][vert];
					if (state) d->SegSel[poly].Set(vert);
					else       d->SegSel[poly].Clear(vert);
					}
				if (!all) break;
				hitRec = hitRec->Next();
				}
			break;

		case SEL_SPLINE:
			while (hitRec) {				
				d = (SplineSelData*)hitRec->modContext->localData;
				if (d) {
					if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
					BOOL state = selected;
					int poly=((ShapeHitData *)(hitRec->hitData))->poly;
					if (invert) state = !d->SplineSel[poly];
					if (state) d->SplineSel.Set(poly);
					else       d->SplineSel.Clear(poly);					
					}
				if (!all) break;
				hitRec = hitRec->Next();
				}
			break;
		};

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void SplineSelMod::ClearSelection(int selLevel)
	{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	SplineSelData *d;
	for (int i=0; i<list.Count(); i++) {
		d = (SplineSelData*)list[i]->localData;
		if (!d) continue;
		if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
		switch (selLevel) {
			case SEL_VERTEX: d->vertSel.ClearAll(); break;
			case SEL_SEGMENT:   d->SegSel.ClearAll(); break;
			case SEL_SPLINE:   d->SplineSel.ClearAll(); break;
			}
		}
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void SplineSelMod::SelectAll(int selLevel)
	{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	SplineSelData *d;
	for (int i=0; i<list.Count(); i++) {
		d = (SplineSelData*)list[i]->localData;
		if (!d) continue;
		if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
		switch (selLevel) {
			case SEL_VERTEX: d->vertSel.SetAll(); break;
			case SEL_SEGMENT:   d->SegSel.SetAll(); break;
			case SEL_SPLINE:   d->SplineSel.SetAll(); break;
			}
		}
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void SplineSelMod::InvertSelection(int selLevel)
	{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	SplineSelData *d;
	for (int i=0; i<list.Count(); i++) {
		d = (SplineSelData*)list[i]->localData;
		if (!d) continue;
		if (theHold.Holding() && !d->held) theHold.Put(new SplSelRestore(this,d));
		switch (selLevel) {
			case SEL_VERTEX:  
				{for (int j=0;j<d->vertSel.polys;j++) d->vertSel[j]=~d->vertSel[j];
				 break;
				}
			case SEL_SEGMENT:  
				{ for (int j=0;j<d->SegSel.polys;j++)
					d->SegSel[j] = ~d->SegSel[j];
				  break;
				}
			case SEL_SPLINE:  d->SplineSel.sel = d->SplineSel.sel; break;
			}
		}
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void SplineSelMod::SelectFrom(int from) 
	{
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	SplineSelData *d;
	theHold.Begin();
	for (int i=0; i<list.Count(); i++) {
		d = (SplineSelData*)list[i]->localData;
		if (!d) continue;
		if (!d->held) theHold.Put(new SplSelRestore(this,d));
		switch (selLevel) {
			case SEL_VERTEX: 
				if (from==SEL_SEGMENT) d->SelVertBySeg();
				else d->SelVertBySpline();
				break;
			case SEL_SEGMENT:   
				if (from==SEL_VERTEX) d->SelSegByVert();
				else d->SelSegBySpline();
				break;
			case SEL_SPLINE:
				if (from==SEL_VERTEX) d->SelSplineByVert();
				else d->SelSplineBySeg();
				break;
			}
		}
	theHold.Accept(GetString(IDS_AP_SELECT));
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	ip->ClearCurNamedSelSet();
	}

#define SELLEVEL_CHUNKID		0x0100
#define VERTSEL_CHUNKID			0x0200
#define SEGSEL_CHUNKID			0x0210
#define SPLINESEL_CHUNKID		0x0220

#define NAMEDVSEL_NAMES_CHUNK	0x2805
#define NAMEDFSEL_NAMES_CHUNK	0x2806
#define NAMEDESEL_NAMES_CHUNK	0x2807
#define NAMEDSEL_STRING_CHUNK	0x2809

#define NvselSet_CHUNK			0x2845
#define FSELSET_CHUNK			0x2846
#define ESELSET_CHUNK			0x2847

static int namedSelID[] = {NAMEDVSEL_NAMES_CHUNK,NAMEDFSEL_NAMES_CHUNK,NAMEDESEL_NAMES_CHUNK};


IOResult SplineSelMod::Save(ISave *isave)
	{
	IOResult res;
	ULONG nb;
	Modifier::Save(isave);
	isave->BeginChunk(SELLEVEL_CHUNKID);
	res = isave->Write(&selLevel, sizeof(selLevel), &nb);
	isave->EndChunk();

	for (int j=0; j<3; j++) {
		if (namedSel[j].Count()) {
			isave->BeginChunk(namedSelID[j]);			
			for (int i=0; i<namedSel[j].Count(); i++) {
				isave->BeginChunk(NAMEDSEL_STRING_CHUNK);
				isave->WriteWString(*namedSel[j][i]);
				isave->EndChunk();
				}
			isave->EndChunk();
			}
		}

	return res;
	}

IOResult SplineSelMod::LoadNamedSelChunk(ILoad *iload,int level)
	{	
	IOResult res;
	
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case NAMEDSEL_STRING_CHUNK: {
				TCHAR *name;
				res = iload->ReadWStringChunk(&name);
				AddSet(TSTR(name),level+1);
				break;
				}
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult SplineSelMod::Load(ILoad *iload)
	{
	IOResult res;
	ULONG nb;
	Modifier::Load(iload);
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SELLEVEL_CHUNKID:
				iload->Read(&selLevel, sizeof(selLevel), &nb);
				break;

			case NAMEDVSEL_NAMES_CHUNK: {				
				res = LoadNamedSelChunk(iload,0);
				break;
				}

			case NAMEDFSEL_NAMES_CHUNK: {
				res = LoadNamedSelChunk(iload,1);
				break;
				}

			case NAMEDESEL_NAMES_CHUNK: {
				res = LoadNamedSelChunk(iload,2);
				break;
				}
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult SplineSelMod::SaveLocalData(ISave *isave, LocalModData *ld)
	{	
	SplineSelData *d = (SplineSelData*)ld;

	isave->BeginChunk(VERTSEL_CHUNKID);
	d->vertSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(SEGSEL_CHUNKID);
	d->SegSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(SPLINESEL_CHUNKID);
	d->SplineSel.Save(isave);
	isave->EndChunk();
	
	if (d->NvselSet.Count()) {
		isave->BeginChunk(NvselSet_CHUNK);
		d->NvselSet.Save(isave);
		isave->EndChunk();
		}
	if (d->NsplselSet.Count()) {
		isave->BeginChunk(ESELSET_CHUNK);
		d->NsplselSet.Save(isave);
		isave->EndChunk();
		}
	if (d->NsegselSet.Count()) {
		isave->BeginChunk(FSELSET_CHUNK);
		d->NsegselSet.Save(isave);
		isave->EndChunk();
		}

	return IO_OK;
	}

IOResult SplineSelMod::LoadLocalData(ILoad *iload, LocalModData **pld)
	{
	SplineSelData *d = new SplineSelData;
	*pld = d;
	IOResult res;	
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VERTSEL_CHUNKID:
				d->vertSel.Load(iload);
				break;
			case SEGSEL_CHUNKID:
				d->SegSel.Load(iload);
				break;
			case SPLINESEL_CHUNKID:
				d->SplineSel.Load(iload);
				break;

			case NvselSet_CHUNK:
				res = d->NvselSet.Load(iload);
				break;
			case FSELSET_CHUNK:
				res = d->NsegselSet.Load(iload);
				break;
			case ESELSET_CHUNK:
				res = d->NsplselSet.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}



// Window Procs ------------------------------------------------------

static BOOL CALLBACK SplineSelObjectProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SplineSelMod *mod = (SplineSelMod*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG:
			mod = (SplineSelMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;
			break;
		
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:   			
   			mod->ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			break;
		case WM_UPDATE_CACHE:
			mod->UpdateCache((TimeValue)wParam);
 			break;
		default: return FALSE;
		}
	return TRUE;
	}

static BOOL CALLBACK SplineSelVertProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SplineSelMod *mod = (SplineSelMod*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG: {
			mod = (SplineSelMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;

			ICustButton *but = GetICustButton(GetDlgItem(hWnd,IDC_VERT_PASTENS));
			if (GetShapeNamedVertSelClip()) but->Enable();
			else but->Disable();
			ReleaseICustButton(but);
			break;
			}
		
		case WM_UPDATE_CACHE:
			mod->UpdateCache((TimeValue)wParam);
 			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_SPLSEL_GETSEGMENT: mod->SelectFrom(SEL_SEGMENT); break;
				case IDC_SPLSEL_GETSPLINE: mod->SelectFrom(SEL_SPLINE); break;

				case IDC_VERT_COPYNS:  mod->NSCopy();  break;
				case IDC_VERT_PASTENS: mod->NSPaste(); break;
				}
			break;

		default: return FALSE;
		}
	return TRUE;
	}
static BOOL CALLBACK SplineSelSegProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SplineSelMod *mod = (SplineSelMod*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG: {
			mod = (SplineSelMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;

			ICustButton *but = GetICustButton(GetDlgItem(hWnd,IDC_SEG_PASTENS));
			if (GetShapeNamedSegSelClip()) but->Enable();
			else but->Disable();
			ReleaseICustButton(but);
			break;
			}
		
		case WM_UPDATE_CACHE:
			mod->UpdateCache((TimeValue)wParam);
 			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_SPLSEL_GETVERTEX: mod->SelectFrom(SEL_VERTEX); break;
				case IDC_SPLSEL_GETSPLINE: mod->SelectFrom(SEL_SPLINE); break;

				case IDC_SEG_COPYNS:  mod->NSCopy();  break;
				case IDC_SEG_PASTENS: mod->NSPaste(); break;
				}
			break;

		default: return FALSE;
		}
	return TRUE;
	}

static BOOL CALLBACK SplineSelProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SplineSelMod *mod = (SplineSelMod*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG: {
			mod = (SplineSelMod*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;
			
			ICustButton *but = GetICustButton(GetDlgItem(hWnd,IDC_SPLINE_PASTENS));
			if (GetShapeNamedPolySelClip()) but->Enable();
			else but->Disable();
			ReleaseICustButton(but);
			break;
			}
		
		case WM_UPDATE_CACHE:
			mod->UpdateCache((TimeValue)wParam);
 			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_SPLSEL_GETVERTEX: mod->SelectFrom(SEL_VERTEX); break;
				case IDC_SPLSEL_GETSEGMENT: mod->SelectFrom(SEL_SEGMENT); break;

				case IDC_SPLINE_COPYNS:  mod->NSCopy();  break;
				case IDC_SPLINE_PASTENS: mod->NSPaste(); break;
				}
			break;

		default: return FALSE;
		}
	return TRUE;
	}


static BOOL CALLBACK PickSetNameDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static TSTR *name;

	switch (msg) {
		case WM_INITDIALOG: {
			name = (TSTR*)lParam;
			ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
			edit->SetText(*name);
			ReleaseICustEdit(edit);
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
					TCHAR buf[256];
					edit->GetText(buf,256);
					*name = TSTR(buf);
					ReleaseICustEdit(edit);
					EndDialog(hWnd,1);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,0);
					break;
				}
			break;

		default:
			return FALSE;
		};
	return TRUE;
	}

BOOL SplineSelMod::GetUniqueSetName(TSTR &name)
	{
	while (1) {				
		Tab<TSTR*> &setList = namedSel[selLevel-1];

		BOOL unique = TRUE;
		for (int i=0; i<setList.Count(); i++) {
			if (name==*setList[i]) {
				unique = FALSE;
				break;
				}
			}
		if (unique) break;

		if (!DialogBoxParam(
			hInstance, 
			MAKEINTRESOURCE(IDD_PASTE_NAMEDSET),
			hParams, 
			PickSetNameDlgProc,
			(LPARAM)&name)) return FALSE;		
		}
	return TRUE;
	}

static BOOL CALLBACK PickSetDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{	
	switch (msg) {
		case WM_INITDIALOG:	{
			Tab<TSTR*> *setList = (Tab<TSTR*>*)lParam;
			for (int i=0; i<setList->Count(); i++) {
				int pos  = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_ADDSTRING,0,
					(LPARAM)(TCHAR*)*(*setList)[i]);
				SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_SETITEMDATA,pos,i);
				}
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					int sel = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETCURSEL,0,0);
					if (sel!=LB_ERR) {
						int res =SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETITEMDATA,sel,0);
						EndDialog(hWnd,res);
						break;
						}
					// fall through
					}

				case IDCANCEL:
					EndDialog(hWnd,-1);
					break;
				}
			break;

		default:
			return FALSE;
		};
	return TRUE;
	}

int SplineSelMod::SelectNamedSet()
	{
	Tab<TSTR*> &setList = namedSel[selLevel-1];
	return DialogBoxParam(
		hInstance, 
		MAKEINTRESOURCE(IDD_SEL_NAMEDSET),
		hParams, 
		PickSetDlgProc,
		(LPARAM)&setList);
	}




// SplineSelData -----------------------------------------------------

LocalModData *SplineSelData::Clone()
	{
	SplineSelData *d = new SplineSelData;
	d->vertSel = vertSel;
	d->SegSel = SegSel;
	d->SplineSel = SplineSel;
	held = FALSE;
	shape = NULL;
	return d;
	}

SplineSelData::SplineSelData(BezierShape &shape)
{ 	vertSel = shape.vertSel;
	SegSel = shape.segSel;
	SplineSel = shape.polySel;
//	vertSel.ClearAll();	SegSel.ClearAll();SplineSel.ClearAll();
	held = FALSE;
	this->shape = NULL;
}

void SplineSelData::FreeCache()
{ if (shape) delete shape;
  shape = NULL;
}

void SplineSelData::SetCache(BezierShape &Shape)
{ 	if (this->shape) delete this->shape;
	this->shape = new BezierShape(Shape);
}

void SplineSelData::SelVertBySeg()
{ int maxv=0,nxtj=0,vcnt=1;
  for (int i=0; i<SegSel.polys; i++) 
  {	maxv=vertSel[i].GetSize()-1; vcnt=1;
    for (int j=0;j<SegSel[i].GetSize();j++)
    { if (SegSel[i][j])
	  { vertSel[i].Set(vcnt);nxtj=vcnt+3;
		vertSel[i].Set(nxtj>maxv?1:nxtj);
	  }
	  vcnt+=3;
	}
  }
}

void SplineSelData::SelVertBySpline()
{ for (int i=0; i<SplineSel.polys; i++)
   if (SplineSel[i])		
	 for (int j=0; j<vertSel[i].GetSize();j++)
      	vertSel[i].Set(j);
}

void SplineSelData::SelSegByVert()
{ int maxv=0,nxtj=0,vcnt=1;
  for (int i=0; i<SegSel.polys; i++) 
  {	maxv=vertSel[i].GetSize()-1; vcnt=1;
  	for (int j=0; j<SegSel[i].GetSize(); j++)
	{ if ((vertSel[i][vcnt])||(vertSel[i][(nxtj=vcnt+3)>maxv?1:nxtj]))
	    SegSel[i].Set(j);
	  vcnt+=3;
	}
  }
}

void SplineSelData::SelSegBySpline()
{ for (int i=0; i<SplineSel.polys; i++)
   if (SplineSel[i])		
	 for (int j=0; j<SegSel[i].GetSize();j++)
      	SegSel[i].Set(j);
}

void SplineSelData::SelSplineByVert()
{ for (int i=0; i<SplineSel.polys; i++) 
  	for (int j=0; j<vertSel[i].GetSize(); j++) 
      if (vertSel[i][j]) 
	  {	SplineSel.Set(i);
		break;
	  }
}

void SplineSelData::SelSplineBySeg()
{ for (int i=0; i<SplineSel.polys; i++) 
  	for (int j=0; j<SegSel[i].GetSize(); j++) 
      if (SegSel[i][j]) 
	  {	SplineSel.Set(i);
		break;
	  }
}


// SplSelRestore --------------------------------------------------

SplSelRestore::SplSelRestore(SplineSelMod *m, SplineSelData *data)
	{
	mod     = m;
	level   = mod->selLevel;
	d       = data;
	d->held = TRUE;
	switch (level) {
		case SEL_OBJECT: assert(0); break;
		case SEL_VERTEX: uvsel = d->vertSel; break;
		case SEL_SEGMENT:  ussel = d->SegSel; break;
		case SEL_SPLINE:   upsel = d->SplineSel; break;
		}
	}

void SplSelRestore::Restore(int isUndo)
	{
	if (isUndo) {
		switch (level) {			
			case SEL_VERTEX: rvsel = d->vertSel; break;
			case SEL_SEGMENT:   rssel = d->SegSel; break;
			case SEL_SPLINE:   rpsel = d->SplineSel; break;
			}
		}
	switch (level) {		
		case SEL_VERTEX: d->vertSel = uvsel; break;
		case SEL_SEGMENT:   d->SegSel = ussel; break;
		case SEL_SPLINE:   d->SplineSel = upsel; break;
		}
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void SplSelRestore::Redo()
	{
	switch (level) {		
		case SEL_VERTEX: d->vertSel = rvsel; break;
		case SEL_SEGMENT:   d->SegSel = rssel; break;
		case SEL_SPLINE:   d->SplineSel = rpsel; break;
		}
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}


//--- Named selection sets -----------------------------------------

int SplineSelMod::FindSet(TSTR &setName,int level)
	{
	for (int i=0; i<namedSel[level-1].Count(); i++) {
		if (setName == *namedSel[level-1][i]) {
			return i;			
			}
		}
	return -1;
	}

void SplineSelMod::AddSet(TSTR &setName,int level)
	{
	TSTR *name = new TSTR(setName);
	namedSel[level-1].Append(1,&name);
	}

void SplineSelMod::RemoveSet(TSTR &setName,int level)
	{
	int i = FindSet(setName,level);
	if (i>=0) {
		delete namedSel[level-1][i];
		namedSel[level-1].Delete(i,1);
		}
	}

void SplineSelMod::ClearSetNames()
	{
	for (int i=0; i<3; i++) {
		for (int j=0; j<namedSel[i].Count(); j++) {
			delete namedSel[i][j];
			namedSel[i][j] = NULL;
			}
		}
	}

void SplineSelMod::ActivateSubSelSet(TSTR &setName)
	{ int i;
	ModContextList mcList;
	INodeTab nodes;
	if (!ip) return;
	i = FindSet(setName,selLevel);
	if (i<0) return;
	ip->GetModContexts(mcList,nodes);

	for (int index = 0; index < mcList.Count(); index++) {
		SplineSelData *ShapeData = (SplineSelData*)mcList[index]->localData;
		if (!ShapeData) continue;
		if (theHold.Holding() && !ShapeData->held) theHold.Put(new SplSelRestore(this,ShapeData));

		switch (selLevel) {
			case SEL_VERTEX:
			if (!(ShapeData->NvselSet[i]).IsCompatible(*ShapeData->shape))
				(ShapeData->NvselSet[i]).SetSize(*ShapeData->shape, TRUE);
			ShapeData->vertSel = ShapeData->NvselSet[i];
			break;

			case SEL_SEGMENT:
			if (!ShapeData->NsegselSet[i].IsCompatible(*ShapeData->shape))
				ShapeData->NsegselSet[i].SetSize(*ShapeData->shape, TRUE);
			ShapeData->SegSel = ShapeData->NsegselSet[i];
			break;

			case SEL_SPLINE:
			if (!ShapeData->NsplselSet[i].IsCompatible(*ShapeData->shape))
				ShapeData->NsplselSet[i].SetSize(*ShapeData->shape, TRUE);
			ShapeData->SplineSel = ShapeData->NsplselSet[i];
			break;
			}		
		}
	
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);	
	ip->RedrawViews(ip->GetTime());
	}

void SplineSelMod::NewSetFromCurSel(TSTR &setName)
	{
	ModContextList mcList;
	INodeTab nodes;	
	int index = FindSet(setName,selLevel);	
	
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		SplineSelData *ShapeData = (SplineSelData*)mcList[i]->localData;
		if (!ShapeData) continue;
		
		switch (selLevel) {
			case SEL_VERTEX:	
				if (index>=0) ShapeData->NvselSet[index] = ShapeData->vertSel;
				else ShapeData->NvselSet.AppendSet(ShapeData->vertSel);				
				break;

			case SEL_SEGMENT:
				if (index>=0) ShapeData->NsegselSet[index] = ShapeData->SegSel;
				else ShapeData->NsegselSet.AppendSet(ShapeData->SegSel);					
				break;

			case SEL_SPLINE:
				if (index>=0) ShapeData->NsplselSet[index] = ShapeData->SplineSel;
				else ShapeData->NsplselSet.AppendSet(ShapeData->SplineSel);				
				break;
			}		
		}	
	
	if (index<0) AddSet(setName,selLevel);		
	nodes.DisposeTemporary();
	}

void SplineSelMod::RemoveSubSelSet(TSTR &setName)
	{
	ModContextList mcList;
	INodeTab nodes;
	int index = FindSet(setName,selLevel);

	if (index<0 || !ip) return;		
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		SplineSelData *ShapeData = (SplineSelData*)mcList[i]->localData;
		if (!ShapeData) continue;		

		switch (selLevel) {
			case SEL_VERTEX:	
				if (theHold.Holding()) {
					theHold.Put(new DeleteVertSetRestore(index,&ShapeData->NvselSet));
					}
				ShapeData->NvselSet.DeleteSet(index);
				break;

			case SEL_SEGMENT:
				if (theHold.Holding()) {
					theHold.Put(new DeleteSegSetRestore(index,&ShapeData->NsegselSet));
					}
				ShapeData->NsegselSet.DeleteSet(index);
				break;

			case SEL_SPLINE:
				if (theHold.Holding()) {
					theHold.Put(new DeletePolySetRestore(index,&ShapeData->NsplselSet));
					}
				ShapeData->NsplselSet.DeleteSet(index);
				break;
			}		
		}
	
	if (theHold.Holding()) {
		theHold.Put(new DeleteSetNameRestore(index,&namedSel[selLevel-1],this));
		}			
	RemoveSet(setName,selLevel);
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
	}

void SplineSelMod::SetupNamedSelDropDown()
	{	
	if (selLevel != SEL_OBJECT) {
		ip->ClearSubObjectNamedSelSets();
		for (int i=0; i<namedSel[selLevel-1].Count(); i++) {
			ip->AppendSubObjectNamedSelSet(*namedSel[selLevel-1][i]);
			}
		}	
	}

int SplineSelMod::NumNamedSelSets()
	{
	return namedSel[selLevel-1].Count();
	}

TSTR SplineSelMod::GetNamedSelSetName(int i)
	{
	return *namedSel[selLevel-1][i];
	}

void SplineSelMod::SetNamedSelSetName(int i,TSTR &newName)
{	if (theHold.Holding())
		theHold.Put(new SetNameRestore(i,&namedSel[selLevel-1],this));
	*namedSel[selLevel-1][i] = newName;
}

void SplineSelMod::NewSetByOperator(TSTR &newName,Tab<int> &sets,int op)
{	ModContextList mcList;
	INodeTab nodes;	
	int poly;
	
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		SplineSelData *ShapeData = (SplineSelData*)mcList[i]->localData;
		if (!ShapeData) continue;

	switch(selLevel) {
		case SEL_VERTEX: {
			NVertSelSetList	*vselList= &ShapeData->NvselSet;
			ShapeVSel bits = (*vselList)[sets[0]];
			for(poly = 0; poly < bits.polys; ++poly) {
				for (i=1; i<sets.Count(); i++) {
					ShapeVSel bit2 = (*vselList)[sets[i]];
					switch (op) {
						case NEWSET_MERGE:
							bits[poly] |= bit2[poly];
							break;

						case NEWSET_INTERSECTION:
							bits[poly] &= bit2[poly];
							break;

						case NEWSET_SUBTRACT:
							bits[poly] &= ~(bit2[poly]);
							break;
						}
					}
				}
			ShapeData->NvselSet.AppendSet(bits);
			if (theHold.Holding())
				theHold.Put(new AppendVertSetRestore(&ShapeData->NvselSet));
			}
			break;
		case SEL_SEGMENT: {
			NSegSelSetList sselSet= ShapeData->NsegselSet;
			ShapeSSel bits = *sselSet.sets[sets[0]];
			for(poly = 0; poly < bits.polys; ++poly) {
				for (i=1; i<sets.Count(); i++) {
					ShapeSSel &bit2 = *sselSet.sets[sets[i]];
					switch (op) {
						case NEWSET_MERGE:
							bits[poly] |= bit2[poly];
							break;

						case NEWSET_INTERSECTION:
							bits[poly] &= bit2[poly];
							break;

						case NEWSET_SUBTRACT:
							bits[poly] &= ~(bit2[poly]);
							break;
						}
					}
				}
			ShapeData->NsegselSet.AppendSet(bits);
			if (theHold.Holding())
				theHold.Put(new AppendSegSetRestore(&ShapeData->NsegselSet));
			}
			break;
		case SEL_SPLINE: {
			NPolySelSetList pselSet= ShapeData->NsplselSet;
			ShapePSel bits = *pselSet.sets[sets[0]];
			for (i=1; i<sets.Count(); i++) {
				ShapePSel &bit2 = *pselSet.sets[sets[i]];
				switch (op) {
					case NEWSET_MERGE:
						bits.sel |= bit2.sel;
						break;

					case NEWSET_INTERSECTION:
						bits.sel &= bit2.sel;
						break;

					case NEWSET_SUBTRACT:
						bits.sel &= ~(bit2.sel);
						break;
					}
				}
			ShapeData->NsplselSet.AppendSet(bits);
			if (theHold.Holding())
				theHold.Put(new AppendPolySetRestore(&ShapeData->NsplselSet));
			}
			break;
		}
	}
	AddSet(newName,selLevel);
	if (theHold.Holding()) {
		theHold.Put(new NAppendSetNameRestore(this,&namedSel[selLevel-1]));
		}
}

void SplineSelMod::NSCopy()
	{
	int index = SelectNamedSet();
	if (index>=0) {		
		ModContextList mcList;
		INodeTab nodes;			
		
		// Enable the paste button
		ICustButton *but;
		ip->GetModContexts(mcList,nodes);
		for (int i = 0; i < mcList.Count(); i++) {
			SplineSelData *ShapeData = (SplineSelData*)mcList[i]->localData;
			if (!ShapeData) continue;

			switch (selLevel) {
				case SEL_VERTEX:
				{ ShapeNamedVertSelClip *clip = new ShapeNamedVertSelClip(*namedSel[selLevel-1][index]);
				  ShapeVSel *sel = new ShapeVSel(ShapeData->NvselSet[index]);
				  clip->sets.Append(1,&sel);
				  SetShapeNamedVertSelClip(clip);
				  but = GetICustButton(GetDlgItem(hParams,IDC_VERT_PASTENS));
				  break;				
				}
				case SEL_SEGMENT:
				{ ShapeNamedSegSelClip *clip = new ShapeNamedSegSelClip(*namedSel[selLevel-1][index]);
				  ShapeSSel *sel = new ShapeSSel(ShapeData->NsegselSet[index]);
				  clip->sets.Append(1,&sel);
				  SetShapeNamedSegSelClip(clip);
				  but = GetICustButton(GetDlgItem(hParams,IDC_SEG_PASTENS));
 				  break;
				}
				case SEL_SPLINE: 
				{ ShapeNamedPolySelClip *clip = new ShapeNamedPolySelClip(*namedSel[selLevel-1][index]);
				  ShapePSel *sel = new ShapePSel(ShapeData->NsplselSet[index]);
				  clip->sets.Append(1,&sel);
				  SetShapeNamedPolySelClip(clip);
				  but = GetICustButton(GetDlgItem(hParams,IDC_SPLINE_PASTENS));
 				  break;
				}		
			}
		but->Enable();
		ReleaseICustButton(but);
		}	
	}
}

void SplineSelMod::NSPaste()
	{
	TSTR name;

	ModContextList mcList;
	INodeTab nodes;		
	
	theHold.Begin();
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		SplineSelData *ShapeData = (SplineSelData*)mcList[i]->localData;
		if (!ShapeData) continue;

		switch (selLevel) {
			case SEL_VERTEX:
			{ ShapeNamedVertSelClip *clip = GetShapeNamedVertSelClip();
			  if (!clip) return;	
			  name = clip->name;
			  if (!GetUniqueSetName(name)) return;
			  ShapeData->NvselSet.AppendSet(*clip->sets[0]);	
			  theHold.Put(new AppendVertSetRestore(&ShapeData->NvselSet));
			}
			break;				
			case SEL_SEGMENT:
			{ ShapeNamedSegSelClip *clip = GetShapeNamedSegSelClip();
			  if (!clip) return;
			  name = clip->name;
			  if (!GetUniqueSetName(name)) return;
			  ShapeData->NsegselSet.AppendSet(*clip->sets[0]);	
			  theHold.Put(new AppendSegSetRestore(&ShapeData->NsegselSet));
			}
			break;			
			case SEL_SPLINE:
			{ ShapeNamedPolySelClip *clip = GetShapeNamedPolySelClip();
			  if (!clip) return;
			  name = clip->name;
			  if (!GetUniqueSetName(name)) return;
			  ShapeData->NsplselSet.AppendSet(*clip->sets[0]);	
			  theHold.Put(new AppendPolySetRestore(&ShapeData->NsplselSet));
			}
			break;			
			}
	}
	AddSet(name,selLevel);	
	theHold.Put(new NAppendSetNameRestore(this,&namedSel[selLevel-1]));	
	ActivateSubSelSet(name);
	ip->SetCurNamedSelSet(name);
	theHold.Accept(_T("Paste Named Selection"));
	SetupNamedSelDropDown();	
	}


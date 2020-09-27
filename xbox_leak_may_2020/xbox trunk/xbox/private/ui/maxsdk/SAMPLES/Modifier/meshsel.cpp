/**********************************************************************
 *<
	FILE: meshsel.cpp

	DESCRIPTION:  A selection modifier for meshes

	CREATED BY: Rolf Berteig

	HISTORY: 10/23/96

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm.h"
#include "meshadj.h"
#include "namesel.h"
#include "nsclip.h"
#include "istdplug.h"
#include "iparamm.h"

#define DEF_FALLOFF 20.0f

// Named selection set levels:
#define NS_VERTEX 0
#define NS_EDGE 1
#define NS_FACE 2
static int namedSetLevel[] = { NS_VERTEX, NS_VERTEX, NS_EDGE, NS_FACE, NS_FACE, NS_FACE };
static int namedClipLevel[] = { CLIP_VERT, CLIP_VERT, CLIP_EDGE, CLIP_FACE, CLIP_FACE, CLIP_FACE };

#define WM_UPDATE_CACHE		(WM_USER+0x287)

#define REF_PBLOCK 0

// flags:
#define MS_DISP_END_RESULT 0x01

class MeshSelMod : public Modifier, public IMeshSelect {	
public:
	DWORD selLevel;
	Tab<TSTR*> namedSel[3];		
	Tab<DWORD> ids[3];
	IParamBlock *pblock;
	DWORD flags;
	void SetFlag (DWORD fl) { flags |= fl; }
	void ClearFlag (DWORD fl) { flags &= ~fl; }
	void SetFlag (DWORD fl, bool set) { if (set) SetFlag (fl); else ClearFlag (fl); }
	bool GetFlag (DWORD fl) { return (fl&flags) ? TRUE : FALSE; }

	static IObjParam *ip;
	static IParamMap *pmapParam;
	static MeshSelMod *editMod;
	static BOOL selByVert;
	static BOOL ignoreBackfaces;
	static BOOL ignoreVisEdge;
	static HWND hParams;
	static BOOL arRollup;
	static SelectModBoxCMode *selectMode;
	static BOOL updateCachePosted;

	MeshSelMod();

	// From Animatable
	void DeleteThis() { delete this; }
	void GetClassName(TSTR& s) {s = GetString(IDS_RB_MESHSELMOD);}  
	virtual Class_ID ClassID() { return Class_ID(MESHSELECT_CLASS_ID,0);}		
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	TCHAR *GetObjectName() {return GetString(IDS_RB_MESHSELMOD);}

	// From modifier
	ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO;}
	ChannelMask ChannelsChanged() {return PART_SELECT|PART_SUBSEL_TYPE|PART_TOPO;} // RB 5/27/97: Had to include topo channel because in edge select mode this modifier turns on hidden edges -- which may cause the edge list to be rebuilt, which is effectively a change in topology since the edge list is now part of the topo channel.
	Class_ID InputType() {return triObjectClassID;}
	void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
	Interval LocalValidity(TimeValue t) { return GetValidity(t); }
	Interval GetValidity (TimeValue t);
	BOOL DependOnTopology(ModContext &mc) {return TRUE;}

	// From BaseObject
	CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
	void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
	void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);
	void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
	void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);

	void ActivateSubobjSel(int level, XFormModes& modes);
	void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
	void ClearSelection(int selLevel);
	void SelectAll(int selLevel);
	void InvertSelection(int selLevel);
	void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

	void ShowEndResultChanged (BOOL showEndResult) { NotifyDependents (FOREVER, PART_DISPLAY, REFMSG_CHANGE); }

	BOOL SupportsNamedSubSels() {return TRUE;}
	void ActivateSubSelSet(TSTR &setName);
	void NewSetFromCurSel(TSTR &setName);
	void RemoveSubSelSet(TSTR &setName);
	void SetupNamedSelDropDown();
	int NumNamedSelSets();
	TSTR GetNamedSelSetName(int i);
	void SetNamedSelSetName(int i,TSTR &newName);
	void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);
	void UpdateNamedSelDropDown ();

	// From IMeshSelect
	DWORD GetSelLevel();
	void SetSelLevel(DWORD level);
	void LocalDataChanged();
	BOOL HasWeightedVertSel ();

	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
	IOResult LoadNamedSelChunk(ILoad *iload,int level);
	IOResult SaveLocalData(ISave *isave, LocalModData *ld);
	IOResult LoadLocalData(ILoad *iload, LocalModData **pld);

	IParamArray *GetParamBlock() {return pblock;}
	int GetParamBlockIndex(int id) {return id;}

	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i) { return pblock; }
	void SetReference(int i, RefTargetHandle rtarg) { pblock = (IParamBlock *) rtarg; }

	int NumSubs() {return 1;}
	Animatable* SubAnim(int i) { return GetReference(i); }
	TSTR SubAnimName(int i) {return GetString (IDS_RB_PARAMETERS);}

	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
	   PartID& partID, RefMessage message);

	void UpdateSelLevelDisplay ();
	float GetPolyFaceThresh();
	HitRecord *VertHitsToFaceHits(HitRecord *hitRec,BOOL all);
	void SelectFrom(int from);
	void SetEnableStates();
	void SelectOpenEdges();
	void SelectByMatID(int id);
	void SetNumSelLabel();		
	void UpdateCache(TimeValue t);
	void InvalidateVDistances ();

	// Local methods for handling named selection sets
	int FindSet(TSTR &setName,int level);		
	DWORD AddSet(TSTR &setName,int level);
	void RemoveSet(TSTR &setName,int level);
	void UpdateSetNames ();	// Reconciles names with MeshSelData.
	void ClearSetNames();
	void NSCopy();
	void NSPaste();
	BOOL GetUniqueSetName(TSTR &name);
	int SelectNamedSet();

	ParamDimension *GetParameterDim (int paramID);
	TSTR GetParameterName (int paramID);
};

class MeshSelData : public LocalModData, public IMeshSelectData {
public:
	// LocalModData
	void* GetInterface(ULONG id) { if (id == I_MESHSELECTDATA) return(IMeshSelectData*)this; else return LocalModData::GetInterface(id); }

	// Selection sets
	BitArray vertSel;
	BitArray faceSel;
	BitArray edgeSel;

	// Lists of named selection sets
	GenericNamedSelSetList vselSet;
	GenericNamedSelSetList fselSet;
	GenericNamedSelSetList eselSet;

	BOOL held;
	Mesh *mesh;
	AdjEdgeList *ae;

	// Cache of distances to nearest selected vert, plus cache of selections based on that.
	Tab<float> vd, vs;
	Interval vdValid;

	MeshSelData(Mesh &mesh);
	MeshSelData() { held=0;mesh=NULL; ae=NULL; vdValid=NEVER; }
	~MeshSelData() { FreeCache(); }
	LocalModData *Clone();
	Mesh *GetMesh() {return mesh;}
	AdjEdgeList *GetAdjEdgeList ();
	void SetCache(Mesh &mesh);
	void FreeCache();
	void SynchBitArrays();

	BitArray SelVertByFace();
	BitArray SelVertByEdge();
	BitArray SelFaceByVert();
	BitArray SelFaceByEdge();
	BitArray SelPolygonByVert (float thresh, int igVis);
	BitArray SelPolygonByEdge (float thresh, int igVis);
	BitArray SelElementByVert ();
	BitArray SelElementByEdge ();
	BitArray SelEdgeByVert();
	BitArray SelEdgeByFace();

	BitArray TempVSel (Mesh & m, DWORD slevel);
	void UpdateVDistances (TimeValue t, TriObject *tobj, DWORD slevel);
	void InvalidateVDistances ();
	void UpdateWeightedVSel (float falloff, float pinch, float bubble);
	BitArray GetSel (int nsl) { if (nsl==NS_VERTEX) return vertSel; else if (nsl==NS_EDGE) return edgeSel; else return faceSel; }

	// From IMeshSelectData:
	BitArray GetVertSel() { return vertSel; }
	BitArray GetFaceSel() { return faceSel; }
	BitArray GetEdgeSel() { return edgeSel; }

	void SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t);
	void SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t);
	void SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t);

	GenericNamedSelSetList & GetNamedVertSelList () { return vselSet; }
	GenericNamedSelSetList & GetNamedEdgeSelList () { return eselSet; }
	GenericNamedSelSetList & GetNamedFaceSelList () { return fselSet; }
	GenericNamedSelSetList & GetNamedSel (int nsl) {
		if (nsl==NS_VERTEX) return vselSet;
		if (nsl==NS_EDGE) return eselSet;
		return fselSet;
	}

	void GetWeightedVertSel (int nv, float *sel);
};

class MeshSelRestore : public RestoreObj {
public:
	BitArray usel, rsel;
	BitArray *sel;
	MeshSelMod *mod;
	MeshSelData *d;
	int level;

	MeshSelRestore(MeshSelMod *m, MeshSelData *d);
	MeshSelRestore(MeshSelMod *m, MeshSelData *d, int level);
	void Restore(int isUndo);
	void Redo();
	int Size() { return 1; }
	void EndHold() {d->held=FALSE;}
	TSTR Description() { return TSTR(_T("SelectRestore")); }
};

class AppendSetRestore : public RestoreObj {
public:
	BitArray set;		
	DWORD id;
	GenericNamedSelSetList *setList;
	TSTR name;

	AppendSetRestore(GenericNamedSelSetList *sl, DWORD i, TSTR & n) {setList = sl; id = i; name = n; }
	void Restore(int isUndo) {
		set  = *setList->GetSet (id);		
		setList->RemoveSet(id);			
	}
	void Redo() { setList->InsertSet (set, id, name); }

	TSTR Description() {return TSTR(_T("Append Set"));}
};

class AppendSetNameRestore : public RestoreObj {
	public:		
		TSTR name;
		DWORD id;
		MeshSelMod *et;
		Tab<TSTR*> *sets;
		Tab<DWORD> *ids;

		AppendSetNameRestore(MeshSelMod *e,Tab<TSTR*> *s,Tab<DWORD> *i) 
			{et = e; sets = s; ids = i;}
		void Restore(int isUndo) {			
			name = *(*sets)[sets->Count()-1];
			id   = (*ids)[sets->Count()-1];
			delete (*sets)[sets->Count()-1];
			sets->Delete(sets->Count()-1,1);			
			if (et->ip) et->ip->NamedSelSetListChanged();
			}
		void Redo() {
			TSTR *nm = new TSTR(name);
			sets->Append(1,&nm);
			ids->Append(1,&id);
			if (et->ip) et->ip->NamedSelSetListChanged();
			}
				
		TSTR Description() {return TSTR(_T("Append Set Name"));}
	};

class DeleteSetRestore : public RestoreObj {
public:
	BitArray set;
	DWORD id;
	TSTR name;
	//int index;
	GenericNamedSelSetList *setList;		

	DeleteSetRestore(GenericNamedSelSetList *sl,DWORD i, TSTR & n) {
		setList = sl; 
		id = i;
		BitArray *ptr = setList->GetSet(id);
		if (ptr) set = *ptr;
		name = n;
	}   		
	void Restore(int isUndo) {
		setList->InsertSet(set,id,name);
	}
	void Redo() {
		setList->RemoveSet(id);
	}
			
	TSTR Description() {return TSTR(_T("Delete Set"));}
};

class DeleteSetNameRestore : public RestoreObj {
public:		
	TSTR name;
	//int index;		
	DWORD id;
	MeshSelMod *et;
	Tab<TSTR*> *sets;
	Tab<DWORD> *ids;

	DeleteSetNameRestore(Tab<TSTR*> *s,MeshSelMod *e,Tab<DWORD> *i, DWORD id) {
		sets = s; et = e; //index = i;			
		this->id = id;
		ids = i;
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			name = *(*sets)[index];
			}
		}   		
	void Restore(int isUndo) {			
		TSTR *nm = new TSTR(name);			
		//sets->Insert(index,1,&nm);
		sets->Append(1,&nm);
		ids->Append(1,&id);
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
	void Redo() {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			sets->Delete(index,1);
			ids->Delete(index,1);
			}
		//sets->Delete(index,1);
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
			
	TSTR Description() {return TSTR(_T("Delete Set Name"));}
};

class SetNameRestore : public RestoreObj {
public:
	TSTR undo, redo;
	//int index;
	DWORD id;
	Tab<TSTR*> *sets;
	Tab<DWORD> *ids;
	MeshSelMod *et;
	SetNameRestore(Tab<TSTR*> *s,MeshSelMod *e,Tab<DWORD> *i,DWORD id) {
		//index = i; 
		this->id = id;
		ids = i;
		sets = s; et = e;
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			undo = *(*sets)[index];
			}			
		}

	void Restore(int isUndo) {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			redo = *(*sets)[index];
			*(*sets)[index] = undo;
			}			
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
	void Redo() {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
			}
		}
		if (index>=0) {
			*(*sets)[index] = redo;
		}
		if (et->ip) et->ip->NamedSelSetListChanged();
	}
			
	TSTR Description() {return TSTR(_T("Set Name"));}
};


//--- ClassDescriptor and class vars ---------------------------------

#define SELTYPE_SINGLE	1
#define SELTYPE_POLY	2
#define SELTYPE_ELEMENT	3

#define SEL_OBJECT	0
#define SEL_VERTEX	1
#define SEL_EDGE	2
#define SEL_FACE	3
#define SEL_POLY	4
#define SEL_ELEMENT 5

IObjParam *MeshSelMod::ip              = NULL;
IParamMap *MeshSelMod::pmapParam       = NULL;
MeshSelMod *MeshSelMod::editMod         = NULL;
HWND MeshSelMod::hParams         = NULL;
BOOL MeshSelMod::arRollup = TRUE;
BOOL MeshSelMod::selByVert       = FALSE;
BOOL MeshSelMod::ignoreBackfaces = FALSE;
BOOL MeshSelMod::ignoreVisEdge   = FALSE;
SelectModBoxCMode *MeshSelMod::selectMode      = NULL;
BOOL MeshSelMod::updateCachePosted = FALSE;

static float lastFaceThresh = 45.0f;
static int lastMatID = 1;

class MeshSelClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new MeshSelMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_MESHSELMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(MESHSELECT_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
};

static MeshSelClassDesc meshSelDesc;
ClassDesc* GetMeshSelModDesc() {return &meshSelDesc;}

static BOOL CALLBACK MeshSelectProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class AffectRegionProc : public ParamMapUserDlgProc {
public:
	MeshSelMod *em;
	AffectRegionProc () { em = NULL; }
	BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void DeleteThis() { }
};

static AffectRegionProc theAffectRegionProc;

// Table to convert selLevel values to mesh selLevel flags.
const int meshLevel[] = {MESH_OBJECT, MESH_VERTEX, MESH_EDGE, MESH_FACE, MESH_FACE, MESH_FACE};

// Get display flags based on selLevel.
const DWORD levelDispFlags[] = {0,DISP_VERTTICKS|DISP_SELVERTS,DISP_SELEDGES,DISP_SELFACES,DISP_SELPOLYS,DISP_SELPOLYS};

// For hit testing...
const int hitLevel[] = {0,SUBHIT_VERTS,SUBHIT_EDGES,SUBHIT_FACES,SUBHIT_FACES,SUBHIT_FACES};

// Parameter block:
#define PB_USE_AR 0
#define PB_FALLOFF 1
#define PB_PINCH 2
#define PB_BUBBLE 3

static ParamUIDesc descParam[] = {
	// Use affect region?
	ParamUIDesc (PB_USE_AR, TYPE_SINGLECHEKBOX, IDC_MS_AR_USE),

	ParamUIDesc (PB_FALLOFF, EDITTYPE_POS_UNIVERSE,
		IDC_FALLOFF, IDC_FALLOFFSPIN,
		0.0001f, 9999999999.f, SPIN_AUTOSCALE),

	ParamUIDesc (PB_PINCH, EDITTYPE_UNIVERSE,
		IDC_PINCH, IDC_PINCHSPIN,
		-10.0f, 10.0f, 0.01f),

	ParamUIDesc (PB_BUBBLE, EDITTYPE_UNIVERSE,
		IDC_BUBBLE, IDC_BUBBLESPIN,
		-10.0f, 10.0f, 0.01f),
};
#define PARAMDESC_LENGTH 4

static ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, TRUE, PB_USE_AR },
	{ TYPE_FLOAT, NULL, TRUE, PB_FALLOFF },
	{ TYPE_FLOAT, NULL, TRUE, PB_PINCH },
	{ TYPE_FLOAT, NULL, TRUE, PB_BUBBLE }
};
#define PBLOCK_LENGTH	4

/*
// Array of old versions - not used yet
static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,4,0)
};
#define NUM_OLDVERSIONS	1
*/

// Current version
#define CURRENT_VERSION	0
static ParamVersionDesc curVersion (descVer0, PBLOCK_LENGTH, CURRENT_VERSION);

//--- MeshSel mod methods -------------------------------

MeshSelMod::MeshSelMod() {
	selLevel = SEL_OBJECT;
	pblock = NULL;
	flags = 0x0;

	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descVer0, PBLOCK_LENGTH, CURRENT_VERSION));
	pblock->SetValue (PB_USE_AR, TimeValue(0), 0);
	pblock->SetValue (PB_FALLOFF, TimeValue(0), DEF_FALLOFF);
	pblock->SetValue (PB_PINCH, TimeValue(0), 0.0f);
	pblock->SetValue (PB_BUBBLE, TimeValue(0), 0.0f);
}

RefTargetHandle MeshSelMod::Clone(RemapDir& remap) {
	MeshSelMod *mod = new MeshSelMod();
	mod->selLevel = selLevel;
	mod->ReplaceReference (0, pblock->Clone(remap));
	return mod;
}

Interval MeshSelMod::GetValidity (TimeValue t) {
	Interval ret = FOREVER;
	int useAR;
	pblock->GetValue (PB_USE_AR, t, useAR, ret);
	if (!useAR) return ret;
	float f, p, b;
	pblock->GetValue (PB_FALLOFF, t, f, ret);
	pblock->GetValue (PB_PINCH, t, p, ret);
	pblock->GetValue (PB_BUBBLE, t, b, ret);
	return ret;
}

BOOL MeshSelMod::HasWeightedVertSel() {
	BOOL useAR;
	pblock->GetValue (PB_USE_AR, 0, useAR, FOREVER);
	return useAR;
}

float AffectRegionFunct(float dist,float falloff,float pinch,float bubble) {
	float u = ((falloff - dist)/falloff);
	float u2 = u*u, s = 1.0f-u;	
	return (3*u*bubble*s + 3*u2*(1.0f-pinch))*s + u*u2;
}

BitArray MeshSelData::TempVSel (Mesh & m, DWORD selLevel) {
	BitArray vsel;
	vsel.SetSize (m.numVerts);

	int i, j;
	switch (selLevel) {
	case SEL_OBJECT:
		vsel.SetAll ();
		break;
	case SEL_VERTEX:
		vsel = vertSel;
		break;

	case SEL_EDGE:
		vsel.ClearAll ();
		for (i=0; i<m.numFaces; i++) {
			for (j=0; j<3; j++) {
				if (edgeSel[i*3+j]) {
					vsel.Set (m.faces[i].v[j]);
					vsel.Set (m.faces[i].v[(j+1)%3]);
				}
			}
		}
		break;

	default:
		vsel.ClearAll ();
		for (i=0; i<m.numFaces; i++) {
			if (!faceSel[i]) continue;
			for (j=0; j<3; j++) vsel.Set (m.faces[i].v[j]);
		}
		break;
	}
	return vsel;
}

void MeshSelData::InvalidateVDistances () {
	vdValid.SetEmpty ();
}

void MeshSelData::UpdateVDistances (TimeValue t, TriObject *tobj, DWORD selLevel) {
	if (vdValid.InInterval(t)) return;

	Point3 *v = tobj->GetMesh().verts;
	vdValid = tobj->ChannelValidity (t, GEOM_CHAN_NUM);
	vdValid &= tobj->ChannelValidity (t, TOPO_CHAN_NUM);

	int i, vnum=tobj->GetMesh().numVerts;
	BitArray vsel = TempVSel (tobj->GetMesh(), selLevel);
	vd.SetCount (vnum);

	Tab<int> sverts;
	sverts.Resize (vnum);
	for (i=0; i<vnum; i++) {
		if (!vsel[i]) continue;
		sverts.Append (1, &i);
		vd[i] = 0.0f;
	}
	if (sverts.Count() == 0) {
		for (i=0; i<vnum; i++) vd[i] = -1.0f;
		return;
	}
	for (i=0; i<vnum; i++) {
		if (vsel[i]) continue;
		float mindist = LengthSquared (v[i] - v[sverts[0]]);
		int best = 0;
		for (int j=1; j<sverts.Count(); j++) {
			float dist = LengthSquared (v[i] - v[sverts[j]]);
			if (dist<mindist) {
				best = j;
				mindist = dist;
			}
		}
		vd[i] = Sqrt (mindist);
	}
}

void MeshSelData::UpdateWeightedVSel (float falloff, float pinch, float bubble) {
	int i, nv = vd.Count();
	vs.SetCount (nv);
	for (i=0; i<nv; i++) {
		if (vd[i]<0) {
			vs[i] = 0.0f;
			continue;
		}
		if (vd[i]==0) vs[i] = 1.0f;
		else {
			if (vd[i] > falloff) vs[i] = 0.0f;
			else vs[i] = AffectRegionFunct (vd[i], falloff, pinch, bubble);
		}
	}
}

void MeshSelData::GetWeightedVertSel (int nv, float *wvs) {
	int min = (nv<vertSel.GetSize()) ? nv : vertSel.GetSize ();
	for (int i=0; i<min; i++) wvs[i] = vs[i];
	for (i=min; i<nv; i++) wvs[i] = 0.0f;
}

void MeshSelMod::ModifyObject (TimeValue t, ModContext &mc, ObjectState *os, INode *node) {
	if (!os->obj->IsSubClassOf(triObjectClassID)) return;
	TriObject *tobj = (TriObject*)os->obj;
	MeshSelData *d  = (MeshSelData*)mc.localData;
	if (!d) mc.localData = d = new MeshSelData(tobj->GetMesh());
	if ((editMod==this) && (!d->GetMesh())) d->SetCache(tobj->GetMesh());

	BitArray vertSel = d->vertSel;
	BitArray faceSel = d->faceSel;
	BitArray edgeSel = d->edgeSel;
	vertSel.SetSize(tobj->GetMesh().getNumVerts(),TRUE);
	faceSel.SetSize(tobj->GetMesh().getNumFaces(),TRUE);
	edgeSel.SetSize(tobj->GetMesh().getNumFaces()*3,TRUE);
	tobj->GetMesh().vertSel = vertSel;
	tobj->GetMesh().faceSel = faceSel;
	tobj->GetMesh().edgeSel = edgeSel;

	int useAR;
	Interval outValid;
	outValid = tobj->ChannelValidity (t, SELECT_CHAN_NUM);
	pblock->GetValue (PB_USE_AR, t, useAR, outValid);

	if (useAR) {
		float bubble, pinch, falloff;
		pblock->GetValue (PB_FALLOFF, t, falloff, outValid);
		pblock->GetValue (PB_PINCH, t, pinch, outValid);
		pblock->GetValue (PB_BUBBLE, t, bubble, outValid);

		tobj->GetMesh().SupportVSelectionWeights ();
		float *vs = tobj->GetMesh().getVSelectionWeights ();
		int nv = tobj->GetMesh().getNumVerts();
		d->UpdateVDistances (t, tobj, selLevel);
		d->UpdateWeightedVSel (falloff, pinch, bubble);
		d->GetWeightedVertSel (nv, vs);

		if (pmapParam) pmapParam->Invalidate();
	} else {
		tobj->GetMesh().ClearVSelectionWeights ();
	}

	if (d->GetMesh()) {
		// Keep the cache up to date if it exists.
		d->GetMesh()->vertSel = vertSel;
		d->GetMesh()->faceSel = faceSel;
		d->GetMesh()->edgeSel = edgeSel;
	}

	tobj->GetMesh().dispFlags = 0;
	tobj->GetMesh().SetDispFlag (levelDispFlags[selLevel]);
	tobj->GetMesh().selLevel = meshLevel[selLevel];
	tobj->SetChannelValidity (SELECT_CHAN_NUM, outValid);
}

void MeshSelMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc) {
	if (!mc->localData) return;
	if (partID == PART_SELECT) return;
	((MeshSelData*)mc->localData)->FreeCache();
	if (ip && hParams && editMod==this && !updateCachePosted) {
		TimeValue t = ip->GetTime();
		PostMessage(hParams,WM_UPDATE_CACHE,(WPARAM)t,0);
		updateCachePosted = TRUE;
	}
}

void MeshSelMod::UpdateCache(TimeValue t) {
	NotifyDependents(Interval(t,t), PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|
		PART_DISPLAY|PART_TOPO, REFMSG_MOD_EVAL);
	updateCachePosted = FALSE;
}

void MeshSelMod::InvalidateVDistances () {
	if (!ip) return;
	INodeTab nds;
	ModContextList mcList;
	ip->GetModContexts (mcList, nds);
	int i;
	for (i=0; i<mcList.Count(); i++) {
		MeshSelData *d = (MeshSelData *) mcList[i]->localData;
		if (!d) continue;
		d->InvalidateVDistances ();
	}
}

static int butIDs[] = { 0, IDC_SELVERTEX, IDC_SELEDGE, IDC_SELFACE, IDC_SELPOLY, IDC_SELELEMENT };
void MeshSelMod::UpdateSelLevelDisplay () {
	ICustToolbar *iToolbar = GetICustToolbar(GetDlgItem(hParams,IDC_MS_SELTYPE));
	ICustButton *but;
	for (int i=1; i<6; i++) {
		but = iToolbar->GetICustButton (butIDs[i]);
		but->SetCheck ((DWORD)i==selLevel);
		ReleaseICustButton (but);
	}
	ReleaseICustToolbar (iToolbar);
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	UpdateWindow(hParams);
	ip->RedrawViews(ip->GetTime());
}

static bool oldShowEnd;

void MeshSelMod::BeginEditParams (IObjParam  *ip, ULONG flags,Animatable *prev) {
	this->ip = ip;	
	editMod  = this;
	UpdateSetNames ();

	hParams = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_MESH_SELECT),
		MeshSelectProc, GetString(IDS_MS_PARAMS), (LPARAM)this);
	pmapParam = CreateCPParamMap (descParam, PARAMDESC_LENGTH, pblock,
		ip, hInstance, MAKEINTRESOURCE(IDD_MESHSEL_AFFECTREGION),
		GetString(IDS_MS_AFFECTREGION), arRollup ? APPENDROLL_CLOSED : 0);
	theAffectRegionProc.em = this;
	pmapParam->SetUserDlgProc (&theAffectRegionProc);

	selectMode = new SelectModBoxCMode(this,ip);

	// Add our sub object type
	TSTR type1(GetString(IDS_RB_VERTEX));
	TSTR type2(GetString(IDS_RB_EDGE));
	TSTR type3(GetString(IDS_RB_FACE));
	TSTR type4(GetString(IDS_EM_POLY));
	TSTR type5(GetString(IDS_EM_ELEMENT));
	const TCHAR *ptype[] = {type1, type2, type3, type4, type5};
	ip->RegisterSubObjectTypes(ptype, 5);

	// Restore the selection level.
	ip->SetSubObjectLevel(selLevel);

	SetEnableStates ();
	UpdateSelLevelDisplay ();
	SetNumSelLabel();

	// Set show end result.
	oldShowEnd = ip->GetShowEndResult() ? TRUE : FALSE;
	ip->SetShowEndResult (GetFlag (MS_DISP_END_RESULT));

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);	
}

void MeshSelMod::EndEditParams (IObjParam *ip,ULONG flags,Animatable *next) {
	if (hParams) ip->DeleteRollupPage(hParams);
	hParams = NULL;
	if (pmapParam) {
		arRollup = IsRollupPanelOpen (pmapParam->GetHWnd ());
		DestroyCPParamMap(pmapParam);
		pmapParam = NULL;
	}
	theAffectRegionProc.em = NULL;

	ip->DeleteMode(selectMode);
	if (selectMode) delete selectMode;
	selectMode = NULL;

	// Reset show end result
	SetFlag (MS_DISP_END_RESULT, ip->GetShowEndResult() ? TRUE : FALSE);
	ip->SetShowEndResult(oldShowEnd);

	this->ip = NULL;
	editMod  = NULL;

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	ClearAFlag(A_MOD_BEING_EDITED);
}

int MeshSelMod::HitTest (TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc) {
	Interval valid;
	int savedLimits, res = 0;
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	
	// Setup GW
	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	Matrix3 mat = inode->GetObjectTM(t);
	gw->setTransform(mat);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	if (ignoreBackfaces) 
		 gw->setRndLimits(gw->getRndLimits() |  GW_BACKCULL);
	else gw->setRndLimits(gw->getRndLimits() & ~GW_BACKCULL);
	gw->clearHitCode();
		
	SubObjHitList hitList;
	MeshSubHitRec *rec;	

	if (!mc->localData || !((MeshSelData*)mc->localData)->GetMesh()) return 0;

	Mesh &mesh = *((MeshSelData*)mc->localData)->GetMesh();
	if (selLevel>=SEL_FACE && selByVert) {
		res = mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr,
			flags|hitLevel[SEL_VERTEX]|SUBHIT_USEFACESEL, hitList);
	} else {
		res = mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr,
				flags|hitLevel[selLevel], hitList);
		}

	rec = hitList.First();
	while (rec) {
		vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
		rec = rec->Next();
		}

	gw->setRndLimits(savedLimits);	
	return res;	
	}

int MeshSelMod::Display (TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {
	if (!ip->GetShowEndResult ()) return 0;
	if (!selLevel) return 0;
	if (!mc->localData) return 0;

	MeshSelData *modData = (MeshSelData *) mc->localData;
	Mesh *mesh = modData->GetMesh();
	if (!mesh) return 0;

	// Set up GW
	GraphicsWindow *gw = vpt->getGW();
	Matrix3 tm = inode->GetObjectTM(t);
	int savedLimits;
	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);

	// We need to draw a "gizmo" version of the mesh:
	Point3 colSel=GetSubSelColor();
	Point3 colTicks=GetUIColor (COLOR_VERT_TICKS);
	Point3 colGiz=GetUIColor(COLOR_GIZMOS);
	Point3 colGizSel=GetUIColor(COLOR_SEL_GIZMOS);
	gw->setColor (LINE_COLOR, colGiz);

	AdjEdgeList *ae = modData->GetAdjEdgeList ();
	Point3 rp[3];
	int i, ect = ae->edges.Count();
	int es[3];
	for (i=0; i<ect; i++) {
		MEdge & me = ae->edges[i];
		if (me.Hidden (mesh->faces)) continue;
		if (me.Visible (mesh->faces)) {
			es[0] = GW_EDGE_VIS;
		} else {
			if (selLevel < SEL_EDGE) continue;
			if (selLevel > SEL_FACE) continue;
			es[0] = GW_EDGE_INVIS;
		}
		if (selLevel == SEL_EDGE) {
			if (ae->edges[i].Selected (mesh->faces, modData->GetEdgeSel())) gw->setColor (LINE_COLOR, colGizSel);
			else gw->setColor (LINE_COLOR, colGiz);
		}
		if (selLevel >= SEL_FACE) {
			if (ae->edges[i].AFaceSelected (modData->GetFaceSel())) gw->setColor (LINE_COLOR, colGizSel);
			else gw->setColor (LINE_COLOR, colGiz);
		}
		rp[0] = mesh->verts[me.v[0]];
		rp[1] = mesh->verts[me.v[1]];
		gw->polyline (2, rp, NULL, NULL, FALSE, es);
	}
	if (selLevel == SEL_VERTEX) {
		float *ourvw = NULL;
		int affectRegion=FALSE;
		if (pblock) pblock->GetValue (PB_USE_AR, t, affectRegion, FOREVER);
		if (affectRegion && modData->vs.Count()) ourvw = modData->vs.Addr(0);
		for (i=0; i<mesh->numVerts; i++) {
			if (mesh->vertHide[i]) continue;

			if (modData->GetVertSel()[i]) gw->setColor (LINE_COLOR, colSel);
			else {
				if (ourvw) gw->setColor (LINE_COLOR, SoftSelectionColor (ourvw[i]));
				else gw->setColor (LINE_COLOR, colTicks);
			}

			if(getUseVertexDots()) gw->marker (&(mesh->verts[i]), getVertexDotType() ? DOT_MRKR : SM_DOT_MRKR);
			else gw->marker (&(mesh->verts[i]), PLUS_SIGN_MRKR);
		}
	}
	gw->setRndLimits(savedLimits);
	return 0;	
}

void MeshSelMod::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp *vpt, Box3& box, ModContext *mc) {
	if (!ip->GetShowEndResult() || !mc->localData) return;
	if (!selLevel) return;
	MeshSelData *modData = (MeshSelData *) mc->localData;
	Mesh *mesh = modData->GetMesh();
	if (!mesh) return;
	Matrix3 tm = inode->GetObjectTM(t);
	box = mesh->getBoundingBox (&tm);
}

void MeshSelMod::GetSubObjectCenters (SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc) {
	if (!mc->localData) return;
	if (selLevel == SEL_OBJECT) return;	// shouldn't happen.
	MeshSelData *modData = (MeshSelData *) mc->localData;
	Mesh *mesh = modData->GetMesh();
	if (!mesh) return;
	Matrix3 tm = node->GetObjectTM(t);

	// For Mesh Select, we merely return the center of the bounding box of the current selection.
	BitArray sel = modData->TempVSel(*mesh, selLevel);
	if (!sel.NumberSet()) return;
	Box3 box;
	for (int i=0; i<mesh->numVerts; i++) if (sel[i]) box += mesh->verts[i] * tm;
	cb->Center (box.Center(),0);
}

void MeshSelMod::GetSubObjectTMs (SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc) {
	if (!mc->localData) return;
	if (selLevel == SEL_OBJECT) return;	// shouldn't happen.
	MeshSelData *modData = (MeshSelData *) mc->localData;
	Mesh *mesh = modData->GetMesh();
	if (!mesh) return;
	Matrix3 tm = node->GetObjectTM(t);

	// For Mesh Select, we merely return the center of the bounding box of the current selection.
	BitArray sel = modData->TempVSel(*mesh, selLevel);
	if (!sel.NumberSet()) return;
	Box3 box;
	for (int i=0; i<mesh->numVerts; i++) if (sel[i]) box += mesh->verts[i] * tm;
	Matrix3 ctm(1);
	ctm.SetTrans (box.Center());
	cb->TM (ctm,0);
}

void MeshSelMod::ActivateSubobjSel(int level, XFormModes& modes) {
	// Set the meshes level
	selLevel = level;

	// Fill in modes with our sub-object modes
	if (level!=SEL_OBJECT) {
		modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
	}

	// Update UI
	UpdateSelLevelDisplay ();
	SetEnableStates ();
	SetNumSelLabel ();

	// Setup named selection sets	
	SetupNamedSelDropDown();
	UpdateNamedSelDropDown ();

	// Invalidate the weighted vertex caches
	InvalidateVDistances ();

	NotifyDependents(FOREVER, PART_SUBSEL_TYPE|PART_DISPLAY, REFMSG_CHANGE);
	ip->PipeSelLevelChanged();
	NotifyDependents(FOREVER, SELECT_CHANNEL|DISP_ATTRIB_CHANNEL|SUBSEL_TYPE_CHANNEL, REFMSG_CHANGE);
}

DWORD MeshSelMod::GetSelLevel () {
	switch (selLevel) {
	case SEL_OBJECT: return IMESHSEL_OBJECT;
	case SEL_VERTEX: return IMESHSEL_VERTEX;
	case SEL_EDGE: return IMESHSEL_EDGE;
	}
	return IMESHSEL_FACE;
}

void MeshSelMod::SetSelLevel(DWORD sl) {
	switch (sl) {
	case IMESHSEL_OBJECT:
		selLevel = SEL_OBJECT;
		break;
	case IMESHSEL_VERTEX:
		selLevel = SEL_VERTEX;
		break;
	case IMESHSEL_EDGE:
		selLevel = SEL_EDGE;
		break;
	case IMESHSEL_FACE:
		if (selLevel < SEL_FACE) selLevel = SEL_POLY;	// don't change if we're already in a face mode.
		break;
	}
	if (ip && editMod==this) ip->SetSubObjectLevel(selLevel);
}

void MeshSelMod::LocalDataChanged() {
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	if (ip && editMod==this) {
		SetNumSelLabel();
		UpdateNamedSelDropDown ();
	}
}

void MeshSelMod::UpdateNamedSelDropDown () {
	if (!ip) return;
	if (selLevel == SEL_OBJECT) {
		ip->ClearCurNamedSelSet ();
		return;
	}
	// See if this selection matches a named set
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts (mcList, nodes);
	BitArray nselmatch;
	nselmatch.SetSize (namedSel[namedSetLevel[selLevel]].Count());
	nselmatch.SetAll ();
	int nd, i, foundone=FALSE;
	for (nd=0; nd<mcList.Count(); nd++) {
		MeshSelData *d = (MeshSelData *) mcList[nd]->localData;
		if (!d) continue;
		foundone = TRUE;
		switch (selLevel) {
		case SEL_VERTEX:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->vselSet.sets[i]) == d->vertSel)) nselmatch.Clear(i);
			}
			break;
		case SEL_EDGE:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->eselSet.sets[i]) == d->edgeSel)) nselmatch.Clear(i);
			}
			break;
		default:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->fselSet.sets[i]) == d->faceSel)) nselmatch.Clear(i);
			}
			break;
		}
		if (nselmatch.NumberSet () == 0) break;
	}
	if (foundone && nselmatch.NumberSet ()) {
		for (i=0; i<nselmatch.GetSize(); i++) if (nselmatch[i]) break;
		ip->SetCurNamedSelSet (*(namedSel[namedSetLevel[selLevel]][i]));
	} else ip->ClearCurNamedSelSet ();
}

HitRecord *MeshSelMod::VertHitsToFaceHits(HitRecord *hitRec,BOOL all) {	
	HitRecord *ptr = NULL;
	TimeValue t = ip->GetTime();

	while (hitRec) {
		Mesh &mesh = *(((MeshSelData*)hitRec->modContext->localData)->GetMesh());
		AdjEdgeList el(mesh);		
		
		// Add faces adjacent to this vertex
		DWORDTab &list = el[hitRec->hitInfo];
		for (int i=0; i<list.Count(); i++) {
			if (el.edges[list[i]].f[0]!=UNDEFINED) {
				ptr = new HitRecord(			
					ptr,
					hitRec->nodeRef,
					hitRec->modContext,
					hitRec->distance,
					el.edges[list[i]].f[0],
					NULL);
				}
			if (el.edges[list[i]].f[1]!=UNDEFINED) {
				ptr = new HitRecord(				
					ptr,
					hitRec->nodeRef,
					hitRec->modContext,
					hitRec->distance,
					el.edges[list[i]].f[1],
					NULL);
				}
			}
		
		if (!all) break;
		hitRec = hitRec->Next();
		}
	return ptr;
	}

static AdjFaceList *BuildAdjFaceList(Mesh &mesh)
	{
	AdjEdgeList ae(mesh);
	return new AdjFaceList(mesh,ae);
	}

void MeshSelMod::SelectSubComponent (HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert) {
	MeshSelData *d = NULL, *od = NULL;

	ip->ClearCurNamedSelSet();

	HitRecord *faceHitRec = NULL;
	if ((selLevel >= SEL_FACE) && selByVert) faceHitRec = VertHitsToFaceHits(hitRec,all);

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	TimeValue t = ip->GetTime();
	int nd;
	BitArray nsel;
	AdjFaceList *al = NULL;
	Mesh *mesh;

	for (nd=0; nd<mcList.Count(); nd++) {
		d = (MeshSelData*) mcList[nd]->localData;
		if (d==NULL) continue;
		HitRecord *hr = faceHitRec ? faceHitRec : hitRec;
		if (!all && (hr->modContext->localData != d)) continue;
		for (; hr!=NULL; hr=hr->Next()) if (hr->modContext->localData == d) break;
		if (hr==NULL) continue;

		switch (selLevel) {
		case SEL_VERTEX:
			nsel = d->vertSel;
			for (; hr != NULL; hr = hr->Next()) {
				if (d != hr->modContext->localData) continue;
				nsel.Set (hr->hitInfo, invert ? !nsel[hr->hitInfo] : selected);
				if (!all) break;
			}
			d->SetVertSel (nsel, this, t);
			break;

		case SEL_EDGE:
			nsel = d->edgeSel;
			for (; hr != NULL; hr = hr->Next()) {
				if (d != hr->modContext->localData) continue;
				nsel.Set (hr->hitInfo, invert ? !nsel[hr->hitInfo] : selected);
				if (!all) break;
			}
			d->SetEdgeSel (nsel, this, t);
			break;

		case SEL_FACE:
			mesh = d->GetMesh();
			if (!mesh) break;
			nsel = d->faceSel;
			for (; hr != NULL; hr=hr->Next()) {
				if (d != hr->modContext->localData) continue;
				nsel.Set (hr->hitInfo, invert ? !mesh->faceSel[hr->hitInfo] : selected);
				if (!all) break;
			}
			d->SetFaceSel (nsel, this, t);
			break;

		case SEL_POLY:
		case SEL_ELEMENT:
			mesh = d->GetMesh();
			if (!mesh) break;
			al = BuildAdjFaceList(*mesh);
			nsel.SetSize (mesh->numFaces);
			nsel.ClearAll ();
			for (; hr != NULL; hr=hr->Next()) {
				if (hr->modContext->localData != d) continue;
				if (selLevel == SEL_POLY)
					mesh->PolyFromFace (hr->hitInfo, nsel, GetPolyFaceThresh(), ignoreVisEdge, al);
				else
					mesh->ElementFromFace (hr->hitInfo, nsel, al);
				if (!all) break;
			}
			delete al;

			if (invert) nsel ^= d->faceSel;
			else if (selected) nsel |= d->faceSel;
			else nsel = d->faceSel & ~nsel;

			d->SetFaceSel (nsel, this, t);
			break;
		}
	}

	if (faceHitRec) {
		// Delete the hits:
		HitRecord *ptr = faceHitRec;
		while (ptr) {
			faceHitRec = ptr;
			ptr = ptr->Next();
			delete faceHitRec;
		}
	}

	nodes.DisposeTemporary ();
	LocalDataChanged ();
}

void MeshSelMod::ClearSelection(int selLevel) {
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	BitArray nsel;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;
		if (!d) continue;

		switch (selLevel) {
		case SEL_VERTEX:
			if (!d->vertSel.NumberSet()) break;
			nsel.SetSize (d->vertSel.GetSize());
			nsel.ClearAll ();
			d->SetVertSel (nsel, this, ip->GetTime());
			break;

		case SEL_FACE:
		case SEL_POLY:
		case SEL_ELEMENT:
			if (!d->faceSel.NumberSet()) break;
			nsel.SetSize (d->faceSel.GetSize());
			nsel.ClearAll ();
			d->SetFaceSel (nsel, this, ip->GetTime());
			break;

		case SEL_EDGE:
			if (!d->edgeSel.NumberSet()) break;
			nsel.SetSize (d->edgeSel.GetSize());
			nsel.ClearAll ();
			d->SetEdgeSel (nsel, this, ip->GetTime());
			break;
		}
	}
	nodes.DisposeTemporary();
	LocalDataChanged ();
}

void MeshSelMod::SelectAll(int selLevel) {
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	BitArray nsel;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;		
		if (!d) continue;
		switch (selLevel) {
		case SEL_VERTEX:
			nsel.SetSize (d->vertSel.GetSize());
			nsel.SetAll();
			d->SetVertSel (nsel, this, ip->GetTime());
			break;
		case SEL_FACE:
		case SEL_POLY:
		case SEL_ELEMENT:
			nsel.SetSize (d->faceSel.GetSize());
			nsel.SetAll();
			d->SetFaceSel (nsel, this, ip->GetTime());
			break;
		case SEL_EDGE:
			nsel.SetSize (d->edgeSel.GetSize());
			nsel.SetAll();
			d->SetEdgeSel (nsel, this, ip->GetTime());
			break;
		}
	}
	nodes.DisposeTemporary();
	LocalDataChanged ();
}

void MeshSelMod::InvertSelection(int selLevel) {
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;
		if (!d) continue;
		switch (selLevel) {
		case SEL_VERTEX:
			d->SetVertSel (~d->vertSel, this, ip->GetTime());
			break;
		case SEL_FACE:
		case SEL_POLY:
		case SEL_ELEMENT:
			d->SetFaceSel (~d->faceSel, this, ip->GetTime());
			break;
		case SEL_EDGE:
			d->SetEdgeSel (~d->edgeSel, this, ip->GetTime());
			break;
		}
	}
	nodes.DisposeTemporary();
	LocalDataChanged ();
}

void MeshSelMod::SelectByMatID(int id) {
	BOOL add = GetKeyState(VK_CONTROL)<0 ? TRUE : FALSE;
	BOOL sub = GetKeyState(VK_MENU)<0 ? TRUE : FALSE;
	theHold.Begin();
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	BitArray nsel;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;		
		if (!d) continue;
		nsel = d->faceSel;
		if (!add && !sub) nsel.ClearAll();
		Mesh *mesh = d->GetMesh();
		if (!mesh) continue;
		for (int i=0; i<mesh->numFaces; i++) {
			if (mesh->faces[i].getMatID()==(MtlID)id) {
				if (sub) nsel.Clear(i);
				else nsel.Set(i);
			}
		}
		d->SetFaceSel (nsel, this, ip->GetTime ());
	}
	nodes.DisposeTemporary();
	theHold.Accept(GetString (IDS_RB_SELECTBYMATID));
	LocalDataChanged ();
	ip->RedrawViews(ip->GetTime());
}

void MeshSelMod::SelectOpenEdges() {
	theHold.Begin();
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	BitArray nsel;
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;		
		if (!d) continue;
		if (!d->mesh) continue;

		nsel.SetSize (d->mesh->numFaces*3);
		nsel.ClearAll ();

		AdjEdgeList adj(*d->mesh);
		for (int j=0; j<adj.edges.Count(); j++) {
			MEdge & me = adj.edges[j];
			if (me.f[0]==UNDEFINED) {
				int wh = d->mesh->faces[me.f[1]].GetEdgeIndex (me.v[0], me.v[1]);
				nsel.Set (me.f[1]*3+wh);
			}
			if (me.f[1]==UNDEFINED) {
				int wh = d->mesh->faces[me.f[0]].GetEdgeIndex (me.v[0], me.v[1]);
				nsel.Set (me.f[0]*3+wh);
			}
		}
		d->SetEdgeSel (nsel, this, ip->GetTime());
	}
	nodes.DisposeTemporary();
	theHold.Accept(GetString (IDS_EM_SELECT_OPEN));
	LocalDataChanged ();
	ip->RedrawViews(ip->GetTime());
}

void MeshSelMod::SelectFrom(int from) {
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);
	MeshSelData *d;
	theHold.Begin();
	for (int i=0; i<list.Count(); i++) {
		d = (MeshSelData*)list[i]->localData;
		if (!d) continue;

		switch (selLevel) {
		case SEL_VERTEX: 
			if (from>=SEL_FACE) d->SetVertSel (d->SelVertByFace(), this, ip->GetTime());
			else d->SetVertSel (d->SelVertByEdge(), this, ip->GetTime());
			break;
		case SEL_FACE:
			if (from==SEL_VERTEX) d->SetFaceSel (d->SelFaceByVert(), this, ip->GetTime());
			else d->SetFaceSel (d->SelFaceByEdge(), this, ip->GetTime());
			break;
		case SEL_POLY:
			if (from==SEL_VERTEX) d->SetFaceSel (d->SelPolygonByVert(GetPolyFaceThresh(), ignoreVisEdge), this, ip->GetTime());
			else d->SetFaceSel (d->SelPolygonByEdge(GetPolyFaceThresh(), ignoreVisEdge), this, ip->GetTime());
			break;
		case SEL_ELEMENT:
			if (from==SEL_VERTEX) d->SetFaceSel (d->SelElementByVert(), this, ip->GetTime());
			else d->SetFaceSel (d->SelElementByEdge(), this, ip->GetTime());
			break;
		case SEL_EDGE:
			if (from==SEL_VERTEX) d->SetEdgeSel (d->SelEdgeByVert(), this, ip->GetTime());
			else d->SetEdgeSel (d->SelEdgeByFace(), this, ip->GetTime());
			break;
		}
	}
	theHold.Accept(GetString(IDS_DS_SELECT));
	nodes.DisposeTemporary();
	LocalDataChanged ();
	ip->RedrawViews(ip->GetTime());
}

float MeshSelMod::GetPolyFaceThresh() {
	ISpinnerControl *spin = GetISpinner(GetDlgItem(hParams,IDC_MS_PLANARSPINNER));
	float thresh = spin->GetFVal();
	ReleaseISpinner(spin);
	return DegToRad(thresh);
}

#define SELLEVEL_CHUNKID		0x0100
#define VERTSEL_CHUNKID			0x0200
#define FACESEL_CHUNKID			0x0210
#define EDGESEL_CHUNKID			0x0220
#define VERSION_CHUNKID        0x0230
#define FLAGS_CHUNKID 0x0240
static int currentVersion = 3;

#define NAMEDVSEL_NAMES_CHUNK	0x2805
#define NAMEDFSEL_NAMES_CHUNK	0x2806
#define NAMEDESEL_NAMES_CHUNK	0x2807
#define NAMEDSEL_STRING_CHUNK	0x2809
#define NAMEDSEL_ID_CHUNK		0x2810

#define VSELSET_CHUNK			0x2845
#define FSELSET_CHUNK			0x2846
#define ESELSET_CHUNK			0x2847

static int namedSelID[] = {NAMEDVSEL_NAMES_CHUNK,NAMEDESEL_NAMES_CHUNK,NAMEDFSEL_NAMES_CHUNK, NAMEDFSEL_NAMES_CHUNK, NAMEDFSEL_NAMES_CHUNK};

IOResult MeshSelMod::Save(ISave *isave) {
	IOResult res;
	ULONG nb;
	Modifier::Save(isave);
	isave->BeginChunk(SELLEVEL_CHUNKID);
	res = isave->Write(&selLevel, sizeof(selLevel), &nb);
	isave->EndChunk();
	isave->BeginChunk (VERSION_CHUNKID);
	res = isave->Write (&currentVersion, sizeof(int), &nb);
	isave->EndChunk();

	isave->BeginChunk (FLAGS_CHUNKID);
	res = isave->Write (&flags, sizeof(DWORD), &nb);
	isave->EndChunk();

	for (int j=0; j<3; j++) {
		if (namedSel[j].Count()) {
			isave->BeginChunk(namedSelID[j]);			
			for (int i=0; i<namedSel[j].Count(); i++) {
				isave->BeginChunk(NAMEDSEL_STRING_CHUNK);
				isave->WriteWString(*namedSel[j][i]);
				isave->EndChunk();

				isave->BeginChunk(NAMEDSEL_ID_CHUNK);
				isave->Write(&ids[j][i],sizeof(DWORD),&nb);
				isave->EndChunk();
				}
			isave->EndChunk();
			}
		}

	return res;
	}

IOResult MeshSelMod::LoadNamedSelChunk(ILoad *iload,int level) {	
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
			namedSel[level].Append(1,&newName);				
			ids[level].Append(1,&ix);
			ix++;
			break;
			}
		case NAMEDSEL_ID_CHUNK:
			iload->Read(&ids[level][ids[level].Count()-1],sizeof(DWORD), &nb);
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}

IOResult MeshSelMod::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	int version = 2;
	Modifier::Load(iload);
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case SELLEVEL_CHUNKID:
			iload->Read(&selLevel, sizeof(selLevel), &nb);
			break;

		case VERSION_CHUNKID:
			iload->Read (&version, sizeof(selLevel), &nb);
			break;

		case FLAGS_CHUNKID:
			iload->Read (&flags, sizeof(DWORD), &nb);
			break;

		case NAMEDVSEL_NAMES_CHUNK:
			res = LoadNamedSelChunk(iload,0);
			break;

		case NAMEDESEL_NAMES_CHUNK:
			res = LoadNamedSelChunk(iload,1);
			break;

		case NAMEDFSEL_NAMES_CHUNK:
			res = LoadNamedSelChunk(iload,2);
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	if (version<3) {
		if ((selLevel>1) && (selLevel<4)) selLevel = 5-selLevel;	// switched faces, edges in 3.0
	}
	return IO_OK;
}

IOResult MeshSelMod::SaveLocalData(ISave *isave, LocalModData *ld) {	
	MeshSelData *d = (MeshSelData*)ld;

	isave->BeginChunk(VERTSEL_CHUNKID);
	d->vertSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(FACESEL_CHUNKID);
	d->faceSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(EDGESEL_CHUNKID);
	d->edgeSel.Save(isave);
	isave->EndChunk();
	
	if (d->vselSet.Count()) {
		isave->BeginChunk(VSELSET_CHUNK);
		d->vselSet.Save(isave);
		isave->EndChunk();
		}
	if (d->eselSet.Count()) {
		isave->BeginChunk(ESELSET_CHUNK);
		d->eselSet.Save(isave);
		isave->EndChunk();
		}
	if (d->fselSet.Count()) {
		isave->BeginChunk(FSELSET_CHUNK);
		d->fselSet.Save(isave);
		isave->EndChunk();
		}

	return IO_OK;
	}

IOResult MeshSelMod::LoadLocalData(ILoad *iload, LocalModData **pld) {
	MeshSelData *d = new MeshSelData;
	*pld = d;
	IOResult res;	
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VERTSEL_CHUNKID:
				d->vertSel.Load(iload);
				break;
			case FACESEL_CHUNKID:
				d->faceSel.Load(iload);
				break;
			case EDGESEL_CHUNKID:
				d->edgeSel.Load(iload);
				break;

			case VSELSET_CHUNK:
				res = d->vselSet.Load(iload);
				break;
			case FSELSET_CHUNK:
				res = d->fselSet.Load(iload);
				break;
			case ESELSET_CHUNK:
				res = d->eselSet.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}



// Window Procs ------------------------------------------------------

void MeshSelMod::SetEnableStates() {
	ICustButton *but;
	ISpinnerControl *spin;
	EnableWindow (GetDlgItem (hParams, IDC_MS_SEL_BYVERT), selLevel && (selLevel != SEL_VERTEX));
	EnableWindow (GetDlgItem (hParams, IDC_MS_IGNORE_BACKFACES), selLevel>SEL_VERTEX);
	EnableWindow (GetDlgItem (hParams, IDC_MS_IGNORE_VISEDGE), selLevel == SEL_POLY);
	EnableWindow (GetDlgItem (hParams, IDC_MS_PLANAR_TEXT), selLevel == SEL_POLY);
	spin = GetISpinner (GetDlgItem (hParams, IDC_MS_PLANARSPINNER));
	spin->Enable (selLevel==SEL_POLY);
	ReleaseISpinner (spin);

	but = GetICustButton (GetDlgItem (hParams, IDC_MS_GETVERT));
	but->Enable (selLevel && (selLevel != SEL_VERTEX));
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hParams, IDC_MS_GETEDGE));
	but->Enable (selLevel && (selLevel != SEL_EDGE));
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hParams, IDC_MS_GETFACE));
	but->Enable (selLevel && (selLevel < SEL_FACE));
	ReleaseICustButton (but);

	EnableWindow (GetDlgItem (hParams, IDC_MS_SELBYMAT_BOX), selLevel >= SEL_FACE);
	EnableWindow (GetDlgItem (hParams, IDC_MS_SELBYMAT_TEXT), selLevel >= SEL_FACE);
	but = GetICustButton (GetDlgItem (hParams, IDC_MS_SELBYMAT));
	but->Enable (selLevel >= SEL_FACE);
	ReleaseICustButton (but);
	spin = GetISpinner (GetDlgItem (hParams, IDC_MS_MATIDSPIN));
	spin->Enable (selLevel >= SEL_FACE);
	ReleaseISpinner (spin);

	but = GetICustButton (GetDlgItem (hParams, IDC_MS_COPYNS));
	but->Enable (selLevel);
	ReleaseICustButton(but);
	but = GetICustButton (GetDlgItem (hParams,IDC_MS_PASTENS));
	but->Enable (selLevel && (GetMeshNamedSelClip (namedClipLevel[selLevel]) ? TRUE : FALSE));
	ReleaseICustButton(but);
	but = GetICustButton (GetDlgItem (hParams, IDC_MS_SELOPEN));
	but->Enable (selLevel==SEL_EDGE);
	ReleaseICustButton(but);
}

static MeshSelImageHandler theMeshSelImageHandler;

static BOOL CALLBACK MeshSelectProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {	
	MeshSelMod *mod = (MeshSelMod*)GetWindowLong(hWnd,GWL_USERDATA);
	ICustToolbar *iToolbar;
	ISpinnerControl *spin;

	switch (msg) {
	case WM_INITDIALOG:
		mod = (MeshSelMod*)lParam;
		mod->hParams = hWnd;
		SetWindowLong(hWnd,GWL_USERDATA,lParam);

		iToolbar = GetICustToolbar(GetDlgItem(hWnd,IDC_MS_SELTYPE));
		iToolbar->SetImage (theMeshSelImageHandler.LoadImages());
		iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,0,5,0,5,24,23,24,23,IDC_SELVERTEX));
		iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,1,6,1,6,24,23,24,23,IDC_SELEDGE));
		iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,2,7,2,7,24,23,24,23,IDC_SELFACE));
		iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,3,8,3,8,24,23,24,23,IDC_SELPOLY));
		iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,4,9,4,9,24,23,24,23,IDC_SELELEMENT));
		ReleaseICustToolbar(iToolbar);
		mod->UpdateSelLevelDisplay ();

		spin = GetISpinner(GetDlgItem(hWnd,IDC_MS_PLANARSPINNER));
		spin->SetLimits (0, 180, FALSE);
		spin->SetScale (0.1f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_MS_PLANAR), EDITTYPE_FLOAT);
		spin->SetValue(lastFaceThresh,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_MS_MATIDSPIN));
		spin->SetLimits(1, 65535, FALSE);
		spin->SetScale(0.1f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_MS_MATID), EDITTYPE_INT);
		spin->SetValue(lastMatID,FALSE);
		ReleaseISpinner(spin);

		CheckDlgButton(hWnd,IDC_MS_SEL_BYVERT,mod->selByVert);
		CheckDlgButton(hWnd,IDC_MS_IGNORE_BACKFACES,mod->ignoreBackfaces);
		CheckDlgButton(hWnd,IDC_MS_IGNORE_VISEDGE,mod->ignoreVisEdge);
		mod->SetEnableStates();
		break;

	case WM_UPDATE_CACHE:
		mod->UpdateCache((TimeValue)wParam);
 		break;

	case WM_DESTROY:
		spin = GetISpinner(GetDlgItem(hWnd,IDC_MS_PLANARSPINNER));
		lastFaceThresh = spin->GetFVal();
		ReleaseISpinner(spin);
		spin = GetISpinner(GetDlgItem(hWnd,IDC_MS_MATIDSPIN));
		lastMatID = spin->GetIVal();
		ReleaseISpinner(spin);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_MS_SELBYMAT:
			spin = GetISpinner(GetDlgItem(hWnd,IDC_MS_MATIDSPIN));
			mod->SelectByMatID(spin->GetIVal()-1);
			ReleaseISpinner(spin);
			break;

		case IDC_SELVERTEX:
			if (mod->selLevel == SEL_VERTEX) mod->ip->SetSubObjectLevel (SEL_OBJECT);
			else mod->ip->SetSubObjectLevel (SEL_VERTEX);
			break;

		case IDC_SELEDGE:
			if (mod->selLevel == SEL_EDGE) mod->ip->SetSubObjectLevel (SEL_OBJECT);
			else mod->ip->SetSubObjectLevel (SEL_EDGE);
			break;

		case IDC_SELFACE:
			if (mod->selLevel == SEL_FACE) mod->ip->SetSubObjectLevel (SEL_OBJECT);
			else mod->ip->SetSubObjectLevel (SEL_FACE);
			break;

		case IDC_SELPOLY:
			if (mod->selLevel == SEL_POLY) mod->ip->SetSubObjectLevel (SEL_OBJECT);
			else mod->ip->SetSubObjectLevel (SEL_POLY);
			break;

		case IDC_SELELEMENT:
			if (mod->selLevel == SEL_ELEMENT) mod->ip->SetSubObjectLevel (SEL_OBJECT);
			else mod->ip->SetSubObjectLevel (SEL_ELEMENT);
			break;

		case IDC_MS_SEL_BYVERT:
			mod->selByVert = IsDlgButtonChecked(hWnd,IDC_MS_SEL_BYVERT);
			break;

		case IDC_MS_IGNORE_BACKFACES:
			mod->ignoreBackfaces = IsDlgButtonChecked(hWnd,IDC_MS_IGNORE_BACKFACES);
			break;

		case IDC_MS_IGNORE_VISEDGE:
			mod->ignoreVisEdge = IsDlgButtonChecked(hWnd,IDC_MS_IGNORE_VISEDGE);
			break;

		case IDC_MS_SELOPEN: mod->SelectOpenEdges(); break;

		case IDC_MS_GETVERT: mod->SelectFrom(SEL_VERTEX); break;
		case IDC_MS_GETEDGE: mod->SelectFrom(SEL_EDGE); break;
		case IDC_MS_GETFACE: mod->SelectFrom(SEL_FACE); break;

		case IDC_MS_COPYNS:  mod->NSCopy();  break;
		case IDC_MS_PASTENS: mod->NSPaste(); break;
		}
		break;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code != TTN_NEEDTEXT) break;
		LPTOOLTIPTEXT lpttt;
		lpttt = (LPTOOLTIPTEXT)lParam;				
		switch (lpttt->hdr.idFrom) {
		case IDC_SELVERTEX:
			lpttt->lpszText = GetString (IDS_RB_VERTEX);
			break;
		case IDC_SELEDGE:
			lpttt->lpszText = GetString (IDS_RB_EDGE);
			break;
		case IDC_SELFACE:
			lpttt->lpszText = GetString(IDS_RB_FACE);
			break;
		case IDC_SELPOLY:
			lpttt->lpszText = GetString(IDS_EM_POLY);
			break;
		case IDC_SELELEMENT:
			lpttt->lpszText = GetString(IDS_EM_ELEMENT);
			break;
		}
		break;
	
	default: return FALSE;
	}
	return TRUE;
}

#define GRAPHSTEPS 20

static void DrawCurve (HWND hWnd,HDC hdc) {
	float pinch, falloff, bubble;
	ISpinnerControl *spin = GetISpinner(GetDlgItem(hWnd,IDC_FALLOFFSPIN));
	falloff = spin->GetFVal();
	ReleaseISpinner(spin);	

	spin = GetISpinner(GetDlgItem(hWnd,IDC_PINCHSPIN));
	pinch = spin->GetFVal();
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hWnd,IDC_BUBBLESPIN));
	bubble = spin->GetFVal();
	ReleaseISpinner(spin);	

	TSTR label = FormatUniverseValue(falloff);
	SetWindowText(GetDlgItem(hWnd,IDC_FARLEFTLABEL),label);
	SetWindowText(GetDlgItem(hWnd,IDC_FARRIGHTLABEL),label);

	Rect rect, orect;
	GetClientRectP(GetDlgItem(hWnd,IDC_AR_GRAPH),&rect);
	orect = rect;

	SelectObject(hdc,GetStockObject(NULL_PEN));
	SelectObject(hdc,GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);	
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
	
	rect.left   += 3;
	rect.right  -= 3;
	rect.top    += 20;
	rect.bottom -= 20;
	
	SelectObject(hdc,CreatePen(PS_DOT,0,GetSysColor(COLOR_BTNFACE)));
	MoveToEx(hdc,orect.left,rect.top,NULL);
	LineTo(hdc,orect.right,rect.top);
	MoveToEx(hdc,orect.left,rect.bottom,NULL);
	LineTo(hdc,orect.right,rect.bottom);
	MoveToEx(hdc,(rect.left+rect.right)/2,orect.top,NULL);
	LineTo(hdc,(rect.left+rect.right)/2,orect.bottom);
	DeleteObject(SelectObject(hdc,GetStockObject(BLACK_PEN)));
	
	MoveToEx(hdc,rect.left,rect.bottom,NULL);
	for (int i=0; i<=GRAPHSTEPS; i++) {
		float dist = falloff * float(abs(i-GRAPHSTEPS/2))/float(GRAPHSTEPS/2);		
		float y = AffectRegionFunct(dist,falloff,pinch,bubble);
		int ix = rect.left + int(float(rect.w()-1) * float(i)/float(GRAPHSTEPS));
		int	iy = rect.bottom - int(y*float(rect.h()-2)) - 1;
		if (iy<orect.top) iy = orect.top;
		if (iy>orect.bottom-1) iy = orect.bottom-1;
		LineTo(hdc, ix, iy);
	}
	
	WhiteRect3D(hdc,orect,TRUE);
}

BOOL AffectRegionProc::DlgProc (TimeValue t, IParamMap *map,
										HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (!em) return FALSE;
	Rect rect;
	TSTR zero;
	ISpinnerControl *spin;

	switch (msg) {
	case WM_INITDIALOG:
		spin = GetISpinner (GetDlgItem(hWnd,IDC_FALLOFFSPIN));
		spin->SetResetValue (DEF_FALLOFF);
		ReleaseISpinner (spin);
		zero = FormatUniverseValue(0.0f);
		SetWindowText(GetDlgItem(hWnd,IDC_NEARLABEL),zero);
		ShowWindow(GetDlgItem(hWnd,IDC_AR_GRAPH),SW_HIDE);
		break;
		
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd,&ps);
		DrawCurve(hWnd,hdc);
		EndPaint(hWnd,&ps);
		return FALSE;
		}

	case CC_SPINNER_CHANGE:
		GetClientRectP(GetDlgItem(hWnd,IDC_AR_GRAPH),&rect);
		InvalidateRect(hWnd,&rect,FALSE);
		return FALSE;
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK PickSetNameDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

BOOL MeshSelMod::GetUniqueSetName(TSTR &name) {
	while (1) {				
		Tab<TSTR*> &setList = namedSel[namedSetLevel[selLevel]];

		BOOL unique = TRUE;
		for (int i=0; i<setList.Count(); i++) {
			if (name==*setList[i]) {
				unique = FALSE;
				break;
			}
		}
		if (unique) break;

		if (!ip) return FALSE;
		if (!DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_PASTE_NAMEDSET),
			ip->GetMAXHWnd(), PickSetNameDlgProc, (LPARAM)&name)) return FALSE;
		if (!ip) return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK PickSetDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		case IDC_NS_LIST:
			if (HIWORD(wParam)!=LBN_DBLCLK) break;
			// fall through

		case IDOK:
			int sel;
			sel = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETCURSEL,0,0);
			if (sel!=LB_ERR) {
				int res =SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETITEMDATA,sel,0);
				EndDialog(hWnd,res);
				break;
			}
			// fall through

		case IDCANCEL:
			EndDialog(hWnd,-1);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

// MeshSelData -----------------------------------------------------

LocalModData *MeshSelData::Clone() {
	MeshSelData *d = new MeshSelData;
	d->vertSel = vertSel;
	d->faceSel = faceSel;
	d->edgeSel = edgeSel;
	held = FALSE;
	mesh = NULL;
	return d;
	}

MeshSelData::MeshSelData(Mesh &mesh) {
	vertSel = mesh.vertSel;
	faceSel = mesh.faceSel;
	edgeSel = mesh.edgeSel;
	held = FALSE;
	this->mesh = NULL;
	ae = NULL;
	vdValid = NEVER;
}

void MeshSelData::SynchBitArrays()
{
	if (mesh) {
		vertSel.SetSize(mesh->vertSel.GetSize(),TRUE);
		faceSel.SetSize(mesh->faceSel.GetSize(),TRUE);
		edgeSel.SetSize(mesh->edgeSel.GetSize(),TRUE);
	}
}

AdjEdgeList *MeshSelData::GetAdjEdgeList () {
	if (!mesh) return NULL;
	if (!ae) ae = new AdjEdgeList (*mesh);
	return ae;
}

void MeshSelData::SetCache(Mesh &mesh) {
	if (this->mesh) delete this->mesh;
	if (ae) delete ae;
	ae = NULL;
	this->mesh = new Mesh(mesh);
	SynchBitArrays ();
}

void MeshSelData::FreeCache() {
	if (mesh) delete mesh;
	mesh = NULL;
	if (ae) delete ae;
	ae = NULL;
	InvalidateVDistances ();
}

void MeshSelData::SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
	MeshSelMod *mod = (MeshSelMod *) imod;
	if (theHold.Holding()) theHold.Put (new MeshSelRestore (mod, this, SEL_VERTEX));
	vertSel = set;
	if (mesh) mesh->vertSel = set;
	InvalidateVDistances ();
}

void MeshSelData::SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
	MeshSelMod *mod = (MeshSelMod *) imod;
	if (theHold.Holding()) theHold.Put (new MeshSelRestore (mod, this, SEL_FACE));
	faceSel = set;
	if (mesh) mesh->faceSel = set;
}

void MeshSelData::SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
	MeshSelMod *mod = (MeshSelMod *) imod;
	if (theHold.Holding()) theHold.Put (new MeshSelRestore (mod, this, SEL_EDGE));
	edgeSel = set;
	if (mesh) mesh->edgeSel = set;
}

BitArray MeshSelData::SelVertByFace() {
	BitArray nsel = vertSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {
		if (faceSel[i]) {
			for (int j=0; j<3; j++) {
				nsel.Set (mesh->faces[i].v[j]);
			}
		}
	}
	return nsel;
}

BitArray MeshSelData::SelVertByEdge() {
	BitArray nsel = vertSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {		
		for (int j=0; j<3; j++) {
			if (edgeSel[i*3+j]) {
				nsel.Set(mesh->faces[i].v[j],TRUE);
				nsel.Set(mesh->faces[i].v[(j+1)%3],TRUE);
			}
		}
	}
	return nsel;
}

BitArray MeshSelData::SelFaceByVert() {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) {
			if (vertSel[mesh->faces[i].v[j]]) {
				nsel.Set(i);
				break;
			}
		}
	}
	return nsel;
}

BitArray MeshSelData::SelFaceByEdge() {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) {
			if (edgeSel[i*3+j]) {
				nsel.Set(i);
				break;
			}
		}
	}
	return nsel;
}

BitArray MeshSelData::SelPolygonByVert (float thresh, int igVis) {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	AdjEdgeList al(*mesh);
	AdjFaceList af(*mesh, al);
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) if (vertSel[mesh->faces[i].v[j]]) break;
		if (j==3) continue;
		mesh->PolyFromFace (i, nsel, thresh, igVis, &af);
	}
	return nsel;
}

BitArray MeshSelData::SelPolygonByEdge (float thresh, int igVis) {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	AdjEdgeList al(*mesh);
	AdjFaceList af(*mesh, al);
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) if (edgeSel[i*3+j]) break;
		if (j==3) continue;
		mesh->PolyFromFace (i, nsel, thresh, igVis, &af);
	}
	return nsel;
}

BitArray MeshSelData::SelElementByVert () {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	AdjEdgeList al(*mesh);
	AdjFaceList af(*mesh, al);
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) if (vertSel[mesh->faces[i].v[j]]) break;
		if (j==3) continue;
		mesh->ElementFromFace (i, nsel, &af);
	}
	return nsel;
}

BitArray MeshSelData::SelElementByEdge () {
	BitArray nsel = faceSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	AdjEdgeList al(*mesh);
	AdjFaceList af(*mesh, al);
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) if (edgeSel[i*3+j]) break;
		if (j==3) continue;
		mesh->ElementFromFace (i, nsel, &af);
	}
	return nsel;
}

BitArray MeshSelData::SelEdgeByVert() {
	BitArray nsel = edgeSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {
		for (int j=0; j<3; j++) {
			if (vertSel[mesh->faces[i].v[j]]) nsel.Set(i*3+j);
			if (vertSel[mesh->faces[i].v[(j+1)%3]]) nsel.Set(i*3+j);
		}
	}
	return nsel;
}

BitArray MeshSelData::SelEdgeByFace() {
	BitArray nsel = edgeSel;
	DbgAssert (mesh);
	if (!mesh) return nsel;
	for (int i=0; i<mesh->getNumFaces(); i++) {
		if (faceSel[i]) {
			for (int j=0; j<3; j++) nsel.Set(i*3+j);
		}
	}
	return nsel;
}


// MeshSelRestore --------------------------------------------------

MeshSelRestore::MeshSelRestore(MeshSelMod *m, MeshSelData *data) {
	mod     = m;
	level   = mod->selLevel;
	d       = data;
	d->held = TRUE;
	switch (level) {
	case SEL_OBJECT: MaxAssert(0); break;
	case SEL_VERTEX: usel = d->vertSel; break;
	case SEL_EDGE: usel = d->edgeSel; break;
	default: usel = d->faceSel; break;
	}
}

MeshSelRestore::MeshSelRestore(MeshSelMod *m, MeshSelData *data, int sLevel) {
	mod     = m;
	level   = sLevel;
	d       = data;
	d->held = TRUE;
	switch (level) {
	case SEL_OBJECT: MaxAssert(0); break;
	case SEL_VERTEX: usel = d->vertSel; break;
	case SEL_EDGE: usel = d->edgeSel; break;
	default: usel = d->faceSel; break;
	}
}

void MeshSelRestore::Restore(int isUndo) {
	if (isUndo) {
		switch (level) {			
		case SEL_VERTEX: rsel = d->vertSel; break;
		case SEL_FACE: 
		case SEL_POLY:
		case SEL_ELEMENT:
			rsel = d->faceSel; break;
		case SEL_EDGE:   rsel = d->edgeSel; break;
		}
	}
	switch (level) {		
	case SEL_VERTEX: d->vertSel = usel; break;
	case SEL_FACE:
	case SEL_POLY:
	case SEL_ELEMENT:
		d->faceSel = usel; break;
	case SEL_EDGE:   d->edgeSel = usel; break;
	}
	mod->InvalidateVDistances ();
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	mod->SetNumSelLabel();
	mod->UpdateNamedSelDropDown ();
}

void MeshSelRestore::Redo() {
	switch (level) {		
	case SEL_VERTEX:
		d->vertSel = rsel; break;
	case SEL_FACE:
	case SEL_POLY:
	case SEL_ELEMENT:
		d->faceSel = rsel; break;
	case SEL_EDGE:
		d->edgeSel = rsel; break;
	}
	mod->InvalidateVDistances ();
	mod->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	mod->SetNumSelLabel();
	mod->UpdateNamedSelDropDown ();
	}


//--- Named selection sets -----------------------------------------

int MeshSelMod::FindSet(TSTR &setName, int level) {
	for (int i=0; i<namedSel[level].Count(); i++) {
		if (setName == *namedSel[level][i]) return i;
	}
	return -1;
}

DWORD MeshSelMod::AddSet(TSTR &setName,int level) {
	DWORD id = 0;
	TSTR *name = new TSTR(setName);
	namedSel[level].Append(1,&name);
	BOOL found = FALSE;
	while (!found) {
		found = TRUE;
		for (int i=0; i<ids[level].Count(); i++) {
			if (ids[level][i]!=id) continue;
			id++;
			found = FALSE;
			break;
		}
	}
	ids[level].Append(1,&id);
	return id;
}

void MeshSelMod::RemoveSet(TSTR &setName,int level) {
	int i = FindSet(setName,level);
	if (i<0) return;
	delete namedSel[level][i];
	namedSel[level].Delete(i,1);
	ids[level].Delete(i,1);
}

void MeshSelMod::UpdateSetNames () {
	if (!ip) return;
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	for (int i=0; i<mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if ( !meshData ) continue;
		for (int nsl=0; nsl<3; nsl++) {
			// Make sure the namedSel array is in alpha order.
			// (Crude bubble sort since we expect that it will be.)
			int j, k, kmax = namedSel[nsl].Count();
			for (k=1; k<kmax; k++) {
				if (*(namedSel[nsl][k-1]) < *(namedSel[nsl][k])) continue;
				for (j=0; j<k-1; j++) {
					if (*(namedSel[nsl][j]) > *(namedSel[nsl][k])) break;
				}
				// j now represents the point at which k should be inserted.
				TSTR *hold = namedSel[nsl][k];
				DWORD dhold = ids[nsl][k];
				int j2;
				for (j2=k; j2>j; j2--) {
					namedSel[nsl][j2] = namedSel[nsl][j2-1];
					ids[nsl][j2] = ids[nsl][j2-1];
				}
				namedSel[nsl][j] = hold;
				ids[nsl][j] = dhold;
			}

			GenericNamedSelSetList & gnsl = meshData->GetNamedSel(nsl);
			// Check for old, unnamed sets with ids.
			for (k=0; k<gnsl.Count(); k++) {
				if (gnsl.names[k]) continue;
				for (j=0; j<ids[nsl].Count(); j++) if (ids[nsl][j] == gnsl.ids[k]) break;
				if (j == ids[nsl].Count()) continue;
				gnsl.names[j] = new TSTR(*(namedSel[nsl][j]));
			}
			gnsl.Alphabetize ();

			// Now check lists against each other, adding any missing elements.
			for (j=0; j<gnsl.Count(); j++) {
				if (*(gnsl.names[j]) == *(namedSel[nsl][j])) continue;
				if (j>= namedSel[nsl].Count()) {
					TSTR *nname = new TSTR(*gnsl.names[j]);
					DWORD nid = gnsl.ids[j];
					namedSel[nsl].Append (1, &nname);
					ids[nsl].Append (1, &nid);
					continue;
				}
				if (*(gnsl.names[j]) > *(namedSel[nsl][j])) {
					BitArray baTemp;
					gnsl.InsertSet (j, baTemp, ids[nsl][j], *(namedSel[nsl][j]));
					continue;
				}
				// Otherwise:
				TSTR *nname = new TSTR(*gnsl.names[j]);
				DWORD nid = gnsl.ids[j];
				namedSel[nsl].Insert (j, 1, &nname);
				ids[nsl].Insert (j, 1, &nid);
			}
			for (; j<namedSel[nsl].Count(); j++) {
				BitArray baTemp;
				gnsl.AppendSet (baTemp, ids[nsl][j], *(namedSel[nsl][j]));
			}
		}
	}

	nodes.DisposeTemporary();
}

void MeshSelMod::ClearSetNames() {
	for (int i=0; i<3; i++) {
		for (int j=0; j<namedSel[i].Count(); j++) {
			delete namedSel[i][j];
			namedSel[i][j] = NULL;
		}
	}
}

void MeshSelMod::ActivateSubSelSet(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	int nsl = namedSetLevel[selLevel];
	int index = FindSet (setName, nsl);	
	if (index<0 || !ip) return;

	theHold.Begin ();
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;
		if (theHold.Holding() && !meshData->held) theHold.Put(new MeshSelRestore(this,meshData));

		BitArray *set = NULL;

		switch (nsl) {
		case NS_VERTEX:
			set = meshData->vselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->vertSel.GetSize()) {
					set->SetSize(meshData->vertSel.GetSize(),TRUE);
				}
				meshData->SetVertSel (*set, this, ip->GetTime());
			}
			break;

		case NS_FACE:
			set = meshData->fselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->faceSel.GetSize()) {
					set->SetSize(meshData->faceSel.GetSize(),TRUE);
				}
				meshData->SetFaceSel (*set, this, ip->GetTime());
			}
			break;

		case NS_EDGE:
			set = meshData->eselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->edgeSel.GetSize()) {
					set->SetSize(meshData->edgeSel.GetSize(),TRUE);
				}
				meshData->SetEdgeSel (*set, this, ip->GetTime());
			}
			break;
		}
	}
	
	nodes.DisposeTemporary();
	LocalDataChanged ();
	theHold.Accept (GetString (IDS_DS_SELECT));
	ip->RedrawViews(ip->GetTime());
}

void MeshSelMod::NewSetFromCurSel(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	DWORD id = -1;
	int nsl = namedSetLevel[selLevel];
	int index = FindSet(setName, nsl);
	if (index<0) id = AddSet(setName, nsl);
	else id = ids[nsl][index];

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;
		
		BitArray *set = NULL;

		switch (nsl) {
		case NS_VERTEX:	
			if (index>=0 && (set = meshData->vselSet.GetSet(id))) {
				*set = meshData->vertSel;
			} else meshData->vselSet.InsertSet(meshData->vertSel,id, setName);
			break;

		case NS_FACE:
			if (index>=0 && (set = meshData->fselSet.GetSet(id))) {
				*set = meshData->faceSel;
			} else meshData->fselSet.InsertSet(meshData->faceSel,id, setName);
			break;

		case NS_EDGE:
			if (index>=0 && (set = meshData->eselSet.GetSet(id))) {
				*set = meshData->edgeSel;
			} else meshData->eselSet.InsertSet(meshData->edgeSel,id, setName);
			break;
		}
	}	
	nodes.DisposeTemporary();
}

void MeshSelMod::RemoveSubSelSet(TSTR &setName) {
	int nsl = namedSetLevel[selLevel];
	int index = FindSet (setName, nsl);
	if (index<0 || !ip) return;		

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	DWORD id = ids[nsl][index];

	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;		

		switch (nsl) {
			case NS_VERTEX:	
				if (theHold.Holding()) theHold.Put(new DeleteSetRestore(&meshData->vselSet,id,setName));
				meshData->vselSet.RemoveSet(id);
				break;

			case NS_FACE:
				if (theHold.Holding()) theHold.Put(new DeleteSetRestore(&meshData->fselSet,id,setName));
				meshData->fselSet.RemoveSet(id);
				break;

			case NS_EDGE:
				if (theHold.Holding()) theHold.Put(new DeleteSetRestore(&meshData->eselSet,id,setName));
				meshData->eselSet.RemoveSet(id);
				break;
			}		
		}
	
	if (theHold.Holding()) theHold.Put(new DeleteSetNameRestore(&(namedSel[nsl]),this,&(ids[nsl]),id));
	RemoveSet (setName, nsl);
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
}

void MeshSelMod::SetupNamedSelDropDown() {
	if (selLevel == SEL_OBJECT) return;
	ip->ClearSubObjectNamedSelSets();
	int nsl = namedSetLevel[selLevel];
	for (int i=0; i<namedSel[nsl].Count(); i++)
		ip->AppendSubObjectNamedSelSet(*namedSel[nsl][i]);
}

int MeshSelMod::NumNamedSelSets() {
	int nsl = namedSetLevel[selLevel];
	return namedSel[nsl].Count();
}

TSTR MeshSelMod::GetNamedSelSetName(int i) {
	int nsl = namedSetLevel[selLevel];
	return *namedSel[nsl][i];
}

void MeshSelMod::SetNamedSelSetName(int i,TSTR &newName) {
	int nsl = namedSetLevel[selLevel];
	if (theHold.Holding()) theHold.Put(new SetNameRestore(&namedSel[nsl],this,&ids[nsl],ids[nsl][i]));
	*namedSel[nsl][i] = newName;
}

void MeshSelMod::NewSetByOperator(TSTR &newName,Tab<int> &sets,int op) {
	ModContextList mcList;
	INodeTab nodes;
	
	int nsl = namedSetLevel[selLevel];
	DWORD id = AddSet(newName, nsl);
	if (theHold.Holding()) theHold.Put(new AppendSetNameRestore(this,&namedSel[nsl],&ids[nsl]));

	BOOL delSet = TRUE;
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;
	
		BitArray bits;
		GenericNamedSelSetList *setList;

		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;
		case NS_FACE: setList = &meshData->fselSet; break;			
		case NS_EDGE:   setList = &meshData->eselSet; break;			
		}		

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

		if (!delSet) setList->InsertSet (bits, id, newName);
		if (theHold.Holding()) theHold.Put(new AppendSetRestore(setList, id, newName));
	}
	if (delSet) RemoveSubSelSet(newName);
}

void MeshSelMod::NSCopy() {
	int index = SelectNamedSet();
	if (index<0) return;
	if (!ip) return;

	int nsl = namedSetLevel[selLevel];
	MeshNamedSelClip *clip = new MeshNamedSelClip(*namedSel[nsl][index]);

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;

		GenericNamedSelSetList *setList;
		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;				
		case NS_FACE: setList = &meshData->fselSet; break;			
		case NS_EDGE: setList = &meshData->eselSet; break;			
		}		

		BitArray *bits = new BitArray(*setList->sets[index]);
		clip->sets.Append(1,&bits);
	}
	SetMeshNamedSelClip(clip, namedClipLevel[selLevel]);
	
	// Enable the paste button
	ICustButton *but;
	but = GetICustButton(GetDlgItem(hParams,IDC_MS_PASTENS));
	but->Enable();
	ReleaseICustButton(but);
}

void MeshSelMod::NSPaste() {
	int nsl = namedSetLevel[selLevel];
	MeshNamedSelClip *clip = GetMeshNamedSelClip(namedClipLevel[selLevel]);
	if (!clip) return;	
	TSTR name = clip->name;
	if (!GetUniqueSetName(name)) return;

	ModContextList mcList;
	INodeTab nodes;
	theHold.Begin();

	DWORD id = AddSet (name, nsl);	
	if (theHold.Holding()) theHold.Put(new AppendSetNameRestore(this, &namedSel[nsl], &ids[nsl]));	

	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;

		GenericNamedSelSetList *setList;
		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;
		case NS_EDGE: setList = &meshData->eselSet; break;
		case NS_FACE: setList = &meshData->fselSet; break;
		}
				
		if (i>=clip->sets.Count()) {
			BitArray bits;
			setList->InsertSet(bits,id,name);
		} else setList->InsertSet(*clip->sets[i],id,name);
		if (theHold.Holding()) theHold.Put (new AppendSetRestore (setList, id, name));		
	}	
	
	ActivateSubSelSet(name);
	ip->SetCurNamedSelSet(name);
	theHold.Accept(GetString (IDS_TH_PASTE_NAMED_SEL));
	SetupNamedSelDropDown();
}

int MeshSelMod::SelectNamedSet() {
	Tab<TSTR*> &setList = namedSel[namedSetLevel[selLevel]];
	if (!ip) return FALSE;
	return DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_SEL_NAMEDSET),
		ip->GetMAXHWnd(), PickSetDlgProc, (LPARAM)&setList);
}

void MeshSelMod::SetNumSelLabel() {	
	TSTR buf;
	int num = 0, which;

	if (!hParams) return;

	ModContextList mcList;
	INodeTab nodes;

	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		MeshSelData *meshData = (MeshSelData*)mcList[i]->localData;
		if (!meshData) continue;

		switch (selLevel) {
		case SEL_VERTEX:
			num += meshData->vertSel.NumberSet();
			if (meshData->vertSel.NumberSet() == 1) {
				for (which=0; which<meshData->vertSel.GetSize(); which++) if (meshData->vertSel[which]) break;
			}
			break;
		case SEL_FACE:
		case SEL_POLY:
		case SEL_ELEMENT:
			num += meshData->faceSel.NumberSet();
			if (meshData->faceSel.NumberSet() == 1) {
				for (which=0; which<meshData->faceSel.GetSize(); which++) if (meshData->faceSel[which]) break;
			}
			break;
		case SEL_EDGE:
			num += meshData->edgeSel.NumberSet();
			if (meshData->edgeSel.NumberSet() == 1) {
				for (which=0; which<meshData->edgeSel.GetSize(); which++) if (meshData->edgeSel[which]) break;
			}
			break;
		}
	}

	switch (selLevel) {
	case SEL_VERTEX:			
		if (num==1) buf.printf (GetString(IDS_EM_WHICHVERTSEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMVERTSELP),num);
		break;

	case SEL_FACE:
	case SEL_POLY:
	case SEL_ELEMENT:
		if (num==1) buf.printf (GetString(IDS_EM_WHICHFACESEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMFACESELP),num);
		break;

	case SEL_EDGE:
		if (num==1) buf.printf (GetString(IDS_EM_WHICHEDGESEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMEDGESELP),num);
		break;

	case SEL_OBJECT:
		buf = GetString (IDS_EM_OBJECT_SEL);
		break;
	}

	SetDlgItemText(hParams,IDC_MS_NUMBER_SEL,buf);
}

RefResult MeshSelMod::NotifyRefChanged (Interval changeInt, RefTargetHandle hTarget, 
   		PartID& partID, RefMessage message) {
	GetParamDim *gpd;
	GetParamName *gpn;

	switch (message) {
	case REFMSG_CHANGE:
		if ((editMod==this) && pmapParam) pmapParam->Invalidate();
		break;

	case REFMSG_GET_PARAM_DIM:
		gpd = (GetParamDim*)partID;
		gpd->dim = GetParameterDim(gpd->index);
		return REF_STOP;

	case REFMSG_GET_PARAM_NAME:
		gpn = (GetParamName*)partID;
		gpn->name = GetParameterName(gpn->index);
		return REF_STOP;
	}
	return(REF_SUCCEED);
}

ParamDimension *MeshSelMod::GetParameterDim(int pbIndex) {
	switch (pbIndex) {
		case PB_USE_AR: 	return defaultDim;
		default: return stdWorldDim;
	}
}

TSTR MeshSelMod::GetParameterName(int pbIndex)
{
	switch (pbIndex) {
		case PB_USE_AR: return TSTR(GetString(IDS_MS_USE_AR));
		case PB_FALLOFF: return TSTR(GetString(IDS_AFR_FALLOFF));
		case PB_PINCH: return TSTR(GetString(IDS_AFR_PINCH));
		case PB_BUBBLE: return TSTR(GetString(IDS_AFR_BUBBLE));
		default:			return TSTR(_T(""));
	}
}


/**********************************************************************
 *<
	FILE: surfmod.cpp

	DESCRIPTION:  Varius surface modifiers

	CREATED BY: Rolf Berteig

	HISTORY: 11/07/95

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "mods.h"
#include "iparamm.h"
#include "MeshDLib.h"

class MatMod : public Modifier {	
	public:
		IParamBlock *pblock;
		static IParamMap *pmapParam;

		MatMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_MATMOD); }  
		virtual Class_ID ClassID() { return Class_ID(MATERIALOSM_CLASS_ID,0);}
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() { return GetString(IDS_RB_MATERIAL3); }
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		ChannelMask ChannelsUsed()  {return OBJ_CHANNELS;}
		ChannelMask ChannelsChanged() {return GEOM_CHANNEL|TOPO_CHANNEL;}
		Class_ID InputType() {return triObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);
		IParamArray *GetParamBlock() {return pblock;}
		int GetParamBlockIndex(int id) {return id;}

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock = (IParamBlock*)rtarg;}

		int NumSubs() {return 1;}
		Animatable* SubAnim(int i) {return pblock;}
		TSTR SubAnimName(int i) {return GetString(IDS_RB_PARAMETERS);}

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	};



//--- ClassDescriptor and class vars ---------------------------------

IParamMap *MatMod::pmapParam = NULL;



class MatClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new MatMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_MATERIAL3_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(MATERIALOSM_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFSURFACE);}
	};

static MatClassDesc matDesc;
extern ClassDesc* GetMatModDesc() { return &matDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_MATID 0

//
//
// Parameters

static ParamUIDesc descParam[] = {
	
	// Material ID
	ParamUIDesc(
		PB_MATID,
		EDITTYPE_INT,
		IDC_MATID,IDC_MATIDSPIN,
		1.0f,(float)0xffff,
		0.1f),	
	};
#define PARAMDESC_LENGH 1


static ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, TRUE, 0 }};
#define PBLOCK_LENGTH	1

#define CURRENT_VERSION	0


//--- MatMod methods -------------------------------


MatMod::MatMod()
	{	
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descVer0, PBLOCK_LENGTH, CURRENT_VERSION));	
	pblock->SetValue(PB_MATID,0,1);
	}

void MatMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
	{
	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_MATERIALPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);		
	}
		
void MatMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{	
	DestroyCPParamMap(pmapParam);
	pmapParam = NULL;
	}

Interval MatMod::LocalValidity(TimeValue t)
	{
	int i;
	Interval valid = FOREVER;
	pblock->GetValue(PB_MATID,t,i,valid);	
	return valid;
	}

RefTargetHandle MatMod::Clone(RemapDir& remap) 
	{
	MatMod* newmod = new MatMod();	
	newmod->ReplaceReference(0,pblock->Clone(remap));	
	return newmod;
	}

void MatMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	Interval valid = FOREVER;
	int id;
	pblock->GetValue(PB_MATID,t,id,valid);	
	id--;
	if (id<0) id = 0;
	if (id>0xffff) id = 0xffff;

	assert(os->obj->IsSubClassOf(triObjectClassID));
	TriObject *triOb = (TriObject *)os->obj;
	BOOL useSel = triOb->GetMesh().selLevel==MESH_FACE;

	for (int i=0; i<triOb->GetMesh().getNumFaces(); i++) {
		if (!useSel || triOb->GetMesh().faceSel[i]) {
			triOb->GetMesh().setFaceMtlIndex(i,(MtlID)id);
		}
	}
	triOb->GetMesh().InvalidateTopologyCache();
		
	triOb->UpdateValidity(TOPO_CHAN_NUM,valid);		
	}

RefResult MatMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
   		PartID& partID, 
   		RefMessage message) 
   	{
	switch (message) {
		case REFMSG_CHANGE:
			if (pmapParam && pmapParam->GetParamBlock()==pblock) {
				pmapParam->Invalidate();
				}
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {				
				case 0:
				default: gpd->dim = defaultDim; break;
				}			
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			switch (gpn->index) {				
				case PB_MATID:	gpn->name = GetString(IDS_RB_MATERIALID); break;
				default:		gpn->name = TSTR(_T("")); break;
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}

//-------------------------------------------------------------------
//-------------------------------------------------------------------


class SmoothMod : public Modifier {	
	public:
		IParamBlock *pblock;
		static IParamMap *pmapParam;

		SmoothMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_SMOOTHMOD); }  
		virtual Class_ID ClassID() { return Class_ID(SMOOTHOSM_CLASS_ID,0);}
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() { return GetString(IDS_RB_SMOOTH2); }
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		ChannelMask ChannelsUsed()  {return OBJ_CHANNELS;}
		ChannelMask ChannelsChanged() {return GEOM_CHANNEL|TOPO_CHANNEL;}
		Class_ID InputType() {return triObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);
		IParamArray *GetParamBlock() {return pblock;}
		int GetParamBlockIndex(int id) {return id;}

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock = (IParamBlock*)rtarg;}
		IOResult Load(ILoad *iload);

		int NumSubs() {return 1;}
		Animatable* SubAnim(int i) {return pblock;}
		TSTR SubAnimName(int i) {return GetString(IDS_RB_PARAMETERS);}

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	};



//--- ClassDescriptor and class vars ---------------------------------

IParamMap *SmoothMod::pmapParam = NULL;


class SmoothClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new SmoothMod;}
	const TCHAR *	ClassName() { return GetString(IDS_RB_SMOOTH2_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(SMOOTHOSM_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFSURFACE);}
	};

static SmoothClassDesc smoothDesc;
extern ClassDesc* GetSmoothModDesc() { return &smoothDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_AUTOSMOOTH 		0
#define PB_THRESHOLD		1
#define PB_SMOOTHBITS		2
#define PB_PREVENTINDIRECT	3

//
//
// Parameters

static ParamUIDesc descSmoothParam[] = {
	// Auto smooth
	ParamUIDesc(PB_AUTOSMOOTH,TYPE_SINGLECHEKBOX,IDC_SMOOTH_AUTO),

	// Prevent Indirect Smoothing
	ParamUIDesc(PB_PREVENTINDIRECT,TYPE_SINGLECHEKBOX,IDC_SMOOTH_PREVENTINDIRECT),

	// Threshold
	ParamUIDesc(
		PB_THRESHOLD,
		EDITTYPE_FLOAT,
		IDC_SMOOTH_THRESH,IDC_SMOOTH_THRESHSPIN,
		0.0f,180.0f,
		0.1f,
		stdAngleDim),
	};
#define SMOOTHPARAMDESC_LENGH 3


static ParamBlockDescID descSmoothVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, FALSE, 2 }};

static ParamBlockDescID descSmoothVer1[] = {
	{ TYPE_INT,   NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT,   NULL, FALSE, 2 },
	{ TYPE_INT,   NULL, FALSE, 3 },
	};

#define SMOOTHPBLOCK_LENGTH	4

// Array of old versions
static ParamVersionDesc versionsSmooth[] = {
	ParamVersionDesc(descSmoothVer0,3,0)
	};
#define NUM_OLDSMOOTHVERSIONS	1

#define CURRENT_SMOOTHVERSION	1
static ParamVersionDesc curSmoothVersion(descSmoothVer1,SMOOTHPBLOCK_LENGTH,CURRENT_SMOOTHVERSION);


//--- SmoothDlgProc --------------------------------

class SmoothDlgProc : public ParamMapUserDlgProc {
	public:
		HWND hWnd;
		SmoothMod *mod;		
		SmoothDlgProc(SmoothMod *m) {mod = m;hWnd = NULL;}		
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
		void Update(TimeValue t);

	};

static void SetSmoothButtonState(HWND hWnd,DWORD bits)
	{
	for (int i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++) {		
		SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_STATE,
			(bits&(1<<(i-IDC_SMOOTH_GRP1)))?TRUE:FALSE);			
		}
	}

void SmoothDlgProc::Update(TimeValue t)
	{	
	if (!mod || !mod->pblock || !hWnd) return;
	int bits;
	mod->pblock->GetValue(PB_SMOOTHBITS,t,bits,FOREVER);
	SetSmoothButtonState(hWnd,(DWORD)bits);
	}

BOOL SmoothDlgProc::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG: {
			this->hWnd = hWnd;
			for (int i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++) {
				SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
				}
			IParamBlock *pblock = (IParamBlock*)map->GetParamBlock();
			int bits;
			pblock->GetValue(PB_SMOOTHBITS,t,bits,FOREVER);
			SetSmoothButtonState(hWnd,(DWORD)bits);
			break;
			}

		case WM_COMMAND:
			if (LOWORD(wParam)>=IDC_SMOOTH_GRP1 &&
				LOWORD(wParam)<=IDC_SMOOTH_GRP32) {
				IParamBlock *pblock = (IParamBlock*)map->GetParamBlock();
				ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,LOWORD(wParam)));
				int bits;
				pblock->GetValue(PB_SMOOTHBITS,t,bits,FOREVER);
				int shift = LOWORD(wParam) - IDC_SMOOTH_GRP1;
				if (iBut->IsChecked()) {
					bits |= 1<<shift;
				} else {
					bits &= ~(1<<shift);
					}
				pblock->SetValue(PB_SMOOTHBITS,t,bits);
				ReleaseICustButton(iBut);
				return REDRAW_VIEWS;
				}
			break;
		
		default:
			return TRUE;
		}
	return FALSE;
	}


//--- SmoothMod methods -------------------------------


SmoothMod::SmoothMod()
	{	
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descSmoothVer1, SMOOTHPBLOCK_LENGTH, CURRENT_SMOOTHVERSION));
	pblock->SetValue(PB_THRESHOLD,0,DegToRad(30.0f));
	}

IOResult SmoothMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versionsSmooth,NUM_OLDSMOOTHVERSIONS,&curSmoothVersion,this,0));
	return IO_OK;
	}


void SmoothMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
	{
	pmapParam = CreateCPParamMap(
		descSmoothParam,SMOOTHPARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_SMOOTHPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);		
	pmapParam->SetUserDlgProc(new SmoothDlgProc(this));
	}
		
void SmoothMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{	
	DestroyCPParamMap(pmapParam);
	pmapParam = NULL;
	}

Interval SmoothMod::LocalValidity(TimeValue t)
	{
	float f;
	Interval valid = FOREVER;
	pblock->GetValue(PB_THRESHOLD,t,f,valid);	
	return valid;
	}

RefTargetHandle SmoothMod::Clone(RemapDir& remap) 
	{
	SmoothMod* newmod = new SmoothMod();	
	newmod->ReplaceReference(0,pblock->Clone(remap));	
	return newmod;
	}

#if 0 // moved into mesh class
static void AutoSmooth(
		Mesh *mesh,
		AdjFaceList &af,
		AdjEdgeList &ae,
		float angle,BOOL useSel)
	{
	FaceClusterList clust(mesh,af,angle,useSel);
	int *grp = new int[clust.count];
	
	// For each cluster, pick the first unused bit
	for (DWORD i=0; i<clust.count; i++) {
		int bit = 0;
		 DWORD used = 0;
		
		// Go through the faces in this cluster and see which
		// bits are used		
		for (int j=0; j<mesh->getNumFaces(); j++) {
			if (useSel&&!mesh->faceSel[j]) continue;

			if (clust[j]==i) {				
				int c;				
							
				for (int k=0; k<3; k++) {					 					
					// New way: We gotta look at all faces that share
					// a vertex even if they are not adjacent across an edge.
					DWORD v = mesh->faces[j].v[k];
					for (int l=0; l<ae[v].Count(); l++) {
						MEdge edge = ae.edges[ae[v][l]];
						if (edge.f[0]!=UNDEFINED && edge.f[0]!=(DWORD)j) {
							c = clust[edge.f[0]];
							if (c!=UNDEFINED && (DWORD)c<i) {
								used |= 1<<(grp[c]);
								}
							}
						if (edge.f[1]!=UNDEFINED && edge.f[1]!=(DWORD)j) {
							c = clust[edge.f[1]];
							if (c!=UNDEFINED && (DWORD)c<i) {
								used |= 1<<(grp[c]);
								}
							}
						}
					}				
				}			
			}

		// Pick the first free bit
		for (j=0; j<32; j++) {
			if (!((1<<j) & used)) {
				bit = j;
				break;
				}
			}

		// We've got a smooth bit for this cluster!
		grp[i] = bit;
		}
	
	// Now we've got bits for each cluster ID. Assign the faces.
	for (int j=0; j<mesh->getNumFaces(); j++) {
		if (useSel&&!mesh->faceSel[j]) continue;
		
		assert(clust[j]!=UNDEFINED);		
		mesh->faces[j].smGroup = (1<<grp[clust[j]]);		
		}
	
	delete[] grp;
	}
#endif


void SmoothMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	Interval valid = FOREVER;
	int autoSmooth, bits, prevent;
	float thresh;
	pblock->GetValue(PB_AUTOSMOOTH,t,autoSmooth,valid);	
	pblock->GetValue(PB_THRESHOLD,t,thresh,valid);	
	pblock->GetValue(PB_SMOOTHBITS,t,bits,valid);	
	pblock->GetValue(PB_PREVENTINDIRECT,t,prevent,valid);

	assert(os->obj->IsSubClassOf(triObjectClassID));
	TriObject *triOb = (TriObject *)os->obj;
	BOOL useSel = triOb->GetMesh().selLevel==MESH_FACE;

	if (!autoSmooth) {
		for (int i=0; i<triOb->GetMesh().getNumFaces(); i++) {
			if (!useSel || triOb->GetMesh().faceSel[i]) {			
				triOb->GetMesh().faces[i].smGroup = (DWORD)bits;
				}
			}
		valid = FOREVER; // No animating without autosmooth
	} else {
		//AdjEdgeList ae(triOb->mesh);
		//AdjFaceList af(triOb->mesh,ae);
		//AutoSmooth(&triOb->mesh,af,ae,thresh,useSel);
		triOb->GetMesh().AutoSmooth(thresh,useSel,prevent);
		}
	triOb->GetMesh().InvalidateTopologyCache ();
			
	triOb->UpdateValidity(TOPO_CHAN_NUM,valid);		
	}

RefResult SmoothMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
   		PartID& partID, 
   		RefMessage message) 
   	{
	switch (message) {
		case REFMSG_CHANGE:
			if (pmapParam && pmapParam->GetParamBlock()==pblock) {
				pmapParam->Invalidate();
				}
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {				
				case PB_THRESHOLD: gpd->dim = stdAngleDim; break;
				default: gpd->dim = defaultDim; break;
				}			
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			switch (gpn->index) {				
				case PB_THRESHOLD:	gpn->name = GetString(IDS_RB_THRESHOLD); break;
				default:		gpn->name = TSTR(_T("")); break;
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}


//-----------------------------------------------------------
//-----------------------------------------------------------


class NormalMod : public Modifier {	
	public:
		IParamBlock *pblock;
		static IParamMap *pmapParam;

		NormalMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_NORMALMOD); }  
		virtual Class_ID ClassID() { return Class_ID(NORMALOSM_CLASS_ID,0);}
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() { return GetString(IDS_RB_NORMAL); }
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		ChannelMask ChannelsUsed()  {return OBJ_CHANNELS;}
		ChannelMask ChannelsChanged() {return PART_TOPO|PART_TEXMAP|PART_VERTCOLOR;}
		Class_ID InputType() {return triObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);
		IParamArray *GetParamBlock() {return pblock;}
		int GetParamBlockIndex(int id) {return id;}

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock = (IParamBlock*)rtarg;}

		int NumSubs() {return 1;}
		Animatable* SubAnim(int i) {return pblock;}
		TSTR SubAnimName(int i) {return GetString(IDS_RB_PARAMETERS);}

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	};



//--- ClassDescriptor and class vars ---------------------------------

IParamMap *NormalMod::pmapParam = NULL;



class NormalClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new NormalMod;}
	const TCHAR *	ClassName() { return GetString(IDS_RB_NORMAL_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(NORMALOSM_CLASS_ID,0);}
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFSURFACE);}
	};

static NormalClassDesc normalDesc;
extern ClassDesc* GetNormalModDesc() { return &normalDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_UNIFY	0
#define PB_FLIP 	1

//
//
// Parameters

static ParamUIDesc descNormParam[] = {
	// Unify
	ParamUIDesc(PB_UNIFY,TYPE_SINGLECHEKBOX,IDC_NORM_UNIFY),

	// Flip
	ParamUIDesc(PB_FLIP,TYPE_SINGLECHEKBOX,IDC_NORM_FLIP),
	};
#define NORMPARAMDESC_LENGH 2


static ParamBlockDescID descNormVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_INT, NULL, FALSE, 1 }};
#define NORMPBLOCK_LENGTH	2

#define CURRENT_NORMVERSION	0


//--- NormalMod methods -------------------------------


NormalMod::NormalMod()
	{	
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descNormVer0, NORMPBLOCK_LENGTH, CURRENT_NORMVERSION));	
	}

void NormalMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
	{
	pmapParam = CreateCPParamMap(
		descNormParam,NORMPARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_NORMALPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);		
	}
		
void NormalMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{	
	DestroyCPParamMap(pmapParam);
	pmapParam = NULL;
	}

Interval NormalMod::LocalValidity(TimeValue t)
	{	
	return FOREVER;
	}

RefTargetHandle NormalMod::Clone(RemapDir& remap) 
	{
	NormalMod* newmod = new NormalMod();	
	newmod->ReplaceReference(0,pblock->Clone(remap));	
	return newmod;
	}

void FlipMeshNormal(Mesh *mesh,DWORD face) {
	DWORD vis  = 0;
	if (mesh->faces[face].flags&EDGE_A) vis |= EDGE_A;
	if (mesh->faces[face].flags&EDGE_B) vis |= EDGE_C;
	if (mesh->faces[face].flags&EDGE_C) vis |= EDGE_B;
	DWORD temp = mesh->faces[face].v[0];
	mesh->faces[face].v[0] = mesh->faces[face].v[1];
	mesh->faces[face].v[1] = temp;				
	mesh->faces[face].flags &= ~EDGE_ALL;
	mesh->faces[face].flags |= vis;	
	if (mesh->tvFace) {		
		temp = mesh->tvFace[face].t[0];
		mesh->tvFace[face].t[0] = mesh->tvFace[face].t[1];
		mesh->tvFace[face].t[1] = temp;		
	}
}

void NormalMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	Interval valid = FOREVER;
	int flip, unify;
	pblock->GetValue(PB_FLIP,t,flip,valid);	
	pblock->GetValue(PB_UNIFY,t,unify,valid);	

	assert(os->obj->IsSubClassOf(triObjectClassID));
	TriObject *triOb = (TriObject *)os->obj;
	BOOL useSel = triOb->GetMesh().selLevel==MESH_FACE;

	if (unify) {
		triOb->GetMesh().UnifyNormals(useSel);
		triOb->GetMesh().InvalidateTopologyCache ();
		}

	if (flip) {
		for (int i=0; i<triOb->GetMesh().getNumFaces(); i++) {
			if (!useSel || triOb->GetMesh().faceSel[i]) {
				FlipMeshNormal(&triOb->GetMesh(),(DWORD)i);
			}
		}
		triOb->GetMesh().InvalidateTopologyCache ();
	}
			
	triOb->UpdateValidity(TOPO_CHAN_NUM,valid);		
	}

RefResult NormalMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
   		PartID& partID, 
   		RefMessage message) 
   	{	
	switch (message) {
		case REFMSG_CHANGE:
			if (pmapParam && pmapParam->GetParamBlock()==pblock) {
				pmapParam->Invalidate();
				}
			break;
		}
	return REF_SUCCEED;
	}

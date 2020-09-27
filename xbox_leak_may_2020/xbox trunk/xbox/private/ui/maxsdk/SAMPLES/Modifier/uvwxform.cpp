/**********************************************************************
 *<
	FILE: uvwxform.cpp

	DESCRIPTION:  Transforms UVW coords

	CREATED BY: Rolf Berteig

	HISTORY: 10/26/96

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm.h"


class UVWXFormMod : public Modifier {	
public:		
	IParamBlock *pblock;
	
	static IObjParam *ip;
	static IParamMap *pmapParam;
	static UVWXFormMod *editMod;

	UVWXFormMod();

	// From Animatable
	void DeleteThis() { delete this; }
	void GetClassName(TSTR& s) {s = GetString(IDS_RB_UVWXFORMMOD);}  
	virtual Class_ID ClassID() { return Class_ID(UVW_XFORM_CLASS_ID,0);}		
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	TCHAR *GetObjectName() {return GetString(IDS_RB_UVWXFORMMOD);}
	void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);

	IOResult Load(ILoad *iload);

	// From modifier
	ChannelMask ChannelsUsed()  {return PART_TEXMAP|PART_VERTCOLOR|PART_TOPO|PART_GEOM;} // mjm - 10.5.99 - added PART_GEOM
	ChannelMask ChannelsChanged() {return PART_TEXMAP|PART_VERTCOLOR;}
	Class_ID InputType() {return triObjectClassID;}
	void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
	Interval LocalValidity(TimeValue t);

	// From BaseObject
	CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
	IParamArray *GetParamBlock() {return pblock;}
	int GetParamBlockIndex(int id) {return id;}

	int NumRefs() {return 1;}
	RefTargetHandle GetReference(int i) {return pblock;}
	void SetReference(int i, RefTargetHandle rtarg) {pblock = (IParamBlock*)rtarg;}

	int NumSubs() {return 1;}
	Animatable* SubAnim(int i) {return pblock;}
	TSTR SubAnimName(int i) {return _T("");}

	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
	   PartID& partID, RefMessage message);
};


//--- ClassDescriptor and class vars ---------------------------------

IParamMap       *UVWXFormMod::pmapParam = NULL;
IObjParam       *UVWXFormMod::ip        = NULL;
UVWXFormMod     *UVWXFormMod::editMod   = NULL;


class UVWXFormClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new UVWXFormMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_UVWXFORMMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(UVW_XFORM_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
};

static UVWXFormClassDesc uvwxformDesc;
ClassDesc* GetUVWXFormModDesc() {return &uvwxformDesc;}

//--- Parameter map/block descriptors -------------------------------

#define PB_UTILE	0
#define PB_VTILE	1
#define PB_WTILE	2
#define PB_UOFFSET	3
#define PB_VOFFSET	4
#define PB_WOFFSET	5
#define PB_UFLIP	6
#define PB_VFLIP	7
#define PB_WFLIP	8
#define PB_CHANNEL	9
#define PB_MAPCHANNEL 10

//
//
// Parameters

static int chanIDs[] = {IDC_MAP_TEXMAP, IDC_MAP_VERTCOL};

static ParamUIDesc descParam[] = {
	// U Tile
	ParamUIDesc (PB_UTILE, EDITTYPE_FLOAT,
		IDC_MAP_UTILE,IDC_MAP_UTILESPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// V Tile
	ParamUIDesc (PB_VTILE, EDITTYPE_FLOAT,
		IDC_MAP_VTILE,IDC_MAP_VTILESPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// W Tile
	ParamUIDesc (PB_WTILE, EDITTYPE_FLOAT,
		IDC_MAP_WTILE,IDC_MAP_WTILESPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// U Offset
	ParamUIDesc (PB_UOFFSET, EDITTYPE_FLOAT,
		IDC_MAP_UOFFSET,IDC_MAP_UOFFSETSPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// V Offset
	ParamUIDesc (PB_VOFFSET, EDITTYPE_FLOAT,
		IDC_MAP_VOFFSET,IDC_MAP_VOFFSETSPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// W Offset
	ParamUIDesc (PB_WOFFSET, EDITTYPE_FLOAT,
		IDC_MAP_WOFFSET,IDC_MAP_WOFFSETSPIN,
		-BIGFLOAT,BIGFLOAT, SPIN_AUTOSCALE),

	// U Flip
	ParamUIDesc(PB_UFLIP,TYPE_SINGLECHEKBOX,IDC_MAP_UFLIP),

	// V Flip
	ParamUIDesc(PB_VFLIP,TYPE_SINGLECHEKBOX,IDC_MAP_VFLIP),

	// W Flip
	ParamUIDesc(PB_WFLIP,TYPE_SINGLECHEKBOX,IDC_MAP_WFLIP),

	// Channel
	ParamUIDesc(PB_CHANNEL,TYPE_RADIO,chanIDs,2),

	// MapChannel
	ParamUIDesc (PB_MAPCHANNEL, EDITTYPE_POS_INT,
		IDC_MAP_CHAN, IDC_MAP_CHAN_SPIN,
		1, MAX_MESHMAPS-1, SPIN_AUTOSCALE),
};
#define PARAMDESC_LENGTH	11

static ParamBlockDescID descVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE,  PB_UTILE },		// U Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_VTILE },		// V Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_WTILE },		// W Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_UOFFSET },		// U Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_VOFFSET },		// V Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_WOFFSET },		// W Offset
	{ TYPE_INT,   NULL, FALSE, PB_UFLIP },		// U Flip
	{ TYPE_INT,   NULL, FALSE, PB_VFLIP },		// V Flip
	{ TYPE_INT,   NULL, FALSE, PB_WFLIP },		// W Flip	
	};

static ParamBlockDescID descVer1[] = {
	{ TYPE_FLOAT, NULL, TRUE,  PB_UTILE },		// U Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_VTILE },		// V Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_WTILE },		// W Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_UOFFSET },		// U Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_VOFFSET },		// V Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_WOFFSET },		// W Offset
	{ TYPE_INT,   NULL, FALSE, PB_UFLIP },		// U Flip
	{ TYPE_INT,   NULL, FALSE, PB_VFLIP },		// V Flip
	{ TYPE_INT,   NULL, FALSE, PB_WFLIP },		// W Flip	
	{ TYPE_INT,   NULL, FALSE, PB_CHANNEL },		// channel
};

static ParamBlockDescID descVer2[] = {
	{ TYPE_FLOAT, NULL, TRUE,  PB_UTILE },		// U Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_VTILE },		// V Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_WTILE },		// W Tile
	{ TYPE_FLOAT, NULL, TRUE,  PB_UOFFSET },		// U Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_VOFFSET },		// V Offset
	{ TYPE_FLOAT, NULL, TRUE,  PB_WOFFSET },		// W Offset
	{ TYPE_INT,   NULL, FALSE, PB_UFLIP },		// U Flip
	{ TYPE_INT,   NULL, FALSE, PB_VFLIP },		// V Flip
	{ TYPE_INT,   NULL, FALSE, PB_WFLIP },		// W Flip	
	{ TYPE_INT,   NULL, FALSE, PB_CHANNEL },		// channel
	{ TYPE_INT,   NULL, FALSE, PB_MAPCHANNEL },
};

#define PBLOCK_LENGTH	11

// Array of old versions
static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,9,0),	
	ParamVersionDesc(descVer1,10,1),	
};
#define NUM_OLDVERSIONS	2

// Current version
#define CURRENT_VERSION	2
static ParamVersionDesc curVersion(descVer2, PBLOCK_LENGTH, CURRENT_VERSION);


//--- UVWXForm mod methods -------------------------------

UVWXFormMod::UVWXFormMod() {
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descVer2, PBLOCK_LENGTH, CURRENT_VERSION));

	pblock->SetValue(PB_UTILE,0,1.0f);
	pblock->SetValue(PB_VTILE,0,1.0f);
	pblock->SetValue(PB_WTILE,0,1.0f);
	pblock->SetValue (PB_MAPCHANNEL, 0, 1);
}

// Following is necessary to make sure mapchannel doesn't come in as zero;
// this happens when loading up an old scene -- the pblock with the defaults
// is overwritten.
class XSetChannelToOne : public PostLoadCallback {
public:
	UVWXFormMod *xm;
	XSetChannelToOne (UVWXFormMod *xMod) { xm = xMod; }
	void proc (ILoad *iload);
};

void XSetChannelToOne::proc (ILoad *iload) {
	if (xm && xm->pblock) {
		int mapChan;
		xm->pblock->GetValue (PB_MAPCHANNEL, 0, mapChan, FOREVER);
		if (!mapChan) xm->pblock->SetValue (PB_MAPCHANNEL, 0, 1);
	}
	delete this;
}

IOResult UVWXFormMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);

	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,0));
	iload->RegisterPostLoadCallback (new XSetChannelToOne (this));
	return IO_OK;
	}

void UVWXFormMod::BeginEditParams (IObjParam  *ip, ULONG flags,Animatable *prev) {
	this->ip = ip;
	editMod  = this;
	
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);	
	SetAFlag(A_MOD_BEING_EDITED);

	pmapParam = CreateCPParamMap (descParam,PARAMDESC_LENGTH,
		pblock, ip, hInstance, MAKEINTRESOURCE(IDD_UVWXFORMPARAM),
		GetString(IDS_RB_PARAMETERS), 0);	
}

void UVWXFormMod::EndEditParams(IObjParam *ip,ULONG flags,Animatable *next) {
	this->ip = NULL;
	editMod  = NULL;

	TimeValue t = ip->GetTime();

	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	
	DestroyCPParamMap(pmapParam);
	}

RefTargetHandle UVWXFormMod::Clone(RemapDir& remap)
	{
	UVWXFormMod *mod = new UVWXFormMod();
	mod->ReplaceReference(0,pblock->Clone(remap));	
	return mod;
	}

void UVWXFormMod::ModifyObject (TimeValue t, ModContext &mc, ObjectState *os, INode *node) {
	if (!os->obj->IsSubClassOf(triObjectClassID)) return;

	TriObject *tobj = (TriObject*)os->obj;
	Mesh &mesh = tobj->GetMesh();
	Interval iv = FOREVER;
	float ut, vt, wt, uo, vo, wo;
	int uf, vf, wf, channel, mapChannel;
	pblock->GetValue(PB_UTILE,t,ut,iv);
	pblock->GetValue(PB_VTILE,t,vt,iv);
	pblock->GetValue(PB_WTILE,t,wt,iv);
	pblock->GetValue(PB_UOFFSET,t,uo,iv);
	pblock->GetValue(PB_VOFFSET,t,vo,iv);
	pblock->GetValue(PB_WOFFSET,t,wo,iv);	
	pblock->GetValue(PB_UFLIP,t,uf,iv);
	pblock->GetValue(PB_VFLIP,t,vf,iv);
	pblock->GetValue(PB_WFLIP,t,wf,iv);
	pblock->GetValue(PB_CHANNEL,t,channel,iv);
	pblock->GetValue (PB_MAPCHANNEL, t, mapChannel, iv);

	if (channel) {
		mapChannel = 0;
		iv &= tobj->ChannelValidity (t, VERT_COLOR_CHAN_NUM);
	} else {
		iv &= tobj->ChannelValidity (t, TEXMAP_CHAN_NUM);
	}

	if (!mesh.mapSupport(mapChannel)) return;
	int ct = mesh.getNumMapVerts (mapChannel);
	UVVert *mv = mesh.mapVerts (mapChannel);
	for (int i=0; i<ct; i++) {
		Point3 & uvw = mv[i];
		if (uf) uvw.x = 1.0f-uvw.x;
		if (vf) uvw.y = 1.0f-uvw.y;
		if (wf) uvw.z = 1.0f-uvw.z;
		if (ut!=0.0f) uvw.x *= ut;
		if (vt!=0.0f) uvw.y *= vt;
		if (wt!=0.0f) uvw.z *= wt;
		uvw.x += uo;
		uvw.y += vo;
		uvw.z += wo;
	}

	if (channel) os->obj->UpdateValidity (VERT_COLOR_CHAN_NUM, iv);
	else os->obj->UpdateValidity(TEXMAP_CHAN_NUM,iv);
}

Interval UVWXFormMod::LocalValidity(TimeValue t)
	{
	Interval iv = FOREVER;
	float f;
	pblock->GetValue(PB_UTILE,t,f,iv);
	pblock->GetValue(PB_VTILE,t,f,iv);
	pblock->GetValue(PB_WTILE,t,f,iv);
	pblock->GetValue(PB_UOFFSET,t,f,iv);
	pblock->GetValue(PB_VOFFSET,t,f,iv);
	pblock->GetValue(PB_WOFFSET,t,f,iv);	
	return iv;
	}

RefResult UVWXFormMod::NotifyRefChanged( 
		Interval changeInt,RefTargetHandle hTarget, 
		PartID& partID, RefMessage message)
	{
	switch (message) {
		case REFMSG_CHANGE:
			if (editMod==this && pmapParam) pmapParam->Invalidate();
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {
				case PB_UTILE:
				case PB_VTILE:
				case PB_WTILE:
				case PB_UOFFSET:
				case PB_VOFFSET:
				case PB_WOFFSET: gpd->dim = stdNormalizedDim; break;				
				}
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;			
			switch (gpn->index) {
				case PB_UTILE:	 gpn->name = GetString(IDS_RB_UTILE); break;
				case PB_VTILE:	 gpn->name = GetString(IDS_RB_VTILE); break;
				case PB_WTILE:	 gpn->name = GetString(IDS_RB_WTILE); break;
				case PB_UOFFSET: gpn->name = GetString(IDS_RB_UOFFSET); break;
				case PB_VOFFSET: gpn->name = GetString(IDS_RB_VOFFSET); break;
				case PB_WOFFSET: gpn->name = GetString(IDS_RB_WOFFSET); break;
				case PB_UFLIP: gpn->name = GetString(IDS_UVWX_UFLIP); break;
				case PB_VFLIP: gpn->name = GetString(IDS_UVWX_VFLIP); break;
				case PB_WFLIP: gpn->name = GetString(IDS_UVWX_WFLIP); break;
				case PB_CHANNEL: gpn->name = GetString(IDS_UVWX_CHANNEL); break;
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}

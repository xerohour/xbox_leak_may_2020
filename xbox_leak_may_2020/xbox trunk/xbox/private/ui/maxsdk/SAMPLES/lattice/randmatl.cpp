/**********************************************************************
 *<
	FILE: randmatl.cpp

	DESCRIPTION:  Assigns Material ID randomly by element

	CREATED BY: Eric Peterson

	HISTORY: 3/98

 *>	Copyright (c) 1998 for Kinetix, All Rights Reserved.
 **********************************************************************/
#include "mods2.h"
#include "iparamm.h"
 #include "meshadj.h"

const float IntMax=32767.0f;

float RND01()
{	float num;
	num=(float)rand();
	return(num/IntMax);
}

static Class_ID RANDMATLOSM_CLASS_ID(0x597d58ee,0xdec17af);


class RandMatByElemMod : public Modifier {	
	public:
		IParamBlock *pblock;
		static IParamMap *pmapParam;

		RandMatByElemMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_EP_RANDMATLMOD); }  
		virtual Class_ID ClassID() { return RANDMATLOSM_CLASS_ID;}
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() { return GetString(IDS_EP_RANDMATLMOD); }
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
		TSTR SubAnimName(int i) {return GetString(IDS_AP_PARAMETERS);}

		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	};



//--- ClassDescriptor and class vars ---------------------------------

IParamMap *RandMatByElemMod::pmapParam = NULL;



class RandMatlClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new RandMatByElemMod; }
	const TCHAR *	ClassName() { return GetString(IDS_EP_RANDMATLMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return RANDMATLOSM_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_EP_DEFSURFACE);}
	};

static RandMatlClassDesc matDesc;
extern ClassDesc* GetRandMatlModDesc() { return &matDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_MATLMETHOD	0
#define PB_IDCOUNT		1
#define PB_IDONE		2
#define PB_IDTWO		3
#define PB_IDTHREE		4
#define PB_IDFOUR		5
#define PB_IDFIVE		6
#define PB_IDSIX		7
#define PB_IDSEVEN		8
#define PB_IDEIGHT		9
#define	PB_SEED			10

static int methodIDs[] = {IDC_METHRND,IDC_METHLIST};

//
//
// Parameters

static ParamUIDesc descParam[] = {
	

	// Methods
	ParamUIDesc(PB_MATLMETHOD,TYPE_RADIO,methodIDs,2),

	// ID Count
	ParamUIDesc(
		PB_IDCOUNT,
		EDITTYPE_INT,
		IDC_IDCOUNT,IDC_IDCOUNTSPIN,
		1.0f,(float)0xffff,
		1.0f),	

	// Material ID 
	ParamUIDesc(
		PB_IDONE,
		EDITTYPE_FLOAT,
		IDC_MATLID1,IDC_MATLID1SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDTWO,
		EDITTYPE_FLOAT,
		IDC_MATLID2,IDC_MATLID2SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDTHREE,
		EDITTYPE_FLOAT,
		IDC_MATLID3,IDC_MATLID3SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDFOUR,
		EDITTYPE_FLOAT,
		IDC_MATLID4,IDC_MATLID4SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDFIVE,
		EDITTYPE_FLOAT,
		IDC_MATLID5,IDC_MATLID5SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDSIX,
		EDITTYPE_FLOAT,
		IDC_MATLID6,IDC_MATLID6SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDSEVEN,
		EDITTYPE_FLOAT,
		IDC_MATLID7,IDC_MATLID7SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Material ID
	ParamUIDesc(
		PB_IDEIGHT,
		EDITTYPE_FLOAT,
		IDC_MATLID8,IDC_MATLID8SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),

	// Random Seed
	ParamUIDesc(
		PB_SEED,
		EDITTYPE_INT,
		IDC_SEED,IDC_SEEDSPIN,
		0.0f,32000.0f,
		1.0f),
};

#define PARAMDESC_LENGH 11

static ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_INT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_FLOAT, NULL, TRUE, 4 },
	{ TYPE_FLOAT, NULL, TRUE, 5 },
	{ TYPE_FLOAT, NULL, TRUE, 6 },
	{ TYPE_FLOAT, NULL, TRUE, 7 },
	{ TYPE_FLOAT, NULL, TRUE, 8 },
	{ TYPE_FLOAT, NULL, TRUE, 9 },
	{ TYPE_INT, NULL, FALSE, 10 }
};

#define PBLOCK_LENGTH	11

#define CURRENT_VERSION	0

//--- RandMatByElemMod methods -------------------------------

RandMatByElemMod::RandMatByElemMod()
{	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descVer0, PBLOCK_LENGTH, CURRENT_VERSION));	
	pblock->SetValue(PB_MATLMETHOD,0,1);
	pblock->SetValue(PB_IDCOUNT,0,2);
	pblock->SetValue(PB_IDONE,0,0.5f);
	pblock->SetValue(PB_IDTWO,0,0.5f);
	pblock->SetValue(PB_IDTHREE,0,0.0f);
	pblock->SetValue(PB_IDFOUR,0,0.0f);
	pblock->SetValue(PB_IDFIVE,0,0.0f);
	pblock->SetValue(PB_IDSIX,0,0.0f);
	pblock->SetValue(PB_IDSEVEN,0,0.0f);
	pblock->SetValue(PB_IDEIGHT,0,0.0f);
	pblock->SetValue(PB_SEED,0,12345);
}

void RandMatByElemMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_RNDMATLPARAMS),
		GetString(IDS_AP_PARAMETERS),
		0);		
}
		
void RandMatByElemMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{	DestroyCPParamMap(pmapParam);
	pmapParam = NULL;
}

Interval RandMatByElemMod::LocalValidity(TimeValue t)
{	int i;
	float f;
	Interval valid = FOREVER;
	pblock->GetValue(PB_IDCOUNT,t,i,valid);	
	pblock->GetValue(PB_IDONE,t,f,valid);	
	pblock->GetValue(PB_IDTWO,t,f,valid);	
	pblock->GetValue(PB_IDTHREE,t,f,valid);	
	pblock->GetValue(PB_IDFOUR,t,f,valid);	
	pblock->GetValue(PB_IDFIVE,t,f,valid);	
	pblock->GetValue(PB_IDSIX,t,f,valid);	
	pblock->GetValue(PB_IDSEVEN,t,f,valid);	
	pblock->GetValue(PB_IDEIGHT,t,f,valid);	
	return valid;
}

RefTargetHandle RandMatByElemMod::Clone(RemapDir& remap) 
{	RandMatByElemMod* newmod = new RandMatByElemMod();	
	newmod->ReplaceReference(0,pblock->Clone(remap));	
	return newmod;
}

void RandMatByElemMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{	Interval valid = FOREVER;
	int method,nIDs,rndseed;
	float pctIDs[8];
	pblock->GetValue(PB_MATLMETHOD,t,method   ,valid);
	pblock->GetValue(PB_IDCOUNT   ,t,nIDs     ,valid);
	pblock->GetValue(PB_IDONE     ,t,pctIDs[0],valid);
	pblock->GetValue(PB_IDTWO     ,t,pctIDs[1],valid);
	pblock->GetValue(PB_IDTHREE   ,t,pctIDs[2],valid);
	pblock->GetValue(PB_IDFOUR    ,t,pctIDs[3],valid);
	pblock->GetValue(PB_IDFIVE    ,t,pctIDs[4],valid);
	pblock->GetValue(PB_IDSIX     ,t,pctIDs[5],valid);
	pblock->GetValue(PB_IDSEVEN   ,t,pctIDs[6],valid);
	pblock->GetValue(PB_IDEIGHT   ,t,pctIDs[7],valid);
	float firstseven=pctIDs[0]+pctIDs[1]+pctIDs[2]+pctIDs[3]+pctIDs[4]+pctIDs[5]+pctIDs[6];
	if ((pctIDs[7]+firstseven)<1.0f)pctIDs[7]=1.0f-firstseven;
	pblock->GetValue(PB_SEED      ,t,rndseed  ,valid);
	srand(rndseed);

	assert(os->obj->IsSubClassOf(triObjectClassID));

	TriObject *triOb = (TriObject *)os->obj;
	BOOL useSel=triOb->GetMesh().selLevel==MESH_FACE;

	int facecount=triOb->GetMesh().getNumFaces();

	AdjEdgeList aelist(triOb->GetMesh());
	AdjFaceList aflist(triOb->GetMesh(),aelist);
	FaceElementList elemlist(triOb->GetMesh(),aflist);

	int elemcount=elemlist.count;

	int *elemmatlref;
	elemmatlref=new int[elemcount];

	int iloop,jloop,rndindex;
	float rndvar,currentpct;
	BOOL isfound;
	if (method==0) //fill in the element material reference array using random materials
	{	for (iloop=0;iloop<elemcount;iloop++) 
		{	rndindex=int((float)nIDs*RND01());
			if (rndindex==nIDs)rndindex--;
			if (rndindex<0)rndindex=0;
			elemmatlref[iloop]=rndindex;
		}
	}
	else
	{	for (iloop=0;iloop<elemcount;iloop++)
		{	rndvar=RND01();
			isfound=FALSE;
			jloop=0;
			currentpct=0.0f;
			while ((!isfound)&&(jloop<8))
			{	currentpct+=pctIDs[jloop];
				if (rndvar<=currentpct)
				{	elemmatlref[iloop]=jloop;
					isfound=TRUE;
				}
				jloop++;
			}
		}
	}
	
	for (int i=0; i<facecount; i++)
	{	if ((!useSel)||(triOb->GetMesh().faceSel[i]))
		{	triOb->GetMesh().setFaceMtlIndex(i,(MtlID)elemmatlref[elemlist[i]]);
		}
	}
		
	triOb->UpdateValidity(TOPO_CHAN_NUM,valid);		

	delete[] elemmatlref;
}

RefResult RandMatByElemMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
   		PartID& partID, 
   		RefMessage message) 
{	switch (message) {
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
				case PB_MATLMETHOD:	gpn->name=GetString(IDS_AP_METHOD); break;
				case PB_IDCOUNT:	gpn->name=GetString(IDS_AP_COUNT); break;
				case PB_IDONE:		gpn->name=GetString(IDS_AP_MAT1); break;
				case PB_IDTWO:		gpn->name=GetString(IDS_AP_MAT2); break;
				case PB_IDTHREE:	gpn->name=GetString(IDS_AP_MAT3); break;
				case PB_IDFOUR:		gpn->name=GetString(IDS_AP_MAT4); break;
				case PB_IDFIVE:		gpn->name=GetString(IDS_AP_MAT5); break;
				case PB_IDSIX:		gpn->name=GetString(IDS_AP_MAT6); break;
				case PB_IDSEVEN:	gpn->name=GetString(IDS_AP_MAT7); break;
				case PB_IDEIGHT:	gpn->name=GetString(IDS_AP_MAT8); break;
				case PB_SEED:		gpn->name=GetString(IDS_AP_SEED); break;
				default:			gpn->name = TSTR(_T("")); break;
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
}


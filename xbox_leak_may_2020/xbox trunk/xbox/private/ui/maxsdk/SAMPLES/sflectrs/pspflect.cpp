/**********************************************************************
 *<
	FILE: pspflect.cpp

	DESCRIPTION: Enhanced Deflector

	CREATED BY: Eric Peterson from Rolf Berteig's Deflector

	HISTORY: 7-17-97

 **********************************************************************/
#include "sflectr.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

static Class_ID PLANARSPAWNDEF_CLASS_ID(0x4e94628d, 0x4e437774);
static Class_ID PLANARSPAWNDEFMOD_CLASS_ID(0x7d257b98, 0x439e09de);

class PSpawnDeflObj : public SimpleWSMObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		int lastrnd;
		TimeValue t;
		PSpawnDeflObj();
		BOOL SupportsDynamics() {return FALSE;}

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		void MapKeys(TimeMap *map,DWORD flags);
		Class_ID ClassID() {return PLANARSPAWNDEF_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_PLANARSPAWNDEF);}
				
		// from object		
		CreateMouseCallBack* GetCreateMouseCallBack();		
		
		// From SimpleWSMObject		
		void InvalidateUI();		
		void BuildMesh(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);		
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);	
		
		// from ref
		IOResult Load(ILoad *iload);
	};

//--- ClassDescriptor and class vars ---------------------------------

IObjParam *PSpawnDeflObj::ip        = NULL;
IParamMap *PSpawnDeflObj::pmapParam = NULL;
HWND       PSpawnDeflObj::hSot      = NULL;

class PSpawnDeflClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new PSpawnDeflObj;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_PLANARSPAWNDEF);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return PLANARSPAWNDEF_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_PARTICLES);}
	};

static PSpawnDeflClassDesc pspawndeflDesc;
ClassDesc* GetPSpawnDeflObjDesc() {return &pspawndeflDesc;}

//--- DeflectMod -----------------------------------------------------

class PSpawnDeflField : public CollisionObject {
	public:		
		PSpawnDeflObj *obj;
		INode *node;
		Matrix3 tm,invtm,tp;
		Interval tmValid;		
		Point3 Vc,Vcp;
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class PSpawnDeflMod : public SimpleWSMMod {
	public:				
		PSpawnDeflField deflect;

		PSpawnDeflMod() {}
		PSpawnDeflMod(INode *node,PSpawnDeflObj *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_EP_PLANARSPAWNDEFMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return PLANARSPAWNDEFMOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_PLANARSPAWNDEFMODBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class PSpawnDeflModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new PSpawnDeflMod;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_PLANARSPAWNDEFMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return PLANARSPAWNDEFMOD_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};

static PSpawnDeflModClassDesc pspawndeflModDesc;
ClassDesc* GetPSpawnDeflModDesc() {return &pspawndeflModDesc;}

//--- DeflectObject Parameter map/block descriptors ------------------

#define PB_TIMEON		0
#define PB_TIMEOFF		1
#define PB_AFFECTS		2
#define PB_BOUNCE		3
#define PB_BOUNCEVAR	4
#define PB_CHAOS		5
#define PB_INHERIT		6
#define PB_REFRACTS		7
#define PB_DECEL		8
#define PB_DECELVAR		9
#define PB_REFRACTION	10
#define PB_REFRACTVAR	11
#define PB_DIFFUSION	12
#define PB_DIFFUSIONVAR	13
#define PB_ICONSIZE		14
#define PB_ICONSIZE1	15
#define PB_SPAWN		16
#define PB_PASSVEL		17
#define PB_PASSVELVAR	18


static ParamUIDesc descPSpawnDeflParam[] = {
	// Time On
	ParamUIDesc(
		PB_TIMEON,
		EDITTYPE_TIME,
		IDC_EP_TIMEON,IDC_EP_TIMEONSPIN,
		-999999999.0f, 999999999.0f,
		10.0f),
	
	// Time Off
	ParamUIDesc(
		PB_TIMEOFF,
		EDITTYPE_TIME,
		IDC_EP_TIMEOFF,IDC_EP_TIMEOFFSPIN,
		-999999999.0f, 999999999.0f,
		10.0f),
	
	// Affects Percentage
	ParamUIDesc(
		PB_AFFECTS,
		EDITTYPE_FLOAT,
		IDC_EP_AFFECTS,IDC_EP_AFFECTSSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Bounce
	ParamUIDesc(
		PB_BOUNCE,
		EDITTYPE_FLOAT,
		IDC_EP_BOUNCE,IDC_EP_BOUNCESPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),
	
	// BounceVar
	ParamUIDesc(
		PB_BOUNCEVAR,
		EDITTYPE_FLOAT,
		IDC_EP_BOUNCEVAR,IDC_EP_BOUNCEVARSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),
	
	// Chaos
	ParamUIDesc(
		PB_CHAOS,
		EDITTYPE_FLOAT,
		IDC_EP_CHAOS,IDC_EP_CHAOSSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),
	
	// Inherit
	ParamUIDesc(
		PB_INHERIT,
		EDITTYPE_FLOAT,
		IDC_EP_INHERIT,IDC_EP_INHERITSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Refracts Percentage
	ParamUIDesc(
		PB_REFRACTS,
		EDITTYPE_FLOAT,
		IDC_EP_REFRACTS,IDC_EP_REFRACTSSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Refraction Decel
	ParamUIDesc(
		PB_DECEL,
		EDITTYPE_FLOAT,
		IDC_EP_DECEL,IDC_EP_DECELSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),
	
	// Refraction Decel Var
	ParamUIDesc(
		PB_DECELVAR,
		EDITTYPE_FLOAT,
		IDC_EP_DECELVAR,IDC_EP_DECELVARSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Refraction
	ParamUIDesc(
		PB_REFRACTION,
		EDITTYPE_FLOAT,
		IDC_EP_REFRACTION,IDC_EP_REFRACTIONSPIN,
		-100.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Refraction Var
	ParamUIDesc(
		PB_REFRACTVAR,
		EDITTYPE_FLOAT,
		IDC_EP_REFRACTVAR,IDC_EP_REFRACTVARSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Diffusion
	ParamUIDesc(
		PB_DIFFUSION,
		EDITTYPE_FLOAT,
		IDC_EP_DIFFUSION,IDC_EP_DIFFUSIONSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Diffusion Var
	ParamUIDesc(
		PB_DIFFUSIONVAR,
		EDITTYPE_FLOAT,
		IDC_EP_DIFFUSIONVAR,IDC_EP_DIFFUSIONVARSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Spawns Percentage
	ParamUIDesc(
		PB_SPAWN,
		EDITTYPE_FLOAT,
		IDC_EP_SPAWNSONLY,IDC_EP_SPAWNSONLYSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Pass Velocity
	ParamUIDesc(
		PB_PASSVEL,
		EDITTYPE_FLOAT,
		IDC_EP_SPAWNONLYDECEL,IDC_EP_SPAWNONLYDECELSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),
	
	// Pass Velocity Var
	ParamUIDesc(
		PB_PASSVELVAR,
		EDITTYPE_FLOAT,
		IDC_EP_SPAWNSONLYDECELVAR,IDC_EP_SPAWNSONLYDECELVARSPIN,
		0.0f, 100.0f,
		1.0f,
		stdPercentDim),

	// Icon Size
	ParamUIDesc(
		PB_ICONSIZE,
		EDITTYPE_UNIVERSE,
		IDC_EP_ICONSIZE,IDC_EP_ICONSIZESPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Icon Size1
	ParamUIDesc(
		PB_ICONSIZE1,
		EDITTYPE_UNIVERSE,
		IDC_EP_ICONSIZE1,IDC_EP_ICONSIZE1SPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE)
};
#define PARAMDESC_LENGTH	19


ParamBlockDescID descPSpawnDeflVer0[] = {
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
	{ TYPE_FLOAT, NULL, TRUE, 10 },
	{ TYPE_FLOAT, NULL, TRUE, 11 },
	{ TYPE_FLOAT, NULL, TRUE, 12 },
	{ TYPE_FLOAT, NULL, TRUE, 13 },
	{ TYPE_FLOAT, NULL, TRUE, 14 },
	{ TYPE_FLOAT, NULL, TRUE, 15 }};

ParamBlockDescID descPSpawnDeflVer1[] = {
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
	{ TYPE_FLOAT, NULL, TRUE, 10 },
	{ TYPE_FLOAT, NULL, TRUE, 11 },
	{ TYPE_FLOAT, NULL, TRUE, 12 },
	{ TYPE_FLOAT, NULL, TRUE, 13 },
	{ TYPE_FLOAT, NULL, TRUE, 14 },
	{ TYPE_FLOAT, NULL, TRUE, 15 },
	{ TYPE_FLOAT, NULL, TRUE, 16 },
	{ TYPE_FLOAT, NULL, TRUE, 17 },
	{ TYPE_FLOAT, NULL, TRUE, 18 },
};

#define PBLOCK_LENGTH	19

static ParamVersionDesc psversions[] = {
	ParamVersionDesc(descPSpawnDeflVer0,16,0),
	};

#define NUM_OLDVERSIONS	1
#define CURRENT_VERSION	1

static ParamVersionDesc pcurVersion(descPSpawnDeflVer1,PBLOCK_LENGTH,CURRENT_VERSION);

//--- PSpawnDeflector object methods -----------------------------------------


PSpawnDeflObj::PSpawnDeflObj()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descPSpawnDeflVer1, PBLOCK_LENGTH, CURRENT_VERSION));
	assert(pblock);	

	pblock->SetValue(PB_TIMEON,0,0);
	pblock->SetValue(PB_TIMEOFF,0,100*GetTicksPerFrame());
	pblock->SetValue(PB_AFFECTS,0,1.0f);
	pblock->SetValue(PB_BOUNCE,0,1.0f);
	pblock->SetValue(PB_BOUNCEVAR,0,0.0f);
	pblock->SetValue(PB_CHAOS,0,0.0f);
	pblock->SetValue(PB_INHERIT,0,1.0f);
	pblock->SetValue(PB_REFRACTS,0,1.0f);
	pblock->SetValue(PB_DECEL,0,1.0f);
	pblock->SetValue(PB_DECELVAR,0,0.0f);
	pblock->SetValue(PB_REFRACTION,0,0.5f);
	pblock->SetValue(PB_REFRACTVAR,0,0.0f);
	pblock->SetValue(PB_DIFFUSION,0,0.0f);
	pblock->SetValue(PB_DIFFUSIONVAR,0,0.0f);
	pblock->SetValue(PB_SPAWN,0,1.0f);
	pblock->SetValue(PB_PASSVEL,0,1.0f);
	pblock->SetValue(PB_PASSVELVAR,0,0);
	srand(lastrnd=12345);
	t=99999;
	}

Modifier *PSpawnDeflObj::CreateWSMMod(INode *node)
	{
	return new PSpawnDeflMod(node,this);
	}

RefTargetHandle PSpawnDeflObj::Clone(RemapDir& remap) 
	{
	PSpawnDeflObj* newob = new PSpawnDeflObj();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
	}

void PSpawnDeflObj::BeginEditParams(
		IObjParam *ip,ULONG flags,Animatable *prev)
	{
	SimpleWSMObject::BeginEditParams(ip,flags,prev);
	this->ip = ip;

	if (pmapParam) {		
		// Left over
		pmapParam->SetParamBlock(pblock);
	} else {		
		hSot = ip->AddRollupPage( 
			hInstance, 
			MAKEINTRESOURCE(IDD_SW_DESC),
			DefaultSOTProc,
			GetString(IDS_EP_TOP), 
			(LPARAM)ip,APPENDROLL_CLOSED);

		// Gotta make a new one.
		pmapParam = CreateCPParamMap(
			descPSpawnDeflParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_AP_PSPAWNDEFL),
			GetString(IDS_EP_PARAMETERS),
			0);
		}
	}

void PSpawnDeflObj::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
	{		
	SimpleWSMObject::EndEditParams(ip,flags,next);
	this->ip = NULL;

	if (flags&END_EDIT_REMOVEUI ) {		
		DestroyCPParamMap(pmapParam);
		ip->DeleteRollupPage(hSot);
		pmapParam = NULL;		
		}	
	}

void PSpawnDeflObj::MapKeys(TimeMap *map,DWORD flags)
{	Animatable::MapKeys(map,flags);
	TimeValue TempTime;
// remap values
	pblock->GetValue(PB_TIMEON,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_TIMEON,0,TempTime);
	pblock->GetValue(PB_TIMEOFF,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_TIMEOFF,0,TempTime);
}  

void PSpawnDeflObj::BuildMesh(TimeValue t)
{	float width, height;
	float w2,h2,h2p,h2m,h3,h4,h5;
	ivalid = FOREVER;
	pblock->GetValue(PB_ICONSIZE,t,width,ivalid);
	pblock->GetValue(PB_ICONSIZE1,t,height,ivalid);
	width  *= 0.5f;
	w2=width*0.5f;
	height *= 0.5f;
	h2=height*0.5f;
	h2p=h2*1.5f;
	h2m=h2*0.5f;
	h3=h2*0.1f;
	h4=h2*0.07f;
	h5=h2*0.5f;

	mesh.setNumVerts(34);
	mesh.setNumFaces(17);
//square base 
	mesh.setVert(0, Point3(-width,-height, 0.0f));
	mesh.setVert(1, Point3( width,-height, 0.0f));
	mesh.setVert(2, Point3( width, height, 0.0f));
	mesh.setVert(3, Point3(-width, height, 0.0f));

//tripods
	mesh.setVert( 4, Point3( -w2, h2, 0.0f));
	mesh.setVert( 5, Point3( -w2, h2p, -h5));
	mesh.setVert( 6, Point3( -w2, h2 ,h5));
	mesh.setVert( 7, Point3( -w2, h2m, -h5));
	mesh.setVert( 8, Point3(  w2, h2, 0.0f));
	mesh.setVert( 9, Point3(  w2, h2p, -h5));
	mesh.setVert(10, Point3(  w2, h2 ,h5));
	mesh.setVert(11, Point3(  w2, h2m, -h5));
	mesh.setVert(12, Point3(0.0f,-h2, 0.0f));
	mesh.setVert(13, Point3(0.0f,-h2m, -h5));
	mesh.setVert(14, Point3(0.0f,-h2 ,h5));
	mesh.setVert(15, Point3(0.0f,-h2p, -h5));

//each group is a set of three arrowheads for one tripod
	mesh.setVert(16, Point3( -w2, h2p-h3, -h5));
	mesh.setVert(17, Point3( -w2, h2p, -h5+h3));
//	mesh.setVert(18, Point3( -w2, h2+h4 ,-h5-h4));
//	mesh.setVert(19, Point3( -w2, h2-h4 ,-h5-h4));
	mesh.setVert(18, Point3( -w2+h4, h2 ,h4));
	mesh.setVert(19, Point3( -w2-h4, h2 ,h4));
	mesh.setVert(20, Point3( -w2, h2m, -h5+h3));
	mesh.setVert(21, Point3( -w2, h2m+h3, -h5));

	mesh.setVert(22, Point3(  w2, h2p-h3, -h5));
	mesh.setVert(23, Point3(  w2, h2p, -h5+h3));
//	mesh.setVert(24, Point3(  w2, h2+h4 ,-h5-h4));
//	mesh.setVert(25, Point3(  w2, h2-h4 ,-h5-h4));
	mesh.setVert(24, Point3(  w2+h4, h2 ,h4));
	mesh.setVert(25, Point3(  w2-h4, h2 ,h4));
	mesh.setVert(26, Point3(  w2, h2m, -h5+h3));
	mesh.setVert(27, Point3(  w2, h2m+h3, -h5));
	
	mesh.setVert(28, Point3(0.0f,-h2m-h3, -h5));
	mesh.setVert(29, Point3(0.0f,-h2m, -h5+h3));
//	mesh.setVert(30, Point3(0.0f,-h2+h4 ,-h5-h4));
//	mesh.setVert(31, Point3(0.0f,-h2-h4 ,-h5-h4));
	mesh.setVert(30, Point3(0.0f+h4,-h2 ,h4));
	mesh.setVert(31, Point3(0.0f-h4,-h2 ,h4));
	mesh.setVert(32, Point3(0.0f,-h2p, -h5+h3));
	mesh.setVert(33, Point3(0.0f,-h2p+h3, -h5));

	mesh.faces[0].setEdgeVisFlags(1,0,1);
	mesh.faces[0].setSmGroup(0);
	mesh.faces[0].setVerts(0,1,3);

	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(0);
	mesh.faces[1].setVerts(1,2,3);	

	mesh.faces[2].setEdgeVisFlags(1,0,1);
	mesh.faces[2].setSmGroup(0);
	mesh.faces[2].setVerts(4,5,6);	
	mesh.faces[3].setEdgeVisFlags(1,0,1);
	mesh.faces[3].setSmGroup(0);
	mesh.faces[3].setVerts(4,7,6);	
	mesh.faces[4].setEdgeVisFlags(1,0,1);
	mesh.faces[4].setSmGroup(0);
	mesh.faces[4].setVerts(8,9,10);	
	mesh.faces[5].setEdgeVisFlags(1,0,1);
	mesh.faces[5].setSmGroup(0);
	mesh.faces[5].setVerts(8,11,10);	
	mesh.faces[6].setEdgeVisFlags(1,0,1);
	mesh.faces[6].setSmGroup(0);
	mesh.faces[6].setVerts(12,13,14);	
	mesh.faces[7].setEdgeVisFlags(1,0,1);
	mesh.faces[7].setSmGroup(0);
	mesh.faces[7].setVerts(12,15,14);	

	mesh.faces[8].setEdgeVisFlags(1,1,1);
	mesh.faces[8].setSmGroup(0);
	mesh.faces[8].setVerts(5,17,16);	
	mesh.faces[9].setEdgeVisFlags(1,1,1);
	mesh.faces[9].setSmGroup(0);
//	mesh.faces[9].setVerts(6,19,18);	
	mesh.faces[9].setVerts(4,19,18);	
	mesh.faces[10].setEdgeVisFlags(1,1,1);
	mesh.faces[10].setSmGroup(0);
	mesh.faces[10].setVerts(7,21,20);	

	mesh.faces[11].setEdgeVisFlags(1,1,1);
	mesh.faces[11].setSmGroup(0);
	mesh.faces[11].setVerts(9,23,22);	
	mesh.faces[12].setEdgeVisFlags(1,1,1);
	mesh.faces[12].setSmGroup(0);
//	mesh.faces[12].setVerts(10,25,24);	
	mesh.faces[12].setVerts(8,25,24);	
	mesh.faces[13].setEdgeVisFlags(1,1,1);
	mesh.faces[13].setSmGroup(0);
	mesh.faces[13].setVerts(11,27,26);	

	mesh.faces[14].setEdgeVisFlags(1,1,1);
	mesh.faces[14].setSmGroup(0);
	mesh.faces[14].setVerts(13,29,28);	
	mesh.faces[15].setEdgeVisFlags(1,1,1);
	mesh.faces[15].setSmGroup(0);
//	mesh.faces[15].setVerts(14,31,30);	
	mesh.faces[15].setVerts(12,31,30);	
	mesh.faces[16].setEdgeVisFlags(1,1,1);
	mesh.faces[16].setSmGroup(0);
	mesh.faces[16].setVerts(15,33,32);	

	mesh.InvalidateGeomCache();	
	}


class PSpawnDeflObjCreateCallback : public CreateMouseCallBack {
	public:
		PSpawnDeflObj *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int PSpawnDeflObjCreateCallback::proc(
		ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
	{

	#ifdef _OSNAP
	if (msg == MOUSE_FREEMOVE)
	{
		#ifdef _3D_CREATE
			vpt->SnapPreview(m,m,NULL, SNAP_IN_3D);
		#else
			vpt->SnapPreview(m,m,NULL, SNAP_IN_PLANE);
		#endif
	}
	#endif

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
			case 0:
				sp0 = m;
				#ifdef _3D_CREATE	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				mat.SetTrans(p0);
				ob->pblock->SetValue(PB_ICONSIZE,0,0.01f);
				ob->pblock->SetValue(PB_ICONSIZE1,0,0.01f);
				ob->pmapParam->Invalidate();
				break;

			case 1: {
				mat.IdentityMatrix();
				sp1 = m;
				#ifdef _3D_CREATE	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				Point3 center = (p0+p1)/float(2);
				mat.SetTrans(center);
				ob->pblock->SetValue(PB_ICONSIZE,0,(float)fabs(p1.x-p0.x));
				ob->pblock->SetValue(PB_ICONSIZE1,0,(float)fabs(p1.y-p0.y));
				ob->pmapParam->Invalidate();

				if (msg==MOUSE_POINT) {
					if (Length(m-sp0)<3) {						
						return CREATE_ABORT;
					} else {
						return CREATE_STOP;
						}
					}
				break;
				}

			}
	} else {
		if (msg == MOUSE_ABORT)
			return CREATE_ABORT;
		}	
	return TRUE;
	}
static PSpawnDeflObjCreateCallback pspawndeflCreateCB;

CreateMouseCallBack* PSpawnDeflObj::GetCreateMouseCallBack()
	{
	pspawndeflCreateCB.ob = this;
	return &pspawndeflCreateCB;
	}

void PSpawnDeflObj::InvalidateUI() 
	{
	if (pmapParam) pmapParam->Invalidate();
	}

ParamDimension *PSpawnDeflObj::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {		
		case PB_TIMEON:			return stdTimeDim;
		case PB_TIMEOFF:		return stdTimeDim;
		case PB_AFFECTS:		return stdPercentDim;
		case PB_BOUNCEVAR:		return stdPercentDim;
		case PB_CHAOS:			return stdPercentDim;
		case PB_REFRACTS:		return stdPercentDim;
		case PB_DECELVAR:		return stdPercentDim;
		case PB_REFRACTION:		return stdPercentDim;
		case PB_REFRACTVAR:		return stdPercentDim;
		case PB_DIFFUSION:		return stdPercentDim;
		case PB_DIFFUSIONVAR:	return stdPercentDim;
		case PB_SPAWN:			return stdPercentDim;
		case PB_PASSVELVAR:		return stdPercentDim;
		default:				return defaultDim;
		}
	}

TSTR PSpawnDeflObj::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {				
		case PB_TIMEON:			return GetString(IDS_EP_TIMEON);
		case PB_TIMEOFF:		return GetString(IDS_EP_TIMEOFF);
		case PB_AFFECTS:		return GetString(IDS_EP_AFFECTS);
		case PB_BOUNCE:			return GetString(IDS_EP_BOUNCE);
		case PB_BOUNCEVAR:		return GetString(IDS_EP_BOUNCEVAR);
		case PB_CHAOS:			return GetString(IDS_EP_CHAOS);
		case PB_INHERIT:		return GetString(IDS_EP_INHERIT);
		case PB_REFRACTS:		return GetString(IDS_EP_REFRACTS);
		case PB_DECEL:			return GetString(IDS_EP_PASSVEL);
		case PB_DECELVAR:		return GetString(IDS_EP_PASSVELVAR);
		case PB_REFRACTION:		return GetString(IDS_EP_REFRACTION);
		case PB_REFRACTVAR:		return GetString(IDS_EP_REFRACTVAR);
		case PB_DIFFUSION:		return GetString(IDS_EP_DIFFUSION);
		case PB_DIFFUSIONVAR:	return GetString(IDS_EP_DIFFUSIONVAR);
		case PB_SPAWN:			return GetString(IDS_AP_SPAWN);
		case PB_PASSVEL:		return GetString(IDS_AP_PASSVEL);
		case PB_PASSVELVAR:		return GetString(IDS_AP_PASSVELVAR);
		case PB_ICONSIZE:		return GetString(IDS_EP_WIDTH);
		case PB_ICONSIZE1:		return GetString(IDS_EP_HEIGHT);
		default: 				return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

PSpawnDeflMod::PSpawnDeflMod(INode *node,PSpawnDeflObj *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval PSpawnDeflMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;
		PSpawnDeflObj *obj = (PSpawnDeflObj*)GetWSMObject(t);
		TimeValue TempT;
		obj->pblock->GetValue(PB_TIMEOFF,t,TempT,valid);
		obj->pblock->GetValue(PB_AFFECTS,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCE,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCEVAR,t,f,valid);
		obj->pblock->GetValue(PB_CHAOS,t,f,valid);
		obj->pblock->GetValue(PB_INHERIT,t,f,valid);
		obj->pblock->GetValue(PB_REFRACTS,t,f,valid);
		obj->pblock->GetValue(PB_DECEL,t,f,valid);
		obj->pblock->GetValue(PB_DECELVAR,t,f,valid);
		obj->pblock->GetValue(PB_REFRACTION,t,f,valid);
		obj->pblock->GetValue(PB_REFRACTVAR,t,f,valid);
		obj->pblock->GetValue(PB_DIFFUSION,t,f,valid);
		obj->pblock->GetValue(PB_DIFFUSIONVAR,t,f,valid);
		obj->pblock->GetValue(PB_ICONSIZE,t,f,valid);
		obj->pblock->GetValue(PB_ICONSIZE1,t,f,valid);
		obj->pblock->GetValue(PB_SPAWN,t,f,valid);
		obj->pblock->GetValue(PB_PASSVEL,t,f,valid);
		obj->pblock->GetValue(PB_PASSVELVAR,t,f,valid);
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class PSpawnDeflDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static PSpawnDeflDeformer pspawndeformer;

Deformer& PSpawnDeflMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return pspawndeformer;
	}

RefTargetHandle PSpawnDeflMod::Clone(RemapDir& remap) 
	{
	PSpawnDeflMod *newob = new PSpawnDeflMod(nodeRef,(PSpawnDeflObj*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
	}


void PSpawnDeflMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (PSpawnDeflObj*)GetWSMObject(t);
		deflect.node = nodeRef;
		deflect.tmValid.SetEmpty();		
		if (t<=deflect.obj->t) deflect.obj->lastrnd=12345;
		deflect.obj->t=t;
		obj->ApplyCollisionObject(&deflect);
		}
	}

Object *PSpawnDeflField::GetSWObject()
{ return obj;
}

BOOL PSpawnDeflField::CheckCollision(TimeValue t,Point3 &pos,Point3 &vel,float dt,int index,float *ct,BOOL UpdatePastCollide)
{	if (!tmValid.InInterval(t))
	{	tmValid=FOREVER;
		tm=node->GetObjectTM(t,&tmValid);
		invtm=Inverse(tm);
		Interval tmpValid=FOREVER;
		tp=node->GetObjectTM(t-(int)dt,&tmpValid);
		Vc=Zero;
		Vcp=Zero*tp*invtm;
	}
	float stepsize=dt,intimestep=dt;
	Point3 invel=vel;

// test for time limits
	TimeValue startt,endt;
	obj->pblock->GetValue(PB_TIMEON,t,startt,FOREVER);
	obj->pblock->GetValue(PB_TIMEOFF,t,endt,FOREVER);
	if ((t<startt)||(t>endt))
	{	obj->lastrnd=rand();
		return FALSE;
	}

	srand(obj->lastrnd);
	float width,height,at,inherit,theta;
	Point3 p,v,ph,vdt,vrel;
	Point3 tabs2,tabs1,vabsdefl;
	
//test for reflection
	float affectsthisportion;
	obj->pblock->GetValue(PB_AFFECTS,t,affectsthisportion,FOREVER);

	if (RND01()<affectsthisportion)
	{	obj->pblock->GetValue(PB_ICONSIZE,t,width,FOREVER);
		obj->pblock->GetValue(PB_ICONSIZE1,t,height,FOREVER);
		width *=0.5f;
		height*=0.5f;
		float bounce,bouncevar,chaos;
		obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
		obj->pblock->GetValue(PB_BOUNCEVAR,t,bouncevar,FOREVER);
		obj->pblock->GetValue(PB_CHAOS,t,chaos,FOREVER);
		obj->pblock->GetValue(PB_INHERIT,t,inherit,FOREVER);
		p=pos*invtm; 
		v=VectorTransform(invtm,vel);
		vdt=(Vc-Vcp)/dt;
		vrel=v-vdt;
// Compute the point of intersection
		if (fabs(p.z)<0.001f)
		{	//v.z=0.0f;
			at=0.0f;		
		}
		else
		{	if (vrel.z==0.0)
			{	obj->lastrnd=rand();
				return FALSE;
			}
			at= -p.z/vrel.z;
			if ((at<0.0f)||(at>dt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
		}
		ph=p+at*vrel;
// See if the point is within our range
		if ((ph.x<-width)||(ph.x>width)||(ph.y<-height)||(ph.y>height))
		{	obj->lastrnd=rand();
			return FALSE;
		}
// if we're here, we have a collision
// Remove the part of dt we used to get to the collision point
		float holddt = dt;
		dt-=at;
// go back to particle speed to find location of hit
		ph=p+at*v;
// Reflect the velocity about the XY plane and attenuate with the bounce factor
		v.z=-v.z;
// reduce v by bounce parameters
		v*=bounce*(1.0f-bouncevar*RND01());
// rotate velocity vector to give us chaos
		if (!FloatEQ0(chaos))
		{	theta=(float)acos(DotProd(Normalize(v),Point3(0.0f,0.0f,1.0f)));
			theta=(HalfPI-theta)*chaos*RND01();
			Point3 d=Point3(RND11(),RND11(),RND11());
			Point3 c=Normalize(v^d);
			RotateOnePoint(v,&Zero.x,&c.x,theta);
		}
// Tranform back into world space.
		pos = ph*tm;
		vel = VectorTransform(tm,v);
// find absolute motion of deflector and add inheritance
		tabs2 = tm.GetRow(3);
		tabs1 = tp.GetRow(3);
		vabsdefl = (tabs2-tabs1)/intimestep;
		vel += inherit*vabsdefl;
// use up the rest of the time available to find the final position
//		pos += vel*dt;

		if (UpdatePastCollide)
		{	pos += vel*dt;
			if (ct) (*ct) = holddt;
		}
		else
		{	if (ct) (*ct) = at;
		}

		obj->lastrnd=rand();
		return TRUE;
	}

// test for refraction
	float refracts;
	obj->pblock->GetValue(PB_REFRACTS,t,refracts,FOREVER);

	if (RND01()<refracts)
	{	obj->pblock->GetValue(PB_ICONSIZE,t,width,FOREVER);
		obj->pblock->GetValue(PB_ICONSIZE1,t,height,FOREVER);
		width *=0.5f;
		height*=0.5f;
		float refvol,refvar,decel,decelvar;
		obj->pblock->GetValue(PB_REFRACTION,t,refvol,FOREVER);
		obj->pblock->GetValue(PB_REFRACTVAR,t,refvar,FOREVER);
		obj->pblock->GetValue(PB_DECEL,t,decel,FOREVER);
		obj->pblock->GetValue(PB_DECELVAR,t,decelvar,FOREVER);
		obj->pblock->GetValue(PB_INHERIT,t,inherit,FOREVER);
		p=pos*invtm; 
		v=VectorTransform(invtm,vel);
		vdt=(Vc-Vcp)/dt;
		vrel=v-vdt;
// Compute the point of intersection
		if (fabs(p.z)<0.001f)
		{	//v.z=0.0f;
			at=0.0f;		
		}
		else
		{	if (vrel.z==0.0)
			{	obj->lastrnd=rand();
				return FALSE;
			}
			at= -p.z/vrel.z;
			if ((at<0.0f)||(at>dt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
		}
		ph=p+at*vrel;
// See if the point is within our range
		if ((ph.x<-width)||(ph.x>width)||(ph.y<-height)||(ph.y>height))
		{	obj->lastrnd=rand();
			return FALSE;
		}
// if we're here, we have a collision
// Remove the part of dt we used to get to the collision point
		float holddt = dt;
		dt-=at;
// figure out if we're hitting the front or back
		float dirapproach;
		Point3 ZVec=Point3(0.0f,0.0f,1.0f);
		dirapproach=(DotProd(vrel,ZVec)<0.0f?1.0f:-1.0f);
		Point3 MZVec=Point3(0.0f,0.0f,-1.0f); 
// go back to particle speed to find location of hit
		if (dirapproach>0.0f)
			ph=p+at*v-ZVec;
		else
			ph=p+at*v-MZVec;
// reduce v by decel parameters
		v*=decel*(1.0f-decelvar*RND01());
// rotate velocity vector
		float maxref,refangle,maxvarref;
		refangle=0.0f;
		if (!FloatEQ0(refvol))
		{	if (dirapproach>0.0f)
				theta=(float)acos(DotProd(Normalize(-v),ZVec));
			else
				theta=(float)acos(DotProd(Normalize(-v),MZVec));
			if ((refvol>0.0f)==(dirapproach>0.0f))
				maxref=-theta;
			else 
				maxref=HalfPI-theta;
			refangle=maxref*(float)fabs(refvol);
			float frefangle=(float)fabs(refangle);
			if ((refvol>0.0f)==(dirapproach>0.0f))
				maxvarref=HalfPI-theta-frefangle;
			else
				maxvarref=theta-frefangle;
			refangle+=maxvarref*RND11()*refvar;
			Point3 c,d;
			if (theta<0.01f)
			{	d=Point3(RND11(),RND11(),RND11());
				c=Normalize(v^d);
			}
			else
			{	if (dirapproach>0.0f)
				 	c=Normalize(ZVec^(-v));
				else
				 	c=Normalize(MZVec^(-v));
			}
			RotateOnePoint(v,&Zero.x,&c.x,refangle);
			if ((v.z>0.0f)==(dirapproach>0.0f)) v.z=0.0f;
		}
		float maxdiff,diffuse,diffvar,diffangle;
		obj->pblock->GetValue(PB_DIFFUSION,t,diffuse,FOREVER);
		obj->pblock->GetValue(PB_DIFFUSIONVAR,t,diffvar,FOREVER);
//		if ((refvol>0.0f)==(dirapproach>0.0f))
			maxdiff=HalfPI-theta-refangle;
//		else
//			maxdiff=theta-refangle;
		if (!FloatEQ0(diffuse))
		{	Point3 d=Point3(RND11(),RND11(),RND11());
			Point3 c=Normalize(v^d);
			diffangle=0.5f*maxdiff*diffuse*(1.0f+RND11()*diffvar);
			RotateOnePoint(v,&Zero.x,&c.x,diffangle);
			if ((v.z>0.0f)==(dirapproach>0.0f)) v.z=0.0f;
		}
// Transform back into world space.
		pos = ph*tm;
		vel = VectorTransform(tm,v);
// find absolute motion of deflector and add inheritance
		tabs2 = tm.GetRow(3);
		tabs1 = tp.GetRow(3);
		vabsdefl = (tabs2-tabs1)/intimestep;
		vel += inherit*vabsdefl;
// use up the rest of the time available to find the final position
//		pos += vel*dt;

		if (UpdatePastCollide)
		{	pos += vel*dt;
			if (ct) (*ct) = holddt;
		}
		else
		{	if (ct) (*ct) = at;
		}

		obj->lastrnd=rand();
		return TRUE;
	}

// test for spawns only
	float spawnsonly;
	obj->pblock->GetValue(PB_SPAWN,t,spawnsonly,FOREVER);

	if (RND01()<spawnsonly)
	{	obj->pblock->GetValue(PB_ICONSIZE,t,width,FOREVER);
		obj->pblock->GetValue(PB_ICONSIZE1,t,height,FOREVER);
		width *= 0.5f;
		height *= 0.5f;
		p = pos*invtm; 
		v = VectorTransform(invtm,vel);
		vdt = (Vc-Vcp)/dt;
		vrel = v-vdt;
// Compute the point of intersection
		if (fabs(p.z)<0.001f)
		{	//v.z=0.0f;
			at=0.0f;		
		}
		else
		{	if (vrel.z==0.0)
			{	obj->lastrnd=rand();
				return FALSE;
			}
			at= -p.z/vrel.z;
			if ((at<0.0f)||(at>dt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
		}
		ph = p + at*vrel;
// See if the point is within our range
		if ((ph.x<-width)||(ph.x>width)||(ph.y<-height)||(ph.y>height))
		{	obj->lastrnd=rand();
			return FALSE;
		}
// if we're here, we have a collision
		float passvel,passvelvar;
		obj->pblock->GetValue(PB_PASSVEL,t,passvel,FOREVER);
		obj->pblock->GetValue(PB_PASSVELVAR,t,passvelvar,FOREVER);
		pos += vel*at;
		vel *= passvel*(1.0f+passvelvar*RND11());

		if (UpdatePastCollide)
		{	pos += vel*(dt-at);
			if (ct) (*ct) = dt;
		}
		else
		{	if (ct) (*ct) = at;
		}

		return TRUE;
	}

// nothing happened
	obj->lastrnd=rand();
	return FALSE;
}

class PSpawnDeflPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		PSpawnDeflPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload) {
			DWORD oldVer = ((PSpawnDeflObj*)(cb->targ))->pblock->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<1) {	
 				((PSpawnDeflObj*)targ)->pblock->SetValue(PB_SPAWN,0,1.0f);
 				((PSpawnDeflObj*)targ)->pblock->SetValue(PB_PASSVEL,0,1.0f);
 				((PSpawnDeflObj*)targ)->pblock->SetValue(PB_PASSVELVAR,0,0);
				}
			delete this;
			}
	};

IOResult PSpawnDeflObj::Load(ILoad *iload) 
	{	
	// This is the callback that corrects for any older versions
	// of the parameter block structure found in the MAX file 
	// being loaded.
	iload->RegisterPostLoadCallback(
		new PSpawnDeflPostLoadCallback(
		new ParamBlockPLCB(psversions,NUM_OLDVERSIONS,&pcurVersion,this,0)));
	return IO_OK;
	}


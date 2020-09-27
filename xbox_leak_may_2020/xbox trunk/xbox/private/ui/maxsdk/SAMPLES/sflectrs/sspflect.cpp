/**********************************************************************
 *<
	FILE: sspflect.cpp

	DESCRIPTION: Enhanced Deflector

	CREATED BY: Eric Peterson from Audrey Peterson's SDeflector code

	HISTORY: 7/97

 **********************************************************************/
#include "sflectr.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

static Class_ID SSPAWNDEFL_CLASS_ID(0x656107ca, 0x1f284a6f);
static Class_ID SSPAWNDEFLMOD_CLASS_ID(0x72a61178, 0x21b407d9);

class SSpawnDeflObject : public SimpleWSMObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		int lastrnd;
		TimeValue t;
		SSpawnDeflObject();
		BOOL SupportsDynamics() {return FALSE;}

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		void MapKeys(TimeMap *map,DWORD flags);
		Class_ID ClassID() {return SSPAWNDEFL_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_SSPAWNDEFLECTOR);}
				
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

IObjParam *SSpawnDeflObject::ip        = NULL;
IParamMap *SSpawnDeflObject::pmapParam = NULL;
HWND       SSpawnDeflObject::hSot      = NULL;

class SSpawnDeflClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new SSpawnDeflObject;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_SSPAWNDEFLECTOR);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return SSPAWNDEFL_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_PARTICLES);}
	};

static SSpawnDeflClassDesc SSpawnDeflDesc;
ClassDesc* GetSSpawnDeflObjDesc() {return &SSpawnDeflDesc;}

//--- DeflectMod -----------------------------------------------------

class SSpawnDeflField : public CollisionObject {
	public:		
		SSpawnDeflObject *obj;
		INode *node;
		Matrix3 tm, invtm,tp;
		Interval tmValid;
		Point3 Vc,Vcp;
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class SSpawnDeflMod : public SimpleWSMMod {
	public:				
		SSpawnDeflField deflect;

		SSpawnDeflMod() {}
		SSpawnDeflMod(INode *node,SSpawnDeflObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_EP_SSPAWNDEFLECTORMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return SSPAWNDEFLMOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_SSPAWNDEFLECTORBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class SSpawnDeflModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new SSpawnDeflMod;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_SSPAWNDEFLECTORMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return SSPAWNDEFLMOD_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};

static SSpawnDeflModClassDesc SSpawnDeflModDesc;
ClassDesc* GetSSpawnDeflModDesc() {return &SSpawnDeflModDesc;}

//--- SphereFlectorObject Parameter map/block descriptors ------------------

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
#define PB_RADIUS		14
#define PB_SPAWN		15
#define PB_PASSVEL		16
#define PB_PASSVELVAR	17

static ParamUIDesc descSSpawnDeflParam[] = {
	// Start Time
	ParamUIDesc(
		PB_TIMEON,
		EDITTYPE_TIME,
		IDC_EP_TIMEON,IDC_EP_TIMEONSPIN,
		-999999999.0f, 999999999.0f,
		10.0f),
	
	// Stop Time
	ParamUIDesc(
		PB_TIMEOFF,
		EDITTYPE_TIME,
		IDC_EP_TIMEOFF,IDC_EP_TIMEOFFSPIN,
		-999999999.0f, 999999999.0f,
		10.0f),
	
	// Affects
	ParamUIDesc(
		PB_AFFECTS,
		EDITTYPE_FLOAT,
		IDC_EP_AFFECTS,IDC_EP_AFFECTSSPIN,
		0.0f,100.0f,
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

	// Radius
	ParamUIDesc(
		PB_RADIUS,
		EDITTYPE_UNIVERSE,
		IDC_EP_ICONSIZE,IDC_EP_ICONSIZESPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	};

#define PARAMDESC_LENGTH	18

ParamBlockDescID SSpawnDefldescVer0[] = {
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
	{ TYPE_FLOAT, NULL, TRUE, 14 }};
	
ParamBlockDescID SSpawnDefldescVer1[] = {
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
};	

#define PBLOCK_LENGTH	18

static ParamVersionDesc ssversions[] = {
	ParamVersionDesc(SSpawnDefldescVer0,15,0),
	};

#define NUM_OLDVERSIONS	1
#define CURRENT_VERSION	1

static ParamVersionDesc scurVersion(SSpawnDefldescVer1,PBLOCK_LENGTH,CURRENT_VERSION);


//--- Deflect object methods -----------------------------------------

SSpawnDeflObject::SSpawnDeflObject()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(SSpawnDefldescVer1, PBLOCK_LENGTH, CURRENT_VERSION));
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

Modifier *SSpawnDeflObject::CreateWSMMod(INode *node)
	{
	return new SSpawnDeflMod(node,this);
	}

RefTargetHandle SSpawnDeflObject::Clone(RemapDir& remap) 
	{
	SSpawnDeflObject* newob = new SSpawnDeflObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
	}

void SSpawnDeflObject::BeginEditParams(
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
			descSSpawnDeflParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_AP_SSPAWNDEFL),
			GetString(IDS_EP_PARAMETERS),
			0);
		}
	}

void SSpawnDeflObject::EndEditParams(
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

void SSpawnDeflObject::MapKeys(TimeMap *map,DWORD flags)
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

void SSpawnDeflObject::BuildMesh(TimeValue t)
{	ivalid = FOREVER;
	float length,r2,r3,r,r4;
	pblock->GetValue(PB_RADIUS,t,length,ivalid);
	float u;
	#define NUM_SEGS	24
	 r=length;
	 r2=0.5f*r;
	 r3=0.25f*r2;
	 r4=0.71f*r3;

	mesh.setNumVerts(3*NUM_SEGS+21);
	mesh.setNumFaces(3*NUM_SEGS+10);

	for (int i=0; i<NUM_SEGS; i++)
	{	u=float(i)/float(NUM_SEGS) * TWOPI;
		mesh.setVert(i, Point3((float)cos(u) * length, (float)sin(u) * length, 0.0f));
		mesh.setVert(i+NUM_SEGS, Point3(0.0f, (float)cos(u) * length, (float)sin(u) * length));
		mesh.setVert(i+2*NUM_SEGS, Point3((float)cos(u) * length, 0.0f, (float)sin(u) * length));
	}
//	for (i=0; i<NUM_SEGS; i++)
//	{	u=float(i)/float(NUM_SEGS) * TWOPI;
//	}
//	for (i=0; i<NUM_SEGS; i++)
//	{	u=float(i)/float(NUM_SEGS) * TWOPI;
//	}		
	mesh.setVert(3*NUM_SEGS, Point3(0.0f, 0.0f, 0.0f));

	mesh.setVert(73,Point3(0.0f,0.0f  ,r      ));//a //juncture on sphere
	mesh.setVert(74,Point3(0.0f,0.0f  ,r+r2   ));//b //end third prong
	mesh.setVert(75,Point3(0.0f,-r2   ,r-r2   ));//c // end second prong
	mesh.setVert(76,Point3(0.0f, r2   ,r-r2   ));//d //end second prong
//	mesh.setVert(77,Point3(0.0f, r4   ,r+r2+r4));//b1 //lone arrow head
//	mesh.setVert(78,Point3(0.0f,-r4   ,r+r2+r4));//b2 //lone arrow head
	mesh.setVert(77,Point3(0.0f, r4   ,r+r4   ));//b1 //lone arrow head
	mesh.setVert(78,Point3(0.0f,-r4   ,r+r4   ));//b2 //lone arrow head
	mesh.setVert(79,Point3(0.0f,-r2   ,r-r2+r3));//c1
	mesh.setVert(80,Point3(0.0f,-r2+r3,r-r2   ));//c2
	mesh.setVert(81,Point3(0.0f, r2-r3,r-r2   ));//d1
	mesh.setVert(82,Point3(0.0f, r2   ,r-r2+r3));//d2
	
	mesh.setVert(83,Point3(0.0f,0.0f  ,-r      ));//a //juncture on sphere
	mesh.setVert(84,Point3(0.0f,0.0f  ,-r-r2   ));//b //end third prong
	mesh.setVert(85,Point3(0.0f,-r2   ,-r+r2   ));//c // end second prong
	mesh.setVert(86,Point3(0.0f, r2   ,-r+r2   ));//d //end second prong
//	mesh.setVert(87,Point3(0.0f, r4   ,-r-r2-r4));//b1 //lone arrow head
//	mesh.setVert(88,Point3(0.0f,-r4   ,-r-r2-r4));//b2 //lone arrow head
	mesh.setVert(87,Point3(0.0f, r4   ,-r-r4   ));//b1 //lone arrow head
	mesh.setVert(88,Point3(0.0f,-r4   ,-r-r4   ));//b2 //lone arrow head
	mesh.setVert(89,Point3(0.0f,-r2   ,-r+r2-r3));//c1
	mesh.setVert(90,Point3(0.0f,-r2+r3,-r+r2   ));//c2
	mesh.setVert(91,Point3(0.0f, r2-r3,-r+r2   ));//d1
	mesh.setVert(92,Point3(0.0f, r2   ,-r+r2-r3));//d2
	
	for (i=0; i<3*NUM_SEGS; i++)
	{	int i1 = i+1;
		if (i1%NUM_SEGS==0) i1 -= NUM_SEGS;
		mesh.faces[i].setEdgeVisFlags(1,0,0);
		mesh.faces[i].setSmGroup(0);
		mesh.faces[i].setVerts(i,i1,3*NUM_SEGS);
	}

	mesh.faces[72].setEdgeVisFlags(1,0,1);
	mesh.faces[72].setSmGroup(0);
	mesh.faces[72].setVerts(73,75,76);
	mesh.faces[73].setEdgeVisFlags(1,1,1);
	mesh.faces[73].setSmGroup(0);
	mesh.faces[73].setVerts(73,74,74);
	mesh.faces[74].setEdgeVisFlags(1,1,1);
	mesh.faces[74].setSmGroup(0);
//	mesh.faces[74].setVerts(74,78,77);
	mesh.faces[74].setVerts(73,78,77);
	mesh.faces[75].setEdgeVisFlags(1,1,1);
	mesh.faces[75].setSmGroup(0);
	mesh.faces[75].setVerts(75,79,80);
	mesh.faces[76].setEdgeVisFlags(1,1,1);
	mesh.faces[76].setSmGroup(0);
	mesh.faces[76].setVerts(76,81,82);

	mesh.faces[77].setEdgeVisFlags(1,0,1);
	mesh.faces[77].setSmGroup(0);
	mesh.faces[77].setVerts(83,85,86);
	mesh.faces[78].setEdgeVisFlags(1,0,1);
	mesh.faces[78].setSmGroup(0);
	mesh.faces[78].setVerts(83,84,84);
	mesh.faces[79].setEdgeVisFlags(1,1,1);
	mesh.faces[79].setSmGroup(0);
//	mesh.faces[79].setVerts(84,88,87);
	mesh.faces[79].setVerts(83,88,87);
	mesh.faces[80].setEdgeVisFlags(1,1,1);
	mesh.faces[80].setSmGroup(0);
	mesh.faces[80].setVerts(85,89,90);
	mesh.faces[81].setEdgeVisFlags(1,1,1);
	mesh.faces[81].setSmGroup(0);
	mesh.faces[81].setVerts(86,91,92);

	mesh.InvalidateGeomCache();
}


class SSpawnDeflObjCreateCallback : public CreateMouseCallBack {
	public:
		SSpawnDeflObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int SSpawnDeflObjCreateCallback::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
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
	
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE)
	{	switch(point)
		{	case 0:
				sp0 = m;
				#ifdef _3D_CREATE	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				mat.SetTrans(p0);
				ob->pblock->SetValue(PB_RADIUS,0,0.01f);
				ob->pmapParam->Invalidate();
				break;
			case 1:
			{	sp1 = m;
				#ifdef _3D_CREATE	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				ob->pblock->SetValue(PB_RADIUS,0,Length(p1-p0));
				ob->pmapParam->Invalidate();

				if (msg==MOUSE_POINT)
				{	if (Length(m-sp0)<3) 
						return CREATE_ABORT;
					else
						return CREATE_STOP;
				}
				break;
			}
		}
	}
	else
		if (msg == MOUSE_ABORT)	return CREATE_ABORT;
	return TRUE;
}

static SSpawnDeflObjCreateCallback SSpawnDeflCreateCB;

CreateMouseCallBack* SSpawnDeflObject::GetCreateMouseCallBack()
	{
	SSpawnDeflCreateCB.ob = this;
	return &SSpawnDeflCreateCB;
	}

void SSpawnDeflObject::InvalidateUI() 
{	if (pmapParam) pmapParam->Invalidate();
}

ParamDimension *SSpawnDeflObject::GetParameterDim(int pbIndex) 
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

TSTR SSpawnDeflObject::GetParameterName(int pbIndex) 
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
		case PB_RADIUS:			return GetString(IDS_EP_RADIUS);
		case PB_SPAWN:			return GetString(IDS_AP_SPAWN);
		case PB_PASSVEL:		return GetString(IDS_AP_PASSVEL);
		case PB_PASSVELVAR:		return GetString(IDS_AP_PASSVELVAR);
		default: 				return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

SSpawnDeflMod::SSpawnDeflMod(INode *node,SSpawnDeflObject *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval SSpawnDeflMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		SSpawnDeflObject *obj = (SSpawnDeflObject*)GetWSMObject(t);
		obj->pblock->GetValue(PB_TIMEOFF,t,f,valid);
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
		obj->pblock->GetValue(PB_RADIUS,t,f,valid);
		obj->pblock->GetValue(PB_SPAWN,t,f,valid);
		obj->pblock->GetValue(PB_PASSVEL,t,f,valid);
		obj->pblock->GetValue(PB_PASSVELVAR,t,f,valid);
		tm=nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class SSpawnDeflDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static SSpawnDeflDeformer SSpawnDeflddeformer;

Deformer& SSpawnDeflMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return SSpawnDeflddeformer;
	}

RefTargetHandle SSpawnDeflMod::Clone(RemapDir& remap) 
{	SSpawnDeflMod *newob = new SSpawnDeflMod(nodeRef,(SSpawnDeflObject*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
}


void SSpawnDeflMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (SSpawnDeflObject*)GetWSMObject(t);
		deflect.node = nodeRef;
		deflect.tmValid.SetEmpty();		
		if (t<=deflect.obj->t) deflect.obj->lastrnd=12345;
		deflect.obj->t=t;
		obj->ApplyCollisionObject(&deflect);
		}
	}

Object *SSpawnDeflField::GetSWObject()
{ return obj;
}

BOOL SSpawnDeflField::CheckCollision(TimeValue t,Point3 &pos,Point3 &vel,float dt,int index,float *ct,BOOL UpdatePastCollide)
{	if (!tmValid.InInterval(t))
	{	tmValid=FOREVER;
		tm=node->GetObjectTM(t,&tmValid);
		invtm=Inverse(tm);
		Interval tmpValid=FOREVER;
		tp=node->GetObjectTM(t+(int)dt,&tmpValid);
		Vc=Zero;
		Vcp=Zero*tp*invtm;
	}
	TimeValue startt,endt;
	obj->pblock->GetValue(PB_TIMEON,t,startt,FOREVER);
	obj->pblock->GetValue(PB_TIMEOFF,t,endt,FOREVER);
	if ((t<startt)||(t>endt))
	{	obj->lastrnd=rand();
		return FALSE;
	}

    srand(obj->lastrnd);

// Main reflection / refraction loop starts here
	float radius,vinher;
	float rsquare,rplus,rminus,TempDP;
	Point3 p,vr,Vdt,Vrel,Vreln;

// test for reflection
	float affectsthisportion;
	obj->pblock->GetValue(PB_AFFECTS,t,affectsthisportion,FOREVER);

	if (RND01()<affectsthisportion) 
	{	float bounce,bvar,chaos;
		obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
		obj->pblock->GetValue(PB_BOUNCEVAR,t,bvar,FOREVER);
		obj->pblock->GetValue(PB_CHAOS,t,chaos,FOREVER);
		obj->pblock->GetValue(PB_INHERIT,t,vinher,FOREVER);
		
		obj->pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
		p=pos*invtm; 
		vr=VectorTransform(invtm,vel);
		Vdt=(Vcp-Vc)/dt;
		Vrel=vr-Vdt;
		rsquare=radius*radius;
		rplus=radius;rminus=radius;
		Vreln=Normalize(Vrel);
		if (LengthSquared(p-Vc)>=(rminus*rminus)) //outside
		{	Point3 P1;
			P1=p+dt*Vrel; //second particle position
			if (!(LengthSquared(P1-Vc)<(rplus*rplus)))
			{	float Dist,Dist1;
				Dist=DotProd(Vreln,(Vc-p));
				if (Dist<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Dist1=DotProd(-Vreln,(Vc-P1));
				if (Dist1<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Point3 P10=P1-p,Pc=Vc-p;
				float gamma=(float)acos(DotProd(P10,Pc)/(Length(P10)*Length(Pc)));
				float Dist2=Length(Pc)*(float)cos(HalfPI-gamma);
				if (Dist2>radius)
				{	obj->lastrnd=rand();
					return FALSE;
				}
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
// solve for collision params starts here
			Point3 XI,r,n;
			XI=p+omega*vr;
			Point3 Ci=Vc+omega*Vdt;
			r=Normalize(XI-Ci);
			float q1=DotProd(-Vreln,r);
			float theta=(float)acos(q1);
			if (theta>=HalfPI) theta-=PI;
			float v[4];
			if (theta<FLOAT_EPSILON)
				vr=-vr;
			else 
			{	n=Normalize((-Vreln)^r);
				vr=-vr;
				memcpy(v,vr,row3size);v[3]=1.0f;
				RotateOnePoint(v,&Zero.x,&n.x,2*theta);
				memcpy(vr,v,row3size);
			}
			vr=vr*bounce*(1-bvar*RND01());
			if (!FloatEQ0(chaos))
			{	theta=(HalfPI-theta)*chaos*RND01();
				Point3 d=Point3(RND11(),RND11(),RND11());
				Point3 c=Normalize(vr^d);
				memcpy(v,vr,row3size);v[3]=1.0f;
				RotateOnePoint(v,&Zero.x,&c.x,theta);
				memcpy(vr,v,row3size);
			}
			if ((vinher>0.0f)&&(t>0)){vr=vr+DotProd(Vdt*vinher,r)*r;}
			pos=XI+(dt-omega)*vr;
		}
		else //inside
		{	Point3 P1;
			P1=p+dt*Vrel;
			if (LengthSquared(P1-Vc)<(rplus*rplus))
			{	obj->lastrnd=rand();
				return FALSE;
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
			Point3 XI,r,n;
			XI=p+omega*vr;
			Point3 Ci=Vc+omega*Vdt;
			r=Normalize(XI-Ci);
			float q1=DotProd(Vreln,r);
			float theta=(float)acos(q1);
			float v[4];
			if (theta<FLOAT_EPSILON)
				vr=-vr;
			else 
			{	n=Normalize(Vreln^r);
				vr=-vr;
				memcpy(v,vr,row3size);v[3]=1.0f;
				RotateOnePoint(v,&Zero.x,&n.x,2*theta);
				memcpy(vr,v,row3size);
			}
			vr=vr*bounce*(1-bvar*RND01());
			if (!FloatEQ0(chaos))
			{	theta=(HalfPI-theta)*chaos*RND01();
				Point3 d=Point3(RND11(),RND11(),RND11());
				Point3 c=Normalize(vr^d);
				memcpy(v,vr,row3size);v[3]=1.0f;
				RotateOnePoint(v,&Zero.x,&c.x,theta);
				memcpy(vr,v,row3size);
			}
			if ((vinher>0.0f)&&(t>0)){vr=vr+DotProd(Vdt*vinher,r)*r;}
			if (UpdatePastCollide)
			{	pos = XI + (dt-omega)*vr;
				if (ct) (*ct) = dt;
			}
			else
			{	pos = XI;
				if (ct) (*ct) = omega;
			}
		}
		pos = pos*tm;
		vel = VectorTransform(tm,vr);
		obj->lastrnd=rand();
		return TRUE;
	}

// test for refraction
	float refracts;
	obj->pblock->GetValue(PB_REFRACTS,t,refracts,FOREVER);

	if (RND01()<refracts)
	{	float refvol,refvar,decel,decelvar;
		obj->pblock->GetValue(PB_REFRACTION,t,refvol,FOREVER);
		obj->pblock->GetValue(PB_REFRACTVAR,t,refvar,FOREVER);
		obj->pblock->GetValue(PB_DECEL,t,decel,FOREVER);
		obj->pblock->GetValue(PB_DECELVAR,t,decelvar,FOREVER);
		obj->pblock->GetValue(PB_INHERIT,t,vinher,FOREVER);
		obj->pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
		p=pos*invtm; 
		vr=VectorTransform(invtm,vel);
		Vdt=(Vcp-Vc)/dt;
		Vrel=vr-Vdt;
		rsquare=radius*radius;
		rplus=radius;rminus=radius;
		Vreln=Normalize(Vrel);
		if (LengthSquared(p-Vc)>=(rminus*rminus)) //outside
		{	Point3 P1;
			P1=p+dt*Vrel; //second particle position
			if (!(LengthSquared(P1-Vc)<(rplus*rplus)))
			{	float Dist,Dist1;
				Dist=DotProd(Vreln,(Vc-p));
				if (Dist<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Dist1=DotProd(-Vreln,(Vc-P1));
				if (Dist1<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Point3 P10=P1-p,Pc=Vc-p;
				float gamma=(float)acos(DotProd(P10,Pc)/(Length(P10)*Length(Pc)));
				float Dist2=Length(Pc)*(float)cos(HalfPI-gamma);
				if (Dist2>radius)
				{	obj->lastrnd=rand();
					return FALSE;
				}
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
// solve for collision params starts here
			Point3 XI,r,n;
			XI=p+omega*vr;
			Point3 Ci=Vc+omega*Vdt;
			r=Normalize(XI-Ci);
			float q1=DotProd(-Vreln,r);
			float theta=(float)acos(q1);
			if (theta>=HalfPI) theta-=PI;
// refraction solution
// reduce v by decel parameters
			vr*=decel*(1.0f-decelvar*RND01());
// rotate velocity vector
			float maxref,refangle,maxvarref;
			refangle=0.0f;
			if (!FloatEQ0(refvol))
			{	if (refvol>0.0f)
					maxref=-theta;
				else 
					maxref=HalfPI-theta;
				refangle=maxref*(float)fabs(refvol);
				float frefangle=(float)fabs(refangle);
				if (refvol>0.0f)
					maxvarref=HalfPI-theta-frefangle;
				else
					maxvarref=theta-frefangle;
				refangle+=maxvarref*RND11()*refvar;
				Point3 c,d;
				if (theta<0.01f)
				{	d=Point3(RND11(),RND11(),RND11());
					c=Normalize(vr^d);
				}
				else
				{	c=Normalize(r^(-vr));
				}
				RotateOnePoint(vr,&Zero.x,&c.x,refangle);
				if ((TempDP=DotProd(vr,r))>0.0f) vr=vr-TempDP*r;
			}
			float maxdiff,diffuse,diffvar,diffangle;
			obj->pblock->GetValue(PB_DIFFUSION,t,diffuse,FOREVER);
			obj->pblock->GetValue(PB_DIFFUSIONVAR,t,diffvar,FOREVER);
			maxdiff=HalfPI-theta-refangle;
			if (!FloatEQ0(diffuse))
			{	Point3 d=Point3(RND11(),RND11(),RND11());
				Point3 c=Normalize(vr^d);
				diffangle=0.5f*maxdiff*diffuse*(1.0f+RND11()*diffvar);
				RotateOnePoint(vr,&Zero.x,&c.x,diffangle);
				if (TempDP=(DotProd(vr,r))>0.0f) vr=vr-TempDP*r;
			}
			if ((vinher>0.0f)&&(t>0)){vr=vr+DotProd(Vdt*vinher,r)*r;}
			pos=XI+(dt-omega)*vr;
		}
		else //inside
		{	Point3 P1;
			P1=p+dt*Vrel;
			if (LengthSquared(P1-Vc)<(rplus*rplus))
			{	obj->lastrnd=rand();
				return FALSE;
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
			Point3 XI,r,n;
			XI=p+omega*vr;
			Point3 Ci=Vc+omega*Vdt;
			r=Normalize(XI-Ci);
			float q1=DotProd(Vreln,r);
			if (q1>1.0f) q1=1.0f;
			if (q1<-1.0f) q1=-1.0f;
			float theta=(float)acos(q1);
// refraction solution
// reduce v by decel parameters
			vr *= decel*(1.0f-decelvar*RND01());
// rotate velocity vector
			float maxref,refangle,maxvarref;
			refangle=0.0f;
			if (!FloatEQ0(refvol))
			{	if (refvol>0.0f)
					maxref=HalfPI-theta;
				else 
					maxref=-theta;
				refangle=maxref*(float)fabs(refvol);
				float frefangle=(float)fabs(refangle);
				if (refvol>0.0f)
					maxvarref=theta-frefangle;
				else
					maxvarref=HalfPI-theta-frefangle;
				refangle+=maxvarref*RND11()*refvar;
				Point3 c,d;
				if (theta<0.01f)
				{	d=Point3(RND11(),RND11(),RND11());
					c=Normalize(vr^d);
				}
				else
				{	c=Normalize(r^vr);
				}
				RotateOnePoint(vr,&Zero.x,&c.x,refangle);
				if ((TempDP=DotProd(vr,r))<0.0f) vr=vr-TempDP*r;
			}
			float maxdiff,diffuse,diffvar,diffangle;
			obj->pblock->GetValue(PB_DIFFUSION,t,diffuse,FOREVER);
			obj->pblock->GetValue(PB_DIFFUSIONVAR,t,diffvar,FOREVER);
			maxdiff=HalfPI-theta-refangle;
			if (!FloatEQ0(diffuse))
			{	Point3 d=Point3(RND11(),RND11(),RND11());
				Point3 c=Normalize(vr^d);
				diffangle=0.5f*maxdiff*diffuse*(1.0f+RND11()*diffvar);
				RotateOnePoint(vr,&Zero.x,&c.x,diffangle);
				if (TempDP=(DotProd(vr,r))<0.0f) vr=vr-TempDP*r;
			}
			if ((vinher>0.0f)&&(t>0)){vr=vr+DotProd(Vdt*vinher,r)*r;}
			if (UpdatePastCollide)
			{	pos = XI + (dt-omega)*vr;
				if (ct) (*ct) = dt;
			}
			else
			{	pos = XI;
				if (ct) (*ct) = omega;
			}
		}
		pos = pos*tm;
		vel = VectorTransform(tm,vr);
		obj->lastrnd=rand();
		return TRUE;
	}

// test for spawns only
	float spawnsonly;
	obj->pblock->GetValue(PB_SPAWN,t,spawnsonly,FOREVER);

	if (RND01()<spawnsonly)
	{	obj->pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
		p=pos*invtm; 
		vr=VectorTransform(invtm,vel);
		Vdt=(Vcp-Vc)/dt;
		Vrel=vr-Vdt;
		rsquare=radius*radius;
		rplus=radius;rminus=radius;
		Vreln=Normalize(Vrel);
		if (LengthSquared(p-Vc)>=(rminus*rminus)) //outside
		{	Point3 P1;
			P1=p+dt*Vrel; //second particle position
			if (!(LengthSquared(P1-Vc)<(rplus*rplus)))
			{	float Dist,Dist1;
				Dist=DotProd(Vreln,(Vc-p));
				if (Dist<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Dist1=DotProd(-Vreln,(Vc-P1));
				if (Dist1<0.0f)
				{	obj->lastrnd=rand();
					return FALSE;
				}
				Point3 P10=P1-p,Pc=Vc-p;
				float gamma=(float)acos(DotProd(P10,Pc)/(Length(P10)*Length(Pc)));
				float Dist2=Length(Pc)*(float)cos(HalfPI-gamma);
				if (Dist2>radius)
				{	obj->lastrnd=rand();
					return FALSE;
				}
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
// solve for collision params starts here
			Point3 XI;
			XI=p+omega*vr;
			float passvel,passvelvar;
			obj->pblock->GetValue(PB_PASSVEL,t,passvel,FOREVER);
			obj->pblock->GetValue(PB_PASSVELVAR,t,passvelvar,FOREVER);
			vr*=passvel*(1.0f+passvelvar*RND11());
			pos=XI+(dt-omega)*vr;
		}
		else //inside
		{	Point3 P1;
			P1=p+dt*Vrel;
			if (LengthSquared(P1-Vc)<(rplus*rplus))
			{	obj->lastrnd=rand();
				return FALSE;
			}
			float A,B,C,omega,omega1,omegaend,a2,ptmp,c,d;
			A=LengthSquared(Vrel);
			B=2.0f*DotProd(p,Vrel)-2.0f*DotProd(Vrel,Vc);
			C=(ptmp=LengthSquared(p))+(c=LengthSquared(Vc))-(d=2.0f*DotProd(p,Vc))-rsquare;
			omegaend=B*B-4.0f*A*C;
			if (omegaend<0.0f) omegaend=0.0f;
			else omegaend=(float)sqrt(omegaend);
			a2=2.0f*A;
			omega1=(-B+omegaend)/a2;
			omega=(-B-omegaend)/a2;
			if (((omega1>0.0f)&&(omega1<omega))||((omega<0.0f)&&(omega1>omega))) omega=omega1;
			float fdt=1.1f*dt;
			if ((omega>fdt)||(omega<-fdt))
			{	obj->lastrnd=rand();
				return FALSE;
			}
// solve for collision params starts here
			Point3 XI;
			XI=p+omega*vr;
			float passvel,passvelvar;
			obj->pblock->GetValue(PB_PASSVEL,t,passvel,FOREVER);
			obj->pblock->GetValue(PB_PASSVELVAR,t,passvelvar,FOREVER);
			vr *= passvel*(1.0f+passvelvar*RND11());
			if (UpdatePastCollide)
			{	pos = XI + (dt-omega)*vr;
				if (ct) (*ct) = dt;
			}
			else
			{	pos = XI;
				if (ct) (*ct) = omega;
			}
		}
		pos = pos*tm;
		vel = VectorTransform(tm,vr);
		obj->lastrnd=rand();
		return TRUE;
	}

// nothing happens
	obj->lastrnd=rand();
	return FALSE;
}

class SSpawnDeflPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		SSpawnDeflPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload) {
			DWORD oldVer = ((SSpawnDeflObject*)(cb->targ))->pblock->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<1) {	
 				((SSpawnDeflObject*)targ)->pblock->SetValue(PB_SPAWN,0,1.0f);
 				((SSpawnDeflObject*)targ)->pblock->SetValue(PB_PASSVEL,0,1.0f);
 				((SSpawnDeflObject*)targ)->pblock->SetValue(PB_PASSVELVAR,0,0);
				}
			delete this;
			}
	};

IOResult SSpawnDeflObject::Load(ILoad *iload) 
	{	
	// This is the callback that corrects for any older versions
	// of the parameter block structure found in the MAX file 
	// being loaded.
	iload->RegisterPostLoadCallback(
		new SSpawnDeflPostLoadCallback(
		new ParamBlockPLCB(ssversions,NUM_OLDVERSIONS,&scurVersion,this,0)));
	return IO_OK;
	}


/**********************************************************************
 *<
	FILE: sdyflect.cpp

	DESCRIPTION: spherical dynamics momentum xfer deflector

	CREATED BY: Eric Peterson from Audrey Peterson's SDeflector code

	HISTORY: 7/97

 **********************************************************************/
#include "sflectr.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

static Class_ID SDYNADEFL_CLASS_ID(0x44692f1c, 0x13ca051a);
static Class_ID SDYNADEFLMOD_CLASS_ID(0x70051444, 0x2bf5270a);

const float dymin=0.0001f;

class SDynaDeflObject : public DynamModObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		int lastrnd;
		TimeValue t;
		SDynaDeflObject();
		BOOL SupportsDynamics() {return TRUE;}

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		void MapKeys(TimeMap *map,DWORD flags);
		Class_ID ClassID() {return SDYNADEFL_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_SDYNADEFLECTOR);}
				
		// from object		
		CreateMouseCallBack* GetCreateMouseCallBack();		

		// From BaseObject
		IParamArray *GetParamBlock() {return pblock;}
		int GetParamBlockIndex(int id) {return id;}

		// From SimpleWSMObject		
		void InvalidateUI();		
		void BuildMesh(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);		
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);		
		FlectForces ForceData(TimeValue t);
		FlectForces ffdata;	
		TimeValue ctime;
};

//--- ClassDescriptor and class vars ---------------------------------

IObjParam *SDynaDeflObject::ip        = NULL;
IParamMap *SDynaDeflObject::pmapParam = NULL;
HWND       SDynaDeflObject::hSot      = NULL;

class SDynaDeflClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new SDynaDeflObject;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_SDYNADEFLECTOR);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return SDYNADEFL_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_DYNAMICS);}
	};

static SDynaDeflClassDesc SDynaDeflDesc;
ClassDesc* GetSDynaDeflObjDesc() {return &SDynaDeflDesc;}

//--- DeflectMod -----------------------------------------------------

class SDynaDeflField : public CollisionObject {
	public:		
		SDynaDeflObject *obj;
		INode *node;
		Matrix3 tm, invtm,tp;
		Interval tmValid;
		Point3 totalforce,applyat;
		int totalnumber;
		TimeValue curtime;
		Point3 Vc,Vcp;
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class SDynaDeflMod : public SimpleWSMMod {
	public:				
		SDynaDeflField deflect;

		SDynaDeflMod() {deflect.curtime=NoAni;}
		SDynaDeflMod(INode *node,SDynaDeflObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_EP_SDYNADEFLECTORMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return SDYNADEFLMOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_SDYNADEFLECTORBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class SDynaDeflModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new SDynaDeflMod;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_SDYNADEFLECTORMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return SDYNADEFLMOD_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};

static SDynaDeflModClassDesc SDynaDeflModDesc;
ClassDesc* GetSDynaDeflModDesc() {return &SDynaDeflModDesc;}

//--- SphereFlectorObject Parameter map/block descriptors ------------------

#define PB_TIMEON		0
#define PB_TIMEOFF		1
#define PB_AFFECTS		2
#define PB_BOUNCE		3
#define PB_BOUNCEVAR	4
#define PB_CHAOS		5
#define PB_INHERIT		6
#define PB_RADIUS		7
#define PB_MASS			8
#define PB_MASSUNITS	9
//#define PB_FORCEX		10
//#define PB_FORCEY		11
//#define PB_FORCEZ		12
//#define PB_APPLYX		13
//#define PB_APPLYY		14
//#define PB_APPLYZ		15
//#define PB_NUMBER		16

static int massSunitsIDs[] = {IDC_EP_MASSGM,IDC_EP_MASSKG,IDC_EP_MASSLBM};

static ParamUIDesc descSDynaDeflParam[] = {
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
		
	// Radius
	ParamUIDesc(
		PB_RADIUS,
		EDITTYPE_UNIVERSE,
		IDC_EP_ICONSIZE,IDC_EP_ICONSIZESPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Particle Mass
	ParamUIDesc(
		PB_MASS,
		EDITTYPE_FLOAT,
		IDC_EP_PARTICLEMASS,IDC_EP_PARTICLEMASSSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Particle Mass Units
	ParamUIDesc(PB_MASSUNITS,TYPE_RADIO,massSunitsIDs,3)

	};

#define PARAMDESC_LENGTH	10

ParamBlockDescID SDynaDefldescVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },//time on	
	{ TYPE_INT, NULL, TRUE, 1 },	//time off
	{ TYPE_FLOAT, NULL, TRUE, 2 },	//affects
	{ TYPE_FLOAT, NULL, TRUE, 3 },//bounce
	{ TYPE_FLOAT, NULL, TRUE, 4 },//bounce var
	{ TYPE_FLOAT, NULL, TRUE, 5 },//chaos
	{ TYPE_FLOAT, NULL, TRUE, 6 },//vel inherit
	{ TYPE_FLOAT, NULL, TRUE, 7 },//radius
	{ TYPE_FLOAT, NULL, TRUE, 8 },//mass
	{ TYPE_INT, NULL, FALSE, 9 },//mass units
	{ TYPE_FLOAT, NULL, FALSE, 10 },//forcex
	{ TYPE_FLOAT, NULL, FALSE, 11 },
	{ TYPE_FLOAT, NULL, FALSE, 12 },
	{ TYPE_FLOAT, NULL, FALSE, 13 },//apply at x
	{ TYPE_FLOAT, NULL, FALSE, 14 },
	{ TYPE_FLOAT, NULL, FALSE, 15 },
	{ TYPE_INT, NULL, FALSE, 16 } //total number
};	

#define PBLOCK_LENGTH	17

#define CURRENT_VERSION	0


//--- Deflect object methods -----------------------------------------

SDynaDeflObject::SDynaDeflObject()
{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(SDynaDefldescVer0, PBLOCK_LENGTH, CURRENT_VERSION));
	assert(pblock);	

	pblock->SetValue(PB_TIMEON,0,0);
	pblock->SetValue(PB_TIMEOFF,0,100*GetTicksPerFrame());
	pblock->SetValue(PB_AFFECTS,0,100.0f);
	pblock->SetValue(PB_BOUNCE,0,1.0f);
	pblock->SetValue(PB_BOUNCEVAR,0,0.0f);
	pblock->SetValue(PB_CHAOS,0,0.0f);
	pblock->SetValue(PB_INHERIT,0,1.0f);
	pblock->SetValue(PB_MASS,0,1.0f);
	pblock->SetValue(PB_MASSUNITS,0,0);
	srand(lastrnd=12345);
	ffdata.FlectForce=Zero;ffdata.ApplyAt=Zero;ffdata.Num=0;
	t=99999;
	ctime=99999;
}

Modifier *SDynaDeflObject::CreateWSMMod(INode *node)
	{
	return new SDynaDeflMod(node,this);
	}

RefTargetHandle SDynaDeflObject::Clone(RemapDir& remap) 
	{
	SDynaDeflObject* newob = new SDynaDeflObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
	}

void SDynaDeflObject::BeginEditParams(
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
			MAKEINTRESOURCE(IDD_SW_DESC_BOTH),
			DefaultSOTProc,
			GetString(IDS_EP_TOP), 
			(LPARAM)ip,APPENDROLL_CLOSED);

		// Gotta make a new one.
		pmapParam = CreateCPParamMap(
			descSDynaDeflParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_AP_SDYNADEFL),
			GetString(IDS_EP_PARAMETERS),
			0);
		}
	}

void SDynaDeflObject::EndEditParams(
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

void SDynaDeflObject::MapKeys(TimeMap *map,DWORD flags)
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

void SDynaDeflObject::BuildMesh(TimeValue t)
{	ivalid = FOREVER;
	float r,r2,r3,r4;
	pblock->GetValue(PB_RADIUS,t,r,ivalid);
	float u;
	#define NUM_SEGS	24
	 r2=0.5f*r;
	 r3=0.15f*r2;
	 r4=0.25f*r2;

	mesh.setNumVerts(3*NUM_SEGS+16);
	mesh.setNumFaces(3*NUM_SEGS+9);

	for (int i=0; i<NUM_SEGS; i++)
	{	u=float(i)/float(NUM_SEGS) * TWOPI;
		mesh.setVert(i, Point3((float)cos(u) * r, (float)sin(u) * r, 0.0f));
	}
	for (i=0; i<NUM_SEGS; i++)
	{	u=float(i)/float(NUM_SEGS) * TWOPI;
		mesh.setVert(i+NUM_SEGS, Point3(0.0f, (float)cos(u) * r, (float)sin(u) * r));
	}
	for (i=0; i<NUM_SEGS; i++)
	{	u=float(i)/float(NUM_SEGS) * TWOPI;
		mesh.setVert(i+2*NUM_SEGS, Point3((float)cos(u) * r, 0.0f, (float)sin(u) * r));
	}		

	mesh.setVert(72, Point3(0.0f,0.0f,0.0f));

	mesh.setVert(73, Point3(0.0f,0.0f,  r ));
	mesh.setVert(74, Point3(0.0f, r2 ,r+r2));
	mesh.setVert(75, Point3(0.0f,-r2 ,r+r2));
	mesh.setVert(76, Point3(0.0f, r2+r3,r+r2));
	mesh.setVert(77, Point3(0.0f, r2,r+r2+r3));
	mesh.setVert(78, Point3(0.0f,-r2,r+r2-r3));
	mesh.setVert(79, Point3(0.0f,-r2+r3,r+r2));

	mesh.setVert(80, Point3(0.0f, r4   ,-r ));
	mesh.setVert(81, Point3(0.0f, r4   ,-r-r2));
	mesh.setVert(82, Point3(0.0f, r4+r3,-r-r2));
	mesh.setVert(83, Point3(0.0f,0.0f  ,-r-r2-r3-r3));
	mesh.setVert(84, Point3(0.0f,-r4-r3,-r-r2));
	mesh.setVert(85, Point3(0.0f,-r4   ,-r-r2));
	mesh.setVert(86, Point3(0.0f,-r4   ,-r));
	mesh.setVert(87, Point3(0.0f,0.0f  ,-r-r4));
	
	for (i=0; i<3*NUM_SEGS; i++)
	{	int i1 = i+1;
		if (i1%NUM_SEGS==0) i1 -= NUM_SEGS;
		mesh.faces[i].setEdgeVisFlags(1,0,0);
		mesh.faces[i].setSmGroup(1);
		mesh.faces[i].setVerts(i,i1,3*NUM_SEGS);
	}

	mesh.faces[72].setEdgeVisFlags(1,0,1);
	mesh.faces[72].setSmGroup(1);
	mesh.faces[72].setVerts(73,75,74);	
	mesh.faces[73].setEdgeVisFlags(1,0,1);
	mesh.faces[73].setSmGroup(1);
	mesh.faces[73].setVerts(75,78,79);	
	mesh.faces[74].setEdgeVisFlags(1,0,1);
	mesh.faces[74].setSmGroup(1);
	mesh.faces[74].setVerts(74,77,76);

	mesh.faces[75].setEdgeVisFlags(1,0,1);
	mesh.faces[75].setSmGroup(1);
	mesh.faces[75].setVerts(80,81,87);	
	mesh.faces[76].setEdgeVisFlags(0,0,0);
	mesh.faces[76].setSmGroup(1);
	mesh.faces[76].setVerts(81,85,87);	
	mesh.faces[77].setEdgeVisFlags(1,1,0);
	mesh.faces[77].setSmGroup(1);
	mesh.faces[77].setVerts(85,86,87);	
	mesh.faces[78].setEdgeVisFlags(1,1,0);
	mesh.faces[78].setSmGroup(1);
	mesh.faces[78].setVerts(81,82,83);	
	mesh.faces[79].setEdgeVisFlags(0,0,0);
	mesh.faces[79].setSmGroup(1);
	mesh.faces[79].setVerts(81,83,85);	
	mesh.faces[80].setEdgeVisFlags(1,1,0);
	mesh.faces[80].setSmGroup(1);
	mesh.faces[80].setVerts(83,84,85);	

	mesh.InvalidateGeomCache();
/*	int j;
	for (i=0;i<88;i++)
	{	if ((mesh.verts[i])||(!mesh.verts[i]))
		{	j++;	}
	}*/ //debugging code only
}


class SDynaDeflObjCreateCallback : public CreateMouseCallBack {
	public:
		SDynaDeflObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int SDynaDeflObjCreateCallback::proc(
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
				ob->pblock->SetValue(PB_RADIUS,0,0.01f);
				ob->pmapParam->Invalidate();
				break;

			case 1: {
				sp1 = m;
				#ifdef _3D_CREATE	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				ob->pblock->SetValue(PB_RADIUS,0,Length(p1-p0));
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

static SDynaDeflObjCreateCallback SDynaDeflCreateCB;

CreateMouseCallBack* SDynaDeflObject::GetCreateMouseCallBack()
	{
	SDynaDeflCreateCB.ob = this;
	return &SDynaDeflCreateCB;
	}

void SDynaDeflObject::InvalidateUI() 
{	if (pmapParam) pmapParam->Invalidate();
}

ParamDimension *SDynaDeflObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {		
		case PB_TIMEON:
				 return stdTimeDim;
		case PB_TIMEOFF:
				 return stdTimeDim;
		case PB_AFFECTS:
				 return stdPercentDim;
		case PB_BOUNCEVAR:
				 return stdPercentDim;
		case PB_CHAOS:
				 return stdPercentDim;
		default: return defaultDim;
		}
	}

TSTR SDynaDeflObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {				
		case PB_TIMEON: 	return GetString(IDS_EP_TIMEON);
		case PB_TIMEOFF:	return GetString(IDS_EP_TIMEOFF);
		case PB_AFFECTS:	return GetString(IDS_EP_AFFECTS);
		case PB_BOUNCE:		return GetString(IDS_EP_BOUNCE);
		case PB_BOUNCEVAR:	return GetString(IDS_EP_BOUNCEVAR);
		case PB_CHAOS:		return GetString(IDS_EP_CHAOS);
		case PB_INHERIT:	return GetString(IDS_EP_INHERIT);
		case PB_RADIUS:		return GetString(IDS_EP_RADIUS);
		case PB_MASS:		return GetString(IDS_EP_MASS);
		case PB_MASSUNITS:	return GetString(IDS_EP_MASSUNITS);
		default: 			return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

SDynaDeflMod::SDynaDeflMod(INode *node,SDynaDeflObject *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval SDynaDeflMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		SDynaDeflObject *obj = (SDynaDeflObject*)GetWSMObject(t);
		obj->pblock->GetValue(PB_TIMEOFF,t,f,valid);
		obj->pblock->GetValue(PB_AFFECTS,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCE,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCEVAR,t,f,valid);
		obj->pblock->GetValue(PB_CHAOS,t,f,valid);
		obj->pblock->GetValue(PB_INHERIT,t,f,valid);
		obj->pblock->GetValue(PB_RADIUS,t,f,valid);
		obj->pblock->GetValue(PB_MASS,t,f,valid);
		tm=nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class SDynaDeflDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static SDynaDeflDeformer SDynaDeflddeformer;

Deformer& SDynaDeflMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return SDynaDeflddeformer;
	}

RefTargetHandle SDynaDeflMod::Clone(RemapDir& remap) 
{	SDynaDeflMod *newob = new SDynaDeflMod(nodeRef,(SDynaDeflObject*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
}


void SDynaDeflMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (SDynaDeflObject*)GetWSMObject(t);
		deflect.node = nodeRef;
		deflect.tmValid.SetEmpty();		
		deflect.totalforce=Zero;
		deflect.applyat=Zero;
		deflect.totalnumber=0;
		if (t<=deflect.obj->t) deflect.obj->lastrnd=12345;
		deflect.obj->t=t;
		TimeValue tmpt=GetCOREInterface()->GetTime();
		if (deflect.obj->ctime!=tmpt)
		{ deflect.obj->ctime=tmpt;
		  deflect.obj->ffdata.FlectForce=deflect.totalforce;
		  deflect.obj->ffdata.ApplyAt=deflect.applyat;
		  deflect.obj->ffdata.Num=deflect.totalnumber;
		}
//		deflect.curtime=NoAni;
		obj->ApplyCollisionObject(&deflect);
		}
	}

Object *SDynaDeflField::GetSWObject()
{ return obj;
}

BOOL SDynaDeflField::CheckCollision(TimeValue t,Point3 &pos,Point3 &vel,float dt,int index,float *ct,BOOL UpdatePastCollide)
{	if (!tmValid.InInterval(t))
	{	tmValid=FOREVER;
		tm=node->GetObjectTM(t,&tmValid);
		invtm=Inverse(tm);
		Interval tmpValid=FOREVER;
		tp=node->GetObjectTM(t+(int)dt,&tmpValid);
		Vc=Zero;
		Vcp=Zero*tp*invtm;
	}
	
    if (curtime!=t)
	{	totalforce=Zero;
		applyat=Zero;
		totalnumber=0;
		curtime=t;
//		obj->ffdata.FlectForce=totalforce;
//		obj->ffdata.ApplyAt=applyat;
//		obj->ffdata.Num=totalnumber;
	}
	float K=(float)GetMasterScale(UNITS_CENTIMETERS);
	float stepsize=dt;
	Point3 invel=vel;

	TimeValue startt,endt;
	obj->pblock->GetValue(PB_TIMEON,t,startt,FOREVER);
	obj->pblock->GetValue(PB_TIMEOFF,t,endt,FOREVER);
	if ((t<startt)||(t>endt))
	{	obj->lastrnd=rand();
		return FALSE;
	}

	float affectsthisportion;
	obj->pblock->GetValue(PB_AFFECTS,t,affectsthisportion,FOREVER);
    srand(obj->lastrnd);
	if (RND01()>affectsthisportion)
	{	obj->lastrnd=rand();
		return FALSE;
	}
	
	float bounce,bvar,chaos,vinher;
	obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
	obj->pblock->GetValue(PB_BOUNCEVAR,t,bvar,FOREVER);
	obj->pblock->GetValue(PB_CHAOS,t,chaos,FOREVER);
	obj->pblock->GetValue(PB_INHERIT,t,vinher,FOREVER);

	float radius;
	obj->pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
	Point3 p,vr;
	p=pos*invtm; 
	vr=VectorTransform(invtm,vel);
	Point3 Vdt, XI;
	Vdt=(Vcp-Vc)/dt;
	Point3 Vrel=vr-Vdt;
	float rsquare=radius*radius;
	float omega;
	float rplus=radius,rminus=radius;
	Point3 Vreln=Normalize(Vrel);
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
		float A,B,C,omega1,omegaend,a2,ptmp,c,d;
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
		Point3 r,n;
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
		if (UpdatePastCollide)
		{	pos = XI + (dt-omega)*vr;
		}
		else
			pos = XI;
	}
	else //inside
	{	Point3 P1;
		P1=p+dt*Vrel;
		if (LengthSquared(P1-Vc)<(rplus*rplus))
		{	obj->lastrnd=rand();
			return FALSE;
		}
		float A,B,C,omega1,omegaend,a2,ptmp,c,d;
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
		Point3 r,n;
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
		}
		else
			pos = XI;
	}
	pos = pos*tm;
	applyat = XI*tm;
	vel = VectorTransform(tm,vr);
	if (UpdatePastCollide)
	{ if (ct) (*ct) = dt;}
	else
	{ if (ct) (*ct) = omega;}

// get physical parameters
	float mass;
	int massunits;
	obj->pblock->GetValue(PB_MASS,t,mass,FOREVER);
	obj->pblock->GetValue(PB_MASSUNITS,t,massunits,FOREVER);
// compensate for units of measure
	switch(massunits)
	{	case 0: mass*=0.001f; break;
		case 1: break;
		case 2: mass*=0.454f; break;
	}
// increment physical property params
// put information into parameter block
	if (t==obj->ctime)
	{ totalnumber+=1;
	  totalforce+=(invel-vel)*K*mass/stepsize;
      obj->ffdata.FlectForce+=totalforce;
	  obj->ffdata.ApplyAt+=applyat;
	  obj->ffdata.Num=+totalnumber;
	}
	obj->lastrnd=rand();
	return TRUE;
}

FlectForces SDynaDeflObject::ForceData(TimeValue t)
{ pblock->GetValue(PB_TIMEON,t,ffdata.t1,FOREVER);
  pblock->GetValue(PB_TIMEOFF,t,ffdata.t2,FOREVER);
  return ffdata;
}

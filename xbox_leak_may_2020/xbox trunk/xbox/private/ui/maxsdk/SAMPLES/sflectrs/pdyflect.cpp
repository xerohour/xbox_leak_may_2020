/**********************************************************************
 *<
	FILE: pdyflect.cpp

	DESCRIPTION: A Dynamics-linked Momentum transfer planar deflector

	CREATED BY: Audrey and Eric Peterson from Rolf Berteig's Deflector

	HISTORY: 7-17-97

 **********************************************************************/
#include "sflectr.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

static Class_ID PDYNADEF_CLASS_ID(0xb46c87, 0x3eee2ac4);
static Class_ID PDYNADEFMOD_CLASS_ID(0x783f281d, 0xade1abe);

const float dymin=0.0001f;

class PDynaDeflObj : public DynamModObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		int lastrnd;
		TimeValue t;
		PDynaDeflObj();
		BOOL SupportsDynamics() {return TRUE;}

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		void MapKeys(TimeMap *map,DWORD flags);
		Class_ID ClassID() {return PDYNADEF_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_PDYNADEF);}
				
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

IObjParam *PDynaDeflObj::ip        = NULL;
IParamMap *PDynaDeflObj::pmapParam = NULL;
HWND       PDynaDeflObj::hSot      = NULL;

class PDynaDeflClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new PDynaDeflObj;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_PDYNADEF);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return PDYNADEF_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_DYNAMICS);}
	};

static PDynaDeflClassDesc pspawndeflDesc;
ClassDesc* GetPDynaDeflObjDesc() {return &pspawndeflDesc;}

//--- DeflectMod -----------------------------------------------------

class PDynaDeflField : public CollisionObject {
	public:		
		PDynaDeflObj *obj;
		INode *node;
		Matrix3 tm,invtm,tp;
		Interval tmValid;
		Point3 totalforce,applyat;
		int totalnumber;
		TimeValue curtime;
		Point3 Vc,Vcp;
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index, float *ct, BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class PDynaDeflMod : public SimpleWSMMod {
	public:				
		PDynaDeflField deflect;

		PDynaDeflMod() {deflect.curtime=NoAni;}
		PDynaDeflMod(INode *node,PDynaDeflObj *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_EP_PDYNADEFMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return PDYNADEFMOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_EP_PDYNADEFMODBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class PDynaDeflModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new PDynaDeflMod;}
	const TCHAR *	ClassName() {return GetString(IDS_EP_PDYNADEFMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return PDYNADEFMOD_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};

static PDynaDeflModClassDesc pspawndeflModDesc;
ClassDesc* GetPDynaDeflModDesc() {return &pspawndeflModDesc;}

//--- DeflectObject Parameter map/block descriptors ------------------

#define PB_TIMEON		0
#define PB_TIMEOFF		1
#define PB_AFFECTS		2
#define PB_BOUNCE		3
#define PB_BOUNCEVAR	4
#define PB_CHAOS		5
#define PB_INHERIT		6
#define PB_ICONSIZE		7
#define PB_ICONSIZE1	8
#define PB_MASS			9
#define PB_MASSUNITS	10
#define PB_FORCEX		11
#define PB_FORCEY		12
#define PB_FORCEZ		13
#define PB_APPLYX		14
#define PB_APPLYY		15
#define PB_APPLYZ		16
#define PB_NUMBER		17

static int massunitsIDs[] = {IDC_EP_MASSGM,IDC_EP_MASSKG,IDC_EP_MASSLBM};

static ParamUIDesc descPDynaDeflParam[] = {
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
		SPIN_AUTOSCALE),

	// Particle Mass
	ParamUIDesc(
		PB_MASS,
		EDITTYPE_FLOAT,
		IDC_EP_PARTICLEMASS,IDC_EP_PARTICLEMASSSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Particle Mass Units
	ParamUIDesc(PB_MASSUNITS,TYPE_RADIO,massunitsIDs,3)

	};

#define PARAMDESC_LENGTH	11

ParamBlockDescID descPDynaDeflVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },//time on
	{ TYPE_INT, NULL, TRUE, 1 },//time off
	{ TYPE_FLOAT, NULL, TRUE, 2 },//affects
	{ TYPE_FLOAT, NULL, TRUE, 3 },//bounce
	{ TYPE_FLOAT, NULL, TRUE, 4 },//bounce var
	{ TYPE_FLOAT, NULL, TRUE, 5 },//chaos
	{ TYPE_FLOAT, NULL, TRUE, 6 },//vel inherit
	{ TYPE_FLOAT, NULL, TRUE, 7 },//icon size 1
	{ TYPE_FLOAT, NULL, TRUE, 8 },//icon size 2
	{ TYPE_FLOAT, NULL, TRUE, 9 },//mass
	{ TYPE_INT, NULL, FALSE, 10 },//mass units
	{ TYPE_FLOAT, NULL, FALSE, 11 },
	{ TYPE_FLOAT, NULL, FALSE, 12 },
	{ TYPE_FLOAT, NULL, FALSE, 13 },
	{ TYPE_FLOAT, NULL, FALSE, 14 },
	{ TYPE_FLOAT, NULL, FALSE, 15 },
	{ TYPE_FLOAT, NULL, FALSE, 16 },
	{ TYPE_INT, NULL, FALSE, 17 }//total number
	};
#define PBLOCK_LENGTH	18

#define CURRENT_VERSION	0


//--- PDynaDeflector object methods -----------------------------------------


PDynaDeflObj::PDynaDeflObj()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descPDynaDeflVer0, PBLOCK_LENGTH, CURRENT_VERSION));
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
	ffdata.FlectForce=Zero;ffdata.ApplyAt=Zero;ffdata.Num=0;
	srand(lastrnd=12345);
	t=99999;
	ctime=99999;
}

Modifier *PDynaDeflObj::CreateWSMMod(INode *node)
{	return new PDynaDeflMod(node,this);
}

RefTargetHandle PDynaDeflObj::Clone(RemapDir& remap) 
{	PDynaDeflObj* newob = new PDynaDeflObj();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
}

void PDynaDeflObj::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{	SimpleWSMObject::BeginEditParams(ip,flags,prev);
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
			descPDynaDeflParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_AP_PDYNADEFL),
			GetString(IDS_EP_PARAMETERS),
			0);
		}
	}

void PDynaDeflObj::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
	{		
	SimpleWSMObject::EndEditParams(ip,flags,next);
	this->ip = NULL;

	if (flags&END_EDIT_REMOVEUI ) {		
		DestroyCPParamMap(pmapParam);
		ip->DeleteRollupPage(hSot);
		pmapParam = NULL;		
		}	
	}

void PDynaDeflObj::MapKeys(TimeMap *map,DWORD flags)
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

void PDynaDeflObj::BuildMesh(TimeValue t)
	{
	float w, h;
	float w2,h2,h3,h4;
	ivalid = FOREVER;
	pblock->GetValue(PB_ICONSIZE,t,w,ivalid);
	pblock->GetValue(PB_ICONSIZE1,t,h,ivalid);
	w*=0.5f;
	w2=w*0.5f;
	h*=0.5f;
	h2=h*0.5f;
	h3=h2*0.15f;
	h4=h2*0.25f;

	mesh.setNumVerts(19);
	mesh.setNumFaces(11);
	mesh.setVert(0, Point3(-w,-h, 0.0f));
	mesh.setVert(1, Point3( w,-h, 0.0f));
	mesh.setVert(2, Point3( w, h, 0.0f));
	mesh.setVert(3, Point3(-w, h, 0.0f));

	mesh.setVert( 4, Point3(0.0f,0.0f,0.0f));
	mesh.setVert( 5, Point3(0.0f,  h2,  h2));
	mesh.setVert( 6, Point3(0.0f, -h2,  h2));
	mesh.setVert( 7, Point3(0.0f,  h2+h3,  h2));
	mesh.setVert( 8, Point3(0.0f,  h2,  h2+h3));
	mesh.setVert( 9, Point3(0.0f, -h2,  h2-h3));
	mesh.setVert(10, Point3(0.0f, -h2+h3,  h2));

	mesh.setVert(11, Point3(0.0f, h4, 0.0f));
	mesh.setVert(12, Point3(0.0f, h4, -h2));
	mesh.setVert(13, Point3(0.0f, h4+h3, -h2));
	mesh.setVert(14, Point3(0.0f, 0.0f, -h2-h3-h3));
	mesh.setVert(15, Point3(0.0f,-h4-h3, -h2));
	mesh.setVert(16, Point3(0.0f,-h4, -h2));
	mesh.setVert(17, Point3(0.0f,-h4, 0.0f));
	mesh.setVert(18, Point3(0.0f,0.0f,-h4));
	
	mesh.faces[0].setEdgeVisFlags(1,1,0);
	mesh.faces[0].setSmGroup(1);
	mesh.faces[0].setVerts(0,1,2);
	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(1);
	mesh.faces[1].setVerts(2,3,0);	

	mesh.faces[2].setEdgeVisFlags(1,0,1);
	mesh.faces[2].setSmGroup(1);
	mesh.faces[2].setVerts(4,6,5);	
	mesh.faces[3].setEdgeVisFlags(1,0,1);
	mesh.faces[3].setSmGroup(1);
	mesh.faces[3].setVerts(6,9,10);	
	mesh.faces[4].setEdgeVisFlags(1,0,1);
	mesh.faces[4].setSmGroup(1);
	mesh.faces[4].setVerts(5,8,7);
	mesh.faces[5].setEdgeVisFlags(1,0,1);
	mesh.faces[5].setSmGroup(1);
	mesh.faces[5].setVerts(11,12,18);	
	mesh.faces[6].setEdgeVisFlags(0,0,0);
	mesh.faces[6].setSmGroup(1);
	mesh.faces[6].setVerts(12,16,18);	
	mesh.faces[7].setEdgeVisFlags(1,1,0);
	mesh.faces[7].setSmGroup(1);
	mesh.faces[7].setVerts(16,17,18);	
	mesh.faces[8].setEdgeVisFlags(1,1,0);
	mesh.faces[8].setSmGroup(1);
	mesh.faces[8].setVerts(12,13,14);	
	mesh.faces[9].setEdgeVisFlags(0,0,0);
	mesh.faces[9].setSmGroup(1);
	mesh.faces[9].setVerts(12,14,16);	
	mesh.faces[10].setEdgeVisFlags(1,1,0);
	mesh.faces[10].setSmGroup(1);
	mesh.faces[10].setVerts(14,15,16);	

	mesh.InvalidateGeomCache();	
	}


class PDynaDeflObjCreateCallback : public CreateMouseCallBack {
	public:
		PDynaDeflObj *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int PDynaDeflObjCreateCallback::proc(
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
static PDynaDeflObjCreateCallback pspawndeflCreateCB;

CreateMouseCallBack* PDynaDeflObj::GetCreateMouseCallBack()
	{
	pspawndeflCreateCB.ob = this;
	return &pspawndeflCreateCB;
	}

void PDynaDeflObj::InvalidateUI() 
	{
	if (pmapParam) pmapParam->Invalidate();
	}

ParamDimension *PDynaDeflObj::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {		
		case PB_TIMEON:		return stdTimeDim;
		case PB_TIMEOFF:	return stdTimeDim;
		case PB_AFFECTS:	return stdPercentDim;
		case PB_BOUNCEVAR:	return stdPercentDim;
		case PB_CHAOS:		return stdPercentDim;
		default:			return defaultDim;
		}
	}

TSTR PDynaDeflObj::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {				
		case PB_TIMEON:			return GetString(IDS_EP_TIMEON);
		case PB_TIMEOFF:		return GetString(IDS_EP_TIMEOFF);
		case PB_AFFECTS:		return GetString(IDS_EP_AFFECTS);
		case PB_BOUNCE:			return GetString(IDS_EP_BOUNCE);
		case PB_BOUNCEVAR:		return GetString(IDS_EP_BOUNCEVAR);
		case PB_CHAOS:			return GetString(IDS_EP_CHAOS);
		case PB_INHERIT:		return GetString(IDS_EP_INHERIT);
		case PB_ICONSIZE:		return GetString(IDS_EP_WIDTH);
		case PB_ICONSIZE1:		return GetString(IDS_EP_HEIGHT);
		case PB_MASS:			return GetString(IDS_EP_MASS);
		case PB_MASSUNITS:		return GetString(IDS_EP_MASSUNITS);
		default: 				return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

PDynaDeflMod::PDynaDeflMod(INode *node,PDynaDeflObj *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval PDynaDeflMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		PDynaDeflObj *obj = (PDynaDeflObj*)GetWSMObject(t);
		TimeValue TempT;
		obj->pblock->GetValue(PB_TIMEOFF,t,TempT,valid);
		obj->pblock->GetValue(PB_AFFECTS,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCE,t,f,valid);
		obj->pblock->GetValue(PB_BOUNCEVAR,t,f,valid);
		obj->pblock->GetValue(PB_CHAOS,t,f,valid);
		obj->pblock->GetValue(PB_INHERIT,t,f,valid);
		obj->pblock->GetValue(PB_ICONSIZE,t,f,valid);
		obj->pblock->GetValue(PB_ICONSIZE1,t,f,valid);
		obj->pblock->GetValue(PB_MASS,t,f,valid);
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class PDynaDeflDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static PDynaDeflDeformer pspawndeformer;

Deformer& PDynaDeflMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return pspawndeformer;
	}

RefTargetHandle PDynaDeflMod::Clone(RemapDir& remap) 
	{
	PDynaDeflMod *newob = new PDynaDeflMod(nodeRef,(PDynaDeflObj*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
	}


void PDynaDeflMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (PDynaDeflObj*)GetWSMObject(t);
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

Object *PDynaDeflField::GetSWObject()
{ return obj;
}

BOOL PDynaDeflField::CheckCollision(TimeValue t,Point3 &pos,Point3 &vel,float dt,int index, float *ct, BOOL UpdatePastCollide)
{	if (!tmValid.InInterval(t))
	{	tmValid=FOREVER;
		tm=node->GetObjectTM(t,&tmValid);
		invtm=Inverse(tm);
		Interval tmpValid=FOREVER;
		tp=node->GetObjectTM(t-(int)dt,&tmpValid);
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
	float stepsize=dt,intimestep=dt;
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

	float width,height;
	obj->pblock->GetValue(PB_ICONSIZE,t,width,FOREVER);
	obj->pblock->GetValue(PB_ICONSIZE1,t,height,FOREVER);
	width *=0.5f;
	height*=0.5f;

	float bounce,bouncevar,chaos,inherit;
	obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
	obj->pblock->GetValue(PB_BOUNCEVAR,t,bouncevar,FOREVER);
	obj->pblock->GetValue(PB_CHAOS,t,chaos,FOREVER);
	obj->pblock->GetValue(PB_INHERIT,t,inherit,FOREVER);
	float at;
	Point3 p,v,ph;
	p=pos*invtm; 
	v=VectorTransform(invtm,vel);
	Point3 vdt=(Vc-Vcp)/dt;
	Point3 vrel=v-vdt;

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
// use relative speeds to check for hit
	ph=p+at*vrel;
// See if the point is within our range
	if ((ph.x<-width)||(ph.x>width)||(ph.y<-height)||(ph.y>height))
	{	obj->lastrnd=rand();
		return FALSE;
	}

// if we're here, we have a collision
// Remove the part of dt we used to get to the collision point
	float holddt = dt;
	dt -= at;
// go back to particle speed to find location of hit
	ph = p + at*v;
// save as the hit location in wcs before allowing for remaining travel
	applyat=ph*tm;
// Reflect the velocity about the XY plane and attenuate with the bounce factor
	BOOL ispos=(v.z>0.0f);
	v.z=-v.z;
// reduce v by bounce parameters
	v*=bounce*(1.0f-bouncevar*RND01());
// rotate velocity vector to give us chaos
	if (!FloatEQ0(chaos))
	{	float theta=(float)acos(DotProd(Normalize(v),Point3(0.0f,0.0f,1.0f)));
		theta=(HalfPI-theta)*chaos*RND01();
		Point3 d=Point3(RND11(),RND11(),RND11());
		Point3 c=Normalize(v^d);
		RotateOnePoint(v,&Zero.x,&c.x,theta);
		if (ispos==(v.z>0.0f)) v.z=-v.z;
	}
// Tranform back into world space.
	pos = ph*tm;
	vel = VectorTransform(tm,v);
// find absolute motion of deflector and add inheritance
	Point3 tabs2,tabs1,vabsdefl;
	tabs2 = tm.GetRow(3);
	tabs1 = tp.GetRow(3);
	vabsdefl = (tabs2-tabs1)/intimestep;
	vel += inherit*vabsdefl;
// use up the rest of the time available to find the final position
//	pos+=vel*dt;

	if (UpdatePastCollide)
	{	pos += vel*dt;
		if (ct) (*ct) = holddt;
	}
	else
	{	if (ct) (*ct) = at;
	}

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
FlectForces PDynaDeflObj::ForceData(TimeValue t)
{ pblock->GetValue(PB_TIMEON,t,ffdata.t1,FOREVER);
  pblock->GetValue(PB_TIMEOFF,t,ffdata.t2,FOREVER);
  return ffdata;
}

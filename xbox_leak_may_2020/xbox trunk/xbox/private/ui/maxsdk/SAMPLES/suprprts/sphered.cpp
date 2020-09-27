/**********************************************************************
 *<
	FILE: sphered.cpp

	DESCRIPTION: A simple spherical deflector object for particles

	CREATED BY: Audrey Peterson

	HISTORY: 19November 96

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/
#include "suprprts.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

static Class_ID SPHEREDEF_CLASS_ID(0x6cbd289d, 0x3fef6656);
static Class_ID SPHEREDEFMOD_CLASS_ID(0x5cdf4181, 0x4c5b42f9);

class SphereDefObject : public SimpleWSMObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
		int lastrnd;
		TimeValue t;
					
		SphereDefObject();

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		Class_ID ClassID() {return SPHEREDEF_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_SPHDEFLECTOR);}
				
		// from object		
		CreateMouseCallBack* GetCreateMouseCallBack();		
		
		// From SimpleWSMObject		
		void InvalidateUI();		
		void BuildMesh(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);		
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);		
	};

//--- ClassDescriptor and class vars ---------------------------------

IObjParam *SphereDefObject::ip        = NULL;
IParamMap *SphereDefObject::pmapParam = NULL;
HWND       SphereDefObject::hSot      = NULL;

class SphereDeflectorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new SphereDefObject;}
	const TCHAR *	ClassName() {return GetString(IDS_AP_SPHDEFLECTOR_CLASS);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return SPHEREDEF_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_PARTICLES);}
	};

static SphereDeflectorClassDesc SpheredeflectDesc;
ClassDesc* GetSphereDefDesc() {return &SpheredeflectDesc;}

//--- DeflectMod -----------------------------------------------------

class SphereDeflectorField : public CollisionObject {
	public:		
		SphereDefObject *obj;
		INode *node;
		Matrix3 tm, invtm,tp;
		Interval tmValid;
		Point3 Vc,Vcp;
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class SphereDeflectMod : public SimpleWSMMod {
	public:				
		SphereDeflectorField deflect;

		SphereDeflectMod() {}
		SphereDeflectMod(INode *node,SphereDefObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_RB_SPHDEFLECTMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return SPHEREDEFMOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_SPHDEFBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class SphereDeflectorModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new SphereDeflectMod;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_SPHDEFLECTMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return SPHEREDEFMOD_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	};

static SphereDeflectorModClassDesc SpheredeflectModDesc;
ClassDesc* GetSphereDefModDesc() {return &SpheredeflectModDesc;}

//--- SphereDefObject Parameter map/block descriptors ------------------

#define PB_BOUNCE	 0
#define PB_BVAR		 1
#define PB_CHAOS	 2
#define PB_RADIUS	 3
#define PB_VELOCITY	 4

static ParamUIDesc descParam[] = {
	// Bounce
	ParamUIDesc(
		PB_BOUNCE,
		EDITTYPE_FLOAT,
		IDC_UDEFL_MULT,IDC_UDEFL_MULTSPIN,
		0.0f, 9999999.0f,
		0.01f),
	
	// Bounce Var
	ParamUIDesc(
		PB_BVAR,
		EDITTYPE_FLOAT,
		IDC_UDEFL_MULTVAR,IDC_UDEFL_MULTVARSPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),
	
	// Chaos
	ParamUIDesc(
		PB_CHAOS,
		EDITTYPE_FLOAT,
		IDC_UDEFL_CHAOS,IDC_UDEFL_CHAOSSPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),
		
	// Radius
	ParamUIDesc(
		PB_RADIUS,
		EDITTYPE_UNIVERSE,
		IDC_SP_UDEFL_ICONSIZE,IDC_SP_UDEFL_ICONSIZESPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),

	// Velocity Inheritance
	ParamUIDesc(
		PB_VELOCITY,
		EDITTYPE_FLOAT,
		IDC_SP_UDEFL_VELINH,IDC_SP_UDEFL_VELINHSPIN,
		0.0f,1000.0f,SPIN_AUTOSCALE),

	};

#define PARAMDESC_LENGTH	5

ParamBlockDescID SphDefdescVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },	//Bounce
	{ TYPE_FLOAT, NULL, FALSE, 1 },	//Bounce Var
	{ TYPE_FLOAT, NULL, TRUE, 2 },	//Chaos
	{ TYPE_FLOAT, NULL, FALSE, 3 },	//Radius
	{ TYPE_FLOAT, NULL, TRUE, 4 }};	//Velocity Inheritance

#define PBLOCK_LENGTH	5

#define CURRENT_VERSION	0


//--- Deflect object methods -----------------------------------------

SphereDefObject::SphereDefObject()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(SphDefdescVer0, PBLOCK_LENGTH, CURRENT_VERSION));
	assert(pblock);	

	pblock->SetValue(PB_BOUNCE,0,1.0f);
	pblock->SetValue(PB_BVAR,0,0.0f);
	pblock->SetValue(PB_CHAOS,0,0.0f);
	pblock->SetValue(PB_VELOCITY,0,0.0f);
	srand(lastrnd=12345);
	t=99999;
	}

Modifier *SphereDefObject::CreateWSMMod(INode *node)
	{
	return new SphereDeflectMod(node,this);
	}

RefTargetHandle SphereDefObject::Clone(RemapDir& remap) 
	{
	SphereDefObject* newob = new SphereDefObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
	}

void SphereDefObject::BeginEditParams(
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
			GetString(IDS_RB_TOP), 
			(LPARAM)ip,APPENDROLL_CLOSED);

		// Gotta make a new one.
		pmapParam = CreateCPParamMap(
			descParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_SW_SPHEREDEFL),
			GetString(IDS_RB_PARAMETERS),
			0);
		}
	}

void SphereDefObject::EndEditParams(
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

void SphereDefObject::BuildMesh(TimeValue t)
	{
	ivalid = FOREVER;
	float length;
	pblock->GetValue(PB_RADIUS,t,length,ivalid);
	float u;
	#define NUM_SEGS	16
	length/=2.0f;

	mesh.setNumVerts(3*NUM_SEGS+1);
	mesh.setNumFaces(3*NUM_SEGS);

	for (int i=0; i<NUM_SEGS; i++) {
			u = float(i)/float(NUM_SEGS) * TWOPI;
			mesh.setVert(i, Point3((float)cos(u) * length, (float)sin(u) * length, 0.0f));
			}
	for (i=0; i<NUM_SEGS; i++) {
			u = float(i)/float(NUM_SEGS) * TWOPI;
			mesh.setVert(i+NUM_SEGS, Point3(0.0f, (float)cos(u) * length, (float)sin(u) * length));
			}
	for (i=0; i<NUM_SEGS; i++) {
			u = float(i)/float(NUM_SEGS) * TWOPI;
			mesh.setVert(i+2*NUM_SEGS, Point3((float)cos(u) * length, 0.0f, (float)sin(u) * length));
			}		
	mesh.setVert(3*NUM_SEGS, Point3(0.0f, 0.0f, 0.0f));
		
	for (i=0; i<3*NUM_SEGS; i++) {
			int i1 = i+1;
			if (i1%NUM_SEGS==0) i1 -= NUM_SEGS;
			mesh.faces[i].setEdgeVisFlags(1,0,0);
			mesh.faces[i].setSmGroup(0);
			mesh.faces[i].setVerts(i,i1,3*NUM_SEGS);
			}
	mesh.InvalidateGeomCache();
	}


class SphereDeflectObjCreateCallback : public CreateMouseCallBack {
	public:
		SphereDefObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int SphereDeflectObjCreateCallback::proc(
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
				ob->pblock->SetValue(PB_RADIUS,0,2.0f*Length(p1-p0));
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
static SphereDeflectObjCreateCallback SpheredeflectCreateCB;

CreateMouseCallBack* SphereDefObject::GetCreateMouseCallBack()
	{
	SpheredeflectCreateCB.ob = this;
	return &SpheredeflectCreateCB;
	}

void SphereDefObject::InvalidateUI() 
	{
	if (pmapParam) pmapParam->Invalidate();
	}

ParamDimension *SphereDefObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {		
		case PB_BVAR:
		case PB_CHAOS:
				 return stdPercentDim;
		case PB_RADIUS:
				return stdWorldDim;
		default: return defaultDim;
		}
	}

TSTR SphereDefObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {				
		case PB_BOUNCE: 	return GetString(IDS_RB_BOUNCE);
		case PB_BVAR:		return GetString(IDS_RB_BVAR);
		case PB_CHAOS:		return GetString(IDS_RB_CHAOS);
		case PB_RADIUS:		return GetString(IDS_AP_ICONSIZE);
		case PB_VELOCITY:	return GetString(IDS_AP_VELOCITY);
		default: 			return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

SphereDeflectMod::SphereDeflectMod(INode *node,SphereDefObject *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval SphereDeflectMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		SphereDefObject *obj = (SphereDefObject*)GetWSMObject(t);
		obj->pblock->GetValue(PB_BOUNCE,t,f,valid);
		obj->pblock->GetValue(PB_CHAOS,t,f,valid);
		obj->pblock->GetValue(PB_RADIUS,t,f,valid);
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class SphereDeflectDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static SphereDeflectDeformer Sphereddeformer;

Deformer& SphereDeflectMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return Sphereddeformer;
	}

RefTargetHandle SphereDeflectMod::Clone(RemapDir& remap) 
	{
	SphereDeflectMod *newob = new SphereDeflectMod(nodeRef,(SphereDefObject*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
	}


void SphereDeflectMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (SphereDefObject*)GetWSMObject(t);
		deflect.node = nodeRef;
		deflect.tmValid.SetEmpty();	
		if (t<=deflect.obj->t) deflect.obj->lastrnd=12345;
		deflect.obj->t=t;
		obj->ApplyCollisionObject(&deflect);
		}
	}

Object *SphereDeflectorField::GetSWObject()
{ return obj;
}

BOOL SphereDeflectorField::CheckCollision(
		TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide)
{ Point3 zero=Zero;
	if (!tmValid.InInterval(t)) {
		tmValid = FOREVER;
		tm    = node->GetObjectTM(t,&tmValid);
		invtm = Inverse(tm);
		Interval tmpValid=FOREVER;
		tp=node->GetObjectTM(t+(int)dt,&tmpValid);
		Vc=zero;
		Vcp=zero*tp*invtm;
	}
	
	float radius, chaos, bounce,bvar,vinher;
	Point3 p, vr;
    srand(obj->lastrnd);
	p=pos*invtm; 
	vr=VectorTransform(invtm,vel);
	obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
	if (bounce<0.001f) bounce+=0.001f;
	obj->pblock->GetValue(PB_BVAR,t,bvar,FOREVER);
	obj->pblock->GetValue(PB_CHAOS,t,chaos,FOREVER);
	obj->pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
	obj->pblock->GetValue(PB_VELOCITY,t,vinher,FOREVER);
	vinher/=100.0f;
	radius*=0.5f;
	Point3 Vdt;
	Vdt=(Vcp-Vc)/dt;
	Point3 Vrel=vr-Vdt;
	//if (!FGT0(Vrel))return(0);
	float rsquare=radius*radius;
	float rplus=radius,rminus=radius;
	Point3 Vreln=Normalize(Vrel);
	if (LengthSquared(p-Vc)>=(rminus*rminus)) //outside
	{	Point3 P1;
		P1=p+dt*Vrel; //second particle position
		if (!(LengthSquared(P1-Vc)<(rplus*rplus)))
		{	float Dist,Dist1;
			Dist=DotProd(Vreln,(Vc-p));
			if (Dist<0.0f) 
				return(0);
			Dist1=DotProd(-Vreln,(Vc-P1));
			if (Dist1<0.0f)	
				return(0);
			Point3 P10=P1-p,Pc=Vc-p;
			float gamma=(float)acos(DotProd(P10,Pc)/(Length(P10)*Length(Pc)));
			float Dist2=Length(Pc)*(float)cos(HalfPI-gamma);
			if (Dist2>radius) 
				return(0);
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
			return(0);
		Point3 XI,r,n;
		XI=p+omega*vr;
		Point3 Ci=Vc+omega*Vdt;
		r=Normalize(XI-Ci);
		float q1=DotProd(-Vreln,r);
		float theta=(float)acos(q1);
		if (theta>=HalfPI) theta-=PI;
		float v[4];
		if (theta<FLOAT_EPSILON) vr=-vr;
		else 
		{	n=Normalize((-Vreln)^r);
			vr=-vr;
			memcpy(v,vr,row3size);v[3]=1.0f;
			RotateOnePoint(v,&zero.x,&n.x,2*theta);
			memcpy(vr,v,row3size);
		}
		vr=vr*bounce*(1-bvar*RND01());
		if (!FloatEQ0(chaos))
		{	theta=(HalfPI-theta)*chaos*RND01();
			Point3 d=Point3(RND11(),RND11(),RND11());
			Point3 c=Normalize(vr^d);
			memcpy(v,vr,row3size);v[3]=1.0f;
			RotateOnePoint(v,&zero.x,&c.x,theta);
			memcpy(vr,v,row3size);
		}
		if ((vinher>0.0f)&&(t>0))
		{	vr = vr+DotProd(Vdt*vinher,r)*r;
		}
		if (UpdatePastCollide)
		{	pos = XI + (dt-omega)*vr;
			if (ct) (*ct) = dt;
		}
		else
		{	pos = XI;
			if (ct) (*ct) = omega;
		}
		pos=pos*tm;
		vel=VectorTransform(tm,vr);
		obj->lastrnd=rand();
		return TRUE;
	}
	else //inside
	{	Point3 P1;
		P1=p+dt*Vrel;
		if (LengthSquared(P1-Vc)<(rplus*rplus)) 
			return(0);
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
			return(0);
		Point3 XI,r,n;
		XI=p+omega*vr;
		Point3 Ci=Vc+omega*Vdt;
		r=Normalize(XI-Ci);
		float q1=DotProd(Vreln,r);
		float theta=(float)acos(q1);
		float v[4];
		if (theta<FLOAT_EPSILON) vr=-vr;
		else 
		{	n=Normalize(Vreln^r);
			vr=-vr;
			memcpy(v,vr,row3size);v[3]=1.0f;
			RotateOnePoint(v,&zero.x,&n.x,2*theta);
			memcpy(vr,v,row3size);
		}
		vr=vr*bounce*(1-bvar*RND01());
		if (!FloatEQ0(chaos))
		{	theta=(HalfPI-theta)*chaos*RND01();
			Point3 d=Point3(RND11(),RND11(),RND11());
			Point3 c=Normalize(vr^d);
			memcpy(v,vr,row3size);v[3]=1.0f;
			RotateOnePoint(v,&zero.x,&c.x,theta);
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
		pos=pos*tm;
		vel=VectorTransform(tm,vr);
		obj->lastrnd=rand();
		return TRUE;
	}
} 
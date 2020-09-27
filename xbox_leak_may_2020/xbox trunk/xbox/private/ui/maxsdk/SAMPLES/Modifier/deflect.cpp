/**********************************************************************
 *<
	FILE: deflect.cpp

	DESCRIPTION: A simple deflector object for particles

	CREATED BY: Rolf Berteig

	HISTORY: 10-31-95

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "mods.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"

class DeflectObject : public SimpleWSMObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		DeflectObject();

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		Class_ID ClassID() {return Class_ID(DEFLECTOBJECT_CLASS_ID,0);}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTOR);}
				
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

IObjParam *DeflectObject::ip        = NULL;
IParamMap *DeflectObject::pmapParam = NULL;
HWND       DeflectObject::hSot      = NULL;

class DeflectorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new DeflectObject;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_DEFLECTOR_CLASS);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return Class_ID(DEFLECTOBJECT_CLASS_ID,0);}
	const TCHAR* 	Category() {return GetString(SPACEWARPS_FOR_PARTICLES);}
	};

static DeflectorClassDesc deflectDesc;
ClassDesc* GetDeflectObjDesc() {return &deflectDesc;}

//--- DeflectMod -----------------------------------------------------

class DeflectorField : public CollisionObject {
	public:		
		DeflectObject *obj;
		INode *node;
		Matrix3 tm, invtm;
		Interval tmValid;		
		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index, float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();
	};

class DeflectMod : public SimpleWSMMod {
	public:				
		DeflectorField deflect;

		DeflectMod() {}
		DeflectMod(INode *node,DeflectObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_RB_DEFLECTMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return Class_ID(DEFLECTMOD_CLASS_ID,0);}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTORBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
	};

//--- ClassDescriptor and class vars ---------------------------------

class DeflectorModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new DeflectMod;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_DEFLECTMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return Class_ID(DEFLECTMOD_CLASS_ID,0);}
	const TCHAR* 	Category() {return _T("");}
	};

static DeflectorModClassDesc deflectModDesc;
ClassDesc* GetDeflectModDesc() {return &deflectModDesc;}

//--- DeflectObject Parameter map/block descriptors ------------------

#define PB_BOUNCE	0
#define PB_WIDTH	1
#define PB_HEIGHT	2

static ParamUIDesc descParam[] = {
	// Bounce
	ParamUIDesc(
		PB_BOUNCE,
		EDITTYPE_FLOAT,
		IDC_DEFLECT_BOUNCE,IDC_DEFLECT_BOUNCESPIN,
		0.0f, 9999999.0f,
		0.01f),
	
	// Width
	ParamUIDesc(
		PB_WIDTH,
		EDITTYPE_UNIVERSE,
		IDC_DEFLECT_WIDTH,IDC_DEFLECT_WIDTHSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE),
	
	// height
	ParamUIDesc(
		PB_HEIGHT,
		EDITTYPE_UNIVERSE,
		IDC_DEFLECT_HEIGHT,IDC_DEFLECT_HEIGHTSPIN,
		0.0f, 9999999.0f,
		SPIN_AUTOSCALE)
	};

#define PARAMDESC_LENGTH	3

ParamBlockDescID descVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 }};
#define PBLOCK_LENGTH	3

#define CURRENT_VERSION	0


//--- Deflect object methods -----------------------------------------

DeflectObject::DeflectObject()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descVer0, PBLOCK_LENGTH, CURRENT_VERSION));
	assert(pblock);	

	pblock->SetValue(PB_BOUNCE,0,1.0f);
	}

Modifier *DeflectObject::CreateWSMMod(INode *node)
	{
	return new DeflectMod(node,this);
	}

RefTargetHandle DeflectObject::Clone(RemapDir& remap) 
	{
	DeflectObject* newob = new DeflectObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return newob;
	}

void DeflectObject::BeginEditParams(
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
			MAKEINTRESOURCE(IDD_WINDRAIN_SOT),
			DefaultSOTProc,
			GetString(IDS_RB_SOT), 
			(LPARAM)ip,APPENDROLL_CLOSED);

		// Gotta make a new one.
		pmapParam = CreateCPParamMap(
			descParam,PARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_DEFLECTORPARAM),
			GetString(IDS_RB_PARAMETERS),
			0);
		}
	}

void DeflectObject::EndEditParams(
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


void DeflectObject::BuildMesh(TimeValue t)
	{
	float width, height;
	ivalid = FOREVER;
	pblock->GetValue(PB_WIDTH,t,width,ivalid);
	pblock->GetValue(PB_HEIGHT,t,height,ivalid);
	width  *= 0.5f;
	height *= 0.5f;

	mesh.setNumVerts(4);
	mesh.setNumFaces(2);
	mesh.setVert(0, Point3(-width,-height, 0.0f));
	mesh.setVert(1, Point3( width,-height, 0.0f));
	mesh.setVert(2, Point3( width, height, 0.0f));
	mesh.setVert(3, Point3(-width, height, 0.0f));
	
	mesh.faces[0].setEdgeVisFlags(1,0,1);
	mesh.faces[0].setSmGroup(1);
	mesh.faces[0].setVerts(0,1,3);

	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(1);
	mesh.faces[1].setVerts(1,2,3);	
	mesh.InvalidateGeomCache();	
	}


class DeflectObjCreateCallback : public CreateMouseCallBack {
	public:
		DeflectObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};

int DeflectObjCreateCallback::proc(
		ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
	{
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
			case 0:
				sp0 = m;
				p0  = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				mat.SetTrans(p0);
				ob->pblock->SetValue(PB_WIDTH,0,0.01f);
				ob->pblock->SetValue(PB_HEIGHT,0,0.01f);
				ob->pmapParam->Invalidate();
				break;

			case 1: {
				mat.IdentityMatrix();
				sp1 = m;
				p1  = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				Point3 center = (p0+p1)/float(2);
				mat.SetTrans(center);
				ob->pblock->SetValue(PB_WIDTH,0,(float)fabs(p1.x-p0.x));
				ob->pblock->SetValue(PB_HEIGHT,0,(float)fabs(p1.y-p0.y));
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
		} 
	else 
	if (msg == MOUSE_ABORT) {
		return CREATE_ABORT;
		}	
	else
	if (msg == MOUSE_FREEMOVE) {
		vpt->SnapPreview(m,m);
		}

	return TRUE;
	}
static DeflectObjCreateCallback deflectCreateCB;

CreateMouseCallBack* DeflectObject::GetCreateMouseCallBack()
	{
	deflectCreateCB.ob = this;
	return &deflectCreateCB;
	}

void DeflectObject::InvalidateUI() 
	{
	if (pmapParam) pmapParam->Invalidate();
	}

ParamDimension *DeflectObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {		
		case 0:
		default: return defaultDim;
		}
	}

TSTR DeflectObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {				
		case PB_BOUNCE: 	return GetString(IDS_RB_BOUNCE);
		case PB_WIDTH:		return GetString(IDS_RB_WIDTH);
		case PB_HEIGHT:		return GetString(IDS_RB_LENGTH);
		default: 			return TSTR(_T(""));
		}
	}


//--- DeflectMod methods -----------------------------------------------

DeflectMod::DeflectMod(INode *node,DeflectObject *obj)
	{	
	//MakeRefByID(FOREVER,SIMPWSMMOD_OBREF,obj);
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
	pblock = NULL;
	obRef = NULL;
	}

Interval DeflectMod::GetValidity(TimeValue t) 
	{
	if (obRef && nodeRef) {
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		DeflectObject *obj = (DeflectObject*)GetWSMObject(t);
		obj->pblock->GetValue(PB_BOUNCE,t,f,valid);
		obj->pblock->GetValue(PB_WIDTH,t,f,valid);
		obj->pblock->GetValue(PB_HEIGHT,t,f,valid);
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

class DeflectDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
	};
static DeflectDeformer ddeformer;

Deformer& DeflectMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	return ddeformer;
	}

RefTargetHandle DeflectMod::Clone(RemapDir& remap) 
	{
	DeflectMod *newob = new DeflectMod(nodeRef,(DeflectObject*)obRef);	
	newob->SimpleWSMModClone(this);
	return newob;
	}


void DeflectMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) {
		deflect.obj  = (DeflectObject*)GetWSMObject(t);
		deflect.node = nodeRef;
		deflect.tmValid.SetEmpty();		
		obj->ApplyCollisionObject(&deflect);
		}
	}

Object *DeflectorField::GetSWObject()
{ return obj;
}

BOOL DeflectorField::CheckCollision(
		TimeValue t,Point3 &pos, Point3 &vel, float dt, int index, float *ct,BOOL UpdatePastCollide)
{	if (!tmValid.InInterval(t)) 
	{	tmValid = FOREVER;
		tm    = node->GetObjectTM(t,&tmValid);
		invtm = Inverse(tm);
	}
	
	Point3 p, v, ph;
	float width, height, at, bounce;

	obj->pblock->GetValue(PB_WIDTH,t,width,FOREVER);
	obj->pblock->GetValue(PB_HEIGHT,t,height,FOREVER);
	obj->pblock->GetValue(PB_BOUNCE,t,bounce,FOREVER);
	width  *= 0.5f;
	height *= 0.5f;

	// Transform the point and velocity into our space
	p = pos * invtm; 
	v = VectorTransform(invtm,vel);

	// Compute the point of intersection
	if (fabs(p.z)<0.001) 
	{	v.z = 0.0f;
		at  = 0.0f;		
	} 
	else 
	{	if (v.z==0.0f) 
			return FALSE;
		at = -p.z/v.z;
		if (at < 0.0f || at > dt) 
			return FALSE;
	}
	ph = p + at*v;

	// See if the point is within our range
	if (ph.x<-width || ph.x>width || ph.y<-height || ph.y>height) 
		return FALSE;

	// Remove the part of dt we used to get to the collision point
	float holddt = dt;
	dt -= at;

	// Reflect the velocity about the XY plane and attenuate with the bounce factor
	v.z = -v.z;
	if (bounce!=1.0f) 
		v = v*bounce;

	if (UpdatePastCollide)
	{	ph += v * dt;  //uses up the rest of the time with the new velocity
		if (ct) (*ct) = holddt;
	}
	else
	{	if (ct) (*ct) = at;
	}

	// Tranform back into world space.
	pos = ph * tm;
	vel = VectorTransform(tm,v);
	
	return TRUE;
}

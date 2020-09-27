/**********************************************************************
 *<
	FILE: mirror.cpp

	DESCRIPTION: Mirror modifier

	CREATED BY: Rolf Berteig

	HISTORY: 12/06/96

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm.h"
#include "splshape.h"

#define TM_REF		0
#define PBLOCK_REF	1

#define MIRRORMOD_CLASS_ID Class_ID(0xef92aa7c,0x511bbe75);

class MirrorMod : public Modifier {	
	public:		
		IParamBlock *pblock;
		Control *tmControl;
		Control *p1, *p2;
		BYTE sel[2];
		BOOL splineMethod;	// TRUE if using old spline mirroring method (which was wrong)

		static IObjParam *ip;
		static IParamMap *pmapParam;
		static MirrorMod *editMod;
		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;				
		
		MirrorMod();
						
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s = GetString(IDS_RB_MIRRORMOD);}  
		virtual Class_ID ClassID() {return MIRRORMOD_CLASS_ID;}
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_MIRRORMOD);}
		BOOL AssignController(Animatable *control,int subAnim);
		int SubNumToRefNum(int subNum);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		// From modifier
		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE;}
		ChannelMask ChannelsChanged() {return PART_GEOM|PART_TOPO|PART_SELECT|PART_TEXMAP;}
		Class_ID InputType() {return defObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);

		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);		
		void Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin);
		void Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin);
		void Scale(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin);		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void ActivateSubobjSel(int level, XFormModes& modes);
				
		int NumRefs() {return 2;}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int NumSubs() {return 2;}
		Animatable* SubAnim(int i) {return GetReference(i);}
		TSTR SubAnimName(int i);

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		Matrix3 CompMatrix(TimeValue t,INode *inode,ModContext *mc);
		void DrawGizmo(float size,PolyLineProc& lp);
	};


class MirrorDeformer : public Deformer {
	public:		
		Matrix3 tm, itm;
		MirrorDeformer(int axis, float offset, Matrix3 &tm, Matrix3 &itm);			
		Point3 Map(int i, Point3 p) {return (tm*p)*itm;}
	};


//--- ClassDescriptor and class vars ---------------------------------

IParamMap          *MirrorMod::pmapParam   = NULL;
IObjParam          *MirrorMod::ip          = NULL;
MirrorMod          *MirrorMod::editMod     = NULL;
MoveModBoxCMode    *MirrorMod::moveMode    = NULL;
RotateModBoxCMode  *MirrorMod::rotMode 	   = NULL;
UScaleModBoxCMode  *MirrorMod::uscaleMode  = NULL;
NUScaleModBoxCMode *MirrorMod::nuscaleMode = NULL;
SquashModBoxCMode  *MirrorMod::squashMode  = NULL;
		

class MirrorModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MirrorMod;}
	const TCHAR *	ClassName() { return GetString(IDS_RB_MIRRORMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return MIRRORMOD_CLASS_ID;}
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static MirrorModClassDesc mirrorDesc;
extern ClassDesc* GetMirrorModDesc() {return &mirrorDesc;}


//--- Parameter map/block descriptors -------------------------------

#define PB_AXIS				0
#define PB_COPY				1
#define PB_OFFSET			2

// Spline methods
#define SPLINE_REVERSE 0	// Pre-r3 method
#define SPLINE_NOREVERSE 1	// r3 method

//
//
// Parameters

static int axisIDs[] = {IDS_MIRROR_X,IDS_MIRROR_Y,IDS_MIRROR_Z,IDS_MIRROR_XY,IDS_MIRROR_YZ,IDS_MIRROR_ZX};

static ParamUIDesc descParam[] = {	
	// Axis
	ParamUIDesc(PB_AXIS,TYPE_RADIO,axisIDs,6),

	// Copy
	ParamUIDesc(PB_COPY,TYPE_SINGLECHEKBOX,IDC_MIRROR_COPY),	

	// Offset
	ParamUIDesc(
		PB_OFFSET,
		EDITTYPE_UNIVERSE,
		IDC_MIRROR_OFFSET,IDC_MIRROR_OFFSETSPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),
	};
#define PARAMDESC_LENGTH	3

// TH 5/14/99 -- We're faking a different parameter block version to fix a problem in
// previously-saved files (<r3).  We key off the version to set the 'splineMethod' variable.

static ParamBlockDescID descVer0[] = {
	{ TYPE_INT,   NULL, FALSE, 0 },		// Axis
	{ TYPE_INT,   NULL, FALSE, 1 },		// Copy
	{ TYPE_FLOAT, NULL, TRUE, 2 },		// Offset
	};

static ParamBlockDescID descVer1[] = {
	{ TYPE_INT,   NULL, FALSE, 0 },		// Axis
	{ TYPE_INT,   NULL, FALSE, 1 },		// Copy
	{ TYPE_FLOAT, NULL, TRUE, 2 },		// Offset
	};
#define PBLOCK_LENGTH	3

// Array of old versions
static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,3,0),
	};
#define NUM_OLDVERSIONS	1

#define CURRENT_VERSION	1
static ParamVersionDesc curVersion(descVer1,PBLOCK_LENGTH,CURRENT_VERSION);

//--- Affect region mod methods -------------------------------

MirrorMod::MirrorMod() 
	{
	tmControl  = NULL; 	
	pblock     = NULL;
	MakeRefByID(
		FOREVER, PBLOCK_REF, 
		CreateParameterBlock(
			descVer1, PBLOCK_LENGTH, CURRENT_VERSION));	
	MakeRefByID(FOREVER,TM_REF,NewDefaultMatrix3Controller()); 
	splineMethod = SPLINE_NOREVERSE;
	}

void MirrorMod::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	editMod  = this;

	// Add our sub object type
	TSTR type1(GetString(IDS_RB_MIRRORCENTER));
	const TCHAR *ptype[] = {type1};
	ip->RegisterSubObjectTypes(ptype, 1);

	// Create sub object editing modes.
	moveMode    = new MoveModBoxCMode(this,ip);
	rotMode     = new RotateModBoxCMode(this,ip);
	uscaleMode  = new UScaleModBoxCMode(this,ip);
	nuscaleMode = new NUScaleModBoxCMode(this,ip);
	squashMode  = new SquashModBoxCMode(this,ip);	

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGTH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_MIRRORPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);	
	}

void MirrorMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	this->ip = NULL;
	editMod  = NULL;

	TimeValue t = ip->GetTime();

	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

	ip->DeleteMode(moveMode);
	ip->DeleteMode(rotMode);
	ip->DeleteMode(uscaleMode);
	ip->DeleteMode(nuscaleMode);
	ip->DeleteMode(squashMode);	
	if ( moveMode ) delete moveMode;
	moveMode = NULL;
	if ( rotMode ) delete rotMode;
	rotMode = NULL;
	if ( uscaleMode ) delete uscaleMode;
	uscaleMode = NULL;
	if ( nuscaleMode ) delete nuscaleMode;
	nuscaleMode = NULL;
	if ( squashMode ) delete squashMode;
	squashMode = NULL;

	DestroyCPParamMap(pmapParam);
	}

RefTargetHandle MirrorMod::Clone(RemapDir& remap)
	{
	MirrorMod *mod = new MirrorMod();
	mod->ReplaceReference(PBLOCK_REF,pblock->Clone(remap));	
	mod->ReplaceReference(TM_REF,remap.CloneRef(tmControl));
	mod->splineMethod = splineMethod;
	return mod;
	}

MirrorDeformer::MirrorDeformer(
		int axis, float offset, Matrix3 &tm, Matrix3 &itm) 
	{	
	this->tm   = tm;
	this->itm  = itm;
	Point3 scale(1,1,1), off(0,0,0);
	switch (axis) {
		case 0:
		case 1:
		case 2:
			scale[axis] = -1.0f;
			off[axis]   = offset;
			break;
		case 3:
		case 4:
		case 5:
			scale[(axis)%3]   = -1.0f;
			scale[(axis+1)%3] = -1.0f;
			off[(axis)%3]     = offset;
			off[(axis+1)%3]   = offset;
			break;
		}
	this->tm.Scale(scale,TRUE);
	this->tm.Translate(off);
	}

void MirrorMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{	
	Matrix3 itm = CompMatrix(t,NULL,&mc);
	Matrix3 tm  = Inverse(itm);
	Interval iv = FOREVER;
	int axis, copy;
	float offset;
	pblock->GetValue(PB_AXIS,t,axis,iv);
	pblock->GetValue(PB_COPY,t,copy,iv);
	pblock->GetValue(PB_OFFSET,t,offset,iv);
	DWORD oldLevel;
	BOOL convertedShape = FALSE;

	if (os->obj->IsSubClassOf(triObjectClassID)) {
		TriObject *tobj = (TriObject*)os->obj;
		Mesh &mesh = tobj->GetMesh();		
		switch (mesh.selLevel) {
			case MESH_OBJECT: mesh.faceSel.SetAll(); break;			
			case MESH_VERTEX: {
				for (int i=0; i<mesh.getNumFaces(); i++) {
					for (int j=0; j<3; j++) {
						if (mesh.vertSel[mesh.faces[i].v[j]]) {
							mesh.faceSel.Set(i);
							}
						}
					}
				break;
				}
			
			case MESH_EDGE:	{
				for (int i=0; i<mesh.getNumFaces(); i++) {
					for (int j=0; j<3; j++) {
						if (mesh.edgeSel[i*3+j]) {
							mesh.faceSel.Set(i);
							}
						}
					}
				break;
				}		
			}		
		oldLevel = mesh.selLevel;
		mesh.selLevel = MESH_FACE;
		if (copy) mesh.CloneFaces(mesh.faceSel);
		if (axis<3) {
			for (int i=0; i<mesh.getNumFaces(); i++) {
				if (mesh.faceSel[i]) mesh.FlipNormal(i);
				}
			}
		}
	else
	if (os->obj->IsSubClassOf(splineShapeClassID)) {
		SplineShape *ss = (SplineShape*)os->obj;
		BezierShape &shape = ss->shape;		
		oldLevel = shape.selLevel;
		switch (shape.selLevel) {
			case SHAPE_OBJECT:			
			case SHAPE_VERTEX:
				shape.selLevel = SHAPE_SPLINE;
				shape.polySel.SetAll();
				break;
				 
			case SHAPE_SPLINE:
			case SHAPE_SEGMENT:
				break;
			}		
		if (copy)
			shape.CloneSelectedParts((axis < 3 && splineMethod == SPLINE_REVERSE) ? TRUE : FALSE);
		}
	else
	if(os->obj->SuperClassID() == SHAPE_CLASS_ID) {
		ShapeObject *so = (ShapeObject *)os->obj;
		if(so->CanMakeBezier()) {
			SplineShape *ss = new SplineShape();
			so->MakeBezier(t, ss->shape);
			ss->SetChannelValidity(GEOM_CHAN_NUM, LocalValidity(t) & so->ObjectValidity(t));
			os->obj = ss;
			os->obj->UnlockObject();
			convertedShape = TRUE;
			BezierShape &shape = ss->shape;		
			oldLevel = shape.selLevel;
			switch (shape.selLevel) {
				case SHAPE_OBJECT:			
				case SHAPE_VERTEX:
					shape.selLevel = SHAPE_SPLINE;
					shape.polySel.SetAll();
					break;
					 
				case SHAPE_SPLINE:
				case SHAPE_SEGMENT:
					break;
				}		
			if (copy)
				shape.CloneSelectedParts((axis < 3 && splineMethod == SPLINE_REVERSE) ? TRUE : FALSE);
			}
		}

	MirrorDeformer deformer(axis,offset,tm,itm);


	os->obj->Deform(&deformer, TRUE);	

//	if (axis < 3 && splineMethod == SPLINE_REVERSE)
		{
		if (os->obj->IsSubClassOf(splineShapeClassID)) {
			SplineShape *ss = (SplineShape*)os->obj;
			BezierShape &shape = ss->shape;		
			for (int i = 0; i < shape.bindList.Count(); i++)
				{
				int index = 0;
				int spindex = shape.bindList[i].pointSplineIndex;
//			Point3 p=shape.splines[spindex]->GetKnot(index).Knot();
				if (shape.bindList[i].isEnd)
					index = shape.splines[spindex]->KnotCount()-1;
				shape.bindList[i].bindPoint = shape.splines[spindex]->GetKnotPoint(index);
				shape.bindList[i].segPoint = shape.splines[spindex]->GetKnotPoint(index);
				}
			shape.UpdateBindList();
	
			}
		else
		if(os->obj->SuperClassID() == SHAPE_CLASS_ID) {
			ShapeObject *so = (ShapeObject *)os->obj;
			if(so->CanMakeBezier()) {
				SplineShape *ss = new SplineShape();
				so->MakeBezier(t, ss->shape);
				ss->SetChannelValidity(GEOM_CHAN_NUM, LocalValidity(t) & so->ObjectValidity(t));
				os->obj = ss;
				os->obj->UnlockObject();
				convertedShape = TRUE;
				BezierShape &shape = ss->shape;		
				for (int i = 0; i < shape.bindList.Count(); i++)
					{
					int index = 0;
					int spindex = shape.bindList[i].pointSplineIndex;
//				Point3 p;
					if (shape.bindList[i].isEnd)
						index = shape.splines[spindex]->KnotCount()-1;
					shape.bindList[i].bindPoint = shape.splines[spindex]->GetKnotPoint(index);
					shape.bindList[i].segPoint = shape.splines[spindex]->GetKnotPoint(index);
					}
				shape.UpdateBindList();

				}

			}
		}

	os->obj->UpdateValidity(GEOM_CHAN_NUM,LocalValidity(t));	
	
	if (os->obj->IsSubClassOf(triObjectClassID)) {
		TriObject *tobj = (TriObject*)os->obj;
		tobj->GetMesh().selLevel = oldLevel;
		}
	else
	if(os->obj->IsSubClassOf(splineShapeClassID) || convertedShape) {
		SplineShape *ss = (SplineShape*)os->obj;
		ss->shape.selLevel = oldLevel;
		}
	}

Interval MirrorMod::LocalValidity(TimeValue t)
	{
	Interval iv = FOREVER;
	Matrix3 mat(1);		
	tmControl->GetValue(t,&mat,iv,CTRL_RELATIVE);
	float o;
	pblock->GetValue(PB_OFFSET,t,o,iv);
	return iv;
	}


Matrix3 MirrorMod::CompMatrix(
		TimeValue t,INode *inode,ModContext *mc)
	{
	Interval iv;
	Matrix3 tm(1);		
	tmControl->GetValue(t,&tm,iv,CTRL_RELATIVE);
	if (mc && mc->tm) tm = tm * Inverse(*(mc->tm));
	if (inode) tm = tm * inode->GetObjTMBeforeWSM(t,&iv);
	return tm;
	}


#define AXIS_SIZE		2.0f
#define SCREEN_SCALE	0.1f

static void SetupAxisPoints(
		Point3 &v, Point3 &vp, float size,Point3 *pts) 
	{
	pts[0] = (vp)*size;
	pts[2] = (-vp)*size;
	pts[1] = (v^vp)*size;
	pts[3] = (v^(-vp))*size;
	pts[4] = pts[0];

	pts[5] = -v*size*AXIS_SIZE;
	pts[6] = v*size*AXIS_SIZE;
	
	Point3 v2 = v*0.9f*size*AXIS_SIZE;

	pts[7] = v2+pts[0]*0.1f;
	pts[8] = pts[6];
	pts[9] = v2+pts[2]*0.1f;

	pts[10] = v2+pts[1]*0.1f;
	pts[11] = pts[6];
	pts[12] = v2+pts[3]*0.1f;
	
	pts[13] = -v2+pts[0]*0.1f;
	pts[14] = -pts[6];
	pts[15] = -v2+pts[2]*0.1f;

	pts[16] = -v2+pts[1]*0.1f;
	pts[17] = -pts[6];
	pts[18] = -v2+pts[3]*0.1f;
	}

void MirrorMod::DrawGizmo(float size,PolyLineProc& lp)
	{
	Point3 v0(0,0,0), pv0(0,0,0), v1(0,0,0), pv1(0,0,0);
	int ct=0;
	int axis;
	pblock->GetValue(PB_AXIS,0,axis,FOREVER);

	switch (axis) {
		case 0:
		case 1:
		case 2:
			v0[axis]        = -1.0f;
			pv0[(axis+1)%3]	= 1.0f;
			pv0[(axis+2)%3]	= 1.0f;
			ct=1;
			break;
		
		case 3:
		case 4:
		case 5:
			v0[(axis)%3]    = -1.0f;
			pv0[(axis+1)%3]	= 1.0f;
			pv0[(axis+2)%3]	= 1.0f;
			v1[(axis+1)%3]  = -1.0f;
			pv1[(axis+2)%3]	= 1.0f;
			pv1[(axis+3)%3]	= 1.0f;
			ct=2;
			break;
		}
	
	Point3 pts[20];
	SetupAxisPoints(v0, pv0, size, pts);
	lp.proc(pts,5);
	lp.proc(pts+5,2);
	lp.proc(pts+7,3);
	lp.proc(pts+10,3);
	lp.proc(pts+13,3);
	lp.proc(pts+16,3);

	if (ct==2) {
		SetupAxisPoints(v1, pv1, size, pts);
		lp.proc(pts,5);
		lp.proc(pts+5,2);
		lp.proc(pts+7,3);
		lp.proc(pts+10,3);
		lp.proc(pts+13,3);
		lp.proc(pts+16,3);
		}
	}

int MirrorMod::HitTest(
		TimeValue t, INode* inode, 
		int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt;
	HitRegion hr;
	int savedLimits, res = 0;
	Matrix3 tm = CompMatrix(t,inode,mc);

	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);	
	gw->setTransform(tm);
	gw->clearHitCode();
	DrawLineProc lp(gw);
	DrawGizmo(
		vpt->GetScreenScaleFactor(tm.GetTrans())*SCREEN_SCALE,lp);
	gw->setRndLimits(savedLimits);
	if (gw->checkHitCode()) {
		vpt->LogHit(inode, mc, gw->getHitDistance(), 0, NULL); 
		return 1;
		}
	return 0;
	}

int MirrorMod::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flagst, ModContext *mc)
	{
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt[4];
	Matrix3 tm = CompMatrix(t,inode,mc);
	int savedLimits;

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);
	if (ip && ip->GetSubObjectLevel() == 1) {
		//gw->setColor(LINE_COLOR, (float)1.0, (float)1.0, (float)0.0);
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
	} else {
		//gw->setColor(LINE_COLOR, (float).85, (float).5, (float)0.0);
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_GIZMOS));
		}
	
	DrawLineProc lp(gw);
	DrawGizmo(
		vpt->GetScreenScaleFactor(tm.GetTrans())*SCREEN_SCALE,lp);
	
	gw->setRndLimits(savedLimits);
	return 0;
	}

void MirrorMod::GetWorldBoundBox(
		TimeValue t,INode* inode, ViewExp *vpt, 
		Box3& box, ModContext *mc)
	{	
	GraphicsWindow *gw = vpt->getGW();
	Matrix3 tm = CompMatrix(t,inode,mc);
	BoxLineProc bproc(&tm);
	DrawGizmo(
		vpt->GetScreenScaleFactor(tm.GetTrans())*SCREEN_SCALE,bproc);
	box = bproc.Box();	
	}

void MirrorMod::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin)
	{
	SetXFormPacket pckt(val,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
	}

void MirrorMod::Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin)
	{
	SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
	}

void MirrorMod::Scale(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin)
	{
	SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
	}

void MirrorMod::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{
	Matrix3 tm = CompMatrix(t,node,mc);	
	cb->Center(tm.GetTrans(),0);
	}

void MirrorMod::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{
	Matrix3 tm = CompMatrix(t,node,mc);
	cb->TM(tm,0);
	}

void MirrorMod::ActivateSubobjSel(int level, XFormModes& modes)
	{
	switch (level) {
		case 1: // Mirror center
			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,NULL);
			break;		
		}
	NotifyDependents(FOREVER,PART_DISPLAY,REFMSG_CHANGE);
	}


BOOL MirrorMod::AssignController(Animatable *control,int subAnim)
	{
	if (subAnim==TM_REF) {
		ReplaceReference(TM_REF,(ReferenceTarget*)control);
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);		
		return TRUE;
	} else {
		return FALSE;
		}
	}

int MirrorMod::SubNumToRefNum(int subNum)
	{
	if (subNum==TM_REF) return subNum;
	else return -1;
	}

RefTargetHandle MirrorMod::GetReference(int i)
	{
	switch (i) {
		case PBLOCK_REF: return pblock;
		case TM_REF:     return tmControl;		
		default: return NULL;
		}
	}

void MirrorMod::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case PBLOCK_REF: pblock    = (IParamBlock*)rtarg; break;
		case TM_REF:     tmControl = (Control*)rtarg; break;		
		}
	}

TSTR MirrorMod::SubAnimName(int i)
	{
	switch (i) {
		case TM_REF: return GetString(IDS_RB_MIRRORCENTER); break;		
		default: return _T(""); break;
		}
	}

RefResult MirrorMod::NotifyRefChanged(
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
				case PB_OFFSET: gpd->dim = stdWorldDim; break;				
				}
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;			
			switch (gpn->index) {
				case PB_OFFSET: gpn->name = GetString(IDS_RB_OFFSET); break;				
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}

class MirrorPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		MirrorPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload);
	};

void MirrorPostLoadCallback::proc(ILoad *iload) {
	DWORD oldVer = ((MirrorMod*)(cb->targ))->pblock->GetVersion();
	ReferenceTarget *targ = cb->targ;
	cb->proc(iload);
	if (oldVer<1)
		((MirrorMod*)targ)->splineMethod = SPLINE_REVERSE;
	delete this;
	}

#define SPLINE_METHOD_CHUNK 0x1000

IOResult MirrorMod::Save(ISave *isave) {	
	ULONG nb;
	Modifier::Save(isave);
	isave->BeginChunk(SPLINE_METHOD_CHUNK);
	isave->Write(&splineMethod,sizeof(int),&nb);
	isave->EndChunk();
	return IO_OK;
	}

IOResult MirrorMod::Load(ILoad *iload)
	{
	IOResult res;
	ULONG nb;
	Modifier::Load(iload);
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SPLINE_METHOD_CHUNK:
				iload->Read(&splineMethod,sizeof(int),&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	iload->RegisterPostLoadCallback(
		new MirrorPostLoadCallback(
			new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,PBLOCK_REF)));
	return IO_OK;
	}


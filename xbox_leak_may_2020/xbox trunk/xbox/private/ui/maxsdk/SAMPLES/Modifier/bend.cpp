/**********************************************************************
 *<
	FILE: bend.cpp

	DESCRIPTION:  Bend OSM

	CREATED BY: Dan Silva & Rolf Berteig

	HISTORY: created 30 Jauary, 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "mods.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"

#define BIGFLOAT	float(999999)

class BendMod : public SimpleMod {	
	public:
		static IParamMap *pmapParam;

		BendMod();

		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_BENDMOD); }  
		virtual Class_ID ClassID() { return Class_ID(BENDOSM_CLASS_ID,0);}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() { return GetString(IDS_RB_BEND2);}
		IOResult Load(ILoad *iload);

		// From simple mod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		BOOL GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis);
		void InvalidateUI() {if (pmapParam) pmapParam->Invalidate();}
	};

class BendDeformer: public Deformer {
	public:
		Matrix3 tm,invtm, tmAbove, tmBelow;
		Box3 bbox;
		TimeValue time;
		float r, from, to;
		int doRegion;
		BendDeformer();
		BendDeformer(
			TimeValue t, ModContext &mc,
			float angle, float dir, int naxis, 
			float from, float to, int doRegion, 
			Matrix3& modmat, Matrix3& modinv);
		void SetAxis(Matrix3 &tmAxis);
		void CalcR(int axis, float angle);
		Point3 Map(int i, Point3 p); 
	};


#define BENDWSM_CLASSID	Class_ID(BENDOSM_CLASS_ID,1)

class BendWSM : public SimpleOSMToWSMObject {
	public:
		BendWSM() {}
		BendWSM(BendMod *m) : SimpleOSMToWSMObject(m) {}
		void DeleteThis() { delete this; }
		SClass_ID SuperClassID() {return WSM_OBJECT_CLASS_ID;}
		Class_ID ClassID() {return BENDWSM_CLASSID;} 
		TCHAR *GetObjectName() {return GetString(IDS_RB_BEND2);}
		RefTargetHandle Clone(RemapDir& remap)
			{return (new BendWSM((BendMod*)mod->Clone(remap)))->SimpleOSMToWSMClone(this,remap);}
	};

//--- ClassDescriptor and class vars ---------------------------------

IParamMap *BendMod::pmapParam = NULL;


class BendClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new BendMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_BEND_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(BENDOSM_CLASS_ID,0); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static BendClassDesc bendDesc;
extern ClassDesc* GetBendModDesc() { return &bendDesc; }

class BendWSMClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) 
		{if (loading) return new BendWSM; else return new BendWSM(new BendMod);}
	const TCHAR *	ClassName() { return GetString(IDS_RB_BEND_CLASS); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() { return BENDWSM_CLASSID; }
	const TCHAR* 	Category() {return GetSpaceWarpCatString(SPACEWARP_CAT_MODBASED);}
	};

static BendWSMClassDesc bendWSMDesc;
extern ClassDesc* GetBendWSMDesc() { return &bendWSMDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_ANGLE	0
#define PB_DIR		1
#define PB_AXIS		2
#define PB_DOREGION	3
#define PB_FROM		4
#define PB_TO		5


//
//
// Parameters

static int axisIDs[] = {IDC_X,IDC_Y,IDC_Z};

static ParamUIDesc descParam[] = {
	// Angle
	ParamUIDesc(
		PB_ANGLE,
		EDITTYPE_FLOAT,
		IDC_ANGLE,IDC_ANGLESPINNER,
		-BIGFLOAT,BIGFLOAT,
		0.5f),

	// Direction
	ParamUIDesc(
		PB_DIR,
		EDITTYPE_FLOAT,
		IDC_DIR,IDC_DIRSPINNER,
		-BIGFLOAT,BIGFLOAT,
		0.5f),

	// Axis
	ParamUIDesc(PB_AXIS,TYPE_RADIO,axisIDs,3),

	// Affect region
	ParamUIDesc(PB_DOREGION,TYPE_SINGLECHEKBOX,IDC_BEND_AFFECTREGION),

	// From
	ParamUIDesc(
		PB_FROM,
		EDITTYPE_UNIVERSE,
		IDC_BEND_FROM,IDC_BEND_FROMSPIN,
		-BIGFLOAT,0.0f,
		SPIN_AUTOSCALE),

	// To
	ParamUIDesc(
		PB_TO,
		EDITTYPE_UNIVERSE,
		IDC_BEND_TO,IDC_BEND_TOSPIN,
		0.0f,BIGFLOAT,		
		SPIN_AUTOSCALE),	
	};
#define PARAMDESC_LENGH 6


static ParamBlockDescID descVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, FALSE, 2 } };

static ParamBlockDescID descVer1[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },	
	{ TYPE_INT, NULL, FALSE, 2 },
	{ TYPE_INT, NULL, FALSE, 3 },
	{ TYPE_FLOAT, NULL, TRUE, 4 },
	{ TYPE_FLOAT, NULL, TRUE, 5 } };

#define PBLOCK_LENGTH	6

// Array of old versions
static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,3,0)
	};
#define NUM_OLDVERSIONS	1

// Current version
#define CURRENT_VERSION	1
static ParamVersionDesc curVersion(descVer1,PBLOCK_LENGTH,CURRENT_VERSION);

//--- BendDlgProc -------------------------------


class BendDlgProc : public ParamMapUserDlgProc {
	public:
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {}
	};
static BendDlgProc theBendProc;

BOOL BendDlgProc::DlgProc(
		TimeValue t,IParamMap *map,
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_BEND_FROMSPIN: {
					float from, to;
					map->GetParamBlock()->GetValue(PB_FROM,t,from,FOREVER);
					map->GetParamBlock()->GetValue(PB_TO,t,to,FOREVER);
					if (from>to) {
						map->GetParamBlock()->SetValue(PB_TO,t,from);
						map->Invalidate();
						}
					break;
					}
				
				case IDC_BEND_TOSPIN: {
					float from, to;
					map->GetParamBlock()->GetValue(PB_FROM,t,from,FOREVER);
					map->GetParamBlock()->GetValue(PB_TO,t,to,FOREVER);
					if (from>to) {
						map->GetParamBlock()->SetValue(PB_FROM,t,to);
						map->Invalidate();
						}
					break;
					}
				}
			break;
		}
	return FALSE;
	}

//--- Bend methods -------------------------------


BendMod::BendMod() : SimpleMod()
	{	
	MakeRefByID(FOREVER, SIMPMOD_PBLOCKREF, 
		CreateParameterBlock(descVer1, PBLOCK_LENGTH, CURRENT_VERSION));
	
	pblock->SetValue(PB_AXIS, TimeValue(0), 2/*Z*/);
	}

IOResult BendMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,SIMPMOD_PBLOCKREF));
	return IO_OK;
	}

void BendMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	SimpleMod::BeginEditParams(ip,flags,prev);
		
	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_BENDPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);	
	pmapParam->SetUserDlgProc(&theBendProc);
	}
		
void BendMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleMod::EndEditParams(ip,flags,next);
	DestroyCPParamMap(pmapParam);
	}

Interval BendMod::GetValidity(TimeValue t)
	{
	float f;	
	Interval valid = FOREVER;
	pblock->GetValue(PB_ANGLE,t,f,valid);
	pblock->GetValue(PB_DIR,t,f,valid);	
	pblock->GetValue(PB_FROM,t,f,valid);
	pblock->GetValue(PB_TO,t,f,valid);
	return valid;
	}

BOOL BendMod::GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis)
	{
	int limit;
	pblock->GetValue(PB_DOREGION,t,limit,FOREVER);
	pblock->GetValue(PB_FROM,t,zmin,FOREVER);
	pblock->GetValue(PB_TO,t,zmax,FOREVER);
	pblock->GetValue(PB_AXIS,t,axis,FOREVER);
	return limit?TRUE:FALSE;
	}

RefTargetHandle BendMod::Clone(RemapDir& remap) {	
	BendMod* newmod = new BendMod();	
	newmod->ReplaceReference(SIMPMOD_PBLOCKREF,pblock->Clone(remap));
	newmod->SimpleModClone(this);
	return(newmod);
	}

BendDeformer::BendDeformer() 
	{ 
	tm.IdentityMatrix();
	time = 0;	
	}

void BendDeformer::SetAxis(Matrix3 &tmAxis)
	{
	Matrix3 itm = Inverse(tmAxis);
	tm    = tm*tmAxis;
	invtm =	itm*invtm;
	}

void BendDeformer::CalcR(int axis, float angle)
	{
	float len = float(0);
	if (!doRegion) {
		switch (axis) {
			case 0:  len = bbox.pmax.x - bbox.pmin.x; break;
			case 1:	 len = bbox.pmax.y - bbox.pmin.y; break;
			case 2:  len = bbox.pmax.z - bbox.pmin.z; break;
			}
	} else {
		len = to-from;
		}

	// Skip the singularity
	if (fabs(angle) <0.0001) {
		r = float(0);
	} else {
		r = len/angle;
		}	
	}

Point3 BendDeformer::Map(int i, Point3 p)
	{
	float x, y, c, s, yr;
	if (r==0 && !doRegion) return p;
	p = p * tm;
	if (doRegion) {
		if (p.z<from) {
			return tmBelow * p * invtm;			
		} else 
		if (p.z>to) {
			return tmAbove * p * invtm;
			}
		}	
	if (r==0) return p * invtm;
	x = p.x;
	y = p.z;
	yr = y/r;
	c = float(cos(PI-yr));
	s = float(sin(PI-yr));
	p.x = r*c + r - x*c;
	p.z = r*s - x*s;
	p = p * invtm;
	return p;
	}

BendDeformer::BendDeformer(
		TimeValue t, ModContext &mc,
		float angle, float dir, int naxis, 
		float from, float to, int doRegion,
		Matrix3& modmat, Matrix3& modinv) 
	{	
	this->doRegion = doRegion;
	this->from = from;
	this->to   = to;
	Matrix3 mat;
	Interval valid;	
	time   = t;	

	tm = modmat;
	invtm = modinv;
	mat.IdentityMatrix();
	
	switch (naxis) {
		case 0: mat.RotateY( -HALFPI );	 break; //X
		case 1: mat.RotateX( HALFPI );  break; //Y
		case 2: break;  //Z
		}
	mat.RotateZ(DegToRad(dir));	
	SetAxis(mat);	
	assert (mc.box);
	bbox = *mc.box;
	CalcR(naxis,DegToRad(angle));
	
	// Turn this off for a sec.
	this->doRegion = FALSE;
		
	float len  = to-from;
	float rat1, rat2;
	if (len==0.0f) {
		rat1 = rat2 = 1.0f;
	} else {
		rat1 = to/len;
		rat2 = from/len;
		}
	Point3 pt;
	tmAbove.IdentityMatrix();
	tmAbove.Translate(Point3(0.0f,0.0f,-to));
	tmAbove.RotateY(DegToRad(angle * rat1));
	tmAbove.Translate(Point3(0.0f,0.0f,to));
	pt = Point3(0.0f,0.0f,to);
	tmAbove.Translate((Map(0,pt*invtm)*tm)-pt);

	tmBelow.IdentityMatrix();
	tmBelow.Translate(Point3(0.0f,0.0f,-from));
	tmBelow.RotateY(DegToRad(angle * rat2));	
	tmBelow.Translate(Point3(0.0f,0.0f,from));
	pt = Point3(0.0f,0.0f,from);
	tmBelow.Translate((Map(0,pt*invtm)*tm)-pt);	
	
	this->doRegion = doRegion;
	} 


Deformer& BendMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	float angle, dir, from, to;
	int axis;	
	int doRegion;
	pblock->GetValue(PB_ANGLE,t,angle,FOREVER);
	pblock->GetValue(PB_DIR,t,dir,FOREVER);
	pblock->GetValue(PB_AXIS,t,axis,FOREVER);
	pblock->GetValue(PB_FROM,t,from,FOREVER);
	pblock->GetValue(PB_TO,t,to,FOREVER);
	pblock->GetValue(PB_DOREGION,t,doRegion,FOREVER);
	static BendDeformer deformer;
	deformer = BendDeformer(t,mc,angle,dir,axis,from,to,doRegion,mat,invmat);
	return deformer;
	}

ParamDimension *BendMod::GetParameterDim(int pbIndex)
	{
	switch (pbIndex) {
		case PB_ANGLE: 	return defaultDim; // Note: doesn't use angleDim because the bend has been storing the angle in degrees not radians. Changing this would invalidate old files.
		case PB_DIR:	return defaultDim;
		case PB_FROM:	return stdWorldDim;
		case PB_TO:		return stdWorldDim;
		default:		return defaultDim;
		}
	}

TSTR BendMod::GetParameterName(int pbIndex)
	{
	switch (pbIndex) {
		case PB_ANGLE:	return GetString(IDS_RB_ANGLE);
		case PB_DIR:	return GetString(IDS_RB_DIRECTION);
		case PB_FROM:	return GetString(IDS_RB_FROM);
		case PB_TO:		return GetString(IDS_RB_TO);
		default:		return _T("");
		}
	}








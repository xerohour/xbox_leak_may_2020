#include "mods2.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "windows.h"

// in mods.cpp
extern HINSTANCE hInstance;

#define BIGFLOAT	float(999999)

static Class_ID SPINDLEOSM_CLASS_ID(0x11d527a5, 0x3a5149a4);

float squareit(float valuein)
{	return (valuein*valuein);
}

class SpindleMod : public SimpleMod {
	public:
		static IParamMap *pmapParam;

		SpindleMod();		
				
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_EP_SPINDLEMOD); }  
		virtual Class_ID ClassID() { return SPINDLEOSM_CLASS_ID;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next );
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() { return GetString(IDS_EP_SPINDLE); }		
		IOResult Load(ILoad *iload);

		// From simple mod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		BOOL GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis);
		void InvalidateUI() {if (pmapParam) pmapParam->Invalidate();}
	};


class SpindleDeformer: public Deformer {
	public:
		Matrix3 tm,invtm;
		Box3 bbox;
		TimeValue time;
		float ks1,ks2;
		float kq1,kq2;
		float kbias;
		float kvol;
		float fromQ, toQ;
		int doRegionQ;
		SpindleDeformer();
		SpindleDeformer(
			TimeValue t, ModContext &mc, 
			float  amt, float crv,
			float  amtQ, float crvQ,
			float fromQ, float toQ, int doRegionQ,
			float bias, float vol,
			Matrix3& modmat, Matrix3& modinv);
		void SetAxis(Matrix3 &tmAxis);
		void SetKS(float K1, float K2) { ks1 = K1; ks2 = K2; }
		void SetKQ(float K1, float K2) { kq1 = K1; kq2 = K2; }
		void SetBias(float K1) { kbias = K1;}
		void SetVol(float K1) { kvol = K1;}
		Point3 Map(int i, Point3 p); 
	};

#define SPINDLEWSM_CLASSID	Class_ID(SPINDLEOSM_CLASS_ID,1)

class SpindleWSM : public SimpleOSMToWSMObject {
	public:
		SpindleWSM() {}
		SpindleWSM(SpindleMod *m) : SimpleOSMToWSMObject(m) {}
		void DeleteThis() { delete this; }
		SClass_ID SuperClassID() {return WSM_OBJECT_CLASS_ID;}
		Class_ID ClassID() {return SPINDLEOSM_CLASS_ID;} 
		TCHAR *GetObjectName() {return GetString(IDS_EP_SPINDLE);}
		RefTargetHandle Clone(RemapDir& remap)
			{return (new SpindleWSM((SpindleMod*)mod->Clone(remap)))->SimpleOSMToWSMClone(this,remap);}
	};


//--- ClassDescriptor and class vars ---------------------------------

IParamMap *SpindleMod::pmapParam;

class SpindleClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new SpindleMod; }
	const TCHAR *	ClassName() { return GetString(IDS_EP_SPINDLE); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return SPINDLEOSM_CLASS_ID; }
	const TCHAR* 	Category() {return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static SpindleClassDesc taperDesc;
extern ClassDesc* GetSpindleModDesc() { return &taperDesc; }

class SpindleWSMClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) 
		{if (loading) return new SpindleWSM; else return new SpindleWSM(new SpindleMod);}
	const TCHAR *	ClassName() { return GetString(IDS_EP_SPINDLE); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() { return SPINDLEOSM_CLASS_ID; }
	const TCHAR* 	Category() {return GetSpaceWarpCatString(SPACEWARP_CAT_MODBASED);}
	};

static SpindleWSMClassDesc taperWSMDesc;
extern ClassDesc* GetSpindleWSMDesc() { return &taperWSMDesc; }


//--- Parameter map/block descriptors -------------------------------

#define PB_AMT			0
#define PB_CRV			1

#define PB_AMTQ			2
#define PB_CRVQ			3
#define PB_DOREGIONQ	4
#define PB_FROMQ		5
#define PB_TOQ			6

#define PB_BIAS			7
#define PB_VOL			8


//
//
// Parameters

static ParamUIDesc descParam[] = {

	// Amount
	ParamUIDesc(
		PB_AMT,
		EDITTYPE_FLOAT,
		IDC_AMT,IDC_AMTSPINNER,
		-100.0f,100.0f,
		0.01f),

	// Curve
	ParamUIDesc(
		PB_CRV,
		EDITTYPE_FLOAT,
		IDC_CRV,IDC_CRVSPINNER,
		-100.0f,100.0f,
		0.01f),
	
	// Amount
	ParamUIDesc(
		PB_AMTQ,
		EDITTYPE_FLOAT,
		IDC_AMT2,IDC_AMT2SPIN,
		-100.0f,100.0f,
		0.01f),

	// Curve
	ParamUIDesc(
		PB_CRVQ,
		EDITTYPE_FLOAT,
		IDC_CRV2,IDC_CRV2SPIN,
		0.0f,100.0f,
		0.01f),
	
	// Affect region
	ParamUIDesc(PB_DOREGIONQ,TYPE_SINGLECHEKBOX,IDC_SQUEEZE_AFFECTREGION),

	// From
	ParamUIDesc(
		PB_FROMQ,
		EDITTYPE_UNIVERSE,
		IDC_SQUEEZE_FROM,IDC_SQUEEZE_FROMSPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),

	// To
	ParamUIDesc(
		PB_TOQ,
		EDITTYPE_UNIVERSE,
		IDC_SQUEEZE_TO,IDC_SQUEEZE_TOSPIN,
		-BIGFLOAT,BIGFLOAT,		
		SPIN_AUTOSCALE),	

	// Bias
	ParamUIDesc(
		PB_BIAS,
		EDITTYPE_FLOAT,
		IDC_SQUEEZE_BIAS,IDC_SQUEEZE_BIASSPIN,
		-100.0f,100.0f,		
		1.0f,
		stdPercentDim),	

	// Vol
	ParamUIDesc(
		PB_VOL,
		EDITTYPE_FLOAT,
		IDC_SQUEEZE_VOLUME,IDC_SQUEEZE_VOLUMESPIN,
		0.0f,1000.0f,		
		1.0f,
		stdPercentDim),	
		
};

#define PARAMDESC_LENGH 9


static ParamBlockDescID descVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },

	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_FLOAT, NULL, TRUE, 5 },
	{ TYPE_FLOAT, NULL, TRUE, 6 },

	{ TYPE_FLOAT, NULL, TRUE, 7 },
	{ TYPE_FLOAT, NULL, TRUE, 8 },
};

#define PBLOCK_LENGTH	9

// Array of old versions
//static ParamVersionDesc versions[] = {
//	ParamVersionDesc(descVer0,3,0)
//	};
#define NUM_OLDVERSIONS	0

// Current version
#define CURRENT_VERSION	0

static ParamVersionDesc curVersion(descVer0,PBLOCK_LENGTH,CURRENT_VERSION);


//--- SpindleDlgProc -------------------------------

class SpindleDlgProc : public ParamMapUserDlgProc {
	public:
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {}
	};
static SpindleDlgProc theSpindleProc;

BOOL SpindleDlgProc::DlgProc(
		TimeValue t,IParamMap *map,
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_X:
				case IDC_Y:
				case IDC_Z:
					break;		
				}
			break;

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_SQUEEZE_FROMSPIN: {
					float from, to;
					map->GetParamBlock()->GetValue(PB_FROMQ,t,from,FOREVER);
					map->GetParamBlock()->GetValue(PB_TOQ,t,to,FOREVER);
					if (from>to) {
						map->GetParamBlock()->SetValue(PB_TOQ,t,from);
						map->Invalidate();
						}
					break;
					}
				
				case IDC_SQUEEZE_TOSPIN: {
					float from, to;
					map->GetParamBlock()->GetValue(PB_FROMQ,t,from,FOREVER);
					map->GetParamBlock()->GetValue(PB_TOQ,t,to,FOREVER);
					if (from>to) {
						map->GetParamBlock()->SetValue(PB_FROMQ,t,to);
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


SpindleMod::SpindleMod()
{	MakeRefByID(FOREVER, SIMPMOD_PBLOCKREF, 
		CreateParameterBlock(descVer0, PBLOCK_LENGTH, CURRENT_VERSION));

	pblock->SetValue(PB_AMT, TimeValue(0), 0.0f);
	pblock->SetValue(PB_CRV, TimeValue(0), 2.0f);

	pblock->SetValue(PB_AMTQ, TimeValue(0), 0.0f);
	pblock->SetValue(PB_CRVQ, TimeValue(0), 2.0f);
	pblock->SetValue(PB_DOREGIONQ, TimeValue(0), 0);
	pblock->SetValue(PB_FROMQ, TimeValue(0), -50.0f);
	pblock->SetValue(PB_TOQ, TimeValue(0), 50.0f);

	pblock->SetValue(PB_BIAS, TimeValue(0), 0.0f);
	pblock->SetValue(PB_VOL, TimeValue(0), 1.0f);
}

IOResult SpindleMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
//	iload->RegisterPostLoadCallback(
//		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,SIMPMOD_PBLOCKREF));
	return IO_OK;
	}

void SpindleMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
	{
	SimpleMod::BeginEditParams(ip,flags,prev);
		
	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_SPINDLEPARAM),
		GetString(IDS_AP_PARAMETERS),
		0);	
	pmapParam->SetUserDlgProc(&theSpindleProc);
	}
		
void SpindleMod::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
	{
	SimpleMod::EndEditParams(ip,flags,next);
	DestroyCPParamMap(pmapParam);
	}

Interval SpindleMod::GetValidity(TimeValue t)
{	float f;	
	Interval valid = FOREVER;
	pblock->GetValue(PB_AMT,t,f,valid);
	pblock->GetValue(PB_CRV,t,f,valid);

	pblock->GetValue(PB_AMTQ,t,f,valid);
	pblock->GetValue(PB_CRVQ,t,f,valid);
	pblock->GetValue(PB_FROMQ,t,f,valid);
	pblock->GetValue(PB_TOQ,t,f,valid);

	pblock->GetValue(PB_BIAS,t,f,valid);
	pblock->GetValue(PB_VOL,t,f,valid);
	return valid;
}

BOOL SpindleMod::GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis)
{	int limit;
	pblock->GetValue(PB_DOREGIONQ,t,limit,FOREVER);
	pblock->GetValue(PB_FROMQ,t,zmin,FOREVER);
	pblock->GetValue(PB_TOQ,t,zmax,FOREVER);
	axis=2;
	return limit?TRUE:FALSE;
}

RefTargetHandle SpindleMod::Clone(RemapDir& remap) 
	{	
	SpindleMod* newmod = new SpindleMod();
	newmod->ReplaceReference(SIMPMOD_PBLOCKREF,pblock->Clone(remap));
	newmod->SimpleModClone(this);
	return(newmod);
	}

SpindleDeformer::SpindleDeformer() 
{ 	tm.IdentityMatrix();
	time = 0;	
}

void SpindleDeformer::SetAxis(Matrix3 &tmAxis)
{	Matrix3 itm = Inverse(tmAxis);
	tm    = tm*tmAxis;
	invtm =	itm*invtm;
}

Point3 SpindleDeformer::Map(int i, Point3 p)
{	float sqeez, len, maxR;

	Point3 wvector;

	len = 0.5f*(bbox.pmax[2]-bbox.pmin[2]);

	wvector = (bbox.pmax - bbox.pmin);
	wvector[2] = 0.0f;
	maxR = Length(wvector)*0.5f;

	if (FloatEQ0(len)||FloatEQ0(maxR)) return p;

	p = p * tm;

	float eff_r, bulge, radiusfalloff;

	eff_r = (float)sqrt(p.x*p.x + p.y*p.y)/maxR;

	radiusfalloff = 1.0f/(1.0f+float(pow(eff_r,ks2)));

	bulge = kvol*(1.0f+kbias)*((float)fabs(p.z)/len)*ks1*radiusfalloff;

	if (doRegionQ)
	{	if (p.z<fromQ)
		{	sqeez = (float)fabs(fromQ)/len;
		}
		else if (p.z>toQ)
		{	sqeez = (float)fabs(toQ)/len;
		}
		else
		{	sqeez = (float)fabs(p.z)/len;
		}
	}
	else
	{	sqeez = (float)fabs(p.z)/len;
	}	

//	sqeez =  1.0f + sqeez*kq1 + kq2*sqeez*(1.0f - sqeez);
	sqeez =  (1.0f/(1.0f+(float)fabs(kq1))) + 
			 kvol*(1.0f-kbias)*(kq1/(1.0f+(float)fabs(kq1)))*(float)pow(sqeez,kq2);

  	p.x *= sqeez;
  	p.y *= sqeez;
	p.z *= (1.0f + bulge);

	p = p * invtm;

	return p;
}

SpindleDeformer::SpindleDeformer(
		TimeValue t, ModContext &mc, 
		float  amt, float crv,
		float  amtQ, float crvQ,
		float fromQ, float toQ, int doRegionQ,
		float bias, float vol,
 		Matrix3& modmat, Matrix3& modinv)
{	this->doRegionQ = doRegionQ;
	this->fromQ = fromQ;
	this->toQ   = toQ;
	Interval valid;
	Matrix3 mat;	
	time   = t;	
	tm = modmat;
	invtm = modinv;
	mat.IdentityMatrix();
	SetKS(amt,crv);
	SetKQ(amtQ,crvQ);
	SetBias(bias);
	SetVol(vol);
	SetAxis( mat );
	bbox = *mc.box;
	}


Deformer& SpindleMod::GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
{
	static SpindleDeformer deformer;
	float amt, crv;
	float amtQ, crvQ, fromQ, toQ;
	int doRegionQ;
	float bias,vol;

	pblock->GetValue(PB_AMT,t,amt,FOREVER);
	pblock->GetValue(PB_CRV,t,crv,FOREVER);

	pblock->GetValue(PB_AMTQ,t,amtQ,FOREVER);
	pblock->GetValue(PB_CRVQ,t,crvQ,FOREVER);
	pblock->GetValue(PB_FROMQ,t,fromQ,FOREVER);
	pblock->GetValue(PB_TOQ,t,toQ,FOREVER);
	pblock->GetValue(PB_DOREGIONQ,t,doRegionQ,FOREVER);

	pblock->GetValue(PB_BIAS,t,bias,FOREVER);
	pblock->GetValue(PB_VOL,t,vol,FOREVER);
	
	deformer = SpindleDeformer(t,mc,amt,crv,
									amtQ,crvQ,fromQ,toQ,doRegionQ,bias,vol,
									mat,invmat);
	return deformer;
}

ParamDimension *SpindleMod::GetParameterDim(int pbIndex)
{
	switch (pbIndex)
	{
		case PB_AMT: 		return stdNormalizedDim; 
		case PB_CRV:		return stdNormalizedDim;		

		case PB_AMTQ: 		return stdNormalizedDim; 
		case PB_CRVQ:		return stdNormalizedDim;		
		case PB_FROMQ:		return stdWorldDim;
		case PB_TOQ:		return stdWorldDim;

		case PB_VOL:		return stdPercentDim;
		case PB_BIAS:		return stdPercentDim;

		default:			return defaultDim;
	}
}

TSTR SpindleMod::GetParameterName(int pbIndex)
{	switch (pbIndex)
	{
		case PB_AMT:		return GetString(IDS_EP_AMOUNT);
		case PB_CRV:		return GetString(IDS_EP_CURVITURE);

		case PB_AMTQ:		return GetString(IDS_EP_AMOUNTQ);
		case PB_CRVQ:		return GetString(IDS_EP_CURVITUREQ);
		case PB_FROMQ:		return GetString(IDS_EP_FROMQ);
		case PB_TOQ:		return GetString(IDS_EP_TOQ);
		case PB_DOREGIONQ:	return GetString(IDS_EP_REGIONQ);

		case PB_BIAS:		return GetString(IDS_EP_BIAS);
		case PB_VOL:		return GetString(IDS_EP_VOLUME);

		default:			return TSTR(_T(""));
	}
}



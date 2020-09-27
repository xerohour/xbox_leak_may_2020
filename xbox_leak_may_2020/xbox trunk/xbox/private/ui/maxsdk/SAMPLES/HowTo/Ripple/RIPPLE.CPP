/**********************************************************************
 *<
	FILE: sinwave.cpp

	DESCRIPTION:  Simple WSM

	CREATED BY: Rolf Berteig

	HISTORY: created 9 February, 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
 /*
  Note: There are six plug-ins in this file:
  	- The Ripple Space Warp Object
	- The Ripple Space Warp Modifier
	  The two above are used to create the ripple space warp.

	- The Ripple Object Space Modifier
	
	- The Wave Space Warp Object
	- The Wave Space Warp Modifier
	  The two above are used to create the wave space warp.

	- The Wave Object Space Modifier
  */
// Standard MAX include file
#include "max.h"
#include "resource.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"
#include "texutil.h"


HINSTANCE hInstance;

TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

// Class IDs
#define SINEWAVE_OBJECT_CID Class_ID(0x67e85eb8, 0x7c91125d)
#define LINWAVE_OBJECT_CID Class_ID(0x537901c7, 0x28cd395b)

#define SINEWAVE_CID Class_ID(0x288b4629, 0x86c6d7b)
#define LINWAVE_CID Class_ID(0x6de925d3, 0x5a8122e7) 
				
#define SINEWAVE_OMOD_CID Class_ID(0x5fbc7724, 0x7d7153ae)
#define LINWAVE_OMOD_CID Class_ID(0x340b5ca6, 0x77f602a)

// The default value for the flexability parameter.
#define DEF_FLEX		float(1.0)

// This is the function we're usin'
float WaveFunc( float radius, TimeValue t, float amp, 
				float waveLen, float phase, float decay )
	{
	if (waveLen == float(0)) {
		waveLen = float(0.0000001);
		}
	return float( amp * sin( TWOPI * ( radius/waveLen + phase ) ) 
		* exp(-decay * (float)fabs(radius) ) );
	}

BOOL CALLBACK DefaultSOTProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	IObjParam *ip = (IObjParam*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_INITDIALOG:
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			return FALSE;

		default:
			return FALSE;
		}
	return TRUE;
	}

// This is the base class used by the ripple and wave space warp objects.
class WaveObject : public SimpleWSMObject {	
	public:		
		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		WaveObject();		

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
				
		// from object		
		int DoOwnSelectHilite() {return TRUE;}
		CreateMouseCallBack* GetCreateMouseCallBack();
		//int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);		

		// From SimpleObject		
		void InvalidateUI();
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);

		// WaveObject stuff		
		virtual int DialogID()=0;
		virtual int Circles()=0;
	};

// This is the ripple space warp object.
class SinWaveObject : public WaveObject {	
	public:				
		SinWaveObject();

		TCHAR *GetObjectName() {return GetString(IDS_RB_RIPPLE);}		
		Class_ID ClassID() {return SINEWAVE_OBJECT_CID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		IOResult Load(ILoad *iload);

		// From WSMObject
		Modifier *CreateWSMMod(INode *node);
		
		// From SimpleObject
		void MakeCircle(TimeValue t, Mesh &mesh, int startVert, int& face, float radius,float a1,float a2,float w,float s, float d, int numCircleSegs);
		void BuildMesh(TimeValue t);

		// WaveObject stuff
		int DialogID() {return IDD_SINWAVEPARAM1;}
		int Circles() {return 10;}
	};

// This is the wave space warp object.
class LinWaveObject : public WaveObject {	
	public:		
		LinWaveObject();

		TCHAR *GetObjectName() { return GetString(IDS_RB_WAVE); }
		Class_ID ClassID() { return LINWAVE_OBJECT_CID;}
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);
		
		// From SimpleObject		
		void BuildMesh(TimeValue t);

		// WaveObject stuff
		int DialogID() {return IDD_LINWAVEPARAM1;}
		int Circles() {return 4;}
	};

//--- ClassDescriptor and class vars ---------------------------------


IObjParam *SinWaveObject::ip        = NULL;
IParamMap *SinWaveObject::pmapParam = NULL;
HWND       SinWaveObject::hSot      = NULL;

class SinWaveClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new SinWaveObject; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_RIPPLE_CLASS); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() { return SINEWAVE_OBJECT_CID; }
	const TCHAR* 	Category() { return GetSpaceWarpCatString(SPACEWARP_CAT_GEOMDEF);}
	};

static SinWaveClassDesc swDesc;
ClassDesc* GetSinWaveObjDesc() { return &swDesc; }

class LinWaveClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new LinWaveObject; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_WAVE_CLASS); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() { return LINWAVE_OBJECT_CID; }
	const TCHAR* 	Category() { return GetSpaceWarpCatString(SPACEWARP_CAT_GEOMDEF);}
	};

static LinWaveClassDesc lwDesc;
ClassDesc* GetLinWaveObjDesc() { return &lwDesc; }

//--- SineWaveMod -----------------------------------------------------
// This is the base class used for the ripple and wave space warp modifiers.
class WaveMod : public SimpleWSMMod {
	public:		
		static IParamMap *pmapParam;

		WaveMod() {}
		WaveMod(INode *node,WaveObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_WAVEMOD); }
		SClass_ID SuperClassID() { return WSM_CLASS_ID; }		
		void DeleteThis() {delete this;}
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		void InvalidateUI() {if (pmapParam) pmapParam->Invalidate();}
	};

// This is the ripple space warp modifier.
class SinWaveMod : public WaveMod {
	public:		
		
		SinWaveMod() {}
		SinWaveMod(INode *node,SinWaveObject *obj) : WaveMod(node,obj) {}

		// From Animatable		
		SClass_ID SuperClassID() { return WSM_CLASS_ID; }
		Class_ID ClassID() { return SINEWAVE_CID; } 		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_RIPPLE_BINDING);}

		// From SimpleWSMMod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
	};

// This is the wave space warp modifier.
class LinWaveMod : public WaveMod {
	public:		
		
		LinWaveMod() {}
		LinWaveMod(INode *node,LinWaveObject *obj) : WaveMod(node,obj) {}

		// From Animatable		
		SClass_ID SuperClassID() { return WSM_CLASS_ID; }
		Class_ID ClassID() { return LINWAVE_CID; } 		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_WAVEBINDING);}

		// From SimpleWSMMod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
	};

//--- ClassDescriptor and class vars ---------------------------------

class SinWaveModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) { return new SinWaveMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_RIPPLE_CLASS); }
	SClass_ID		SuperClassID() { return WSM_CLASS_ID; }
	Class_ID		ClassID() { return SINEWAVE_CID; }
	const TCHAR* 	Category() { return _T("");}
	};

static SinWaveModClassDesc swModDesc;
ClassDesc* GetSinWaveModDesc() { return &swModDesc; }

class LinWaveModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) { return new LinWaveMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_WAVE_CLASS); }
	SClass_ID		SuperClassID() { return WSM_CLASS_ID; }
	Class_ID		ClassID() { return LINWAVE_CID; }
	const TCHAR* 	Category() { return _T("");}
	};

static LinWaveModClassDesc lwModDesc;
ClassDesc* GetLinWaveModDesc() { return &lwModDesc; }

IParamMap *WaveMod::pmapParam = NULL;


//--- Object Space Modifier versions -----------------------------------------
// This is the base class for the ripple and wave object space modifiers.
class WaveOMod : public SimpleMod {
	public:		
		static IParamMap *pmapParam;

		WaveOMod();

		// From Animatable
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_WAVEMOD); }		
		void DeleteThis() {delete this;}
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);		
		
		// From SimpleMod		
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		void InvalidateUI() {if (pmapParam) pmapParam->Invalidate();}

		virtual int DialogID()=0;
	};

// This is the wave object space modifier.
class SinWaveOMod : public WaveOMod {
	public:
		// From Animatable		
		Class_ID ClassID() { return SINEWAVE_OMOD_CID; } 		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_RIPPLE);}

		// From SimpleMod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
		int DialogID() {return IDD_SINWAVEOMODPARAM;}
	};


// This is the wave object space modifier.
class LinWaveOMod : public WaveOMod {
	public:		
		// From Animatable		
		Class_ID ClassID() { return LINWAVE_OMOD_CID; } 
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_WAVE);}

		// From SimpleMod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
		int DialogID() {return IDD_LINWAVEOMODPARAM;}
	};

//--- ClassDescriptor and class vars ---------------------------------

IParamMap *WaveOMod::pmapParam = NULL;


class SinWaveOModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new SinWaveOMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_RIPPLE_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return SINEWAVE_OMOD_CID; }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static SinWaveOModClassDesc swOModDesc;
ClassDesc* GetSinWaveOModDesc() { return &swOModDesc; }

class LinWaveOModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new LinWaveOMod;}
	const TCHAR *	ClassName() { return GetString(IDS_RB_WAVE_CLASS); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return LINWAVE_OMOD_CID; }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static LinWaveOModClassDesc lwOModDesc;
ClassDesc* GetLinWaveOModDesc() { return &lwOModDesc; }

//--- SineWaveObject Parameter map/block descriptors ------------------
// The parameter map descriptors define the properties of a parameter
// such as the type (spinner, radio button, check box, etc.), which
// resource ID they refer to, and which index into the virtual array
// they use.
#define PB_AMPLITUDE	0
#define PB_AMPLITUDE2	1
#define PB_WAVELEN		2
#define PB_PHASE		3
#define PB_DECAY		4

#define PB_CIRCLES		5
#define PB_SEGMENTS		6
#define PB_DIVISIONS	7

// This one is the older version.
static ParamUIDesc descParamObj[] = {
	
	// Amplitude 1
	ParamUIDesc(
		PB_AMPLITUDE,
		EDITTYPE_UNIVERSE,
		IDC_AMPLITUDE,IDC_AMPSPINNER,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Amplitude 2
	ParamUIDesc(
		PB_AMPLITUDE2,
		EDITTYPE_UNIVERSE,
		IDC_AMPLITUDE2,IDC_AMPSPINNER2,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Wave Length
	ParamUIDesc(
		PB_WAVELEN,
		EDITTYPE_UNIVERSE,
		IDC_WAVELEN,IDC_WAVELENSPINNER,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Phase
	ParamUIDesc(
		PB_PHASE,
		EDITTYPE_FLOAT,
		IDC_PHASE,IDC_PHASESPINNER,
		-9999999.0f, 9999999.0f,
		0.1f),
		
	// Decay
	ParamUIDesc(
		PB_DECAY,
		EDITTYPE_FLOAT,
		IDC_DECAY,IDC_DECAYSPINNER,
		0.0f, 9999999.0f,
		0.001f),
		
	// Circles
	ParamUIDesc(
		PB_CIRCLES,
		EDITTYPE_INT,
		IDC_CIRCLES,IDC_CIRCLESSPINNER,
		3.0f, 200.0f,
		0.1f),

	// Segments
	ParamUIDesc(
		PB_SEGMENTS,
		EDITTYPE_INT,
		IDC_SEGS,IDC_SEGSPINNER,
		3.0f, 200.0f,
		0.1f),
	
	// Divisions
	ParamUIDesc(
		PB_DIVISIONS,
		EDITTYPE_INT,
		IDC_DIVISIONS,IDC_DIVSPINNER,
		1.0f, 200.0f,
		0.1f)
	};
#define OBJPARAMDESC_LENGH 8


// The parameter block descriptor defines the type of value represented
// by the parameter (int, float, Color...) and if it is animated or not.

// This class requires these values to be initialized:
// { - ParameterType, 
//   - Not Used, must be set to NULL, 
//   - Flag which indicates if the parameter is animatable,
//   - ID of the parameter used to match a corresponding ID in the 
//     other version of the parameter
//  }
ParamBlockDescID descObjVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 } };

// This is the current version.
ParamBlockDescID descObjVer1[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 7 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 } };

#define OBJPBLOCK_LENGTH	8


// Array of old versions
static ParamVersionDesc versions[] = {
	ParamVersionDesc(descObjVer0,7,0)
	};
#define NUM_OLDVERSIONS	1

// Current version
#define CURRENT_OBJVERSION	1
static ParamVersionDesc curVersion(descObjVer1,OBJPBLOCK_LENGTH,CURRENT_OBJVERSION);



//--- SineWaveObject methods ------------------------------------------


WaveObject::WaveObject()
	{
	// Create a reference to the parameter block.
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(descObjVer1, OBJPBLOCK_LENGTH, CURRENT_OBJVERSION));
	assert(pblock);	
	}

// This method is called by the system when the user needs 
// to edit the items parameters in the command panel.  
void WaveObject::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
	{
	// Since we subclass off SimpleWSMObject we call it's version first.
	SimpleWSMObject::BeginEditParams(ip,flags,prev);
	this->ip = ip;

	if (pmapParam) {
		
		// Left over from last SinWave ceated
		pmapParam->SetParamBlock(pblock);
	} else {
		hSot = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_SINWAVE_SOT),
				DefaultSOTProc,
				GetString(IDS_RB_SOT), 
				(LPARAM)ip,APPENDROLL_CLOSED);

		// Gotta make a new one.
		// Here we create each new rollup page in the command panel
		// using our descriptors.
		pmapParam = CreateCPParamMap(
			descParamObj,OBJPARAMDESC_LENGH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(DialogID()),
			GetString(IDS_RB_PARAMETERS),
			0);		
		}
	}

// This is called by the system to terminate the editing of the
// parameters in the command panel.  
void WaveObject::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
	{		
	// Since we subclass off SimpleWSMObject we call it's version first.
	SimpleWSMObject::EndEditParams(ip,flags,next);
	this->ip = NULL;

	if (flags&END_EDIT_REMOVEUI ) {				
		DestroyCPParamMap(pmapParam);
		ip->DeleteRollupPage(hSot);
		pmapParam = NULL;		
		}	
	}

// Declare a class derived from CreateMouseCallBack to handle
// the user input during the creation phase of the item.
class SinWaveObjCreateCallBack: public CreateMouseCallBack {	
	WaveObject *ob;	
	Point3 p0, p1;
	IPoint2 sp0, sp1;
	public:
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
		void SetObj(WaveObject *obj) { ob = obj; }
	};

// This is the method that actually handles the user input
// during the item's creation phase.
int SinWaveObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {
	float w;	

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
			case 0:  // only happens with MOUSE_POINT msg
				sp0    = m;
				p0     = vpt->GetPointOnCP(m);
				mat.SetTrans(p0);
				break;
			case 1:								
				sp1 = m;		
				p1  = vpt->GetPointOnCP(m);
				w   = float(4)*Length(p0-p1)/ob->Circles();
				ob->pblock->SetValue(PB_WAVELEN,0,w);
				ob->pmapParam->Invalidate();
				break;
			case 2:	
				if ( Length(sp1-sp0)<3 ) return CREATE_ABORT;			
				w = vpt->GetCPDisp(p1,Point3(0,0,1),sp1,m);				
				ob->pblock->SetValue(PB_AMPLITUDE,0,w);
				ob->pblock->SetValue(PB_AMPLITUDE2,0,w);
				ob->pmapParam->Invalidate();				
				if (msg==MOUSE_POINT) 
					return CREATE_STOP;
				break;					   
			}
		}
	else
	if (msg == MOUSE_ABORT)
		return CREATE_ABORT;

	return TRUE;
	}

// A single instance of the callback object.
static SinWaveObjCreateCallBack sinwaveCreateCB;

// This method allows MAX to access and call our proc method to 
// handle the user input.
CreateMouseCallBack* WaveObject::GetCreateMouseCallBack()
	{
	sinwaveCreateCB.SetObj(this);
	return &sinwaveCreateCB;
	}

// Called when the UI controls need to be updated to reflect a new time.
void WaveObject::InvalidateUI() 
	{
	if (pmapParam) pmapParam->Invalidate();
	}

// The dimension of each parameter.
ParamDimension *WaveObject::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_AMPLITUDE:	return stdWorldDim;		
		case PB_AMPLITUDE2:	return stdWorldDim;	
		case PB_WAVELEN: 	return stdWorldDim;		
		case PB_PHASE:		return stdNormalizedDim;		
		case PB_DECAY:		return stdNormalizedDim;			
		default:			return defaultDim;
		}
	}

// The name of each parameter.
TSTR WaveObject::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_AMPLITUDE:	return GetString(IDS_RB_AMPLITUDE1);
		case PB_AMPLITUDE2:	return GetString(IDS_RB_AMPLITUDE2);
		case PB_WAVELEN:	return GetString(IDS_RB_WAVELEN);
		case PB_PHASE:		return GetString(IDS_RB_PHASE);
		case PB_DECAY:		return GetString(IDS_RB_DECAY);
		default:			return TSTR(_T(""));
		}
	}



//--- Circular sine wave ---------------------------------------------

SinWaveObject::SinWaveObject()
	{
	pblock->SetValue(PB_CIRCLES, TimeValue(0), 10);
	pblock->SetValue(PB_SEGMENTS, TimeValue(0), 16);
	pblock->SetValue(PB_DIVISIONS, TimeValue(0), 4);
	}

IOResult SinWaveObject::Load(ILoad *iload)
	{
	// This is the callback that corrects for any older versions
	// of the parameter block structure found in the MAX file 
	// being loaded.
	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,0));
	return IO_OK;
	}

// When the user binds an object to a space warp, a new modifier
// must be created which is added to the nodes WSM derived object.
// This is the method called by the system when it needs this
// new modifier.
Modifier *SinWaveObject::CreateWSMMod(INode *node)
	{
	return new SinWaveMod(node,this);
	}

// Make a copy of the object and the parameter block.
RefTargetHandle SinWaveObject::Clone(RemapDir& remap) 
	{
	SinWaveObject* newob = new SinWaveObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return(newob);
	}

static void MakeQuad(Face *f, int a,  int b , int c , int d, int sg, int dv = 0) {
	f[0].setVerts( a+dv, b+dv, c+dv);
	f[0].setSmGroup(sg);
	f[0].setEdgeVisFlags(1,1,0);
	f[1].setVerts( c+dv, d+dv, a+dv);
	f[1].setSmGroup(sg);
	f[1].setEdgeVisFlags(1,1,0);
	}

void SinWaveObject::MakeCircle(TimeValue t, Mesh &mesh, 
		int startVert, int& face, float radius,
		float a1,float a2,float w,float s, float d, int numCircleSegs)
	{
	float x, y, z, u, u2;			

	for ( int i = startVert; i < startVert+numCircleSegs; i++ ) {
		u = float(i-startVert) / float(numCircleSegs);
		x = radius * (float)cos( u * TWOPI );
		y = radius * (float)sin( u * TWOPI );		
		
		u2 = (u > 0.5f) ? (u-0.5f) : u;
		u2 = (u2 > 0.25f) ? (0.5f-u2) : u2;
		u2 = u2 * 4.0f;
		u2 = u2*u2;
		z = WaveFunc(radius, t, a1*(1.0f-u2) + a2*u2, w, s, d);

		if ( startVert == 0 ) {
			mesh.setVert( i+1, Point3(x, y, z) );
		} else {
			mesh.setVert( numCircleSegs + i, Point3(x, y, z) );
		
			if ( i < startVert+numCircleSegs-1 ) {
				MakeQuad(&(mesh.faces[face]), 
					i+numCircleSegs,i+numCircleSegs+1,i+1,i,1);
				face += 2;
			} else {
				MakeQuad(&(mesh.faces[face]), 
					i+numCircleSegs,startVert+numCircleSegs,startVert,i,1);
				face += 2;
				}
			}
		}
	}

// Builds the mesh representation for the ripple based on the
// state of it's parameters at the time requested.
void SinWaveObject::BuildMesh(TimeValue t)
	{		
	int startVert = 1, face = 0, nverts, nfaces, numCircles, numCircleSegs, divs;
	float radius = float(0);
	float dr;
	float a, a2, w, s, d;	

	ivalid = FOREVER;
	pblock->GetValue(PB_AMPLITUDE,t,a,ivalid);
	pblock->GetValue(PB_AMPLITUDE2,t,a2,ivalid);
	pblock->GetValue(PB_WAVELEN,t,w,ivalid);
	pblock->GetValue(PB_PHASE,t,s,ivalid);
	pblock->GetValue(PB_DECAY,t,d,ivalid);
	pblock->GetValue(PB_SEGMENTS,t,numCircleSegs,ivalid);
	pblock->GetValue(PB_CIRCLES,t,numCircles,ivalid);
	pblock->GetValue(PB_DIVISIONS,t,divs,ivalid);
	LimitValue(d,0.0f,float(1.0E30));

	dr     = w/float(divs);
	nverts = numCircles * numCircleSegs + 1;
	nfaces = (numCircles-1) * numCircleSegs * 2;
	mesh.setNumVerts(nverts);
	mesh.setNumFaces(nfaces);

	mesh.setVert( 0, Point3(0,0,0) );
	MakeCircle(t,mesh,0,face,radius, a, a2, w, s, d, numCircleSegs);

	for ( int i = 1; i < numCircles; i++ ) {
		MakeCircle(t,mesh,startVert,face,radius, a, a2, w, s, d, numCircleSegs);
		startVert += numCircleSegs;
		radius += dr;
		}
	
	mesh.InvalidateGeomCache();
	}


//--- Linear sine wave ---------------------------------------------

LinWaveObject::LinWaveObject()
	{
	pblock->SetValue(PB_CIRCLES, TimeValue(0), 4);
	pblock->SetValue(PB_SEGMENTS, TimeValue(0), 20);
	pblock->SetValue(PB_DIVISIONS, TimeValue(0), 10);
	}

// When the user binds an object to a space warp, a new modifier
// must be created which is added to the nodes WSM derived object.
// This is the method called by the system when it needs this
// new modifier.
Modifier *LinWaveObject::CreateWSMMod(INode *node)
	{
	return new LinWaveMod(node,this);
	}

// Make a copy of the object and the parameter block.
RefTargetHandle LinWaveObject::Clone(RemapDir& remap) 
	{
	LinWaveObject* newob = new LinWaveObject();	
	newob->ReplaceReference(0,pblock->Clone(remap));
	return(newob);
	}

// Builds the mesh representation for the wave based on the
// state of it's parameters at the time requested.
void LinWaveObject::BuildMesh(TimeValue t)
	{		
	int startVert = 1, face = 0, nverts, nfaces, numSides, numSegs, divs;
	float radius = float(0);
	float dx, dy, starty, startx;
	float a, a2, w, s, d, x, y, z, u;	
	int nv=0, nf=0, ix, den;

	ivalid = FOREVER;
	pblock->GetValue(PB_AMPLITUDE,t,a,ivalid);
	pblock->GetValue(PB_AMPLITUDE2,t,a2,ivalid);
	pblock->GetValue(PB_WAVELEN,t,w,ivalid);
	pblock->GetValue(PB_PHASE,t,s,ivalid);
	pblock->GetValue(PB_DECAY,t,d,ivalid);
	pblock->GetValue(PB_SEGMENTS,t,numSegs,ivalid);
	pblock->GetValue(PB_CIRCLES,t,numSides,ivalid);
	pblock->GetValue(PB_DIVISIONS,t,divs,ivalid);	
	LimitValue(d,0.0f,float(1.0E30));
	
	dy     = w/float(divs);
	dx     = dy * 4;
	starty = -float(numSegs)/2.0f * dy;
	startx = -float(numSides)/2.0f * dx;
	nverts = (numSides+1) * (numSegs+1);
	nfaces = (numSides) * numSegs * 2;
	mesh.setNumVerts(nverts);
	mesh.setNumFaces(nfaces);

	
	for (int i=0; i<=numSides; i++) {
		x   = startx + dx * float(i);		
		den = (int)(dx*numSides*0.5f);
		u   = (float)fabs(x/(den?den:0.00001f));		
		u   = u*u;
		//u = smoothstep(0.0f,1.0f,u);
		for (int j=0; j<=numSegs; j++) {
			y = starty + float(j) * dy;
			z = WaveFunc(y, t, a*(1.0f-u)+a2*u, w, s, d);
			mesh.setVert(nv++,Point3(x, y, z));			
			}
		}
		
	for (i=0; i<numSides; i++) {
		ix = i * (numSegs+1);
		for (int j=0; j<numSegs; j++) {
			MakeQuad(&(mesh.faces[nf]), 
				ix+numSegs+1+j, ix+numSegs+2+j, ix+1+j, ix+j, 1);				
			nf += 2;
			}
		}
	
	assert(nv==mesh.numVerts);
	assert(nf==mesh.numFaces);
	mesh.InvalidateGeomCache();
	}



/*----------------------------------------------------------------*/
// Sine Wave modifier


//--- Parameter map/block descriptors -----------------------------

#define PB_FLEX			0

//
//
// Parameters

static ParamUIDesc descParamMod[] = {
	// Angle
	ParamUIDesc(
		PB_FLEX,
		EDITTYPE_FLOAT,
		IDC_FLEX,IDC_FLEXSPINNER,
		-10.0f,10.0f,
		0.01f)
	};
#define MODPARAMDESC_LENGH 1

ParamBlockDescID descModVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 } };

#define MODPBLOCK_LENGTH	1

#define CURRENT_MODVERSION	0

//--- SinWaveDeformer  --------------------------------------

// This the the class whose Map method is called to deforms each point.
class SinWaveDeformer : public Deformer {
	public:
		float amp, amp2, wave, phase, decay, flex;
		TimeValue time;
		Matrix3 tm, itm;		
		Point3 Map(int i, Point3 p); 
	};

// This is the deformers callback method which gets called for each point. 
Point3 SinWaveDeformer::Map(int i, Point3 p)
	{
	Point3 pt = p * tm;
	float r, oldZ, u, a, len;
		
	if (amp!=amp2) {
		len  = Length(pt);
		if (len==0.0f) {
			a = amp;
		} else {
			u = (float)acos(pt.x/len)/PI;
	 		u = (u > 0.5) ? (1.0f-u) : u;
			u *= 2.0f;
	 		//u = u*u;
	 		u = smoothstep(0.0f,1.0f,u);
	 		a = amp*(1.0f-u) + amp2*u;
			}
	} else {
		a = amp;
		}	
	
	oldZ = pt.z;
	pt.z = float(0);	
	r    = Length(pt);
	pt.z = oldZ + flex * WaveFunc(r, time, a, wave, phase, decay);
	return pt * itm;
	}

// This the the class whose Map method is called to deforms each point.
class LinWaveDeformer : public Deformer {
	public:
		float amp, amp2, wave, phase, decay, flex, dist;
		TimeValue time;
		Matrix3 tm, itm;		
		Point3 Map(int i, Point3 p); 
	};

// This is the deformers callback method which gets called for each point. 
Point3 LinWaveDeformer::Map(int i, Point3 p)
	{
	Point3 pt = p * tm;	
	float u = (float)fabs(2.0f*pt.x/dist);
	u = u*u;
	pt.z += flex * WaveFunc(pt.y, time, amp*(1.0f-u)+amp2*u, wave, phase, decay);
	return pt * itm;
	}

//--- WaveMod methods -----------------------------------------


WaveMod::WaveMod(INode *node,WaveObject *obj)
	{	
	// Make the reference to the space warp node.
	// Note that the reference index (SIMPWSMMOD_NODEREF) and the 
	// reference target handle (node) are both part of the SimpleWSMMod
	// base class.
	MakeRefByID(FOREVER,SIMPWSMMOD_NODEREF,node);	
		// Make the reference to the parameter block.
	// Note that the reference index (SIMPWSMMOD_PBLOCKREF) and the 
	// reference target handle (pblock) are both part of the SimpleWSMMod
	// base class.
	MakeRefByID(FOREVER, SIMPWSMMOD_PBLOCKREF, 
		CreateParameterBlock(descModVer0, MODPBLOCK_LENGTH, CURRENT_MODVERSION));
	obRef = NULL;
	pblock->SetValue(PB_FLEX, TimeValue(0), DEF_FLEX);
	}

// This method is called by the system when the user needs 
// to edit the item's parameters in the command panel.  
void WaveMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	// Call the base class method first.
	SimpleWSMMod::BeginEditParams(ip,flags,prev);
		
	// Create the parameter map and rollup page.
	pmapParam = CreateCPParamMap(
		descParamMod,MODPARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_SINWAVEMODPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);		
	}
		
// This is called by the system to terminate the editing of the
// parameters in the command panel.  
void WaveMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleWSMMod::EndEditParams(ip,flags,next);
	DestroyCPParamMap(pmapParam);
	}

Interval WaveMod::GetValidity(TimeValue t) 
	{
	if (nodeRef) {
		// Start the interval at FOREVER.
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;
		pblock->GetValue(PB_FLEX,t,f,valid);
		SinWaveObject *obj = (SinWaveObject*)GetWSMObject(t);
		// Whittle the interval down using each of the animated parameters
		// of the modifier.
		obj->pblock->GetValue(PB_AMPLITUDE,t,f,valid);
		obj->pblock->GetValue(PB_AMPLITUDE2,t,f,valid);
		obj->pblock->GetValue(PB_WAVELEN,t,f,valid);
		obj->pblock->GetValue(PB_PHASE,t,f,valid);
		obj->pblock->GetValue(PB_DECAY,t,f,valid);
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} else {
		return FOREVER;
		}
	}

ParamDimension *WaveMod::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_FLEX: 	return stdNormalizedDim;
		default:		return defaultDim;
		}	
	}

TSTR WaveMod::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_FLEX: 	return GetString(IDS_RB_FLEXIBILITY);
		default:		return GetString(IDS_RB_PARAMETERS);
		}
	}


// --- SinWaveMod -------
// Returns a reference to the callback object to handle the deformation.
Deformer& SinWaveMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	Interval valid;
	static SinWaveDeformer sd;
	pblock->GetValue(PB_FLEX,t,sd.flex,FOREVER);
	SinWaveObject *obj = (SinWaveObject*)GetWSMObject(t);
	obj->pblock->GetValue(PB_AMPLITUDE,t,sd.amp,FOREVER);
	obj->pblock->GetValue(PB_AMPLITUDE2,t,sd.amp2,FOREVER);
	obj->pblock->GetValue(PB_WAVELEN,t,sd.wave,FOREVER);
	obj->pblock->GetValue(PB_PHASE,t,sd.phase,FOREVER);
	obj->pblock->GetValue(PB_DECAY,t,sd.decay,FOREVER);
	LimitValue(sd.decay,0.0f,float(1.0E30));

	sd.time = t;
	//sd.itm  = nodeRef->GetNodeTM(t,&valid);
	sd.itm  = nodeRef->GetObjectTM(t,&valid);
	sd.tm   = Inverse( sd.itm );
	return sd;
	}

// This method makes a new modifier and copies the state of this modifier
// to the new modifier.  This is called when the user makes a copy of
// an object in the scene or when they press the make unique
// button in the modfier stack rollup page.
RefTargetHandle SinWaveMod::Clone(RemapDir& remap) 
	{
	SinWaveMod *newob = new SinWaveMod(nodeRef,(SinWaveObject*)obRef);
	newob->SimpleWSMModClone(this);
	return newob;
	}

// --- LinWaveMod -------
// Returns a reference to the callback object to handle the deformation.
Deformer& LinWaveMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	Interval valid;
	static LinWaveDeformer ld;
	pblock->GetValue(PB_FLEX,t,ld.flex,FOREVER);
	LinWaveObject *obj = (LinWaveObject*)GetWSMObject(t);
	obj->pblock->GetValue(PB_AMPLITUDE,t,ld.amp,FOREVER);
	obj->pblock->GetValue(PB_AMPLITUDE2,t,ld.amp2,FOREVER);
	obj->pblock->GetValue(PB_WAVELEN,t,ld.wave,FOREVER);
	obj->pblock->GetValue(PB_PHASE,t,ld.phase,FOREVER);
	obj->pblock->GetValue(PB_DECAY,t,ld.decay,FOREVER);
	LimitValue(ld.decay,0.0f,float(1.0E30));

	ld.time = t;
	//ld.itm  = nodeRef->GetNodeTM(t,&valid);
	ld.itm  = nodeRef->GetObjectTM(t,&valid);
	ld.tm   = Inverse(ld.itm);
	//ld.dist = mc.box ? mc.box->Width().x : (4.0f*ld.wave);
	//ld.dist = (2.0f*ld.wave); // Use wave length for the WSM version
		
	int numSides, divs;
	obj->pblock->GetValue(PB_CIRCLES,t,numSides,FOREVER);
	obj->pblock->GetValue(PB_DIVISIONS,t,divs,FOREVER);	
	ld.dist = (ld.wave/float(divs)) * 4.0f * float(numSides);

	if (ld.dist == 0.0f) ld.dist = 1.0f;
	return ld;
	}

// This method makes a new modifier and copies the state of this modifier
// to the new modifier.  This is called when the user makes a copy of
// an object in the scene or when they press the make unique
// button in the modfier stack rollup page.
RefTargetHandle LinWaveMod::Clone(RemapDir& remap) 
	{
	LinWaveMod *newob = new LinWaveMod(nodeRef,(LinWaveObject*)obRef);
	newob->SimpleWSMModClone(this);
	return newob;
	}



//--- Parameter map/block descriptors -----------------------------

//
//
// Parameters

static ParamUIDesc descParamOMod[] = {
	
	// Amplitude 1
	ParamUIDesc(
		PB_AMPLITUDE,
		EDITTYPE_UNIVERSE,
		IDC_AMPLITUDE,IDC_AMPSPINNER,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Amplitude 2
	ParamUIDesc(
		PB_AMPLITUDE2,
		EDITTYPE_UNIVERSE,
		IDC_AMPLITUDE2,IDC_AMPSPINNER2,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Wave Length
	ParamUIDesc(
		PB_WAVELEN,
		EDITTYPE_UNIVERSE,
		IDC_WAVELEN,IDC_WAVELENSPINNER,
		-9999999.0f, 9999999.0f,
		SPIN_AUTOSCALE),	

	// Phase
	ParamUIDesc(
		PB_PHASE,
		EDITTYPE_FLOAT,
		IDC_PHASE,IDC_PHASESPINNER,
		-9999999.0f, 9999999.0f,
		0.1f),
		
	// Decay
	ParamUIDesc(
		PB_DECAY,
		EDITTYPE_FLOAT,
		IDC_DECAY,IDC_DECAYSPINNER,
		0.0f, 9999999.0f,
		0.001f),
	};
#define OMODPARAMDESC_LENGH 5

// The parameter block descriptor defines the type of value represented
// by the parameter (int, float, Color...) and if it is animated or not.

// This class requires these values to be initialized:
// { - ParameterType, 
//   - Not Used, must be set to NULL, 
//   - Flag which indicates if the parameter is animatable,
//   - ID of the parameter used to match a corresponding ID in the 
//     other version of the parameter block.
//  }
ParamBlockDescID descOModVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_FLOAT, NULL, TRUE, 2 },
	{ TYPE_FLOAT, NULL, TRUE, 3 },
	{ TYPE_FLOAT, NULL, TRUE, 4 } };
#define OMODPBLOCK_LENGTH	5

#define CURRENT_OMODVERSION	0


//--- WaveOMod methods -----------------------------------------


WaveOMod::WaveOMod()
	{		
	// Create a reference to the parameter block.
	// SimpleMod maintains the parameter block pointer.
	// Note that SimpleMod uses two references itself.  It provides us
	// with a #define that specifies the reference index of the parameter
	// block.  This is SIMPMOD_PBLOCKREF.
	MakeRefByID(FOREVER, SIMPMOD_PBLOCKREF, 
		CreateParameterBlock(descOModVer0, OMODPBLOCK_LENGTH, CURRENT_OMODVERSION));
	
	pblock->SetValue(PB_WAVELEN,0,50.0f);
	}


// This method is called by the system when the user needs 
// to edit the modifiers parameters in the command panel.  
void WaveOMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	// Call the SimpleMod method first to take care of its processing.
	SimpleMod::BeginEditParams(ip,flags,prev);
		
	// Create the rollup page in the command panel using our descriptors.
	pmapParam = CreateCPParamMap(
		descParamOMod,OMODPARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(DialogID()),
		GetString(IDS_RB_PARAMETERS),
		0);		
	}
		
// This is called by the system to terminate the editing of the
// parameters in the command panel.  
void WaveOMod::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
	{
	SimpleMod::EndEditParams(ip,flags,next);
	DestroyCPParamMap(pmapParam);
	}

// This returns the vality interval of the modifier. The SimpleMod
// class calls this method and combines the result with the 
// validity interval of the controllers controlling the gizmo and center
// to form the total validity interval.
Interval WaveOMod::GetValidity(TimeValue t) 
	{	
	Interval valid = FOREVER;	
	float f;	
	pblock->GetValue(PB_AMPLITUDE,t,f,valid);
	pblock->GetValue(PB_AMPLITUDE2,t,f,valid);
	pblock->GetValue(PB_WAVELEN,t,f,valid);
	pblock->GetValue(PB_PHASE,t,f,valid);
	pblock->GetValue(PB_DECAY,t,f,valid);	
	return valid;	
	}

ParamDimension *WaveOMod::GetParameterDim(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_AMPLITUDE:	return stdWorldDim;			
		case PB_AMPLITUDE2:	return stdWorldDim;	
		case PB_WAVELEN: 	return stdWorldDim;		
		case PB_PHASE:		return stdNormalizedDim;		
		case PB_DECAY:		return stdNormalizedDim;			
		default:			return defaultDim;
		}
	}

TSTR WaveOMod::GetParameterName(int pbIndex) 
	{
	switch (pbIndex) {
		case PB_AMPLITUDE:	return GetString(IDS_RB_AMPLITUDE1);
		case PB_AMPLITUDE2:	return GetString(IDS_RB_AMPLITUDE2);
		case PB_WAVELEN:	return GetString(IDS_RB_WAVELEN);
		case PB_PHASE:		return GetString(IDS_RB_PHASE);
		case PB_DECAY:		return GetString(IDS_RB_DECAY);
		default:			return TSTR(_T(""));
		}
	}


//--- SinWaveOMod methods -----------------------------------------

// Returns a reference to the callback object to handle the deformation.
Deformer& SinWaveOMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	Interval valid;
	static SinWaveDeformer sd;	
	pblock->GetValue(PB_AMPLITUDE,t,sd.amp,FOREVER);
	pblock->GetValue(PB_AMPLITUDE2,t,sd.amp2,FOREVER);
	pblock->GetValue(PB_WAVELEN,t,sd.wave,FOREVER);
	pblock->GetValue(PB_PHASE,t,sd.phase,FOREVER);
	pblock->GetValue(PB_DECAY,t,sd.decay,FOREVER);
	LimitValue(sd.decay,0.0f,float(1.0E30));
	sd.time = t;
	sd.itm  = invmat;
	sd.tm   = mat;
	sd.flex = 1.0f;
	return sd;
	}

// This method makes a new modifier and copies the state of this modifier
// to the new modifier.  This is called when the user makes a copy of
// an object in the scene or when they press the make unique
// button in the modfier stack rollup page.
RefTargetHandle SinWaveOMod::Clone(RemapDir& remap) 
	{
	SinWaveOMod *newob = new SinWaveOMod();
	newob->ReplaceReference(SIMPMOD_PBLOCKREF,pblock->Clone(remap));
	newob->SimpleModClone(this);
	return newob;
	}


//--- LinWaveOMod methods -----------------------------------------

// Returns a reference to the callback object to handle the deformation.
Deformer& LinWaveOMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	Interval valid;
	static LinWaveDeformer ld;	
	pblock->GetValue(PB_AMPLITUDE,t,ld.amp,FOREVER);
	pblock->GetValue(PB_AMPLITUDE2,t,ld.amp2,FOREVER);
	pblock->GetValue(PB_WAVELEN,t,ld.wave,FOREVER);
	pblock->GetValue(PB_PHASE,t,ld.phase,FOREVER);
	pblock->GetValue(PB_DECAY,t,ld.decay,FOREVER);
	LimitValue(ld.decay,0.0f,float(1.0E30));
	ld.time = t;
	ld.itm  = invmat;
	ld.tm   = mat;
	ld.flex = 1.0f;
	ld.dist = mc.box ? mc.box->Width().x : (4.0f*ld.wave);
	return ld;
	}

// This method makes a new modifier and copies the state of this modifier
// to the new modifier.  This is called when the user makes a copy of
// an object in the scene or when they press the make unique
// button in the modfier stack rollup page.
RefTargetHandle LinWaveOMod::Clone(RemapDir& remap) 
	{
	LinWaveOMod *newob = new LinWaveOMod();
	newob->ReplaceReference(SIMPMOD_PBLOCKREF,pblock->Clone(remap));
	newob->SimpleModClone(this);
	return newob;
	}

// See the Advanced Topics section on DLL Functions and Class Descriptors
// for more information.
/*===========================================================================*\
 | The Class Descriptors
\*===========================================================================*/
// ***

// The following five functions are used by every plug-in DLL.
// See the Advanced Topics section on DLL Functions and Class Descriptors
// for more information.
/*===========================================================================*\
 | The DLL Functions
\*===========================================================================*/
// This function is called by Windows when the DLL is loaded for the first time
// or an executable links to the DLL. Since only one instance of MAX can be
// run at one time, this function will only get called once when MAX starts up.
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

	// Initialize MAX's custom controls
	InitCustomControls(hInstance);
		
	// Initialize Win95 controls
	InitCommonControls();
	
	return(TRUE);
	}

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int LibNumberClasses() {return 6;}

// This function return the ith class descriptor
__declspec( dllexport ) ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case 0: return GetSinWaveObjDesc();
		case 1: return GetSinWaveModDesc();
		case 2: return GetSinWaveOModDesc();

		case 3: return GetLinWaveObjDesc();
		case 4: return GetLinWaveModDesc();
		case 5: return GetLinWaveOModDesc();
		default: return 0;
		}
	}

// This function returns a string that describes the DLL.
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR *LibDescription() { return _T("Ripple-Wave"); }

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion() {  return VERSION_3DSMAX; }

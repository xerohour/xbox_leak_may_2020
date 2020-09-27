/**********************************************************************
 *<
	FILE: knot.cpp

	DESCRIPTION:  Defines a PQ-Torus Knot object class

	CREATED BY: Scott Morrison based on the torus MAX primitive

	HISTORY: created 30 November, 1995
	         Internationalized by Audrey Peterson 1996

 *>	Copyright (c) 1995,1996  All Rights Reserved.
 **********************************************************************/

#include "knotbase.h"
#include "surf_api.h"

#define KNOT_CLASS_ID 0x720

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

class KnotObject : public SimpleObject, public IParamArray {
public:
  // Class vars
  static IParamMap *pmapCreate;
  static IParamMap *pmapTypeIn;
  static IParamMap *pmapParam;
  static IObjParam *ip;
  static int dlgSegments, dlgSides;		
  static int dlgCreateMeth;
  static int dlgSmooth;	
  static int dlgBaseCurve;
  static float dlgLumps;	
  static float dlgLumpHeight;
  static float dlgP;
  static float dlgQ;
  static float dlgE;	
  static float dlgTwist;	
  static float dlgRadius2;	
  static float dlgWarpHeight;
  static float dlgWarpFreq;
  static Point3 crtPos;		
  static float crtRadius1;
  static float crtRadius2;	
  static int   dlgGenUV;
  static float dlgUTile;
  static float dlgVTile;
  static float dlgUOff;
  static float dlgVOff;
  
  friend void resetKnotParams(); // mjm - 1.26.99

  KnotObject();
		
  // From BaseObject
  CreateMouseCallBack* GetCreateMouseCallBack();
  void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);		
  void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
  TCHAR *GetObjectName() { return GetString(IDS_AP_KNOTNAME); }
  BOOL HasUVW();
  void SetGenUVW(BOOL sw);

  // Animatable methods		
  void DeleteThis() { delete this; }
  Class_ID ClassID() { return Class_ID( KNOT_CLASS_ID,0); }  
		
  // From ref
  RefTargetHandle Clone(RemapDir& remap = NoRemap());		
  IOResult Load(ILoad *iload);

  // From IParamArray
  BOOL SetValue(int i, TimeValue t, int v);
  BOOL SetValue(int i, TimeValue t, float v);
  BOOL SetValue(int i, TimeValue t, Point3 &v);
  BOOL GetValue(int i, TimeValue t, int &v, Interval &ivalid);
  BOOL GetValue(int i, TimeValue t, float &v, Interval &ivalid);
  BOOL GetValue(int i, TimeValue t, Point3 &v, Interval &ivalid);

  // From SimpleObject
  void BuildMesh(TimeValue t);
  BOOL OKtoDisplay(TimeValue t);
  void InvalidateUI();
  ParamDimension *GetParameterDim(int pbIndex);
  TSTR GetParameterName(int pbIndex);
  Object* ConvertToType(TimeValue t, Class_ID obtype);
  int CanConvertToType(Class_ID obtype);
  void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
};

// segments  = sides
// lsegments = segments

#define MIN_SEGMENTS	4
#define MAX_SEGMENTS	2000

#define MIN_SIDES	3
#define MAX_SIDES	200

#define MIN_RADIUS	float(0)
#define MAX_RADIUS	float( 1.0E30)
#define MIN_PQ	        float(1)
#define MAX_PQ 	        float(25)
#define MIN_E	        float(0.1)
#define MAX_E 	        float(10.0)
#define MIN_LUMPS       float(0)
#define MAX_LUMPS       float(500)
#define MIN_LUMP_HEIGHT float(-4.0)
#define MAX_LUMP_HEIGHT float(4.0)
#define MIN_ROTATION    float(0)
#define MAX_ROTATION    float(360)
#define MIN_TILE        float(-10000)
#define MAX_TILE        float(1000)
#define MIN_OFF         float(-10)
#define MAX_OFF         float(100)
#define MIN_WARP_HEIGHT float(-2)
#define MAX_WARP_HEIGHT  float(4)
#define MIN_WARP_FREQ   float(0)
#define MAX_WARP_FREQ   float(100)

#define DEF_SEGMENTS 	120
#define DEF_SIDES	12
#define DEF_P           2.0f
#define DEF_Q           3.0f
#define DEF_E           1.0f
#define DEF_TWIST       0.0f
#define DEF_RADIUS	0.1f
#define DEF_RADIUS2   	10.0f
#define DEF_LUMPS       0.0f
#define DEF_LUMP_HEIGHT 0.0f
#define DEF_GENUV       0
#define DEF_UTILE       1.0f
#define DEF_VTILE       1.0f
#define DEF_UOFF        0.0f
#define DEF_VOFF        0.0f
#define DEF_WARP_HEIGHT 0.0f
#define DEF_WARP_FREQ   0.0f

#define SMOOTH_ON	2
#define SMOOTH_SIDES	1
#define SMOOTH_OFF	0

#define BASE_KNOT       0
#define BASE_CIRCLE     1


//--- ClassDescriptor and class vars ---------------------------------

class KnotClassDesc:public ClassDesc {
public:									   int 			IsPublic() { return 1; }

  void *			Create(BOOL loading = FALSE) { return new KnotObject; }
  const TCHAR *	ClassName() { return GetString(IDS_AP_KNOTNAME_CDESC); }
  SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
  Class_ID		ClassID() { return Class_ID(KNOT_CLASS_ID,0); }
  const TCHAR* 	Category() { return GetString(IDS_AP_EXTENDED); }
  void			ResetClassParams(BOOL fileReset) { if(fileReset) resetKnotParams(); } // mjm - 1.26.99
};

static KnotClassDesc knotDesc;

ClassDesc* GetTorusKnotDesc() { return &knotDesc; }

// class variable for sphere class.
IObjParam *KnotObject::ip         = NULL;
int KnotObject::dlgSegments       = DEF_SEGMENTS;
int KnotObject::dlgSides          = DEF_SIDES;
int KnotObject::dlgCreateMeth     = 1; // create_radius
int KnotObject::dlgSmooth         = SMOOTH_ON;
int KnotObject::dlgBaseCurve      = BASE_KNOT;
float KnotObject::dlgRadius2      = DEF_RADIUS2;
IParamMap *KnotObject::pmapCreate = NULL;
IParamMap *KnotObject::pmapTypeIn = NULL;
IParamMap *KnotObject::pmapParam  = NULL;
Point3 KnotObject::crtPos         = Point3(0,0,0);
float KnotObject::crtRadius1      = 0.0f;
float KnotObject::crtRadius2      = DEF_RADIUS2;
float KnotObject::dlgP             = DEF_P;
float KnotObject::dlgQ             = DEF_Q;
float KnotObject::dlgE             = DEF_E;
float KnotObject::dlgTwist         = DEF_TWIST;
float KnotObject::dlgLumpHeight    = DEF_LUMP_HEIGHT;
float KnotObject::dlgLumps         = DEF_LUMPS;
int   KnotObject::dlgGenUV         = DEF_GENUV;
float KnotObject::dlgUTile         = DEF_UTILE;
float KnotObject::dlgVTile         = DEF_VTILE;
float KnotObject::dlgUOff          = DEF_UOFF;
float KnotObject::dlgVOff          = DEF_VOFF;
float KnotObject::dlgWarpHeight    = DEF_WARP_HEIGHT;
float KnotObject::dlgWarpFreq      = DEF_WARP_FREQ;

// mjm - begin - 1.26.99
void resetKnotParams() 
{
	KnotObject::dlgSegments		= DEF_SEGMENTS;
	KnotObject::dlgSides		= DEF_SIDES;
	KnotObject::dlgCreateMeth	= 1; // create_radius
	KnotObject::dlgSmooth		= SMOOTH_ON;
	KnotObject::dlgBaseCurve	= BASE_KNOT;
	KnotObject::dlgRadius2		= DEF_RADIUS2;
	KnotObject::crtPos			= Point3(0,0,0);
	KnotObject::crtRadius1      = 0.0f;
	KnotObject::crtRadius2		= DEF_RADIUS2;
	KnotObject::dlgP			= DEF_P;
	KnotObject::dlgQ			= DEF_Q;
	KnotObject::dlgE			= DEF_E;
	KnotObject::dlgTwist		= DEF_TWIST;
	KnotObject::dlgLumpHeight	= DEF_LUMP_HEIGHT;
	KnotObject::dlgLumps		= DEF_LUMPS;
	KnotObject::dlgGenUV		= DEF_GENUV;
	KnotObject::dlgUTile		= DEF_UTILE;
	KnotObject::dlgVTile		= DEF_VTILE;
	KnotObject::dlgUOff			= DEF_UOFF;
	KnotObject::dlgVOff			= DEF_VOFF;
	KnotObject::dlgWarpHeight	= DEF_WARP_HEIGHT;
	KnotObject::dlgWarpFreq		= DEF_WARP_FREQ;
}
// mjm - end


//--- Parameter map/block descriptors -------------------------------

// Parameter map indices
#define PB_RADIUS		0
#define PB_RADIUS2		1
#define PB_ROTATION		2
#define PB_TWIST		3
#define PB_SEGMENTS		4
#define PB_SIDES		5
#define PB_SMOOTH		6
#define PB_P	7
#define PB_Q	8
#define PB_E    9
#define PB_LUMPS    10
#define PB_LUMP_HEIGHT    11
#define PB_BASE_CURVE     12
#define PB_GENUV          13
#define PB_UTILE          14
#define PB_VTILE          15
#define PB_UOFF           16
#define PB_VOFF           17
#define PB_WARP_HEIGHT    18
#define PB_WARP_FREQ      19

// Non-parameter block indices
#define PB_CREATEMETHOD		0
#define PB_TI_POS			1
#define PB_TI_RADIUS1		2
#define PB_TI_RADIUS2		3


//
//
//	Creation method

static int createMethIDs[] = {IDC_CREATEDIAMETER,IDC_CREATERADIUS};

static ParamUIDesc descCreate[] = {
  // Diameter/radius
  ParamUIDesc(PB_CREATEMETHOD,TYPE_RADIO,createMethIDs,2)
};
#define CREATEDESC_LENGH 1


//
//
// Type in
static ParamUIDesc descTypeIn[] = {
	
  // Position
  ParamUIDesc(
              PB_TI_POS,
              EDITTYPE_UNIVERSE,
              IDC_TI_POSX,IDC_TI_POSXSPIN,
              IDC_TI_POSY,IDC_TI_POSYSPIN,
              IDC_TI_POSZ,IDC_TI_POSZSPIN,
              -99999999.0f,99999999.0f,
              SPIN_AUTOSCALE),
	
  // Radius 1
  ParamUIDesc(
              PB_TI_RADIUS1,
              EDITTYPE_UNIVERSE,
              IDC_RADIUS1,IDC_RADSPINNER1,
              MIN_RADIUS,MAX_RADIUS,
              SPIN_AUTOSCALE),
	
  // Radius 2
  ParamUIDesc(
              PB_TI_RADIUS2,
              EDITTYPE_UNIVERSE,
              IDC_RADIUS2,IDC_RAD2SPINNER,
              MIN_RADIUS,MAX_RADIUS,
              SPIN_AUTOSCALE),	
};
#define TYPEINDESC_LENGH 3


//
//
// Parameters

static int smoothIDs[] = {IDC_SMOOTH_NONE,IDC_SMOOTH_SIDES,IDC_SMOOTH_ALL};

static int curveIDs[] = {IDC_KNOT_CURVE,IDC_CIRCLE_CURVE};

static ParamUIDesc descParam[] = {
  // Radius
  ParamUIDesc(
              PB_RADIUS,
              EDITTYPE_UNIVERSE,
              IDC_RADIUS1,IDC_RADSPINNER1,
              MIN_RADIUS,MAX_RADIUS,
              SPIN_AUTOSCALE),	

  // Radius
  ParamUIDesc(
              PB_RADIUS2,
              EDITTYPE_UNIVERSE,
              IDC_RADIUS2,IDC_RAD2SPINNER,
              MIN_RADIUS,MAX_RADIUS,
              SPIN_AUTOSCALE),	

  // Rotation
  ParamUIDesc(
              PB_ROTATION,
              EDITTYPE_FLOAT,
              IDC_TORUS_ROT,IDC_TORUS_ROTSPIN,
              MIN_ROTATION,MAX_ROTATION,
              1.0f,
              stdAngleDim),	

  // Twist
  ParamUIDesc(
              PB_TWIST,
              EDITTYPE_FLOAT,
              IDC_TORUS_TWIST,IDC_TORUS_TWISTSPIN,
              0.0f,1000.0f,
              1.0f,
              defaultDim),	

  // Segments
  ParamUIDesc(
              PB_SEGMENTS,
              EDITTYPE_INT,
              IDC_SEGMENTS,IDC_SEGSPINNER,
              (float)MIN_SEGMENTS,(float)MAX_SEGMENTS,
              1.0f),
	
  // Sides
  ParamUIDesc(
              PB_SIDES,
              EDITTYPE_INT,
              IDC_SIDES,IDC_SIDESPINNER,
              (float)MIN_SIDES,(float)MAX_SIDES,
              1.0f),
	
  // Smooth	
  ParamUIDesc(PB_SMOOTH,TYPE_RADIO,smoothIDs,3),

  // P
  ParamUIDesc(
              PB_P,
              EDITTYPE_FLOAT,
              IDC_P,IDC_PSPIN,
              MIN_PQ,MAX_PQ,
              0.25f,
              defaultDim),	

  // Q
  ParamUIDesc(
              PB_Q,
              EDITTYPE_FLOAT,
              IDC_Q,IDC_QSPIN,
              MIN_PQ,MAX_PQ,		
              0.25f,
              defaultDim),

  // Ecentricity
  ParamUIDesc(
              PB_E,
              EDITTYPE_FLOAT,
              IDC_E,IDC_ESPIN,
              MIN_E,MAX_E,		
              0.05f,
              defaultDim),

  // Lumps
  ParamUIDesc(
              PB_LUMPS,
              EDITTYPE_FLOAT,
              IDC_LUMPS,IDC_LUMPSSPIN,
              MIN_LUMPS,MAX_LUMPS,		
              1.0f,
              defaultDim),

  // Lump Height
  ParamUIDesc(
              PB_LUMP_HEIGHT,
              EDITTYPE_FLOAT,
              IDC_LUMP_HEIGHT,IDC_LUMP_HEIGHT_SPIN,
              MIN_LUMP_HEIGHT,MAX_LUMP_HEIGHT,		
              0.01f,
              defaultDim),

  // Base Curve
  ParamUIDesc(PB_BASE_CURVE,TYPE_RADIO,curveIDs,2),

  // Generate UV
  ParamUIDesc(PB_GENUV, TYPE_SINGLECHEKBOX, IDC_GENUVS),

  // U Tile
  ParamUIDesc(
              PB_UTILE,
              EDITTYPE_FLOAT,
              IDC_UTILE,IDC_UTILESPIN,
              MIN_TILE,MAX_TILE,		
              0.1f,
              defaultDim),
  // V Tile
  ParamUIDesc(
              PB_VTILE,
              EDITTYPE_FLOAT,
              IDC_VTILE,IDC_VTILESPIN,
              MIN_TILE,MAX_TILE,		
              0.1f,
              defaultDim),
  

  // U Off
  ParamUIDesc(
              PB_UOFF,
              EDITTYPE_FLOAT,
              IDC_UOFF,IDC_UOFFSPIN,
              MIN_OFF,MAX_OFF,		
              0.1f,
              defaultDim),
  // V Off
  ParamUIDesc(
              PB_VOFF,
              EDITTYPE_FLOAT,
              IDC_VOFF,IDC_VOFFSPIN,
              MIN_OFF,MAX_OFF,		
              0.1f,
              defaultDim),
  

  // Warp Height
  ParamUIDesc(
              PB_WARP_HEIGHT,
              EDITTYPE_FLOAT,
              IDC_WARP_HEIGHT,IDC_WARP_HEIGHT_SPIN,
              MIN_WARP_HEIGHT,MAX_WARP_HEIGHT,		
              0.01f,
              defaultDim),

  // Warp Freq
  ParamUIDesc(
              PB_WARP_FREQ,
              EDITTYPE_FLOAT,
              IDC_WARP_FREQ,IDC_WARP_FREQ_SPIN,
              MIN_WARP_FREQ,MAX_WARP_FREQ,		
              1.0f,
              defaultDim),

};
#define PARAMDESC_LENGH 20

static ParamBlockDescID descVer0[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_INT, NULL, TRUE, 2 },
  { TYPE_INT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 } };

static ParamBlockDescID descVer1[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_INT, NULL, TRUE, 2 },
  { TYPE_INT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_FLOAT, NULL, TRUE, 6 },
  { TYPE_FLOAT, NULL, TRUE, 7 } };

static ParamBlockDescID descVer2[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 },
  { TYPE_INT, NULL, TRUE, 2 },
  { TYPE_INT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_FLOAT, NULL, TRUE, 6 },
  { TYPE_FLOAT, NULL, TRUE, 7 } };

static ParamBlockDescID descVer3[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 2 },
  { TYPE_FLOAT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_INT, NULL, TRUE, 5 },
  { TYPE_INT, NULL, TRUE, 6 },
  { TYPE_INT, NULL, TRUE, 7 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 }, 
  { TYPE_FLOAT, NULL, TRUE, 10 }, 
  { TYPE_FLOAT, NULL, TRUE, 11 }, 
  { TYPE_FLOAT, NULL, TRUE, 12 } };

static ParamBlockDescID descVer4[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 2 },
  { TYPE_FLOAT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_INT, NULL, TRUE, 5 },
  { TYPE_INT, NULL, TRUE, 6 },
  { TYPE_INT, NULL, TRUE, 7 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 }, 
  { TYPE_FLOAT, NULL, TRUE, 10 }, 
  { TYPE_FLOAT, NULL, TRUE, 11 }, 
  { TYPE_FLOAT, NULL, TRUE, 12 },
  { TYPE_INT, NULL, TRUE, 13 } };

static ParamBlockDescID descVer5[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 2 },
  { TYPE_FLOAT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_INT, NULL, TRUE, 5 },
  { TYPE_INT, NULL, TRUE, 6 },
  { TYPE_INT, NULL, TRUE, 7 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 }, 
  { TYPE_FLOAT, NULL, TRUE, 10 }, 
  { TYPE_FLOAT, NULL, TRUE, 11 }, 
  { TYPE_FLOAT, NULL, TRUE, 12 },
  { TYPE_INT, NULL, TRUE, 13 },
  { TYPE_INT, NULL, TRUE, 14 },
  { TYPE_FLOAT, NULL, TRUE, 15 },
  { TYPE_FLOAT, NULL, TRUE, 16 },
  { TYPE_FLOAT, NULL, TRUE, 17 },
  { TYPE_FLOAT, NULL, TRUE, 18 } };

static ParamBlockDescID descVer6[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 2 },
  { TYPE_FLOAT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_INT, NULL, TRUE, 5 },
  { TYPE_INT, NULL, FALSE, 6 },
  { TYPE_INT, NULL, FALSE, 7 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 }, 
  { TYPE_FLOAT, NULL, TRUE, 10 }, 
  { TYPE_FLOAT, NULL, TRUE, 11 }, 
  { TYPE_FLOAT, NULL, TRUE, 12 },
  { TYPE_INT, NULL, FALSE, 13 },
  { TYPE_INT, NULL, FALSE, 14 },
  { TYPE_FLOAT, NULL, TRUE, 15 },
  { TYPE_FLOAT, NULL, TRUE, 16 },
  { TYPE_FLOAT, NULL, TRUE, 17 },
  { TYPE_FLOAT, NULL, TRUE, 18 },
  { TYPE_FLOAT, NULL, TRUE, 19 },
  { TYPE_FLOAT, NULL, TRUE, 20 } };

static ParamBlockDescID descVer7[] = {
  { TYPE_FLOAT, NULL, TRUE, 0 },
  { TYPE_FLOAT, NULL, TRUE, 1 },
  { TYPE_FLOAT, NULL, TRUE, 2 },
  { TYPE_FLOAT, NULL, TRUE, 3 },
  { TYPE_INT, NULL, TRUE, 4 },
  { TYPE_INT, NULL, TRUE, 5 },
  { TYPE_INT, NULL, FALSE, 6 },
  { TYPE_FLOAT, NULL, TRUE, 8 },
  { TYPE_FLOAT, NULL, TRUE, 9 }, 
  { TYPE_FLOAT, NULL, TRUE, 10 }, 
  { TYPE_FLOAT, NULL, TRUE, 11 }, 
  { TYPE_FLOAT, NULL, TRUE, 12 },
  { TYPE_INT, NULL, FALSE, 13 },
  { TYPE_INT, NULL, FALSE, 14 },
  { TYPE_FLOAT, NULL, TRUE, 15 },
  { TYPE_FLOAT, NULL, TRUE, 16 },
  { TYPE_FLOAT, NULL, TRUE, 17 },
  { TYPE_FLOAT, NULL, TRUE, 18 },
  { TYPE_FLOAT, NULL, TRUE, 19 },
  { TYPE_FLOAT, NULL, TRUE, 20 } };

#define PBLOCK_LENGTH	20

// Array of old versions
static ParamVersionDesc versions[] = {
  ParamVersionDesc(descVer0,5,0),
  ParamVersionDesc(descVer1,9,1),
  ParamVersionDesc(descVer2,9,2),
  ParamVersionDesc(descVer3,13,3),
  ParamVersionDesc(descVer4,14,4),
  ParamVersionDesc(descVer5,19,5),
  ParamVersionDesc(descVer6,19,6)
};

#define NUM_OLDVERSIONS	7

// Current version
static ParamVersionDesc curVersion(descVer7,PBLOCK_LENGTH,7);
#define CURRENT_VERSION	7


//--- TypeInDlgProc --------------------------------

class KnotTypeInDlgProc : public ParamMapUserDlgProc {
public:
  KnotObject *ob;

  KnotTypeInDlgProc(KnotObject *o) {ob=o;}
  BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
  void DeleteThis() {delete this;}
};

BOOL KnotTypeInDlgProc::DlgProc(TimeValue t,IParamMap *map,HWND
                                hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  switch (msg) {
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_TI_CREATE: {
      if (ob->crtRadius1==0.0) return TRUE;
					
      // We only want to set the value if the object is 
      // not in the scene.
      if (ob->TestAFlag(A_OBJ_CREATING)) {
        ob->pblock->SetValue(PB_RADIUS,0,ob->crtRadius1);
        ob->pblock->SetValue(PB_RADIUS2,0,ob->crtRadius2);
      }

      Matrix3 tm(1);
      tm.SetTrans(ob->crtPos);
      ob->ip->NonMouseCreate(tm);
      // NOTE that calling NonMouseCreate will cause this
      // object to be deleted. DO NOT DO ANYTHING BUT RETURN.
      return TRUE;	
    }
    }
    break;	
  }
  return FALSE;
}


class KnotParamDlgProc : public ParamMapUserDlgProc {
public:
  KnotObject *ob;

  KnotParamDlgProc(KnotObject *o) {ob=o;}
  BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
  void DeleteThis() {delete this;}
};

static HWND hDlg;

BOOL KnotParamDlgProc::DlgProc(TimeValue t,IParamMap *map,HWND
                                hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  int baseCurve;
  if (ob->pmapParam) {
    ob->pblock->GetValue(PB_BASE_CURVE,t,baseCurve,FOREVER);
    // Enable/disable controls
    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_P),
                 baseCurve == BASE_KNOT);
    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_Q),
                 baseCurve == BASE_KNOT);

    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_PSPIN),
                 baseCurve == BASE_KNOT);
    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_QSPIN),
                 baseCurve == BASE_KNOT);

    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_WARP_HEIGHT),
                 baseCurve == BASE_CIRCLE);
    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_WARP_FREQ),
                 baseCurve == BASE_CIRCLE);

    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_WARP_HEIGHT_SPIN),
                 baseCurve == BASE_CIRCLE);
    EnableWindow(GetDlgItem(ob->pmapParam->GetHWnd(), IDC_WARP_FREQ_SPIN),
                 baseCurve == BASE_CIRCLE);

  }
    
  return FALSE;
}


//--- Knot methods -------------------------------


KnotObject::KnotObject()
{	
  MakeRefByID(FOREVER, 0,
              CreateParameterBlock(descVer7, PBLOCK_LENGTH, CURRENT_VERSION));
  assert(pblock);

  pblock->SetValue(PB_SMOOTH,0,dlgSmooth);
  pblock->SetValue(PB_BASE_CURVE,0,dlgBaseCurve);
  pblock->SetValue(PB_SEGMENTS,0,dlgSegments);
  pblock->SetValue(PB_SIDES,0,dlgSides);	
  pblock->SetValue(PB_RADIUS,0,crtRadius1);
  pblock->SetValue(PB_RADIUS2,0,crtRadius2);	
  pblock->SetValue(PB_P,0,dlgP);	
  pblock->SetValue(PB_Q,0,dlgQ);	
  pblock->SetValue(PB_E,0,dlgE);	
  pblock->SetValue(PB_TWIST,0,dlgTwist);	
  pblock->SetValue(PB_LUMPS,0,dlgLumps);	
  pblock->SetValue(PB_LUMP_HEIGHT,0,dlgLumpHeight);	
  pblock->SetValue(PB_GENUV,0,dlgGenUV);	
  pblock->SetValue(PB_UTILE,0,dlgUTile);	
  pblock->SetValue(PB_VTILE,0,dlgVTile);	
  pblock->SetValue(PB_UOFF,0,dlgUOff);	
  pblock->SetValue(PB_VOFF,0,dlgVOff);	
  pblock->SetValue(PB_WARP_HEIGHT,0,dlgWarpHeight);	
  pblock->SetValue(PB_WARP_FREQ,0,dlgWarpFreq);	
}

IOResult KnotObject::Load(ILoad *iload) 
{
  iload->RegisterPostLoadCallback(new ParamBlockPLCB(versions,
                                                     NUM_OLDVERSIONS,
                                                     &curVersion,this,0));
  return IO_OK;
}


void KnotObject::BeginEditParams( IObjParam *ip, ULONG flags,
                                  Animatable *prev )
{
  SimpleObject::BeginEditParams(ip,flags,prev);
  this->ip = ip;

  if (pmapCreate && pmapParam && pmapTypeIn) {
		
    // Left over from last Knot ceated		
    pmapCreate->SetParamBlock(this);
    pmapTypeIn->SetParamBlock(this);
    pmapParam->SetParamBlock(pblock);
  } else {
		
    // Gotta make a new one.
    if (flags&BEGIN_EDIT_CREATE) {
      pmapCreate = CreateCPParamMap(descCreate,CREATEDESC_LENGH,
                                    this,
                                    ip,
                                    hInstance,
                                    MAKEINTRESOURCE(IDD_TORUSPARAM1),
                                    GetString(IDS_AP_CREATE_DIALOG),
                                    0);

			
      pmapTypeIn = CreateCPParamMap(descTypeIn,TYPEINDESC_LENGH,
                                    this,
                                    ip,
                                    hInstance,
                                    MAKEINTRESOURCE(IDD_TORUSPARAM3),
                                    GetString(IDS_AP_KEYBOARDENTRY),
                                    APPENDROLL_CLOSED);			
    }

    pmapParam = CreateCPParamMap(descParam,PARAMDESC_LENGH,
                                 pblock,
                                 ip,
                                 hInstance,
                                 MAKEINTRESOURCE(IDD_KNOTPARAM2),
                                 GetString(IDS_AP_PARAMETERS),
                                 0);
  }

  if(pmapTypeIn) {
    // A callback for the type in.
    pmapTypeIn->SetUserDlgProc(new KnotTypeInDlgProc(this));
  }
  if(pmapParam) {
    // A callback for the type in.
    pmapParam->SetUserDlgProc(new KnotParamDlgProc(this));
  }
}
		
void KnotObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
  SimpleObject::EndEditParams(ip,flags,next);
  this->ip = NULL;

  if (flags&END_EDIT_REMOVEUI ) {
    if (pmapCreate) DestroyCPParamMap(pmapCreate);
    if (pmapTypeIn) DestroyCPParamMap(pmapTypeIn);
    DestroyCPParamMap(pmapParam);
    pmapParam  = NULL;
    pmapTypeIn = NULL;
    pmapCreate = NULL;
  }

  // Save these values in class variables so the next object created will inherit them.
  pblock->GetValue(PB_SIDES,ip->GetTime(),dlgSides,FOREVER);
  pblock->GetValue(PB_SEGMENTS,ip->GetTime(),dlgSegments,FOREVER);
  pblock->GetValue(PB_SMOOTH,ip->GetTime(),dlgSmooth,FOREVER);	
  pblock->GetValue(PB_BASE_CURVE,ip->GetTime(),dlgBaseCurve,FOREVER);	
  pblock->GetValue(PB_RADIUS2,ip->GetTime(),dlgRadius2,FOREVER);
  pblock->GetValue(PB_P,ip->GetTime(),dlgP,FOREVER);
  pblock->GetValue(PB_Q,ip->GetTime(),dlgQ,FOREVER);
  pblock->GetValue(PB_E,ip->GetTime(),dlgE,FOREVER);
  pblock->GetValue(PB_TWIST,ip->GetTime(),dlgTwist,FOREVER);
  pblock->GetValue(PB_LUMPS,ip->GetTime(),dlgLumps,FOREVER);
  pblock->GetValue(PB_LUMP_HEIGHT,ip->GetTime(),dlgLumpHeight,FOREVER);
  pblock->GetValue(PB_GENUV,ip->GetTime(),dlgGenUV,FOREVER);
  pblock->GetValue(PB_UTILE,ip->GetTime(),dlgUTile,FOREVER);
  pblock->GetValue(PB_VTILE,ip->GetTime(),dlgVTile,FOREVER);
  pblock->GetValue(PB_UOFF,ip->GetTime(),dlgUOff,FOREVER);
  pblock->GetValue(PB_VOFF,ip->GetTime(),dlgVOff,FOREVER);
  pblock->GetValue(PB_WARP_HEIGHT,ip->GetTime(),dlgWarpHeight,FOREVER);
  pblock->GetValue(PB_WARP_FREQ,ip->GetTime(),dlgWarpFreq,FOREVER);
}


FrenetFrame::FrenetFrame(Curve3D spaceCurve, float t, float deltaT)
{
  Point3 f0 = (*spaceCurve)(t);
  Point3 f1 = (*spaceCurve)(t+deltaT);
  Point3 t0 = f1 - f0;
  Point3 n0 = f1 + f0;
  Point3 b = CrossProd(t0, n0);
  Point3 n = CrossProd(b, t0);
  float lengthN = Length(n);
  float lengthB = Length(b);
  if (lengthB != 0.0f)
    b = (1.0f / lengthB) * b;
  if (lengthN != 0.0F)
    n = (1.0f / lengthN) * n;
  mValue    = f0;
  mTangent  = t0;
  mNormal   = n;
  mBiNormal = b;
}

Point3 generalKnot(Curve3D spaceCurve, Curve2D planeCurve, float t,
                   float deltaT, float u)
{
  FrenetFrame frame(spaceCurve, t, deltaT);
  Point2 p2 = (*planeCurve)(u, t);
  Point3 p3 = frame.Value() + p2.x * frame.Normal() + p2.y * frame.BiNormal();
  return p3;
}

static float P, Q, E, Radius1, Radius2, Twist, Lumps, LumpHeight, Rotation,
             WarpHeight, WarpFreq;
int BaseCurve;

Point3 torusKnot(float t)
{
  float fac = 4.0f / (2.2f - (float)sin(t * Q));
  Point3 p;
  p.x = Radius1*(fac * (float)cos(P * t))*0.5f;
  p.y = Radius1*(fac * (float)sin(P * t))*0.5f;
  p.z = Radius1*(fac * (float)cos(Q * t))*0.5f;
  return p;
}

Point3 baseCircle(float t)
{
  Point3 p;
  float r = Radius1 + Radius1 * WarpHeight *
    (0.5f - 0.5f * (float) cos(WarpFreq*t));
  p.x = r*((float)cos(t));
  p.y = r*((float)sin(t));
  p.z = 0.0f;
  return p;
}

Point2 circle(float u, float t)
{
  float eInv = 1.0f / E;
  float theta = Twist * t;
  float r = Radius2 + Radius2* LumpHeight *
    (0.5f - 0.5f * (float)cos((t + Rotation) * Lumps));
  Point2 p(E * r * (float)sin(u), eInv * r * (float)cos(u));
  float c = (float) cos(theta);
  float s = (float) sin(theta);
  Point2 p2(p.x*c - p.y*s, p.x*s + p.y*c);
  return p2;
}
BOOL KnotObject::HasUVW() { 
	BOOL genUVs;
	Interval v;
	pblock->GetValue(PB_GENUV, 0, genUVs, v);
	return genUVs; 
	}

void KnotObject::SetGenUVW(BOOL sw) {  
	if (sw==HasUVW()) return;
	pblock->SetValue(PB_GENUV,0, sw);				
	}

void KnotObject::BuildMesh(TimeValue t)
{
	Point3 p;
	int ix, na, nb, nc, nd, jx, kx, jxTex, naTex, nbTex, ncTex, ndTex; // mjm - 5.25.99
	float delTheta, theta;
	float delPhi, phi;
	int sides, segs, smooth;
	float radius,radius2;
	float startTheta = 0.0f;
	int connect;
	int genUV;
	float uTile, vTile, uOff, vOff;
	
	// Start the validity interval at forever and widdle it down.
	ivalid = FOREVER;
	pblock->GetValue(PB_RADIUS,t,radius,ivalid);
	pblock->GetValue(PB_RADIUS2,t,radius2,ivalid);
	pblock->GetValue(PB_ROTATION,t,Rotation,ivalid);
	pblock->GetValue(PB_TWIST,t,Twist,ivalid);
	pblock->GetValue(PB_SEGMENTS,t,segs,ivalid);
	pblock->GetValue(PB_SIDES,t,sides,ivalid);
	pblock->GetValue(PB_SMOOTH,t,smooth,ivalid);
	pblock->GetValue(PB_BASE_CURVE,t,BaseCurve,ivalid);
	pblock->GetValue(PB_P,t,P,ivalid);
	pblock->GetValue(PB_Q,t,Q,ivalid);
	pblock->GetValue(PB_E,t,E,ivalid);
	pblock->GetValue(PB_LUMPS,t,Lumps,ivalid);
	pblock->GetValue(PB_LUMP_HEIGHT,t,LumpHeight,ivalid);
	pblock->GetValue(PB_GENUV,t,genUV,ivalid);
	pblock->GetValue(PB_UTILE,t,uTile,ivalid);
	pblock->GetValue(PB_VTILE,t,vTile,ivalid);
	pblock->GetValue(PB_UOFF,t,uOff,ivalid);
	pblock->GetValue(PB_VOFF,t,vOff,ivalid);
	pblock->GetValue(PB_WARP_HEIGHT,t,WarpHeight,ivalid);
	pblock->GetValue(PB_WARP_FREQ,t,WarpFreq,ivalid);
	LimitValue( radius, MIN_RADIUS, MAX_RADIUS );
	LimitValue( radius2, MIN_RADIUS, MAX_RADIUS );
	LimitValue( segs, MIN_SEGMENTS, MAX_SEGMENTS );
	LimitValue( sides, MIN_SIDES, MAX_SIDES );
	
	delTheta = (float)2.0*PI/(float)segs;
	delPhi   = (float)2.0*PI/(float)sides;
	
	// if P and Q are integers tie up the ends
	connect = (fabs(P - (int)P) < 0.00001 && fabs(Q - (int)Q) < 0.00001);
	if (BaseCurve == BASE_CIRCLE && fabs(WarpFreq - (int)WarpFreq) > 0.00001)
		connect = FALSE;
	
	int nVerts = (sides)*(connect?segs:segs+1); // mjm - 5.13.99
	int nTVerts = (sides+1)*(segs+1);			// mjm - 5.25.99
	int nFaces = 2*sides*segs;
	mesh.setNumVerts(nVerts);
	mesh.setNumFaces(nFaces);
	mesh.setSmoothFlags(smooth);
	
	if (genUV)
	{
		mesh.setNumTVerts(nTVerts); // mjm - 5.25.99
		mesh.setNumTVFaces(nFaces);
	}
	
	theta = startTheta;
	
// Make verts
	Radius1 = radius;
	Radius2 = radius2;
	float deltaT = (2.0f*PI) / 10000.0f;
	int noVertSeg = connect ? segs : segs+1; // mjm - 5.25.99 - if connected, no verts at last segment (same as first segment)
	int nV=0, nTV=0; // mjm - 5.25.99
	for (ix=0; ix<=segs; ix++)
	{
		phi = 0.0f;
		for (jx = 0; jx<=sides; jx++)
		{
			if (ix!=noVertSeg)		// mjm - 5.25.99 - if not skipping vertex segment
			{
				if (BaseCurve == BASE_KNOT)
					p = generalKnot(torusKnot, circle, theta, deltaT, phi);
				else
					p = generalKnot(baseCircle, circle, theta, deltaT, phi);
				if (jx!=sides)		// mjm - 5.25.99 - no verts at last side (same as first side)
					mesh.setVert(nV++, p);
			}
			if (genUV)				// mjm - 5.25.99 - always make texture verts at last segment and last side
				mesh.setTVert(nTV++, uOff + uTile * theta / (2.0f*PI), vOff + vTile * phi / (2.0f*PI), 0.0f);
			phi += delPhi;
		}
		theta += delTheta;
	}

// Make faces
	int nF=0;
	for(ix = 0; ix < segs; ix++)
	{
		jx    = ix * (sides);	// mjm - 5.13.99
		jxTex = ix * (sides+1); // mjm - 5.25.99
		for (kx = 0; kx<sides; kx++)
		{
			na = jx + kx;
			nb = (connect && ix == (segs-1)) ? kx : na+sides; // mjm - 5.13.99
			nd = (kx == (sides-1)) ? jx : na+1;
			nc = nb+nd-na;

			naTex = jxTex + kx;			// mjm - 5.25.99
			nbTex = naTex+sides+1;		// mjm - 5.25.99
			ndTex = naTex+1;			// mjm - 5.25.99
			ncTex = nbTex+ndTex-naTex;	// mjm - 5.25.99

			DWORD grp = 0;
			if (smooth==SMOOTH_SIDES)
			{
				if (kx == sides-1 && (sides&1))
					grp = (1<<2);
				else
					grp = (kx&1) ? (1<<0) : (1<<1);
			}
			else if (smooth > 0)
			{
				grp = 1;
			}
			
			mesh.faces[nF].setEdgeVisFlags(1,1,0);
			mesh.faces[nF].setSmGroup(grp);
			mesh.faces[nF].setVerts( na,nb,nc);
			if (genUV)
				mesh.tvFace[nF].setTVerts(naTex, nbTex, ncTex);
			nF++;
			
			mesh.faces[nF].setEdgeVisFlags(0,1,1);
			mesh.faces[nF].setSmGroup(grp);
			mesh.faces[nF].setVerts(na,nc,nd);
			if (genUV)
				mesh.tvFace[nF].setTVerts(naTex, ncTex, ndTex);
			nF++;
		}
	}
	
	mesh.InvalidateGeomCache();
	mesh.InvalidateTopologyCache();
}




class KnotObjCreateCallBack: public CreateMouseCallBack {
  KnotObject *ob;	
  Point3 p0, p1, p2;
  IPoint2 sp0,sp1,sp2;	
public:
  int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m,
            Matrix3& mat );
  void SetObj(KnotObject *obj) { ob = obj; }
};



int KnotObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags,
                                IPoint2 m, Matrix3& mat ) {
  float r, r2;
  Point3 center;
#ifdef _OSNAP
	if(msg == MOUSE_FREEMOVE)
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
    case 0:  // only happens with MOUSE_POINT msg				
      sp0 = m;
#ifdef _3D_CREATE
      p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
#else	
      p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
#endif
      mat.SetTrans(p0);
      break;
    case 1:
      mat.IdentityMatrix();
      sp1 = m;							   
#ifdef _3D_CREATE
      p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
#else
      p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
#endif
      if (ob->dlgCreateMeth) {	
        // radius	
        r = Length(p1-p0) - ob->crtRadius2;
        mat.SetTrans(p0);
      } else {
        // diameter
        Point3 center = (p0+p1)/float(2);
        r = Length(center-p0) - ob->crtRadius2;
        mat.SetTrans(center);  // Modify Node's transform
      }
      
      if (msg==MOUSE_POINT) {
          ob->suspendSnap = FALSE;
          if (Length(m-sp0)<3 || Length(p1-p0)<0.1f)
              return CREATE_ABORT;
      }
      
      ob->pblock->SetValue(PB_RADIUS,0,r);
      ob->pmapParam->Invalidate();
				
      if (flags&MOUSE_CTRL) {
        float ang = (float)atan2(p1.y-p0.y,p1.x-p0.x);					
        mat.PreRotateZ(ob->ip->SnapAngle(ang));
      }
      break;
			
    case 2:					
      center = mat.GetTrans();
      mat.IdentityMatrix();
      mat.SetTrans(center);

#ifdef _3D_CREATE
      p2  = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
#else
      p2  = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
#endif
      r   = Length(p1-p0);
      r2  = Length(p2-p0);

      ob->crtRadius2 = (float)fabs(r-r2)/2.0f;
      ob->pblock->SetValue(PB_RADIUS,0,(r2+r)/2);
      ob->pblock->SetValue(PB_RADIUS2,0,ob->crtRadius2);
      ob->pmapParam->Invalidate();
				
      if (flags&MOUSE_CTRL) {
        float ang = (float)atan2(p2.y-p0.y,p2.x-p0.x);					
        mat.PreRotateZ(ob->ip->SnapAngle(ang));
      }

      if (msg==MOUSE_POINT) {					
        return (Length(sp1-sp0)<3)?CREATE_ABORT:CREATE_STOP;
      }
      break;					   
    }
  }
  else
    if (msg == MOUSE_ABORT) {		
      return CREATE_ABORT;
    }

  return TRUE;
}


static KnotObjCreateCallBack knotCreateCB;

CreateMouseCallBack* KnotObject::GetCreateMouseCallBack() {
  knotCreateCB.SetObj(this);
  return(&knotCreateCB);
}

BOOL KnotObject::OKtoDisplay(TimeValue t) 
{
  float radius;
  pblock->GetValue(PB_RADIUS,t,radius,FOREVER);
  if (radius==0.0f) return FALSE;
  else return TRUE;
}


// From ParamArray
BOOL KnotObject::SetValue(int i, TimeValue t, int v) 
{
  switch (i) {
  case PB_CREATEMETHOD: dlgCreateMeth = v; break;
  }		
  return TRUE;
}

BOOL KnotObject::SetValue(int i, TimeValue t, float v)
{
  switch (i) {				
  case PB_TI_RADIUS1: crtRadius1 = v; break;
  case PB_TI_RADIUS2: crtRadius2 = v; break;
  }	
  return TRUE;
}

BOOL KnotObject::SetValue(int i, TimeValue t, Point3 &v) 
{
  switch (i) {
  case PB_TI_POS: crtPos = v; break;
  }		
  return TRUE;
}

BOOL KnotObject::GetValue(int i, TimeValue t, int &v, Interval &ivalid) 
{
  switch (i) {
  case PB_CREATEMETHOD: v = dlgCreateMeth; break;
  }
  return TRUE;
}

BOOL KnotObject::GetValue(int i, TimeValue t, float &v, Interval &ivalid) 
{	
  switch (i) {		
  case PB_TI_RADIUS1: v = crtRadius1; break;
  case PB_TI_RADIUS2: v = crtRadius2; break;
  }
  return TRUE;
}

BOOL KnotObject::GetValue(int i, TimeValue t, Point3 &v, Interval &ivalid) 
{	
  switch (i) {		
  case PB_TI_POS: v = crtPos; break;		
  }
  return TRUE;
}


void KnotObject::InvalidateUI() 
{
  if (pmapParam) pmapParam->Invalidate();
}

ParamDimension *KnotObject::GetParameterDim(int pbIndex) 
{
  switch (pbIndex) {
  case PB_RADIUS:		return stdWorldDim;
  case PB_RADIUS2:	return stdWorldDim;
  case PB_ROTATION:	return stdAngleDim;
  case PB_TWIST:		return defaultDim;
  case PB_SEGMENTS:	return stdSegmentsDim;
  case PB_SIDES:		return stdSegmentsDim;
  case PB_SMOOTH:		return stdNormalizedDim;
  case PB_BASE_CURVE:		return stdNormalizedDim;
  case PB_P:	return defaultDim;
  case PB_Q:	return defaultDim;
  case PB_E:	return defaultDim;
  case PB_LUMPS:	return defaultDim;
  case PB_LUMP_HEIGHT:	return defaultDim;
  case PB_GENUV:	return defaultDim;
  case PB_UTILE:	return defaultDim;
  case PB_VTILE:	return defaultDim;
  case PB_UOFF: 	return defaultDim;
  case PB_VOFF:	        return defaultDim;
  case PB_WARP_HEIGHT:  return defaultDim;
  case PB_WARP_FREQ:    return defaultDim;
  default: return defaultDim;
  }
}

TSTR KnotObject::GetParameterName(int pbIndex) 
{
  switch (pbIndex) {
  case PB_RADIUS:	return GetString(IDS_AP_RADIUS);		
  case PB_RADIUS2:	return GetString(IDS_AP_RADIUS2);
  case PB_ROTATION:	return GetString(IDS_AP_ROTATION);
  case PB_TWIST:	return GetString(IDS_AP_TWIST);
  case PB_SEGMENTS:	return GetString(IDS_AP_SEGMENTS);
  case PB_SIDES:	return GetString(IDS_AP_SIDES);
  case PB_SMOOTH:	return GetString(IDS_AP_SMOOTH);
  case PB_BASE_CURVE:	return GetString(IDS_AP_BASECURVE);
  case PB_P:	        return GetString(IDS_AP_P);
  case PB_Q:	        return GetString(IDS_AP_Q);		
  case PB_E:	        return GetString(IDS_AP_ECENTRICITY);		
  case PB_LUMPS:	return GetString(IDS_AP_LUMPS);		
  case PB_LUMP_HEIGHT:	return GetString(IDS_AP_LUMPHEIGHT);		
  case PB_GENUV:	return GetString(IDS_AP_GENUV);		
  case PB_UTILE:	return GetString(IDS_AP_UTILE);		
  case PB_VTILE:	return GetString(IDS_AP_VTILE);		
  case PB_UOFF: 	return GetString(IDS_AP_UOFFSET);		
  case PB_VOFF:	        return GetString(IDS_AP_VOFFSET);		
  case PB_WARP_HEIGHT:	return GetString(IDS_AP_WARPHEIGHT);		
  case PB_WARP_FREQ:	return GetString(IDS_AP_WARPFREQUENCY);		
  default: return TSTR(_T(""));
  }
}

RefTargetHandle KnotObject::Clone(RemapDir& remap) 
{
  KnotObject* newob = new KnotObject();	
  newob->ReplaceReference(0,pblock->Clone(remap));	
  newob->ivalid.SetEmpty();	
  return(newob);
}



#ifndef NO_NURBS

NURBSResult
ParameterRangeProc(double& uMin, double& uMax, double& vMin, double& vMax)
{
	uMin = vMin = 0.0;
	uMax = vMax = 2.0f*PI;

	return kNOk;
}

NURBSResult
EvaluateProc(double u, double v, Point3& pt)
{
	float deltaT = (2.0f*PI) / 10000.0f;
	float theta = (float)u;
	float phi = (float)v;

	if (BaseCurve == BASE_KNOT)
		pt = generalKnot(torusKnot, circle, theta, deltaT, phi);
	else
		pt = generalKnot(baseCircle, circle, theta, deltaT, phi);

	return kNOk;
}

#endif
Object* KnotObject::ConvertToType(TimeValue t, Class_ID obtype)
	{
#ifndef NO_NURBS
	if (obtype == EDITABLE_SURF_CLASS_ID) {
		NURBSSet nset;

		float uTile, vTile, uOff, vOff;
		int genUV;

		float radius, radius2;
		Interval ivalid;
		pblock->GetValue(PB_RADIUS,		t,	radius,		ivalid);
		pblock->GetValue(PB_RADIUS2,	t,	radius2,	ivalid);

		pblock->GetValue(PB_ROTATION,	t,	Rotation,	ivalid);
		pblock->GetValue(PB_TWIST,		t,	Twist,		ivalid);
		pblock->GetValue(PB_BASE_CURVE,	t,	BaseCurve,	ivalid);
		pblock->GetValue(PB_P,			t,	P,			ivalid);
		pblock->GetValue(PB_Q,			t,	Q,			ivalid);	
		pblock->GetValue(PB_E,			t,	E,			ivalid);	
		pblock->GetValue(PB_LUMPS,		t,	Lumps,		ivalid);	
		pblock->GetValue(PB_LUMP_HEIGHT,t,	LumpHeight,	ivalid);

		pblock->GetValue(PB_GENUV,		t,	genUV,		ivalid);	
		pblock->GetValue(PB_UTILE,		t,	uTile,		ivalid);	
		pblock->GetValue(PB_VTILE,		t,	vTile,		ivalid);	
		pblock->GetValue(PB_UOFF,		t,	uOff,		ivalid);	
		pblock->GetValue(PB_VOFF,		t,	vOff,		ivalid);

		pblock->GetValue(PB_WARP_HEIGHT,t,	WarpHeight,	ivalid);	
		pblock->GetValue(PB_WARP_FREQ,	t,	WarpFreq,	ivalid);

		LimitValue( radius, MIN_RADIUS, MAX_RADIUS );
		LimitValue( radius2, MIN_RADIUS, MAX_RADIUS );

		Radius1 = radius;
		Radius2 = radius2;


		NURBSProceeduralSurface pSurf((SurfParamRangeProc)ParameterRangeProc,
										(SurfEvalProc)EvaluateProc,
										(SurfEvalTan)NULL,
										(SurfEvalMixedProc)NULL);

		NURBSCVSurface nSurf;
		double tol = (radius > radius2) ? radius2 : radius;
		if (E > 1.0f) tol /= E; else tol *= E;
		tol /= 100.0;
		GenNURBSCVSurfaceProceedurally(&pSurf, tol, nSurf);

		if (genUV) {
			nSurf.SetGenerateUVs(TRUE);
			nSurf.SetTileOffset(0, uTile, vTile, uOff, vOff, 0.0f);
		}

		nset.AppendObject(&nSurf);

		Matrix3 mat;
		mat.IdentityMatrix();
		Object *ob = CreateNURBSObject(NULL, &nset, mat);

		ob->SetChannelValidity(TOPO_CHAN_NUM,FOREVER);
		ob->SetChannelValidity(GEOM_CHAN_NUM,FOREVER);
		ob->UnlockObject();
		return ob;
	} 
#endif

    return SimpleObject::ConvertToType(t,obtype);
	}

int KnotObject::CanConvertToType(Class_ID obtype)
	{
#ifndef NO_NURBS
	if (obtype==EDITABLE_SURF_CLASS_ID)
		return 1;
#endif

    return SimpleObject::CanConvertToType(obtype);
	}

void KnotObject::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
#ifndef NO_NURBS
    Class_ID id = EDITABLE_SURF_CLASS_ID;
    TSTR *name = new TSTR(GetString(IDS_SM_NURBS_SURFACE));
    clist.Append(1,&id);
    nlist.Append(1,&name);
#endif
}

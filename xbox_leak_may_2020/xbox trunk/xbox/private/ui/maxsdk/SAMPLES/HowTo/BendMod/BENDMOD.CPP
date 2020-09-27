/*===========================================================================*\
  How To Create an Object Modifier Plug-In

  FILE: BendMod.cpp

  DESCRIPTION:  Bend OSM

  CREATED BY: Dan Silva & Rolf Berteig

  HISTORY: created 30 Jauary, 1995

  Copyright (c) 1994, All Rights Reserved.
\*===========================================================================*/
#include "max.h"
#include "resource.h"
#include "iparamm.h"
#include "simpmod.h"
#include "simpobj.h"

#define BIGFLOAT	float(999999)

// The unique Class_ID of this modifier.  It is specified as 
// two 32-bit quantities.
#define BEND_CID Class_ID(0x21066be6, 0x462d11cd)

// This is the Class_ID for the world space modifier version of Bend
#define BENDWSM_CID Class_ID(0x73cc4a3d, 0x3a304fb5)

// This is the DLL instance handle passed in when the plug-in is 
// loaded at startup.
HINSTANCE hInstance;

// This function returns a pointer to a string in the string table of
// the resource library.  Note that this function maintains the buffer
// and that only one string is loaded at a time.  Therefore if you intend
// to use this string, you must copy to another buffer since it will 
// be overwritten on the next GetString() call.
TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

// The bend modifier is subclasses off the SimpleMod class that handles 
// the implementation of many methods from the base classes.
class BendMod : public SimpleMod {	
	public:
		static IParamMap *pmapParam;

		BendMod();

		// --- Interhited virtual methods of Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_BENDMOD); }  
		virtual Class_ID ClassID() { return BEND_CID;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);

		// --- Interhited virtual methods of ReferenceMaker
		IOResult Load(ILoad *iload);

		// --- Interhited virtual methods of ReferenceTarget
		RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// --- Interhited virtual methods of BaseObject
		TCHAR *GetObjectName() { return GetString(IDS_RB_BEND2);}

		// --- Interhited virtual methods of SimpleMod
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);		
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
		BOOL GetModLimits(TimeValue t,float &zmin, float &zmax, int &axis);
		void InvalidateUI() {if (pmapParam) pmapParam->Invalidate();}
	};

// This the the class whose Map method is called to deforms each point.
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
		// This is the deformers callback method which gets
		// called for each point. 
		Point3 Map(int i, Point3 p); 
	};

// This is the class to create the world space modifier version of Bend.
class BendWSM : public SimpleOSMToWSMObject {
	public:
		BendWSM() {}
		BendWSM(BendMod *m) : SimpleOSMToWSMObject(m) {}

		// --- Interhited virtual methods of Animatable
		void DeleteThis() { delete this; }
		SClass_ID SuperClassID() {return WSM_OBJECT_CLASS_ID;}
		Class_ID ClassID() {return BENDWSM_CID;} 

		// --- Interhited virtual methods of ReferenceTarget
		RefTargetHandle Clone(RemapDir& remap)
			{return (new BendWSM((BendMod*)mod->Clone(remap)))->SimpleOSMToWSMClone(this,remap);}

		// --- Interhited virtual methods of BaseObject
		TCHAR *GetObjectName() {return GetString(IDS_RB_BEND2);}
	};

//--- ClassDescriptor and class vars ---------------------------------

IParamMap *BendMod::pmapParam = NULL;


//--- Parameter map/block descriptors -------------------------------

//--- Parameter map/block descriptors -------------------------------
// The parameter map descriptors define the properties of a parameter
// such as the type (spinner, radio button, check box, etc.), which
// resource ID they refer to, and which index into the virtual array
// they use.
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


// The parameter block descriptor defines the type of value represented
// by the parameter (int, float, Color...) and if it is animated or not.

// This class requires these values to be initialized:
// { - ParameterType, 
//   - Not Used, must be set to NULL, 
//   - Flag which indicates if the parameter is animatable,
//   - ID of the parameter used to match a corresponding ID in the 
//     other version of the parameter block (used for backwards 
//     compatability.
//  }
// The old version of the pblock.
static ParamBlockDescID descVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, FALSE, 2 } };

// The current version
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

// Create an instance of a class derived from ParamMapUserDlgProc.  This 
// is used to provide special processing of controls in the rollup page.
class BendDlgProc : public ParamMapUserDlgProc {
	public:
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis() {}
	};
static BendDlgProc theBendProc;

// The dialog proc where the special processing of the limit controls is 
// handled.  This is used to ensure the limit spinners don't 'overlap'
// one another in value.
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
	// Create a reference to the parameter block.
	// SimpleMod maintains the parameter block pointer.
	// Note that SimpleMod uses two references itself.  It provides us
	// with a #define that specifies the reference index of the parameter
	// block.  This is SIMPMOD_PBLOCKREF.
	MakeRefByID(FOREVER, SIMPMOD_PBLOCKREF, 
		CreateParameterBlock(descVer1, PBLOCK_LENGTH, CURRENT_VERSION));
	
	pblock->SetValue(PB_AXIS, TimeValue(0), 2/*Z*/);
	}

// --- Interhited virtual methods of Animatable
// This method is called by the system when the user needs 
// to edit the modifiers parameters in the command panel.  
void BendMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	// Call the SimpleMod method first to take care of its processing.
	SimpleMod::BeginEditParams(ip,flags,prev);
		
	// Create the rollup page in the command panel using our descriptors.
	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_BENDPARAM),
		GetString(IDS_RB_PARAMETERS),
		0);	
	// Here is where the special processing of the limits controls is set.
	pmapParam->SetUserDlgProc(&theBendProc);
	}
		
// This is called by the system to terminate the editing of the
// parameters in the command panel.  
void BendMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	// We must call the SimpleMod method first to take care of its 
	// processing.
	SimpleMod::EndEditParams(ip,flags,next);
	// Remove the rollup page.
	DestroyCPParamMap(pmapParam);
	}

// --- Interhited virtual methods of ReferenceMaker
// Called by MAX when the modifier is loaded from disk.
IOResult BendMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	// This is the callback that corrects for any older versions
	// of the parameter block structure found in the MAX file 
	// being loaded.
	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,SIMPMOD_PBLOCKREF));
	return IO_OK;
	}

// --- Interhited virtual methods of ReferenceTarget
// This method makes a new BendMod and copies the state of this modifier
// to the new modifier.
// This method is called when the user makes a copy of
// an object in the scene or when they press the make unique
// button in the modfier stack rollup page.
RefTargetHandle BendMod::Clone(RemapDir& remap) {	
	BendMod* newmod = new BendMod();	
	newmod->ReplaceReference(SIMPMOD_PBLOCKREF,pblock->Clone(remap));
	newmod->SimpleModClone(this);
	return(newmod);
	}

// --- Interhited virtual methods of SimpleMod
// This returns the vality interval of the modifier. The SimpleMod
// class calls this method and combines the result with the 
// validity interval of the controllers controlling the gizmo and center
// to form the total validity interval.
Interval BendMod::GetValidity(TimeValue t)
	{
	float f;	
	// Start our interval at forever...
	Interval valid = FOREVER;
	// Intersect each parameters interval to narrow it down.
	pblock->GetValue(PB_ANGLE,t,f,valid);
	pblock->GetValue(PB_DIR,t,f,valid);	
	pblock->GetValue(PB_FROM,t,f,valid);
	pblock->GetValue(PB_TO,t,f,valid);
	// Return the final validity interval.
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

// This is the deformers callback method which actually gets
// called for each selected point. It deforms the point passed
// and returns it.
Point3 BendDeformer::Map(int i, Point3 p)
	{
	float x, y, c, s, yr;
	if (r==0 && !doRegion) return p;
	// The point is first transformed by the tm.  This is typical
	// for all modifiers. See the Advanced Topics section on the 
	// Geometry Pipeline for more details.
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
	// The point is finally transformed by the inverse of the tm.
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

// Provides a reference to our callback object to handle the deformation.
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

// Returns the dimension of the requested parameter.  The dimension is the 
// type and magnitude of the values used by that parameter.
ParamDimension *BendMod::GetParameterDim(int pbIndex)
	{
	switch (pbIndex) {
		case PB_ANGLE: 	return defaultDim; // Note: doesn't use angleDim 
										   // because the bend has been storing
										   // the angle in degrees not radians. 
										   // Changing this would invalidate 
										   // old files.
		case PB_DIR:	return defaultDim;
		case PB_FROM:	return stdWorldDim;
		case PB_TO:		return stdWorldDim;
		default:		return defaultDim;
		}
	}

// Returns the name of the parameter requested.
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

// See the Advanced Topics section on DLL Functions and Class Descriptors
// for more information.
/*===========================================================================*\
 | The Class Descriptor
\*===========================================================================*/
class BendClassDesc:public ClassDesc {
	public:
	// The IsPublic() method should return TRUE if the plug-in can be picked
	// and assigned by the user. Some plug-ins may be used privately by other
	// plug-ins implemented in the same DLL and should not appear in lists for
	// user to choose from -- these plug-ins would return FALSE.
	int 			IsPublic() { return 1; }
	// This is the method that actually creates a new instance of
	// a plug-in class.  The system calls the correspoding 
	// DeleteThis() method of the plug-in to free the memory.  Our 
	// implementations use 'new' and 'delete'.
	void *			Create(BOOL loading = FALSE) { return new BendMod; }
	// This is the name that appears on the button
	// in the MAX user interface.
	const TCHAR *	ClassName() { return GetString(IDS_RB_BEND_CLASS); }
	// The system calls this method at startup to determine the type of object
	// this is.  In our case, we're a geometric object so we return 
	// GEOMOBJECT_CLASS_ID.  The possible options are defined in PLUGAPI.H
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	// The system calls this method to retrieve the unique
	// class id for this item.
	Class_ID		ClassID() { return BEND_CID; }
	// The category is selected
	// in the bottom most drop down list in the create branch.
	// If this is set to be an exiting category (i.e. "Primatives", ...) then
	// the plug-in will appear in that category. If the category doesn't
	// yet exists then it is created.  We use the new How To category for
	// all the example plug-ins in the How To sections.
	const TCHAR* 	Category() { return GetString(IDS_RB_HOWTO);}
	};

static BendClassDesc bendDesc;
ClassDesc* GetBendModDesc() { return &bendDesc; }

class BendWSMClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) 
		{if (loading) return new BendWSM; else return new BendWSM(new BendMod);}
	const TCHAR *	ClassName() { return GetString(IDS_RB_BEND_CLASS); }
	SClass_ID		SuperClassID() { return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() { return BENDWSM_CID; }
	const TCHAR* 	Category() {return GetString(IDS_MODBASED);}
	};

static BendWSMClassDesc bendWSMDesc;
ClassDesc* GetBendWSMDesc() { return &bendWSMDesc; }

// The following five functions are used by every plug-in DLL.
/*===========================================================================*\
 | The DLL and Library Functions
\*===========================================================================*/
// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.
int controlsInit = FALSE;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

	if (! controlsInit) {
		controlsInit = TRUE;
		
		// Initialize MAX's custom controls
		InitCustomControls(hInstance);
		
		// Initialize Win95 controls
		InitCommonControls();
	}
	
	return(TRUE);
	}

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int LibNumberClasses() {return 2;}

// This function return the ith class descriptor
__declspec( dllexport ) ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case 0: return GetBendModDesc();		
		case 1: return GetBendWSMDesc();		
		default: return 0;
		}
	}

// This function returns a string that describes the DLL.  This string appears in 
// the File / Summary Info / Plug-In Info dialog box.
__declspec( dllexport ) const TCHAR *LibDescription() { 
	return GetString(IDS_LIB_DESC); 
	}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }








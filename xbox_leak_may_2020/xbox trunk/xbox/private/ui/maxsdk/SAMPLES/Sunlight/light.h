/**********************************************************************
 *<
	FILE: light.h

	DESCRIPTION:  Defines lights

	CREATED BY: Dan Silva

	HISTORY: created 2 January 1995
			 added directional and spot lights, Feb 95

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __LIGHT__H__ 

#define __LIGHT__H__

// reference #s
#define PBLOCK_REF 0
#define PROJMAP_REF 1

class GeneralLightCreateCallBack;
class TSpotLightClassDesc;

class GeneralLight: public GenLight {			   
	friend class GeneralLightCreateCallBack;
	friend class LightPostLoad;
	friend class TSpotLightClassDesc;
	friend BOOL CALLBACK GeneralLightParamDialogProc( HWND hDlg, UINT message,
		 WPARAM wParam, LPARAM lParam );
	friend void resetLightParams();

	// Class vars
	static Mesh staticMesh[2];
	static short meshBuilt;
	static HWND hGeneralLight;
	static HWND hSpotLight;
	static HWND hShadow;
	static IObjParam* iObjParams;
	static int dlgRed;
	static int dlgGreen;
	static int dlgBlue;
	static int dlgH;
	static int dlgS;
	static int dlgV;
	static float dlgIntensity;
	static float dlgHotsize;
	static float dlgFallsize;
	static float dlgTDist;
	static short dlgShowCone;
	static short dlgCrossover;
	static short dlgUseAtten;
	static float dlgAttenStart;
	static float dlgAttenEnd;
	static short dlgShowAtten;
	static short dlgShape;
	static float dlgAspect;
	static float dlgMapBias;
	static float dlgMapRange;
	static float dlgRayBias;
	static int dlgMapSize;

	static short globShadowType; 
	static short globAbsMapBias;
	static float globMapRange;
	static float globMapBias;
	static int   globMapSize;
	static float globRayBias;

	static ISpinnerControl *redSpin;
	static ISpinnerControl *greenSpin;
	static ISpinnerControl *blueSpin;
	static ISpinnerControl *hSpin;
	static ISpinnerControl *sSpin;
	static ISpinnerControl *vSpin;
	static ISpinnerControl *intensitySpin;
	static ISpinnerControl *hotsizeSpin;
	static ISpinnerControl *fallsizeSpin;
	static ISpinnerControl *tDistSpin;
	static ISpinnerControl *attenStartSpin;
	static ISpinnerControl *attenEndSpin;
	static ISpinnerControl *aspectSpin;
	static ISpinnerControl *mapBiasSpin;
	static ISpinnerControl *mapSizeSpin;
	static ISpinnerControl *mapRangeSpin;
	static ISpinnerControl *rayBiasSpin;
	static IColorSwatch *colorSwatch;
	
	// Object parameters
	IParamBlock *pblock;
	Texmap *projMap;   // a reference
	short updateHSVSpin;
	short type;
	short enable;
	short useLight;		// on/off toggle
	short useAtten;
	short attenDisplay;
	short coneDisplay;
	short shape;
	short shadow;
	short useGlobalShadowParams;
	short shadowType;  // 0: ShadowMap   1: RayTrace
	short absMapBias;
	short overshoot;
	short projector;
	int extDispFlags;
	Mesh spotMesh;
	Mesh *mesh;
	NameTab exclList;
	void BuildStaticMeshes();
	void BuildSpotMesh(float coneSize);
	void UpdateUI(TimeValue t);

	//  inherited virtual methods for Reference-management
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );

	void GetMat(TimeValue t, INode* inode, ViewExp *vpt, Matrix3& mat);
	
public:
	GeneralLight(int type);
	~GeneralLight();

	//  inherited virtual methods:

	// From GenLight
	GenLight *NewLight(int type) { return new GeneralLight(type); }

	// From BaseObject
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	void SetExtendedDisplay(int flags);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
	TCHAR *GetObjectName();

	void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp* vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp* vpt,  Box3& box );
	void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );

	// From Object
	ObjectState Eval(TimeValue time);
	void InitNodeName(TSTR& s);
	Interval ObjectValidity(TimeValue t);
	BOOL UsesWireColor()	{ return 1; }
	int DoOwnSelectHilite() { return 1; }		

	// From Light
	RefResult EvalLightState(TimeValue t, Interval& valid, LightState* cs);
#ifdef SIMPLEDIR
	BOOL IsSpot()	{ return type == FSPOT_LIGHT || type == TSPOT_LIGHT; }
#else
	BOOL IsSpot()	{ return type == FSPOT_LIGHT || type == TSPOT_LIGHT || type == DIR_LIGHT; }
#endif
	BOOL IsDir()	{ return type == DIR_LIGHT; }
	void SetUseLight(int onOff);
	BOOL GetUseLight(void)		{ return useLight; }
	void SetSpotShape(int s);
	int GetSpotShape(void)	{ return shape; }
	void SetHotspot(TimeValue time, float f);
	float GetHotspot(TimeValue t, Interval& valid = Interval(0,0));
	void SetFallsize(TimeValue time, float f);
	float GetFallsize(TimeValue t, Interval& valid = Interval(0,0));
	void SetAtten(TimeValue time, int which, float f);
	float GetAtten(TimeValue t, int which, Interval& valid = Interval(0,0));
	void SetTDist(TimeValue time, float f);
	float GetTDist(TimeValue t, Interval& valid = Interval(0,0));
	int GetShadowMethod() {return shadow?(shadowType==0?LIGHTSHADOW_MAPPED:LIGHTSHADOW_RAYTRACED):LIGHTSHADOW_NONE;}
	void ReloadShadCtrls(HWND hDlg, TimeValue t);	

	ObjLightDesc *CreateLightDesc(INode *n);
	
	// internal set/get routines
	void SetRGBColor(TimeValue t, Point3& rgb);
	Point3 GetRGBColor(TimeValue t, Interval &valid = Interval(0,0));
	void SetHSVColor(TimeValue t, Point3& hsv);
	Point3 GetHSVColor(TimeValue t, Interval &valid = Interval(0,0));
	void SetUpdateHSVSpin(int b)	{ updateHSVSpin = b; }
	void SetIntensity(TimeValue time, float f);
	float GetIntensity(TimeValue t, Interval& valid = Interval(0,0));
	void SetAspect(TimeValue t, float f);
	float GetAspect(TimeValue t, Interval& valid = Interval(0,0));
	void SetConeDisplay(int s, int notify=TRUE);
	BOOL GetConeDisplay(void)		{ return coneDisplay; }
	void GetConePoints(TimeValue t, float aspect, float angle, float dist, Point3 *q);
	void DrawCone(TimeValue t, GraphicsWindow *gw, float dist);
	int DrawConeAndLine(TimeValue t, INode* inode, GraphicsWindow *gw, int drawing );
	void SetUseAtten(int s);
	BOOL GetUseAtten(void)			{ return useAtten; }
	void SetAttenDisplay(int s);
	BOOL GetAttenDisplay(void)		{ return attenDisplay; }
	void GetAttenPoints(TimeValue t, float rad, Point3 *q);
	int DrawAtten(TimeValue t, INode *inode, GraphicsWindow *gw);
	void Enable(int enab) { enable = enab; }

	void FixHotFallConstraint();
	int GetUseGlobal() { return useGlobalShadowParams; }
	void SetUseGlobal(int a) {useGlobalShadowParams =a; }
	int GetShadow() { return shadow; }
	void SetShadow(int a) { shadow = a; }
	int GetShadowType() { return useGlobalShadowParams?globShadowType:shadowType; }
	void SetShadowType(int a) { 
		if (useGlobalShadowParams) 	globShadowType = a; 
		else shadowType = a;
		}
	void SetMapBias(TimeValue t, float f);
	float GetMapBias(TimeValue t, Interval& valid = Interval(0,0));
	void SetMapRange(TimeValue t, float f);
	float GetMapRange(TimeValue t, Interval& valid = Interval(0,0));
	void SetMapSize(TimeValue t, int f);
	int GetMapSize(TimeValue t, Interval& valid = Interval(0,0));
	void SetRayBias(TimeValue t, float f);
	float GetRayBias(TimeValue t, Interval& valid = Interval(0,0));
	int GetAbsMapBias() { return useGlobalShadowParams ? globAbsMapBias : absMapBias; }
	void SetAbsMapBias(int a) {
		if (useGlobalShadowParams) 	globAbsMapBias = a; 
		else absMapBias = a; 
		}
	int GetOvershoot() { return overshoot; }
	void SetOvershoot(int a);
	int GetProjector() { return projector; }
	void SetProjector(int a) { projector = a; }
	void DoBitmapFit(HWND hwnd, TimeValue t);
	NameTab* GetExclList() {return &exclList; }
	BOOL Include() {return exclList.TestFlag(NT_INCLUDE); }
	Texmap* GetProjMap() { return projMap;  }
	void SetProjMap(Texmap* pmap);
	void AssignProjectorMap(HWND hWnd);

	NameTab& GetExclusionList() {return exclList;}
	void SetExclusionList(NameTab &list) {exclList = list;}

	BOOL SetHotSpotControl(Control *c);
	BOOL SetFalloffControl(Control *c);
	BOOL SetColorControl(Control *c);
	Control* GetHotSpotControl();
	Control* GetFalloffControl();
	Control* GetColorControl();

	// Animatable methods
	void DeleteThis() { delete this; }
	Class_ID ClassID() { return  Class_ID(OMNI_LIGHT_CLASS_ID+type,0); }  
	void GetClassName(TSTR& s) { s = TSTR(_T("FOO")); }

	void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags); 
	
	int NumSubs() { return 1; }  
	Animatable* SubAnim(int i) { return pblock; }
	TSTR SubAnimName(int i) { return TSTR(_T("FOO"));}

	// From ref
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	int NumRefs() {return 2;}
	RefTargetHandle GetReference(int i) {
		 return (i==0)?(RefTargetHandle)pblock: (RefTargetHandle)projMap;
		 }
	void SetReference(int i, RefTargetHandle rtarg) {
		if (i==0)pblock=(IParamBlock*)rtarg; 
		else projMap = (Texmap *)rtarg;
		}

	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	LRESULT CALLBACK TrackViewWinProc( HWND hwnd,  UINT message, 
            WPARAM wParam,   LPARAM lParam ){return(0);}

	/*New call required by Athena*/

	//void SetAspect(TimeValue t, float f){if(!IsSpot()) return; pblock->SetValue( PB_ASPECT, t, f );
	//NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);}

	//float GetAspect(TimeValue t, Interval& valid = Interval(0,0)){if(!IsSpot()) return -1.0f; float f;
	//pblock->GetValue( PB_ASPECT, t, f, valid ); return f;}

	void SetUseAttenNear(int s) {NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);}
	BOOL GetUseAttenNear(void) {return FALSE;}
	void SetAttenNearDisplay(int s) {NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);}
	BOOL GetAttenNearDisplay(void) {return FALSE;}

	int Type();  // OMNI_LIGHT, TSPOT_LIGHT, DIR_LIGHT, FSPOT_LIGHT, TDIR_LIGHT
	void SetContrast(TimeValue t, float f){pblock->SetValue( 2, t, f ); NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);}
	float GetContrast(TimeValue t, Interval& valid = Interval(0,0)){float f; pblock->GetValue( 2, t, f, valid ); return f;}
	void SetAffectDiffuse(BOOL onOff){;}
	BOOL GetAffectDiffuse() {return FALSE;}
	void SetAffectSpecular(BOOL onOff){;}
	BOOL GetAffectSpecular() {return FALSE;}
};

#endif

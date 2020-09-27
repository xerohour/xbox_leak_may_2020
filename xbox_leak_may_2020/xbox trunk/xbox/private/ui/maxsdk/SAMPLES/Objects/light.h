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
#define SHADPROJMAP_REF 2
#define SHADTYPE_REF 3
#define EMITTER_REF 4

class GeneralLight;


class ProjMapDADMgr: public DADMgr {
	GeneralLight *gl;
	public:
		ProjMapDADMgr(GeneralLight *l=NULL) { gl = l;}
		void Init(GeneralLight *d) { gl = d; }
		// called on the draggee to see what if anything can be dragged from this x,y
		SClass_ID GetDragType(HWND hwnd, POINT p);
		// called on potential dropee to see if can drop type at this x,y
		BOOL OkToDrop(ReferenceTarget *dropThis, HWND hfrom, HWND hto, POINT p, SClass_ID type, BOOL isNew);
		int SlotOwner() { return OWNER_SCENE; }  
		ReferenceTarget *GetInstance(HWND hwnd, POINT p, SClass_ID type);
		void Drop(ReferenceTarget *dropThis, HWND hwnd, POINT p, SClass_ID type);
		BOOL AutoTooltip() { return TRUE; }
	};

class GeneralLightCreateCallBack;
class TSpotLightClassDesc;
class SetTypeRest;

class GeneralLight: public GenLight {			   
	friend class GeneralLightCreateCallBack;
	friend class LightPostLoad;
	friend class TSpotLightClassDesc;
	friend class ProjMapDADMgr;
	friend class SetTypeRest;
	friend class BaseObjLight;
	friend class OmniLight;
	friend class DirLight;
	friend class SpotLight;
	friend BOOL CALLBACK GeneralLightParamDialogProc( HWND hDlg, UINT message,
		 WPARAM wParam, LPARAM lParam );
	friend void resetLightParams();

	// Class vars
	static BOOL inCreate;
	static Mesh staticMesh[2];
	static short meshBuilt;
	static HWND hGeneralLight;
	static HWND hAttenLight;
	static HWND hSpotLight;
	static HWND hOmni;
	static HWND hShadow;
	static HWND hEmitter;
	static IObjParam* iObjParams;
	static int dlgRed;
	static int dlgGreen;
	static int dlgBlue;
	static int dlgH;
	static int dlgS;
	static int dlgV;
	static float dlgIntensity;
	static float dlgContrast;
	static float dlgDiffsoft;
	static float dlgHotsize;
	static float dlgFallsize;
	static float dlgTDist;
	static short dlgShowCone;
	static short dlgCrossover;
	static short dlgUseAtten;
	static float dlgAttenStart;
	static float dlgAttenEnd;
	static float dlgAtten1Start;
	static float dlgAtten1End;
	static short dlgShowAtten;
	static short dlgShowAttenNear;
	static short dlgShowDecay;
	static short dlgShape;
	static float dlgAspect;
//	static int dlgMapSize;
//	static float dlgMapBias;
//	static float dlgMapRange;
//	static float dlgRayBias;
	static float dlgDecayRadius;
	static short dlgAtmosShadows;
	static float dlgAtmosOpacity;
	static float dlgAtmosColamt;
	static short dlgLightAffectShadColor;
	static short dlgUseShadowColorMap;
	static Color24 dlgShadColor;
	static float dlgShadMult;
	static Class_ID dlgShadType;
	static short dlgUseGlobalShadowParams;

	static short dlgEmitterEnable;
	static float dlgEmitterEnergy;
	static short dlgEmitterDecayType;
	static int	 dlgEmitterCAPhotons;
	static int	 dlgEmitterGIPhotons;

	static short globShadowType; 
	static short globAbsMapBias;
	static float globMapRange;
	static float globMapBias;
	static int   globMapSize;
	static float globRayBias;
	static short globAtmosShadows;
	static float globAtmosOpacity;
	static float globAtmosColamt;

	static ISpinnerControl *redSpin;
	static ISpinnerControl *greenSpin;
	static ISpinnerControl *blueSpin;
	static ISpinnerControl *hSpin;
	static ISpinnerControl *sSpin;
	static ISpinnerControl *vSpin;
	static ISpinnerControl *intensitySpin;
	static ISpinnerControl *contrastSpin;
	static ISpinnerControl *diffsoftSpin;
	static ISpinnerControl *hotsizeSpin;
	static ISpinnerControl *fallsizeSpin;
	static ISpinnerControl *tDistSpin;
	static ISpinnerControl *attenStartSpin;
	static ISpinnerControl *attenEndSpin;
	static ISpinnerControl *attenStart1Spin;
	static ISpinnerControl *attenEnd1Spin;
	static ISpinnerControl *aspectSpin;
	static ISpinnerControl *decaySpin;
	static ISpinnerControl *atmosOpacitySpin;
	static ISpinnerControl *atmosColAmtSpin;
	static ISpinnerControl *shadMultSpin;
	static ISpinnerControl *emitterEnergySpin;
	static ISpinnerControl *emitterCAPhotonsSpin;
	static ISpinnerControl *emitterGIPhotonsSpin;
	static IColorSwatch *colorSwatch;
	static IColorSwatch *shadColorSwatch;
	static ICustButton *projMapName;
	static ICustButton *shadProjMapName;
	static ProjMapDADMgr projDADMgr;
	static ShadowParamDlg *shadParamDlg;
	
	// Object parameters
	IParamBlock *pblock;
	Texmap *projMap;   // a reference
	Texmap *shadProjMap;   // a reference
	ShadowType *shadType;    // a reference
	IParamBlock *temppb;
	IParamBlock *emitter; // a reference
	short updateHSVSpin;
	short type;
	short enable;
	short useLight;		// on/off toggle
	short useAtten;
	short attenDisplay;
	short decayDisplay;
	short useAttenNear;
	short attenNearDisplay;
	short coneDisplay;
	short shape;
	short shadow;
	short useGlobalShadowParams;
	short shadowType;  // 0: ShadowMap   1: RayTrace
	short absMapBias;
	short overshoot;
	short projector;
	short softenDiffuse;
	short affectDiffuse;
	short affectSpecular;
	short decayType;
	short ltAffectsShadow;
	short ambientOnly;
	int extDispFlags;
	float targDist; 
	short atmosShadows;
	float atmosOpacity;
	float atmosColAmt;
	short emitterEnable;
	float emitterEnergy;
	short emitterDecayType;
	int   emitterCAPhotons;
	int   emitterGIPhotons;
	Mesh spotMesh;
	Mesh *mesh;
	NameTab exclList;
	void BuildStaticMeshes();
	void BuildSpotMesh(float coneSize);
	void UpdateUI(TimeValue t);

	//  inherited virtual methods for Reference-management
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );

	void GetMat(TimeValue t, INode* inode, ViewExp *vpt, Matrix3& mat);
	BOOL IsCompatibleRenderer ( void );
	
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
	void UpdtShadowTypeList(HWND hwndDlg);
	void UpdtShadowTypeSel();
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
	int Type() { return type; }  // OMNI_LIGHT, TSPOT_LIGHT, DIR_LIGHT, FSPOT_LIGHT, TDIR_LIGHT
	void SetType(int t);             // OMNI_LIGHT, TSPOT_LIGHT, DIR_LIGHT, FSPOT_LIGHT, TDIR_LIGHT
#ifdef SIMPLEDIR
	BOOL IsSpot()	{ return type == FSPOT_LIGHT || type == TSPOT_LIGHT; }
#else
	BOOL IsSpot()	{ return type == FSPOT_LIGHT || type == TSPOT_LIGHT || type == DIR_LIGHT || type==TDIR_LIGHT; }
#endif
	BOOL IsDir()	{ return type == DIR_LIGHT|| type == TDIR_LIGHT; }
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
	void SetDecayRadius(TimeValue time, float f);
	float GetDecayRadius(TimeValue t, Interval& valid = Interval(0,0));
	int GetShadowMethod() {return shadow?(shadowType==0?LIGHTSHADOW_MAPPED:LIGHTSHADOW_RAYTRACED):LIGHTSHADOW_NONE;}
	void ReloadShadCtrls(HWND hDlg, TimeValue t);	

	ObjLightDesc *CreateLightDesc(INode *n);
	
	// GenLight routines
	void SetRGBColor(TimeValue t, Point3& rgb);
	Point3 GetRGBColor(TimeValue t, Interval &valid = Interval(0,0));
	void SetHSVColor(TimeValue t, Point3& hsv);
	Point3 GetHSVColor(TimeValue t, Interval &valid = Interval(0,0));
	void SetShadColor(TimeValue t, Point3& rgb);
	Point3 GetShadColor(TimeValue t, Interval &valid = Interval(0,0));
	void SetShadMult(TimeValue t, float m);
    float GetShadMult(TimeValue t, Interval &valid = Interval(0,0));
	BOOL GetLightAffectsShadow() { return ltAffectsShadow; }
	void SetLightAffectsShadow(BOOL b) { ltAffectsShadow = b; }
	void SetUpdateHSVSpin(int b)	{ updateHSVSpin = b; }
	void SetIntensity(TimeValue time, float f);
	float GetIntensity(TimeValue t, Interval& valid = Interval(0,0));
	void SetContrast(TimeValue time, float f);
	float GetContrast(TimeValue t, Interval& valid = Interval(0,0));
	void SetDiffuseSoft(TimeValue time, float f);
	float GetDiffuseSoft(TimeValue t, Interval& valid = Interval(0,0));
	void SetAspect(TimeValue t, float f);
	float GetAspect(TimeValue t, Interval& valid = Interval(0,0));
	void SetConeDisplay(int s, int notify=TRUE);
	BOOL GetConeDisplay(void)		{ return coneDisplay; }
	void DrawX(TimeValue t, float asp, int npts, float dist, GraphicsWindow *gw, int indx);
	void DrawSphereArcs(TimeValue t, GraphicsWindow *gw, float r, Point3 *q);
    int GetCirXPoints(TimeValue t, float angle, float dist, Point3 *q);
	void BoxDirPoints(TimeValue t, float angle, float dist, Box3 &box, Matrix3 *tm);
	void BoxPoints(TimeValue t, float angle, float dist, Box3 &box, Matrix3 *tm);
	int GetRectXPoints(TimeValue t, float angle, float dist, Point3 *q);
	void DrawCircleX(TimeValue t, GraphicsWindow *gw, float angle, float dist, Point3 *q);
	void DrawWarpRect(TimeValue t, GraphicsWindow *gw, float angle, float dist, Point3 *q);
	void DrawAttenCirOrRect(TimeValue t, GraphicsWindow *gw, float dist, BOOL froze, int uicol);
	void GetConePoints(TimeValue t, float aspect, float angle, float dist, Point3 *q);
	void DrawCone(TimeValue t, GraphicsWindow *gw, float dist);
	int DrawConeAndLine(TimeValue t, INode* inode, GraphicsWindow *gw, int drawing );
	void SetUseAtten(int s);
	BOOL GetUseAtten(void)			{ return useAtten; }
	void SetUseAttenNear(int s);	   	
	BOOL GetUseAttenNear(void)	   	{ return useAttenNear; }
	void SetAttenDisplay(int s);
	BOOL GetAttenDisplay(void)		{ return attenDisplay; }
	void SetAttenNearDisplay(int s);
	BOOL GetDecayDisplay(void)		{ return decayDisplay; }
	void SetDecayDisplay(int s);
	BOOL GetAttenNearDisplay(void)		{ return attenNearDisplay; }
	void GetAttenPoints(TimeValue t, float rad, Point3 *q);
	int DrawAtten(TimeValue t, INode *inode, GraphicsWindow *gw);
	void Enable(int enab) { enable = enab; }

	void BuildMeshes(BOOL isnew = TRUE);
	void FixHotFallConstraint();
	int GetUseGlobal() { return useGlobalShadowParams; }
	void SetUseGlobal(int a);
	int GetShadow() { return shadow; }
	void SetShadow(int a) { shadow = a; }
	int GetShadowType();
	void SetShadowType(int a);

	// Plug-in shadow generator
	void SetShadowGenerator(ShadowType *s);
	ShadowType * GetShadowGenerator() { return shadType; }
	ShadowType * ActiveShadowType();

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

	void SetAtmosShadows(TimeValue t, int onOff);
	int GetAtmosShadows(TimeValue t);
	void SetAtmosOpacity(TimeValue t, float f);
	float GetAtmosOpacity(TimeValue t, Interval& valid=FOREVER);
	void SetAtmosColAmt(TimeValue t, float f);
	float GetAtmosColAmt(TimeValue t, Interval& valid=FOREVER);
	void SetUseShadowColorMap(TimeValue t, int onOff);
	int GetUseShadowColorMap(TimeValue t);
	
	int GetOvershoot() { return overshoot; }
	void SetOvershoot(int a);
	int GetProjector() { return projector; }
	void SetProjector(int a) { projector = a; }
	void DoBitmapFit(HWND hwnd, TimeValue t);
	NameTab* GetExclList() {return &exclList; }
	BOOL Include() {return exclList.TestFlag(NT_INCLUDE); }
	Texmap* GetProjMap() { return projMap;  }
	void SetProjMap(Texmap* pmap);
	Texmap* GetShadowProjMap() { return shadProjMap;  }
	void SetShadowProjMap(Texmap* pmap);
	void UpdateTargDistance(TimeValue t, INode* inode);
	void AssignProjectorMap(Texmap *m, BOOL newmat);
	void AssignShadProjMap(Texmap *m, BOOL newmat);
	void BrowseProjectorMap(HWND hWnd);	
	void BrowseShadProjMap(HWND hWnd);

	NameTab& GetExclusionList() {return exclList;}
	void SetExclusionList(NameTab &list) {exclList = list;}

	BOOL SetHotSpotControl(Control *c);
	BOOL SetFalloffControl(Control *c);
	BOOL SetColorControl(Control *c);
	Control* GetHotSpotControl();
	Control* GetFalloffControl();
	Control* GetColorControl();
	void SetAffectDiffuse(BOOL onOff) { affectDiffuse = onOff; }
	BOOL GetAffectDiffuse() {return affectDiffuse; }
	void SetAffectSpecular(BOOL onOff) { affectSpecular = onOff; }
	BOOL GetAffectSpecular() {return affectSpecular; }
	void SetSoftenDiffuse(BOOL onOff) { softenDiffuse = onOff; }
	BOOL GetSoftenDiffuse() {return softenDiffuse; }
	void SetDecayType(BOOL onOff) { decayType = onOff; }
	BOOL GetDecayType() {return decayType; }
	void SetAmbientOnly(BOOL onOff) { ambientOnly = onOff; }
	BOOL GetAmbientOnly() {return ambientOnly; }

	void SetEmitterEnable(TimeValue t, BOOL onOff);
	BOOL GetEmitterEnable(TimeValue t, Interval& valid=FOREVER);
	void SetEmitterEnergy(TimeValue t, float energy);
	float GetEmitterEnergy(TimeValue t, Interval& valid=FOREVER);
	void SetEmitterDecayType(TimeValue t, int decay);
	int  GetEmitterDecayType(TimeValue t, Interval& valid=FOREVER);
	void SetEmitterCausticPhotons(TimeValue t, int photons);
	int  GetEmitterCausticPhotons(TimeValue t, Interval& valid=FOREVER);
	void SetEmitterGlobalIllumPhotons(TimeValue t, int photons);
	int  GetEmitterGlobalIllumPhotons(TimeValue t, Interval& valid=FOREVER);


	// internal routines
	void BoxCircle(TimeValue t, float r, float d, Box3& box, int extraPt=0, Matrix3 *tm=NULL);
	void BoxLight(TimeValue t, INode *inode, Box3& box, Matrix3 *tm=NULL);
	ISpinnerControl* AttenSpinner(int atype);
	void ChangeAtten(int atype);
	void UpdateColBrackets(TimeValue t);
	void SetSpinnerBracket(ISpinnerControl *spin, int pbid);
	void UpdateForAmbOnly();
	void PreSaveOld(); 
	void PostSaveOld(); 

	// Animatable methods
	void DeleteThis() { delete this; }
	Class_ID ClassID() { return  Class_ID(OMNI_LIGHT_CLASS_ID+type,0); }  
	void GetClassName(TSTR& s) { s = TSTR(GetString(IDS_DB_LIGHT_CLASS)); }

	void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags); 
	
	int NumSubs() { 
		if ( IsCompatibleRenderer ()) 
			return 5;
		else
			return 4;
		}  
	Animatable* SubAnim(int i) {
		switch(i) {
			case PBLOCK_REF: return (Animatable*)pblock;
			case PROJMAP_REF: return (Animatable*)projMap;
			case SHADPROJMAP_REF: return (Animatable*)shadProjMap;
			case SHADTYPE_REF: return (Animatable*)shadType;
			case EMITTER_REF: 
				if ( IsCompatibleRenderer ())
					return (Animatable*)emitter;
				else
					return NULL;
			default: return NULL;
			}
		 }
	TSTR SubAnimName(int i) { 
		switch(i) {
			case PBLOCK_REF: return TSTR(GetString(IDS_RB_PARAMETERS));
			case PROJMAP_REF: return TSTR(GetString(IDS_DS_PROJMAP));
			case SHADPROJMAP_REF: return TSTR(GetString(IDS_DS_SHADPROJMAP));
			case SHADTYPE_REF: return TSTR(GetString(IDS_DS_SHAD_GEN));
			case EMITTER_REF: 
				if ( IsCompatibleRenderer ())
					return TSTR(GetString(IDS_EMITTER));
				else
					return _T("");
			default: return _T("");
			}
		}

	// From ref
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	int NumRefs() { 
		if ( GetSavingVersion() == 2000 )
			return 2;
		else
			return 5;
		}
	RefTargetHandle GetReference(int i) {
		switch(i) {
			case PBLOCK_REF: return (RefTargetHandle)(temppb?temppb:pblock);
			case PROJMAP_REF: return (RefTargetHandle)projMap;
			case SHADPROJMAP_REF: return (RefTargetHandle)shadProjMap;
			case SHADTYPE_REF: return (RefTargetHandle)shadType;
			case EMITTER_REF: return (RefTargetHandle)emitter;
			}
		return NULL;
		}

	void SetReference(int i, RefTargetHandle rtarg) {
		switch(i) {
			case PBLOCK_REF:
				pblock=(IParamBlock*)rtarg; 
				break;
			case PROJMAP_REF:
				projMap = (Texmap *)rtarg;
				if (projMapName) 
					projMapName->SetText(projMap?projMap->GetFullName().data():GetString(IDS_DB_NONE));
				break;
			case SHADPROJMAP_REF:
				shadProjMap = (Texmap *)rtarg;
				if (shadProjMapName) 
					shadProjMapName->SetText(shadProjMap?shadProjMap->GetFullName().data():GetString(IDS_DB_NONE));
				break;
			case SHADTYPE_REF:
				shadType = (ShadowType *)rtarg;
				break;
			case EMITTER_REF:
				emitter = (IParamBlock*)rtarg; 
				break;

			}
		}

	// IO
	void FixOldVersions(ILoad *iload);
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	LRESULT CALLBACK TrackViewWinProc( HWND hwnd,  UINT message, 
            WPARAM wParam,   LPARAM lParam ){return(0);}
};

#endif

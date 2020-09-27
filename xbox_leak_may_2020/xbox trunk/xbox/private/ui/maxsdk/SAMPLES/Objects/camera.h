/**********************************************************************
 *<
	FILE: camera.h

	DESCRIPTION:  Defines a simple Camera

	CREATED BY: Dan Silva

	HISTORY: created 13 September 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __CAMERA__H__ 

#define __CAMERA__H__

// reference #s
#define PBLOCK_REF 0
#define DOF_REF 1

#define MIN_FOV 	0.000025f
#define MAX_FOV 	175.0f

#define MIN_LENS	9.857142f
#define MAX_LENS	100000.0f

#define MIN_TDIST	1.0f
#define MAX_TDIST	100000.0f

#define FIXED_CONE_DIST  160.0f

#define FOV_W 0	  // width-related FOV
#define FOV_H 1   // height-related FOV
#define FOV_D 2   // diagonal-related FOV

class CamMtl: public Material {
	public:
	CamMtl();
	};

extern CamMtl camMtl;

class SCamCreateCallBack;
class SetCamTypeRest;

class SimpleCamera: public GenCamera {			   
		friend class SCamCreateCallBack;
		friend class CameraPostLoad;
		friend class SetCamTypeRest;
		friend BOOL CALLBACK SimpleCamParamDialogProc( HWND hDlg, UINT message,
			 WPARAM wParam, LPARAM lParam );
		friend void resetCameraParams();

		// Class vars
		static Mesh mesh;
		static short meshBuilt;
		static short dlgIsOrtho;
		static BOOL inCreate;
		static float dlgFOV;
		static short dlgShowCone;
		static short dlgShowHorzLine;
		static float dlgTDist;
		static short dlgClip;
		static float dlgHither;
		static float dlgYon;
		static short dlgRangeDisplay;
		static float dlgNearRange;
		static float dlgFarRange;
		static short dlgDOFEnable;
		static float dlgDOFFStop;
		static HWND hSimpleCamParams;
		static HWND hDepthOfFieldParams;
		static IObjParam* iObjParams;		
		static ISpinnerControl *fovSpin;
		static ISpinnerControl *lensSpin;
		static ISpinnerControl *tdistSpin;
		static ISpinnerControl *hitherSpin;
		static ISpinnerControl *yonSpin;
		static ISpinnerControl *envNearSpin;
		static ISpinnerControl *envFarSpin;
		static ISpinnerControl *fStopSpin;
		static ICustButton *iFovType;
		
		IParamBlock *pblock;
		IParamBlock *depthOfFieldPB;
		short isOrtho;
		short enable;
		short hasTarget;
		short coneState;
		short horzLineState;
		short manualClip;
		short rangeDisplay;
		int extDispFlags;
		float targDist; // for target cameras
		int fovType;
		short dofEnable;
		float fStop;

	//  inherited virtual methods for Reference-management
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
		void BuildMesh();
		void UpdateUI(TimeValue t);
		void GetMat(TimeValue t, INode* inod, ViewExp *vpt, Matrix3& mat);
		void GetConePoints(TimeValue t, Point3* q, float dist);
		void DrawCone(TimeValue t, GraphicsWindow *gw, float dist, int colid=0, BOOL drawSides=FALSE, BOOL drawDiags=FALSE);
		int DrawConeAndLine(TimeValue t, INode* inode, GraphicsWindow *gw, int drawing);
		int DrawRange(TimeValue t, INode *inode, GraphicsWindow *gw);
		
		BOOL IsCompatibleRenderer ( void );
	public:
		SimpleCamera(int look = 0);
		~SimpleCamera();
		GenCamera *NewCamera(int type) { return new SimpleCamera(type); }
		void SetOrtho(BOOL b);
		BOOL IsOrtho()	{ return isOrtho; }
		void SetFOV(TimeValue t, float f);
		float GetFOV(TimeValue t, Interval& valid = Interval(0,0));
		void SetLens(TimeValue t, float f);
		float GetLens(TimeValue t, Interval& valid = Interval(0,0));
		void SetTDist(TimeValue t, float f);
		float GetTDist(TimeValue t, Interval& valid = Interval(0,0));
		void SetConeState(int s);
		int GetConeState() {return coneState;}
		void SetHorzLineState(int s);
		int GetHorzLineState() {return horzLineState;}
		int GetManualClip() { return manualClip; }
		void SetManualClip(int onOff);
		float GetClipDist(TimeValue t, int which, Interval &valid = Interval(0,0));
		void SetClipDist(TimeValue t, int which, float f);
		float MMtoFOV(float mm);  
		float FOVtoMM(float fov); 
		float CurFOVtoWFOV(float cfov);
		float WFOVtoCurFOV(float wfov);
		void  SetFOVType(int ft);
		int GetFOVType() {return fovType;}
		void  SetDOFEnable(TimeValue t, BOOL onOff);
		BOOL GetDOFEnable(TimeValue t, Interval& valid = Interval(0,0));
		void  SetDOFFStop(TimeValue t, float fS);
		float GetDOFFStop(TimeValue t, Interval& valid = Interval(0,0));

		BOOL SetFOVControl(Control *c);
		Control *GetFOVControl();

		void SetEnvRange(TimeValue time, int which, float f);
		float GetEnvRange(TimeValue t, int which, Interval& valid = Interval(0,0));
		void SetEnvDisplay(BOOL b, int notify=TRUE);
		BOOL GetEnvDisplay(void)	{ return rangeDisplay; }
		void RenderApertureChanged(TimeValue t);
		void UpdateTargDistance(TimeValue t, INode* inode);

		void UpdateKeyBrackets(TimeValue t);

		void Enable(int enab) { enable = enab; }

		//  inherited virtual methods:

		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		void SetExtendedDisplay(int flags);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CreateMouseCallBack* GetCreateMouseCallBack();
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		TCHAR *GetObjectName() { return hasTarget ? GetString(IDS_DB_TARGET_CAM) : GetString(IDS_DB_FREE_CAM); }
		int  Type() { return hasTarget ? TARGETED_CAMERA:FREE_CAMERA; }
		void SetType(int tp);
		void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp* vpt, Box3& box );
		void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp* vpt, Box3& box );
		void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );

		// From Object
		ObjectState Eval(TimeValue time);
		void InitNodeName(TSTR& s) { s = GetString(IDS_DB_CAMERA); }		
		Interval ObjectValidity();
		int DoOwnSelectHilite() { return 1; }
		Interval ObjectValidity(TimeValue time);
		
		// From Camera
		RefResult EvalCameraState(TimeValue t, Interval& valid, CameraState* cs);

		// Animatable methods
		void DeleteThis() { delete this; }
		Class_ID ClassID() { return Class_ID(hasTarget?LOOKAT_CAM_CLASS_ID:SIMPLE_CAM_CLASS_ID,0); }  
		void GetClassName(TSTR& s) { s = TSTR(GetString(IDS_DB_SIMPLECAM_CLASS)); }
		
		int NumSubs() { return IsCompatibleRenderer () ? 2 : 1; }
		Animatable* SubAnim(int i) {
			switch(i) {
				case PBLOCK_REF: return (Animatable*)pblock;
				case DOF_REF: return IsCompatibleRenderer () ? (Animatable*)depthOfFieldPB : NULL;
				default: return NULL;
			}
		}

		TSTR SubAnimName(int i) { 
			switch(i) {
				case PBLOCK_REF: return TSTR(GetString(IDS_RB_PARAMETERS));
				case DOF_REF: return IsCompatibleRenderer () ? TSTR(GetString(IDS_DOF)) : _T("");
				default: return _T("");
			}
		}

		// From ref
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		int NumRefs() {return 2;}
		RefTargetHandle GetReference(int i) {
			switch(i) {
				case PBLOCK_REF: return (RefTargetHandle)pblock;
				case DOF_REF: return (RefTargetHandle)depthOfFieldPB;
				default: return NULL;
			}
		}

		void SetReference(int i, RefTargetHandle rtarg) {
			switch(i) {
				case PBLOCK_REF:
					pblock=(IParamBlock*)rtarg; 
					break;
				case DOF_REF:
					depthOfFieldPB = (IParamBlock*)rtarg; 
					break;
				default:
					break;
			}
		}

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		LRESULT CALLBACK TrackViewWinProc( HWND hwnd,  UINT message, 
	            WPARAM wParam,   LPARAM lParam ){return(0);}
	};


#endif

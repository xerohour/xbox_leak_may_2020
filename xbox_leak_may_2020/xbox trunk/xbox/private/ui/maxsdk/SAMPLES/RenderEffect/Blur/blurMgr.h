/* -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

   FILE: blurMgr.h

	 DESCRIPTION: blur manager - class declarations

	 CREATED BY: michael malone (mjm)

	 HISTORY: created November 4, 1998

	 Copyright (c) 1998, All Rights Reserved

// -----------------------------------------------------------------------------
// -------------------------------------------------------------------------- */

#if !defined(_BLUR_H_INCLUDED_)
#define _BLUR_H_INCLUDED_

// maxsdk includes and predeclarations
#include <max.h>
#include <iparamm2.h>
#include <bmmlib.h>

// local includes and predeclarations
#include "globals.h"
#include "resource.h"
#include "dllMain.h"
#include "dlgProcs.h"
#include "_blurTypes\blurUniform.h"
#include "_blurTypes\blurDirectional.h"
#include "_blurTypes\blurRadial.h"
#include "_selectTypes\selImage.h"
#include "_selectTypes\selIgnBack.h"
#include "_selectTypes\selLum.h"
#include "_selectTypes\selMaps.h"

#define NUM_SUBS	numIDs
#define NUM_REFS	NUM_SUBS
#define NUM_PBLOCKS	NUM_SUBS

// ----------------------------------------
// blur effect - class declaration
// ----------------------------------------
class BlurMgr : public Effect
{
protected:
	DWORD m_lastBMModifyID;
	CheckAbortCallback *m_checkAbort;
	int m_imageSz, m_imageW, m_imageH;
	CompMap m_compMap;
	bool m_compValid;

	// blur type instances
	BlurBase *m_blurs[numBlurs];

	// selection type instances
	SelBase *m_sels[numSels];

	// local copies of paramBlock elements
	BOOL m_activeSels[numSels];

	void updateSelections(TimeValue t, Bitmap *bm, RenderGlobalContext *gc);
	void blur(TimeValue t, Bitmap *bm, RenderGlobalContext *gc, BOOL composite);

public:
	// paramblocks & parammaps
	IParamBlock2 *pbMaster, *pbBlurData, *pbSelData;
	static IParamMap2 *pmMaster;
	static IParamMap2 *pmBlurData;
	static IParamMap2 *pmSelData;

	// class descriptor
	static const Class_ID blurMgrClassID;

	// dialog procs
	static MasterDlgProc masterDlgProc;
	static BlurDataDlgProc blurDataDlgProc;
	static SelDataDlgProc selDataDlgProc;

	// paramblock descriptors
	static ParamBlockDesc2 pbdMaster;
	static ParamBlockDesc2 pbdBlurData;
	static ParamBlockDesc2 pbdSelData;

	BlurMgr();
	~BlurMgr();
	BOOL progress(const TCHAR *title,int done, int total) { m_checkAbort->SetTitle(title); return m_checkAbort->Progress(done,total); }
	BOOL getBlurValue(ParamID id, TimeValue t, float& v, Interval &ivalid, int tabIndex=0) { return pbBlurData->GetValue(id, t, v, ivalid, tabIndex); }
	BOOL getBlurValue(ParamID id, TimeValue t, int& v, Interval &ivalid, int tabIndex=0) { return pbBlurData->GetValue(id, t, v, ivalid, tabIndex); }
	BOOL getBlurValue(ParamID id, TimeValue t, INode*& v, Interval &ivalid, int tabIndex=0) { return pbBlurData->GetValue(id, t, v, ivalid, tabIndex); }
	BOOL setBlurValue(ParamID id, TimeValue t, INode*& v, int tabIndex=0) { return pbBlurData->SetValue(id, t, v, tabIndex); }
	BOOL getSelValue(ParamID id, TimeValue t, float& v, Interval &ivalid, int tabIndex=0) { return pbSelData->GetValue(id, t, v, ivalid, tabIndex); }
	BOOL getSelValue(ParamID id, TimeValue t, int& v, Interval &ivalid, int tabIndex=0) { return pbSelData->GetValue(id, t, v, ivalid, tabIndex); }
	BOOL getSelValue(ParamID id, TimeValue t, Texmap*& v, Interval &ivalid, int tabIndex=0) { return pbSelData->GetValue(id, t, v, ivalid, tabIndex); }
	void blurEnable(ParamID id, BOOL onOff, int tabIndex=0) { pmBlurData->Enable(id, onOff, tabIndex); }
	void selEnable(ParamID id, BOOL onOff, int tabIndex=0) { pmSelData->Enable(id, onOff, tabIndex); }
	HWND getBlurHWnd() { return pmBlurData->GetHWnd(); }
	HWND getSelHWnd() { return pmBlurData->GetHWnd(); }

	// Animatable/Reference
	int NumSubs() { return NUM_SUBS; }
	Animatable* SubAnim(int i) { return GetReference(i); }
	TSTR SubAnimName(int i);
	int NumRefs() { return NUM_REFS; }
	RefTargetHandle GetReference(int i);
	void SetReference(int i, RefTargetHandle rtarg);
	Class_ID ClassID() { return blurMgrClassID; }
	void GetClassName(TSTR& s) { s = GetString(IDS_CLASS_NAME); }
	void DeleteThis() { delete this; }
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);
	int NumParamBlocks() { return NUM_PBLOCKS; }
	IParamBlock2* GetParamBlock(int i) { return (IParamBlock2 *)GetReference(i); }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (IParamBlock2 *)GetReference(id); }
	IOResult Load(ILoad *iload);

	// Effect
	TSTR GetName() { return GetString(IDS_NAME); }
	EffectParamDlg *CreateParamDialog(IRendParams *ip);
	DWORD GBufferChannelsRequired(TimeValue t) { return BMM_CHAN_NONE; }
	void Apply(TimeValue t, Bitmap *bm, RenderGlobalContext *gc, CheckAbortCallback *_checkAbort);
};

// --------------------------------------------------
// blur class descriptor - class declaration
// --------------------------------------------------
class BlurMgrClassDesc : public ClassDesc2
{
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new BlurMgr; }
	const TCHAR *	ClassName() { return GetString(IDS_CDESC_CLASS_NAME); }
	SClass_ID		SuperClassID() { return RENDER_EFFECT_CLASS_ID; }
	Class_ID		ClassID() { return BlurMgr::blurMgrClassID; }
	const TCHAR*	Category() { return _T(""); }
	const TCHAR*	InternalName() { return _T("Blur"); } // hard-coded for scripter
	HINSTANCE		HInstance() { return hInstance; }
	};

extern BlurMgrClassDesc blurMgrCD;

#endif // !defined(_BLUR_H_INCLUDED_)

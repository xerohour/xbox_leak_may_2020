/*===========================================================================*\
 | 
 |  FILE:	RenderEffect.cpp
 |			Skeleton project and code for a RenderEffect
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "RenderEffect.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonRFXClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()				{ return TRUE; }
	void *			Create( BOOL loading )	{ return new SkeletonEffect; }
	const TCHAR *	ClassName()				{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()			{ return RENDER_EFFECT_CLASS_ID; }
	Class_ID 		ClassID()				{ return RFX_CLASSID; }
	const TCHAR* 	Category()				{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()			{ return _T("SkeletonEffect"); }
	HINSTANCE		HInstance()				{ return hInstance; }
};

static SkeletonRFXClassDesc SkeletonRFXCD;
ClassDesc* GetSkeletonRFXDesc() {return &SkeletonRFXCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

BOOL SkeletonEffectDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (msg) 
	{
		case WM_INITDIALOG:
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			break;
	}
	return FALSE;
}

static SkeletonEffectDlgProc SkelEfDlgProc;



/*===========================================================================*\
 |	Paramblock2 Descriptor
\*===========================================================================*/

static ParamBlockDesc2 skeleton_param_blk ( skeleton_params, _T("SkeletonEffect parameters"),  0, &SkeletonRFXCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_SKELETON_EFFECT, IDS_PARAMETERS, 0, 0, &SkelEfDlgProc, 
	// params
	skeleton_simple_param,	_T("simpleParam"),	TYPE_BOOL,	P_ANIMATABLE,	IDS_SIMPLE,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_SIMPLE, 
		end,
	end
	);



/*===========================================================================*\
 |	Constructor
 |  Ask the ClassDesc2 to make the AUTO_CONSTRUCT paramblocks and wire them in
\*===========================================================================*/

SkeletonEffect::SkeletonEffect()
	{
	SkeletonRFXCD.MakeAutoParamBlocks(this);
	assert(pblock);
	}


/*===========================================================================*\
 |	Standard Load
\*===========================================================================*/

IOResult SkeletonEffect::Load(ILoad *iload)
	{
	Effect::Load(iload);
	return IO_OK;
	}


/*===========================================================================*\
 |	Ask the ClassDesc2 to make the AUTO_UI SkeletonEffectDlgProc
\*===========================================================================*/

EffectParamDlg *SkeletonEffect::CreateParamDialog(IRendParams *ip)
	{	
	return SkeletonRFXCD.CreateParamDialogs(ip, this);
	}




/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletonEffect::SubAnim(int i) 	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
TSTR SkeletonEffect::SubAnimName(int i) {
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
	}

RefTargetHandle SkeletonEffect::GetReference(int i)
	{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
	}
void SkeletonEffect::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case 0: pblock = (IParamBlock2*)rtarg; break;
		}
	}
RefResult SkeletonEffect::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_CHANGE:
			skeleton_param_blk.InvalidateUI();
			break;
		}
	return REF_SUCCEED;
	}


/*===========================================================================*\
 |	This method is called once per frame when the renderer begins.  
\*===========================================================================*/

void SkeletonEffect::Update(TimeValue t, Interval& valid)
	{
	}


/*===========================================================================*\
 |	Called to initialize or clear up before and after rendering  
 |	These will get called once each, not per frame
\*===========================================================================*/

int SkeletonEffect::RenderBegin(TimeValue t, ULONG flags)
	{
	return 0;
	}

int SkeletonEffect::RenderEnd(TimeValue t)
	{
	return 0;
	}


/*===========================================================================*\
 |	Apply the actual changes to the rendered bitmap at time 't'
 |	In this skeleton project, we simply darken every pixel on every other line
\*===========================================================================*/

void SkeletonEffect::Apply(TimeValue t, Bitmap *bm, RenderGlobalContext *gc,CheckAbortCallback *checkAbort) 
{
	int w = bm->Width();
	int h = bm->Height();
	Interval valid;	
	
	BOOL simple;

	// Get the simple value out of our paramblock2
	pblock->GetValue(skeleton_simple_param, t, simple, valid);

	PixelBuf l64(w);
	BMM_Color_64 *p=l64.Ptr();

	BOOL flipflop = FALSE;

	for (int y = 0; y<h; y++) {
	
		bm->GetPixels(0, y, w, p);

		for (int x=0; x<w; x++) {
			if(simple&&flipflop)
				{
				p[x].r /= 2.0;
				p[x].g /= 2.0;
				p[x].b /= 2.0;
				}
			}

		bm->PutPixels(0, y, w, p);
		flipflop=!flipflop;
		
		if (((y&3)==0)&&checkAbort&&checkAbort->Progress(y,h)) return;
	}
}
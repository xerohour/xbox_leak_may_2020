/*===========================================================================*\
 | 
 |  FILE:	Dialog.cpp
 |			Skeleton project and code for a Material Shader
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-1-99
 | 
\*===========================================================================*/


#include "Shader.h"


#define SHADER_NCOLBOX 1


/*===========================================================================*\
 |	Complete dialog handler implimentation
\*===========================================================================*/

class SkeletonShaderDlg : public ShaderParamDlg {
public:
	
	SkeletonShader*	pShader;
	StdMat2*		pMtl;
	HPALETTE		hOldPal;
	HWND			hwmEdit;
	IMtlParams*		pMtlPar;
	HWND			hwHilite;
	HWND			hRollup;
	TimeValue		curTime;
	BOOL			valid;
	BOOL			isActive;


	IColorSwatch *cs[SHADER_NCOLBOX];
	ISpinnerControl *brSpin, *trSpin;
	ICustButton* texMBut[NMBUTS];
	TexDADMgr dadMgr;
	

	SkeletonShaderDlg( HWND hwMtlEdit, IMtlParams *pParams ); 
	~SkeletonShaderDlg(); 


	// DnD: Required for correctly operating map buttons
	int FindSubTexFromHWND(HWND hw) {
		for (long i=0; i<NMBUTS; i++) {
			if (hw == texMBut[i]->GetHwnd()) 
				return texmapFromMBut[i];
		}	
		return -1;
	}


	BOOL PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ); 
	Class_ID ClassID(){ return SKELSHADER_CLASSID; }
	void DeleteThis() { delete this; }


	// Set shader or material host from incoming values
	void SetThing(ReferenceTarget *m){ pMtl = (StdMat2*)m; }
	void SetThings( StdMat2* theMtl, Shader* theShader )
	{	pShader = (SkeletonShader*)theShader; 
		if (pShader)pShader->SetParamDlg(this); 
		pMtl = theMtl; 
	}

	// Get the shader and material host
	ReferenceTarget* GetThing(){ return (ReferenceTarget*)pMtl; }
	Shader* GetShader(){ return pShader; }
	
	void SetTime(TimeValue t) 
	{ 
		if (!pShader->ivalid.InInterval(t)) {
			Interval v;
			pShader->Update(t,v);
			LoadDialog(TRUE); 
		}
		curTime = t; 
	}		

	// Is there a keyframe at this time?
	BOOL KeyAtCurTime(int id) { return pShader->KeyAtTime(id,curTime); } 

	void ActivateDlg( BOOL dlgOn ){ isActive = dlgOn; }
	HWND GetHWnd(){ return hRollup; }

	// Get notified of changes, and update dialog controls
	void NotifyChanged(){ pShader->NotifyChanged(); }
	void LoadDialog(BOOL draw);
	void ReloadDialog(){ Interval v; pShader->Update(pMtlPar->GetTime(), v); LoadDialog(FALSE);}

	// Update the various components of the dialog
	void UpdateDialog( ParamID paramId ){ ReloadDialog(); }
	void UpdateMtlDisplay(){ pMtlPar->MtlChanged(); }
    void UpdateHilite( );
	void UpdateColSwatches();
	void UpdateMapButtons();
	void UpdateOpacity();

	// Start editing a color swatch
	void SelectEditColor(int i) { cs[ i ]->EditThis(FALSE); }
};

static BOOL CALLBACK  SkeletonShaderDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	SkeletonShaderDlg *theDlg;
	if (msg == WM_INITDIALOG) {
		theDlg = (SkeletonShaderDlg*)lParam;
		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
	} else {
	    if ( (theDlg = (SkeletonShaderDlg *)GetWindowLong(hwndDlg, GWL_USERDATA) ) == NULL )
			return FALSE; 
	}
	theDlg->isActive = 1;
	BOOL res = theDlg->PanelProc(hwndDlg, msg, wParam, lParam);
	theDlg->isActive = 0;
	return res;
}



/*===========================================================================*\
 |	Create the dialog panels themselves
\*===========================================================================*/

ShaderParamDlg* SkeletonShader::CreateParamDialog(HWND hOldRollup, HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen ) 
{
	Interval v;
	Update(imp->GetTime(),v);
	
	SkeletonShaderDlg *pDlg = new SkeletonShaderDlg(hwMtlEdit, imp);
	pDlg->SetThings( theMtl, this  );

	// If it already exists, replace it
	if ( hOldRollup ) {
		pDlg->hRollup = imp->ReplaceRollupPage( 
			hOldRollup,
			hInstance,
			MAKEINTRESOURCE(IDD_SHADER_SKEL1),
			SkeletonShaderDlgProc, 
			GetString(IDS_SKELETON_DESC),
			(LPARAM)pDlg , 
			rollupOpen
			);
	} else
		// otherwise, add one
		pDlg->hRollup = imp->AddRollupPage( 
			hInstance,
			MAKEINTRESOURCE(IDD_SHADER_SKEL1),
			SkeletonShaderDlgProc, 
			GetString(IDS_SKELETON_DESC),	
			(LPARAM)pDlg , 
			rollupOpen
			);

	return (ShaderParamDlg*)pDlg;	
}



/*===========================================================================*\
 |	Update the UI
\*===========================================================================*/

RefResult SkeletonShader::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
									  PartID& partID, RefMessage message ) 
{
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget == pblock){
				// update UI if paramblock changed, possibly from scripter
				ParamID changingParam = pblock->LastNotifyParamID();
				// reload the dialog if present
				if (paramDlg){
					paramDlg->UpdateDialog(changingParam);
				}
				// notify our dependents that we've changed
				NotifyChanged();
			}
			break;
	}
	return(REF_SUCCEED);
}




/*===========================================================================*\
 |	Methods to draw the graphical represenation of the shader's hilite
 |	Mainly this just calls EvalHiliteCurve in Eval.cpp and draws a line as 
 |		high as the value it gets returned (clamped)
\*===========================================================================*/

static void VertLine(HDC hdc,int x, int ystart, int yend) 
{
	MoveToEx(hdc, x, ystart, NULL); 
	if (ystart <= yend)
		LineTo(hdc, x, yend+1);
	else 
		LineTo(hdc, x, yend-1);
}

void DrawHilite(HDC hdc, Rect& rect, Shader* pShader )
{
int w,h,npts,xcen,ybot,ytop,ylast,i,iy;

	HPEN linePen = (HPEN)GetStockObject(WHITE_PEN);
	HPEN fgPen = CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNFACE));
	HPEN bgPen = CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW));

	w = rect.w();
	h = rect.h()-3;
	npts = (w-2)/2;
	xcen = rect.left+npts;
	ybot = rect.top+h;
	ytop = rect.top+2;
	ylast = -1;
	for (i=0; i<npts; i++) {
		float v = pShader->EvalHiliteCurve( (float)i/((float)npts*2.0f) );
		if (v>2.0f) v = 2.0f; // keep iy from wrapping
		iy = ybot-(int)(v*((float)h-2.0f));

		if (iy<ytop) iy = ytop;

		SelectPen(hdc, fgPen);
		VertLine(hdc,xcen+i,ybot,iy);
		VertLine(hdc,xcen-i,ybot,iy);

		if (iy-1>ytop) {
			// Fill in above curve
			SelectPen(hdc,bgPen);
			VertLine(hdc,xcen+i, ytop, iy-1);
			VertLine(hdc,xcen-i, ytop, iy-1);
			}
		if (ylast>=0) {
			SelectPen(hdc,linePen);
			VertLine(hdc,xcen+i-1,iy-1,ylast);
			VertLine(hdc,xcen-i+1,iy-1,ylast);
			}

		ylast = iy;
	}

	SelectObject( hdc, linePen );
	DeleteObject(fgPen);
	DeleteObject(bgPen);
	WhiteRect3D(hdc, rect, 1);
}

LRESULT CALLBACK HiliteWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
	int id = GetWindowLong(hwnd,GWL_ID);
	HWND hwParent = GetParent(hwnd);
	ShaderParamDlg *theDlg = (ShaderParamDlg *)GetWindowLong(hwParent, GWL_USERDATA);
	if (theDlg==NULL) return FALSE;

    switch (msg) {
		case WM_COMMAND: 	
		case WM_MOUSEMOVE: 	
		case WM_LBUTTONUP: 
		case WM_CREATE:
		case WM_DESTROY: 
		break;

		case WM_PAINT: 	
		{
			PAINTSTRUCT ps;
			Rect rect;
			HDC hdc = BeginPaint( hwnd, &ps );
			if (!IsRectEmpty(&ps.rcPaint)) {
				GetClientRect( hwnd, &rect );
				Shader* pShader = (Shader*)(theDlg->GetShader());
				DrawHilite(hdc, rect, pShader );
			}
			EndPaint( hwnd, &ps );
		}													
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
} 



/*===========================================================================*\
 |	Constructor/Destructor
 |	Initialize and destroy all custom controls
\*===========================================================================*/

SkeletonShaderDlg::SkeletonShaderDlg( HWND hwMtlEdit, IMtlParams *pParams)
{
	pMtl = NULL;
	pShader = NULL;
	hwmEdit = hwMtlEdit;
	pMtlPar = pParams;
	dadMgr.Init(this);
	brSpin = trSpin = NULL;
	hRollup = hwHilite = NULL;
	curTime = 0;
	isActive = valid = FALSE;

	for( long i = 0; i < SHADER_NCOLBOX; ++i )
		cs[ i ] = NULL;

	for( i = 0; i < NMBUTS; ++i )
		texMBut[ i ] = NULL;
}

SkeletonShaderDlg::~SkeletonShaderDlg()
{
	HDC hdc = GetDC(hRollup);
	GetGPort()->RestorePalette(hdc, hOldPal);
	ReleaseDC(hRollup, hdc);

	if( pShader ) pShader->SetParamDlg(NULL);

	for (long i=0; i < NMBUTS; i++ ){
		ReleaseICustButton( texMBut[i] );
		texMBut[i] = NULL; 
	}
	
	ReleaseISpinner(brSpin);
	ReleaseISpinner(trSpin);

	SetWindowLong(hRollup, GWL_USERDATA, NULL);
	SetWindowLong(hwHilite, GWL_USERDATA, NULL);
	hwHilite = hRollup = NULL;
}


/*===========================================================================*\
 |	Called to set the custom controls with their current settings
\*===========================================================================*/

void SkeletonShaderDlg::LoadDialog(BOOL draw) 
{
	if (pShader && hRollup) {
		brSpin->SetValue( pShader->GetBrightness() ,FALSE);
		brSpin->SetKeyBrackets(KeyAtCurTime(sk_brightness));
		
		trSpin->SetValue(FracToPc(pMtl->GetOpacity(curTime)),FALSE);
		trSpin->SetKeyBrackets(pMtl->KeyAtTime(OPACITY_PARAM, curTime));

		UpdateColSwatches();
		UpdateHilite();
	}
}



/*===========================================================================*\
 |	Lots of functions that will update the various componenets of the UI
 |	such as the hilite curve, specific values, etc
\*===========================================================================*/

static TCHAR* mapStates[] = { _T(" "), _T("m"),  _T("M") };

void SkeletonShaderDlg::UpdateMapButtons() 
{

	for ( long i = 0; i < NMBUTS; ++i ) {
		int nMap = texmapFromMBut[ i ];
		int state = pMtl->GetMapState( nMap );
		texMBut[i]->SetText( mapStates[ state ] );

		TSTR nm = pMtl->GetMapName( nMap );
		texMBut[i]->SetTooltip(TRUE,nm);
	}
}


void SkeletonShaderDlg::UpdateOpacity() 
{
	trSpin->SetValue(FracToPc(pMtl->GetOpacity(curTime)),FALSE);
	trSpin->SetKeyBrackets(pMtl->KeyAtTime(OPACITY_PARAM, curTime));
}

void SkeletonShaderDlg::UpdateColSwatches() 
{
	cs[0]->SetKeyBrackets( pShader->KeyAtTime(sk_diffuse,curTime) );
	cs[0]->SetColor( pShader->GetDiffuseClr() );
}


void SkeletonShaderDlg::UpdateHilite()
{
	HDC hdc = GetDC(hwHilite);
	Rect r;
	GetClientRect(hwHilite,&r);
	DrawHilite(hdc, r, pShader );
	ReleaseDC(hwHilite,hdc);
}




/*===========================================================================*\
 |	Translate a color IDC (control ID) to a color index
\*===========================================================================*/

static int ColorIDCToIndex(int idc) {
	switch (idc) {
		case IDC_COLOR: return 0;
		default: return 0;
	}
}





/*===========================================================================*\
 |	The dialog message handler
\*===========================================================================*/

BOOL SkeletonShaderDlg::PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
    switch (msg) {
		case WM_INITDIALOG:
			{
			HDC theHDC = GetDC(hwndDlg);
			hOldPal = GetGPort()->PlugPalette(theHDC);
			ReleaseDC(hwndDlg,theHDC);

			HWND hwndCS = GetDlgItem(hwndDlg, IDC_COLOR);
			cs[0] = GetIColorSwatch( hwndCS, pShader->GetDiffuseClr(), GetString(IDS_COLOR) );

			hwHilite = GetDlgItem(hwndDlg, IDC_HIGHLIGHT);
			SetWindowLong( hwHilite, GWL_WNDPROC, (LONG)HiliteWndProc);

			brSpin = SetupFloatSpinner(hwndDlg, IDC_BR_SPIN, IDC_BR_EDIT, 0.0f,1.0f, 0);
			brSpin->SetScale(0.01f);

			trSpin = SetupIntSpinner(hwndDlg, IDC_TR_SPIN, IDC_TR_EDIT, 0,100, 0);

			for (int j=0; j<NMBUTS; j++) {
				texMBut[j] = GetICustButton(GetDlgItem(hwndDlg,texMButtonsIDC[j]));
				assert( texMBut[j] );
				texMBut[j]->SetRightClickNotify(TRUE);
				texMBut[j]->SetDADMgr(&dadMgr);
			}
			LoadDialog(TRUE);
		}
		break;

		case WM_COMMAND: 
			{
			for ( int i=0; i<NMBUTS; i++) {
				if (id == texMButtonsIDC[i]) {
					PostMessage(hwmEdit,WM_TEXMAP_BUTTON, texmapFromMBut[i],(LPARAM)pMtl );
					UpdateMapButtons();
					goto exit;
					}
				}
			}

			break;

		case CC_COLOR_SEL: {
			int id = LOWORD(wParam);
			SelectEditColor(ColorIDCToIndex(id));
		}			
		break;
		case CC_COLOR_DROP:	{
			int id = LOWORD(wParam);
			SelectEditColor(ColorIDCToIndex(id));
		}
		break;
		case CC_COLOR_BUTTONDOWN:
			theHold.Begin();
		 break;
		case CC_COLOR_BUTTONUP:
			if (HIWORD(wParam)) theHold.Accept(GetString(IDS_PARAMCHG));
			else theHold.Cancel();
			break;
		case CC_COLOR_CHANGE: {			
			int id = LOWORD(wParam);
			int buttonUp = HIWORD(wParam); 
			int n = ColorIDCToIndex(id);
			if (buttonUp) theHold.Begin();
			DWORD curRGB = cs[n]->GetColor();
			pShader->SetDiffuseClr(curRGB, curTime); 
			if (buttonUp) {
				theHold.Accept(GetString(IDS_PARAMCHG));
				UpdateMtlDisplay();				
				}
		} break;
		case WM_PAINT: 
			if (!valid) {
				valid = TRUE;
				ReloadDialog();
				}
			return FALSE;
		case WM_CLOSE:
		case WM_DESTROY: 
			break;
		case CC_SPINNER_CHANGE: 
			if (!theHold.Holding()) theHold.Begin();
			switch (id) {
				case IDC_BR_SPIN: 
					pShader->SetBrightness( brSpin->GetFVal() , curTime); 
					UpdateHilite();
					break;
				case IDC_TR_SPIN: 
					pMtl->SetOpacity(PcToFrac( trSpin->GetIVal()),curTime); 
					break;
			}
			UpdateMtlDisplay();
		break;

		case CC_SPINNER_BUTTONDOWN:
			theHold.Begin();
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP: 
			if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
				theHold.Accept(GetString(IDS_PARAMCHG));
			else 
				theHold.Cancel();
			UpdateMtlDisplay();
			break;

    }
	exit:
	return FALSE;
	}



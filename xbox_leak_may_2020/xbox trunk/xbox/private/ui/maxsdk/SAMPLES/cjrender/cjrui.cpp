//***************************************************************************
// CJRender - [cjrui.cpp] Sample Plugin Renderer for 3D Studio MAX.
// 
// By Christer Janson - Kinetix
//
// Description:
// Implementation of the user interface dialogs
//
//***************************************************************************

#include "maxincl.h"
#include "resource.h"
#include "cjrender.h"

extern HINSTANCE hInstance;
static BOOL CALLBACK CJRendParamsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

//***************************************************************************
// Class definition of the render parameters dialog class
//***************************************************************************

class CJRendParamDlg : public RendParamDlg {
	public:
		CJRenderer *rend;
		IRendParams *ir;
		HWND hPanel;
		BOOL prog;
		HFONT hFont;
		ISpinnerControl* depthSpinner;

		CJRendParamDlg(CJRenderer *r,IRendParams *i,BOOL prog);
		~CJRendParamDlg();
		void AcceptParams();
		void RejectParams();
		void DeleteThis() {delete this;}
		void InitParamDialog(HWND hWnd);
		void InitProgDialog(HWND hWnd);
};


//***************************************************************************
// Constructor
//***************************************************************************

CJRendParamDlg::CJRendParamDlg(
		CJRenderer *r,IRendParams *i,BOOL prog)
{
	hFont      = hFont = CreateFont(14,0,0,0,FW_BOLD,0,0,0,0,0,0,0, VARIABLE_PITCH | FF_SWISS, _T(""));
	rend       = r;
	ir         = i;
	this->prog = prog;

	// Create the rollup pages.
	// If we are doing the progress dialog..
	if (prog) {		
		hPanel = ir->AddRollupPage(
			hInstance, 
			MAKEINTRESOURCE(IDD_CCJRENDERDLG_PROG),
			CJRendParamsDlgProc,
			"Sample B-Buffer Renderer",
			(LPARAM)this);
	} else {
		// ...or the user config dialog
		hPanel = ir->AddRollupPage(
			hInstance, 
			MAKEINTRESOURCE(IDD_CCJRENDERDLG),
			CJRendParamsDlgProc,
			"Sample B-Buffer Renderer",
			(LPARAM)this);
	}
}


//***************************************************************************
// Destructor
//***************************************************************************

CJRendParamDlg::~CJRendParamDlg()
{
	// Delete the font
	DeleteObject(hFont);
	// And the rollup page
	ir->DeleteRollupPage(hPanel);
}


//***************************************************************************
// Initialize the progress dialog
//***************************************************************************
void CJRendParamDlg::InitProgDialog(HWND hWnd)
{
	// Set the bold font to the options in the progress dialog.
	SendDlgItemMessage(hWnd,IDC_PROG_DEPTH,WM_SETFONT,(WPARAM)hFont,TRUE);
	SendDlgItemMessage(hWnd,IDC_PROG_ANTIALIAS,WM_SETFONT,(WPARAM)hFont,TRUE);
	SendDlgItemMessage(hWnd,IDC_PROG_REFLENV,WM_SETFONT,(WPARAM)hFont,TRUE);

	TSTR tbuf;
	tbuf.printf("%d", rend->rendParams.nMaxDepth);
	SetWindowText(GetDlgItem(hWnd,IDC_PROG_DEPTH), tbuf);

	tbuf.printf("%d", rend->rendParams.nAntiAliasLevel);
	SetWindowText(GetDlgItem(hWnd,IDC_PROG_ANTIALIAS), tbuf);

	tbuf.printf("%s", rend->rendParams.bReflectEnv ? GetString(IDS_YES) : GetString(IDS_NO));
	SetWindowText(GetDlgItem(hWnd,IDC_PROG_REFLENV), tbuf);
}


//***************************************************************************
// Initialize the render dialog
//***************************************************************************

void CJRendParamDlg::InitParamDialog(HWND hWnd)
{
	CheckRadioButton(hWnd, IDC_AA_NONE, IDC_AA_HIGH,
		rend->rendParams.nAntiAliasLevel == AA_NONE ? IDC_AA_NONE :
		rend->rendParams.nAntiAliasLevel == AA_MEDIUM ? IDC_AA_MEDIUM :
		IDC_AA_HIGH);

	CheckDlgButton(hWnd, IDC_REFLENV, rend->rendParams.bReflectEnv);

	// Setup the spinner controls for raytrace depth
	depthSpinner = GetISpinner(GetDlgItem(hWnd, IDC_DEPTH_SPIN)); 
	depthSpinner->LinkToEdit(GetDlgItem(hWnd,IDC_DEPTH), EDITTYPE_INT ); 
	depthSpinner->SetLimits(0, 25, TRUE); 
	depthSpinner->SetValue(rend->rendParams.nMaxDepth ,FALSE);
}

//***************************************************************************
// Accept parameters.
// This is called if the user clicks "Ok" or "Close"
//***************************************************************************

void CJRendParamDlg::AcceptParams()
{
	rend->rendParams.nMaxDepth = depthSpinner->GetIVal();

	rend->rendParams.nAntiAliasLevel = IsDlgButtonChecked(hPanel, IDC_AA_NONE) ? AA_NONE :
		IsDlgButtonChecked(hPanel, IDC_AA_MEDIUM) ? AA_MEDIUM : 
		IsDlgButtonChecked(hPanel, IDC_AA_HIGH) ? AA_MEDIUM : 0;
	rend->rendParams.bReflectEnv = IsDlgButtonChecked(hPanel, IDC_REFLENV);
}


//***************************************************************************
// Called if the user cancels the render param dialog.
// Reset any options you have changed here.
// Since we don't update the parameters until AcceptParams() is called,
// we don't need to do anything here.
//***************************************************************************

void CJRendParamDlg::RejectParams()
{
}


//***************************************************************************
// Dialog procedure for the rollup pages.
//***************************************************************************

static BOOL CALLBACK CJRendParamsDlgProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	CJRendParamDlg *dlg = (CJRendParamDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	switch (msg) {
		case WM_INITDIALOG:
			dlg = (CJRendParamDlg*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			if (dlg) {
				if (dlg->prog)
					dlg->InitProgDialog(hWnd);
				else
					dlg->InitParamDialog(hWnd);
			}
			break;
		case WM_DESTROY:
			if (!dlg->prog) {
				ReleaseISpinner(dlg->depthSpinner);
			}
			break;		
		case WM_COMMAND:
			// We don't care about the UI controls.
			// We take the value in AcceptParams() instead.
			break;

		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
			dlg->ir->RollupMouseMessage(hWnd,msg,wParam,lParam);
			break;
		default:
			return FALSE;
		}	
	return TRUE;
}


//***************************************************************************
// Create the user interface object
//***************************************************************************

RendParamDlg *CJRenderer::CreateParamDialog(IRendParams *ir,BOOL prog)
{
	return new CJRendParamDlg(this, ir, prog);
}

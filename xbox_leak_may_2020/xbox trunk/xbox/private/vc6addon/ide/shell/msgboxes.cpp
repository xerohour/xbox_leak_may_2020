/* MsgBoxes.cpp
 *
 * Purpose:	To provide a uniform set of message box services for all
 *			packages in Dolphin that conforms with WADG.
 *
 * Created:	22 July 1993 -by- Andrew Milton [w-amilt]
 *
 * Notes:	Message box calls come in several flavours and default values
 *			see the header file <MsgBoxes.h> for more information.
 *
 * Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
 ****************************************************************************/

#include "stdafx.h"
#include <tchar.h>
#include "msgboxes.h"
#include "resource.h"

#include "utilauto.h" // g_pAutomationState

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/* Local Data and Macros ****************************************************/

#define ALL_ICONS	(MB_ICONEXCLAMATION |\
					 MB_ICONINFORMATION |\
					 MB_ICONQUESTION 	|\
					 MB_ICONSTOP)

#define ALL_MODES	(MB_APPLMODAL 	|\
					 MB_SYSTEMMODAL	|\
					 MB_TASKMODAL)


/* WARNING!!!	WARNING!!!	WARNING!!!
 *
 * The following three arrays define the style of each type of message box.
 * If you add a message box type, then you MUST alter all three of these
 * arrays to reflect the change.
 *
 * The default message box style is given by
 *
 * nStyle = g_rnButtonStyles[BoxType] | g_rnIconStyles[BoxType] | MB_APPLMODAL
 *
 * (except for InternalErrors, which are MB_SYSTEMMODAL).
 *
 * The message box will beep when raised if g_rfBeepOnMessage[BoxType] is
 * TRUE.
 */

#define ArrayLength(x) (sizeof(x)/sizeof((x)[0]))

static UINT g_rnButtonStyles[] = {
	MB_OK,
	MB_YESNOCANCEL,
	MB_OK,
	MB_OK,
	MB_OK
};

static UINT g_rnIconStyles[] = {
	MB_ICONINFORMATION | MB_SETFOREGROUND,
	MB_ICONQUESTION | MB_SETFOREGROUND,
	MB_ICONEXCLAMATION | MB_SETFOREGROUND,
	MB_ICONSTOP | MB_SETFOREGROUND,
	MB_ICONSTOP | MB_SETFOREGROUND
};

static BOOL g_rfBeepOnMessage[] = {
	FALSE,
	FALSE,
	FALSE,
	TRUE,
	TRUE
};

static CUIntArray g_HelpStack;
static UINT g_nCrntHelp;

/**************************** Public Function *******************************\
 InitMsgBoxes()

 Purpose:	To perform a sanity check on the message box style arrays and to
 			initialize the default F1-help behavior

 Returns:	Nada.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

extern void
InitMsgBoxes()
{
// Make sure that our message box data arrays are the correct length.

	ASSERT(ArrayLength(g_rnButtonStyles) == NUMBER_OF_MESSAGE_BOXES);
	ASSERT(ArrayLength(g_rnIconStyles) == NUMBER_OF_MESSAGE_BOXES);
	ASSERT(ArrayLength(g_rfBeepOnMessage) == NUMBER_OF_MESSAGE_BOXES);

// Initialize the help stack.

	g_HelpStack.SetSize(20, 20);
	g_nCrntHelp = 0;
	g_HelpStack[g_nCrntHelp] = 0;
	return;
}

/************************** DLL Exported Function ***************************\
 PushHelpContext()
 PopHelpContext()

 Purpose:	To set/reset the default message box F1-help context.

 Returns:	Push - Nada.  Pop - last help context

 Notes:		These functions should *not* be called directly.  The
 			CLocalHelpContext constuctor/destructor call them to set up local
			help contexts, and using this class guarantees that help contexts
			will be reset at the end of a scope block.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

void
PushHelpContext(
	UINT nNewHelp)
{
	g_nCrntHelp++;
	g_HelpStack.SetAtGrow(g_nCrntHelp, nNewHelp);
	return;
}

UINT
PopHelpContext()
{
	ASSERT(g_nCrntHelp > 0);
	g_nCrntHelp--;
	return(g_HelpStack[g_nCrntHelp + 1]);
}

/************************** DLL Exported Function ***************************\
 MsgText()

 Purpose:	To format text for display in a message box.

 Returns:	Constant pointer to the formatted text.

 Notes:		This is an overloaded function.  The implementation below is for
 			formatting file-related error messages in the standard COMMDLG
			format.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

LPCTSTR
MsgText(
	CString &strBuffer,
	const CPath &pathOffendingFile,
	LPCTSTR pszOperation,
	const CFileException *peOffence)
{
	CString strMsgFormat;
	CString strOffense;

// Fetch the strings we need

	VERIFY(strMsgFormat.LoadString(IDS_FILE_ERROR_FORMAT));
    VERIFY(strOffense.LoadString(IDS_FEXCEP_NONE +
    								peOffence->m_cause));

// Get some space in the buffer...

    int cchBuffer = strMsgFormat.GetLength() + MAX_SUBSTITUTION_BYTES;
    LPTSTR pszBuffer = strBuffer.GetBuffer(cchBuffer);

// ...and fill it.

	VERIFY(_sntprintf(pszBuffer, cchBuffer,
					(const TCHAR *) strMsgFormat,
					(const TCHAR *) pathOffendingFile,
					(const TCHAR *) pszOperation,
					(const TCHAR *) strOffense) < cchBuffer);
	strBuffer.ReleaseBuffer();

	return strBuffer;
}

/************************** DLL Exported Function ***************************\
 SubstituteBytes()

 Purpose:	To construct a display string given a CString buffer, a format
 			string, and a va_list of substitution variables.

 Returns:	Constant pointer to the constructed string.

 Notes:		This function should *not* be called directly.  It is used by the
 			inline versions of the MsgText() functions to construct message
 			box string.  See the header file for details.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

LPCTSTR
SubstituteBytes(
	CString &strBuffer,
	LPCTSTR pszFormat,
	va_list VarArguments)
{
// Get some space in the buffer...

    int cchBuffer = lstrlen(pszFormat) + MAX_SUBSTITUTION_BYTES;
    LPTSTR pszBuffer = strBuffer.GetBuffer(cchBuffer);

// ...and fill it.

	VERIFY(wvsprintf(pszBuffer, pszFormat, VarArguments) < cchBuffer);
	strBuffer.ReleaseBuffer();

	return strBuffer;
}

/************************** DLL Exported Function ***************************\
 MsgBox()

 Purpose:	To display a message box that conforms to the WADG.

 Returns:	The ID of the button pressed to dismiss the message.

 Notes:		- No mode or icon flags are allowed to be set for the message
 			box by the calling function.  If any are slipped in with the
			button options, then we ASSERT.
			- No button options are allowed with Information or FatalError
			message box types
			- If Dolphin is not active, then the IDE main frame is set into
 			a flash state & the message box is suppressed until Dolphin
 			becomes active.
			- The displayed icon & default buttons are determined by the
			g_rnDefaultButtons[] and g_rnIconStyles[] arrays.  See the
			WARNING!!! above.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

int
MsgBox(
	const MsgBoxTypes MsgBoxType,
	LPCTSTR pszMessage,
	UINT nIDButtons,	/* = DEFAULT_BUTTONS	*/
	UINT nHelpContext)	/* = DEFAULT_HELP		*/
{
		

// Entering sanity check:
// 	- no button options with an info or fatal error box, and
// 	- the button options contain no icon or mode requests.

	ASSERT(MsgBoxType != Information || nIDButtons == DEFAULT_BUTTONS);
	ASSERT(MsgBoxType != FatalError  || nIDButtons == DEFAULT_BUTTONS);

	ASSERT(nIDButtons == DEFAULT_BUTTONS || (nIDButtons & (~(MB_TYPEMASK | MB_DEFMASK))) == 0);

//
// If we are in automation mode, we shouldn't put up the message box.
//
	if ((!g_pAutomationState->DisplayUI()) || (theApp.m_bRunInvisibly))
	{
		if (theApp.m_bInvokedCommandLine)
		{
			theApp.WriteLog(pszMessage, TRUE);
		}
		switch(MsgBoxType)
		{
		// Pretend that we put up the dialog.
		case Information:
			return IDOK; 
			break ;

		// Put up the message box no matter what.
		case InternalError:
		case FatalError:
			// These are serious errors, so ignore the automation object!!!
			break ;

		// The following may have changed their button styles
		case Error:
			if (nIDButtons == DEFAULT_BUTTONS)
			{
				// Pretend that we did the message box.
				return IDOK;
			}
			else
			{
				// Don't know which button to press without a user!
				ASSERT(0) ;
			}
			break ;

		// We don't know how to handle the following!!!
		case Question:
		default:
			ASSERT(0) ;
			break ;
		};
		if (theApp.m_bInvokedCommandLine)
		{
			return IDOK; // make the best of it
		}
	}

// Do special processing for different box types.

	CString strErrText;
	BOOL fPanic;
	switch (MsgBoxType)
	{
	case InternalError:
	// Substitute the message into the "Internal Error" string
		pszMessage = MsgText(strErrText, IDS_INTERNAL_ERROR, pszMessage);
		break;

	case FatalError:
	// Start to panic.  Note that panicking is not implemented yet.
		fPanic = TRUE;
		break;

	default:
		break;
	}

// Set the message box style & F1 help context

	if (nIDButtons == DEFAULT_BUTTONS)
		nIDButtons = g_rnButtonStyles[MsgBoxType];
	if (nHelpContext == DEFAULT_HELP)
		nHelpContext = g_HelpStack[g_nCrntHelp];

	nIDButtons &= (MB_TYPEMASK | MB_DEFMASK);
	UINT nBoxStyle;
	if (MsgBoxType != InternalError)
		nBoxStyle = nIDButtons | g_rnIconStyles[MsgBoxType] | MB_APPLMODAL;
	else
		nBoxStyle = nIDButtons | g_rnIconStyles[MsgBoxType] | MB_SYSTEMMODAL;

	// testing would like a TRACE for all assertions
	if (MsgBoxType == InternalError)
	{
		::OutputDebugString(_T("Assertion failed: "));
		::OutputDebugString(pszMessage);
		::OutputDebugString(_T("\r\n"));
	}

// If we're not active, enter a new flash state pump.  (Do not do this for
// internal errors, because internal error message boxes could be coming
// from any thread, and FlashStatePump only works for the main thread.)

	if ((!theApp.m_fVCPPIsActive || theApp.m_pMainWnd->IsIconic()) && MsgBoxType != InternalError)
	{
		MessageBeep(g_rnIconStyles[MsgBoxType]);
		((CTheApp *)AfxGetApp())->FlashStatePump();
	}
	if (g_rfBeepOnMessage[MsgBoxType])
		MessageBeep(g_rnIconStyles[MsgBoxType]);

// We guarantee that the cursor is visible with a call to ShowCursor.
// NB:  if we don't have a mouse, the cursor show count *should*
// be -1, so incrementing won't cause the cursor to come up.

	ShowCursor(TRUE);

// Now pop the box.  For InternalErrors (assertions), use the desktop
// as the owner window rather than letting MFC decide what window to use
// as the owner, because we're multithreaded, and MFC may pick a window
// whose thread is currently blocked.

	int nRetCode;
	if (MsgBoxType != InternalError)
	{
		nRetCode = AfxMessageBox(pszMessage, nBoxStyle, nHelpContext);
	}
	else
	{
		PreModalWindow();
		AfxGetApp()->EnableModeless(FALSE);
		nRetCode = ::MessageBox(NULL, pszMessage, theApp.m_pszAppName,
			nBoxStyle);
		AfxGetApp()->EnableModeless(TRUE);
		PostModalWindow();
	}
	ShowCursor(FALSE);
	return nRetCode;
}

/*

The FatalErrorBox() code is still here for example purposes only.  This is
what the IDE did in a panic situation. [w-amilt]

void FatalErrorBox(
	UINT line1,
	LPSTR line2)
{
	char text[MAX_MSG_TXT], buffer[256];

	if (line1 == 0)
		*text = 0;
	else
		Dbg(LoadString(hInst, line1, text, sizeof(text)));

	if (line2)
		sprintf(buffer, "%s %Fs",text, line2);
	else
		sprintf(buffer, "%s",text);


	Dbg(MsgBox(FatalError, buffer));

	emergency = TRUE;

	// Don't send this message if hwndFrame doesn't exist yet!
	if (hwndFrame != NULL)
		SendMessage(hwndFrame, WM_CLOSE, 0, 0L);
}
*/

#ifndef DS_CONTEXTHELP
#define DS_CONTEXTHELP 0x2000L
#endif

static const int nFontSize = 10;

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog

int CMessageBox::DoMessageBox(LPCSTR lpszText, LPCSTR lpszCaption,
	LPCSTR lpszButtons, WORD wStyle, int nDef, int nCancel,
	DWORD* pHelpIDs, CWnd* pParentWnd)
{
	CMessageBox dlg(lpszText, lpszCaption, lpszButtons, wStyle, pHelpIDs,
		pParentWnd);
	dlg.SetDefault(nDef);
	dlg.SetCancel(nCancel);
	return dlg.DoModal();
}

CMessageBox::CMessageBox(LPCSTR lpszText, LPCSTR lpszCaption,
	LPCSTR lpszButtons, WORD wStyle, DWORD* pHelpIDs ,
	CWnd* pParentWnd)
{

	ASSERT(lpszText != NULL);
	ASSERT(lpszCaption != NULL);
	if (HIWORD(lpszText) == NULL)
		VERIFY(m_strText.LoadString(LOWORD((DWORD)lpszText)));
	else
		m_strText = lpszText;
	if (HIWORD(lpszCaption) == NULL)
		VERIFY(m_strCaption.LoadString(LOWORD((DWORD)lpszCaption)));
	else
		m_strCaption = lpszCaption;
	if (lpszButtons != NULL)
		AddButtons(lpszButtons);

	m_pParentWnd = pParentWnd;
	m_nDefButton = 0;
	m_nCancel = -1;
	m_pButtons = NULL;
	m_wStyle = wStyle;
	m_nBaseID = 10;		// don't use IDOK, IDCANCEL, etc
	m_hDlgTmp = NULL;

	m_pFont = GetStdFont(theApp.m_bWin4 ? font_Normal : font_Bold);

//	LOGFONT lf;
//	memcpy(&lf, &theApp.m_lf, sizeof(LOGFONT));
//	lf.lfHeight = -nFontSize;
//	lf.lfWidth = 0;
//	lf.lfWeight = FW_NORMAL;
//	VERIFY(m_font.CreateFontIndirect(&lf));

//	m_font.CreateFont(-nFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
//		FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
//		DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE, szFontName);
	m_pHelpIDs = pHelpIDs;
}

CMessageBox::~CMessageBox()
{
	delete [] m_pButtons;
	if (m_hDlgTmp != NULL)
		GlobalFree(m_hDlgTmp);
}

BEGIN_MESSAGE_MAP(CMessageBox, CDialog)
	//{{AFX_MSG_MAP(CMessageBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageBox message handlers

int CMessageBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (m_pHelpIDs != NULL)
	{
		for (int i=0;i<m_strArray.GetSize();i++)
			m_pHelpIDs[i*2] = i+m_nBaseID;
	}
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowText(m_strCaption);
	m_pButtons = new CButton[m_strArray.GetSize()];

	CRect rect(0, 0, 10, 10);
	if (!m_staticIcon.Create(NULL,
		SS_ICON | WS_GROUP | WS_CHILD | WS_VISIBLE, rect, this))
	{
		return -1;
	}
	m_staticIcon.SetIcon(::LoadIcon(NULL, GetIconID(m_wStyle)));

	if (!m_staticText.Create(m_strText, SS_LEFT | SS_NOPREFIX | WS_GROUP |
		WS_CHILD | WS_VISIBLE, rect, this))
	{
		return -1;
	}
	m_staticText.SetFont(m_pFont);
	
	for (int i=0;i<m_strArray.GetSize();i++)
	{
		if (!m_pButtons[i].Create(m_strArray[i], WS_TABSTOP | WS_CHILD |
			WS_VISIBLE | ((i == 0) ? WS_GROUP : 0) |
		    ((i == m_nDefButton) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
		    rect, this, i+m_nBaseID))
		{
			return -1;
		}
		m_pButtons[i].SetFont(m_pFont);
	}
	PositionControls();
	return 0;
}

BOOL CMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_pHelpIDs != NULL) // context help
		ModifyStyleEx(0, WS_EX_CONTEXTHELP); //add

	m_pButtons[m_nDefButton].SetFocus();	
	return FALSE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CMessageBox operations

void CMessageBox::AddButtons(LPCSTR lpszButton)
{
	CString str, strButtons;
	int i=0;
	if (HIWORD(lpszButton) == NULL)
		strButtons.LoadString(LOWORD((DWORD)lpszButton));
	else
		strButtons = lpszButton;
	while (AfxExtractSubString(str, strButtons, i++, '\n'))
		AddButton(str);
}

#ifndef DS_3DLOOK
#define DS_3DLOOK 0x4
#endif

void CMessageBox::FillInHeader(LPDLGTEMPLATE lpDlgTmp)
{
	lpDlgTmp->style = DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_VISIBLE |
		WS_CAPTION | WS_SYSMENU;
//	if (theApp.m_bOnChicago)
//		lpDlgTmp->style |= DS_CONTEXTHELP;		// we don't want to deal with Help here
	lpDlgTmp->dwExtendedStyle = 0;
	lpDlgTmp->cdit = 0;
	lpDlgTmp->x = 0;
	lpDlgTmp->y = 0;
	lpDlgTmp->cx = 100;
	lpDlgTmp->cy = 100;
	
	LPWSTR lpStr = (LPWSTR)(lpDlgTmp + 1); /* Move ptr to the variable fields */

	*lpStr++ = 0;  /* No Menu resource for Message Box */
	*lpStr++ = 0;  /* No Class name for MessageBox */
	
	mbstowcs(lpStr, m_strCaption, m_strCaption.GetLength());

	lpStr += m_strCaption.GetLength()+1;
	WORD* pWord = (WORD*)lpStr;
	*pWord = 10; // 10 pt font
	pWord++;
	lpStr = (LPWSTR) pWord;

	const LOGFONT* plf = GetStdLogfont(font_Normal);
	LPCTSTR lpsz = plf->lfFaceName;
	mbstowcs(lpStr, lpsz, lstrlen(lpsz));
}

/////////////////////////////////////////////////////////////////////////////
// CMessageBox overridables

BOOL CMessageBox::OnCommand(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam == IDCANCEL && m_nCancel != -1)
	{
		EndDialog(m_nCancel);
		return TRUE;
	}
	if (::GetDlgItem(m_hWnd, wParam)==NULL)
		return FALSE;
	EndDialog(wParam-m_nBaseID);
	return TRUE;
}

int CMessageBox::DoModal()
{
	ASSERT(m_strArray.GetSize() != 0);
	if (m_strArray.GetSize() == 0)
		return (m_nCancel != -1) ? m_nCancel : 0;

	// compute size of header
	// Fixed portions of DLG template header = sizeof(DLGTEMPLATE);
    // One null byte for menu name and one for class name = 2
	// Caption text plus NULL = m_strCaption.GetLength()+1
	int nSize = sizeof(DLGTEMPLATE);
	const LOGFONT* plf = GetStdLogfont(font_Normal);
	nSize += (2 + m_strCaption.GetLength()+1+lstrlen(plf->lfFaceName)+1)*2 +sizeof(WORD);
	m_hDlgTmp = GlobalAlloc(GPTR, nSize);
	if (m_hDlgTmp == NULL)
		return IDCANCEL;
	LPDLGTEMPLATE lpDlgTmp = (LPDLGTEMPLATE)GlobalLock(m_hDlgTmp);
	FillInHeader(lpDlgTmp);
	GlobalUnlock(m_hDlgTmp);
	InitModalIndirect(m_hDlgTmp);	

	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMessageBox implementation

void CMessageBox::PositionControls()
{
	CSize sizeBase = GetBaseUnits();
	int nButtonHeight = (sizeBase.cy*14)/8;
	int nHeight = 0;
	int nSep,nLeftMargin,nRightMargin,nTopMargin,nBottomMargin;
    int nButtonAdj;
    int nWidth = 0;
    CRect rectText;

	// a) 5/8 screen Width
	// b) Caption
	// c) nLeftMargin ICON nSep TEXT nRightMargin
	// d) nLeftMargin Button1 nSep Button2 ... nRightMargin
	// client width is max(b,d, min(c,a))

	CSize sizeIcon(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	nSep = sizeIcon.cx/2;
	nLeftMargin = nSep;
	nRightMargin = nSep;
	nTopMargin = nSep;
	nBottomMargin = nSep;
	
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(m_pFont);
	
	nButtonAdj = dc.GetTextExtent("XXX",3).cx; // padding on buttons
	
	int nScreenWidth58 = dc.GetDeviceCaps(HORZRES)*5/8;
	int nCaptionWidth = dc.GetTextExtent(m_strCaption, m_strCaption.
		GetLength()).cx;
//	CSize sizeText = dc.GetTextExtent(m_strText,m_strText.GetLength());
	CRect rcText(0, 0, 0, 0);
	CSize sizeText = rcText.Size();
	dc.DrawText(m_strText,m_strText.GetLength(), &rcText, DT_CALCRECT);
	int nTextIconWidth = nLeftMargin+sizeIcon.cx+nSep+sizeText.cx+nRightMargin;
	int nButtons = m_strArray.GetSize();
	int nButtonsWidth = nLeftMargin+nRightMargin+(nButtons-1)*nSep;
	for (int i=0;i<nButtons;i++)
	{
		nButtonsWidth +=
			dc.GetTextExtent(m_strArray[i],m_strArray[i].GetLength()).cx+
			nButtonAdj*2;
	}
	
	nWidth = min(nTextIconWidth,nScreenWidth58);
	nWidth = max(nWidth, nCaptionWidth);
	nWidth = max(nWidth, nButtonsWidth);

	m_staticIcon.SetWindowPos(NULL, nLeftMargin, nTopMargin, sizeIcon.cx,
		sizeIcon.cy, SWP_NOZORDER);

	if (sizeText.cx > nWidth-nLeftMargin-nRightMargin-sizeIcon.cx-nSep)
	{
		sizeText.cx = nWidth-nLeftMargin-nRightMargin-sizeIcon.cx-nSep;
//		int nTextWidth = nWidth-nLeftMargin-nRightMargin-sizeIcon.cx-nSep;
//		rectText.SetRect(0, 0, nTextWidth, 32767);
		rectText.SetRect(0, 0, sizeText.cx, 32767);
		/* Ask DrawText for the right cy */
		sizeText.cy = dc.DrawText(m_strText, m_strText.GetLength(), &rectText,
			DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS | DT_NOPREFIX);
	}
	
	m_staticText.SetWindowPos(NULL, nSep+sizeIcon.cx+nSep, nTopMargin,
		sizeText.cx, sizeText.cy, SWP_NOZORDER);
	
	sizeText.cy = max(sizeText.cy, sizeIcon.cy); // at least icon height
	nHeight = nTopMargin + sizeText.cy + nSep + nButtonHeight + nBottomMargin;

	CRect rect;
	rect.left = (nWidth - (nButtonsWidth - nLeftMargin - nRightMargin))/2;
	rect.top = nTopMargin + sizeText.cy + nSep;
	rect.bottom = rect.top + nButtonHeight;

	for (i=0;i<m_strArray.GetSize();i++)
	{
		rect.right = rect.left + dc.GetTextExtent(m_strArray[i],m_strArray[i].GetLength()).cx +
			2*nButtonAdj;
		m_pButtons[i].MoveWindow(&rect);
		rect.left = rect.right + nSep;
	}

	rect.SetRect(0,0,nWidth,nHeight);
	CalcWindowRect(&rect);
	SetWindowPos(NULL, (dc.GetDeviceCaps(HORZRES)-rect.Width())/2,
		(dc.GetDeviceCaps(VERTRES)-rect.Height())/2, rect.Width(), rect.Height(),
		SWP_NOZORDER|SWP_NOACTIVATE);
	if(m_nCancel == -1) // no cancel button
	{
//		CMenu* pMenu = GetSystemMenu(FALSE);
//		if (pMenu != NULL)
//			pMenu->DeleteMenu(SC_CLOSE, MF_BYCOMMAND);
	}
	dc.SelectObject(pOldFont);
}

CSize CMessageBox::GetBaseUnits()
{
	CWindowDC dc(GetDesktopWindow());
	CFont* pFont = dc.SelectObject(m_pFont);
	TEXTMETRIC tm;
	VERIFY(dc.GetTextMetrics(&tm));
	dc.SelectObject(pFont);
	return CSize(tm.tmAveCharWidth, tm.tmHeight);
}

LPCSTR CMessageBox::GetIconID(WORD wFlags)
{
	wFlags &= MB_ICONMASK;
	switch( wFlags )
	{
		case MB_ICONHAND:
			return IDI_HAND;
		case MB_ICONQUESTION:
			return IDI_EXCLAMATION;
//			return IDI_QUESTION; NOTE: style guide says don't use this!
		case MB_ICONEXCLAMATION:
			return IDI_EXCLAMATION;
		case MB_ICONASTERISK:
			return IDI_ASTERISK;
	}
	return NULL;
}	

//
// bpdlgex.cpp
//
// tabbed breakpoints dialog implementation... (version II)
//

#include "stdafx.h"
#pragma hdrstop
#include "dbg.hid" // context help ids

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define CB_MAX_LOCATION 	255
#define CB_MAX_EXPRESSION 	255
#define CB_MAX_LENGTH		20
#define CB_MAX_PASSCNT		20
#define CB_MAX_MESSAGE		128
#define CB_TMP_BUF_SIZE		1024  // Allocate a large buffer so we don't overflow

// The EE chokes on identifiers > 255, so we use our own MAX_PATH instead
// of the CRTs which is 260.
#define CB_MAX_PATH			255

// All extracted from CVDEF.H
typedef PROCSYM16		far *PROCPTR16;
typedef PROCSYM32		far *PROCPTR32;
typedef PROCSYMMIPS		far *PROCPTRMIPS;
typedef TYPTYPE 		far *TYPPTR;
typedef SYMTYPE 		far *SYMPTR;
typedef HMEM			HMDS ;
typedef lfArgList		far *plfArgList;
typedef lfProc			far *plfProc;
typedef lfStructure 	far *plfStructure;
typedef lfFieldList 	far *plfFieldList;
#define hmdsNull		(HMDS)NULL

void SetLastBreakpointNode(PBREAKPOINTNODE pbpn);
BOOL FBreakWhenExprTrue(LPCSTR szExpr);

extern BOOL FAddrFromSz(char *, ADDR*);

// Use this to dynamically add menuitems
BEGIN_POPUP_MENU(EmptyMenu)
END_POPUP_MENU()

#define theApp (*((CTheApp*) AfxGetApp()))

//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBPDlgEx, CTabbedDialog)

BEGIN_MESSAGE_MAP(CBPDlgEx, CTabbedDialog)
	//{{AFX_MSG_MAP(CBPDlgEx)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_BP_LIST, OnChangedSel)
	ON_BN_CLICKED(IDC_BP_REMOVE, OnRemoveBP)
	ON_BN_CLICKED(IDC_BP_GOTOCODE, OnGotoCode)
	ON_BN_CLICKED(IDC_BP_REMOVE_ALL, OnRemoveAllBP)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnInitMoveFocus)
END_MESSAGE_MAP()

CBPDlgEx *pbpDlg;

static const int mpBptItab[] =
{
	0,		// BPLOC
	0,		// BPLOCEXPRTRUE
	0,		// BPLOCEXPRCHGD
	1,		// BPEXPRTRUE
	1,		// BPEXPRCHGD
	2		// BPWNDPROCMSGRCVD
};	// set to the preferred tab...

// construst the breakpoints dialog
CBPDlgEx::CBPDlgEx() : CTabbedDialog(IDS_BPDLG, NULL, 0)
{
	m_pdlgLocation	= new CLocationTab;
	m_pdlgData  	= new CDataTabEx;
	m_pdlgMessages	= new CMessagesTabEx;

	// for access by the individual tabs...
	pbpDlg 			= this;

	// these fields keep state for the operation the user
	// is in the middle of doing	
	m_fCurBPDirty	  = FALSE;
	m_fSelfChange	  = FALSE;
	m_fInsertingNew   = FALSE;
	m_iItemChanging   = -1;

	// these fields represent the current breakpoint
	m_bptypeCur		  = BPNIL;
	m_szLocation[0]   = 0;
	m_szExpression[0] = 0;
	m_szPassCnt[0] 	  = 0;
	m_szMessage[0] 	  = 0;
	m_fExpTrue		  = FALSE;
	m_iedLocStart	  = 0;
	m_iedLocEnd		  = -1;
	m_iedExpStart	  = 0;
	m_iedExpEnd		  = -1;
	m_itabPrev		  = -1;

	_tcscpy(m_szLength, "1");
}	

CBPDlgEx::~CBPDlgEx()
{
}

// create the breakpoints dialog and set up the child windows
// that are not part of the tabs (ie. the buttons & listbox)
int CBPDlgEx::OnCreate(LPCREATESTRUCT lpcs)
{
	CString str;
	RECT rect = {0};
	DWORD dwStyle;

	AddTab(m_pdlgLocation);
	// Java: no data or message bps for Java, so don't add the tabs
	if (pDebugCurr && pDebugCurr->IsJava())
		;
	else
	{
		AddTab(m_pdlgData);
		AddTab(m_pdlgMessages);
	}

	dwStyle = LBS_EXTENDEDSEL|LBS_HASSTRINGS|LBS_NOTIFY|
	 		  WS_BORDER|WS_CHILD|WS_TABSTOP|WS_VSCROLL;

	if (!m_lstBP.Create(dwStyle, rect, this, IDC_BP_LIST))
		return -1;
	m_lstBP.SetWindowContextHelpId(HIDC_BP_LIST);

	VERIFY(str.LoadString(IDS_BPLABEL));
	dwStyle = SS_LEFT|WS_CHILD|WS_GROUP;
	if (!m_lblBP.Create(str, dwStyle, rect, this, IDC_STATIC))
		return -1;

	// style for all our buttons...
	dwStyle = BS_PUSHBUTTON|WS_CHILD|WS_TABSTOP;

	VERIFY(str.LoadString(IDS_BP_GOTOCODE));
	if (!m_butGotoCode.Create(str, dwStyle, rect, this, IDC_BP_GOTOCODE))
		return -1;
	m_butGotoCode.SetWindowContextHelpId(HIDC_BP_GOTOCODE);
	m_butGotoCode.EnableWindow(FALSE);

	VERIFY(str.LoadString(IDS_BP_REMOVE));
	if (!m_butRemove.Create(str, dwStyle, rect, this, IDC_BP_REMOVE))
		return -1;
	m_butRemove.SetWindowContextHelpId(HIDC_BP_REMOVE);
	m_butRemove.EnableWindow(FALSE);

	VERIFY (str.LoadString (IDS_BP_REMOVE_ALL));
	if (!m_butRemoveAll.Create (str, dwStyle, rect, this, IDC_BP_REMOVE_ALL))
		return -1;
	m_butRemoveAll.SetWindowContextHelpId(HIDC_BP_REMOVE_ALL);
	m_butRemoveAll.EnableWindow (FALSE);
	
//	PostMessage(WM_USER, 0, 0L);

	return CTabbedDialog::OnCreate(lpcs);
}

LRESULT CBPDlgEx::OnInitMoveFocus(WPARAM wParam, LPARAM lParam)
{
	SetFocusToControl(&m_pdlgLocation->m_edLocation);
	return 0;
}

// fill the breakpoints list with the current breakpoints
void CBPDlgEx::FillBPList()
{
	m_lstBP.ResetContent();

	PBREAKPOINTNODE pbpn = BHFirstBPNode();
	while (pbpn)
	{
		if (!pbpnMarkDel(pbpn))	// do not add items marked for deletion...
		{
			char szBigBuffer[cbBpCmdMax];

			// FUTURE -billjoy- should the context be set up here?
			// Format the bp and update the list box
			FormatDisplayBP (pbpn, szBigBuffer, sizeof(szBigBuffer));
			m_lstBP.AddString(szBigBuffer);
		}
		pbpn = pbpnNext (pbpn);
	}
	m_lstBP.AddString("->");

	int iSel = m_lstBP.GetCount() - 1;
	m_lstBP.SetSel(iSel);
	m_lstBP.SetCaretIndex(iSel);
	m_butRemoveAll.EnableWindow (iSel > 0);
}

// the breakpoints list selection changed, we must check to see if a
// breakpoint needs to be committed and remember which breakpoint we're
// now editing, plus change the dialog state to reflect the selected
// breakpoint
void CBPDlgEx::OnChangedSel()
{
	CListBox *plst = &pbpDlg->m_lstBP;
	int iItemSel  = plst->GetCaretIndex();
	int cItemsMac = plst->GetCount();

	if (m_bptypeCur != BPNIL && !AddReplaceBP())
	{
		plst->SelItemRange(FALSE, 0, cItemsMac-1);
		plst->SetSel(m_iItemChanging);
		plst->SetCaretIndex(m_iItemChanging);
		return;
	}
	
	m_fCurBPDirty	= FALSE;
	m_bptypeCur     = BPNIL;
	m_fInsertingNew = FALSE;
	m_szPassCnt[0]  = 0;	// don't preserve passcount
	m_iItemChanging = iItemSel;

	BOOL fRestoreFocus = FALSE;
	if (CWnd::GetFocus() == pbpDlg->GetDlgItem(IDC_BP_LIST)) {
		fRestoreFocus = TRUE;
	}
	SetBPFields();
	if (fRestoreFocus) {
		SetFocusToControl(plst);
	}

	for (int i=0; i < cItemsMac -1; i++)
		if (plst->GetSel(i))
			break;

	BOOL fCanRemove = (i < cItemsMac - 1);

	// move focus to OK button if we can't remove anymore
	if (!fCanRemove &&
		(m_butRemove.m_hWnd == ::GetFocus() || m_butRemoveAll.m_hWnd == ::GetFocus ()))
	{
		SetFocusToControl(GetButtonFromIndex(0));
		SetDefButtonIndex(0);
	}

	m_butRemove.EnableWindow(fCanRemove);
	m_butRemoveAll.EnableWindow (cItemsMac > 1);

	// if we're not in the middle of editing a new breakpoint then set the button
	// according to the current selection...
	// if we are in the middle of editing a breakpoin then we can't do a GetBreakpoint(i)
	// and we'll use the breakpoint type and fields to guess if we can enable the button
	// or not as the edit proceeds.

	if (m_bptypeCur == BPNIL)
		m_butGotoCode.EnableWindow(fCanRemove && BHCanGotoBP(GetBreakPoint(i)));
}

// get the ith undeleted breakpoint from the breakpoint list
// bogus indices will return NULL
PBREAKPOINTNODE CBPDlgEx::GetBreakPoint(UINT ibp)
{
	PBREAKPOINTNODE	pbpn = BHFirstBPNode();
	PBREAKPOINTNODE pbpnPrev = (PBREAKPOINTNODE)NULL;

	// If we're asked for item 0, we want to find the 1st non-deleted node
	++ibp;

	do
	{
		if (pbpn == NULL)
			return NULL;

		if (!pbpnMarkDel(pbpn))
		{
			pbpnPrev = pbpn;
			--ibp;
		}
		pbpn = pbpnNext(pbpn);
	} while (ibp);

	return pbpnPrev;
}

void CBPDlgEx::ClearBPFields()
{
	m_szLocation[0]   = 0;
	m_szExpression[0] = 0;
	m_szPassCnt[0]    = 0;
	m_szMessage[0]    = 0;
	strcpy(m_szLength, "1");

	m_iedLocStart = 0;
	m_iedLocEnd = -1;
	m_iedExpStart = 0;
	m_iedExpEnd = -1;
}

// Set the fields of the dialog as appropriate for the current
// selected breakpoint.  The dialogs breakpoint state variables
// are also set to reflect the new current breakpoint.
void CBPDlgEx::SetBPFields()
{
	CListBox *plst = &pbpDlg->m_lstBP;
	int iItemSel = plst->GetCaretIndex();
	int iItemLast = plst->GetCount() - 1;

	ClearBPFields();

	if (iItemSel != iItemLast)
	{
		PBREAKPOINTNODE pbpn = GetBreakPoint(iItemSel);
		ASSERT(pbpn);
		m_bptypeCur = pbpnType(pbpn);
		ASSERT(m_bptypeCur >= BPLOC && m_bptypeCur <= BPWNDPROCMSGRCVD);

		// Enable/disable "goto bp" button
		m_butGotoCode.EnableWindow(BHCanGotoBP(pbpn));

		//
		// Get the other fields depending on the type:
		//

		switch (m_bptypeCur)
		{
		case BPLOC:
		case BPLOCEXPRTRUE:
		case BPLOCEXPRCHGD:
		case BPWNDPROCMSGRCVD:
			BuildCV400Location(pbpn, m_szLocation, sizeof(m_szLocation), TRUE, TRUE, FALSE, FALSE);
		default:
			break;
		}

		switch (m_bptypeCur)
		{
		case BPLOCEXPRTRUE:
		case BPLOCEXPRCHGD:
		case BPEXPRTRUE:
		case BPEXPRCHGD:
			BuildCV400Expression(pbpn, m_szExpression, sizeof(m_szExpression), TRUE, TRUE, FALSE);
		default:
			break;
		}

		switch (m_bptypeCur)
		{
		case BPLOCEXPRCHGD:
		case BPEXPRCHGD:
			_itoa (pbpnExprLen(pbpn), m_szLength, 10);
		default:
			break;
		}

		if (m_bptypeCur == BPWNDPROCMSGRCVD)
		{
			// Set the Message fields into Messages
			UINT msgNum = pbpnMessageNum(pbpn);
			UINT msgCls = pbpnMessageClass(pbpn);

			if (msgCls != msgSingleOnly)
				msgNum = 0;

			LoadString(hInst, msgNum + DBG_Msgs_Start, m_szMessage, sizeof(m_szMessage));
		}

		// Pass count
		if (pbpnPassCount(pbpn))
			_itoa(pbpnPassCount(pbpn), m_szPassCnt, 10);
		else
			*m_szPassCnt = '\0';

		//m_fChanges = (pbpnType(pbpn) == BPLOCEXPRCHGD || pbpnType(pbpn) == BPEXPRCHGD);
		m_fExpTrue = (pbpnType(pbpn) == BPLOCEXPRTRUE || pbpnType(pbpn) == BPEXPRTRUE);

		if (mpBptItab[m_bptypeCur] != m_itabPrev)
		{
			SelectTab(mpBptItab[m_bptypeCur]);
		}
	}

	// Now set the fields

	m_pdlgLocation->SetBPFields();
	m_pdlgData->SetBPFields();
	m_pdlgMessages->SetBPFields();
	//m_pdlgAdvanced->SetBPFields();
}

// Accept changes and dismiss the dialog.
// Add/Replace any breakpoint that is pending addition before exiting.
void CBPDlgEx::OnOK()
{
	if (m_bptypeCur != BPNIL && !AddReplaceBP())
		return;
	
	AcceptChanges();
	CTabbedDialog::OnOK();
}

// make a pass over the breakpoint list, remove any nodes that need
// removal and clear the modification state bits so the list is clean
void CBPDlgEx::AcceptChanges()
{
	PBREAKPOINTNODE pbpn;
	PBREAKPOINTNODE	pbpnCur;
	int				ibp;

	// We do this in two passes to ensure that remaining breakpoints
	// are highlighted. (case where there is more than breakpoint
	// on a line.)

	// Deletions first
	pbpn = BHFirstBPNode ();
	ibp = 0;

	while (pbpn != NULL)
	{
		pbpnCur = pbpn;
		pbpn = pbpnNext (pbpn);

		// If marked delete, delete the node permanently
		if (pbpnMarkDel(pbpnCur))
		{
			DeleteBreakpointNode (ibp);

			// Don't do anything else with this bp - it has been deleted!
			continue;
		}

		// Marked enable, change the status from disable to enable and enable
		// real hbpi
		else if (pbpnMarkEnable(pbpnCur))
		{
			if (DebuggeeAlive() && !pbpnBPVirtual(pbpnCur))
			{
				BPEnableFar(pbpnBPhbpi(pbpnCur));
			}
			pbpnEnabled(pbpnCur) = TRUE;
		}

		// Marked disable, change the status from enable to disable and disable
		// real hbpi
		else if (pbpnMarkDisable(pbpnCur))
		{
			if (DebuggeeAlive() && !pbpnBPVirtual(pbpnCur))
			{
				BPDisableFar(pbpnBPhbpi(pbpnCur));
			}
			pbpnEnabled(pbpnCur) = FALSE;
		}

		pbpnMarkAdd(pbpnCur) = FALSE;
		pbpnMarkEnable(pbpnCur) = FALSE;
		pbpnMarkDisable(pbpnCur) = FALSE;

		// Keep track so deleting will be correct
		++ibp;
	}

	// Update the UI after the list has been updated!
	pbpn = BHFirstBPNode ();
	while (pbpn != NULL)
	{
		BFHShowBPNode (pbpn) ;
		pbpnMarkAdd (pbpn) = FALSE;
		pbpn = pbpnNext (pbpn);
	}

	UpdateBPInViews();
}

// reject any changes that the user has made and dimiss the dialog
void CBPDlgEx::OnCancel()
{
	RejectChanges();
	CTabbedDialog::OnCancel();
}

// make pass over the list of breakpoints, restore the original
// breakpoints that are pending deletion and remove any that were
// added while the dialog was up.  Then clear the modfication bits
void CBPDlgEx::RejectChanges()
{
	PBREAKPOINTNODE pbpn = BHFirstBPNode();
	int ibp = 0;

	while (pbpn)
	{
		PBREAKPOINTNODE	pbpnCur = pbpn;
		pbpn = pbpnNext(pbpn);

		// Delete nodes marked for add
		if (pbpnMarkAdd(pbpnCur))
		{
			DeleteBreakpointNode(ibp);
		}
		else
		{
			// Reset all other nodes to regular settings
			pbpnMarkDel(pbpnCur) = FALSE;
			pbpnMarkEnable(pbpnCur) = FALSE;
			pbpnMarkDisable(pbpnCur) = FALSE;

			// increment counter
			++ibp;
		}
	}
}

// bring up the breakpoints dialog
void DoBPDlgEx()
{
	CBPDlgEx dlg;
	dlg.DoModal();
}

#define CY_LIST  100
#define CY_LABEL 15
#define CY_SPACE 3

// compute the space we need to reserve to put the buttons on the right
// of the breakpoints dialog and the listbox on the bottom
void CBPDlgEx::GetMargins(CRect& rect)
{
	CTabbedDialog::GetMargins(rect);

	// Override to specify left, right, top and bottom margins around the tabs
	rect.SetRect(cxTabMargin, cyTabMargin, cxTabMargin, cyTabMargin);

	// If we have any buttons, leave space for them at the right
	if (GetButtonFromIndex(0) != NULL)
		rect.right += cxTabMargin + m_buttonSize.cx;

	rect.bottom += CY_LIST + CY_SPACE + CY_LABEL;
}

// subclass hook, not used yet
void CBPDlgEx::CreateButtons()
{
	CTabbedDialog::CreateButtons();
}

// return the button from the button number, the first button 2 buttons
// are handled by the base class (OK, Cancel)
CButton* CBPDlgEx::GetButtonFromIndex(int index)
{
	switch (index)
	{
	case 2:
		return &m_butGotoCode;
	case 3:
		return &m_butRemove;
	case 4:
		return &m_butRemoveAll;
	default:
		break;
	}
	return CTabbedDialog::GetButtonFromIndex(index);
}

// Put the buttons where they belong in the dialog.
// Use the computed margins and #defines for spacing
// to lay them out in a column on the right.
void CBPDlgEx::MoveButtons(int nLeft, int nCenter)
{
	// Position the buttons along the right of the dialog
	CRect rcWin;
	GetClientRect(rcWin);

	int x  = cxTabMargin;
	int y  = rcWin.bottom - CY_LIST - CY_LABEL - CY_SPACE;
	int cx = rcWin.right - m_buttonSize.cx - 3*cxTabMargin;

	m_lblBP.SetFont(GetStdFont(font_Normal));
	m_lblBP.SetWindowPos(&wndBottom, x, y, cx, CY_LABEL, SWP_NOACTIVATE);
	m_lblBP.ShowWindow(SW_SHOW);

	y  = rcWin.bottom - CY_LIST;

	FillBPList();
	int cItemsMac = m_lstBP.GetCount();
	m_lstBP.SetCaretIndex(cItemsMac-1);
	m_lstBP.SetFont(GetStdFont(font_Normal));
	m_lstBP.SetWindowPos(&m_lblBP, x, y, cx, CY_LIST - cyTabMargin, SWP_NOACTIVATE);
	m_lstBP.ShowWindow(SW_SHOW);

	x = rcWin.right - m_buttonSize.cx - cxTabMargin;
	y = cyTabMargin;

	CButton* pButton;
	CButton* pButtonPrev = NULL;
	int index = 0;

	// Cycle through all the buttons, positioning them along the bottom
	for (pButton = GetButtonFromIndex(index) ; pButton != NULL ;
	     pButton = GetButtonFromIndex(++index))
	{
		if (index == 3)	// remove button	(beside the list)
			y = rcWin.bottom - CY_LIST;

		pButton->SetFont(GetStdFont(font_Normal));

		pButton->SetWindowPos(pButtonPrev == NULL ? (CWnd*)&m_lstBP : (CWnd*)pButtonPrev,
		                      x, y, m_buttonSize.cx, m_buttonSize.cy, SWP_NOACTIVATE);
		pButton->ShowWindow(SW_SHOW);

		y += m_buttonSize.cy + m_cxBetweenButtons/2;
		pButtonPrev = pButton;
	}
}

// The user has finished entering his new breakpoint or changing an
// existing one.   Update the breakpoint list to reflect those changes.
BOOL CBPDlgEx::AddReplaceBP()
{
	if (!m_fCurBPDirty || m_bptypeCur == BPNIL)
		return TRUE;

	BREAKPOINTNODE bp;
	int iRet = GetBPNodeFromControls(&bp);

	// if we can't get a breakpoint node we're outta here...
	if (iRet != ERR_NULL)
	{
		HandleBPError(iRet, FALSE);
		return FALSE;
	}

	// now try to add the node to the list... if this fails it means
	// we were running and the breakpoint could not be instantiated
	// or else we just ran out of memory
	
	PBREAKPOINTNODE pbpn = AddBreakpointNode(&bp, TRUE, FALSE, FALSE, &iRet);
	
	if (!pbpn)
	{
		// a message has already been displayed by the low level if we fail at
		// this stage.  Furthermore the return codes don't map to the internal errors we have
		// we need to map them to our error fields if we want to move the focus to the
		// right place.  [rm]

		switch (iRet)
		{
		case BPCODEADDR: iRet = ERR_LOC;     break;
		case BPDATAADDR: iRet = ERR_EXPR;    break;
		case BPLENGTH:	 iRet = ERR_LEN;     break;
		case BPPASSCNT:  iRet = ERR_PASSCNT; break;

		case BPOPTCMD:
		case BPFIELDCNT:
		default:		
			iRet = ERR_NULL;
			break;
		}

		if (iRet != ERR_NULL) {
			HandleBPError(iRet, TRUE);
		}
		return FALSE;
	}

	// mark the breakpoint as added...
	pbpnMarkAdd(pbpn) = TRUE;

	if (!m_fInsertingNew)
	{
		// if we're here it means that we're replacing an existing
		// breakpoint, we have to mark that one to be deleted

		ASSERT(m_iItemChanging != -1);

		// get the node that we're changing
		PBREAKPOINTNODE	pbpnCur = GetBreakPoint(m_iItemChanging);
		ASSERT(pbpnCur != NULL);

		// If the new node is not already in the right place then we have
		// to move it.  We put it right after the node we are changing so
		// it will effectively replace the old breakpoint...

		if (pbpnNext(pbpnCur) != pbpn)
		{			
			// remove the new node from the end of the list...
			PBREAKPOINTNODE pbpnT = BHFirstBPNode();
			while (pbpnNext(pbpnT) != pbpn)
				pbpnT = pbpnNext(pbpnT);
			pbpnNext(pbpnT) = NULL;
			SetLastBreakpointNode(pbpnT);
		
			// insert it back at the right place...

			pbpnNext(pbpn) = pbpnNext(pbpnCur);
			pbpnNext(pbpnCur) = pbpn;
		}

		// If the node we're changing was added during this session we nuke
		// it right away... we don't have to keep it so we can cancel out.
		// Otherwise just mark it for deletion -- the change is effected by
		// a delete/add pair in the breakpoint list.

		if (pbpnMarkAdd(pbpnCur))
			//DeleteBreakpointNode(m_iItemChanging);
			// This should never fail since we supposedly just added this bp
			VERIFY(DeleteBreakpointNode(pbpnCur));
		else
			pbpnMarkDel(pbpnCur) = TRUE;
	}

	m_fInsertingNew = FALSE;
	m_fCurBPDirty = FALSE;
	//m_iItemChanging  = -1;
	//m_bptypeCur 	= BPNIL;

	// Enable/disable "goto bp" button
	m_butGotoCode.EnableWindow(BHCanGotoBP(pbpn));

	return TRUE;
}

// Examine the current controls and construct a breakpoint node
// from their contents...
int CBPDlgEx::GetBPNodeFromControls(PBREAKPOINTNODE pbp)
{
	// Clear all fields in the BREAKPOINTNODE global
	_fmemset(pbp, 0, sizeof(BREAKPOINTNODE));

	BREAKPOINTTYPES bpType = m_bptypeCur;
	pbpnType(pbp) = bpType;
	ASSERT(bpType != BPNIL);

	// Location
	if (bpType == BPLOC || bpType == BPLOCEXPRTRUE || bpType == BPLOCEXPRCHGD)
	{
		if (!ParseCV400Location(m_szLocation, pbp) &&
		    !ParseQC25Location (m_szLocation, pbp))
			return ERR_LOC ;
	}

	// Wnd Proc
	if (bpType == BPWNDPROCMSGRCVD)
	{
		if (!ParseWndProc(m_szLocation, pbp))
			return ERR_PROC ;
	}

	// Expression
	if (bpType >= BPLOCEXPRTRUE && bpType <= BPEXPRCHGD)
	{
		if (!ParseExpression(m_szExpression, pbp))
			return ERR_EXPR;
	}

	LONG lTmp;

	// Length
	if (bpType == BPLOCEXPRCHGD || bpType == BPEXPRCHGD)
	{
		// check for valid length
		if (!fScanAnyLong(m_szLength, &lTmp, 1L, (long)INT_MAX))
			return ERR_LEN;

		pbpnExprLen(pbp) = lTmp;
	}

	// Pass count
	if (m_szPassCnt[0])
	{
		// check for valid pass count
		if (!fScanAnyLong(m_szPassCnt, &lTmp, 0L, (long)INT_MAX)) // SHRT_MAX
			return ERR_PASSCNT;

		pbpnPassCount(pbp) = (USHORT)lTmp;
	}
	else
	{
		pbpnPassCount(pbp) = 0;
	}

	// Messages
	if (bpType == BPWNDPROCMSGRCVD)
	{
		int 	i;
		TCHAR *	ptch;
		TCHAR *	ptchNext;

		char BigBuffer[255];
		_tcsncpy(BigBuffer, m_szMessage, sizeof(BigBuffer)-1);

		ptch = BigBuffer;
		while (*ptch)	// squeeze out all whitespace, DBCS safe
		{
			ptchNext = ptch;

			// skip over white space
			while (_istspace(*ptchNext)) ptchNext = _tcsinc(ptchNext);

			if (ptch != ptchNext)
				memmove(ptch, ptchNext, _tcslen(ptchNext) + 1);

			// locate end of non-white space
			while (*ptch && !_istspace(*ptch)) ptch = _tcsinc(ptch);
		}

		i = DBG_Msgs_Start;

		do
		{
			LoadString(hInst, i++, szTmp, sizeof(szTmp));
		}
		while (_tcsicmp(BigBuffer, szTmp) && szTmp[0]);

		// no message selected or invalid message...
		if (!szTmp[0])
			return ERR_MSG ;

		pbpnMessageClass(pbp)  = msgSingleOnly;
		pbpnMessageNum(pbp)    = i - 1 - DBG_Msgs_Start;
	}

	// make it an enabled breakpoint node...
	pbpnEnabled(pbp) = TRUE;

	// If get to here the Breakpoint action has necessary data
	return ERR_NULL ;
}

// insert/replace the given formatted breakpoint into the listbox
void CBPDlgEx::UpdateListString(char *buf)
{
	CListBox *plst = &pbpDlg->m_lstBP;
	int iItemSel   = plst->GetCaretIndex();
	int cItemsMac  = plst->GetCount();

	// Is this a brand new bp?
	if (iItemSel == -1 || iItemSel == cItemsMac - 1)
	{
		if (FCurrentBPIsEmpty())
		{
			// We don't want to add an empty bp - that would be "bad"
			return;
		}

		m_fInsertingNew = TRUE;
		m_iItemChanging = cItemsMac-1;

		plst->InsertString(cItemsMac-1, buf);
		plst->SetCaretIndex(cItemsMac-1);
		plst->SelItemRange(FALSE, 0, cItemsMac);
		plst->SetSel(cItemsMac-1);

		m_butRemove.EnableWindow(TRUE);		// there is now a BP to delete
		m_butRemoveAll.EnableWindow (TRUE);
		return;
	}

	// If we have an empty bp, delete the current string entirely
	if (FCurrentBPIsEmpty())
	{
		plst->DeleteString(iItemSel);

		if (!m_fInsertingNew)
		{
			PBREAKPOINTNODE	pbpn = GetBreakPoint(iItemSel);
			pbpnMarkDel(pbpn) = TRUE;
		}

		ASSERT(plst->GetCount() == cItemsMac-1);
		plst->SetCaretIndex(cItemsMac-2);
		plst->SelItemRange(FALSE, 0, cItemsMac-2);
		plst->SetSel(cItemsMac-2);
		m_bptypeCur = BPNIL;
		return;
	}

	// Not a new or empty bp - must be modifying the current one
	BOOL bSingleSelect = (plst->GetSelCount() == 1);

	plst->SetRedraw(FALSE);
	plst->DeleteString(iItemSel);
	plst->InsertString(iItemSel, buf);
	plst->SetCaretIndex(iItemSel);
	plst->SelItemRange(FALSE, 0, cItemsMac-1);
	plst->SetSel(iItemSel);

	// repaint only the item being edited.
	plst->SetRedraw();

	if (bSingleSelect)
	{
		CRect rectItem;
		plst->GetItemRect(iItemSel, rectItem);

		// We might have just introduced the last row, we need to invalidate that area
		// as well so the focus rect is drawn
		if ( iItemSel == cItemsMac - 2)
		{
			CRect rectLast;
			plst->GetItemRect(iItemSel + 1, rectLast);
			rectItem |= rectLast;	// Get the union of the two rectangles.
		}

		plst->InvalidateRect(rectItem, FALSE);
	}
	else
	{
		// If we had multiple selection before we need to
		// repaint the items that just got deselected, so invalidate everything.
		plst->Invalidate();
	}

	// don't clear m_fInsertingNew here... this could be a subsequent edit
	// on the just inserted BP
	m_iItemChanging = iItemSel;

}

// the current breakpoint is being edited, update it's
// type and reformat its display string accordingly
void CBPDlgEx::UpdateCurrentBP(BREAKPOINTTYPES bptype)
{
	CString str;
	char buf[1024];	// big enough to hold a couple of paths + two big symbol names
	char *p = buf;

	buf[0] = '\0';

	m_bptypeCur = bptype;
	m_fCurBPDirty = TRUE;

	m_butGotoCode.EnableWindow(bptype != BPEXPRTRUE && bptype != BPEXPRCHGD);

	switch (bptype)
	{
	case BPLOC:
	case BPLOCEXPRTRUE:
	case BPLOCEXPRCHGD:
	case BPWNDPROCMSGRCVD:
		VERIFY(str.LoadString(BPFMT_LOCATION));
		p += sprintf(p, str, m_szLocation);
	default:
		break;
	}

	LONG lPass = -1;
	LONG lLength = -1;

	fScanAnyLong(m_szLength,  &lLength, 1L, (long)INT_MAX);
	fScanAnyLong(m_szPassCnt, &lPass,   0L, (long)INT_MAX); //SHRT_MAX

	char bufLen[20], bufPass[20];
	sprintf(bufLen, "%ld",  lLength);
	sprintf(bufPass, "%ld", lPass);

	switch (bptype)
	{
	case BPLOCEXPRCHGD:
	case BPEXPRCHGD:
		VERIFY(str.LoadString(BPFMT_WHENCHANGES));
		p += sprintf(p, str, m_szExpression, lLength > 0 ? bufLen : "??");
		break;

	case BPLOCEXPRTRUE:	
	case BPEXPRTRUE:
		VERIFY(str.LoadString(BPFMT_WHENTRUE));
		p += sprintf(p, str, m_szExpression);
		break;

	case BPWNDPROCMSGRCVD:
		VERIFY(str.LoadString(BPFMT_ONMESSAGE));
		p += sprintf(p, str, m_szMessage);
		break;

	default:
		break;
	}

	if (lPass != 0 && bptype != BPWNDPROCMSGRCVD)
	{
		VERIFY(str.LoadString(BPFMT_PASSCOUNT));
		p += sprintf(p, str, lPass > 0 ? bufPass : "??");
	}

	UpdateListString(buf);
}

//
BOOL CBPDlgEx::FCurrentBPIsEmpty(void)
{
	return (
		m_szLocation[0]	  == '\0' &&
		m_szExpression[0] == '\0' &&
		m_szMessage[0]    == '\0'
	);

}

// the breakpoint couldn't be commited, display a suitable error and
// move focus
void CBPDlgEx::HandleBPError(int ierr, BOOL fQuiet)
{
	CString str;

	ASSERT(mpBptItab[m_bptypeCur] == m_itabPrev);

	switch ((ERR_PARSE)ierr)
	{
		case ERR_NULL:
			return;

		case ERR_ACTION:
			ASSERT(FALSE);
			return;
				
		case ERR_LOC:
			VERIFY(str.LoadString(IDS_BP_INVALID_LOCATION));
			//SelectTab(LOCATION_TAB);
			m_pdlgLocation->m_edLocation.SetSel(0,-1);
			SetFocusToControl(&m_pdlgLocation->m_edLocation);
			break;

		case ERR_PROC:
			VERIFY(str.LoadString(IDS_BP_INVALID_WNDPROC));
			//SelectTab(MESSAGES_TAB);
			m_pdlgMessages->m_lstWndProc.SetEditSel(1,1);
			SetFocusToControl(&m_pdlgMessages->m_lstWndProc);
			break;

		case ERR_EXPR:
			VERIFY(str.LoadString(IDS_BP_INVALID_EXPRESSION));
			//SelectTab(DATA_TAB);
			if (m_itabPrev == LOCATION_TAB) {
				// FUTURE -billjoy- need to bring up the condition dlg
				//m_pdlgLocation->m_edExpr.SetSel(0,-1);
				//SetFocusToControl(&m_pdlgLocation->m_edExpr);
			} else {
				m_pdlgData->m_edExpr.SetSel(0,-1);
				SetFocusToControl(&m_pdlgData->m_edExpr);
			}
			break;

		case ERR_LEN:
			VERIFY(str.LoadString(IDS_BP_INVALID_LENGTH));
			//SelectTab(DATA_TAB);
			m_pdlgData->m_edLength.SetSel(0,-1);
			SetFocusToControl(&m_pdlgData->m_edLength);
			break;

		case ERR_MSG:
			VERIFY(str.LoadString(IDS_BP_INVALID_MESSAGE));
			//SelectTab(MESSAGES_TAB);
			m_pdlgMessages->m_lstMessage.SetEditSel(1,1);
			SetFocusToControl(&m_pdlgMessages->m_lstMessage);
			break;

		case ERR_PASSCNT:

			// this shouldn't ever happen since we check the pass
			// count when it is entered
			ASSERT (FALSE);

			//str.LoadString(IDS_BP_INVALID_HITCOUNT);
			break;

		default:
			ASSERT(FALSE);
	}

	if (!str.IsEmpty() && !fQuiet)
	{
		CString strCap;
		VERIFY(strCap.LoadString(IDS_BP_INVALID_CAPTION));
		MessageBox(str, strCap, MB_OK|MB_ICONEXCLAMATION);
	}
}

// the user wants to see the code for this breakpoint, navigate there for him...
void CBPDlgEx::OnGotoCode()
{
	if (m_bptypeCur != BPNIL && !AddReplaceBP())
		return;

	int iItemSel  = m_lstBP.GetCaretIndex();
	int cItemsMac = m_lstBP.GetCount();

	if (iItemSel == cItemsMac-1 || iItemSel == -1)
		return;

	AcceptChanges();

	PBREAKPOINTNODE	pbpn = GetBreakPoint(iItemSel);

	if (BHCanGotoBP(pbpn))
	{
		CTabbedDialog::OnOK();
		BHGotoBP(pbpn);
	}
	else
	{
		::MessageBeep(0);
	}
}

// the selected breakpoints are to be removed
void CBPDlgEx::OnRemoveBP()
{
	int cItemsMac   = m_lstBP.GetCount();
	int iItemSelNew = -1;

	for (int i=0; i < cItemsMac - 1; i++)
	{
		if (!m_lstBP.GetSel(i))
			continue;

		if (iItemSelNew == -1)
			iItemSelNew = i;

		// remove from the list first...
		m_lstBP.DeleteString(i);
	
		// check if we need to remove it from the breakpoint list
		if (i != cItemsMac-2 || !m_fInsertingNew)
		{
			// this item has been added to the list
			// mark it for removal...

			PBREAKPOINTNODE	pbpn = GetBreakPoint(i);

			// Mark this BP as deleted
			pbpnMarkDel(pbpn) = TRUE;
		}

		cItemsMac--;
		i--;
	}

	m_bptypeCur     = BPNIL;
	m_fInsertingNew = FALSE;
	m_iItemChanging = -1;

	if (iItemSelNew == -1) {
		iItemSelNew = 0;
	} else {
		// If anything besides the last (empty) bp was selected, make sure the
		// last bp is NOT selected.
		m_lstBP.SetSel(cItemsMac-1, FALSE);
	}
	
	m_lstBP.SetCaretIndex(iItemSelNew);
	m_lstBP.SetSel(iItemSelNew);
	OnChangedSel();
}

void CBPDlgEx::OnRemoveAllBP()
{
	int cItemsMac   = m_lstBP.GetCount();

	for (int i=0; i < cItemsMac - 1; i++)
	{
		// remove from the list first...
		m_lstBP.DeleteString(i);
	
		// check if we need to remove it from the breakpoint list
		if (i != cItemsMac-2 || !m_fInsertingNew)
		{
			// this item has been added to the list 
			// mark it for removal...

			PBREAKPOINTNODE	pbpn = GetBreakPoint(i);

			// Mark this BP as deleted
			pbpnMarkDel(pbpn) = TRUE;
		}

		cItemsMac--;
		i--;
	}

	m_bptypeCur     = BPNIL;
	m_fInsertingNew = FALSE;
	m_iItemChanging = -1;

	m_lstBP.SetCaretIndex(0);
	m_lstBP.SetSel(0);
	OnChangedSel();
}


// the selected breakpoints are to be disabled or enabled
void CBPDlgEx::OnToggleBP()
{
	BOOL fSomeChecked = FALSE;
	int cItemsMac = m_lstBP.GetCount();

	// Pass1: are any boxes checked?
	for (int i=0; i < cItemsMac - 1; i++)
	{
		if (!m_lstBP.GetSel(i))
			continue;

		if (m_lstBP.GetCheck(i)	== 1)
		{
			fSomeChecked = TRUE;
			break;
		}
	}

	// Pass2: enable or disable all boxes
	for (i=0; i < cItemsMac - 1; i++)
	{
		if (!m_lstBP.GetSel(i))
			continue;

		m_lstBP.SetCheck(i, fSomeChecked ? 0 : 1);
	}

	m_lstBP.Invalidate();
}

// a new tab has been selected, force it to update its
// fields to reflect the current state of the selected breakpoint
void CBPDlgEx::OnSelectTab(int nTab)
{
	CListBox *plst = &pbpDlg->m_lstBP;
	int iItemSel = plst->GetCaretIndex();
	int iLastItem = plst->GetCount() - 1;

	// May need to set the focus in the bp list to the last item
	// (we don't allow the user to change the type of an existing bp)
	if (iItemSel != iLastItem && iLastItem != -1)
	{
		// ValidateTab should fail if this bp is not valid
		ASSERT(!m_fCurBPDirty);

		PBREAKPOINTNODE pbpn = GetBreakPoint(iItemSel); ASSERT(pbpn);
		BREAKPOINTTYPES bptype = pbpnType(pbpn);
		ASSERT(bptype >= BPLOC && bptype <= BPWNDPROCMSGRCVD);
		if (nTab != mpBptItab[bptype])
		{
			plst->SelItemRange(FALSE, 0, iLastItem);
			plst->SetSel(iLastItem);
			plst->SetCaretIndex(iLastItem);
			OnChangedSel();
		}
	}

	switch (m_itabPrev)
	{
	case LOCATION_TAB:
		m_pdlgLocation->m_edLocation.GetSel(m_iedLocStart, m_iedLocEnd);
		break;
	case DATA_TAB:
		//m_pdlgData->m_edLocation.GetSel(m_iedLocStart, m_iedLocEnd);
		m_pdlgData->m_edExpr.GetSel(m_iedExpStart, m_iedExpEnd);
		break;
	case MESSAGES_TAB:
		DWORD dw   = m_pdlgMessages->m_lstWndProc.GetEditSel();
		m_iedLocStart = LOWORD(dw);
		m_iedLocEnd   = HIWORD(dw);
		break;
	}

	m_itabPrev = nTab;

	switch (nTab)
	{
	case LOCATION_TAB:	m_pdlgLocation->SetBPFields();	break;
	case DATA_TAB:		m_pdlgData->SetBPFields();		break;
	case MESSAGES_TAB:	m_pdlgMessages->SetBPFields();	break;
	//case 3:	m_pdlgAdvanced->SetBPFields();	break;
	}
}

BREAKPOINTTYPES	CBPDlgEx::GetCurrentBPType()
{
	char *szLoc  = m_szLocation;
	char *szExpr = m_szExpression;

	if (m_itabPrev == MESSAGES_TAB)
	{
		return BPWNDPROCMSGRCVD;
	}
		
	if (szExpr[0])
	{
		if (FBreakWhenExprTrue(szExpr))
		{
			if (szLoc[0])
				return BPLOCEXPRTRUE;
			else
				return BPEXPRTRUE;
		}
		else
		{
			if (szLoc[0])
				return BPLOCEXPRCHGD;
			else
				return BPEXPRCHGD;
		}
	}

	return BPLOC;
}

// if the user types in the breakpoint list, the forward the character
// to the appropriate edit field...
BOOL CBPDlgEx::PreTranslateMessage(MSG *pmsg)
{
	if (pmsg->message == WM_KEYDOWN)
	{
		switch (pmsg->wParam)
		{
		case VK_DELETE:
			if (pmsg->hwnd == m_lstBP.m_hWnd)
			{
				int cItemsMac = m_lstBP.GetCount();

				for (int i=0; i < cItemsMac -1; i++)
					if (m_lstBP.GetSel(i))
						break;

				if (i < cItemsMac - 1)
				{
					OnRemoveBP();
					return TRUE;
				}
			}
			break;

		case VK_SPACE:
			if (pmsg->hwnd == m_lstBP.m_hWnd)
			{
				OnToggleBP();
				return TRUE;
			}
			break;

		case VK_RETURN:
			if (!FCurrentBPIsEmpty())
			{
				// If we can't commit the bp, don't select something else in the bp list
				if (pbpDlg->m_bptypeCur != BPNIL && !pbpDlg->AddReplaceBP())
				{
					return TRUE;
				}

				// "Commit" the current bp
				CListBox *plst = &pbpDlg->m_lstBP;
				int iLastItem = plst->GetCount() - 1;

				plst->SelItemRange(FALSE, 0, iLastItem);
				plst->SetSel(iLastItem);
				plst->SetCaretIndex(iLastItem);

				OnChangedSel();

				return TRUE;
			}
			break;
		}
	}

	// do not intercept the space character because it is used to toggle enable/disable
	// Also, only intercept if the CTRL keys isn't down.
	if (pmsg->message == WM_CHAR && pmsg->hwnd == m_lstBP.m_hWnd && pmsg->wParam != ' ' &&
		GetKeyState(VK_CONTROL) >= 0)
	{
		int iItemSel  = m_lstBP.GetCaretIndex();
		int cItemsMac = m_lstBP.GetCount();

		if (m_bptypeCur != BPNIL)
		{
			ASSERT(m_iItemChanging != -1);
			ASSERT(iItemSel == m_iItemChanging);
			ASSERT(iItemSel < cItemsMac - 1);
		}
		else
		{
			// The only empty bp is on the last line
			ASSERT (iItemSel==cItemsMac-1);

			switch (m_itabPrev)
			{
				case LOCATION_TAB:	m_bptypeCur = BPLOC;			break;
				case DATA_TAB:		m_bptypeCur = BPEXPRCHGD;		break;
				case MESSAGES_TAB:	m_bptypeCur = BPWNDPROCMSGRCVD;	break;
				default:			ASSERT(FALSE);					break;
			}
		}

		// FUTURE DBCS enable?
		char buf[5];
		buf[0] = (char)pmsg->wParam;
		buf[1] = 0;

		ASSERT(m_bptypeCur >= BPLOC && m_bptypeCur <= BPWNDPROCMSGRCVD);

		switch (m_bptypeCur)
		{
		case BPLOC:
		case BPLOCEXPRCHGD:
		case BPLOCEXPRTRUE:
			m_pdlgLocation->m_edLocation.SetWindowText(buf);
			SetFocusToControl(&m_pdlgLocation->m_edLocation);
			m_pdlgLocation->m_edLocation.SetSel(1,1);
			break;

		case BPEXPRCHGD:
		case BPEXPRTRUE:
			m_pdlgData->m_edExpr.SetWindowText(buf);
			SetFocusToControl(&m_pdlgData->m_edExpr);
			m_pdlgData->m_edExpr.SetSel(1,1);
			break;

		case BPWNDPROCMSGRCVD:
			m_pdlgMessages->m_lstWndProc.SetWindowText(buf);
			SetFocusToControl(&m_pdlgMessages->m_lstWndProc);
			m_pdlgMessages->m_lstWndProc.SetEditSel(1,1);
			break;

		default:
			ASSERT(FALSE);
			break;
		}
		return TRUE;
	}

	return CTabbedDialog::PreTranslateMessage(pmsg);
}


//////////////////////////////////////////////////////////////////////////////

static void InitChildFont(CWnd *pwnd, int nID)
{
	CWnd *pChild = pwnd->GetDlgItem(nID);
	ASSERT(pChild);
	pChild->SetFont(GetStdFont(font_Normal));
}

//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLocationTab, CDlgTab)

BEGIN_MESSAGE_MAP(CLocationTab, CDlgTab)
	//{{AFX_MSG_MAP(CLocationTab)
	ON_EN_CHANGE(IDC_LOCATION, OnChangeLocation)
	ON_BN_CLICKED(IDC_BP_BTN_COND, OnCondition)
	ON_COMMAND(IDM_BP_CURRENT_ADDR, OnCurrentAddr)
	ON_COMMAND(IDM_BP_CURRENT_LINE, OnCurrentLine)
	ON_COMMAND(IDM_BP_CURRENT_FUNC, OnCurrentFunc)
	ON_COMMAND(IDM_BP_ADVANCED, OnAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLocationTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocationTab)
	DDX_Control(pDX, IDC_LOCATION, m_edLocation);
	//}}AFX_DATA_MAP
}

CLocationTab::CLocationTab() : CDlgTab(IDD_BPTAB_LOCATION,  IDS_BPTAB_LOCATION)
{
	//{{AFX_DATA_INIT(CLocationTab)
	//}}AFX_DATA_INIT
	m_fInitialized = FALSE;
}

BOOL CLocationTab::OnInitDialog()
{
	BOOL fUseSeparator = FALSE;
	CString strT;

	if (!CDlgTab::OnInitDialog())
		return FALSE;

	m_fInitialized = TRUE;

	//CRect rect(0, 0, 64, 22);
	VERIFY(m_butLocMenu.SubclassDlgItem(IDC_LOC_MENU, this));
	m_butLocMenu.SetPopup(MENU_CONTEXT_POPUP(EmptyMenu));
	m_butLocMenu.m_pNewMenu->SetAutoEnable(FALSE);

	InitializeLocationMenu();

	if (!m_strCurAddr.IsEmpty()) {
		m_butLocMenu.m_pNewMenu->AddItem(IDM_BP_CURRENT_ADDR, m_strCurAddr);
		fUseSeparator = TRUE;
	}
	if (!m_strCurLine.IsEmpty()) {
		char szTmp[256];
		VERIFY(strT.LoadString(IDS_BPMENU_LINE));
		_ftcscpy(szTmp, strT);
		_ftcscat(szTmp, m_strCurLine);
		m_butLocMenu.m_pNewMenu->AddItem(IDM_BP_CURRENT_LINE, szTmp);
		fUseSeparator = TRUE;
	}
	if (!m_strCurFunc.IsEmpty()) {
		m_butLocMenu.m_pNewMenu->AddItem(IDM_BP_CURRENT_FUNC, m_strCurFunc);
		fUseSeparator = TRUE;
	}
	if (fUseSeparator) {
		m_butLocMenu.m_pNewMenu->AppendMenu(MF_SEPARATOR);
	}

	VERIFY(strT.LoadString(IDS_BPMENU_ADVANCED));
	m_butLocMenu.m_pNewMenu->AddItem(IDM_BP_ADVANCED, strT);

	m_edLocation.LimitText(CB_MAX_LOCATION-1);

	SetBPFields();
	return TRUE;	
}

// initialize the breakpoint location drop down values based on the current
// editing context of the user
void CLocationTab::InitializeLocationMenu()
{
	ADDR addr = {0};
	BOOL fSetFunc = FALSE;

	// set the current location
	CView   *pCurTextView = GetCurView();

	if (pCurTextView == NULL)
		return;

	char    szTmp[256];
    DOCTYPE dt;

    if ( gpISrc->ViewBelongsToSrc(pCurTextView) )
        dt = DOC_WIN;
    else if ( pCurTextView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
        dt = ((CMultiEdit *)pCurTextView)->m_dt;
    else
        dt = MEMORY_WIN; // just to get through the switch block

	switch ( dt )
	{
		case DOC_WIN:
		{
			if (!(GetDebugPathName(pCurTextView->GetDocument()).IsEmpty()))
			{
				int iSrcLine ;

				// Set the current line
				iSrcLine = gpISrc->GetCurrentLine(pCurTextView);
				_itoa(iSrcLine+1,szTmp,10) ;
				m_strCurLine = szTmp;

				// Different format for calling into breakpts.cpp (must use  '.')
				szTmp[0] = _T('.');
				_itoa(iSrcLine+1,szTmp+1,10) ;

				// If this source line maps to multiple code locations,
				// just prime the drop down with the current line number.
				// This avoids any problems with bringing up the ambiguous
				// source dialog

				if (DebuggeeAlive ())
				{
					BREAKPOINTNODE	bpn = {BPLOC,};
					if (ParseCV400Location(szTmp, &bpn) ||
						ParseQC25Location (szTmp, &bpn))
					{
						LPSLP rgslp = NULL;
						int csl = SLCAddrFromLine ((HEXE)NULL,
												   (HMOD)NULL,
												   bpnLocCxtSource (bpn),
												   iSrcLine+1,
												   &rgslp
												   );

						if (csl > 1)
							fSetFunc = FALSE;
						else
							fSetFunc = FAddrFromSz(szTmp, &addr);

						if ( rgslp )
							SHFree( (LPV)rgslp );
					}
				}
			}
			break;
		}

		case CALLS_WIN:
		case DISASSY_WIN:
		case BREAKPT_WIN:
		{
			GCAINFO	gcai = {0};

			// Get the address for where the cursor is.  The window
			// must be sensitive to this message.  A non-zero return
			// value indicates that the addr packet has been filled in.
			// it is GROSSLY ASSUMED that it IS valid!
			if ((LRESULT)gcafAddress != pCurTextView->SendMessage(WU_GETCURSORADDR,0, (DWORD)&gcai))
				break;
			
			SYFixupAddr(&gcai.addr);

   			if (ADDR_IS_FLAT(gcai.addr))
			{
				ASSERT(ADDR_IS_OFF32(gcai.addr));
				sprintf(szTmp, "0x%08lx", (LONG)GetAddrOff(gcai.addr));
			}
			else
			{
				BOOL fOffset32 = ADDR_IS_OFF32(gcai.addr);
				sprintf(szTmp, "0x%04x:0x%0*x",	GetAddrSeg(gcai.addr), fOffset32 ? 8:4, GetAddrOff(gcai.addr));
			}

			//strcpy(m_szLocation, szTmp);
			m_strCurAddr = szTmp;
			fSetFunc = TRUE;
			addr = gcai.addr;

			// Set the line number
			LONG ln=0;
			char szFname[_MAX_PATH];
			UOFFSET Delta;
			HSF hsf;

			SYUnFixupAddr(&addr);
			if (GetSourceFrompADDR(&addr,szFname,sizeof(szFname),&ln,&Delta,&hsf))
            {
				_itoa(ln,szTmp,10) ;
				m_strCurLine = szTmp;
				m_strCurFile = szFname;
				if (Delta==0)
                {
					// If the cursor is at the beginning of a source line's
					// code, the address becomes noise.
					m_strCurAddr.Empty();
				}
			}

			break;
		}
	}

	if (fSetFunc)
	{
		UOFF32 off;
		CXT	cxt = {0};
		HSYM hsym;

		SYUnFixupAddr(&addr);

		// Get the bounding context for compare or fill in the BREAKPOINTNDOE package
		SHSetCxt( &addr, &cxt );

		// "Function"
		off = SHGetNearestHsym(&addr,	SHHMODFrompCXT( &cxt ),	EECODE,	&hsym);
		ASSERT (sizeof(szTmp) >= FUNC_NAME_LEN+1);
		FnNameFromHsym( hsym, &cxt, szTmp );
		m_strCurFunc = szTmp;

		if (off == 0)
		{
			// The cursor is on the opening curly of a func, so the function
			// name is the only interesting location for the menu.
			m_strCurAddr.Empty();
			m_strCurLine.Empty();
		}
	}
}

void CLocationTab::SetBPFields()
{
	if (!m_fInitialized) return;

	pbpDlg->m_fSelfChange++;

	m_edLocation.SetWindowText(pbpDlg->m_szLocation);
	m_edLocation.SetSel(pbpDlg->m_iedLocStart, pbpDlg->m_iedLocEnd);

	pbpDlg->m_fSelfChange--;
}


void CLocationTab::OnChangeLocation()
{
	BREAKPOINTTYPES bptype;
	CString str;
	m_edLocation.GetWindowText(str);

	EnableButton(GetDlgItem(IDC_BP_BTN_COND),!str.IsEmpty());

	if (pbpDlg->m_fSelfChange)	return;

	_tcscpy(pbpDlg->m_szLocation, str);

	if (pbpDlg->m_szLocation[0] == '\0') {
		// Who cares whether there's an expression - this is a location bp
		bptype = BPNIL;
		pbpDlg->m_szExpression[0] = '\0';
	} else	{
		bptype = pbpDlg->GetCurrentBPType();
	}
	pbpDlg->UpdateCurrentBP(bptype);
}

void CLocationTab::OnCondition()
{
	CBPCondDlg BPCondDlg(this);

	BPCondDlg.m_strExpr = pbpDlg->m_szExpression;
	BPCondDlg.m_strLength = pbpDlg->m_szLength;
	BPCondDlg.m_strSkip = pbpDlg->m_szPassCnt;

	BPCondDlg.DoModal();

	// Did anything change?
	if (_ftcscmp(pbpDlg->m_szExpression, BPCondDlg.m_strExpr)
		|| _ftcscmp(pbpDlg->m_szLength, BPCondDlg.m_strLength)
		|| _ftcscmp(pbpDlg->m_szPassCnt, BPCondDlg.m_strSkip))
	{
		_ftcscpy(pbpDlg->m_szExpression, BPCondDlg.m_strExpr);
		_ftcscpy(pbpDlg->m_szLength, BPCondDlg.m_strLength);
		_ftcscpy(pbpDlg->m_szPassCnt, BPCondDlg.m_strSkip);

		if (FBreakWhenExprTrue(pbpDlg->m_szExpression)) {
			pbpDlg->m_fExpTrue = TRUE;
			pbpDlg->m_szLength[0] = '\0';
		} else	{
			pbpDlg->m_fExpTrue = FALSE;
			pbpDlg->m_szPassCnt[0] = '\0';
		}

		BREAKPOINTTYPES bptype = pbpDlg->GetCurrentBPType();
		pbpDlg->UpdateCurrentBP(bptype);
	}

	// OK button should be def button	
	pbpDlg->SetDefButtonIndex(0);

	m_edLocation.SetFocus();
}

void CLocationTab::OnCurrentAddr()
{
	ASSERT(!m_strCurAddr.IsEmpty());

	m_edLocation.SetWindowText(m_strCurAddr);
	m_edLocation.SetFocus();
	m_edLocation.SetSel(0,-1);
}

void CLocationTab::OnCurrentLine()
{
	ASSERT(!m_strCurLine.IsEmpty());

	char szTmp[256];
	szTmp[0] = '\0';

	// If disasm wnd is active, we'll need to use context to specify the source file
	if (!m_strCurFile.IsEmpty()) {
		sprintf(szTmp, "{,%s,} ", m_strCurFile);
	}
	_ftcscat(szTmp, ".");
	_ftcscat(szTmp, m_strCurLine);
	m_edLocation.SetWindowText(szTmp);
	m_edLocation.SetFocus();
	m_edLocation.SetSel(0,-1);
}

void CLocationTab::OnCurrentFunc()
{
	ASSERT(!m_strCurFunc.IsEmpty());

	m_edLocation.SetWindowText(m_strCurFunc);
	m_edLocation.SetFocus();
	m_edLocation.SetSel(0,-1);
}

void CLocationTab::OnAdvanced()
{
	CBPContextDlg BPContextDlg(this);
	BREAKPOINTNODE bpn;
	CString str;
	//int iItemSel = pbpDlg->m_lstBP.GetCaretIndex();
	char szBuf[CB_TMP_BUF_SIZE];

	VERIFY(BPContextDlg.m_strlblWhere.LoadString(IDS_BPCONTEXT_LOCATION));

	m_edLocation.GetWindowText(szBuf, cbBpCmdMax);
	char *pLoc = (char *)FindNonQuotedChar(szBuf, CLOSECXT);
	if (pLoc)
	{
		// The user has used the context operator
		BPContextDlg.m_strWhere = _ftcsinc(pLoc);
		if (ParseCV400Location(szBuf, &bpn) || ParseQC25Location (szBuf, &bpn))
		{
			BPContextDlg.m_strFunc = bpnLocCxtFunction( bpn );
			BPContextDlg.m_strSource = bpnLocCxtSource( bpn );
			BPContextDlg.m_strModule = bpnLocCxtExe( bpn );
		}
	}
	else
	{
		// No context operator specified - use what we know
		BPContextDlg.m_strWhere = szBuf;
#if	0	// The pbpn has no context, and FormatDisplayBP doesn't
		// quite format it correctly.
		PBREAKPOINTNODE	pbpn = pbpDlg->GetBreakPoint(iItemSel);
		if (pbpn)
		{
			FormatDisplayBP (pbpn, szBuf, sizeof(szBuf));
			if (ParseCV400Location(szBuf, &bpn) || ParseQC25Location (szBuf, &bpn))
			{
				BPContextDlg.m_strFunc = bpnLocCxtFunction( bpn );
				BPContextDlg.m_strSource = bpnLocCxtSource( bpn );
				BPContextDlg.m_strModule = bpnLocCxtExe( bpn );
			}
		}
#endif
	}

	BPContextDlg.m_strWhere.TrimLeft();

	if (BPContextDlg.DoModal() == IDCANCEL) {
		// no op
		return;
	}

	szBuf[0] = '\0';

	if (!BPContextDlg.m_strFunc.IsEmpty() ||
		!BPContextDlg.m_strSource.IsEmpty() ||
		!BPContextDlg.m_strModule.IsEmpty() )
	{
		sprintf(szBuf, "{%s,%s,%s} ",
			(LPCTSTR)BPContextDlg.m_strFunc,
			(LPCTSTR)BPContextDlg.m_strSource,
			(LPCTSTR)BPContextDlg.m_strModule
		);
	}

	// Make sure we don't overflow the max location size
	if (_ftcslen(szBuf) + BPContextDlg.m_strWhere.GetLength() < CB_TMP_BUF_SIZE)
	{
		_ftcscat(szBuf, BPContextDlg.m_strWhere);
	}

	// If the bp command is bigger than the max bp size allowed truncate it.
	if (_ftcslen(szBuf) >= cbBpCmdMax)
	{
		szBuf[cbBpCmdMax - 1] = '\0';
	}

	m_edLocation.SetWindowText(szBuf);
}

BOOL CLocationTab::ValidateTab()
{
	return (pbpDlg->AddReplaceBP());
}

BOOL CLocationTab::PreTranslateMessage(MSG *pmsg)
{
	if (pmsg->message == WM_KEYDOWN)
	{
		switch (pmsg->wParam)
		{
		case VK_RETURN:
			if (pmsg->hwnd == m_edLocation.m_hWnd && !pbpDlg->FCurrentBPIsEmpty())
			{
				// If we can't commit the bp, don't select something else in the bp list
				if (pbpDlg->m_bptypeCur != BPNIL && !pbpDlg->AddReplaceBP())
				{
					return TRUE;
				}

				// "Commit" the current bp
				CListBox *plst = &pbpDlg->m_lstBP;
				int iLastItem = plst->GetCount() - 1;

				plst->SelItemRange(FALSE, 0, iLastItem);
				plst->SetSel(iLastItem);
				plst->SetCaretIndex(iLastItem);

				pbpDlg->OnChangedSel();

				m_edLocation.SetFocus();
				return TRUE;
			}
			break;
		}
	}

	return CDlgTab::PreTranslateMessage(pmsg);
}

//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDataTabEx, CDlgTab)

BEGIN_MESSAGE_MAP(CDataTabEx, CDlgTab)
	//{{AFX_MSG_MAP(CDataTabEx)
	ON_COMMAND(IDM_BP_ADVANCED, OnAdvanced)
	ON_EN_CHANGE(IDC_BP_EXPRESSION, OnChangeExpression)
	ON_EN_CHANGE(IDC_BP_NUM_ELEMENTS, OnChangeLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDataTabEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataTabEx)
	DDX_Control(pDX, IDC_BP_EXPRESSION, m_edExpr);
	DDX_Control(pDX, IDC_BP_NUM_ELEMENTS, m_edLength);
	//}}AFX_DATA_MAP
}

CDataTabEx::CDataTabEx() : CDlgTab(IDD_BPTAB_DATA1, IDS_BPTAB_DATA1)
{
	//{{AFX_DATA_INIT(CDataTabEx)
	//}}AFX_DATA_INIT
	m_fInitialized = FALSE;
}

BOOL CDataTabEx::OnInitDialog()
{
	if (!CDlgTab::OnInitDialog())
		return FALSE;

	VERIFY(m_butExprMenu.SubclassDlgItem(IDC_EXPR_MENU, this));
	m_butExprMenu.SetPopup(MENU_CONTEXT_POPUP(EmptyMenu));
	m_butExprMenu.m_pNewMenu->SetAutoEnable(FALSE);
	// Someday we may use the parser to create some entries in the menu, but
	// for now there's just the advanced item.
	CString str;
	VERIFY(str.LoadString(IDS_BPMENU_ADVANCED));
	m_butExprMenu.m_pNewMenu->AddItem(IDM_BP_ADVANCED, str);

	m_fInitialized = TRUE;

	m_edExpr.LimitText(CB_MAX_EXPRESSION-1);
	m_edLength.LimitText(CB_MAX_LENGTH-1);

	SetBPFields();
	return TRUE;
}

void CDataTabEx::SetBPFields()
{
	if (!m_fInitialized) return;

	pbpDlg->m_fSelfChange++;

	m_edExpr.SetWindowText(pbpDlg->m_szExpression);
	m_edExpr.SetSel(pbpDlg->m_iedExpStart, pbpDlg->m_iedExpEnd);
	m_edLength.SetWindowText(pbpDlg->m_szLength);
	m_edLength.SetSel(0,-1);

    ((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_TRUE))->ShowWindow(pbpDlg->m_fExpTrue);
	((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_CHANGES))->ShowWindow(!pbpDlg->m_fExpTrue);

	pbpDlg->m_fSelfChange--;
}

void CDataTabEx::OnChangeLength()
{
	if (pbpDlg->m_fSelfChange)	return;

	CString str;
	m_edLength.GetWindowText(str);
	_tcscpy(pbpDlg->m_szLength, str);

	BREAKPOINTTYPES bptype = pbpDlg->m_bptypeCur;

	if (bptype != BPNIL)
		pbpDlg->UpdateCurrentBP(bptype);
}

/***
* _mbsistr - Search for one MBCS string inside another (case insensitive)
* a modified version of _mbsstr
*
*Purpose:
*       Find the first case insensitive occurrence of str2 in str1.
*
*Entry:
*       char *str1 = beginning of string
*       char *str2 = string to search for
*
*Exit:
*       Returns a pointer to the first occurrence of str2 in
*       str1, or NULL if str2 does not occur in str1
*
*Exceptions:
*
*******************************************************************************/

static const char * __cdecl _mbsistr(
    const char *str1,
    const char *str2
    )
{
        const char *cp, *s1, *s2, *endp;

        cp = str1;
        endp = (str1 + (_tcslen(str1) - _tcslen(str2)));

        while (*cp && (cp <= endp))
        {
                s1 = cp;
                s2 = str2;

                /*
                 * MBCS: ok to ++ since doing equality comparison.
                 * [This depends on MBCS strings being "legal".]
                 */

                while ( *s1 && *s2 && (_totlower(*s1) == _totlower(*s2)) )
                        s1++, s2++;

                if (!(*s2))
                        return(cp);     /* success! */

                /*
                 * bump pointer to next char
                 */

                cp = _tcsinc(cp);

        }

        return(NULL);

}

// Helper function returns
//   o TRUE if the bp is a "break when expression is true" bp
//   o FALSE if the bp is a "break when expression changes" bp

BOOL FBreakWhenExprTrue(LPCSTR szExpr)
{
	const char *rgsz[] = {"==", "<", ">", "!=", "&&", "||",
		".EQ.", ".LT.", ".GT.", ".LE.", ".GE.", ".NE.", "/=",
		".AND.", ".OR.", ".XOR.", ".NOT.", ".EQV.", ".NEQV.",
		0};
	
	// Special case so the condition dialog works properly
	if (szExpr[0] == '\0')
		return TRUE;

	for (int i=0; rgsz[i]; i++)
	{
		// We have to special case > because we don't want to
		// include expressions with a -> as boolean expressions.
		if ( _tcscmp(rgsz[i],">") == 0 )
		{
			BOOL fFound = FALSE;
			LPCSTR lpCur = szExpr;
			while (!fFound && ((lpCur = _mbsistr(lpCur, rgsz[i])) != NULL))
			{
				// If the first character is > it is probably
				// an ill-formed expression but we will let is slip.
				// we definetely don't want to do a _tcsdec though.
				if (lpCur == szExpr)
					fFound = TRUE;
				else {
					LPCSTR lpPrev = _tcsdec(szExpr, lpCur);
					if (*lpPrev != '-')
						fFound = TRUE;
					else
						lpCur = _tcsinc(lpCur);
				}
			}
			if ( fFound ) break;
		}
		else {
			if (_mbsistr(szExpr, rgsz[i]))
				break;
		}
	}

	if (rgsz[i] || szExpr[0] == '!')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CDataTabEx::OnChangeExpression()
{
	if (pbpDlg->m_fSelfChange)	return;

	CString str;
	m_edExpr.GetWindowText(str);

	pbpDlg->m_fExpTrue = FBreakWhenExprTrue(str);

	pbpDlg->m_fSelfChange++;

    ((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_TRUE))->ShowWindow(pbpDlg->m_fExpTrue);
	((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_CHANGES))->ShowWindow(!pbpDlg->m_fExpTrue);

	((CWnd *)GetDlgItem(IDC_BP_TXT_NUM_ELEMENTS))->EnableWindow(!pbpDlg->m_fExpTrue);
	((CWnd *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->EnableWindow(!pbpDlg->m_fExpTrue);

	pbpDlg->m_fSelfChange--;

	// update the listbox to show what we're typing as we type it

	m_edExpr.GetWindowText(str);
	_tcscpy(pbpDlg->m_szExpression, str);

	// There shouldn't be a location if the Data Tab is selected
	ASSERT(pbpDlg->m_szLocation[0] == '\0');

	BREAKPOINTTYPES bptype = pbpDlg->GetCurrentBPType();
	pbpDlg->UpdateCurrentBP(bptype);
}

void CDataTabEx::OnAdvanced()
{
	CBPContextDlg BPContextDlg(this);
	BREAKPOINTNODE bpn;
	CString str;
	//int iItemSel = pbpDlg->m_lstBP.GetCaretIndex();
	char szBuf[CB_TMP_BUF_SIZE];

	VERIFY(BPContextDlg.m_strlblWhere.LoadString(IDS_BPCONTEXT_EXPRESSION));

	m_edExpr.GetWindowText(szBuf, cbBpCmdMax);
	char *pLoc = (char *)FindNonQuotedChar(szBuf, CLOSECXT);
	if (pLoc)
	{
		// The user has used the context operator
		BPContextDlg.m_strWhere = _ftcsinc(pLoc);
		// FUTURE -billjoy- if this works, use it for the Location page also
		if (ExtractCV400Context(szBuf, &bpnExprCxt(bpn)))
		{
			BPContextDlg.m_strFunc = bpnExprCxtFunction( bpn );
			BPContextDlg.m_strSource = bpnExprCxtSource( bpn );
			BPContextDlg.m_strModule = bpnExprCxtExe( bpn );
		}
	}
	else
	{
		// No context operator specified - use what we know
		BPContextDlg.m_strWhere = szBuf;
#if	0	// The pbpn has no context, and FormatDisplayBP doesn't
		// quite format it correctly.
		PBREAKPOINTNODE	pbpn = pbpDlg->GetBreakPoint(iItemSel);
		if (pbpn)
		{
			FormatDisplayBP (pbpn, szBuf, sizeof(szBuf));
			if (ParseExpression(szBuf, &bpn))
			{
				BPContextDlg.m_strFunc = bpnLocCxtFunction( bpn );
				BPContextDlg.m_strSource = bpnLocCxtSource( bpn );
				BPContextDlg.m_strModule = bpnLocCxtExe( bpn );
			}
		}
#endif
	}

	BPContextDlg.m_strWhere.TrimLeft();

	if (BPContextDlg.DoModal() == IDCANCEL) {
		// no op
		return;
	}

	szBuf[0] = '\0';

	if (!BPContextDlg.m_strFunc.IsEmpty() ||
		!BPContextDlg.m_strSource.IsEmpty() ||
		!BPContextDlg.m_strModule.IsEmpty() )
	{
		sprintf(szBuf, "{%s,%s,%s} ",
			(LPCTSTR)BPContextDlg.m_strFunc,
			(LPCTSTR)BPContextDlg.m_strSource,
			(LPCTSTR)BPContextDlg.m_strModule
		);
	}

	// Make sure we don't overflow the max location size
	if (_ftcslen(szBuf) + BPContextDlg.m_strWhere.GetLength() < CB_TMP_BUF_SIZE)
	{
		_ftcscat(szBuf, BPContextDlg.m_strWhere);
	}
		
	// If the bp command is bigger than the max bp size allowed truncate it.
	if (_ftcslen(szBuf) >= cbBpCmdMax)
	{
		szBuf[cbBpCmdMax - 1] = '\0';
	}

	m_edExpr.SetWindowText(szBuf);
}

BOOL CDataTabEx::ValidateTab()
{
	return (pbpDlg->AddReplaceBP());
}

BOOL CDataTabEx::PreTranslateMessage(MSG *pmsg)
{
	if (pmsg->message == WM_KEYDOWN)
	{
		switch (pmsg->wParam)
		{
		case VK_RETURN:
			// The one control that will not cause ENTER to commit the bp is
			// the drop down menu.
			if (pmsg->hwnd != ((CButton *)GetDlgItem(IDC_EXPR_MENU))->m_hWnd)
			{
				if (!pbpDlg->FCurrentBPIsEmpty())
				{
					// If we can't commit the bp, don't select something else in the bp list
					if (pbpDlg->m_bptypeCur != BPNIL && !pbpDlg->AddReplaceBP())
					{
						return TRUE;
					}

					// "Commit" the current bp
					CListBox *plst = &pbpDlg->m_lstBP;
					int iLastItem = plst->GetCount() - 1;

					plst->SelItemRange(FALSE, 0, iLastItem);
					plst->SetSel(iLastItem);
					plst->SetCaretIndex(iLastItem);

					pbpDlg->OnChangedSel();

					m_edExpr.SetFocus();
					return TRUE;
				}
			}
			break;
		}
	}

	return CDlgTab::PreTranslateMessage(pmsg);
}

//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMessagesTabEx, CDlgTab)

BEGIN_MESSAGE_MAP(CMessagesTabEx, CDlgTab)
	//{{AFX_MSG_MAP(CMessagesTabEx)
	ON_CBN_DROPDOWN(IDC_MESSAGE, OnDropdownMessage)
	ON_CBN_DROPDOWN(IDC_WNDPROC, OnDropdownWndproc)
	ON_CBN_EDITCHANGE(IDC_WNDPROC, OnChangeWndProc)
	ON_CBN_EDITCHANGE(IDC_MESSAGE, OnChangeMessage)
	ON_CBN_SELCHANGE(IDC_MESSAGE, OnSelchangeMessage)
	ON_CBN_SELCHANGE(IDC_WNDPROC, OnSelchangeWndproc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMessagesTabEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessagesTabEx)
	DDX_Control(pDX, IDC_WNDPROC, m_lstWndProc);
	DDX_Control(pDX, IDC_MESSAGE, m_lstMessage);
	//}}AFX_DATA_MAP
}

CMessagesTabEx::CMessagesTabEx() : CDlgTab(IDD_BPTAB_MESSAGES, IDS_BPTAB_MESSAGES)
{
	//{{AFX_DATA_INIT(CMessagesTabEx)
	//}}AFX_DATA_INIT
	
	m_fInitialized = FALSE;
}

BOOL CMessagesTabEx::OnInitDialog()
{
	if (!CDlgTab::OnInitDialog())
		return FALSE;

	InitChildFont(this, IDC_BPTAB_MSG1);

	m_fInitialized = TRUE;

	m_lstMessage.LimitText(CB_MAX_MESSAGE-1);
	m_lstWndProc.LimitText(CB_MAX_LOCATION-1);
	m_lstMessage.SetExtendedUI(TRUE);
	m_lstWndProc.SetExtendedUI(TRUE);

	SetBPFields();	
	return TRUE;
}

void CMessagesTabEx::SetBPFields()
{
	if (!m_fInitialized) return;

	pbpDlg->m_fSelfChange++;

	m_lstWndProc.SetWindowText(pbpDlg->m_szLocation);
	m_lstWndProc.SetEditSel(pbpDlg->m_iedLocStart, pbpDlg->m_iedLocEnd);
	m_lstMessage.SetWindowText(pbpDlg->m_szMessage);
	m_lstMessage.SetEditSel(0,-1);

	pbpDlg->m_fSelfChange--;
}

// the button has been clicked, populate the list with the various available
// messages from our resource table
void CMessagesTabEx::OnDropdownMessage()
{
	if (m_lstMessage.GetCount())
		return;

	for (int i = DBG_Msgs_Start; ; i++)
	{
		char szTmp[80];
		LoadString(hInst, i, szTmp, sizeof(szTmp));
		if (!szTmp[0]) break;
		m_lstMessage.AddString(szTmp);
	}
}

// the user has dropped down the list of window procs, enumerate
// them all and populate the list with them...
void CMessagesTabEx::OnDropdownWndproc()
{
	if (m_lstWndProc.GetCount() || !DebuggeeAlive())
		return;

	extern CXF cxfIp;
    CXT cxt = cxfIp.cxt;

	// get the handle of the exe
	HEXE hexe = (HEXE)NULL;
	if (cxt.hMod)
		hexe = SHHexeFromHmod(cxt.hMod);

    WORD mask  = HSYMR_module | HSYMR_global | HSYMR_exe;
	HMEM hsyml = 0;

    while (EEGetHSYMList(&hsyml, &cxt, mask, NULL, FALSE) == EENOERROR)
    {
        // display the syms
        PHSL_HEAD lphsymhead = (PHSL_HEAD)BMLock(hsyml);
        PHSL_LIST lphsyml = (PHSL_LIST)(lphsymhead + 1);

        for (WORD i = 0; i != lphsymhead->blockcnt; i++)
        {
            for (WORD j = 0; j != lphsyml->symbolcnt; j++)
            {
				char szNameBuf[257];
				if (!SHGetSymName(lphsyml->hSym[j], (char far *)szNameBuf))
					continue;

				// get the type info for this item, make sure it is a procedure
				CV_typ_t tiProc = T_NOTYPE;
				SYMPTR pSym = (SYMPTR)MHOmfLock(lphsyml->hSym[j]);
				switch (pSym->rectyp)
				{
					case S_LPROC32:
					case S_GPROC32:
						tiProc = ((PROCPTR32)pSym)->typind;
						break;

					case S_GPROCMIPS:
					case S_LPROCMIPS:
						tiProc = ((PROCPTRMIPS)pSym)->typind;
						break;
				}
				MHOmfUnLock (lphsyml->hSym[j]);

				// make sure we have a procudure
				if (tiProc == T_NOTYPE)
					continue;

				// now try to fetch the type info
				HTYPE hType = THGetTypeFromIndex(cxt.hMod, tiProc);
				TYPPTR pTyp = (TYPPTR)MHOmfLock(hType);
				if (!pTyp)
					continue;

				// map the generic type info into the procedure type record
				lfProc leafProc = *(plfProc)((LPSTR )pTyp + sizeof(pTyp->len));
				MHOmfUnLock (hType);

				// check calling convention for the three wndproc possibles
				if (leafProc.calltype != CV_CALL_NEAR_STD && leafProc.calltype != CV_CALL_MIPSCALL && leafProc.calltype != CV_CALL_ALPHACALL)
					continue;

				// check for 4 parmeters
				if (leafProc.parmcount != 4)
					continue;

				// check return type of function
				if (leafProc.rvtype != T_LONG &&  leafProc.rvtype != T_ULONG &&
					leafProc.rvtype != T_INT4 &&  leafProc.rvtype != T_UINT4)
					continue;

				// Test the types of the four parameters
				hType = THGetTypeFromIndex (cxt.hMod, leafProc.arglist);
				pTyp = (TYPPTR)MHOmfLock (hType);
				if (pTyp == 0)
					continue;

				// fetch the types of all the arguments
				ASSERT (pTyp->leaf == LF_ARGLIST);
				plfArgList pleafArgList = (plfArgList)((LPSTR)pTyp + sizeof(pTyp->len));
				ASSERT (pleafArgList->count == 4);
				CV_typ_t tihWnd    = pleafArgList->arg[0];
				CV_typ_t timessage = pleafArgList->arg[1];
				CV_typ_t tiwParam  = pleafArgList->arg[2];
				CV_typ_t tilParam  = pleafArgList->arg[3];
				MHOmfUnLock ( hType );

				// check for a reasonable HWND parameter
				if (!CV_IS_PRIMITIVE(tihWnd))
				{
					// If we compile with #define STRICT
					// HWND is defined as struct HWND__ { int unused ; } NEAR *
					// Just check for a pointer for performance reason...
					// We could also look for a LF_MODIFIER
					// LF_STRUCTURE, LF_FIELDLIST etc...

					// try to fetch the type info for this arg
					HTYPE  hTyp = THGetTypeFromIndex(cxt.hMod, tihWnd);
					TYPPTR pTyp = (TYPPTR)MHOmfLock(hTyp) ;
					if (!pTyp)
						continue;

					unsigned short leaf = pTyp->leaf;
					MHOmfUnLock(hTyp);
					if (leaf != LF_POINTER)
						 continue ;
				}
				else
				{
					if (tihWnd != T_32PVOID)
						continue;
				}

				// message
				if (timessage != T_UINT4)
					continue;

				// wParam
				if (tiwParam != T_UINT4 &&  tilParam != T_ULONG)
					continue;

				// lParam (allow signed/unsiged here)
				if (tilParam != T_LONG &&  tilParam != T_ULONG &&
					tilParam != T_INT4 &&  tilParam != T_UINT4)
					continue;

				// Here we've got what we were looking for so make
				// the WndProc specifier and add it to the list

				BREAKPOINTNODE bpWP;
				bpnType(bpWP) 	 = BPWNDPROCMSGRCVD;
				bpnLocType(bpWP) = BPLOCADDREXPR;

				// Get WndProc name
				_ftcscpy (bpnWndProc(bpWP), szNameBuf);

				// Set up WndProc context
				*bpnLocCxtFunction(bpWP) = 0;
				*bpnLocCxtSource(bpWP)   = 0;
				*bpnLocCxtExe(bpWP)      = 0;

				// Get exe name if we can
				if (hexe != (HEXE)NULL)
				{
					_ftcsncpy(bpnLocCxtExe(bpWP), SHGetExeName(hexe), sizeof(bpnLocCxtExe(bpWP))- 1);
					bpnLocCxtExe(bpWP)[sizeof(bpnLocCxtExe(bpWP))-1]='\0';
				}

				char szWP[255];
				BuildCV400Location(&bpWP, szWP, sizeof(szWP), FALSE, TRUE, FALSE, FALSE);
				m_lstWndProc.AddString(szWP);
            }
            lphsyml = (PHSL_LIST) &(lphsyml->hSym[j]);
        }
        BMUnlock (hsyml);

		// if the whole search completed we're done, otherwise pick up at
		// the point the EE ran out of memory...
		if (lphsymhead->status.endsearch)
			break;
    }

    if (hsyml) EEFreeHSYMList (&hsyml);
	m_lstWndProc.SetCurSel(0);
}

// the window proc name is changing, update the breakpoint
void CMessagesTabEx::OnChangeWndProc()
{
	if (pbpDlg->m_fSelfChange)	return;

	CString str;
	m_lstWndProc.GetWindowText(str);
	_tcscpy(pbpDlg->m_szLocation, str);

	pbpDlg->UpdateCurrentBP(BPWNDPROCMSGRCVD);
}

// the message name is changing, update the breakpoint
void CMessagesTabEx::OnChangeMessage()
{
	if (pbpDlg->m_fSelfChange)	return;

	CString str;
	m_lstMessage.GetWindowText(str);
	_tcscpy(pbpDlg->m_szMessage, str);

	pbpDlg->UpdateCurrentBP(BPWNDPROCMSGRCVD);
}

// a new message has been selected, update the list of breakpoints
void CMessagesTabEx::OnSelchangeMessage()
{
	int iItemSel = m_lstMessage.GetCurSel();
	if (iItemSel == -1)
		return;

	CString str;
	m_lstMessage.GetLBText(iItemSel, str);
	_tcscpy(pbpDlg->m_szMessage, str);

	pbpDlg->UpdateCurrentBP(BPWNDPROCMSGRCVD);
}

// a new wndproc has been selected, update the list of breakpoints
void CMessagesTabEx::OnSelchangeWndproc()
{
	int iItemSel = m_lstWndProc.GetCurSel();
	if (iItemSel == -1)
		return;

	CString str;
	m_lstWndProc.GetLBText(iItemSel, str);
	_tcscpy(pbpDlg->m_szLocation, str);

	pbpDlg->UpdateCurrentBP(BPWNDPROCMSGRCVD);
}

BOOL CMessagesTabEx::ValidateTab()
{
	return (pbpDlg->AddReplaceBP());
}

BOOL CMessagesTabEx::PreTranslateMessage(MSG *pmsg)
{
	if (pmsg->message == WM_KEYDOWN)
	{
		switch (pmsg->wParam)
		{
			case VK_RETURN:
			{
				BOOL fHandle = TRUE;

				// Are we "committing" the current bp?
				if (fHandle	&& !pbpDlg->FCurrentBPIsEmpty())
				{
					// If we can't commit the bp, don't select something else in the bp list
					if (pbpDlg->m_bptypeCur != BPNIL && !pbpDlg->AddReplaceBP())
					{
						return TRUE;
					}

					// "Commit" the current bp
					CListBox *plst = &pbpDlg->m_lstBP;
					int iLastItem = plst->GetCount() - 1;

					plst->SelItemRange(FALSE, 0, iLastItem);
					plst->SetSel(iLastItem);
					plst->SetCaretIndex(iLastItem);

					pbpDlg->OnChangedSel();

					m_lstWndProc.SetFocus();
					return TRUE;
				}
				break;
			}
		}
	}
	return CDlgTab::PreTranslateMessage(pmsg);
}

//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBPListEx, CCheckList)
	//{{AFX_MSG_MAP(CBPListEx)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBPListEx::SetCheck(int nItem, int nCheck)
{
	// we don't support changes to indeterminate state...
	if (nCheck == 2 || nCheck == GetCheck(nItem) || GetCheck(nItem) == 2)
		return;

	// we're now toggling the state...
	int cItemsMac = GetCount();
	BOOL fAddedBP = FALSE;

	ASSERT(nItem < cItemsMac - 1);	// the magic item is already ruled out because its state is '2'

	if (pbpDlg->m_fInsertingNew && cItemsMac - 2 == nItem)
	{
		// the breakpoint is being added and not yet committed, must commit now
		if (!pbpDlg->AddReplaceBP())
			return;
		fAddedBP = TRUE;
	} 
	
	PBREAKPOINTNODE	pbpn = pbpDlg->GetBreakPoint(nItem);
	
	if (!fAddedBP && (nCheck == 1) && pbpnBPVirtual(pbpn)){
		//may not have commited change from a revised bp - do so now
		if (!pbpDlg->AddReplaceBP())
			return;
		pbpn = pbpDlg->GetBreakPoint(nItem);
	}
	

	if (!BHFTypeEnabled(pbpnType(pbpn)))
	{
		::MessageBeep(0);	// emit some kind of error?
		return;
	}

	// If we're debugging and the node is virtual (and disabled), create a CV
	// node and mark it in the disabled state
	if (DebuggeeAlive() && !pbpnEnabled(pbpn) && pbpnBPVirtual(pbpn))
	{
		int	iErr;

		if (BHCreateBPHandle(pbpn, FALSE, FALSE, &iErr))
		{
			if (!pbpnBPVirtual(pbpn))
				BPDisableFar(pbpnBPhbpi(pbpn));
		}
		else
		{
			::MessageBeep(0);	// emit some kind of error?
			return;
		}
	}

	// mark to enable or disable as required...
	if (pbpnEnabled(pbpn))
		pbpnMarkDisable(pbpn) = !pbpnMarkDisable(pbpn);
	else
		pbpnMarkEnable(pbpn) = !pbpnMarkEnable(pbpn);

	// reflect the check state in the bp list so CAFE can see it
	SetItemData(nItem, nCheck);
}

int CBPListEx::GetCheck(int nItem)
{
	int nCheck = 0;
	int cItemsMac = GetCount();

	if (cItemsMac - 1 == nItem)
	{
		// the magic insertion thing is indeterminate
		nCheck = 2;
	}
	else if (pbpDlg->m_fInsertingNew && cItemsMac - 2 == nItem)
	{
		// a breakpoint that is being edited but not commited yet is enabled
		nCheck = 1;
	}
	else
	{
		PBREAKPOINTNODE pbpn = pbpDlg->GetBreakPoint((UINT)nItem);

		if (!BHFTypeEnabled(pbpnType(pbpn)))
			nCheck = 2;	// indeterminate
		else
			nCheck = pbpnEnabled(pbpn) ? !pbpnMarkDisable(pbpn) : pbpnMarkEnable(pbpn);
	}

	// update item data to reflect check state
	// must do this now to give accurate info to CAFE

	SetItemData(nItem, nCheck);
	return nCheck;
}


void CBPListEx::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	int cItemsMac = GetCount();

	if (lpdis->itemID == (UINT)cItemsMac-1)
	{
		CDC dc;
		dc.Attach(lpdis->hDC);
		RECT rct = lpdis->rcItem;

		COLORREF clr;
		if (lpdis->itemState & ODS_SELECTED)
			clr = dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		else
			clr = dc.SetBkColor(GetSysColor(COLOR_WINDOW));

		dc.ExtTextOut(rct.left, rct.top, ETO_CLIPPED|ETO_OPAQUE, &rct, "", 0, NULL);
		dc.SetBkColor(clr);
		
		rct.top++;
		rct.bottom--;
		rct.left += 15;
		rct.right = rct.left + (rct.right - rct.left)/2 ;
		dc.DrawFocusRect(&rct);

		if (lpdis->itemState & ODS_SELECTED	&&
			CWnd::GetFocus() &&
			m_hWnd == (CWnd::GetFocus())->m_hWnd)
		{
			rct = lpdis->rcItem;
			dc.DrawFocusRect(&rct);
		}

		dc.Detach();
		return;
	}

	CCheckList::DrawItem(lpdis);
}

void CBPListEx::OnLButtonDown(UINT nFlags, CPoint pt)
{
	int cItemsMac = GetCount();
	int iClick = GetTopIndex() + pt.y / m_cyItem;

	if (iClick == cItemsMac-1)	// last item has no checkbox
		CListBox::OnLButtonDown(nFlags, pt);
	else
		CCheckList::OnLButtonDown(nFlags, pt);
}



///////////////////////////////////////////////////////////////////////////////
// CBPCondDlg implementation
///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBPCondDlg, C3dDialog)
	//{{AFX_MSG_MAP(CLocationTab)
	ON_EN_CHANGE(ID_BPCOND_EXPR, OnChangeExpression)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CBPCondDlg::CBPCondDlg(CWnd * pParent /* = NULL */)
	: C3dDialog(CBPCondDlg::IDD, pParent)
{
	// Don't forget to set the parent to the bp dlg!
	ASSERT(pParent);
}

BOOL CBPCondDlg::OnInitDialog()
{
	if (!C3dDialog::OnInitDialog())
		return FALSE;

	((CEdit *)GetDlgItem(ID_BPCOND_EXPR))->LimitText(CB_MAX_EXPRESSION-1);
	((CEdit *)GetDlgItem(ID_BPCOND_EXPR))->SetWindowText(m_strExpr);
	((CEdit *)GetDlgItem(ID_BPCOND_EXPR))->SetSel(0,-1);

	((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->LimitText(CB_MAX_PASSCNT-1);
	((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->SetWindowText(m_strSkip);

	((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->LimitText(CB_MAX_LENGTH-1);
	((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->SetWindowText(m_strLength);

	OnChangeExpression();

	return TRUE;
}

void CBPCondDlg::OnChangeExpression()
{
	CString strExpr;

	((CEdit *)GetDlgItem(ID_BPCOND_EXPR))->GetWindowText(strExpr);

    if ( !strExpr.IsEmpty() ) {
		m_fBreakWhenExprTrue = FBreakWhenExprTrue(strExpr);
	}
	else {
		m_fBreakWhenExprTrue = FALSE;
	}	
	
	// Enable the correct fields...
	((CButton *)GetDlgItem(ID_BPCOND_TXT_SKIPCOUNT))->EnableWindow(strExpr.IsEmpty() || m_fBreakWhenExprTrue);
	((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->EnableWindow(strExpr.IsEmpty() || m_fBreakWhenExprTrue);

    // Java: Don't allow ranges for condition BP's
    if (pDebugCurr && pDebugCurr->IsJava()) {
        LONG   dy;

        // Hide the range edit box and text
	    ((CButton *)GetDlgItem(IDC_BP_TXT_NUM_ELEMENTS))->ShowWindow(FALSE);
	    ((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->ShowWindow(FALSE);

        // move the other stuff up
        WINDOWPLACEMENT wndpl1;
        WINDOWPLACEMENT wndpl2;
        wndpl1.length = sizeof(WINDOWPLACEMENT);
        wndpl2.length = sizeof(WINDOWPLACEMENT);

        ((CWnd *)GetDlgItem(IDC_BP_TXT_NUM_ELEMENTS))->GetWindowPlacement (&wndpl2);
	    ((CWnd *)GetDlgItem(ID_BPCOND_TXT_SKIPCOUNT))->GetWindowPlacement (&wndpl1);
         dy = wndpl1.rcNormalPosition.top - wndpl2.rcNormalPosition.top;
        OffsetRect(&wndpl1.rcNormalPosition, 0, -dy);
	    ((CWnd *)GetDlgItem(ID_BPCOND_TXT_SKIPCOUNT))->SetWindowPlacement (&wndpl1);
	    ((CWnd *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->GetWindowPlacement (&wndpl1);
        OffsetRect(&wndpl1.rcNormalPosition, 0, -dy);
	    ((CWnd *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->SetWindowPlacement (&wndpl1);

        ((CButton *)GetDlgItem(ID_BPCOND_TXT_SKIPCOUNT))->UpdateWindow();
	    ((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->UpdateWindow();

    }
    else {
		((CButton *)GetDlgItem(IDC_BP_TXT_NUM_ELEMENTS))->EnableWindow(!m_fBreakWhenExprTrue);
		((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->EnableWindow(!m_fBreakWhenExprTrue);
	}

	((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_TRUE))->ShowWindow(m_fBreakWhenExprTrue);
	((CWnd *)GetDlgItem(IDC_BP_BREAK_WHEN_CHANGES))->ShowWindow(!m_fBreakWhenExprTrue);
	
}

///////////////////////////////////////////////////////////////////////////////
// void CBPCondDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CBPCondDlg::OnOK()
{	
	BOOL fTranslated;
	LONG lVal;
	char szPass[CB_MAX_PASSCNT];
	char szLength[CB_MAX_LENGTH];
	CString str;
	CString strCap;

	if (m_fBreakWhenExprTrue)
	{
		// Verify the skip count field
		((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->GetWindowText(szPass, sizeof(szPass));
		if (szPass[0] != '\0')
        {
			fTranslated = fScanAnyLong(szPass, &lVal, 0L, (long)INT_MAX); //SHRT_MAX

			if (!fTranslated)
			{
				VERIFY(str.LoadString(IDS_BP_INVALID_HITCOUNT));
				VERIFY(strCap.LoadString(IDS_BP_INVALID_CAPTION));
				
				MessageBox(str, strCap, MB_OK|MB_ICONEXCLAMATION);
				
				((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->SetFocus();
				((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->SetSel(0,-1);
				return;
			}
		}
	}
	else
	{
		// Verify the Number of Elements field
		((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->GetWindowText(szLength, sizeof(szLength));
		if (szLength[0] != '\0')
        {
			fTranslated = fScanAnyLong(szLength, &lVal, 1L, (long)INT_MAX);

			if (!fTranslated)
			{
				VERIFY(str.LoadString(IDS_BP_INVALID_LENGTH));
				VERIFY(strCap.LoadString(IDS_BP_INVALID_CAPTION));

				MessageBox(str, strCap, MB_OK|MB_ICONEXCLAMATION);

				((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->SetFocus();
				((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->SetSel(0,-1);
				return;
			}
		}
	}

	((CEdit *)GetDlgItem(ID_BPCOND_SKIPCOUNT))->GetWindowText(m_strSkip);
	((CEdit *)GetDlgItem(IDC_BP_NUM_ELEMENTS))->GetWindowText(m_strLength);
	((CEdit *)GetDlgItem(ID_BPCOND_EXPR))->GetWindowText(m_strExpr);

	CDialog::OnOK();
}


///////////////////////////////////////////////////////////////////////////////
// CBPContextDlg implementation
///////////////////////////////////////////////////////////////////////////////

CBPContextDlg::CBPContextDlg(CWnd * pParent /* = NULL */)
	: C3dDialog(CBPContextDlg::IDD, pParent)
{
	// Don't forget to set the parent to the bp dlg!
	ASSERT(pParent);
}

BOOL CBPContextDlg::OnInitDialog()
{
	if (!C3dDialog::OnInitDialog())
		return FALSE;

	((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC))->LimitText(FUNC_NAME_LEN-1);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_SOURCE))->LimitText(CB_MAX_PATH-1);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_MODULE))->LimitText(CB_MAX_PATH-1);

	((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC))->SetWindowText(m_strFunc);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_SOURCE))->SetWindowText(m_strSource);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_MODULE))->SetWindowText(m_strModule);

	((CEdit *)GetDlgItem(IDC_BP_TXT_WHERE))->SetWindowText(m_strlblWhere);
	((CEdit *)GetDlgItem(IDC_BP_WHERE))->LimitText(CB_MAX_LOCATION - 1);
	((CEdit *)GetDlgItem(IDC_BP_WHERE))->SetWindowText(m_strWhere);

	// For Java, the function name part of the context doesn't make sense,
	// so we'll gray it out here. We'll also change the static text for
	// the module part of the context to say "Class file:".
	if (pDebugCurr && pDebugCurr->IsJava())
	{
		// Disable to function field.
		((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC))->EnableWindow(FALSE);
		((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC_TXT))->EnableWindow(FALSE);
		// Change "Executable file:" to "Class file:".
		CString strClassFile;
		VERIFY(strClassFile.LoadString(IDS_JAVA_BPCXT_CLASS));
		((CEdit *)GetDlgItem(ID_BPCONTEXT_MODULE_TXT))->SetWindowText(strClassFile);
	}

	if (!m_strWhere.IsEmpty())
	{
		// If location is already entered, put focus in context group
		((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC))->SetFocus();
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// void CBPContextDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CBPContextDlg::OnOK()
{	
	((CEdit *)GetDlgItem(IDC_BP_WHERE))->GetWindowText(m_strWhere);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_FUNC))->GetWindowText(m_strFunc);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_SOURCE))->GetWindowText(m_strSource);
	((CEdit *)GetDlgItem(ID_BPCONTEXT_MODULE))->GetWindowText(m_strModule);

	// Quote Filenames if necessary. 
	if (FFilenameNeedsQuotes (m_strSource)) {
		m_strSource = _T('"') + m_strSource + _T('"');
	}
	if (FFilenameNeedsQuotes (m_strModule)) {
		m_strModule = _T('"') + m_strModule + _T('"');
	}

	CDialog::OnOK();
}






// msgptabs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "hotlinkc.h"
#include "proptab.h"
#include "msgptabs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMessageGeneralPropTab

CMessageGeneralPropTab::~CMessageGeneralPropTab()
{
}


INT CMessageGeneralPropTab::m_aControlIDs[] =
{
	IDC_MGT_HWND,
	IDC_MGT_NESTLEVEL,
	IDC_MGT_MESSAGE1,
	IDC_MGT_MESSAGE2,
	IDC_MGT_LRESULT,
	IDC_MGT_LIST,
	0
};


BEGIN_MESSAGE_MAP(CMessageGeneralPropTab, CPropertyTab)
	//{{AFX_MSG_MAP(CMessageGeneralPropTab)
	ON_BN_CLICKED(IDC_MGT_MSGHELP, OnMessageHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageGeneralPropTab message handlers

BOOL CMessageGeneralPropTab::OnInitDialog()
{
	CPropertyTab::OnInitDialog();

	m_HotLinkWindowHandle.SubclassDlgItem(IDC_MGT_HWND, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMessageGeneralPropTab::UpdateFields()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	m_HotLinkWindowHandle.SetJumpObject((DWORD)pTabDlg->m_msd2.hwnd);

	SetDlgItemInt(IDC_MGT_NESTLEVEL, pTabDlg->m_msd2.nLevel);
	SetDlgItemText(IDC_MGT_MESSAGE1, pTabDlg->m_strMessage1);
	SetDlgItemText(IDC_MGT_MESSAGE2, pTabDlg->m_strMessage2);

	if (pTabDlg->m_msd2.fPostType == POSTTYPE_RETURN)
	{
		GetDlgItem(IDC_MGT_LRESULTLABEL)->ShowWindow(TRUE);
		SetDlgItemText(IDC_MGT_LRESULT, FormatHex(pTabDlg->m_msd2.lResult));
	}
	else
	{
		GetDlgItem(IDC_MGT_LRESULTLABEL)->ShowWindow(FALSE);
		SetDlgItemText(IDC_MGT_LRESULT, "");
	}

	//
	// Show the parameters in the listbox.  To do this, we construct
	// a CMsgParmStream object that the parm cracker will write to,
	// attach the listbox to it then call the appropriate cracker.
	//
	CMsgParmStream MsgParmStream;
	CListBox* pLB = (CListBox *)GetDlgItem(IDC_MGT_LIST);
	pLB->ResetContent();
	MsgParmStream.SetOutputLB(pLB);

	switch (pTabDlg->m_msd2.mtMsgType)
	{
		case MT_NORMAL:
			//
			// Call the message specific parm decoder.
			//
			(*CMsgDoc::m_apmdLT[pTabDlg->m_msd2.msg]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);

			//
			// Enable the help button.
			//
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_DIALOG:
			(*CMsgDoc::m_apmdDlgLT[pTabDlg->m_msd2.msg - WM_USER]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

#ifndef DISABLE_WIN95_MESSAGES
		case MT_ANIMATE:
			(*CMsgDoc::m_apmdAniLT[pTabDlg->m_msd2.msg - (WM_USER + 100)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_HEADER:
			(*CMsgDoc::m_apmdHdrLT[pTabDlg->m_msd2.msg - HDM_FIRST]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_HOTKEY:
			(*CMsgDoc::m_apmdHKLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_LISTVIEW:
			(*CMsgDoc::m_apmdLVLT[pTabDlg->m_msd2.msg - LVM_FIRST]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_PROGRESS:
			(*CMsgDoc::m_apmdProgLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_STATUSBAR:
			(*CMsgDoc::m_apmdStatLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_TOOLBAR:
			(*CMsgDoc::m_apmdTBLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_TRACKBAR:
			(*CMsgDoc::m_apmdTrkLT[pTabDlg->m_msd2.msg - WM_USER]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_TABCTRL:
			(*CMsgDoc::m_apmdTabLT[pTabDlg->m_msd2.msg - TCM_FIRST]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_TOOLTIP:
			(*CMsgDoc::m_apmdTTLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_TREEVIEW:
			(*CMsgDoc::m_apmdTVLT[pTabDlg->m_msd2.msg - TV_FIRST]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;

		case MT_UPDOWN:
			(*CMsgDoc::m_apmdUpDnLT[pTabDlg->m_msd2.msg - (WM_USER + 101)]->pfnDecodeParm)(&pTabDlg->m_msd2, &MsgParmStream);
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;
#endif	// DISABLE_WIN95_MESSAGES
		default:
			//
			// Call a generic parm decoder.
			//
			ParmGeneric(&pTabDlg->m_msd2, &MsgParmStream);

			//
			// No help for unknown messages, so disable the button.
			//
			GetDlgItem(IDC_MGT_MSGHELP)->EnableWindow(FALSE);
			break;
	}

	//
	// Set the horizontal extent on the scrollbar to the widest
	// string that was logged by the stream.  Note that we add
	// a little slop here to be SURE we don't clip the string.
	//
	pLB->SendMessage(LB_SETHORIZONTALEXTENT, MsgParmStream.GetMaxLineWidth() + 10, 0);
}



void CMessageGeneralPropTab::OnMessageHelp()
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	//
	// Must be a known message (the button should be grayed if not)
	//
	switch(pTabDlg->m_msd2.mtMsgType)
	{
		case MT_NORMAL:
			ASSERT(pTabDlg->m_msd2.msg < MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdLT[pTabDlg->m_msd2.msg]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdLT[pTabDlg->m_msd2.msg]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_DIALOG:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdDlgLT[pTabDlg->m_msd2.msg - WM_USER]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdDlgLT[pTabDlg->m_msd2.msg - WM_USER]->pszMsg, HELPTYPE_MSGHELP);
			break;

#ifndef DISABLE_WIN95_MESSAGES
		case MT_ANIMATE:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdAniLT[pTabDlg->m_msd2.msg - (WM_USER + 100)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdAniLT[pTabDlg->m_msd2.msg - (WM_USER + 100)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_HEADER:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdHdrLT[pTabDlg->m_msd2.msg - HDM_FIRST]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdHdrLT[pTabDlg->m_msd2.msg - HDM_FIRST]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_HOTKEY:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdHKLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdHKLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_LISTVIEW:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdLVLT[pTabDlg->m_msd2.msg - LVM_FIRST]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdLVLT[pTabDlg->m_msd2.msg - LVM_FIRST]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_PROGRESS:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdProgLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdProgLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_STATUSBAR:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdStatLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdStatLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_TOOLBAR:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdTBLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdTBLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_TRACKBAR:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdTrkLT[pTabDlg->m_msd2.msg - WM_USER]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdTrkLT[pTabDlg->m_msd2.msg - WM_USER]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_TABCTRL:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdTabLT[pTabDlg->m_msd2.msg - TCM_FIRST]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdTabLT[pTabDlg->m_msd2.msg - TCM_FIRST]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_TOOLTIP:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdTTLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdTTLT[pTabDlg->m_msd2.msg - (WM_USER + 1)]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_TREEVIEW:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdTVLT[pTabDlg->m_msd2.msg - TV_FIRST]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdTVLT[pTabDlg->m_msd2.msg - TV_FIRST]->pszMsg, HELPTYPE_MSGHELP);
			break;

		case MT_UPDOWN:
			ASSERT(pTabDlg->m_msd2.msg >= MAX_MESSAGES);
			ASSERT(CMsgDoc::m_apmdUpDnLT[pTabDlg->m_msd2.msg - (WM_USER + 101)]);
			theApp.InvokeWinHelp((DWORD)CMsgDoc::m_apmdUpDnLT[pTabDlg->m_msd2.msg - (WM_USER + 101)]->pszMsg, HELPTYPE_MSGHELP);
			break;
#endif	// DISABLE_WIN95_MESSAGES
	}
}

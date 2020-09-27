// ipcfrmhk.cpp : implementation of the CIPCompFrameHook class
//

#include "stdafx.h"
#pragma hdrstop

#include <stdidcmd.h>
#include <docobj.h>
#include <oleipcid.h>
#include "shldocs_.h"
#include "ipcmpctr.h"
#include "ipcfrmhk.h"
#include "ipchost.h"
#include "resource.h"
#include <cmguids.h>

#include <shlmenu.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINT WM_RESETCOMMANDTARGET = RegisterMessage(_T("Reset Command Target"));

/////////////////////////////////////////////////////////////////////////////
// CIPCompFrameHook implementation

BEGIN_INTERFACE_MAP(CIPCompFrameHook, COleFrameHook )
	INTERFACE_PART(CIPCompFrameHook, IID_IOleComponentUIManager, OleCompUIMgr)
	INTERFACE_PART(CIPCompFrameHook, IID_IOleInPlaceComponentUIManager, OleIPCompUIMgr)
	INTERFACE_PART(CIPCompFrameHook, IID_IServiceProvider, ServiceProvider)
END_INTERFACE_MAP()

CIPCompFrameHook::CIPCompFrameHook(CFrameWnd* pFrameWnd, CFrameWnd* pDocFrameWnd, COleClientItem* pItem)
	:COleFrameHook(pFrameWnd, pItem)
{
	m_fEnteringState = FALSE;
	m_fTopLevelContext = FALSE;
	ASSERT((CWnd *)pFrameWnd == AfxGetMainWnd());
	((CMainFrame *)AfxGetMainWnd())->AddIPCompFrameHook(this);
	
	m_pDocFrameWnd = pDocFrameWnd;
	if (pDocFrameWnd)
	{
		ASSERT(pDocFrameWnd->m_pNotifyHook == NULL); //since we are inplace component
		pDocFrameWnd->m_pNotifyHook = this;    // assume start out hooked
	}
}

CIPCompFrameHook::~CIPCompFrameHook()
{
}

IOleComponentUIManager * CIPCompFrameHook::GetIOleComponentUIManager()
{
	return &m_xOleCompUIMgr;
}

IOleInPlaceComponentUIManager * CIPCompFrameHook::GetIOleInPlaceComponentUIManager()
{
	return &m_xOleIPCompUIMgr;
}

BOOL CIPCompFrameHook::OnCompCmdMsg(GUID *pguid, DWORD cmdid, DWORD rgf,
		int nCode, void* pExtra)
{
	BOOL fHandled = FALSE;

	// The in-place component spec describes how commands are to be
	// routed.	Commands are divided into two categories, Active Object
	// commands and Container commands.  In addition, the main hardwired
	// component can request that Active Object commands be routed as
	// Container commands by returning the OLECOMPFLAG_ROUTEACTIVEASCNTRCMD
	// from IOleInPlaceComponent::UseComponentUIManager in the pgrf
	// parameter.
	//
	// This gives three possibilities for routing:
	//
	// 1. Active Object command:
	//		Active component control (if any)
	//		shell
	//		main hardwired component
	//		shell
	//
	// 2. Container command:
	//		shell
	//		main hardwired component
	//		shell
	//
	// 3. Active Object command routed as Container command:
	//		shell
	//		main hardwired component
	//		active object
	//		shell
	//
	// Normally, the shell does not take advantage of its first
	// opportunity to handle a command.  However, if the command
	// is marked with the OLECMDROUTEFLAG_SHELLFIRST flag, then the
	// shell does exploit this opportunity.
	//
	// Merged together, these give the following sequence:
	//
	//		Active component control (if neither container command
	//			nor routed as container command)
	//		Shell (if shell-first container command)
	//		Main hardwired component
	//		Active component control (if active object command
	//			routed as container command)
	//		Shell (if not shell-first container command)
	//
	// The Shell responsibilities are handled by the view; this
	// object is only responsible for communication with the component.
	//
	// Note that while we are displaying a context menu associated
	// with a top-level component, commands are not routed to the
	// main in-place component.
	//
	if (nCode == CN_COMMAND)
	{
		// Execute the command.
		//
		//$CONSIDER -- we don't want to route commands if the
		// main component is not active.  However, in that
		// case, we should not have any command targets, so
		// it's only an optimization.
		//
		if (m_srpMainComp != NULL)
		{
			// Note that the first and third clauses below are
			// mutually exclusive.
			//
			if (!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
				!(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
				m_srpActiveCmdTrgt != NULL)
			{
				fHandled = ExecCmd(m_srpActiveCmdTrgt, pguid, cmdid);
			}

			if (!m_fTopLevelContext && !fHandled && m_srpMainCmdTrgt != NULL)
			{
				fHandled = ExecCmd(m_srpMainCmdTrgt, pguid, cmdid);
			}

			if (!fHandled &&
				!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
				(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
				m_srpActiveCmdTrgt != NULL)
			{
				fHandled = ExecCmd(m_srpActiveCmdTrgt, pguid, cmdid);
			}
		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		// Determine the appropriate state of this command.
		// pExtra is the CCmdUI object.
		//
		ASSERT(pExtra != NULL);
		CCmdUI *pCmdUI = (CCmdUI *)pExtra;

		//$CONSIDER -- we don't want to route commands if the
		// main component is not active.  However, in that
		// case, we should not have any command targets, so
		// it's only an optimization.
		//
		if (m_srpMainComp != NULL)
		{
			// Note that the first and third clauses below are
			// mutually exclusive.
			//
			if (!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
				!(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
				m_srpActiveCmdTrgt != NULL)
			{
				fHandled = QueryCmdStatus(m_srpActiveCmdTrgt, pguid, cmdid, pCmdUI, rgf);
			}

			if (!m_fTopLevelContext && !fHandled && m_srpMainCmdTrgt != NULL)
			{
				fHandled = QueryCmdStatus(m_srpMainCmdTrgt, pguid, cmdid, pCmdUI, rgf);
			}

			if (!fHandled &&
				!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
				(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
				m_srpActiveCmdTrgt != NULL)
			{
				fHandled = QueryCmdStatus(m_srpActiveCmdTrgt, pguid, cmdid, pCmdUI, rgf);
			}
		}
	}

	return fHandled;
}

BOOL CIPCompFrameHook::QueryCmdStatus(IMsoCommandTarget *pCmdTrgt,
		GUID *pguid, UINT cmdid, CCmdUI *pCmdUI, DWORD rgf)
{
	//$UNDONE -- numeric constant.
	//
	BYTE rgbCmdTextBuf[sizeof(OLECMDTEXT)+255*sizeof(WCHAR)];
	OLECMDTEXT *pcmdtext = NULL;
	OLECMD msocmd;
	BOOL fIsMenu;

	msocmd.cmdID = (ULONG)cmdid;
	msocmd.cmdf = 0;

	fIsMenu = ::IsMenu(pCmdUI);

	if (fIsMenu)
	{
		// This is a menu item; give the component the
		// opportunity to change its text.
		//
		pcmdtext = (OLECMDTEXT *)((PVOID)rgbCmdTextBuf);
		pcmdtext->cmdtextf = OLECMDTEXTF_NAME;
		pcmdtext->cwActual = 0;
		pcmdtext->cwBuf = 255;
		pcmdtext->rgwz[0] = 0;
	}

	HRESULT hr = pCmdTrgt->QueryStatus(pguid, 1, &msocmd, pcmdtext);
	BOOL fRet = FALSE;

	if (SUCCEEDED(hr) && (msocmd.cmdf & OLECMDF_SUPPORTED))
	{
		// The component recognizes & supports this command.
		// Add a terminating null to the label string.
		//
		if (pcmdtext != NULL)
		{
			pcmdtext->rgwz[pcmdtext->cwActual] = 0;
		}

		pCmdUI->Enable((msocmd.cmdf & OLECMDF_ENABLED) ? TRUE : FALSE);

		if (!(rgf & OLECMDROUTEFLAG_SINGLESTATE))
			pCmdUI->SetCheck((msocmd.cmdf & OLECMDF_LATCHED) ? 1 : 0);

		USES_CONVERSION;
		if (pcmdtext != NULL && pcmdtext->cwActual != 0)
			pCmdUI->SetText(OLE2T(pcmdtext->rgwz));

		// If the command is enabled and the document has requested
		// an opportunity for further command UI processing (indicated
		// by OLECMDROUTEFLAG_PACKAGEUPDATE in the routing flags), give
		// it that opportunity now.
		//
		if ( (rgf & OLECMDROUTEFLAG_PACKAGEUPDATE) &&
			 (msocmd.cmdf & OLECMDF_ENABLED) &&
			 (m_pActiveItem != NULL))
		{
			CIPCompDoc *pDoc;

			ASSERT(m_pActiveItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)));
			pDoc = ((CIPCompContainerItem *)(m_pActiveItem))->GetDocument();

			if (pDoc != NULL)
			{
				pDoc->UpdateIPCToolbarControl(pguid, cmdid, rgf);
			}
		}

		//$CONSIDER -- treating all supported commands as handled
		// prevents a later contributor to the chain from enabling
		// a command.  Is this a good thing or a bad thing?
		//
		fRet = TRUE;
	}

	return fRet;
}

BOOL CIPCompFrameHook::ExecCmd(IMsoCommandTarget *pCmdTrgt, GUID *pguid, UINT cmdid)
{
	HRESULT hr = pCmdTrgt->Exec(pguid, cmdid, OLECMDEXECOPT_DODEFAULT, NULL, NULL);

#ifndef DOCOBJ_CHANGE
	BOOL fRet = (hr == S_OK);
#else
	BOOL fRet = (hr != OLECMDERR_E_NOTSUPPORTED &&
			hr != OLECMDERR_E_UNKNOWNGROUP &&
			hr != E_NOTIMPL);
#endif
	return fRet;
}

IMsoCommandTarget * CIPCompFrameHook::GetSupportingTarget(GUID * pguid, DWORD cmdid, DWORD rgf)
{
	OLECMD msocmd;
	IMsoCommandTarget *pCmdTrgt = NULL;
	HRESULT hr;

	msocmd.cmdID = (ULONG)cmdid;
	msocmd.cmdf = 0;

	if (!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
		!(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
		m_srpActiveCmdTrgt != NULL)
	{
		hr = m_srpActiveCmdTrgt->QueryStatus(pguid, 1, &msocmd, NULL);
		if (SUCCEEDED(hr) &&
			(msocmd.cmdf & OLECMDF_SUPPORTED) &&
			(msocmd.cmdf & OLECMDF_ENABLED))
		{
			pCmdTrgt = m_srpActiveCmdTrgt;
		}
	}

	if (pCmdTrgt == NULL && m_srpMainCmdTrgt != NULL)
	{
		hr = m_srpMainCmdTrgt->QueryStatus(pguid, 1, &msocmd, NULL);
		if (SUCCEEDED(hr) &&
			(msocmd.cmdf & OLECMDF_SUPPORTED) &&
			(msocmd.cmdf & OLECMDF_ENABLED))
		{
			pCmdTrgt = m_srpMainCmdTrgt;
		}
	}

	if (pCmdTrgt == NULL &&
		!(rgf & OLECMDROUTEFLAG_CONTAINER) &&
		(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD) &&
		m_srpActiveCmdTrgt != NULL)
	{
		hr = m_srpActiveCmdTrgt->QueryStatus(pguid, 1, &msocmd, NULL);
		if (SUCCEEDED(hr) &&
			(msocmd.cmdf & OLECMDF_SUPPORTED) &&
			(msocmd.cmdf & OLECMDF_ENABLED))
		{
			pCmdTrgt = m_srpActiveCmdTrgt;
		}
	}

	// Note that this is not an add-ref'd pointer.
	//
	return pCmdTrgt;
}

void CIPCompFrameHook::SetIPCComboEditText(CComboBox * pcombo,
		GUID * pguid, DWORD cmdid, DWORD rgf)
{
	// First, determine whether to ask the main component or
	// the active control.
	//
	IMsoCommandTarget *pCmdTrgt = GetSupportingTarget(pguid, cmdid, rgf);

	if (pCmdTrgt == NULL)
	{
		// Nobody is willing to update this command.
		//
		return;
	}

	if (CMDSETID_StandardCommandSet97 == *pguid)
	{
		switch (cmdid)
		{
			case cmdidZoomPercent:
			{
				// Ask the component for the current zoom value.
				//
				HRESULT hr;
				VARIANT var;
				::VariantInit(&var);
				V_VT(&var) = VT_I4;

				CString strZoom, strCur;
				hr = pCmdTrgt->Exec(pguid,
									cmdid,
									OLECMDEXECOPT_DODEFAULT,
									NULL,
									&var);

				if (SUCCEEDED(hr) &&
					V_VT(&var) == VT_I4 &&
					V_I4(&var) > 0)
				{
					strZoom.Format(TEXT("%d%%"), V_I4(&var));
				}

				::VariantClear(&var);

				pcombo->GetWindowText(strCur);

				if (strCur != strZoom)
					pcombo->SetWindowText(strZoom);

			}
			return;

			case cmdidFontName:
			{
				// Ask the component for the current font name
				//
				HRESULT hr;
				VARIANT var;
				::VariantInit(&var);
				V_VT(&var) = VT_BSTR;

				CString strItem, strCur;
				hr = pCmdTrgt->Exec(pguid,
									cmdid,
									OLECMDEXECOPT_DODEFAULT,
									NULL,
									&var);
				if(SUCCEEDED(hr) && V_VT(&var) == VT_BSTR && V_BSTR(&var) != NULL)
				{
					strItem = V_BSTR(&var);
					::SysFreeString(V_BSTR(&var));
				}

				::VariantClear(&var);

				// Setting an empty Item empties the combo
				if(!strItem.IsEmpty())
				{
					pcombo->GetWindowText(strCur);
					// Only change text if Font Name differs from current selection.
					if(strCur != strItem)
						pcombo->SetWindowText(strItem);
				}
			}
			return;

			case cmdidFontSize:
			{
				// Ask the component for the current font size
				//
				HRESULT hr;
				VARIANT var;
				::VariantInit(&var);
				V_VT(&var) = VT_I4;

				CString strItem, strCur;
				hr = pCmdTrgt->Exec(pguid,
									cmdid,
									OLECMDEXECOPT_DODEFAULT,
									NULL,
									&var);
				if(SUCCEEDED(hr) && V_VT(&var) == VT_I4)
				{
					strItem.Format(_T("%d"), V_I4(&var));
				}

				::VariantClear(&var);

				// Setting an empty Item empties the combo
				if(!strItem.IsEmpty())
				{
					pcombo->GetWindowText(strCur);
					// Only change text if Font Name differs from current selection.
					if(strCur != strItem)
						pcombo->SetWindowText(strItem);
				}
			}
			return;

			default:
				// Unrecognized.
				break;
		}
	}
}

void CIPCompFrameHook::FillZoomCombo(CComboBox * pcombo, IMsoCommandTarget *pCmdTrgt)
{
	CString str;
	VARIANT var;
	HRESULT hr;
	long i, iLow, iHigh;
	int cCur;
	UINT sid;

	::VariantInit(&var);
	V_VT(&var) = VT_ARRAY | VT_I4;

	hr = pCmdTrgt->Exec(&CMDSETID_StandardCommandSet97,
						cmdidGetZoom,
						OLECMDEXECOPT_DODEFAULT,
						NULL,
						&var);

	if (SUCCEEDED(hr) && V_VT(&var) == (VT_ARRAY | VT_I4))
	{
		SafeArrayGetLBound(V_ARRAY(&var), 1, &iLow);
		SafeArrayGetUBound(V_ARRAY(&var), 1, &iHigh);
		HRESULT hr2 = NOERROR;

		for (i = iLow; i <= iHigh && SUCCEEDED(hr2); i++)
		{
			hr2 = SafeArrayGetElement(V_ARRAY(&var), &i, &cCur);
			if (FAILED(hr2))
				break;

			if (cCur >= 0)
			{
				str.Format(TEXT("%d%%"), cCur);
			}
			else
			{
				// This is a special string.  Fetch it as a resource.
				//
				switch(cCur)
				{
				case CMD_ZOOM_PAGEWIDTH:
					sid = IDS_ZOOM_PAGE_WIDTH;
					break;
				case CMD_ZOOM_ONEPAGE:
					sid = IDS_ZOOM_ONE_PAGE;
					break;
				case CMD_ZOOM_TWOPAGES:
					sid = IDS_ZOOM_TWO_PAGES;
					break;
				case CMD_ZOOM_SELECTION:
					sid = IDS_ZOOM_SELECTION;
					break;
				case CMD_ZOOM_FIT:
					sid = IDS_ZOOM_FIT;
					break;
				default:
					// Unrecognized negative number--skip this one.
					//
					continue;
				}

				str.Format(sid);
			}

			pcombo->SetItemData(pcombo->AddString(str), cCur);
		}
	}

	if (SUCCEEDED(hr))
	{
		::VariantClear(&var);
	}
}

int CALLBACK EnumFontNamesCallBack
(
	const ENUMLOGFONT*		pEnumLogFont,
	const NEWTEXTMETRIC*	pTextMetric,
	int						fontType,
	LPARAM					lParam
) 
{
	ASSERT(pEnumLogFont != NULL);
	ASSERT(pTextMetric != NULL);
	ASSERT(lParam != NULL);

	// ComboBox in lParam
	CComboBox* pComboBox = (CComboBox*)lParam;

	// Add the font name to Combo Box
	ASSERT(pEnumLogFont->elfLogFont.lfFaceName != NULL);

	int iRet = pComboBox->AddString(pEnumLogFont->elfLogFont.lfFaceName);
	ASSERT(iRet >= 0);

	// returns TRUE if String added successfully, FALSE otherwise
	return (iRet >= 0);
}


// Gets the list of Fonts and populates the list
void CIPCompFrameHook::FillFontNameCombo(CComboBox* pComboBox, IMsoCommandTarget *pCmdTrgt)
{
	ASSERT(pComboBox != NULL);
	ASSERT(pCmdTrgt != NULL);

	// REVIEW(cgomes): Should I check if the combo box has already been filled?
	// I could just remove all items.  Fonts could be added while the combo is up!!!!
	// Skip this step if Fonts already in ComboBox
	int nFontNames = pComboBox->GetCount();
	ASSERT(nFontNames != CB_ERR);
	if(nFontNames == CB_ERR || nFontNames > 0)
		return;

	// Get DC for EnumFontFamiliess
	CWnd* pWnd = ::AfxGetMainWnd();
	ASSERT(pWnd != NULL);
	ASSERT_KINDOF(CWnd, pWnd);

	CDC* pDC = pWnd->GetDC();
	ASSERT(pDC != NULL);
	ASSERT(pDC->m_hDC);

	// Enum Fonts via Call Back
	// The Call Back will add the Fonts to the ComboBox
	VERIFY(EnumFontFamilies(pDC->m_hDC, NULL, (FONTENUMPROC)EnumFontNamesCallBack, 
							(LPARAM)pComboBox));

	pWnd->ReleaseDC(pDC);
}

// HACK! EnumFontSizesCallBack needs this
static int s_nPixPerInchY;

int CALLBACK EnumFontSizesCallBack
(
	const ENUMLOGFONT*		pEnumLogFont,
	const NEWTEXTMETRIC*	pTextMetric,
	int						fontType,
	LPARAM					lParam
) 
{
	if (fontType & TRUETYPE_FONTTYPE) 
		return TRUETYPE_FONTTYPE;
	else
	{
		UINT nCharSet = theApp.GetCharSet();
		if ((nCharSet == DEFAULT_CHARSET || nCharSet == pTextMetric->tmCharSet || pTextMetric->tmCharSet == OEM_CHARSET))
		{
			// ComboBox in lParam
			CComboBox* pComboBox = (CComboBox*)lParam;

			CString strItem;
			strItem.Format(_T("%d"), MulDiv(pTextMetric->tmHeight - pTextMetric->tmInternalLeading,72,s_nPixPerInchY));
			pComboBox->AddString(strItem);
		}
		return RASTER_FONTTYPE;
	}
}

// True Type Font sizes
static DWORD s_rgTTSizes[] = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72 };

// Gets the Current Font, then Enumerates size for that font
void CIPCompFrameHook::FillFontSizeCombo(CComboBox* pComboBox, LPCTSTR pszFontName)
{
	ASSERT(pComboBox != NULL);
	ASSERT(pszFontName != NULL);

	// REVIEW(cgomes): Should I check if the combo box has already been filled?
	// I could just remove all items.  Fonts could be added while the combo is up!!!!
	// Skip this step if Fonts already in ComboBox
	int nFontSizes = pComboBox->GetCount();
	ASSERT(nFontSizes != CB_ERR);
	if(nFontSizes == CB_ERR || nFontSizes > 0)
		return;

	// Get DC for EnumFontFamiliess
	CWnd* pWnd = ::AfxGetMainWnd();
	ASSERT(pWnd != NULL);
	ASSERT_KINDOF(CWnd, pWnd);

	CDC* pDC = pWnd->GetDC();
	ASSERT(pDC != NULL);
	ASSERT(pDC->m_hDC);

	// Set s_nPixPerInchY for EnumFontSizesCallBack
	s_nPixPerInchY = pDC->GetDeviceCaps(LOGPIXELSY);

	// Enum Font Sizes via Call Back
	// If the Font is not a TrueType font then 
	//		the Call Back will add the Sizes to the ComboBox
	int nType = EnumFontFamilies(pDC->m_hDC, pszFontName, 
							(FONTENUMPROC)EnumFontSizesCallBack, (LPARAM)pComboBox);
	if(nType == TRUETYPE_FONTTYPE)
	{
		CString strItem;
		for (int i = 0; i < sizeof(s_rgTTSizes) / sizeof (s_rgTTSizes[0]); i++)
		{
			strItem.Format(_T("%d"), s_rgTTSizes[i]);
			pComboBox->AddString(strItem);
		}
	}
	pWnd->ReleaseDC(pDC);
}

void CIPCompFrameHook::FillIPCCombo(CComboBox * pcombo, GUID * pguid,
		DWORD cmdid, DWORD rgf)
{
	// Extract the current value, so we can set the current selection.
	//
	CString strCur;
	pcombo->GetWindowText(strCur);

	// Clean out the old strings.  Note that ResetContent would
	// also clear the edit box, which we don't want.
	//
	while (pcombo->DeleteString(0) != CB_ERR);

	// First, determine whether to ask the main component or
	// the active control.
	//
	IMsoCommandTarget *pCmdTrgt = GetSupportingTarget(pguid, cmdid, rgf);

	if (pCmdTrgt == NULL)
	{
		// Nobody is willing to update this command.
		//
		return;
	}

	if (CMDSETID_StandardCommandSet97 == *pguid)
	{
		if(cmdid == cmdidZoomPercent)
		{
			// Zoom has special semantics.
			//
			FillZoomCombo(pcombo, pCmdTrgt);
		}
		else if(cmdid == cmdidFontName)
		{
			FillFontNameCombo(pcombo, pCmdTrgt);
		}
		else if(cmdid == cmdidFontSize)
		{
			// Ask the component for the current font name
			//
			HRESULT hr;
			VARIANT var;
			::VariantInit(&var);
			V_VT(&var) = VT_BSTR;

			hr = pCmdTrgt->Exec(pguid, cmdidFontName, OLECMDEXECOPT_DODEFAULT,
								NULL, &var);
			if(SUCCEEDED(hr) && V_VT(&var) == VT_BSTR && V_BSTR(&var) != NULL)
			{
				USES_CONVERSION;
				// Fill Font Size using current font name
				FillFontSizeCombo(pcombo, OLE2T(V_BSTR(&var)));
				::SysFreeString(V_BSTR(&var));
			}
			::VariantClear(&var);
		}
		else
		// Unrecognized.
		//
		//$CONSIDER -- we could support the standard contract here
		//(call Exec to retrieve the list of strings that go into
		// the combo box).
		return;
	} 
	else 
	{

		// Unrecognized.
		//
		//$CONSIDER -- we could support the standard contract here
		//(call Exec to retrieve the list of strings that go into
		// the combo box).
		return;
	}

	// We've filled the list box; now set the initial selection.
	//
	int i = pcombo->FindStringExact(-1, strCur);

	if (i != CB_ERR)
	{
		pcombo->SetCurSel(i);
	}

}

void CIPCompFrameHook::OnIPCComboSelect(CComboBox * pcombo, GUID * pguid,
		DWORD cmdid, DWORD rgf)
{
	// The user selected an item from the combo box's list.

	// First, determine whether to ask the main component or
	// the active control.
	//
	IMsoCommandTarget *pCmdTrgt = GetSupportingTarget(pguid, cmdid, rgf);

	if (pCmdTrgt == NULL)
	{
		// Nobody is willing to update this command.
		//
		return;
	}

	if (pguid == NULL)
	{
		// Not supported.
		return;
	}

	CString strCur;
	VARIANT varNew, varResult;
	::VariantInit(&varNew);
	::VariantInit(&varResult);

	if (CMDSETID_StandardCommandSet97 == *pguid)
	{
		switch (cmdid)
		{
		case cmdidZoomPercent:
			{
				HRESULT hr;

				//$CONSIDER -- common worker for this function
				// and OnIPCComboEnter?

				V_VT(&varNew) = VT_I4;
				V_I4(&varNew) = pcombo->GetItemData(pcombo->GetCurSel());

				hr = pCmdTrgt->Exec(pguid,
									cmdid,
									OLECMDEXECOPT_DODEFAULT,
									&varNew,
									&varResult);

				//$CONSIDER -- use varResult to update the edit box.
				//Can we count on all commands updating the same way,
				//and eliminate the call to UpdateIPCToolbarControl
				//from the CBN_SELENDOK handler?
			}

			break;

		case cmdidFontSize:

			// Since the shell didn't fill this combo box, it can't
			// rely on item data and has to use strings.
			//
			pcombo->GetLBText(pcombo->GetCurSel(), strCur);

			if (!strCur.IsEmpty())
			{
				float fp;
				int cch;
				const int cTwipsPerPoint = 20;

				sscanf((LPCSTR)strCur, TEXT("%f%n"), &fp, &cch);
				if (cch != strCur.GetLength())
				{
					// Let the component display the error.
					fp = (float)(0.0);
				}

				int cTwips = (int)(fp * cTwipsPerPoint + 0.5);

				V_VT(&varNew) = VT_I4;
				V_I4(&varNew) = cTwips;

				HRESULT hr = pCmdTrgt->Exec(pguid,
											cmdid,
											OLECMDEXECOPT_DODEFAULT,
											&varNew,
											&varResult);

				//$CONSIDER -- update the edit box with the result.
			}

			break;

		case cmdidFontName:
			// Since the shell didn't fill this combo box, it can't
			// rely on item data and has to use strings.
			//
			pcombo->GetLBText(pcombo->GetCurSel(), strCur);
			if(!strCur.IsEmpty())
			{
				V_VT(&varNew)		= VT_BSTR;
				V_BSTR(&varNew)		= strCur.AllocSysString();
				V_VT(&varResult)	= VT_BSTR;

				HRESULT hr = pCmdTrgt->Exec(pguid,
											cmdid,
											OLECMDEXECOPT_DODEFAULT,
											&varNew,
											&varResult);
				if(SUCCEEDED(hr) && V_VT(&varResult)==VT_BSTR && 
					V_BSTR(&varResult) != NULL)
				{
					// REVIEW(cgomes): what to do with result???
					::SysFreeString(V_BSTR(&varResult));
				}

			}
			break;

		default:
			// Drop through into standard contract.
			//
			break;
		}
	}

	// Cleanup
	::VariantClear(&varNew);
	::VariantClear(&varResult);
}

void CIPCompFrameHook::OnIPCComboEnter(CComboBox * pcombo, GUID * pguid,
		DWORD cmdid, DWORD rgf)
{
	// The user pressed Enter in a combo box; take the contents of
	// the edit control as the new value.

	// First, determine whether to ask the main component or
	// the active control.
	//
	IMsoCommandTarget *pCmdTrgt = GetSupportingTarget(pguid, cmdid, rgf);

	if (pCmdTrgt == NULL)
	{
		// Nobody is willing to update this command.
		//
		return;
	}

	CString strCur;
	VARIANT varNew, varResult;
	::VariantInit(&varNew);
	::VariantInit(&varResult);

	if (CMDSETID_StandardCommandSet97 == *pguid)
	{
		switch (cmdid)
		{
		case cmdidZoomPercent:
			{
				int cZoom;

				// If the user selected a string that is in the
				// list, then we can just use the corresponding
				// value.  Otherwise we have to fetch a number
				// from the string.
				//
				pcombo->GetWindowText(strCur);
				int i = pcombo->FindStringExact(-1, strCur);

				if (i != CB_ERR)
				{
					cZoom = pcombo->GetItemData(i);

				} else {

					int cch = 0;
					float fp;

					sscanf((LPCSTR)strCur, TEXT("%f%n"), &fp, &cch);

					if (cch != strCur.GetLength())
					{
						sscanf((LPCSTR)strCur, TEXT("%f%%%n"), &fp, &cch);
						if (cch != strCur.GetLength())
						{
							// Get the component to display the error
							// for us.
							fp = (float)(0.0);
						}
					}

					cZoom = (int)(fp + 0.5);
				}

				V_VT(&varNew) = VT_I4;
				V_I4(&varNew) = cZoom;

				pCmdTrgt->Exec(pguid,
								cmdid,
								OLECMDEXECOPT_DODEFAULT,
								&varNew,
								&varResult);

				//$CONSIDER -- use varResult to update the edit box.
				//Can we count on all commands updating the same way,
				//and eliminate the call to UpdateIPCToolbarControl
				//from the CBN_SELENDOK handler?
			}

			break;

		case cmdidFontSize:
			// Since the shell didn't fill this combo box, it can't
			// rely on item data and has to use strings.
			//
			pcombo->GetWindowText(strCur);
			if (strCur.GetLength() != 0)
			{
				float fp;
				int cch;
				const int cTwipsPerPoint = 20;

				sscanf((LPCSTR)strCur, TEXT("%f%n"), &fp, &cch);
				if (cch != strCur.GetLength())
				{
					// Let the component display the error.
					fp = (float)(0.0);
				}

				int cTwips = (int)(fp * cTwipsPerPoint + 0.5);

				V_VT(&varNew) = VT_I4;
				V_I4(&varNew) = cTwips;

				pCmdTrgt->Exec(pguid,
								cmdid,
								OLECMDEXECOPT_DODEFAULT,
								&varNew,
								&varResult);

				//$CONSIDER -- update the edit box with the result.
			}

			break;

		case cmdidFontName:
			// Since the shell didn't fill this combo box, it can't
			// rely on item data and has to use strings.
			//
			pcombo->GetLBText(pcombo->GetCurSel(), strCur);
			if(!strCur.IsEmpty())
			{
				V_VT(&varNew)		= VT_BSTR;
				V_BSTR(&varNew)		= strCur.AllocSysString();
				V_VT(&varResult)	= VT_BSTR;

				HRESULT hr = pCmdTrgt->Exec(pguid,
											cmdid,
											OLECMDEXECOPT_DODEFAULT,
											&varNew,
											&varResult);
				if(SUCCEEDED(hr) && V_VT(&varResult)==VT_BSTR && 
					V_BSTR(&varResult) != NULL)
				{
					// REVIEW(cgomes): what to do with result???
					::SysFreeString(V_BSTR(&varResult));
				}

			}
			break;


		default:
			// Unrecognized.
			break;
		}
	}
	::VariantClear(&varNew);
	::VariantClear(&varResult);
}


/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleComponentUIManager

HRESULT CIPCompFrameHook::XOleCompUIMgr::GetPredefinedStrings(DISPID dispid,
						CALPOLESTR *pcaStringsOut, CADWORD *pcaCookiesOut)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	return E_NOTIMPL;
}

HRESULT CIPCompFrameHook::XOleCompUIMgr::GetPredefinedValue(DISPID dispid,
						DWORD dwCookie,	VARIANT *pvarOut)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	return E_NOTIMPL;
}

void CIPCompFrameHook::XOleCompUIMgr::OnUIEvent(DWORD dwCompRole,
						REFCLSID rclsidComp, const GUID *pguidUIEventGroup,
						DWORD nUIEventId, DWORD dwUIEventStatus,
						DWORD dwEventFreq, RECT *prcEventRegion,
						VARIANT *pvarEventArg)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	return;
}

HRESULT CIPCompFrameHook::XOleCompUIMgr::OnUIEventProgress(DWORD *pdwCookie,
						BOOL fInProgress, LPOLESTR pwszLabel, ULONG nComplete,
						ULONG nTotal)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	return E_NOTIMPL;
}

HRESULT CIPCompFrameHook::XOleCompUIMgr::SetStatus(LPCOLESTR pwszStatusText,
						DWORD dwReserved)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	return pThis->m_xOleInPlaceFrame.SetStatusText(pwszStatusText);
}

#if 0
// Had to pull this out b/c Forms Opens an Undo Record on the RBUTTONDOWN 
// that initiates the context menu.  This has the affect of causing the 
// Undo command to fail b/c there is an open undo record.
//
//	Description:
//		On return from this function the active IMsoCommandTarget will get reset.  
//		By default TrackPopupMenu will use PostMessage to route the selected 
//		command.  By the time the command gets handled the IMsoCommandTarget has
//		already been reset.  To fix this we need to ensure that the command gets
//		handled before the IMsoCommandTarget is reset.  Using the TPM_RETURNCMD
//		flag we can get the selected command.  Then use SendMessage to route the
//		command immediately.  Since SendMessage is synchronous IMsoCommandTarget
//		will not be reset before the command is handled.
//
static void DoContextPopupMenu(POPDESC* ppop, CPoint pt)
{
	CMainFrame* pWndCommand = (CMainFrame*) AfxGetMainWnd();
	ASSERT_VALID(pWndCommand);

	CBContextPopupMenu menuPopup;
	menuPopup.Create(ppop);

	// Call TrackPopup with the TPM_RETURNCMD to return the command. 
	int iCmdID = menuPopup.TrackPopup(TPM_LEFTALIGN | TPM_RIGHTBUTTON | 
									TPM_RETURNCMD, pt.x, pt.y, pWndCommand);

	// Use SendMessage to immediately route the command
	if(iCmdID != FALSE)
		// Send message
		::AfxGetMainWnd()->SendMessage(WM_COMMAND, iCmdID);
}
#endif
HRESULT CIPCompFrameHook::XOleCompUIMgr::ShowContextMenu(DWORD dwCompRole,
						REFCLSID rclsidActive, LONG nMenuId, POINTS &pos,
						IMsoCommandTarget *pCmdTrgtActive)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	// Before the context menu takes over, make sure the appropriate
	// command bars are visible.
	//
	if (theApp.m_bMenuDirty)
	{		
		DkUpdateAvailableWnds(TRUE);
	}

	CIPCompDoc *pDoc = NULL;

	if (pThis->m_pActiveItem != NULL)
	{
		ASSERT(pThis->m_pActiveItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)));
		pDoc = ((CIPCompContainerItem *)(pThis->m_pActiveItem))->GetDocument();
	}

	if (pThis->m_srpMainComp == NULL || pDoc == NULL)
		return E_UNEXPECTED;

	HRESULT hr = S_OK;
	
	// While the context menu is being processed, the command target
	// supplied as a parameter to this method is used in place of
	// the active object's command target.  Remember the current
	// active command target, so we can restore it later.
	// Note that the temporary holder for the command target is
	// _not_ a smart reference, because we will restore it before
	// we return.  (And we never throw out of this function.)
	//
	//
	IMsoCommandTarget * pOldCmdTrgt = pThis->m_srpActiveCmdTrgt.Disown();

	// If the component displaying the context menu is a top level
	// component, set a flag which inhibits routing commands to the
	// main in-place component.
	//
	if (dwCompRole == OLEROLE_TOPLEVELCOMPONENT)
	{
		pThis->m_fTopLevelContext = TRUE;
	}

	pThis->m_srpActiveCmdTrgt = pCmdTrgtActive;
	ASSERT(pCmdTrgtActive != NULL);
	pThis->m_srpActiveCmdTrgt->AddRef();

	POPDESC *ppopCntr = NULL;
	POPDESC *ppopActive = NULL;
	
	if (dwCompRole == OLEROLE_MAINCOMPONENT)
		{
		// Context menus from the main component never need to be merged.
		//
		ppopCntr = pDoc->PopupDescFromGuidId(rclsidActive, nMenuId);
		}
	else
		{
		CLSID clsidCntr = CLSID_NULL;
		OLEMENUID menuidCntr = {0, 0, NULL};
		DWORD rgf = 0;

		hr = pThis->m_srpMainComp->GetCntrContextMenu(dwCompRole, rclsidActive,
						nMenuId, pos, &clsidCntr, &menuidCntr, &rgf);

		// Note that the main component will return S_FALSE to
		// prevent any menu from being displayed.

		if (SUCCEEDED(hr) && hr != S_FALSE)
			{
			//$UNDONE - context menus can be specified by name
			// (indicated by -1 for menuidCntr.nMenuId).
			//
			ASSERT(menuidCntr.nMenuId != -1);

			if (rgf & OLEMENU_MENUMERGE)
				{
				// The main component has requested that we merge
				// the active component's context menu into the
				// main component's menu.
				//
				ppopCntr = pDoc->PopupDescFromGuidId(clsidCntr, menuidCntr.nMenuId);
				ppopActive = pDoc->PopupDescFromGuidId(rclsidActive, nMenuId);
				}
			else if (rgf & OLEMENU_CNTRMENUONLY)
				{
				// The main component wants to suppress the active
				// component control's context menu in favor of its own.
				//
				ppopCntr = pDoc->PopupDescFromGuidId(clsidCntr, menuidCntr.nMenuId);
				}
			else if (rgf & OLEMENU_OBJECTMENUONLY)
				{
				// The main component doesn't have anything to contribute;
				// just display the active component control's context menu.
				//
				ppopActive = pDoc->PopupDescFromGuidId(rclsidActive, nMenuId);
				}
			}
		}

	CPoint pt(pos.x, pos.y);

	if (ppopCntr == NULL && ppopActive == NULL)
	{
		hr = E_UNEXPECTED;
	}else if (ppopCntr == NULL)
	{
		//DoContextPopupMenu(ppopActive, pt);
		//::ShowContextPopupMenu(ppopActive, pt);
		::CBShowContextPopupMenu(ppopActive, pt);
		
	}else if (ppopActive == NULL)
	{
		//DoContextPopupMenu(ppopCntr, pt);
		//::ShowContextPopupMenu(ppopCntr, pt);
		::CBShowContextPopupMenu(ppopCntr, pt);
	}else
	{
		//$UNDONE -- Need to merge--I'm not ready to deal with this yet.
		//
		ASSERT(FALSE);
	}
#if 0
	// restore the active object command target and reset the
	// top-level-context-menu flag.
	//
	pThis->m_srpActiveCmdTrgt.SRelease();
	pThis->m_srpActiveCmdTrgt = pOldCmdTrgt;
#else
	// Defer resetting the ActiveCmdTrgt since the context menu command
	// is routed with a PostMessage.  
	// CmdTrgt will be reset when the view recieves a WM_RESETCOMMANDTARGET
	// The view will then call CIPCompFrameHook->ResetCommandTarget();
	// m_srpPreContextMenuCmdTrgt holds pOldCmdTrgt
	// UIActivateForMe can block the resetting of the CommandTarget.
	CView* pView = pThis->m_pDocFrameWnd->GetActiveView();
	ASSERT(NULL != pView);
	ASSERT(pThis->m_srpPreContextMenuCmdTrgt == NULL);
	pThis->m_srpPreContextMenuCmdTrgt = pOldCmdTrgt;
	pView->PostMessage(WM_RESETCOMMANDTARGET, 0, 0);
#endif

	pThis->m_fTopLevelContext = FALSE;

	return hr;
}

HRESULT CIPCompFrameHook::XOleCompUIMgr::ShowHelp(DWORD dwCompRole,
						REFCLSID rclsidComp,
						POINT posMouse, DWORD dwHelpCmd,
						LPOLESTR pwszHelpFile, DWORD dwData)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)

	CIPCompDoc *pDoc = NULL;

	if (pThis->m_pActiveItem != NULL)
	{
		ASSERT(pThis->m_pActiveItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)));
		pDoc = ((CIPCompContainerItem *)(pThis->m_pActiveItem))->GetDocument();
	}

	if (pThis->m_srpMainComp == NULL || pDoc == NULL)
		return E_UNEXPECTED;

	COleStrPtr srpwszNewHelpFile;
	CLSID clsid = rclsidComp;
	DWORD dwNewData = dwData;
	HRESULT hr = S_OK;

	// need to call main component's GetCntrHelp() method to see if the
	// main component want to override anything about the help.
	if (dwCompRole != OLEROLE_MAINCOMPONENT	&& dwCompRole != OLEROLE_COMPONENTHOST)
	{
		hr = pThis->m_srpMainComp->GetCntrHelp(&dwCompRole, &clsid,
					posMouse, dwHelpCmd, pwszHelpFile, &srpwszNewHelpFile,
					dwData, &dwNewData);

		// Note that the main component will return S_FALSE to
		// prevent any message from being displayed.

		//$CONSIDER -- if GetCntrMessage returns an error, should we
		// proceed and show the active object's message, or what?
		//
		if(hr == S_FALSE)
		{
			ASSERT(srpwszNewHelpFile == NULL);
			return S_FALSE;		// message suppressed by the MainComponent
		}
		else if (FAILED(hr))
		{
			return hr;
		} else {
			// check if MainComponent overrode the help file name or the data.
			if (srpwszNewHelpFile != NULL)
				pwszHelpFile = srpwszNewHelpFile;
			if (dwNewData != dwData)
				dwData = dwNewData;
		}
	}

	return pDoc->ShowIPCHelp(dwCompRole, rclsidComp, posMouse, dwHelpCmd, pwszHelpFile, dwData);
}

HRESULT CIPCompFrameHook::XOleCompUIMgr::ShowMessage(DWORD dwCompRole,
						REFCLSID rclsidComp, LPOLESTR pwszTitle, LPOLESTR pwszText,
						LPOLESTR pwszHelpFile, DWORD dwHelpContextID,
						OLEMSGBUTTON msgbtn, OLEMSGDEFBUTTON msgdefbtn,
						OLEMSGICON msgicon, BOOL fSysAlert, LONG *pnResult)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleCompUIMgr)
	USES_CONVERSION;

	*pnResult = 0;
	ASSERT(pThis->m_srpMainComp != NULL);
	
	COleStrPtr srpwszNewTitle;
	COleStrPtr srpwszNewText;
	COleStrPtr srpwszNewHelpFile;
	CLSID clsid = rclsidComp;
	HRESULT hr = S_OK;

	// need to call main component's GetCntrMessage() method to see if the
	// main component want to override anything about the message.
	if (dwCompRole != OLEROLE_MAINCOMPONENT	&& dwCompRole != OLEROLE_COMPONENTHOST)
	{
		hr = pThis->m_srpMainComp->GetCntrMessage(&dwCompRole, &clsid,
					pwszTitle, pwszText, pwszHelpFile,
					&srpwszNewTitle, &srpwszNewText, &srpwszNewHelpFile,
					&dwHelpContextID, &msgbtn, &msgdefbtn, &msgicon,
					&fSysAlert);

		// Note that the main component will return S_FALSE to
		// prevent any message from being displayed.

		//$CONSIDER -- if GetCntrMessage returns an error, should we
		// proceed and show the active object's message, or what?
		//
		if(hr == S_FALSE)
		{
			ASSERT(srpwszNewTitle == NULL);
			ASSERT(srpwszNewText == NULL);
			ASSERT(srpwszNewHelpFile == NULL);
			return S_FALSE;		// message suppressed by the MainComponent
		}
		else
		{
			// check if MainComponent overrode any of the strings.
			if (srpwszNewTitle != NULL)
				pwszTitle = srpwszNewTitle;
			if (srpwszNewText != NULL)
				pwszText = srpwszNewText;
			if (srpwszNewHelpFile != NULL)
				pwszHelpFile = srpwszNewHelpFile;
		}
	}

	// need to translate our OLEMSGBTN to MB_xxx, and do W2A (OLESTR2TCHAR - see ScotG if this function
	// is not there), then call theApp.DoMessageBox
	// buttons:
	//
	UINT dwStyle = 0;
	const UINT cchMsgMax = 2000;
	TCHAR szPrompt[cchMsgMax];

	switch (msgbtn)
	{
	case OLEMSGBUTTON_OK:
		dwStyle = MB_OK;
		break;
	case OLEMSGBUTTON_OKCANCEL:
		dwStyle = MB_OKCANCEL;
		break;
	case OLEMSGBUTTON_ABORTRETRYIGNORE:
		dwStyle = MB_ABORTRETRYIGNORE;
		break;
	case OLEMSGBUTTON_YESNOCANCEL:
		dwStyle = MB_YESNOCANCEL;
		break;
	case OLEMSGBUTTON_YESNO:
		dwStyle = MB_YESNO;
		break;
	case OLEMSGBUTTON_RETRYCANCEL:
		dwStyle = MB_RETRYCANCEL;
		break;
	case OLEMSGBUTTON_YESALLNOCANCEL:
		ASSERT(FALSE);
		dwStyle = MB_YESNOCANCEL;
		break;
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	switch (msgdefbtn)
	{
	case OLEMSGDEFBUTTON_FIRST:
		dwStyle |= MB_DEFBUTTON1;
		break;
	case OLEMSGDEFBUTTON_SECOND:
		dwStyle |= MB_DEFBUTTON2;
		break;
	case OLEMSGDEFBUTTON_THIRD:
		dwStyle |= MB_DEFBUTTON3;
		break;
	case OLEMSGDEFBUTTON_FOURTH:
		ASSERT(FALSE);
		dwStyle |= MB_DEFBUTTON3;
		break;
	}

	switch (msgicon)
	{
	case OLEMSGICON_NOICON:
		break;
	case OLEMSGICON_CRITICAL:
		dwStyle |= MB_ICONSTOP;
		break;
	case OLEMSGICON_QUERY:
		dwStyle |= MB_ICONQUESTION;
		break;
	case OLEMSGICON_WARNING:
		dwStyle |= MB_ICONEXCLAMATION;
		break;
	case OLEMSGICON_INFO:
		dwStyle |= MB_ICONINFORMATION;
		break;
	}

	if (pwszTitle == NULL)
		{
		return E_INVALIDARG;
		}

	lstrcpyn(szPrompt, OLE2T(pwszTitle), cchMsgMax);

	if (pwszText != NULL)
		{
		ASSERT(cchMsgMax > (lstrlen(szPrompt) + 2 + lstrlen(OLE2T(pwszText))));
		lstrcat(szPrompt, TEXT("\n\n"));
		lstrcat(szPrompt, OLE2T(pwszText));
		}

	*pnResult = AfxMessageBox(szPrompt, dwStyle);

	return S_OK;
}

IMPLEMENT_ADDREF		(CIPCompFrameHook, OleCompUIMgr)
IMPLEMENT_RELEASE		(CIPCompFrameHook, OleCompUIMgr)
IMPLEMENT_QUERYINTERFACE(CIPCompFrameHook, OleCompUIMgr)

/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleInplaceComponentUIManager

HRESULT CIPCompFrameHook::XOleIPCompUIMgr::UIActivateForMe(DWORD dwCompRole,
						REFCLSID rclsidActive, IOleInPlaceActiveObject *pIPActObj,
						IMsoCommandTarget *pCmdTrgtActive,
	        			ULONG cCmdGrpId, LONG *rgnCmdGrpId)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr)

#ifdef UNDONE
	// Remove custom commandbars for current active object
	if (m_cCmdGrpId > 0)
	{
		ShowCustomCommandbars(FALSE);

		m_cCmdGrpId = 0;
		Assert(m_prgnCmdGrpId != NULL);
		delete m_prgnCmdGrpId;
		m_prgnCmdGrpId = NULL;
	}
#endif

	// clear everything first
	pThis->m_dwCompRole = OLEROLE_UNKNOWN;
	pThis->m_srpActiveCmdTrgt.SRelease();
//	m_cmdActive.SetCmdTrgt(NULL);

	pThis->m_xOleInPlaceFrame.SetActiveObject(pIPActObj, NULL);

	ASSERT(dwCompRole != OLEROLE_COMPONENTHOST &&
		   dwCompRole != OLEROLE_UNKNOWN);

	// let's be robust in case a stupid component is confused
	// to think that it is a host or unknown. we will treat them
	// as a component control. (this is prompted by IStudio bug #5243)
	if(!(dwCompRole != OLEROLE_COMPONENTHOST &&
		   dwCompRole != OLEROLE_UNKNOWN))
		dwCompRole = OLEROLE_COMPONENTCONTROL;
	
	pThis->m_dwCompRole = dwCompRole;
	pThis->m_clsidActive = rclsidActive;
	pThis->m_srpActiveCmdTrgt = pCmdTrgtActive;
	if (pThis->m_srpActiveCmdTrgt != NULL)
		pThis->m_srpActiveCmdTrgt->AddRef();
	//
	// If invoked from a context menu then the Pre ContextMenu CmdTrgt, is
	// no longer valid.  We need to prevent the pending WM_RESETCOMMANDTARGET
	// from reseting to the invalid pre-ContextMenu CmdTrgt.  This is done
	// by Releasing m_srpPreContextMenuCmdTrgt
	pThis->m_srpPreContextMenuCmdTrgt.SRelease();

//	m_cmdActive.SetCmdTrgt(m_srpActiveCmdTrgt);

#ifdef UNDONE
	// Show custom commandbars for active object
	if (m_srpActiveObject &&
		(m_dwCompRole == OLEROLE_MAINCOMPONENT ||
		 !(m_grfComp & OLECOMPFLAG_INHIBITNESTEDCOMPUI)))
		{
		// Either this is the main component, or the main
		// component has not prohibited subcomponents from
		// putting up custom menus and toolbars.

		// commandbars are cached to track activation changes.
		Assert(m_cCmdGrpId==0);
		Assert(m_prgnCmdGrpId==NULL);
		if (cCmdGrpId >0)
			{
			Try
				m_cCmdGrpId = cCmdGrpId;
				m_prgnCmdGrpId = new LONG[m_cCmdGrpId];
				PvCpyPvCb(m_prgnCmdGrpId, rgnCmdGrpId, (sizeof(LONG)*m_cCmdGrpId));
				
				ShowCustomCommandbars(TRUE);
			CatchCuexNull
				m_cCmdGrpId = 0;
				m_prgnCmdGrpId = NULL;
			EndCatch
			}
		}
	//$UNDONE only the success case should return NOERROR.
#endif

	theApp.m_bMenuDirty = TRUE;
	return S_OK;
}

HRESULT CIPCompFrameHook::XOleIPCompUIMgr::UpdateUI(DWORD dwCompRole, BOOL fImmediateUpdate,
						DWORD dwReserved)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr);

	//$UNDONE -- what about fImmediate
	theApp.m_bMenuDirty = TRUE;
	return NOERROR;
}

HRESULT CIPCompFrameHook::XOleIPCompUIMgr::SetActiveUI(DWORD dwCompRole, REFCLSID rclsid,
						ULONG cCmdGrpId, LONG *rgnCmdGrpId)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr)

	if ((pThis->m_dwCompRole == OLEROLE_MAINCOMPONENT ||
		 !(pThis->m_grfComp & OLECOMPFLAG_INHIBITNESTEDCOMPUI)))
	{
		// Either this is the main component, or the main
		// component has not prohibited subcomponents from
		// putting up custom menus and toolbars.
		//
		//$UNDONE - install the custom menus and toolbars.
		
		//$UNDONE only the success case should return NOERROR.
			
	}
	return S_OK;
}

void CIPCompFrameHook::XOleIPCompUIMgr::OnUIComponentEnterState(DWORD dwCompRole,
						DWORD dwStateId, DWORD dwReserved)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr)

	if (pThis->m_fEnteringState)
		return;

	pThis->m_fEnteringState = TRUE;
	pThis->m_dwRoleEnteringState = dwCompRole;
	
	// Announce the state change to the component manager.
	CMainFrame * pMainFrm = (CMainFrame*) AfxGetMainWnd();
	ASSERT(pMainFrm != NULL);
	if (pMainFrm->GetComponentMgr() != NULL)
		pMainFrm->GetComponentMgr()->OnComponentEnterState(pMainFrm->GetComponentID(),
			dwStateId, msoccontextAll, 0, NULL, 0);

	ASSERT(pMainFrm->m_pIPCmpMainFrm != NULL);
	IMsoComponent * pMsoComp = pMainFrm->m_pIPCmpMainFrm->GetIMsoComponent();
	ASSERT(pMsoComp != NULL);
	pMsoComp->OnEnterState(dwStateId, TRUE);
	pThis->m_fEnteringState = FALSE;
}

BOOL CIPCompFrameHook::XOleIPCompUIMgr::FOnUIComponentExitState(DWORD dwCompRole,
						DWORD dwStateId, DWORD dwReserved)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr);

	BOOL fRet = FALSE;
	// Announce the state change to the component manager.
	CMainFrame * pMainFrm = (CMainFrame*) AfxGetMainWnd();
	if (pMainFrm->GetComponentMgr() != NULL)
	{
		fRet = pMainFrm->GetComponentMgr()->FOnComponentExitState(pMainFrm->GetComponentID(),
			dwStateId, msoccontextAll, 0, NULL);

		if (!fRet)
		{
			ASSERT(pMainFrm->m_pIPCmpMainFrm != NULL);
			IMsoComponent * pMsoComp = pMainFrm->m_pIPCmpMainFrm->GetIMsoComponent();
			ASSERT(pMsoComp != NULL);
			pMsoComp->OnEnterState(dwStateId, FALSE);
		}
	}
	return fRet;
}

BOOL CIPCompFrameHook::XOleIPCompUIMgr::FUIComponentInState(DWORD dwStateId)
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr);

	BOOL fRet = FALSE;

	CMainFrame * pMainFrm = (CMainFrame*) AfxGetMainWnd();
	if (pMainFrm->GetComponentMgr() != NULL)
	{
		fRet = pMainFrm->GetComponentMgr()->FInState(dwStateId, NULL);
	}

	return fRet;
}

BOOL CIPCompFrameHook::XOleIPCompUIMgr::FContinueIdle()
{
	METHOD_PROLOGUE(CIPCompFrameHook, OleIPCompUIMgr);

	BOOL fRet = FALSE;

	CMainFrame * pMainFrm = (CMainFrame*) AfxGetMainWnd();
	if (pMainFrm->GetComponentMgr() != NULL)
	{
		fRet = pMainFrm->GetComponentMgr()->FContinueIdle();
	}

	return fRet;
}

IMPLEMENT_ADDREF		(CIPCompFrameHook, OleIPCompUIMgr)
IMPLEMENT_RELEASE		(CIPCompFrameHook, OleIPCompUIMgr)
IMPLEMENT_QUERYINTERFACE(CIPCompFrameHook, OleIPCompUIMgr)

/////////////////////////////////////////////////////////////////////////////

HRESULT CIPCompFrameHook::SetInPlaceComponent(IUnknown * punkIPComp)	// IUnknown from which to QI for the in-place component
{
	HRESULT hr = NOERROR;
	ASSERT(punkIPComp != NULL);

	//$UNDONE 4.5 -- if we are reactivating a deactivated component, we already have m_srpMainComp.
	// Should we continue to use the same one, or get a new?
	//
	m_srpMainComp.SRelease();
	m_srpMainCmdTrgt.SRelease();

	COleRef<IMsoCommandTarget> srpMainCmdTrgt;
	// we purposely use the real data member rather than a temp variable, because Davinci wants to
	// call us back during UseComponentUIManager to display a warning (if necessary).  If we don't 
	// setup the m_srpMainComp, our ShowMessage code won't work.
	if (FAILED(hr = punkIPComp->QueryInterface(IID_IOleInPlaceComponent,(void **)&m_srpMainComp)))
		goto ErrRtn;

	//	m_iguidMain = iguid;
	// Initialize the component flags, in case the component neglects to do so.
	//
	m_grfComp = 0;

	if (FAILED(hr = m_srpMainComp->UseComponentUIManager(OLEROLE_MAINCOMPONENT,
										 &m_grfComp, &m_xOleCompUIMgr,
										 ((CIPCompContainerItem *)m_pActiveItem)->GetIOleInPlaceComponentSite())))
		goto ErrRtn;

	// Now fetch the command target.  Note that we don't want to get the
	// command target until after we've called UseComponentUIManager, to
	// make sure we don't use that command target until the component
	// has finished initializing.
	//
	if (FAILED(hr = punkIPComp->QueryInterface(IID_IMsoCommandTarget,(void **)&srpMainCmdTrgt)))
		goto ErrRtn;

//	m_cmdMain.SetCmdTrgt(m_scrpMainCmdTrgt);
	m_srpMainCmdTrgt = srpMainCmdTrgt.Disown();
	
ErrRtn:
	if (FAILED(hr))
		m_srpMainComp.SRelease();
		
	return hr;
	}


void CIPCompFrameHook::DoClose()
{
	m_srpMainComp.SRelease();
	m_srpMainCmdTrgt.SRelease();
	m_srpActiveCmdTrgt.SRelease();
	// remove the object from the mainframe list
	((CMainFrame *)AfxGetMainWnd())->RemoveIPCompFrameHook(this);
}


BOOL CIPCompFrameHook::OnPreTranslateMessage(MSG* pMsg)
{
	// An hardwired in-place component gets a chance at accelerators.
	// If it returned the OLECOMPFLAG_ROUTEACTIVEASCNTRCMD flag from
	// UseComponentUIManager, it gets accelerators before the active
	// object; otherwise, it gets them after.
	//
	//$CONSIDER -- if the active object is the main component or a
	// subcomponent, we can omit calling the parent class (which
	// calls the active object's TranslateAccelerator method).
	// Is m_dwCompRole a reliable indicator--i.e. is it set to
	// OLEROLE_UNKNOWN when a non-component activates?
	//
	BOOL fHandled = FALSE;

	if (m_srpMainComp != NULL && (m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD))
	{
		fHandled = (m_srpMainComp->TranslateCntrAccelerator(pMsg) == NOERROR);
	}

	if (!fHandled)
	{
		fHandled = COleFrameHook::OnPreTranslateMessage(pMsg);
	}

	if (!fHandled && m_srpMainComp != NULL && !(m_grfComp & OLECOMPFLAG_ROUTEACTIVEASCNTRCMD))
	{
		fHandled = (m_srpMainComp->TranslateCntrAccelerator(pMsg) == NOERROR);
	}


	return fHandled;
}

BOOL CIPCompFrameHook::OnDocActivate(BOOL bActivate)
{
	ASSERT_VALID(this);

//	m_cmdMain.Invalidate();
//	m_cmdActive.Invalidate();

	// Notify the hardwired component and the active object
	// (if either is present).	Note that if the active
	// object is a main or sub-component, we omit the
	// call to OnDocWindowActivate; that call is only made
	// for OLE objects or component controls.
	//

	if (m_srpMainComp != NULL)
		m_srpMainComp->OnWindowActivate(OLEACTIVATE_DOCWINDOW, bActivate);

	if (m_lpActiveObject != NULL &&
		(m_srpMainComp == NULL ||
		 m_dwCompRole == OLEROLE_COMPONENTCONTROL ||
		 m_dwCompRole == OLEROLE_UNKNOWN))
		m_lpActiveObject->OnDocWindowActivate(bActivate);

	// remove/install the Active HookNotify on the main MDI app frame.
	// it is the responsibility of the container to manage the frame-level
	// ActiveObject pointer through MDI child window activation changes.
	//
	ASSERT_VALID(m_pFrameWnd);
	// make sure window caption gets updated later
	CIPCompFrameHook* pNotifyHook = (CIPCompFrameHook *)m_pActiveItem->m_pInPlaceFrame;
	pNotifyHook->m_pFrameWnd->DelayUpdateFrameTitle();

	// let the document know about our activatation state
	ASSERT(m_pActiveItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)));
	CIPCompDoc *pDoc = ((CIPCompContainerItem *)(m_pActiveItem))->GetDocument();
	if(pDoc != NULL)
		pDoc->OnDocActivate(bActivate);

	
	if (!bActivate)
	{
		// if we're the hook installed on the top-level frame, unhook ourselves.
		// (Because of the ordering, when a new in-place component is created,
		// it registers itself as the top-level hook before it gets MDI activated.)
		//
		if (pNotifyHook->m_pFrameWnd->m_pNotifyHook == this)
		{
			pNotifyHook->m_pFrameWnd->m_pNotifyHook = NULL;
			((CMainFrame *)AfxGetMainWnd())->RemoveIPCompFrameHook(pNotifyHook);
		}
	}
	else
	{
		// rehook top-level frame if necessary (no effect if top-level == doc-level)
		pNotifyHook->m_pFrameWnd->m_pNotifyHook = pNotifyHook;
		// the following will move the pNotifyHook to the head of the Hook list
		((CMainFrame *)AfxGetMainWnd())->RemoveIPCompFrameHook(pNotifyHook);
		((CMainFrame *)AfxGetMainWnd())->AddIPCompFrameHook(pNotifyHook);
	}

	// so the menu will always be rebuilt in CMDIChildWnd::OnMDIActivate
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IServiceProvider methods implementation

HRESULT CIPCompFrameHook::XServiceProvider::QueryService(REFGUID guidService, REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CIPCompFrameHook, ServiceProvider)

	if (ppvObj == NULL)
		return E_INVALIDARG;

	*ppvObj = NULL;
	
	ASSERT(pThis->m_pActiveItem != NULL);
	ASSERT(pThis->m_pActiveItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)));
	
	return ((CIPCompContainerItem *)(pThis->m_pActiveItem))->GetIServiceProvider()
				->QueryService(guidService, riid, ppvObj);
}

IMPLEMENT_ADDREF		(CIPCompFrameHook, ServiceProvider)
IMPLEMENT_RELEASE		(CIPCompFrameHook, ServiceProvider)
IMPLEMENT_QUERYINTERFACE(CIPCompFrameHook, ServiceProvider)

/////////////////////////////////////////////////////////////////////////////
// Component Manager related methods implementation
void CIPCompFrameHook::OnEnterState(ULONG dwStateId, BOOL fEnter)
{
	// if this object didn't start the transition, notify
	// both the active object and the hardwired component.
	// If this object did start the transition,
	if (m_srpMainComp != NULL &&
		(!m_fEnteringState || m_dwRoleEnteringState != OLEROLE_MAINCOMPONENT))
	{
		m_srpMainComp->OnEnterState(dwStateId, fEnter);
	}
}

void CIPCompFrameHook::OnActiveEnableModelss(ULONG dwStateId, BOOL fEnter)
{
	// if this active view holds an active object,
	// and it isn't responsible for starting the transition, then
	// notify active object.  The active object only cares about modal
	// state.
	//
	if (m_lpActiveObject != NULL &&
		dwStateId == msocstateModal &&
		(!m_fEnteringState || m_dwRoleEnteringState != OLEROLE_COMPONENTCONTROL))
	{
		m_lpActiveObject->EnableModeless(!fEnter);
	}
}

void CIPCompFrameHook::OnFrameActivate(BOOL fActivate)
{
	if (m_srpMainComp != NULL)
		m_srpMainComp->OnWindowActivate(OLEACTIVATE_FRAMEWINDOW, fActivate);

	// If this view has an active object that isn't the main hard-
	// wired component, notify it, too.
	//
	if (m_lpActiveObject != NULL && m_dwCompRole != OLEROLE_MAINCOMPONENT)
		m_lpActiveObject->OnFrameWindowActivate(fActivate);
}

BOOL CIPCompFrameHook::FDoIdle(DWORD grfidlef)
{
	if (m_pDocFrameWnd)
	{
		ASSERT_VALID(m_pDocFrameWnd);
		ASSERT(m_pDocFrameWnd->IsKindOf(RUNTIME_CLASS(CPartFrame)));
		m_pDocFrameWnd->OnUpdateFrameTitle(TRUE);	//will put * on any document that's dirty
	}
	
	if (m_srpMainComp != NULL)
		return m_srpMainComp->FDoIdle(grfidlef);

	return FALSE;
}

BOOL CIPCompFrameHook::FQueryClose(BOOL fPromptUser)
{
	BOOL fRet = TRUE;

	if (m_srpMainComp != NULL)
		fRet = m_srpMainComp->FQueryClose(fPromptUser);

	return fRet;
}

//
//
//	BOOL CIPCompFrameHook::ResetCommandTarget
//
//	Description:
//		Should only be called in response to WM_RESETCOMMANDTARGET message
//		Used to reset the command target after the message from a context
//		menu has been handled.
//
//	Arguments:
//		void
//
//	Return (BOOL): TRUE if command target is reset
//
BOOL CIPCompFrameHook::ResetCommandTarget()
{
	// Reset command target iff PreContextMenu CmdTrgt is valid
	if(m_srpPreContextMenuCmdTrgt != NULL)
	{
		// Release current command target and set the new one
		m_srpActiveCmdTrgt.SRelease();
		m_srpActiveCmdTrgt = m_srpPreContextMenuCmdTrgt.Disown();
	}
	return TRUE;
}

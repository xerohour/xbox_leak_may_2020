// outfdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "findtool.h"
#include "filtrdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFiltersOutputDlgTab dialog

CFiltersOutputDlgTab::~CFiltersOutputDlgTab()
{
}

BEGIN_MESSAGE_MAP(CFiltersOutputDlgTab, CDlgTab)
	//{{AFX_MSG_MAP(CFiltersOutputDlgTab)
	ON_BN_CLICKED(IDC_FOT_LOGTOFILE, OnClickedLogToFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersOutputDlgTab message handlers

BOOL CFiltersOutputDlgTab::OnInitDialog()
{
	CDlgTab::OnInitDialog();

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_FOT_LINES, WM_SETFONT, (WPARAM)hfont, FALSE);

	m_pDoc = ((CFiltersTabbedDialog*)GetParent())->m_pDoc;

	if (m_pDoc->m_fShowNestingLevel)
	{
		CheckDlgButton(IDC_FOT_NESTLEVEL, 1);
	}

	if (m_pDoc->m_fShowRawParms)
	{
		CheckDlgButton(IDC_FOT_RAWPARM, 1);
	}

	if (m_pDoc->m_fShowDecodedParms)
	{
		CheckDlgButton(IDC_FOT_DECODEPARM, 1);
	}

	if (m_pDoc->m_fShowRawReturn)
	{
		CheckDlgButton(IDC_FOT_RAWRET, 1);
	}

	if (m_pDoc->m_fShowDecodedReturn)
	{
		CheckDlgButton(IDC_FOT_DECODERET, 1);
	}

	if (m_pDoc->m_fShowOriginTime)
	{
		CheckDlgButton(IDC_FOT_SHOWTIME, 1);
	}

	if (m_pDoc->m_fShowMousePosition)
	{
		CheckDlgButton(IDC_FOT_SHOWPOINT, 1);
	}

	SetDlgItemInt(IDC_FOT_LINES, m_pDoc->GetMaxLines());

	if (m_pDoc->m_fLogToFile)
	{
		CheckDlgButton(IDC_FOT_LOGTOFILE, 1);
		GetDlgItem(IDC_FOT_LOGFILENAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_FOT_LOGFILEAPPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_FOT_LOGFILEOVERWRITE)->EnableWindow(TRUE);
	}
	else
	{
		CheckDlgButton(IDC_FOT_LOGTOFILE, 0);
		GetDlgItem(IDC_FOT_LOGFILENAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_FOT_LOGFILEAPPEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_FOT_LOGFILEOVERWRITE)->EnableWindow(FALSE);
	}

	SetDlgItemText(IDC_FOT_LOGFILENAME, m_pDoc->m_strLogFileName);

	if (m_pDoc->m_fAppendFile)
	{
		CheckDlgButton(IDC_FOT_LOGFILEAPPEND, 1);
	}
	else
	{
		CheckDlgButton(IDC_FOT_LOGFILEOVERWRITE, 1);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFiltersOutputDlgTab::OnClickedLogToFile()
{
	GetDlgItem(IDC_FOT_LOGFILENAME)->EnableWindow(IsDlgButtonChecked(IDC_FOT_LOGTOFILE));
	GetDlgItem(IDC_FOT_LOGFILEAPPEND)->EnableWindow(IsDlgButtonChecked(IDC_FOT_LOGTOFILE));
	GetDlgItem(IDC_FOT_LOGFILEOVERWRITE)->EnableWindow(IsDlgButtonChecked(IDC_FOT_LOGTOFILE));
}

BOOL CFiltersOutputDlgTab::ValidateTab()
{
	BOOL fTranslated;
	int cLinesMax = (int)GetDlgItemInt(IDC_FOT_LINES, &fTranslated, TRUE);

	if (!fTranslated || cLinesMax < 1 || cLinesMax > MAXLINESMAX)
	{
		SpyMessageBox(IDS_MSG_INVALIDMAXLINES);
		GetDlgItem(IDC_FOT_LINES)->SetFocus();
		return FALSE;
	}

	CHAR szTmp[_MAX_PATH];
	if (IsDlgButtonChecked(IDC_FOT_LOGTOFILE))
	{
		HANDLE hFile;

		if (!GetDlgItemText(IDC_FOT_LOGFILENAME, szTmp, sizeof(szTmp)))
		{
			SpyMessageBox(IDS_MSG_NOLOGFILE);
			GetDlgItem(IDC_FOT_LOGFILENAME)->SetFocus();
			return FALSE;
		}
		else if (((m_pDoc->m_strLogFileName != szTmp) && (theApp.m_strlistLogFiles.Find(szTmp) != NULL)) ||
				((m_pDoc->m_strLogFileNameDef == szTmp) && (theApp.m_strlistLogFiles.Find(szTmp) != NULL)))
		{
			// if szTmp != m_strLogFileName then the user has changed the file name since dialog init.  if
			// szTmp == m_strLogFileNameDef then the user has not changed the file name (ever).  we should
			// verify the uniqueness of the name in either of these cases.  if m_strLogFileName == szTmp and
			// m_strLogFileNameDef != szTmp then the user has changed the name before (and it was unique) so
			// we do not need to verify it again (which will fail since file name is already in global list).
			SpyMessageBox(IDS_CANNOTLOGTOFILE);
			GetDlgItem(IDC_FOT_LOGFILENAME)->SetFocus();
			return FALSE;
		}
		else if ((hFile = CreateFile(szTmp, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			SpyMessageBox(IDS_MSG_INVALIDLOGFILE);
			GetDlgItem(IDC_FOT_LOGFILENAME)->SetFocus();
			return FALSE;
		}
		else
			CloseHandle(hFile);
	}

	return TRUE;
}

void CFiltersOutputDlgTab::CommitTab()
{
	BOOL fTranslated;
	int cLinesMax = (int)GetDlgItemInt(IDC_FOT_LINES, &fTranslated, TRUE);

	m_pDoc->m_fShowNestingLevel = IsDlgButtonChecked(IDC_FOT_NESTLEVEL);
	m_pDoc->m_fShowRawParms = IsDlgButtonChecked(IDC_FOT_RAWPARM);
	m_pDoc->m_fShowDecodedParms = IsDlgButtonChecked(IDC_FOT_DECODEPARM);

	//
	// When getting the options for displaying the return
	// values, be sure to keep the return value display
	// use count up to date!
	//

	if (IsDlgButtonChecked(IDC_FOT_RAWRET))
	{
		if (!m_pDoc->m_fShowRawReturn)
		{
			m_pDoc->m_fShowRawReturn = TRUE;
			m_pDoc->m_cShowReturnUseCount++;
		}
	}
	else
	{
		if (m_pDoc->m_fShowRawReturn)
		{
			m_pDoc->m_fShowRawReturn = FALSE;
			m_pDoc->m_cShowReturnUseCount--;
		}
	}

	if (IsDlgButtonChecked(IDC_FOT_DECODERET))
	{
		if (!m_pDoc->m_fShowDecodedReturn)
		{
			m_pDoc->m_fShowDecodedReturn = TRUE;
			m_pDoc->m_cShowReturnUseCount++;
		}
	}
	else
	{
		if (m_pDoc->m_fShowDecodedReturn)
		{
			m_pDoc->m_fShowDecodedReturn = FALSE;
			m_pDoc->m_cShowReturnUseCount--;
		}
	}

	m_pDoc->m_fShowOriginTime = IsDlgButtonChecked(IDC_FOT_SHOWTIME);
	m_pDoc->m_fShowMousePosition = IsDlgButtonChecked(IDC_FOT_SHOWPOINT);
	m_pDoc->m_fLogToFile = IsDlgButtonChecked(IDC_FOT_LOGTOFILE);
	m_pDoc->m_fAppendFile = IsDlgButtonChecked(IDC_FOT_LOGFILEAPPEND);

	CHAR szTmp[_MAX_PATH];
	GetDlgItemText(IDC_FOT_LOGFILENAME, szTmp, sizeof(szTmp));

	if (m_pDoc->m_fLogToFile)
	{
		if (m_pDoc->m_strLogFileName != szTmp)
		{
			// we've already verified that specified file name is unique--if it's different
			// from the previous log file name then delete the previous file name from the
			// global list.
			POSITION pos = theApp.m_strlistLogFiles.Find(m_pDoc->m_strLogFileName);
			if (pos != NULL)
				theApp.m_strlistLogFiles.RemoveAt(pos);
		}

		theApp.m_strlistLogFiles.AddTail(szTmp);
		if (!m_pDoc->m_fAppendFile)
		{
			HANDLE hLoggingFile = CreateFile((LPCSTR)szTmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hLoggingFile != NULL)
				CloseHandle(hLoggingFile);
		}
	}

	m_pDoc->m_strLogFileName = szTmp;

	//
	// Enable/disable hook subclassing depending on whether any
	// message stream views still want to see return values.
	//
	CMsgDoc::EnableSubclass(m_pDoc->m_cShowReturnUseCount ? TRUE : FALSE);

	m_pDoc->SetMaxLines(cLinesMax);

	//
	// If they have selected "Save as Default", copy the
	// current settings to the default settings.
	//
	if (IsDlgButtonChecked(IDC_FOT_SAVE))
	{
		CMsgDoc::m_fShowNestingLevelDef = m_pDoc->m_fShowNestingLevel;
		CMsgDoc::m_fShowRawParmsDef = m_pDoc->m_fShowRawParms;
		CMsgDoc::m_fShowDecodedParmsDef = m_pDoc->m_fShowDecodedParms;
		CMsgDoc::m_fShowRawReturnDef = m_pDoc->m_fShowRawReturn;
		CMsgDoc::m_fShowDecodedReturnDef = m_pDoc->m_fShowDecodedReturn;
		CMsgDoc::m_fShowOriginTimeDef = m_pDoc->m_fShowOriginTime;
		CMsgDoc::m_fShowMousePositionDef = m_pDoc->m_fShowMousePosition;
		CMsgDoc::m_fLogToFileDef = m_pDoc->m_fLogToFile;
		CMsgDoc::m_strLogFileNameDef = m_pDoc->m_strLogFileName;
		CMsgDoc::m_fAppendFileDef = m_pDoc->m_fAppendFile;
		CMsgDoc::m_cLinesMaxDef = cLinesMax;
	}

	CDlgTab::CommitTab();
}

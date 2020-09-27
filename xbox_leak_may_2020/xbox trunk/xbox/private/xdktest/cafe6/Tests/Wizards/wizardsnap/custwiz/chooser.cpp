// chooser.cpp : Implements the CDialogChooser class
//

#include "stdafx.h"
#include "apwiztes.h"
#include "chooser.h"
#include "cstm1dlg.h"
#include "apwizaw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// On construction, set up internal array with pointers to each page.
CDialogChooser::CDialogChooser()
{
	m_pDlgs[0] = NULL;
	
	m_pDlgs[1] = new CCustom1Dlg;
	m_pDlgs[2] = GetDialog(APWZDLG_APPTYPE);
	m_pDlgs[3] = GetDialog(APWZDLG_DATABASE);
	m_pDlgs[4] = GetDialog(APWZDLG_OLE);
	m_pDlgs[5] = GetDialog(APWZDLG_DOCAPPOPTIONS);
	m_pDlgs[6] = GetDialog(APWZDLG_PROJOPTIONS);
	m_pDlgs[7] = GetDialog(APWZDLG_CLASSES);
	m_pDlgs[8] = GetDialog(APWZDLG_DLGAPPOPTIONS);

	m_nCurrDlg = 0;
	m_nTrack = 0;
}
// Remember where the custom pages begin, so we can delete them in
//  the destructor
#define FIRST_CUSTOM_DLG 1

// Set up arrays of indices to simulate behavior of AppWizard (i.e., to have
//  separate "tracks" for an MDI/SDI app and a dialog-based app).
static int nDocTrack[] = {0,1, 2, 3, 4, 5,6,7};
static int nDlgTrack[] = {0,1, 2, 8, 6, 7};
static int* pnTrack[] = {nDocTrack, nDlgTrack};
static int nLast[] = {7, 5};

// The destructor deletes entries in the internal array corresponding to
//  custom pages.
CDialogChooser::~CDialogChooser()
{
	for (int i = FIRST_CUSTOM_DLG; i <= 1/*LAST_DLG*/; i++)
	{
		ASSERT(m_pDlgs[i] != NULL);
		delete m_pDlgs[i];
	}
}

// Use the internal array to determine the next page.
CAppWizStepDlg* CDialogChooser::Next(CAppWizStepDlg* pDlg)
{
	ASSERT(m_nTrack == 0 || m_nTrack == 1);
	ASSERT(0 <= m_nCurrDlg && m_nCurrDlg < nLast[m_nTrack]);
	ASSERT(pDlg == m_pDlgs[(pnTrack[m_nTrack])[m_nCurrDlg]]);

	// If the current page is the "project type" page, the AppWizard "track" may
	//  have changed.
	if (m_nCurrDlg == 1)
	{
		CString strTemp;
		m_nTrack = apwiztesaw.m_Dictionary.Lookup("PROJTYPE_DLG", strTemp) ? 1 : 0;
		SetNumberOfSteps(nLast[m_nTrack]);
	}

	m_nCurrDlg++;

	// If the new page is the "project type" page, don't display the max number
	//  of steps.
	if (m_nCurrDlg == 1)
		SetNumberOfSteps(-1);

	return m_pDlgs[(pnTrack[m_nTrack])[m_nCurrDlg]];
}

// Use the internal array to determine the previous page.
CAppWizStepDlg* CDialogChooser::Back(CAppWizStepDlg* pDlg)
{
	ASSERT(m_nTrack == 0 || m_nTrack == 1);
	ASSERT(1 <= m_nCurrDlg && m_nCurrDlg <= nLast[m_nTrack]);
	ASSERT(pDlg == m_pDlgs[(pnTrack[m_nTrack])[m_nCurrDlg]]);

	m_nCurrDlg--;

	// If the new page is the "project type" page, don't display the max number
	//  of steps.
	if (m_nCurrDlg == 1)
		SetNumberOfSteps(-1);

	return m_pDlgs[(pnTrack[m_nTrack])[m_nCurrDlg]];
}

// WaveInfoPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "WaveInfoPropPg.h"
#include "WavePropPgMgr.h"
#include "Wave.h"
#include "collection.h"
#include "GuidDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveInfoPropPg property page

IMPLEMENT_DYNCREATE(CWaveInfoPropPg, CPropertyPage)

CWaveInfoPropPg::CWaveInfoPropPg() : CPropertyPage(CWaveInfoPropPg::IDD), m_bSaveUndoState(true), m_bInInitialUpdate(false)
{
	//{{AFX_DATA_INIT(CWaveInfoPropPg)
	m_csAuthor = _T("");
	m_csComment = _T("");
	m_csCopyright = _T("");
	m_csSubject = _T("");
	m_dwLength = 0;
	m_dwSampleSize = 0;
	m_dwSampleRate = 0;
	//}}AFX_DATA_INIT

	m_fNeedToDetach = FALSE;
}

CWaveInfoPropPg::~CWaveInfoPropPg()
{
}

void CWaveInfoPropPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveInfoPropPg)
	DDX_Text(pDX, IDC_AUTHOR, m_csAuthor);
	DDX_Text(pDX, IDC_COMMENT, m_csComment);
	DDX_Text(pDX, IDC_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDC_SUBJECT, m_csSubject);
	DDX_Text(pDX, IDC_LENGTH, m_dwLength);
	DDX_Text(pDX, IDC_SAMPLESIZE, m_dwSampleSize);
	DDX_Text(pDX, IDC_SAMPLERATE, m_dwSampleRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaveInfoPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CWaveInfoPropPg)
	ON_EN_CHANGE(IDC_AUTHOR, OnChangeAuthor)
	ON_EN_CHANGE(IDC_COMMENT, OnChangeComment)
	ON_EN_CHANGE(IDC_COPYRIGHT, OnChangeCopyright)
	ON_EN_CHANGE(IDC_SUBJECT, OnChangeSubject)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_AUTHOR, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_COMMENT, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillfocusEdits)
	ON_BN_CLICKED(IDC_EDIT_GUID_BUTTON, OnEditGuidButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveInfoPropPg message handlers

void CWaveInfoPropPg::OnChangeAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_bInInitialUpdate)
		return;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pWave->m_Info.m_csEngineer != m_csAuthor)
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(m_bSaveUndoState && pWaveEditor)
			{
				m_bSaveUndoState = false;
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_AUTHOR)))
				{
					m_bSaveUndoState = true;
					UpdateData(FALSE);
					return;
				}
			}
				
			m_pWave->m_Info.m_csEngineer = m_csAuthor;
			
			m_pWave->SetDirtyFlag();
		}
	}
	
}

void CWaveInfoPropPg::OnChangeComment() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_bInInitialUpdate)
		return;


    ASSERT(m_pWave);
    BOOL bUpdate = UpdateData(TRUE);
	if(bUpdate)
	{		
		if(m_pWave->m_Info.m_csComment != m_csComment)
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(m_bSaveUndoState && pWaveEditor)
			{
				m_bSaveUndoState = false;
				
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_COMMENT)))
				{
					m_bSaveUndoState = true;
					UpdateData(FALSE);
					return;
				}
			}

			m_pWave->m_Info.m_csComment = m_csComment;
			m_pWave->SetDirtyFlag();
		}
	}

}

void CWaveInfoPropPg::OnChangeCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_bInInitialUpdate)
		return;

	ASSERT(m_pWave);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pWave->m_Info.m_csCopyright != m_csCopyright)
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(m_bSaveUndoState && pWaveEditor)
			{
				m_bSaveUndoState = false;
				
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_COPYRIGHT)))
				{
					m_bSaveUndoState = true;
					UpdateData(FALSE);
					return;
				}
			}

			m_pWave->m_Info.m_csCopyright = m_csCopyright;
			m_pWave->SetDirtyFlag();
		}
	}
	
}

void CWaveInfoPropPg::OnChangeSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_bInInitialUpdate)
		return;

	ASSERT(m_pWave);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pWave->m_Info.m_csSubject != m_csSubject)
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(m_bSaveUndoState && pWaveEditor)
			{
				m_bSaveUndoState = false;
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_SUBJECT)))
				{
					m_bSaveUndoState = true;
					UpdateData(FALSE);
					return;
				}
			}

			m_pWave->m_Info.m_csSubject = m_csSubject;
			
			m_pWave->SetDirtyFlag();
		}
	}
	
}

BOOL CWaveInfoPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (m_pWave)
    {
//        EnableControls(TRUE);

	    m_csCopyright = m_pWave->m_Info.m_csCopyright;
	    m_csAuthor = m_pWave->m_Info.m_csEngineer;
	    m_csSubject = m_pWave->m_Info.m_csSubject;
	    m_csComment = m_pWave->m_Info.m_csComment;
        m_dwLength = m_pWave->m_dwWaveLength; 

        m_dwSampleRate = m_pWave->m_rWaveformat.nSamplesPerSec;
        m_dwSampleSize = m_pWave->m_rWaveformat.wBitsPerSample;
    }
    else
    {
//        EnableControls(FALSE);
    }

	CWavePropPgMgr::dwLastSelPage = WAVE_INFO_PAGE;

	return CPropertyPage::OnSetActive();
}

int CWaveInfoPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if(!FromHandlePermanent(m_hWnd))
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );

		m_fNeedToDetach = TRUE;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

    //initialize the values for the dialog.
    ASSERT(m_pWave);
    if (m_pWave)
    {
	    m_csCopyright = m_pWave->m_Info.m_csCopyright;
	    m_csAuthor = m_pWave->m_Info.m_csEngineer;
	    m_csSubject = m_pWave->m_Info.m_csSubject;
	    m_csComment = m_pWave->m_Info.m_csComment;
        m_dwLength = m_pWave->m_dwWaveLength; 

        m_dwSampleRate = m_pWave->m_rWaveformat.nSamplesPerSec;
        m_dwSampleSize = m_pWave->m_rWaveformat.wBitsPerSample;
    }

	return 0;
}

void CWaveInfoPropPg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd != NULL )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
		
}

void CWaveInfoPropPg::OnKillfocusEdits() 
{
	m_bSaveUndoState = true;
}

BOOL CWaveInfoPropPg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CWnd* pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
		((CEdit*)pCtrl)->SetLimitText(COMMENT_TEXT_LIMIT);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaveInfoPropPg::InitializeDialogValues()
{
	m_bInInitialUpdate = true;

	m_csCopyright = m_pWave->m_Info.m_csCopyright;
	m_csAuthor = m_pWave->m_Info.m_csEngineer;
	m_csSubject = m_pWave->m_Info.m_csSubject;
	m_csComment = m_pWave->m_Info.m_csComment;
    m_dwLength = m_pWave->m_dwWaveLength; 

    m_dwSampleRate = m_pWave->m_rWaveformat.nSamplesPerSec;
    m_dwSampleSize = m_pWave->m_rWaveformat.wBitsPerSample;

	CWnd* pCtrl = GetDlgItem(IDC_SUBJECT);
	if(pCtrl)
	{
		pCtrl->SetWindowText(m_csSubject);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_AUTHOR);
	if(pCtrl)
	{
		pCtrl->SetWindowText(m_csAuthor);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_COPYRIGHT);
	if(pCtrl)
	{
		pCtrl->SetWindowText(m_csCopyright);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
	{
		pCtrl->SetWindowText(m_csComment);
		pCtrl = NULL;
	}
	
	CString sWindowText;
	sWindowText.Format("%d", m_dwLength);
	pCtrl = GetDlgItem(IDC_LENGTH);
	if(pCtrl)
	{
		pCtrl->SetWindowText(sWindowText);
		pCtrl = NULL;
	}

	sWindowText.Format("%d", m_dwSampleRate);
	pCtrl = GetDlgItem(IDC_SAMPLERATE);
	if(pCtrl)
	{
		pCtrl->SetWindowText(sWindowText);
		pCtrl = NULL;
	}

	sWindowText.Format("%d", m_dwSampleSize);
	pCtrl = GetDlgItem(IDC_SAMPLESIZE);
	if(pCtrl)
	{
		pCtrl->SetWindowText(sWindowText);
		pCtrl = NULL;
	}

	m_bInInitialUpdate = false;
}

void CWaveInfoPropPg::OnEditGuidButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( m_pWave)
	{
		CGuidDlg dlg;
		dlg.m_guid = m_pWave->GetFileGUID();
		if(dlg.DoModal() == IDOK)
		{
			//m_pCollection->m_pUndoMgr->SaveState( m_pCollection, theApp.m_hInstance, IDS_UNDO_STYLE_GUID );
			m_pWave->SetFileGUID(dlg.m_guid);
			m_pWave->NotifyWaveChange(true);
		}
	}
}

// DlgMIDIExport.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMIDIExport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMIDIExport dialog


CDlgMIDIExport::CDlgMIDIExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMIDIExport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMIDIExport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_fLeadInMeasureMIDIExport = FALSE;
}


void CDlgMIDIExport::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMIDIExport)
	DDX_Control(pDX, IDC_LEADIN_MEASURE, m_radioLeadInMeasure);
	DDX_Control(pDX, IDC_FIRST_MEASURE, m_radioFirstMeasure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMIDIExport, CDialog)
	//{{AFX_MSG_MAP(CDlgMIDIExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMIDIExport message handlers

BOOL CDlgMIDIExport::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();
	
	m_radioLeadInMeasure.SetCheck( m_fLeadInMeasureMIDIExport );
	m_radioFirstMeasure.SetCheck( !m_fLeadInMeasureMIDIExport );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMIDIExport::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_fLeadInMeasureMIDIExport = m_radioLeadInMeasure.GetCheck();
	
	CDialog::OnOK();
}

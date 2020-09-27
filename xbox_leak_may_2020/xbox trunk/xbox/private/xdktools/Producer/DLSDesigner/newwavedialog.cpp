// newwavedialog.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "newwavedialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CArray <DWORD, DWORD> CNewWaveDialog::m_arraySampleRate;
CString CNewWaveDialog::m_sLastSelectedRate = "22050";
bool	CNewWaveDialog::m_bLastSelected8Bit = false;
bool	CNewWaveDialog::m_bLastSelectedStereo = false;


/////////////////////////////////////////////////////////////////////////////
// CNewWaveDialog dialog

CNewWaveDialog::CNewWaveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewWaveDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewWaveDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewWaveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWaveDialog)
	DDX_Control(pDX, IDC_SAMPLE_RATE_COMBO, m_SampleRateCombo);
	DDX_Control(pDX, IDC_MONO_RADIOBTN, m_MonoButton);
	DDX_Control(pDX, IDC_8BITSAMPLE_RADIOBTN, m_8BitSampleButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWaveDialog, CDialog)
	//{{AFX_MSG_MAP(CNewWaveDialog)
	ON_BN_CLICKED(IDC_16BITSAMPLE_RADIOBTN, On16bitsampleRadiobtn)
	ON_BN_CLICKED(IDC_STEREO_RADIOBTN, OnStereoRadiobtn)
	ON_BN_CLICKED(IDC_8BITSAMPLE_RADIOBTN, On8bitsampleRadiobtn)
	ON_BN_CLICKED(IDC_MONO_RADIOBTN, OnMonoRadiobtn)
	ON_CBN_KILLFOCUS(IDC_SAMPLE_RATE_COMBO, OnKillfocusSampleRateCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWaveDialog message handlers

BOOL CNewWaveDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Init the array if it's empty
	if(m_arraySampleRate.GetSize() == 0)
	{
		for(int nIndex = 0; nIndex < DEFAULT_SAMPLERATE_COUNT; nIndex++)
		{
			DWORD dwSampleRate = atoi(arrDefaultSampleRates[nIndex]);
			m_arraySampleRate.Add(dwSampleRate);
		}
	}

	// Init the sample rates combo
	int nCount = m_arraySampleRate.GetSize();
	for(int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CString sSampleRate;
		sSampleRate.Format("%d", m_arraySampleRate[nIndex]);
		m_SampleRateCombo.AddString(sSampleRate);
	}

	// Set the last selected SampleRate, SampleSize and Channel selection
	int nLastRateIndex = m_SampleRateCombo.FindStringExact(0, m_sLastSelectedRate);
	if(nLastRateIndex == CB_ERR)
	{
		nLastRateIndex = m_SampleRateCombo.AddString(m_sLastSelectedRate);
	}
	m_SampleRateCombo.SelectString(0, m_sLastSelectedRate);

	SetSampleSizeButtons();
	SetChannelButtons();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CNewWaveDialog::IsValidSampleRate(DWORD& dwSampleRate)
{
	CString sSampleRate = "";
	m_SampleRateCombo.GetWindowText(sSampleRate);
	dwSampleRate = atoi((LPCSTR)sSampleRate);

	if(dwSampleRate < 3000 || dwSampleRate > 88000)
	{
		dwSampleRate = dwSampleRate < 3000 ? 3000 : 88000;
		CString sSampleRate;
		sSampleRate.Format("%d", dwSampleRate);
		m_SampleRateCombo.SetWindowText(sSampleRate);
		return FALSE;
	}

	return TRUE;
}

void CNewWaveDialog::OnOK() 
{
	
	m_bLastSelected8Bit = (m_8BitSampleButton.GetCheck() != 0);
	m_bLastSelectedStereo = (m_MonoButton.GetCheck() == 0);

	DWORD dwSampleRate = 0;
	if(IsValidSampleRate(dwSampleRate) == false)
		return;

	m_sLastSelectedRate.Format("%d", dwSampleRate);

	
	CDialog::OnOK();
}

void CNewWaveDialog::On16bitsampleRadiobtn() 
{
	m_bLastSelected8Bit = false;
	SetSampleSizeButtons();
}

void CNewWaveDialog::OnStereoRadiobtn() 
{
	m_bLastSelectedStereo = true;
	SetChannelButtons();
}

void CNewWaveDialog::SetSampleSizeButtons()
{
	m_8BitSampleButton.SetCheck(m_bLastSelected8Bit);
	CButton* pButton = (CButton*)GetDlgItem(IDC_16BITSAMPLE_RADIOBTN);
	pButton->SetCheck(!m_bLastSelected8Bit);
}

void CNewWaveDialog::SetChannelButtons()
{
	m_MonoButton.SetCheck(!m_bLastSelectedStereo);
	CButton* pButton = (CButton*)GetDlgItem(IDC_STEREO_RADIOBTN);
	pButton->SetCheck(m_bLastSelectedStereo);
}

void CNewWaveDialog::On8bitsampleRadiobtn() 
{
	m_bLastSelected8Bit = true;
	SetSampleSizeButtons();
}

void CNewWaveDialog::OnMonoRadiobtn() 
{
	m_bLastSelectedStereo = false;
	SetChannelButtons();
}

void CNewWaveDialog::OnKillfocusSampleRateCombo() 
{
	CString sSampleRate = "";
	m_SampleRateCombo.GetWindowText(sSampleRate);
	DWORD dwSampleRate = atoi((LPCSTR)sSampleRate);

	if(dwSampleRate < 3000 || dwSampleRate > 88000)
	{
		dwSampleRate = dwSampleRate < 3000 ? 3000 : 88000;
		CString sSampleRate;
		sSampleRate.Format("%d", dwSampleRate);
		m_SampleRateCombo.SetWindowText(sSampleRate);
	}
}

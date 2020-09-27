// AdpcmLoopWarnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "AdpcmLoopWarnDlg.h"
#include "DLSComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef DMP_XBOX

#define RESAMPLE_SIZE_CHANGE( dwNewSampleRate, dwOldSampleRate, dwOldSize ) ((__int64(dwNewSampleRate) * __int64(dwOldSize) + __int64(dwOldSampleRate)/2)/__int64(dwOldSampleRate))
#define SIZE_CHANGE_TO_RESAMPLE( dwNewSize, dwOldSize, dwOldSampleRate ) ((__int64(dwOldSampleRate) * __int64(dwNewSize) + __int64(dwOldSize)/2) / __int64(dwOldSize))

/////////////////////////////////////////////////////////////////////////////
// CAdpcmLoopWarnDlg dialog


CAdpcmLoopWarnDlg::CAdpcmLoopWarnDlg(CDLSComponent* pComponent, const DWORD dwSampleRate, const DWORD dwLoopStart, const DWORD dwLoopLength, CWnd* pParent /*=NULL*/)
	: CDialog(CAdpcmLoopWarnDlg::IDD, pParent)
{
	ASSERT( pComponent );
	m_pComponent = pComponent;

	m_dwOrigSampleRate = dwSampleRate;
	m_dwOrigLoopStart = dwLoopStart;
	m_dwOrigLoopLength = dwLoopLength;

	DWORD dwNewLoopSize = (dwLoopLength / 64) * 64 + 64;

	m_dwUpsampleSampleRate = DWORD(SIZE_CHANGE_TO_RESAMPLE( dwNewLoopSize, dwLoopLength, m_dwOrigSampleRate ));
	m_dwUpsampleLoopStart = DWORD(RESAMPLE_SIZE_CHANGE( m_dwUpsampleSampleRate, m_dwOrigSampleRate, m_dwOrigLoopStart ));
	m_dwUpsampleLoopLength = dwNewLoopSize;
	m_dwUpsampleInserted = m_dwUpsampleLoopStart % 64;
	if( m_dwUpsampleInserted )
	{
		m_dwUpsampleInserted = 64 - m_dwUpsampleInserted;
	}
	m_dwUpsampleLoopStart += m_dwUpsampleInserted;

	dwNewLoopSize = (dwLoopLength / 64) * 64;
	if( dwNewLoopSize == 0 )
	{
		m_dwDownsampleSampleRate = 0;
		m_dwDownsampleLoopStart = 0;
		m_dwDownsampleLoopLength = 0;
		m_dwDownsampleInserted = 0;
	}
	else
	{
		m_dwDownsampleSampleRate = DWORD(SIZE_CHANGE_TO_RESAMPLE( dwNewLoopSize, dwLoopLength, m_dwOrigSampleRate ));
		m_dwDownsampleLoopStart = DWORD(RESAMPLE_SIZE_CHANGE( m_dwDownsampleSampleRate, m_dwOrigSampleRate, m_dwOrigLoopStart ));
		m_dwDownsampleLoopLength = dwNewLoopSize;
		m_dwDownsampleInserted = m_dwDownsampleLoopStart % 64;
		if( m_dwDownsampleInserted )
		{
			m_dwDownsampleInserted = 64 - m_dwDownsampleInserted;
		}
		m_dwDownsampleLoopStart += m_dwDownsampleInserted;
	}

	m_xbChange = XBADPCM_UPSAMPLE;

	//{{AFX_DATA_INIT(CAdpcmLoopWarnDlg)
	//}}AFX_DATA_INIT
}


void CAdpcmLoopWarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdpcmLoopWarnDlg)
	DDX_Control(pDX, IDC_CHECK_NOT_AGAIN, m_checkNotAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdpcmLoopWarnDlg, CDialog)
	//{{AFX_MSG_MAP(CAdpcmLoopWarnDlg)
	ON_BN_CLICKED(IDC_CHECK_NOT_AGAIN, OnCheckNotAgain)
	ON_BN_DOUBLECLICKED(IDC_CHECK_NOT_AGAIN, OnDoubleclickedCheckNotAgain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdpcmLoopWarnDlg message handlers

void CAdpcmLoopWarnDlg::OnCheckNotAgain() 
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	int nCheck = m_checkNotAgain.GetCheck();
	m_pComponent->SetBadXboxLoopWarning(nCheck == 0 ? false : true);
}

void CAdpcmLoopWarnDlg::OnDoubleclickedCheckNotAgain() 
{
	OnCheckNotAgain();
}

BOOL CAdpcmLoopWarnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemInt( IDC_EDIT_ORIGINAL_RATE, m_dwOrigSampleRate, FALSE );
	SetDlgItemInt( IDC_EDIT_ORIGINAL_LOOP_START, m_dwOrigLoopStart, FALSE );
	SetDlgItemInt( IDC_EDIT_ORIGINAL_LOOP_END, m_dwOrigLoopStart + m_dwOrigLoopLength - 1, FALSE );
	SetDlgItemInt( IDC_EDIT_ORIGINAL_INSERTED, 0, FALSE );

	SetDlgItemInt( IDC_EDIT_UPSAMPLE_RATE, m_dwUpsampleSampleRate, FALSE );
	SetDlgItemInt( IDC_EDIT_UPSAMPLE_LOOP_START, m_dwUpsampleLoopStart, FALSE );
	SetDlgItemInt( IDC_EDIT_UPSAMPLE_LOOP_END, m_dwUpsampleLoopStart + m_dwUpsampleLoopLength - 1, FALSE );
	SetDlgItemInt( IDC_EDIT_UPSAMPLE_INSERTED, m_dwUpsampleInserted, FALSE );

	SetDlgItemInt( IDC_EDIT_DOWNSAMPLE_RATE, m_dwDownsampleSampleRate, FALSE );
	SetDlgItemInt( IDC_EDIT_DOWNSAMPLE_LOOP_START, m_dwDownsampleLoopStart, FALSE );
	SetDlgItemInt( IDC_EDIT_DOWNSAMPLE_LOOP_END, m_dwDownsampleLoopStart + m_dwDownsampleLoopLength - 1, FALSE );
	SetDlgItemInt( IDC_EDIT_DOWNSAMPLE_INSERTED, m_dwDownsampleInserted, FALSE );

	// If downsampling isn't an option
	if( m_dwDownsampleSampleRate == 0 )
	{
		// Disable the downsample button
		CWnd *pWnd = GetDlgItem( IDC_RADIO_DOWNSAMPLE );
		if( pWnd )
		{
			pWnd->EnableWindow( FALSE );
		}
	}

	CheckRadioButton( IDC_RADIO_UPSAMPLE, IDC_RADIO_ORIGINAL, IDC_RADIO_UPSAMPLE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdpcmLoopWarnDlg::OnOK() 
{
	switch( GetCheckedRadioButton( IDC_RADIO_UPSAMPLE, IDC_RADIO_ORIGINAL ) )
	{
	case IDC_RADIO_UPSAMPLE:
		m_xbChange = XBADPCM_UPSAMPLE;
		break;
	case IDC_RADIO_DOWNSAMPLE:
		m_xbChange = XBADPCM_DOWNSAMPLE;
		break;
	case IDC_RADIO_ORIGINAL:
		m_xbChange = XBADPCM_ORIG;
		break;
	}

	CDialog::OnOK();
}
#endif //DMP_XBOX

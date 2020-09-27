// XboxSynthConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "XboxSynthConfigDlg.h"
#include "XboxAddinComponent.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )
#include <dmksctrl.h>
#include <dmpprivate.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXboxSynthConfigDlg dialog


CXboxSynthConfigDlg::CXboxSynthConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXboxSynthConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXboxSynthConfigDlg)
	m_bPCSynth = FALSE;
	m_bXboxSynth = FALSE;
	//}}AFX_DATA_INIT
}


void CXboxSynthConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXboxSynthConfigDlg)
	DDX_Check(pDX, IDC_CHECK_PC_SYNTH, m_bPCSynth);
	DDX_Check(pDX, IDC_CHECK_XBOX_SYNTH, m_bXboxSynth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXboxSynthConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CXboxSynthConfigDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXboxSynthConfigDlg message handlers

void CXboxSynthConfigDlg::OnOK() 
{
    if( !UpdateData(TRUE) )
    {
        return;
    }

    BOOL bOldXboxSynth, bOldPCSynth;

	if( SUCCEEDED( theApp.m_pXboxAddinComponent->DoXboxSynthState( false, &bOldXboxSynth, &bOldPCSynth ) ) )
	{
        CDialog dlgConnecting;
        dlgConnecting.Create( IDD_XBOX_CONNECTING, this );
        dlgConnecting.ShowWindow( SW_SHOW );

        HRESULT hr = theApp.m_pXboxAddinComponent->DoXboxSynthState( true, &m_bXboxSynth, &m_bPCSynth );
        dlgConnecting.DestroyWindow();
        if( FAILED(hr) )
        {
            CString strError;
            if( hr == XBDM_CANNOTCONNECT )
            {
                strError.FormatMessage( IDS_ERR_CANT_CONNECT, theApp.m_strXboxName );
            }
			else if( hr == XBDM_CANNOTACCESS )
			{
				strError.FormatMessage( IDS_ERR_XBSYNTH_AUDCONSOLE_VERSION, theApp.m_strXboxName );
			}
            else
            {
                strError.LoadString( IDS_ERR_SETXBOXSYNTH );
            }
			XboxAddinMessageBox( NULL, strError, MB_ICONERROR | MB_OK );

            theApp.m_pXboxAddinComponent->DoXboxSynthState( true, &bOldXboxSynth, &bOldPCSynth );
            m_bXboxSynth = bOldXboxSynth;
            m_bPCSynth = bOldPCSynth;
            return;
        }

        DWORD dwValue = m_bPCSynth;
        SetUserRegDWORD( TEXT("EnablePCSynth"), &dwValue );
        dwValue = m_bXboxSynth;
        SetUserRegDWORD( TEXT("EnableXboxSynth"), &dwValue );

		// If we just enabled the Xbox synth
		if( !bOldXboxSynth && m_bXboxSynth )
		{
			// Re-download everything
			IDMUSProdConductorPrivate *pIDMUSProdConductorPrivate;
			if( SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIConductor->QueryInterface( IID_IDMUSProdConductorPrivate, (void**)&pIDMUSProdConductorPrivate ) ) )
			{
				pIDMUSProdConductorPrivate->PleaseRedownload();
				pIDMUSProdConductorPrivate->Release();
			}
		}
	}

	CDialog::OnOK();
}

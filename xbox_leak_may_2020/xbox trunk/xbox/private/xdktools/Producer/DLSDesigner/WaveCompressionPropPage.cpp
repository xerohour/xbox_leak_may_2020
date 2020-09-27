// WaveCompressionPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "DLSDesignerDLL.h"

#include "dlsdesigner.h"
#include "WavePropPgMgr.h"
#include "Collection.h"
#include "WavENode.h"
#include "Wave.h"
#include "MonoWave.h"
#include "WaveCompressionManager.h"
#include "WaveCompressionPropPage.h"
#include "ProgressDialog.h"
#include "AdpcmLoopWarnDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveCompressionPropPage property page

IMPLEMENT_DYNCREATE(CWaveCompressionPropPage, CPropertyPage)

int CWaveCompressionPropPage::m_nLastCompressionType = 0;
int CWaveCompressionPropPage::m_nLastCompressionAttribute = 0;

CWaveCompressionPropPage::CWaveCompressionPropPage() : CPropertyPage(CWaveCompressionPropPage::IDD),
m_pWave(NULL),
m_bTypeCBClosed(true),	
m_bAttributesCBClosed(true),
m_fNeedToDetach(FALSE),
m_hConversionEvent(0)
{
	//{{AFX_DATA_INIT(CWaveCompressionPropPage)
	//}}AFX_DATA_INIT

	m_hConversionEvent = ::CreateEvent(NULL, TRUE, FALSE, "Conversion Complete");
}

CWaveCompressionPropPage::~CWaveCompressionPropPage()
{
	::CloseHandle(m_hConversionEvent);
}

void CWaveCompressionPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveCompressionPropPage)
	DDX_Control(pDX, IDC_NOPREROLL_CHECK, m_NoPrerollCheck);
	DDX_Control(pDX, IDC_DECOMPRESSEDDATASTART_STATIC2, m_DecompressedStartLabel2);
	DDX_Control(pDX, IDC_DECOMPRESSEDDATASTART_STATIC, m_DecompressedStartLabel);
	DDX_Control(pDX, IDC_DECOMPRESSEDDATASTART_EDIT, m_DecompressedStartEdit);
	DDX_Control(pDX, IDC_MS_STATIC, m_MSLabel);
	DDX_Control(pDX, IDC_READ_AHEAD_STATIC, m_ReadAheadLabel);
	DDX_Control(pDX, IDC_STREAM_CHECK, m_StreamCheck);
	DDX_Control(pDX, IDC_READAHEAD_SPIN, m_ReadAheadSpin);
	DDX_Control(pDX, IDC_READAHEAD_SAMPLES_STATIC, m_ReadAheadStatic);
	DDX_Control(pDX, IDC_READAHEAD_EDIT, m_ReadAheadEdit);
	DDX_Control(pDX, IDC_WAVE_SIZE, m_WaveSize);
	DDX_Control(pDX, IDC_COMPRESSION_RATIO, m_CompressionRatio);
	DDX_Control(pDX, IDC_COMBO1, m_TypeComboBox);
	DDX_Control(pDX, IDC_COMBO2, m_AttributesComboBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWaveCompressionPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWaveCompressionPropPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_COMPRESSED, OnCompressed)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCloseupCombo1)
	ON_CBN_CLOSEUP(IDC_COMBO2, OnCloseupCombo2)
	ON_CBN_DROPDOWN(IDC_COMBO2, OnDropdownCombo2)
	ON_CBN_DROPDOWN(IDC_COMBO1, OnDropdownCombo1)
	ON_BN_CLICKED(IDC_STREAM_CHECK, OnStreamCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_READAHEAD_SPIN, OnDeltaposReadaheadSpin)
	ON_EN_KILLFOCUS(IDC_READAHEAD_EDIT, OnKillfocusReadaheadEdit)
	ON_EN_KILLFOCUS(IDC_DECOMPRESSEDDATASTART_EDIT, OnKillfocusDecompressedStartEdit)
	ON_BN_CLICKED(IDC_NOPREROLL_CHECK, OnNoprerollCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveCompressionPropPage message handlers and other functions

void CWaveCompressionPropPage::SetObject(CWave* pWave) 
{
	m_pWave = pWave;
}

int CWaveCompressionPropPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	// create decompressed data start spinner
	// Note: I use UDS_SETBUDDYINT, because I set the max of the spinner to UD_MAXVAL (about 32K), even if the wave is bigger
	CRect rc;
	if (m_DecompressedStartSpin.Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT | UDS_SETBUDDYINT, rc, this, IDC_DECOMPRESSEDDATASTART_SPIN) == 0)
		return -1;

	return 0;
}

void CWaveCompressionPropPage::OnDestroy() 
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

	ClearTypeComboBox();
	ClearAttributesComboBox();

	CPropertyPage::OnDestroy();
}

BOOL CWaveCompressionPropPage::OnInitDialog()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CPropertyPage::OnInitDialog();
	
	if(m_pWave)
	{
		// compression driver
		CWave::m_CompressionManager.GetSupportedDrivers();
		FillTypeComboBox();
	}

	// position up-down control
	m_DecompressedStartSpin.SetWindowPos(&m_DecompressedStartEdit, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	m_DecompressedStartSpin.SetBuddy(&m_DecompressedStartEdit);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaveCompressionPropPage::FillTypeComboBox()
{
	int nDrivers = CWave::m_CompressionManager.m_arrFormatTags.GetSize();

	for(int nCount = 0; nCount < nDrivers; nCount++)
	{
		FORMAT_TAG_INF0* pFormatInfo = (FORMAT_TAG_INF0*)CWave::m_CompressionManager.m_arrFormatTags[nCount];
		ASSERT(pFormatInfo);

		// Get the wave node to check the format. We will not display the WMA format for stereo waves, not in a collection
		if (m_pWave)
			{
			CWaveNode* pWaveNode = m_pWave->GetNode();
			if ((NULL != pWaveNode) && 
				(false == pWaveNode->IsInACollection()) &&
				(MSAUDIO_FORMAT_TAG == pFormatInfo->dwFormatTag || WMAUDIO_FORMAT_TAG == pFormatInfo->dwFormatTag))
				continue;
			}

		int nIndex = m_TypeComboBox.AddString(pFormatInfo->sDriverName.GetBuffer(0));
		pFormatInfo->sDriverName.ReleaseBuffer();
		if(nIndex != CB_ERR)
		{
			int nResult  = m_TypeComboBox.SetItemDataPtr(nIndex, pFormatInfo);
			if(nResult == CB_ERR)
			{
				m_TypeComboBox.DeleteString(nIndex);
			}
		}
	}
}

BOOL CWaveCompressionPropPage::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	InitializeDialogValues();
	return CPropertyPage::OnSetActive();
}

void CWaveCompressionPropPage::ClearTypeComboBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	int nCount = m_TypeComboBox.GetCount();

	if(nCount > 0) 
	{
		for(int i = 0; i < nCount; i++)
			m_TypeComboBox.DeleteString(i);
	}
}

// Go through the items in the types combo and set the selection to the passed format tag
void CWaveCompressionPropPage::SetSelectedCompressionType(WORD wFormatTag)
{
	int nTypesCount = m_TypeComboBox.GetCount();

	for(int nCount = 0; nCount < nTypesCount; nCount++)
	{
		FORMAT_TAG_INF0* pFormatTagInfo = (FORMAT_TAG_INF0*) m_TypeComboBox.GetItemDataPtr(nCount);
		
		if(pFormatTagInfo && pFormatTagInfo->dwFormatTag == wFormatTag)
		{
			m_TypeComboBox.SetCurSel(nCount);
			FillAttributesComboBox(pFormatTagInfo);
			return;
		}
	}
}


void CWaveCompressionPropPage::SetSelectedFormat(const WAVEFORMATEX* pWaveFormat)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	int nCount = m_AttributesComboBox.GetCount();

	for(int nIndex = 0; nIndex < nCount; nIndex++)
	{
		WAVEFORMATEX* pFormatInfo = (WAVEFORMATEX*) m_AttributesComboBox.GetItemDataPtr(nIndex);
		ASSERT(pFormatInfo);
		if(pFormatInfo == NULL)
		{
			return;
		}

		if(pFormatInfo->wFormatTag == pWaveFormat->wFormatTag &&
			pFormatInfo->wBitsPerSample == pWaveFormat->wBitsPerSample &&
			pFormatInfo->nChannels == pWaveFormat->nChannels &&
			pFormatInfo->nAvgBytesPerSec == pWaveFormat->nAvgBytesPerSec)
		{
			m_AttributesComboBox.SetCurSel(nIndex);
			return;
		}
	}
}



void CWaveCompressionPropPage::ClearAttributesComboBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	int nCount = m_AttributesComboBox.GetCount();
	
	if(nCount > 0)
	{
		for(int i = nCount - 1; i >= 0; i--)
		{
			m_AttributesComboBox.DeleteString(i);
		}
	}
}

void CWaveCompressionPropPage::FillAttributesComboBox(const FORMAT_TAG_INF0* pFormatInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pFormatInfo != (const void *)-1);

	ClearAttributesComboBox();

	int nSupportedFormats = 0;
	if (pFormatInfo != NULL)
		{
		CWave::m_CompressionManager.GetSupportedFormats(pFormatInfo, m_pWave);
		nSupportedFormats = CWave::m_CompressionManager.m_arrSupportedFormats.GetSize();
		}

	if(nSupportedFormats == 0)
	{
		m_AttributesComboBox.AddString("No valid compression");
	}

	for(int nCount = 0; nCount < nSupportedFormats; nCount++)
	{
		SUPPORTED_FORMAT_INFO* pFormatInfo = (SUPPORTED_FORMAT_INFO*) CWave::m_CompressionManager.m_arrSupportedFormats[nCount];

		int nIndex = m_AttributesComboBox.AddString(pFormatInfo->sSupportedFormatName.GetBuffer(0));
		pFormatInfo->sSupportedFormatName.ReleaseBuffer();
		if(nIndex != CB_ERR)
		{
			int nResult = m_AttributesComboBox.SetItemDataPtr(nIndex, pFormatInfo->pWaveFormatEx);
			if(nResult == CB_ERR)
				m_AttributesComboBox.DeleteString(nIndex);
		}
	}

	m_AttributesComboBox.SetCurSel(0);
}


void CWaveCompressionPropPage::OnCompressed() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pWave);
    if (!m_pWave)
    	return;

	CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
	CButton *pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);
	int nChecked = pCheckBox->GetCheck();

	// Save the state for undo/redo
	if(FAILED(m_pWave->SaveUndoState(IDS_UNDO_COMPRESSION)))
	{
		pCheckBox->SetCheck(nChecked ? 0 : 1);
		return;
	}

	if(nChecked)
	{

#ifdef DMP_XBOX
		if( m_pWave->IsLooped() )
		{
			// Try and get the destination format
			WAVEFORMATEX* pwfxDst = NULL;
			int nCurSel = m_AttributesComboBox.GetCurSel();
			if(nCurSel != CB_ERR)
			{
				void* pvoid = m_AttributesComboBox.GetItemDataPtr(nCurSel);
				if(pvoid != (void *)-1 && pvoid != NULL)
				{
					pwfxDst = (LPWAVEFORMATEX)pvoid;
				}
			}

			WLOOP waveLoop = m_pWave->GetWLOOP();
			if( (waveLoop.ulStart % 64) != 0
			||	(waveLoop.ulLength % 64) != 0 )
			{
				// If there is a destination format and we're supposed to show the warning
				if( pwfxDst && !m_pWave->GetNode()->GetComponent()->IsNoShowBadXboxLoopWarning() )
				{
					CAdpcmLoopWarnDlg* pAdpcmLoopWarnDlg = new CAdpcmLoopWarnDlg(m_pWave->GetNode()->GetComponent(), m_pWave->m_rWaveformat.nSamplesPerSec, waveLoop.ulStart, waveLoop.ulLength, this);
					int nRes = pAdpcmLoopWarnDlg->DoModal();
					if( IDOK == nRes )
					{
						// We don't want stereo formats if the wave is not a TRUE stereo wave
						// But we want the stereo formats if it's a stereo wave NOT in a collection
						bool bIsStereo = m_pWave->GetNode()->IsStereo();
						CCollection* pCollection = m_pWave->GetNode()->GetCollection();

						// Modify the sample rate and insert silence
						if( XBADPCM_UPSAMPLE == pAdpcmLoopWarnDlg->m_xbChange )
						{
							CWaitCursor wait;
							m_pWave->Resample( pAdpcmLoopWarnDlg->m_dwUpsampleSampleRate );
							m_pWave->InsertSilence( 0, pAdpcmLoopWarnDlg->m_dwUpsampleInserted );

							// Change destination format to match new sample rate
							CreateXboxAdpcmFormat( (bIsStereo && pCollection == NULL) ? 2 : 1, pAdpcmLoopWarnDlg->m_dwUpsampleSampleRate, (LPIMAADPCMWAVEFORMAT)pwfxDst );
						}
						else if( XBADPCM_DOWNSAMPLE == pAdpcmLoopWarnDlg->m_xbChange )
						{
							CWaitCursor wait;
							m_pWave->Resample( pAdpcmLoopWarnDlg->m_dwDownsampleSampleRate );
							m_pWave->InsertSilence( 0, pAdpcmLoopWarnDlg->m_dwDownsampleInserted );

							// Change destination format to match new sample rate
							CreateXboxAdpcmFormat( (bIsStereo && pCollection == NULL) ? 2 : 1, pAdpcmLoopWarnDlg->m_dwDownsampleSampleRate, (LPIMAADPCMWAVEFORMAT)pwfxDst );
						}
					}
					delete pAdpcmLoopWarnDlg;
					if( IDOK != nRes )
					{
						pCheckBox->SetCheck(nChecked ? 0 : 1);
						return;
					}
				}
			}
		}
#endif // DMP_XBOX

		if (!Compress())
		{
			AfxMessageBox(IDS_ERR_COMPRESSION_FAILED, MB_OK | MB_ICONEXCLAMATION);

LFailed:
			// Couldn't compress successfully so set the compression off and Pop the undo state
            m_pWave->PopUndoState();
			pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED); // window was refreshed
			pCheckBox->SetCheck(nChecked ? 0 : 1);
			EnableDecompressedStartControls();
			return;
		}
        
        // Look at and listen to the Decompressed wave
		if(FAILED(m_pWave->SwitchCompression(true)))
			goto LFailed;

		m_pWave->RevertToAfterCompression();

        if(FAILED(m_pWave->OnWaveBufferUpdated()))
        {
            if(m_pWave->IsLooped())
			{
				m_pWave->RevertToLoopBeforeCompression();
			}

            goto LFailed;
        }
	}
	else
	{
        
	 	// Look at and listen to the original wave now
		if(FAILED(m_pWave->SwitchCompression(false)))
			goto LFailed;

        // Remember the previous loop points
		// Update the loop points if it's looped...
		if(m_pWave->IsLooped())
		{
			m_pWave->RememberLoopAfterCompression();
			m_pWave->RevertToLoopBeforeCompression();
		}
		
        if(FAILED(m_pWave->OnWaveBufferUpdated()))
        {
		    m_pWave->RevertToAfterCompression();
            goto LFailed;
        }
	}

    m_pWave->UpdateLoopInRegions();
	SetCompressionInfo();

	EnableDecompressedStartControls();

	// remember compression setting for other waves
	if (nChecked)
		m_nLastCompressionType = m_TypeComboBox.GetCurSel();
}

void CWaveCompressionPropPage::OnSelchangeCombo1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	BOOL bState = m_TypeComboBox.GetDroppedState();
	if(bState && m_bTypeCBClosed != true)
		return;

	int nCurSel = m_TypeComboBox.GetCurSel();

	// Has the compression entry changed?
	if(nCurSel == CB_ERR)
		return;

	// We need to see what attribute this compression format supports...
	FORMAT_TAG_INF0 *pfti = (FORMAT_TAG_INF0 *)m_TypeComboBox.GetItemDataPtr(nCurSel);
	FillAttributesComboBox(pfti);

	// if wave is not compressed, no more work to do
    if (!m_pWave->m_bCompressed)
    	return;

	// uncompress wave and ready it for new compression
    m_pWave->SaveUndoState(IDS_UNDO_COMPRESSION);
	if(FAILED(m_pWave->SwitchCompression(false)) || FAILED(m_pWave->OnWaveBufferUpdated()))
		{
        m_pWave->PopUndoState();

		// Out of memory??? We failed in updating the wave 
		// So revert back to the old selection
		m_TypeComboBox.SetCurSel(m_nLastCompressionType);
		return;
		}

	m_pWave->UpdateLoopInRegions();

    // turn off compression checkbox
	CButton* pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);
	if(pCheckBox)
		pCheckBox->SetCheck(0);

	m_nLastCompressionType = nCurSel;
	SetCompressionInfo();
}

void CWaveCompressionPropPage::OnSelchangeCombo2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	BOOL bState = m_AttributesComboBox.GetDroppedState();
	if(bState && m_bAttributesCBClosed != true)
		return;

	int nCurSel = m_AttributesComboBox.GetCurSel();

	// Is the wave is already compressed with this attribute?
	if(nCurSel == m_nLastCompressionAttribute && m_pWave->m_bCompressed)
		return;

    m_pWave->SaveUndoState(IDS_UNDO_COMPRESSION);

	// we'll compress if uncompressed, or uncompressed if already compressed (in another format)
	bool bCompress = !m_pWave->m_bCompressed;

	// attempt to compress
	if (bCompress && !Compress())
		{
LFailed:
		if (bCompress)
	        AfxMessageBox(IDS_ERR_COMPRESSION_FAILED, MB_OK | MB_ICONEXCLAMATION);

        m_pWave->PopUndoState();
		m_AttributesComboBox.SetCurSel(m_nLastCompressionAttribute);
        return;
		}

	// switch compression state, download wave
    if (FAILED(m_pWave->SwitchCompression(bCompress)) || FAILED(m_pWave->OnWaveBufferUpdated()))
    	goto LFailed;
    	
    m_pWave->UpdateLoopInRegions();

	// update property page
    m_nLastCompressionAttribute = nCurSel;
    CButton* pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);
    if(pCheckBox)
	    pCheckBox->SetCheck(bCompress ? 1 : 0);
    SetCompressionInfo();
	EnableDecompressedStartControls();
				    
    // Give the focus back to this dropdown
    m_AttributesComboBox.SetFocus();
}

UINT AFX_CDECL UpdateControllingFunction( LPVOID pParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HANDLE* phConversionEvent = (HANDLE*) pParam;

	CProgressDialog progressDlg;
	
	while(WaitForSingleObject(*phConversionEvent, 10) != WAIT_OBJECT_0)
	{
		progressDlg.m_ProgressBar.StepIt();
	}
	
	// Finish it!
	progressDlg.m_ProgressBar.SetPos(100);
	
	// Shoot yourself in the head...
	::AfxEndThread(0);

	return 0;
}



bool CWaveCompressionPropPage::Compress()
{
	ASSERT(m_pWave);

	// There's nothing to compress
	if(m_pWave == NULL)
		return false;

	CWaitCursor wait;
	ResetEvent(m_hConversionEvent);

	int nCurSel = m_AttributesComboBox.GetCurSel();
	if(nCurSel != CB_ERR)
	{
		void* pvoid = m_AttributesComboBox.GetItemDataPtr(nCurSel);
		if(pvoid != (void *)-1 && pvoid != NULL)
		{
			// Start the progress bar thread
			CWinThread* pUpdateThread = ::AfxBeginThread(UpdateControllingFunction, &m_hConversionEvent);
			pUpdateThread->m_bAutoDelete = TRUE;
			
			WAVEFORMATEX* pwfxDst = (LPWAVEFORMATEX)pvoid;

			ASSERT(pwfxDst->wFormatTag != 0);
			if(pwfxDst->wFormatTag == 0)
			{
				return false;
			}

			if(m_pWave->IsLooped())
			{
				m_pWave->RememberLoopBeforeCompression();
			}

			if(SUCCEEDED(m_pWave->Compress(pwfxDst)))
			{
				// Repaint the wave with new values
				CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
				if(pWaveEditor)
				{
					pWaveEditor->InvalidateControl(); 
				}

				// Set the checkbox to compressed...
				CButton *pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);
				if(pCheckBox)
				{
					pCheckBox->SetCheck(1);
				}

				
				::SetEvent(m_hConversionEvent);
				return true;
			}

			// Compression failed
			::SetEvent(m_hConversionEvent);
						
			CButton *pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);
			if(pCheckBox)
			{
				pCheckBox->SetCheck(0);
			}
		}
	}
	return false;
}


void CWaveCompressionPropPage::OnCloseupCombo1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bTypeCBClosed = true;
}

void CWaveCompressionPropPage::OnDropdownCombo1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bTypeCBClosed = false;	
}

void CWaveCompressionPropPage::OnCloseupCombo2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bAttributesCBClosed = true;
}

void CWaveCompressionPropPage::OnDropdownCombo2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_bAttributesCBClosed = false;	
}


void CWaveCompressionPropPage::SetCompressionInfo()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	// Set the wave size text
	CString sWindowText;
	float fCompressionRatio = 1.0;
	DWORD dwWaveSize = m_pWave->GetUncompressedDataSize();

	if(m_pWave->m_bCompressed)
	{
		DWORD dwCompressedWaveSize = m_pWave->GetCompressedDataSize();
		if(dwCompressedWaveSize != 0)
		{
			fCompressionRatio = ((float)dwWaveSize / dwCompressedWaveSize);
			dwWaveSize = dwCompressedWaveSize;
		}
	}

	sWindowText.Format("%d bytes", dwWaveSize);
	m_WaveSize.SetWindowText(sWindowText);

	sWindowText.Format("1:%f", fCompressionRatio);
	m_CompressionRatio.SetWindowText(sWindowText);
}


void CWaveCompressionPropPage::InitializeDialogValues()
{
	if(m_pWave)
	{
		CButton* pCheckBox = (CButton*) GetDlgItem(IDC_COMPRESSED);

		if(pCheckBox)
		{
			if (m_pWave->m_bCompressed)
			{
				pCheckBox->SetCheck(1);
				
				// Set the selected compression to whatever was selected
				// Set from the format tag.
				SetSelectedCompressionType((WORD)m_pWave->GetCompressionFormatTag());
				
				WAVEFORMATEX wfxCompression;
				if(SUCCEEDED(m_pWave->GetCompressionFormat(&wfxCompression)))
				{
					SetSelectedFormat(&wfxCompression);
				}
			}
			else
			{
				pCheckBox->SetCheck(0);
				
				// Set the selected compression to whatever was selected in the last wave compresed
				// Set from the format tag.
				if(m_TypeComboBox.SetCurSel(m_nLastCompressionType) != CB_ERR)
                {
					FORMAT_TAG_INF0 *pfti = (FORMAT_TAG_INF0 *)m_TypeComboBox.GetItemDataPtr(m_nLastCompressionType);
				    FillAttributesComboBox(pfti);
				    m_AttributesComboBox.SetCurSel(m_nLastCompressionAttribute);
                }
			}

			m_DecompressedStartSpin.SetRange32(0, min(m_pWave->m_dwWaveLength, UD_MAXVAL));
			m_DecompressedStartSpin.SetPos(m_pWave->GetDwDecompressedStart(true));
			EnableDecompressedStartControls();
		}

		SetCompressionInfo();

		m_ReadAheadSpin.SetRange(MIN_READAHEAD, MAX_READAHEAD);
		CWaveNode* pWaveNode = m_pWave->GetNode();
		ASSERT(pWaveNode);
		
		m_StreamCheck.SetCheck(m_pWave->IsStreamingWave());
		m_NoPrerollCheck.SetCheck(m_pWave->IsNoPrerollWave());

		BOOL bEnableStreamingControls = pWaveNode->IsInACollection() ? FALSE : TRUE;
		EnableStreamingControls(bEnableStreamingControls);

		CString sReadAheadTime;
		DWORD dwReadAheadTime = m_pWave->GetReadAheadTime();
		sReadAheadTime.Format("%d", dwReadAheadTime);
		m_ReadAheadEdit.SetWindowText(sReadAheadTime);
		m_ReadAheadSpin.SetPos(dwReadAheadTime);
		
		UpdateReadAheadSamples(dwReadAheadTime);
	}
    
	CWavePropPgMgr::dwLastSelPage = WAVE_COMPRESSION_PAGE;
}

void CWaveCompressionPropPage::OnStreamCheck() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	int nStreamCheck = m_StreamCheck.GetCheck();
	if(FAILED(m_pWave->SaveUndoState(IDS_UNDO_STREAMING)))
	{
		int nResetCheck = 0;
		if(nStreamCheck == 0)
		{
			nResetCheck = 1;
		}
		m_StreamCheck.SetCheck(nResetCheck);
		return;
	}

	BOOL bStream = nStreamCheck == 1 ? TRUE : FALSE;
	m_pWave->SetStreamingWave(bStream);
	m_pWave->SetDirtyFlag();
	EnableReadAheadControls(bStream);
}

void CWaveCompressionPropPage::OnNoprerollCheck() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	int nNoPrerollCheck = m_NoPrerollCheck.GetCheck();
	if(FAILED(m_pWave->SaveUndoState(IDS_UNDO_NOPREROLL)))
	{
		int nResetCheck = 0;
		if(nNoPrerollCheck == 0)
		{
			nResetCheck = 1;
		}
		m_NoPrerollCheck.SetCheck(nResetCheck);
		return;
	}

	BOOL bNoPreroll = nNoPrerollCheck == 1 ? TRUE : FALSE;
	m_pWave->SetNoPrerollWave(bNoPreroll);
	m_pWave->SetDirtyFlag();
}

void CWaveCompressionPropPage::OnDeltaposReadaheadSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	OnKillfocusReadaheadEdit();
	
	*pResult = 0;
}

void CWaveCompressionPropPage::OnKillfocusReadaheadEdit() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CString sReadAhead;
	m_ReadAheadEdit.GetWindowText(sReadAhead);
	DWORD dwReadAheadTime = atoi(sReadAhead);
	if(dwReadAheadTime > MAX_READAHEAD)
	{
		dwReadAheadTime = MAX_READAHEAD;
	}
	else if(dwReadAheadTime < MIN_READAHEAD)
	{
		dwReadAheadTime = MIN_READAHEAD;
	}

	if(FAILED(m_pWave->SaveUndoState(IDS_UNDO_READAHEAD_TIME)))
	{
		dwReadAheadTime = m_pWave->GetReadAheadTime();
	}

	sReadAhead.Format("%d", dwReadAheadTime);
	m_ReadAheadEdit.SetWindowText(sReadAhead);

	m_pWave->SetReadAheadTime(dwReadAheadTime);
	m_pWave->SetDirtyFlag();
	UpdateReadAheadSamples(dwReadAheadTime);
}

void CWaveCompressionPropPage::EnableStreamingControls(BOOL bEnable)
{
	m_StreamCheck.EnableWindow(bEnable);
	if(bEnable == FALSE)
	{
		EnableReadAheadControls(bEnable);
	}
	else
	{
		int nStreamCheck = m_StreamCheck.GetCheck();
		BOOL bStream = nStreamCheck == 1 ? TRUE : FALSE;
		EnableReadAheadControls(bStream);
	}
}

void CWaveCompressionPropPage::EnableReadAheadControls(BOOL bEnable)
{
	m_ReadAheadEdit.EnableWindow(bEnable);
	m_ReadAheadSpin.EnableWindow(bEnable);
	m_ReadAheadStatic.EnableWindow(bEnable);
	m_ReadAheadLabel.EnableWindow(bEnable);
	m_MSLabel.EnableWindow(bEnable);
	m_NoPrerollCheck.EnableWindow(bEnable);
}

void CWaveCompressionPropPage::EnableDecompressedStartControls() 
{
	bool bEnable = m_pWave->FConsiderDecompressedStart();
	m_DecompressedStartSpin.EnableWindow(bEnable);
	m_DecompressedStartEdit.EnableWindow(bEnable);
	m_DecompressedStartLabel.EnableWindow(bEnable);
	m_DecompressedStartLabel2.EnableWindow(bEnable);
}

void CWaveCompressionPropPage::UpdateReadAheadSamples(DWORD dwReadAheadTime)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	DWORD dwSampleRatePerMS = m_pWave->m_rWaveformat.nSamplesPerSec / 1000;
	DWORD dwReadAheadSamples = 	dwReadAheadTime * dwSampleRatePerMS;

	CString sReadAheadSamples;
	sReadAheadSamples.Format("%d samples", dwReadAheadSamples);
	m_ReadAheadStatic.SetWindowText(sReadAheadSamples);
}

void CWaveCompressionPropPage::OnKillfocusDecompressedStartEdit() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	// range checking
	if (FAILED(m_pWave->HrSetDecompressedStart(GetDlgItemInt(IDC_DECOMPRESSEDDATASTART_EDIT, NULL, FALSE))))
		SetDlgItemInt(IDC_DECOMPRESSEDDATASTART_EDIT, m_pWave->GetDwDecompressedStart(false));
}

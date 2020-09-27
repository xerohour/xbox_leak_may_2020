// WaveRecordToolbar.cpp : implementation file
//

#include "stdafx.h"
#include <mmsystem.h>
#include "dsoundp.h"
#include "CConduct.h"
#include "phoneyds.h"
#include <PChannelName.h>
#include "WaveRecordToolbar.h"
#include <commdlg.h>
#include <dmksctrl.h>
#include <initguid.h>
//#include "dmusics.h"
#include "MIDISave.h"
#include "DlgMIDIExport.h"
#include "..\WaveSaveDmo\testdmo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD g_dwLatency; // Defined in dslink.cpp

/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar

CWaveRecordToolbar::CWaveRecordToolbar()
{
	m_hWndToolbar = NULL;

	// Wave recording
	m_strOriginalWaveFilename.Empty();
	m_fRecordingWave = FALSE;
	m_fSetWaveFilenameForRecording = FALSE;

	// MIDI recording
	m_strMIDIFilename.Empty();
	m_fExportingMIDI = FALSE;
}

CWaveRecordToolbar::~CWaveRecordToolbar()
{
	if( m_fRecordingWave )
	{
		StartOrStopDumpDMOs( false );
		m_fRecordingWave = FALSE;
	}

	StopExportingMIDI();

	if( m_font.GetSafeHandle() )
	{
		m_font.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar IDMUSProdToolBar::GetInfo

HRESULT CWaveRecordToolbar::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( (phWndOwner == NULL)
	||	(phInstance == NULL)
	||	(pnResourceId == NULL)
	||	(pbstrTitle == NULL) )
	{
		return E_POINTER;
	}

	*phWndOwner = m_hWnd;
	*phInstance = _Module.GetResourceInstance();
	*pnResourceId = (UINT)-1;  // we are going to build the toolbar from scratch

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_WAVERECORD_TITLE ) )
	{
		*pbstrTitle = bstrMenuHelpText.Detach();
	}
	else
	{
		*pbstrTitle = NULL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar IDMUSProdToolBar::GetMenuText

HRESULT CWaveRecordToolbar::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_WAVERECORD_MENUTEXT ) )
	{
		*pbstrText = bstrMenuHelpText.Detach();
		return S_OK;
	}
	else
	{
		*pbstrText = NULL;
		return E_FAIL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar IDMUSProdToolBar::GetMenuHelpText

HRESULT CWaveRecordToolbar::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_WAVERECORD_HELP ) )
	{
		*pbstrMenuHelpText = bstrMenuHelpText.Detach();
		return S_OK;
	}
	else
	{
		*pbstrMenuHelpText = NULL;
		return E_FAIL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar IDMUSProdToolBar::Initialize

HRESULT CWaveRecordToolbar::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( hWndToolBar == NULL )
	{
		return E_INVALIDARG;
	}
	m_hWndToolbar = hWndToolBar;
 
	// Create font for toolbar text
	CClientDC* pDC = new CClientDC( CWnd::FromHandle( m_hWndToolbar) );

	int nHeight = -( (pDC->GetDeviceCaps(LOGPIXELSY) * 8) / 72 );

	CString strFontName;
	if( strFontName.LoadString( IDS_TOOLBAR_FONT ) == 0)
	{
		strFontName = CString("MS Sans Serif");
	}
	m_font.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName );
	
	CFont* pOldFont = pDC->SelectObject( &m_font );
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	int cxChar =  tm.tmAveCharWidth;
	//int cyChar = tm.tmHeight + tm.tmExternalLeading;
	pDC->SelectObject( pOldFont );
	delete pDC;

	::SendMessage( m_hWndToolbar, TB_SETMAXTEXTROWS, 1, 0 );

	TBADDBITMAP tbAddBitmap;
	tbAddBitmap.hInst = _Module.GetResourceInstance();
	tbAddBitmap.nID = IDB_EXPORT_WAVE;
	::SendMessage( m_hWndToolbar, TB_ADDBITMAP, 4, (LPARAM)&tbAddBitmap );

	TBBUTTON button;
	button.dwData = 0;
	button.iString = 0;

	// 8 - Record MIDI
	button.iBitmap = 3;
	button.idCommand = ID_MIDI_EXPORT;
	button.fsState = 0;
	button.fsStyle = TBSTYLE_CHECK;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 7 - MIDI Filename
	button.iBitmap = 2;
	button.idCommand = ID_MIDI_FILENAME;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 6 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 5 - Wide separator for MIDI Filename
	button.iBitmap = cxChar * 20;
	button.idCommand = IDC_RECORD_MIDI_FILENAME;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 4 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 3 - Record wave
	button.iBitmap = 1;
	button.idCommand = ID_WAVE_RECORD;
	button.fsState = 0;
	button.fsStyle = TBSTYLE_CHECK;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 2 - Filename
	button.iBitmap = 0;
	button.idCommand = ID_WAVE_FILENAME;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 1 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 0 - Wide separator for Filename
	button.iBitmap = cxChar * 20;
	button.idCommand = IDC_RECORD_FILENAME;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// Attach the wave filename static text box
	RECT rect;
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rect );
	if( !m_staticWaveFilename.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_RECORD_FILENAME) )
	{
		return -1;
	}
	m_staticWaveFilename.SetFont( &m_font );

	// Attach the MIDI filename static text box
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 5, (LPARAM)&rect );
	if( !m_staticMIDIFilename.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_RECORD_MIDI_FILENAME) )
	{
		return -1;
	}
	m_staticMIDIFilename.SetFont( &m_font );

	return S_OK;
}

LRESULT CWaveRecordToolbar::OnWaveFilenameClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFileDialog dlgFile( FALSE );

	// Set buffer for file name
	TCHAR tcstrFileName[_MAX_PATH];
	ZeroMemory( tcstrFileName, sizeof(TCHAR) * _MAX_PATH );
	dlgFile.m_ofn.lpstrFile = tcstrFileName;
	dlgFile.m_ofn.nMaxFile = _MAX_PATH;

	// Set buffer for file title
	TCHAR tcstrFileTitle[_MAX_FNAME];
	ZeroMemory( tcstrFileTitle, sizeof(TCHAR) * _MAX_FNAME );
	dlgFile.m_ofn.lpstrFileTitle = tcstrFileTitle;
	dlgFile.m_ofn.nMaxFileTitle = _MAX_FNAME;

	CString strTitle, strDefaultExt, strWaveFilterExt;
	VERIFY( strTitle.LoadString( IDS_WAVEFILE_TITLE ) );
	VERIFY( strDefaultExt.LoadString( IDS_WAVEFILE_DEFAULTEXT ) );
	VERIFY( strWaveFilterExt.LoadString( IDS_WAVEFILE_FILTER_EXT ) );

	dlgFile.m_ofn.lpstrDefExt = strDefaultExt;
	dlgFile.m_ofn.lpstrTitle = strTitle;

	// Contruct *.wav filter
	CString strFilter;
	VERIFY( strFilter.LoadString( IDS_WAVEFILE_FILTERDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strWaveFilterExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	CString strAllFilter;
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );

	// Get the default directory for this template
	TCHAR tcstrInitialDir[MAX_PATH];
	ZeroMemory( tcstrInitialDir, sizeof(TCHAR) * MAX_PATH );
	DWORD dwCbData = sizeof(TCHAR) * MAX_PATH;
	GetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\"), _T("WaveOutputRecordPath"),
		tcstrInitialDir, &dwCbData );

	if( tcstrInitialDir[0] )
	{
		dlgFile.m_ofn.lpstrInitialDir = tcstrInitialDir;
	}

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;

	if( bResult )
	{
		// Store the default directory for the next Export Wave dialog
		TCHAR tcsPath[MAX_PATH];
		ZeroMemory( tcsPath, sizeof(TCHAR) * MAX_PATH );
		TCHAR *pLastSlash = _tcsrchr( dlgFile.m_ofn.lpstrFile, '\\' );
		_tcsncpy( tcsPath, dlgFile.m_ofn.lpstrFile, pLastSlash - dlgFile.m_ofn.lpstrFile );
		SetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\"), _T("WaveOutputRecordPath"), tcsPath );

		// Selected filename
		m_strOriginalWaveFilename = dlgFile.m_ofn.lpstrFile;
		m_staticWaveFilename.SetWindowText( dlgFile.m_ofn.lpstrFileTitle );

		m_fSetWaveFilenameForRecording = TRUE;

		/* Always enable the button
		if( g_pconductor->m_fAudiopathHasDumpDMOs )
		{
		*/
			::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(TRUE, 0) );
		/*
		}
		*/
	}

	return FALSE; // Don't process further
}

LRESULT CWaveRecordToolbar::OnWaveRecordClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_fRecordingWave )
	{
		StartRecording();
	}
	else
	{
		StopRecording();
	}

	return FALSE; // Don't process further
}

void CWaveRecordToolbar::StopAll( void )
{
	// Stop wave export
	if( m_fRecordingWave )
	{
		StopRecording();
	}
	
	// Stop MIDI export
	StopExportingMIDI();
}

void CWaveRecordToolbar::StartRecording( void )
{
	// Start recording
	if( g_pconductor->m_fUsePhoneyDSound )
	{
		HRESULT hr;

		if( m_strOriginalWaveFilename.IsEmpty() )
		{
			// Uncheck the record button
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
			return;
		}
		
		WAVEFORMATEX wfex;
		hr = g_pconductor->m_pPhoneyDSound->GetFormat( &wfex );

		if (FAILED(hr) || (wfex.wFormatTag != WAVE_FORMAT_PCM) || (wfex.cbSize != 0))
		{
			// Uncheck the record button
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
			return;
		}

		HANDLE hCaptureFile;
		hCaptureFile = CreateFile(m_strOriginalWaveFilename,
								 GENERIC_WRITE,
								 0,					// No sharing
								 NULL,				// No security attributes
								 CREATE_ALWAYS,		
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);				// No template file

		if (hCaptureFile == INVALID_HANDLE_VALUE)
		{
			CString strMsg;
			LPVOID lpMessageBuffer;
			
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						   NULL, GetLastError(),
						   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
						   (LPTSTR)&lpMessageBuffer, 0, NULL );

			HINSTANCE hInstanceOld = AfxGetResourceHandle();
			AfxSetResourceHandle( _Module.GetResourceInstance() );

			AfxFormatString1( strMsg, IDS_SYSERR_OPEN, m_strOriginalWaveFilename );
			strMsg = strMsg + (LPTSTR)lpMessageBuffer;
			AfxMessageBox( strMsg );

			AfxSetResourceHandle( hInstanceOld );

			LocalFree( lpMessageBuffer );
			// Uncheck the record button
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
			return;
		}

		SetFilePointer(hCaptureFile, sizeof(WaveFileHeader), 0, FILE_BEGIN);

		// Start recording
		g_pconductor->m_pPhoneyDSound->SetFileHandle( hCaptureFile );
		m_fRecordingWave = TRUE;
		::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(TRUE, 0) );

		return;
	}
	else
	{
		StartOrStopDumpDMOs( true );

		// Start recording
		m_fRecordingWave = TRUE;
		::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(TRUE, 0) );
	}
}

void CWaveRecordToolbar::StopRecording( void )
{
	//GetFileHandle
	m_fRecordingWave = FALSE;

	// Stop recording
	if( g_pconductor->m_fUsePhoneyDSound )
	{
		HANDLE hCaptureFile = g_pconductor->m_pPhoneyDSound->GetFileHandle();

		if (hCaptureFile != INVALID_HANDLE_VALUE)
		{
			g_pconductor->m_pPhoneyDSound->SetFileHandle( INVALID_HANDLE_VALUE );

			// Wait for twice the latency, to ensure that the file is no longer being written to
			Sleep( g_dwLatency * 2 );

			WaveFileHeader wfHeader;
			memset(&wfHeader, 0, sizeof(&wfHeader));
			wfHeader.FourCCRiff = FOURCC_RIFF;
			wfHeader.FourCCWave = mmioFOURCC('W', 'A', 'V', 'E');
			wfHeader.FourCCFmt  = mmioFOURCC('f', 'm', 't', ' ');
			wfHeader.FmtLength  = sizeof(wfHeader.Fmt);
			wfHeader.FourCCData = mmioFOURCC('d', 'a', 't', 'a');

			WAVEFORMATEX wfex;
			if( FAILED( g_pconductor->m_pPhoneyDSound->GetFormat( &wfex ) ) )
			{
				wfex.cbSize = 0;
				wfex.nAvgBytesPerSec = 22050 * 2 * 2;
				wfex.nBlockAlign = 4;
				wfex.nChannels = 2;
				wfex.nSamplesPerSec = 22050;
				wfex.wBitsPerSample = 16;
				wfex.wFormatTag = WAVE_FORMAT_PCM;
			}

			memcpy(&wfHeader.Fmt, &wfex, sizeof(wfHeader.Fmt));

			DWORD dwSize = GetFileSize(hCaptureFile, NULL);

			wfHeader.FileLength = dwSize - sizeof(FOURCC) - sizeof(DWORD);
			wfHeader.DataLength = dwSize - sizeof(wfHeader);
			
			SetFilePointer(hCaptureFile, 0, 0, FILE_BEGIN);
			DWORD dwWritten;
			WriteFile(hCaptureFile, &wfHeader, sizeof(wfHeader), &dwWritten, NULL);
		
			CloseHandle(hCaptureFile);
		}
	}
	else
	{
		StartOrStopDumpDMOs( false );
	}

	// Reset static window to NULL and disable the record button
	m_staticWaveFilename.SetWindowText( NULL );
	::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
	::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
	m_fSetWaveFilenameForRecording = FALSE;

	// Display the file in the project tree
	while( !m_lstUsedWaveFilenames.IsEmpty() )
	{
		BSTR bstrFileName = m_lstUsedWaveFilenames.RemoveHead().AllocSysString();
		g_pconductor->m_pFrameWork->ShowFile( bstrFileName );
	}
}

void IncrementFilename( CHAR szDumpWave[_MAX_PATH] )
{
	// Copy the original name
	CHAR szOrigName[_MAX_PATH];
	strcpy( szOrigName, szDumpWave );

	// Find the position of the last '.'
	CHAR *pDot = strrchr( szOrigName, '.' );

	// If we didn't find the dot
	if( pDot == NULL )
	{
		// Point pDot at the trailing NULL
		pDot = szOrigName + strlen( szOrigName );
	}
	else
	{
		// Cut off the name at the dot
		*pDot = NULL;
	}

	// The string to store the number in
	CHAR *pNumberStart = pDot-1;
	CHAR szNbr[_MAX_PATH];
	while( pNumberStart > szOrigName  &&  isdigit( *pNumberStart ) )
	{
		pNumberStart--;
	}

	// Increment to the first number
	*pNumberStart++;

	// Check if we didn't find a number
	if( pNumberStart == pDot )
	{
		// No number - make szNbr empty
		szNbr[0] = NULL;
	}
	else
	{
		// Copy the number to szNbr
		strcpy( szNbr, pNumberStart );

		// Cut off the name at the start of the number
		*pNumberStart = NULL;
	}

	// Save the length of the textual part of the original name
	const int nOrigNameLength = strlen( szOrigName );

	// Set the first number to use
	int i = atoi( szNbr );

	// Convert from a number to a string
	_itoa( ++i, szNbr, 10 );

	// Get the length of the number text
	int nNbrLength = strlen(szNbr);

	// Ensure the name doesn't go longer than _MAX_PATH (5 = length of ".wav" plus the trailing NULL)
	if( (nOrigNameLength + nNbrLength + 5) <= _MAX_PATH )
	{
		// Name will fit within _MAX_PATH, just copy it
		strcpy( szDumpWave, szOrigName );
		strcat( szDumpWave, szNbr );
		strcat( szDumpWave, ".wav" );
	}
	else
	{
		// Name won't fit within _MAX_PATH, clip szOrigName
		ZeroMemory( szDumpWave, _MAX_PATH );
		strncpy( szDumpWave, szOrigName, _MAX_PATH - nNbrLength - 5 );
		strcat( szDumpWave, szNbr );
		strcat( szDumpWave, ".wav" );
	}
}

HRESULT StartOrStopDumpDMOsInternal( const bool fStart, const DWORD dwPChannel, const DWORD dwBufferPath, const DWORD dwDMOPath, bool &fOneSucceeded, bool &fOneFailed, IDirectMusicAudioPath *pAudiopath, char *szFileName, CStringList &stringList, CString &strFailedFiles )
{
	HRESULT hr = S_FALSE;
	WCHAR wcstrFileName[_MAX_PATH];
	wcstrFileName[0] = NULL;
	IDirectSoundBuffer *pIDirectSoundBuffer;
	DWORD dwBufferIndex = 0;
	while( S_OK == pAudiopath->GetObjectInPath( dwPChannel, dwBufferPath, dwBufferIndex,
		 GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer ) )
	{
		IDump* pIDump = NULL;
		DWORD dwDMOIndex = 0;
		while( S_OK == pAudiopath->GetObjectInPath( dwPChannel, dwDMOPath, dwBufferIndex,
			GUID_DSFX_STANDARD_DUMP, dwDMOIndex, IID_IDump, (void**) &pIDump ) )
		{
			if( fStart )
			{
				// If we've found a DMO, increment the filename
				if( fOneSucceeded
				||	fOneFailed )
				{
					IncrementFilename( szFileName );
				}

				// Convert to a wide character string
				VERIFY( mbstowcs( wcstrFileName, szFileName, strlen(szFileName) + 1 ) > 0 );

				// Set the filename
				pIDump->SetOverwrite( TRUE );
				hr = pIDump->SetDumpWave( wcstrFileName );
				if( SUCCEEDED( hr ) )
				{
					TRACE("Starting dump DMO %s\n", szFileName);
					hr = pIDump->Start();
					if( SUCCEEDED( hr ) )
					{
						fOneSucceeded = true;
						stringList.AddTail( wcstrFileName );
					}
					else
					{
						if( strFailedFiles.IsEmpty() )
						{
							strFailedFiles = szFileName;
						}
						else
						{
							strFailedFiles += CString(_T("\n")) + CString(szFileName);
						}
						fOneFailed = true;
					}
				}
				else
				{
					if( strFailedFiles.IsEmpty() )
					{
						strFailedFiles = szFileName;
					}
					else
					{
						strFailedFiles += CString(_T("\n")) + CString(szFileName);
					}
					fOneFailed = true;
				}
			}
			else
			{
				hr = pIDump->Stop();
				if( SUCCEEDED( hr ) )
				{
					fOneSucceeded = true;
				}
				else
				{
					fOneFailed = true;
				}
			}
			dwDMOIndex++;
			pIDump->Release();
		}

		dwBufferIndex++;
		pIDirectSoundBuffer->Release();
	}

	return hr;
}

HRESULT	CWaveRecordToolbar::StartOrStopDumpDMOsOnAudiopath( bool fStart, IDirectMusicAudioPath *pAudiopath )
{
	// If no audiopath, nothing to do
	if( pAudiopath == NULL )
	{
		return S_FALSE;
	}

	char szFileName[_MAX_PATH];
	szFileName[0] = NULL;
	if( m_lstUsedWaveFilenames.IsEmpty() )
	{
		strcpy( szFileName, m_strOriginalWaveFilename );
	}
	else
	{
		strcpy( szFileName, m_lstUsedWaveFilenames.GetTail() );
		IncrementFilename( szFileName );
	}

	bool fOneSucceeded = false;
	bool fOneFailed = false;
	CString strFailedFiles;
	HRESULT hr1 = StartOrStopDumpDMOsInternal( fStart, DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, DMUS_PATH_BUFFER_DMO,
											   fOneSucceeded, fOneFailed, pAudiopath, szFileName, m_lstUsedWaveFilenames, strFailedFiles );

	HRESULT hr2 = StartOrStopDumpDMOsInternal( fStart, 0, DMUS_PATH_MIXIN_BUFFER, DMUS_PATH_MIXIN_BUFFER_DMO,
											   fOneSucceeded, fOneFailed, pAudiopath, szFileName, m_lstUsedWaveFilenames, strFailedFiles );

	// If something failed, display an error message
	if( fOneFailed
	&&	!strFailedFiles.IsEmpty() )
	{
		CString strError;
		AfxFormatString1( strError, IDS_ERR_NOT_ALL_FILE_OUTPUT, strFailedFiles );
		AfxMessageBox( strError, MB_ICONWARNING | MB_OK );
	}

	// If something succeeded, return S_OK
	if( fOneSucceeded )
	{
		return S_OK;
	}
	// If something failed, return hr
	if( fOneFailed )
	{
		if( FAILED( hr1 ) )
		{
			return hr1;
		}
		return hr2;
	}
	// Otherwise, return E_FAIL;
	return E_FAIL;
}

HRESULT	CWaveRecordToolbar::StartOrStopDumpDMOs( bool fStart )
{
	ASSERT( !fStart || m_lstUsedWaveFilenames.IsEmpty() );

	HRESULT hr = StartOrStopDumpDMOsOnAudiopath( fStart, g_pconductor->m_pDMAudiopath );

	::EnterCriticalSection( &g_pconductor->m_csAudiopathList );
	POSITION pos = g_pconductor->m_lstSegStateAudioPaths.GetHeadPosition();
	while( pos )
	{
		SegStateAudioPath *pSegStateAudioPath = g_pconductor->m_lstSegStateAudioPaths.GetNext( pos );
		HRESULT hrTmp = StartOrStopDumpDMOsOnAudiopath( fStart, pSegStateAudioPath->pAudiopath );
		if( SUCCEEDED( hrTmp ) )
		{
			hr = hrTmp;
		}
	}
	::LeaveCriticalSection( &g_pconductor->m_csAudiopathList );

	return hr;
}

void CWaveRecordToolbar::UpdateRecordButtonState( void )
{
	if( m_fRecordingWave )
	{
		return;
	}

	// Enable the button even if we don't have any dump DMOs.
	BOOL fEnable = m_fSetWaveFilenameForRecording && g_pconductor->m_fOutputEnabled/* && g_pconductor->m_fAudiopathHasDumpDMOs*/;

	::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG( fEnable, 0) );
}

LRESULT CWaveRecordToolbar::OnMIDIFilenameClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFileDialog dlgFile( FALSE );

	// Set buffer for file name
	TCHAR tcstrFileName[_MAX_PATH];
	ZeroMemory( tcstrFileName, sizeof(TCHAR) * _MAX_PATH );
	dlgFile.m_ofn.lpstrFile = tcstrFileName;
	dlgFile.m_ofn.nMaxFile = _MAX_PATH;

	// Set buffer for file title
	TCHAR tcstrFileTitle[_MAX_FNAME];
	ZeroMemory( tcstrFileTitle, sizeof(TCHAR) * _MAX_FNAME );
	dlgFile.m_ofn.lpstrFileTitle = tcstrFileTitle;
	dlgFile.m_ofn.nMaxFileTitle = _MAX_FNAME;

	CString strTitle, strDefaultExt, strMIDIFilterExt;
	VERIFY( strTitle.LoadString( IDS_MIDIFILE_TITLE ) );
	VERIFY( strDefaultExt.LoadString( IDS_MIDIFILE_DEFAULTEXT ) );
	VERIFY( strMIDIFilterExt.LoadString( IDS_MIDIFILE_FILTER_EXT ) );

	dlgFile.m_ofn.lpstrDefExt = strDefaultExt;
	dlgFile.m_ofn.lpstrTitle = strTitle;

	// Contruct *.wav filter
	CString strFilter;
	VERIFY( strFilter.LoadString( IDS_MIDIFILE_FILTERDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strMIDIFilterExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	CString strAllFilter;
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );

	// Get the default directory for this template
	TCHAR tcstrInitialDir[MAX_PATH];
	ZeroMemory( tcstrInitialDir, sizeof(TCHAR) * MAX_PATH );
	DWORD dwCbData = sizeof(TCHAR) * MAX_PATH;
	GetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\"), _T("MIDIExportDefaultPath"),
		tcstrInitialDir, &dwCbData );

	if( tcstrInitialDir[0] )
	{
		dlgFile.m_ofn.lpstrInitialDir = tcstrInitialDir;
	}

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;

	// Store the default directory for the next Export MIDI dialog
	if( bResult )
	{
		TCHAR tcsPath[MAX_PATH];
		ZeroMemory( tcsPath, sizeof(TCHAR) * MAX_PATH );
		TCHAR *pLastSlash = _tcsrchr( dlgFile.m_ofn.lpstrFile, '\\' );
		_tcsncpy( tcsPath, dlgFile.m_ofn.lpstrFile, pLastSlash - dlgFile.m_ofn.lpstrFile );
		SetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\"), _T("MIDIExportDefaultPath"), tcsPath );

		// Selected filename
		m_strMIDIFilename = dlgFile.m_ofn.lpstrFile;
		m_staticMIDIFilename.SetWindowText( dlgFile.m_ofn.lpstrFileTitle );

		UpdateExportMIDIButtonState();
	}

	return FALSE; // Don't process further
}

LRESULT CWaveRecordToolbar::OnMIDIExportClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_fExportingMIDI )
	{
		StartExportingMIDI();
	}
	else
	{
		StopExportingMIDI();
	}

	return FALSE; // Don't process further
}

void CWaveRecordToolbar::StartExportingMIDI( void )
{
	if( m_fExportingMIDI )
	{
		// Already exporting, so just exit
		return;
	}

	if( m_strMIDIFilename.IsEmpty() )
	{
		// Can't export unless there is a filename
		return;
	}
		
	m_fExportingMIDI = TRUE;

	// Update UI
	::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(TRUE, 0) );

	// Start exporting
	if( g_pconductor->m_pMIDISaveTool )
	{
		g_pconductor->m_pMIDISaveTool->SetChannels( GetMaxPChannelNbr() );
		g_pconductor->m_pMIDISaveTool->StartRecording();
	}
}

void CWaveRecordToolbar::StopExportingMIDI( void )
{
	if( m_fExportingMIDI == FALSE )
	{
		RELEASE( g_pconductor->m_pIUnknownMIDIExport );
		RELEASE( g_pconductor->m_pISegStateMIDIExport );
		return;
	}

	m_fExportingMIDI = FALSE;

	// Stop exporting and save the MIDI file
	if( g_pconductor->m_pMIDISaveTool )
	{
		g_pconductor->m_pMIDISaveTool->StopRecording();
		SetTrackNames();
		if( SUCCEEDED ( g_pconductor->m_pMIDISaveTool->SaveMIDIFile( m_strMIDIFilename,
																	 g_pconductor->m_pISegStateMIDIExport,
																	 g_pconductor->m_fLeadInMeasureMIDIExport,
																	 FALSE ) ) )
		{
			BSTR bstrFileName = m_strMIDIFilename.AllocSysString();
			g_pconductor->m_pFrameWork->ShowFile( bstrFileName );

			m_strMIDIFilename.Empty();
			m_staticMIDIFilename.SetWindowText( NULL );
	
			// Update UI
			if( ::IsWindow( m_hWndToolbar ) )
			{
				::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(FALSE, 0) );
				::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(FALSE, 0) );
			}
		}
		else
		{
			// Update UI
			if( ::IsWindow( m_hWndToolbar ) )
			{
				::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(TRUE, 0) );
				::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(FALSE, 0) );
			}
		}
	}

	RELEASE( g_pconductor->m_pIUnknownMIDIExport );
	RELEASE( g_pconductor->m_pISegStateMIDIExport );
}

void CWaveRecordToolbar::UpdateExportMIDIButtonState( void )
{
	if( m_fExportingMIDI )
	{
		return;
	}

	BOOL fEnable = FALSE;
	if(	m_strMIDIFilename.IsEmpty() == FALSE
	&&  g_pconductor->m_fOutputEnabled )
	{
		fEnable = TRUE;
	}

	::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG( fEnable, 0) );
}

DWORD CWaveRecordToolbar::GetMaxPChannelNbr( void )
{
	if( g_pconductor->m_pDMAudiopath )
	{
		for( DWORD dwPChannelNbr = 999; dwPChannelNbr > 0; dwPChannelNbr-- )
		{
			IDirectMusicPort *pDMPort = NULL;
			if( SUCCEEDED( g_pconductor->m_pDMAudiopath->GetObjectInPath( dwPChannelNbr,
					DMUS_PATH_PORT, 0, GUID_All_Objects, 0, IID_IDirectMusicPort, (void **)&pDMPort ) ) )
			{
				pDMPort->Release();
				return dwPChannelNbr + 1;
			}
		}
	}

	return 0;
}

void CWaveRecordToolbar::SetTrackNames( void )
{
	IDMUSProdPChannelName* pIPChannelName;
	WCHAR pszPChannelName[DMUS_MAX_NAME];
	char  pszTheName[DMUS_MAX_NAME / 2];

	if( g_pconductor->m_pMIDISaveTool == NULL
	||  g_pconductor->m_pIUnknownMIDIExport == NULL )
	{
		return;
	}

	IDMUSProdNode* pINode;
	if( SUCCEEDED( g_pconductor->m_pIUnknownMIDIExport->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
	{
		IDMUSProdProject* pIProject;
		if( SUCCEEDED( g_pconductor->m_pFrameWork->FindProject( pINode, &pIProject ) ) )
		{
			if( pIProject->QueryInterface(IID_IDMUSProdPChannelName, (void**)&pIPChannelName) == S_OK )
			{
				// Iterate through all PChannels
				const DWORD dwMaxPChannel = GetMaxPChannelNbr();

				for( DWORD dwPChannelNbr = 0; dwPChannelNbr < dwMaxPChannel; dwPChannelNbr++ )
				{
					pIPChannelName->GetPChannelName( dwPChannelNbr, pszPChannelName );

					memset( pszTheName, 0, (DMUS_MAX_NAME / 2) );
					wcstombs( pszTheName, pszPChannelName, wcslen(pszPChannelName) );
					g_pconductor->m_pMIDISaveTool->SetChannelName( dwPChannelNbr, pszTheName );
				}

				RELEASE( pIPChannelName );
			}

			RELEASE( pIProject );
		}

		RELEASE( pINode );
	}
}

LRESULT CWaveRecordToolbar::OnSegmentEnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(bHandled);
	
	if( g_pconductor->AllTransportsSetToPlay() )
	{
		StopExportingMIDI();
	}

	return 0;
}

LRESULT CWaveRecordToolbar::OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(wNotifyCode);

	// Get the cursor position (To put the menu there)
	POINT ptScreen, ptClient;
	BOOL  bResult;
	bResult = ::GetCursorPos( &ptScreen );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	ptClient = ptScreen;
	bResult = ::ScreenToClient( pNMHDR->hwndFrom, &ptClient );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	if( ::IsWindow( m_hWndToolbar ) )
	{
		CRect rect;

		// See if right click was on MIDI Export button
		::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 8, (LPARAM) &rect );
		if( rect.PtInRect( ptClient ) )
		{
			// Make sure button is enabled
			if( ::SendMessage( m_hWndToolbar, TB_ISBUTTONENABLED, ID_MIDI_EXPORT, 0 ) )
			{
				CDlgMIDIExport dlg;

				dlg.m_fLeadInMeasureMIDIExport = g_pconductor->m_fLeadInMeasureMIDIExport;

				if( dlg.DoModal() == IDOK )
				{
					g_pconductor->m_fLeadInMeasureMIDIExport = dlg.m_fLeadInMeasureMIDIExport;
				}
			}
		}
	}

	return TRUE;
}

#if !defined(AFX_WAVERECORDTOOLBAR_H__F283E750_097F_11D3_A709_00105A26620B__INCLUDED_)
#define AFX_WAVERECORDTOOLBAR_H__F283E750_097F_11D3_A709_00105A26620B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaveRecordToolbar.h : header file
//

#include <pshpack1.h>
struct WaveFileHeader
{
	FOURCC			FourCCRiff;		// 'RIFF'
	DWORD			FileLength;		// Riff content length (filesize - 8)
	FOURCC			FourCCWave;		// 'WAVE'
	FOURCC			FourCCFmt;		// 'fmt '
	DWORD			FmtLength;		// Length of format chunk 
	PCMWAVEFORMAT	Fmt;			// Only support render -> PCM wave format
									// (known to obey RIFF alignment requirements)
	FOURCC			FourCCData;		// 'data'
	DWORD			DataLength;		// Length of actual PCM data
};
#include <poppack.h>

/////////////////////////////////////////////////////////////////////////////
// CWaveRecordToolbar window

class CWaveRecordToolbar :
	public CComObjectRoot,
	public CWindowImpl<COptionsToolbarHandler>,
	public IDMUSProdToolBar
{
friend class CConductor;
friend UINT AFX_CDECL NotifyThreadProc( LPVOID pParam );

// Construction
public:
	CWaveRecordToolbar();

// Implementation
public:
	virtual ~CWaveRecordToolbar();

public:
	BEGIN_COM_MAP(CWaveRecordToolbar)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

	// message map
	BEGIN_MSG_MAP(CWaveRecordToolbar)
		COMMAND_HANDLER(ID_WAVE_FILENAME, BN_CLICKED, OnWaveFilenameClicked)
		COMMAND_HANDLER(ID_WAVE_RECORD, BN_CLICKED, OnWaveRecordClicked)
		COMMAND_HANDLER(ID_MIDI_FILENAME, BN_CLICKED, OnMIDIFilenameClicked)
		COMMAND_HANDLER(ID_MIDI_EXPORT, BN_CLICKED, OnMIDIExportClicked)
		MESSAGE_HANDLER(WM_USER + 3, OnSegmentEnd)
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
	END_MSG_MAP()

	// message handler methods
	LRESULT OnWaveFilenameClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnWaveRecordClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnMIDIFilenameClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnMIDIExportClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnSegmentEnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );

	void	StopAll( void );

	void	StartRecording( void );
	void	StopRecording( void );
	HRESULT	StartOrStopDumpDMOs( bool fStart );
	HRESULT	StartOrStopDumpDMOsOnAudiopath( bool fStart, IDirectMusicAudioPath *pAudiopath );
	void	UpdateRecordButtonState( void );

	void	StartExportingMIDI( void );
	void	StopExportingMIDI( void );
	void	UpdateExportMIDIButtonState( void );
	DWORD	GetMaxPChannelNbr( void );
	void	SetTrackNames( void );

	// Base class override methods
//static CWndClassInfo& GetWndClassInfo();

private:
	CFont			m_font;
	HWND			m_hWndToolbar;
	CString			m_strOriginalWaveFilename;
	CStringList		m_lstUsedWaveFilenames;
	BOOL			m_fRecordingWave;
	BOOL			m_fSetWaveFilenameForRecording;
	CStatic			m_staticWaveFilename;

	CStatic			m_staticMIDIFilename;
	CString			m_strMIDIFilename;
	BOOL			m_fExportingMIDI;
};

#endif // !defined(AFX_WAVERECORDTOOLBAR_H__F283E750_097F_11D3_A709_00105A26620B__INCLUDED_)

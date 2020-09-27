#ifndef __TRACK_H_
#define __TRACK_H_

#include "RiffStrm.h"
#include <dmusicf.h>


#define DMUS_FOURCC_TRACK_UI_CHUNK				mmioFOURCC('t','k','u','i')
#define DMUS_FOURCC_TRACK_COPY_PASTE_UI_CHUNK	mmioFOURCC('t','c','p','u')


#pragma pack(2)

typedef struct ioTrackUI
{
	ioTrackUI()
	{
		dwTrackIndex = 0;	
	}

	DWORD	dwTrackIndex;	// Index into Segment			
} ioTrackUI;

typedef struct ioTrackCopyPasteUI
{
	ioTrackCopyPasteUI()
	{
		pSourceSegment = NULL;
	}

	CSourceSegment*		pSourceSegment;			// From Source Segment list
} ioTrackCopyPasteUI;

#pragma pack()


//////////////////////////////////////////////////////////////////////
//  CTrack

class CTrack : public IUnknown
{
friend class CDirectMusicSong;
friend class CVirtualSegment;
friend class CSongDlg;
friend class CTrackListBox;
friend class CDlgAddTracks;
friend class CTrackPropPageObject;

public:
	CTrack( CDirectMusicSong* pSong );
	CTrack( CDirectMusicSong* pSong, CSourceSegment* pSourceSegment,
			DWORD dwTrackIndex,
			DMUS_IO_TRACK_HEADER* pdmusTrackHeader,
			DMUS_IO_TRACK_EXTRAS_HEADER* pdmusTrackExtrasHeader );
	virtual ~CTrack();
	void Copy( CTrack* pTrack );

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// Additional methods
	HRESULT Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT Save( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveTrackHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveSegmentTrackHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveSegmentTrackExtrasHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveUI( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveCopyPasteUI( IDMUSProdRIFFStream* pIRiffStream );
	void FormatTextUI( CString& strText );
	void FormatFileNameText( CString& strText );
	void FormatTrackGroupText( CString& strText );
	DWORD GetExtraFlags();
	void SetExtraFlags( DWORD dwNewFlagsDM );
	DWORD GetExtraMask();
	HRESULT GUIDToStripMgr( REFGUID guidCLSID, GUID* guidStripMgr );

private:
    DWORD					m_dwRef;
	CDirectMusicSong*		m_pSong;
	CSourceSegment*			m_pSourceSegment;	// Track is from this Segment

	//		UI
	ioTrackUI				m_TrackUI;
	DWORD					m_dwBitsUI;

	// Persisted for DirectMusic
	//		DMUS_IO_TRACK_HEADER
    CLSID					m_clsidTrack;     // Class id of track 
    DWORD					m_dwPosition;     // Position in track list
    DWORD					m_dwGroup;        // Group bits for track
    FOURCC					m_ckid;           // Chunk ID of track's data chunk
    FOURCC					m_fccType;        // List type if ckid is RIFF or LIST

	//		DMUS_IO_TRACK_EXTRAS_HEADER
    DWORD					m_dwFlagsDM;      // DX8 Added flags for control tracks
    DWORD					m_dwPriority;     // Priority for composition
};

#endif // __TRACK_H_

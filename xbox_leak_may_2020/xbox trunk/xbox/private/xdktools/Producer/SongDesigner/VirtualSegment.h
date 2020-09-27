#ifndef __VIRTUAL_SEGMENT_H_
#define __VIRTUAL_SEGMENT_H_

#include "RiffStrm.h"
#include "Track.h"
#include "Transition.h"


#define DMUS_FOURCC_SONG_GUID_UI_CHUNK						mmioFOURCC('s','g','u','i')
#define DMUS_FOURCC_VIRTUAL_SEGMENT_UI_CHUNK				mmioFOURCC('v','s','u','i')
#define DMUS_FOURCC_VIRTUAL_SEGMENT_COPY_PASTE_UI_CHUNK		mmioFOURCC('v','c','p','u')

#define NOTE_TO_CLOCKS(note, ppq) ( (ppq)*4 /(note) )


#pragma pack(2)

typedef struct ioVirtualSegmentUI
{
	ioVirtualSegmentUI()
	{
		wFlags = 0;
		CoCreateGuid( &guidVirtualSegment ); 
		dwTimeSigGroupBits = 1;
		dwNbrMeasures = 20;
	}

	WORD	wFlags;			
	GUID	guidVirtualSegment;
	DWORD	dwTimeSigGroupBits;
	DWORD	dwNbrMeasures;
} ioVirtualSegmentUI;

typedef struct ioVirtualSegmentCopyPasteUI
{
	ioVirtualSegmentCopyPasteUI()
	{
		pSourceSegment = NULL;
		pIToolGraphNode = NULL;
	}

	CSourceSegment*		pSourceSegment;			// From Source Segment list
	IDMUSProdNode*		pIToolGraphNode;		// From Source ToolGraph list
} ioVirtualSegmentCopyPasteUI;

#pragma pack()


//////////////////////////////////////////////////////////////////////
//  CVirtualSegment

class CVirtualSegment : public IUnknown
{
friend class CDirectMusicSong;
friend class CSongDlg;
friend class CSegmentListBox;
friend class CFolder;
friend class CVirtualSegmentPropPageObject;

public:
	CVirtualSegment( CDirectMusicSong* pSong );
	virtual ~CVirtualSegment();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// Additional methods
	HRESULT Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT Save( IDMUSProdRIFFStream* pIRiffStream );

private:
	HRESULT SaveVirtualSegment( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedTracks( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedTransitions( IDMUSProdRIFFStream* pIRiffStream, bool fSaveAll );
	HRESULT SaveSongGuidUI( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveVirtualHeader( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSourceHeader( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveUI( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveCopyPasteUI( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveTrackList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveTransitionList( IDMUSProdRIFFStream* pIRiffStream );

	void EmptyTrackList();
	void InsertTrack( CTrack* pTrack );
	void RemoveTrack( CTrack* pTrack );

	void EmptyTransitionList();
	void InsertTransition( CTransition* pTransition );
	void RemoveTransition( CTransition* pTransition );

public:
	HRESULT	ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick );
	HRESULT	MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime );
	HRESULT	ClocksToMeasureBeat( IDirectMusicSegment* pIDMSegment, MUSIC_TIME mtTime, long* plMeasure, long* plBeat );
	HRESULT	MeasureBeatToClocks( IDirectMusicSegment* pIDMSegment, long lMeasure, long lBeat, MUSIC_TIME* pmtTime );
	IDirectMusicSegment* GetSegmentForTimeSigConversions();
	void RemoveVirtualSegmentReferences( CVirtualSegment* pVirtualSegmentToRemove );
	void RemoveSourceSegmentReferences( CSourceSegment* pSourceSegmentToRemove );
	void RemoveToolGraphReferences( IDMUSProdNode* pIToolGraphNodeToRemove );
	void ResolveVirtualSegmentIDs();
	bool IsVirtualSegmentInTransitionList( CVirtualSegment* pVirtualSegment );
	HRESULT SetDefaultValues( IStream* pIStream );
	void GetSourceSegmentName( CString& strSourceSegmentName );
	void GetName( CString& strName );
	void SetName( CString strName );
	bool HasAudioPath();
	BOOL RecomputeLength();

public:
	bool					m_fIsPlaying;

private:
    DWORD					m_dwRef;
	CDirectMusicSong*		m_pSong;

	// UI
	ioVirtualSegmentUI		m_VirtualSegmentUI;
	bool					m_fResolveNextPlayID;
	DWORD					m_dwNextPlayID;
	DWORD					m_dwBitsUI;

	// Persisted for DirectMusic
	//		UNFO/UNAM
	CString					m_strName;				// Name of Virtual Segment

	//		DMUS_IO_SEGREF_HEADER
	CSourceSegment*			m_pSourceSegment;		// From Source Segment list
	IDMUSProdNode*			m_pIToolGraphNode;		// From Source ToolGraph list
	CVirtualSegment*		m_pNextVirtualSegment;	// From Virtual Segment list

	//		DMUS_IO_SEGMENT_HEADER	
    DWORD					m_dwRepeats_SEGH;			// Number of repeats. By default, 0. 
    MUSIC_TIME				m_mtLength_SEGH;			// Length, in music time. 
    MUSIC_TIME				m_mtPlayStart_SEGH;			// Start of playback. By default, 0. 
    MUSIC_TIME				m_mtLoopStart_SEGH;			// Start of looping portion. By default, 0. 
    MUSIC_TIME				m_mtLoopEnd_SEGH;			// End of loop. Must be greater than dwPlayStart. By default equal to length. 
    DWORD					m_dwResolution_SEGH;		// Default resolution. 
    REFERENCE_TIME			m_rtLength_SEGH;			// Length, in reference time (overrides music time length.) 
    DWORD					m_dwFlags_SEGH;

	CTypedPtrList<CPtrList, CTrack*> m_lstTracks;
	CTypedPtrList<CPtrList, CTransition*> m_lstTransitions;
};

#endif // __VIRTUAL_SEGMENT_H_

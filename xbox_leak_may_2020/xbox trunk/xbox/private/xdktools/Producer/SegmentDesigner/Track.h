#ifndef __TRACK_H__
#define __TRACK_H__ 1

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>

interface IDMUSProdStripMgr;
interface IPersistStream;

class CTrack
{
public:
	CTrack();
	~CTrack();
	void SetStream( IStream* pIStream );
	void SetStripMgr( IDMUSProdStripMgr* punkStripMgr );
	void GetStream( IStream** ppIStream );
	void GetStripMgr( IDMUSProdStripMgr** ppunkStripMgr );
	void SetDMTrack( IDirectMusicTrack* pIDMTrack );
	void GetDMTrack( IDirectMusicTrack** ppIDMTrack );
	void FillTrackHeader( DMUS_IO_TRACK_HEADER *pTrackHeader );
	void FillTrackExtrasHeader( DMUS_IO_TRACK_EXTRAS_HEADER *pTrackExtrasHeader );
	void FillProducerOnlyChunk( struct _IOProducerOnlyChunk *pProducerOnlyChunk );

	GUID		m_guidClassID;		// Class ID of track
	GUID		m_guidEditorID;		// Class ID of editor
	DWORD		m_dwPosition;		// Position in track list
	DWORD		m_dwGroupBits;		// Group bits for track
	FOURCC      m_ckid;				// chunk ID of track's data chunk if 0 fccType valid.
	FOURCC      m_fccType;			// list type if NULL ckid valid

	DWORD		m_dwTrackExtrasFlags;	// Flags for the TRACKEXTRAS chunk
	DWORD		m_dwTrackExtrasPriority;// Priority for the TRACKEXTRAS chunk
	DWORD		m_dwProducerOnlyFlags;	// Flags for the PRODUCER_ONLY chunk

private:
	IStream*			m_pIStream;		// Stream containing data for this track
	IDMUSProdStripMgr*	m_pIStripMgr;	// Strip Manager interface
	IDirectMusicTrack*	m_pIDMTrack;	// Interface for the DirectMusic Track
};

#endif //__TRACK_H__

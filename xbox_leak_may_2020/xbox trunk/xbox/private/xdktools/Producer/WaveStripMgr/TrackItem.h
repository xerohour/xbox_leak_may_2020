#ifndef __WAVE_TRACKITEM_H_
#define __WAVE_TRACKITEM_H_

#include "WaveStripLayer.h"
#include <DMUSProd.h>
#include <dmusici.h>
#include <DLSDesigner.h>
#include <WaveTimelineDraw.h>

#pragma pack(2)

typedef struct FileListInfo
{
	FileListInfo()
	{
		pIProject = NULL;
		memset( &guidFile, 0, sizeof(GUID) );
	}

	IDMUSProdProject*	pIProject;
	CString				strProjectName;
	CString				strName;
	CString				strDescriptor;
	GUID				guidFile;
} FileListInfo;

typedef struct FileRef
{
	FileRef()
	{
		pIDocRootNode = NULL;
		fRemoveNotify = FALSE;
	}

	IDMUSProdNode*		pIDocRootNode;		// Pointer to referenced file's DocRoot node
	BOOL				fRemoveNotify;
	FileListInfo		li;
} FileRef;

typedef struct WaveInfo
{
	WaveInfo()
	{
		rtWaveLength = 0;
		dwWaveEnd = 0;
		fIsLooped = FALSE;
		dwLoopType = 0;
		dwLoopStart = 0;
		dwLoopEnd = 0;
		memset( &guidVersion, 0, sizeof(GUID) );
		fIsStreaming= FALSE;
	}

	REFERENCE_TIME	rtWaveLength;		// Length of the wave (in reference time)
	DWORD			dwWaveEnd;			// The end sample for the wave
	BOOL			fIsLooped;			// Is the wave looped?
	DWORD			dwLoopType;			// The type for this loop
	DWORD			dwLoopStart;		// The start sample for the loop
	DWORD			dwLoopEnd;			// The end sample for the loop
	GUID			guidVersion;		// Updated everytime wave changes
										// Used by download mechanosm
	BOOL			fIsStreaming;		// Is the wave streaming?
} WaveInfo;

#pragma pack()


class CTrackMgr;

// m_dwBitsUI
#define UD_DRAGSELECT		0x00000001


// Flags for SetTimePhysical()
#define STP_LOGICAL_NO_ACTION				1
#define STP_LOGICAL_SET_DEFAULT				2
#define STP_LOGICAL_ADJUST					3
#define STP_LOGICAL_FROM_BEAT_OFFSET		4


class CTrackItem
{
public:
	CTrackItem();
	CTrackItem( CTrackMgr* pTrackMgr, CWaveStrip* pWaveStrip );
	CTrackItem( CTrackMgr* pTrackMgr, CWaveStrip* pWaveStrip, const CTrackItem& );
	virtual ~CTrackItem();

	void Clear();
	void ClearListInfo();
	void Copy( const CTrackItem* pItem );

	BOOL After( const CTrackItem& item );
	BOOL Before( const CTrackItem& item );

	HRESULT SetFileReference( IDMUSProdNode* pINewDocRootNode );
	HRESULT SetListInfo( IDMUSProdFramework* pIFramework );
	HRESULT GetWaveInfo();
	void	FormatUIText( CString& strText );
	void	SwitchTimeBase();
	HRESULT	DrawWave( HDC hDC, LONG lXOffset, int nLayerIndex, WaveDrawParams* pWDP, WaveTimelineInfoParams* pWTIP );

	HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr );
	HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr );
	HRESULT LoadTrackItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, IDMUSProdFramework* pIFramework );
	HRESULT SaveTrackItem( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework );
	
	HRESULT SetTimePhysical( REFERENCE_TIME rtTimePhysical, short nAction );
	HRESULT SetTimeLogical( REFERENCE_TIME rtTimeLogical );
	HRESULT SetWaveStripForPropSheet( CWaveStrip* pWaveStrip );
	CWaveStrip* GetWaveStripForPropSheet() const;
	HRESULT SetTrackMgr( CTrackMgr* pTrackMgr );
	CTrackMgr* GetTrackMgr() const;
	REFERENCE_TIME PitchAdjustedStartOffset();
	HRESULT StartOffsetToUnknownTime( CTrackMgr* pTrackMgr, REFERENCE_TIME* prtStartOffset );
	HRESULT SourceWaveLengthToUnknownTime( CTrackMgr* pTrackMgr, REFERENCE_TIME* prtSourceWaveLength );

	HRESULT RefreshWave();

protected:	
	IDMUSProdNode* FindWaveFile( CString strWaveName, IStream* pIStream, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr );
	HRESULT SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework, IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader );
	HRESULT SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework, IDMUSProdNode* pIDocRootNode );

public:
	// Fields that are persisted
	REFERENCE_TIME	 m_rtTimeLogical;		// Belongs To 
	REFERENCE_TIME	 m_rtTimePhysical;		// Time of measure/beat/tick (REFERENCE_TIME or MUSIC_TIME)
	REFERENCE_TIME	 m_rtStartOffset;		// Distance into source wave to start playback (REFERENCE_TIME)
											// m_rtStartOffset is not adjusted for m_lPitch
	REFERENCE_TIME	 m_rtDuration;			// Duration (REFERENCE_TIME or MUSIC_TIME) 
	DWORD			 m_dwVariations;		// What variations this item belongs to
	DWORD			 m_dwFlagsDM;			// DirectMusic flags
	long			 m_lVolume;				// Gain, in 1/100th of dB. Note: All gain values should be negative
	long			 m_lPitch;				// Pitch offset in 1/100th of a semitone
	long			 m_lVolumeRange;		// Gain range, in 1/100th of dB. Note: All gain values should be negative
	long			 m_lPitchRange;         // Pitch range in 1/100th of a semitone
	FileRef			 m_FileRef;				// Info pertaining to referenced file

	// Loop fields
	BOOL			 m_fLoopedUI;			// This item is looped
    DWORD            m_dwLoopStartUI;		// Start point for a looping wave
    DWORD            m_dwLoopEndUI;			// End point for a looping wave
	BOOL			 m_fLockLoopLengthUI;	// State of 'Lock Loop Length' button (Loop Tab)
	BOOL			 m_fLockEndUI;			// State of 'Lock End' button (FileRef Tab)
	BOOL			 m_fLockLengthUI;		// State of 'Lock Length' button (FileRef Tab)

	// Work fields
	CWaveStripLayer* m_pLayer;				// Layer containing this wave
	WaveInfo		 m_WaveInfo;			// Info about wave retrieved from DLS Designer
	DWORD			 m_dwBitsUI;			// Various bits
	BOOL			 m_fSelected;			// This item is currently selected
	BOOL			 m_fSyncDuration;		// Always sync duration to actual length of wave
	int				 m_nPasteLayerIndex;	// Used in drop and paste operations

	// Drawing data
	RECT			 m_rectWave;			// Coords of drawn wave
	RECT			 m_rectSelect;			// Coords used for selection (includes text)

private:
	CTrackMgr*		 m_pTrackMgr;			// Will be NULL when CTrackItem used for properties
	CWaveStrip*		 m_pWaveStrip;			// Will be NULL when CTrackItem used for properties
};

#endif // __WAVE_TRACKITEM_H_

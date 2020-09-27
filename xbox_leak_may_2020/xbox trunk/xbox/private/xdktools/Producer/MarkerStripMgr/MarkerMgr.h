#ifndef __MARKERMGR_H_
#define __MARKERMGR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// MarkerMgr.h : Declaration of the CMarkerMgr

#include "resource.h"		// main symbols
#include "MarkerStripMgr.h"
#include "MarkerItem.h"
#include "SelectedGridRegion.h"
#include "BaseMgr.h"

class CMarkerStrip;
class CMarkerItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT 20

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_CONTROL_ENABLED)

// Cut/Copy/Paste/Drag/Drop chunks IDs
#define DMPROD_FOURCC_MARKERTRACK_LIST		mmioFOURCC('P','M','R','K')
#define DMPROD_FOURCC_VALIDSTART_CHUNK		mmioFOURCC('p','v','l','s')
#define DMPROD_FOURCC_PLAYMARKER_CHUNK		mmioFOURCC('p','p','l','y')

//  Cut/Copy/Paste/Drag/Drop structures
typedef struct _DMPROD_IO_VALID_START
{
	MUSIC_TIME mtGrid;
	MUSIC_TIME mtTick;
} DMPROD_IO_VALID_START;

typedef struct _DMPROD_IO_PLAY_MARKER
{
	MUSIC_TIME mtGrid;
	MUSIC_TIME mtTick;
} DMPROD_IO_PLAY_MARKER;

void	EmptyMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstMarlers );
HRESULT LoadMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream);
HRESULT LoadCopiedMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream );
HRESULT SaveMarkerList(	CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream);
void NormalizeMarkerList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CMarkerItem*>& list, long lGridOffset );
HRESULT GetBoundariesOfMarkers( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long & lStartTime, long & lEndTime, CTypedPtrList<CPtrList, CMarkerItem*>& list);
HRESULT SaveCueMarkers( IDMUSProdRIFFStream* pIRiffStream, CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers );
HRESULT SaveSwitchMarkers( IDMUSProdRIFFStream* pIRiffStream, CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers );
HRESULT CopyMarkerDataToClipboard( IDMUSProdTimelineDataObject* pITimelineDataObject, IStream* pStreamCopy, UINT uiClipFormat, CBaseMgr *pBaseMgr, CMarkerStrip *pMarkerStrip );
HRESULT MeasureBeatGridToGrids( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lMeasure, long lBeat, long lGrid, long &lNumGrids );
HRESULT GridsToMeasureBeatGrid( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lNumGrids, long &lMeasure, long &lBeat, long &lGrid );

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr
class ATL_NO_VTABLE CMarkerMgr : 
	public CBaseMgr,
	public CComCoClass<CMarkerMgr, &CLSID_MarkerMgr>
{
friend CMarkerStrip;

public:
	CMarkerMgr();
	~CMarkerMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_MARKERMGR)

BEGIN_COM_MAP(CMarkerMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes(); // Returns true if anything changed
	
	void	DeleteSelectedMarkers();
	HRESULT SaveSelectedMarkers(LPSTREAM, CMarkerItem* pMarkerAtDragPoint);
	HRESULT	SaveSelectedMarkers(IStream* pStream, long lGridOffset);

// general helpers
	void	MarkSelectedMarkers(DWORD flags);
	void	UnMarkMarkers(DWORD flags);
	void	DeleteMarked(DWORD flags);
	void	UnselectAll();
	void	SelectAll();
	bool	RemoveItem( CMarkerItem* pItem );
	void	InsertByAscendingTime(CMarkerItem *pMarker);
	MUSIC_TIME GetNextGreatestUniqueTime( MARKER_TYPE typeMarker, long lMeasure, long lBeat, long lGrid, long lTick );
	CMarkerItem* FirstSelectedMarker();
	bool	DeleteBetweenMeasureBeatGrids(long lmStart, long lbStart, long lgStart, long lmEnd, long lbEnd, long lgEnd );
	void	RecomputeMeasureBeats();
	HRESULT ClocksToMeasureBeatGrid( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long *plGrid );
	HRESULT ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long *plGrid, long* plTick );
	HRESULT MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime );
	bool	RecomputeTimesHelper( MARKER_TYPE typeMarker );
	HRESULT CycleMarkers( long lXPos, long lYPos );

protected:
	CTypedPtrList<CPtrList, CMarkerItem*> m_lstCueMarkers;
	CTypedPtrList<CPtrList, CMarkerItem*> m_lstSwitchMarkers;
	CMarkerItem					m_SelectedMarkerItem;

	CMarkerStrip*				m_pMarkerStrip;

};


class CMarkerStrip :
	public CBaseStrip
{
friend CMarkerMgr;

public:
	CMarkerStrip( CMarkerMgr* pMarkerMgr );
	~CMarkerStrip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides

// IDMUSProdTimelineEdit overrides
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );

// IDropSource overrides

// IDropTarget overrides
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

protected:
	void		UnselectGutterRange( void );

// IDropTarget helpers
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long lXPos, long lYPos);
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos, long lYPos);

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);


// Overrides
	bool	SelectItemsInSelectedRegions();
	bool	IsSelected(); // Returns true if any items are selected
	bool	IsEmpty(); // Returns false if there are any items
	bool	SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime);
	HRESULT PostRightClickMenu( POINT pt );

// Custom functions
	void	SelectRegionsFromSelectedMarkers();	
	CMarkerItem* GetMarkerFromPoint( long lPos, long lYPos );
	void	DrawMarkers( bool fSelected, MARKER_TYPE typeMarker, long lXOffset, RECT &rectClip, HDC hDC );
	void	InvertSelectedRegion( MARKER_TYPE typeMarker, long lXOffset, RECT &rectClip, HDC hDC );
	CMarkerItem* GetNextTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid );
	CMarkerItem* GetNextSelectedTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid );
	void	AdjustTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid );
	CMarkerItem* GetTopItemFromMeasureBeatGrid( MARKER_TYPE typeMarker, long lMeasure, long lBeat, long lGrid );
	BOOL	CanCycle( void );

// Insert/remove marker helpers
	HRESULT MarkAllHelper( BOOL fInsert, DWORD dwFlags );
	HRESULT MarkRangeHelper( BOOL fInsert, DWORD dwFlags );
	BOOL	MarkTimeHelper( BOOL fInsert, DWORD dwFlags, MUSIC_TIME mtStart, MUSIC_TIME mtEnd );
	BOOL	InsertRemoveMarkerAtTime( BOOL fInsert, MARKER_TYPE typeMarker, MUSIC_TIME mtTime, long lMeasure, long lBeat, long lGrid );

	CMarkerMgr*		m_pMarkerMgr;
	UINT			m_cfMarkerList;			// Clipboard format
	bool			m_fLeftMouseDown;
	CMarkerItem*	m_pMarkerItemToToggle;
	long			m_lYPos;
	MARKER_TYPE		m_typeShiftAnchor;

public:
	CListSelectedGridRegion*	m_pSelectedCueGridRegions;
	CListSelectedGridRegion*	m_pSelectedSwitchGridRegions;
};

inline void CListSelectedGridRegion_AddRegion(CListSelectedGridRegion& csrList, CMarkerItem& item)
{
	CMusicTimeGridConverter cmtBeg(item.m_lMeasure, item.m_lBeat, item.m_lGrid, csrList.Timeline(), csrList.GroupBits());
	CMusicTimeGridConverter cmtEnd = cmtBeg;
	cmtEnd.AddOneGrid(csrList.Timeline(), csrList.GroupBits());
	CSelectedGridRegion* psr = new CSelectedGridRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}


#endif //__MARKERMGR_H_

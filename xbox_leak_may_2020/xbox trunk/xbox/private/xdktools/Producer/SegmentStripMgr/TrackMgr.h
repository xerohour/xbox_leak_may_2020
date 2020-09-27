#ifndef __SEGMENT_TRACKMGR_H_
#define __SEGMENT_TRACKMGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TrackMgr.h : Declaration of the CTrackMgr

#include "resource.h"		// main symbols
#include "SegmentStripMgr.h"
#include "TrackItem.h"
#include "selectedregion.h"
#include "BaseMgr.h"
#include "SegmentGUIDs.h"

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_PLAY_ENABLED)

class CTrackMgr;
class CSegmentStrip;
class CTrackItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;

void	EmptyList( CTypedPtrList<CPtrList, CTrackItem*>& list );
HRESULT LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, IStream* pIStream );
HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, CTrackItem** ppItem );
HRESULT SaveList(	CTypedPtrList<CPtrList, CTrackItem*>& list, CTrackMgr* pTrackMgr, IStream* pIStream);
HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, CTrackMgr* pTrackMgr, CTrackItem* pItem );
void NormalizeList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CTrackItem*>& list, long lBeatOffset );
HRESULT GetBoundariesOfItems( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long & lStartTime, long & lEndTime, CTypedPtrList<CPtrList, CTrackItem*>& list);

#define MAX_TICK (DMUS_PPQ << 1)

#define DMUS_FOURCC_COPYPASTE_UI_CHUNK		mmioFOURCC('c','p','c','u')

struct ioCopyPasteUI
{
	ioCopyPasteUI()
	{
		lTick = 0;
	}

	long	lTick;		// Tick offset of first item in list
};


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr
class ATL_NO_VTABLE CTrackMgr : 
	public CBaseMgr,
	public CComCoClass<CTrackMgr, &CLSID_SegmentTriggerMgr>,
	public ISegmentTriggerMgr
{
friend CSegmentStrip;

public:
	CTrackMgr();
	~CTrackMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_TRACKMGR)

BEGIN_COM_MAP(CTrackMgr)
	COM_INTERFACE_ENTRY_IID(IID_ISegmentTriggerMgr,ISegmentTriggerMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );

// ISegmentTriggerMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes(); // Returns true if anything changed
	void	RecomputeMeasureBeats();
	
	void DeleteSelectedItems();
	HRESULT SaveSelectedItems( LPSTREAM, CTrackItem* pItemAtDragPoint );
	HRESULT	SaveSelectedItems( IStream* pStream, long lBeatOffset );
	HRESULT LoadTrackItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CTrackItem** ppItem );
	HRESULT SaveTrackItem( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem );

// general helpers
	void	MarkSelectedItems( DWORD flags );
	void	UnMarkItems( DWORD flags );
	void	DeleteMarked( DWORD flags );
	void	UnselectAll();
	void	SelectAll();
	bool	RemoveItem( CTrackItem* pItem );
	void	ValidateMotifNames();
	void	InsertByAscendingTime( CTrackItem *pItem, BOOL fPaste );
	MUSIC_TIME GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick );
	CTrackItem* FirstSelectedItem();
	bool	DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd );
	HRESULT CycleItems( long lXPos );

public:
	HRESULT	ClocksToMeasureBeatTick( MUSIC_TIME mTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT	MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmTime );
	HRESULT ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime );
	DWORD	GetGroupBits();
	bool    IsRefTimeTrack();

	HRESULT FindSegmentFile( CTrackItem* pItem, CString strSegmentName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode );
	HRESULT FindStyleFile( CTrackItem* pItem, CString strStyleName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode );
	HRESULT SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader );
	HRESULT SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode );
	HRESULT SaveRefPendingLoad( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem );
	HRESULT SaveTrackHeader( IDMUSProdRIFFStream* pIRiffStream );

protected:
	CTypedPtrList<CPtrList, CTrackItem*> m_lstTrackItems;
	CTrackItem		m_SelectedTrackItem;
	CSegmentStrip*	m_pSegmentStrip;

	DWORD			m_dwTrackFlagsDM;	

public:
	BOOL			m_fGetActiveTab;
	CString			m_strLastSegmentName;
	CString			m_strLastStyleName;
};


class CSegmentStrip :
	public CBaseStrip
{
friend CTrackMgr;

public:
	CSegmentStrip( CTrackMgr* pTrackMgr );
	~CSegmentStrip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides

// IDMUSProdTimelineEdit overrides
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );

// IDropSource overrides

// IDropTarget overrides
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// General helper overrides
	HRESULT	PostRightClickMenu( POINT pt );

protected:
	void	UnselectGutterRange( void );
	BOOL	CanCycle();

// IDropTarget helpers 
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long position);
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos);

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);

// Overrides of CBaseMgr
	bool	SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime );
	bool	IsSelected(); // Returns true if any items are selected
	bool	IsEmpty(); // Returns false if there are any items

	CTrackMgr*		m_pTrackMgr;

	CTrackItem* GetItemFromPoint( long lPos );
	CTrackItem* GetItemFromMeasureBeat( long lMeasure, long lBeat );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelectedItems();	
	CTrackItem* GetTopItemFromMeasureBeat( long lMeasure, long lBeat );
	CTrackItem* GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	CTrackItem* GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	void AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	BOOL RegisterClipboardFormats();

	UINT			m_cfSegmentTrack;			// CF_SEGMENTTRACK Clipboard format
	UINT			m_cfSegment;				// CF_SEGMENT clipboard format
	UINT			m_cfStyle;					// CF_STYLE clipboard format
	bool			m_fLeftMouseDown;
	CTrackItem*		m_pItemToToggle;
};

inline void CListSelectedRegion_AddRegion(CListSelectedRegion& csrList, CTrackItem& item)
{
	CMusicTimeConverter cmtBeg(item.m_lMeasure, item.m_lBeat, csrList.Timeline(), csrList.GroupBits());
	CMusicTimeConverter cmtEnd = cmtBeg;
	cmtEnd.AddOffset(0,1, csrList.Timeline(), csrList.GroupBits());
	CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}


#endif //__SEGMENT_TRACKMGR_H_

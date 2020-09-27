/************************************************************************
*                                                                       *
*   Copyright (c) 1998-2001 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// LyricMgr.h : Declaration of the CLyricMgr

#ifndef __LYRICMGR_H_
#define __LYRICMGR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"		// main symbols
#include "LyricStripMgr.h"
#include "LyricItem.h"
#include "selectedregion.h"
#include "BaseMgr.h"
#include "SegmentGUIDs.h"

#define FOURCC_LYRIC_LIST					mmioFOURCC('l','y','r','l')
#define FOURCC_LYRIC_ITEM					mmioFOURCC('l','y','r','c')
#define DMUS_FOURCC_COPYPASTE_UI_CHUNK		mmioFOURCC('c','p','c','u')

#define MAX_TICK (DMUS_PPQ << 1)

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_PLAY_ENABLED)

class CLyricMgr;
class CLyricStrip;
class CLyricItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;

void	EmptyLyricList( CTypedPtrList<CPtrList, CLyricItem*>& list );
HRESULT LoadLyricList( CTypedPtrList<CPtrList, CLyricItem*>& list, IStream* pIStream, CLyricMgr* pLyricMgr );
HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CLyricMgr* pLyricMgr, CLyricItem** ppItem );
HRESULT SaveLyricList(	CTypedPtrList<CPtrList, CLyricItem*>& list, IStream* pIStream);
HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, CLyricItem* pItem );
void NormalizeLyricList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CLyricItem*>& list, long lBeatOffset );
HRESULT GetBoundariesOfLyrics( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long & lStartTime, long & lEndTime, CTypedPtrList<CPtrList, CLyricItem*>& list);


struct ioCopyPasteUI
{
	ioCopyPasteUI()
	{
		lTick = 0;
	}

	long	lTick;		// Tick offset of first item in list
};


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr
class ATL_NO_VTABLE CLyricMgr : 
	public CBaseMgr,
	public CComCoClass<CLyricMgr, &CLSID_LyricMgr>,
	public ILyricMgr
{
friend CLyricStrip;

public:
	CLyricMgr();
	~CLyricMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_LYRICMGR)

BEGIN_COM_MAP(CLyricMgr)
	COM_INTERFACE_ENTRY_IID(IID_ILyricMgr,ILyricMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides
	virtual STDMETHODIMP SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// ILyricMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );

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
	void	RecomputeMeasureBeats();
	
	void DeleteSelectedLyrics();
	HRESULT SaveSelectedLyrics(LPSTREAM, CLyricItem* pLyricAtDragPoint);
	HRESULT	SaveSelectedLyrics(IStream* pStream, long lBeatOffset);
	HRESULT LoadLyricItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CLyricItem** ppItem );
	HRESULT SaveLyricItem( IDMUSProdRIFFStream* pIRiffStream, CLyricItem* pItem );

// general helpers
	void	MarkSelectedLyrics(DWORD flags);
	void	UnMarkLyrics(DWORD flags);
	void	DeleteMarked(DWORD flags);
	void	UnselectAll();
	void	SelectAll();
	bool	RemoveItem( CLyricItem* pItem );
	void	InsertByAscendingTime(CLyricItem *pLyric, BOOL fPaste);
	MUSIC_TIME GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick );
	CLyricItem* FirstSelectedLyric();
	bool	DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd );
	HRESULT CycleItems( long lXPos );

public:
	HRESULT	ClocksToMeasureBeatTick( MUSIC_TIME mTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT	MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmTime );
	HRESULT ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime );
	DWORD	GetGroupBits();
	bool    IsRefTimeTrack();

protected:
	CTypedPtrList<CPtrList, CLyricItem*> m_lstLyrics;
	CLyricItem					m_SelectedLyricItem;

	CLyricStrip*				m_pLyricStrip;

};


class CLyricStrip :
	public CBaseStrip
{
friend CLyricMgr;

public:
	CLyricStrip( CLyricMgr* pLyricMgr );
	~CLyricStrip();

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

// Overrides of CBaseStrip
	bool	SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime);
	bool	IsSelected(); // Returns true if any items are selected
	bool	IsEmpty(); // Returns false if there are any items

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);

	CLyricMgr*			m_pLyricMgr;

	CLyricItem* GetLyricFromPoint( long lPos );
	CLyricItem* GetLyricFromMeasureBeat( long lMeasure, long lBeat );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelectedLyrics();	
	CLyricItem* GetTopItemFromMeasureBeat( long lMeasure, long lBeat );
	CLyricItem* GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	CLyricItem* GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	void AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );

	UINT		m_cfLyricList;			// Clipboard format
	bool		m_fLeftMouseDown;
	CLyricItem*	m_pLyricItemToToggle;
};

inline void CListSelectedRegion_AddRegion(CListSelectedRegion& csrList, CLyricItem& lyric)
{
	CMusicTimeConverter cmtBeg(lyric.m_lMeasure, lyric.m_lBeat, csrList.Timeline(), csrList.GroupBits());
	CMusicTimeConverter cmtEnd = cmtBeg;
	cmtEnd.AddOffset(0,1, csrList.Timeline(), csrList.GroupBits());
	CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}


#endif //__LYRICMGR_H_

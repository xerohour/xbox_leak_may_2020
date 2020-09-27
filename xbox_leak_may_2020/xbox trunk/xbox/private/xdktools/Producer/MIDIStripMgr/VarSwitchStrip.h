// VarSwitchStrip.h : Declaration of the VarSwitchStrip

#ifndef __VARSWITCHSTRIP_H_
#define __VARSWITCHSTRIP_H_

#include "resource.h"       // main symbols
#include "timeline.h"
#include "MIDIStripMgr.h"
#include <afxtempl.h>

#define VARSWITCH_HEIGHT 15

#define FOURCC_STYLE_MARKER_CLIPBOARD_CHUNK	mmioFOURCC('s','m','c','c')

#pragma pack(2)
struct ioDMStyleMarkerClipInfo
{
	DWORD	m_dwVariations;			// Variation buttons selected at time of clip
};
#pragma pack()

class CPropPageVarSwitch;

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip class

class CVarSwitchStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDMUSProdPropPageObject
{
friend CMIDIMgr;
friend CPianoRollStrip;
friend CPropPageVarSwitch;
friend CMarkerPropPageMgr;

public:
	CVarSwitchStrip( CMIDIMgr* pMIDIMgr, CPianoRollStrip* pPianoRollStrip );
	~CVarSwitchStrip();

public:
// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

// IDMUSProdStrip
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdTimelineEdit
	HRESULT STDMETHODCALLTYPE Cut( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

// Additional methods
protected:
	HRESULT OnContextMenu( void );
	HRESULT OnLButtonDown( WPARAM wParam, long lXPos, long lYPos );
	HRESULT OnRButtonUp( WPARAM wParam, long lXPos, long lYPos );
    HRESULT Load( IStream* pIStream );
    HRESULT LoadMarkers( IStream* pIStream, long lChunkSize );
    HRESULT Save( IStream* pIStream );
	HRESULT SaveSelectedMarkers( IStream *pIStream );
	
	// Drawing methods
	void InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset );
	void GetGridRect( MUSIC_TIME mtGrid, RECT *pRectMarker, MUSIC_TIME mtClockOffset, MUSIC_TIME mtMax ) const;

	// Selection methods
	void UnselectGutterRange();
	void OnGutterSelectionChange( BOOL fChanged );
	WORD SelectAllVarSwitches( BOOL fState );
	WORD SelectOnlyMarkersBetweenGrids( MUSIC_TIME mtGridStart, MUSIC_TIME mtGridEnd );

	// Helper methods
	void RefreshUI(WORD& wRefreshUI);
	HWND GetMyWindow();
	BOOL GetStripRect(LPRECT rectStrip);
	int AdjustXPos( int nStripXPos );
	long AdjustClock( long lClock );
	long CalcPartClockLength( void ) const;
	void RefreshPropertyPage();
	void RefreshVarSwitchStrips();
	HRESULT InsertHelper( WORD wMarkerType );
	BOOL InsertMarkerAtGrid( WORD wMarkerType, MUSIC_TIME mtGrid );
	CDirectMusicStyleMarker* GetMarkerFromPoint( long lXPos, long lYPos, DWORD dwVariations ) const;
	CDirectMusicStyleMarker* GetMarkerFromGrid( MUSIC_TIME mtGrid, DWORD dwVariations ) const;
	CDirectMusicStyleMarker* GetEarliestSelectedMarker( void ) const;
	CDirectMusicStyleMarker* CreateNewMarker( MUSIC_TIME mtGrid );
	BOOL IsAnyMarkerSelected( void ) const;
	HRESULT MarkAllHelper( BOOL fInsert, DWORD dwFlags );
	HRESULT MarkRangeHelper( BOOL fInsert, DWORD dwFlags );
	HRESULT MarkTimeHelper( BOOL fInsert, DWORD dwFlags, MUSIC_TIME mtStart, MUSIC_TIME mtEnd );
	void DeleteMarkersBetweenTimes( MUSIC_TIME mtGridEarliestMarker, MUSIC_TIME mtGridLatestMarker );
	
protected:
	DWORD				m_dwRef;
	IDMUSProdFramework* m_pIFramework;
	CMIDIMgr*			m_pMIDIMgr;			// pointer to our MIDI manager
	IDMUSProdStripMgr*	m_pStripMgr;
	CPianoRollStrip*	m_pPianoRollStrip;	// pointer to corresponding Piano Roll strip

	MUSIC_TIME			m_mtGridToShiftSelFrom;

	long				m_lStartSelectInRange;
	long				m_lEndSelectInRange;
};


#endif //__VARSWITCHSTRIP_H_

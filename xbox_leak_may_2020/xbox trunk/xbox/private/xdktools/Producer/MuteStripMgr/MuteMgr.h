// MuteMgr.h : Declaration of the CMuteMgr

#ifndef __MUTEMGR_H_
#define __MUTEMGR_H_

#include "resource.h"       // main symbols
#include "MuteItem.h"
#include "timeline.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#include "MuteScrollBar.h"
#include <mmsystem.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_CONTROL_ENABLED)

#define MAX_PCHANNEL 999

class CMuteStrip;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr
class ATL_NO_VTABLE CMuteMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMuteMgr, &CLSID_MuteMgr>,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{

friend CMuteStrip;

public:
	CMuteMgr();
	~CMuteMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_MUTEMGR)

BEGIN_COM_MAP(CMuteMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

public:
// IDMUSProdStripMgr methods
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist methods
	STDMETHOD(GetClassID)( CLSID* pClsId );

// IPersistStream methods
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
	STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject methods
	HRESULT STDMETHODCALLTYPE GetData(void **ppData);
	HRESULT STDMETHODCALLTYPE SetData(void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties(void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager(void);

protected:
	// helper methods
	HRESULT		SaveRange(IStream* pIStream, MUSIC_TIME mtStart, MUSIC_TIME mtEnd);
	void		ShiftMuteItems(MUSIC_TIME mtStart, MUSIC_TIME mtOffset);
	void		DeleteRange(MUSIC_TIME mtStart, MUSIC_TIME mtEnd);
	HRESULT		LoadPaste(IStream* pIStream, MUSIC_TIME mtPos, MUSIC_TIME mtStart, MUSIC_TIME mtEnd);
	void		PasteMerge(CMuteItem* pMuteItem);
	HRESULT STDMETHODCALLTYPE OnDataChanged(void);
	HRESULT		SyncWithDM();
	void		EmptyMuteList();
	void		InsertMuteItem(CMuteItem* pMuteItem);	
	void		MergeMuteItem(CMuteItem* pMuteItem);
	void		UpdateMuteTime(CMuteItem* pMuteItem);	
	void		UpdateMuteMeasureBeat(CMuteItem* pMuteItem);
	void		RecomputeMuteTimes();
	void		RecomputeMuteMeasureBeats();
	MUSIC_TIME	GetMusicTime(long lMeasure, long lBeat);
	void		GetMeasureBeat(MUSIC_TIME mtTime, long &lMeasure, long &lBeat);
	void		NormalizeMeasureBeat(long &lMeasure, long &lBeat);
	void		GetNextMuteMeasureBeat(long lMeasure, long lBeat, DWORD dwPChannel, 
									   long &lNextMeasure, long &lNextBeat);

	enum MutePos { 
		mpLeft,		// position is to the left of a mute item
		mpRight,	// position is to the right of a mute item
		mpMiddle,	// position is within (overlapping) a mute item
		mpBetween	// position is in between two mute items
	};
	MutePos	FindMuteItem(long lMeasure, long lBeat, DWORD dwPChannel, CMuteItem*& pItem, CMuteItem*& pOther);

	// methods useful to the mute strip
	void	TurnOnMute(long lMeasure, long lBeat, DWORD dwPChannel);
	void	TurnOffMute(long lMeasure, long lBeat, DWORD dwPChannel);
	void	RemapPChannel(long lMeasure, long lBeat, DWORD dwPChannel, DWORD dwPChannelMap);
	void	ClearRemap(long lMeasure, long lBeat, DWORD dwPChannel);
	void	MuteEntirePChannel(DWORD dwPChannel);
	void	UnmuteEntirePChannel(DWORD dwPChannel);
	void	RemoveChannelData(DWORD dwPChannel);
	
	bool	IsPositionMuted(long lMeasure, long lBeat, DWORD dwPChannel);
	CMuteItem* GetItemAtPosition(long lMeasure, long lBeat, DWORD dwPChannel);
	bool	ChannelContainsData(DWORD dwPChannel);
	
protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CMuteItem*> m_lstMutes;
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdNode*				m_pDMProdSegmentNode;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDataObject*				m_pCopyDataObject;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	bool						m_fDirty;
	bool						m_fTimeSigChange;
	CMuteStrip*					m_pMuteStrip;

};

// structure for mute strip design time information
struct ioMuteStripDesign
{
	long m_lVScroll;
	long m_lHeight;
	STRIPVIEW m_svView;
};
// TODO: move to includes/ioDmStyle.h
#define DMUS_FOURCC_MUTE_UI_CHUNK		mmioFOURCC('m','u','t','u')

class CMuteStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDMUSProdPropPageObject
{
friend CMuteMgr;
friend CMuteScrollBar;
public:
	CMuteStrip( CMuteMgr* pMuteMgr );
	~CMuteStrip();

public:
// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

// IDMUSProdStrip
	HRESULT STDMETHODCALLTYPE Draw(HDC hDC, STRIPVIEW sv, LONG lXOffset);
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

protected:
	// vertical scroll stuff (via scroll bar)
	void	UpdateVScroll();	
	void	SetVScroll(long lNewValue);
	void	LimitVScroll();
	void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	HRESULT OnVScroll( WPARAM wParam, LPARAM lParam);
	HRESULT OnSize( WPARAM wParam, LPARAM lParam);

	// horizontal/vertical scroll stuff (auto scroll while dragging mouse)
	void StartScrollTimer( UINT nScrollTimerID );
	void StopScrollTimer();
	CWnd* GetTimelineCWnd();
	void OnTimer();

	// general helpers
	HWND	GetMyWindow();
	void	UpdateStateData();
	void	ApplyStateData();
	bool	IsBeyondMaxTime(long lMeasure, long lBeat);
	void	SnapTimes(MUSIC_TIME mtBegin, MUSIC_TIME mtEnd, MUSIC_TIME &mtBeginResult, MUSIC_TIME &mtEndResult);
	void	GetSelectTimes(MUSIC_TIME &mtBeginTime, MUSIC_TIME &mtEndTime);
	void	UpdateName();
	void	DoAddRemoveDlg();
	void	DoRemapDlg();
	void	SyncStateData();
	HRESULT SaveStateData(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT LoadStateData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pck);
	HRESULT MergeStateData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pck);


	// mouse stuff
	HRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	
	// drawing stuff
	void	GetPosRect(long chIdx, long lMeasure, long lBeat, LONG lXOffset, LPRECT pRectBox);
	int		YPosToChannelIdx(long lYPos);
	void	DrawMuteItem(HDC hDC, CMuteItem* pMuteItem, int nChanIdx, MUSIC_TIME mtMax, LONG lXOffset);
	void	InvalidatePosition(long chIdx, long lMeasure, long lBeat);
	
	// mouse editing mode
	enum EditMode { 
		emNone, emToggleOn, emToggleOff
	} m_EditMode;	// current editing mode for mouse operations

	long		m_cRef;				// reference count
	MUSIC_TIME	m_mtBeginSelect;
	MUSIC_TIME	m_mtEndSelect;
	static MUSIC_TIME	m_mtBeginCopied;// static because they must
	static MUSIC_TIME	m_mtEndCopied;	// be remembered across strips
	CMuteMgr*			m_pMuteMgr;
	IDMUSProdStripMgr*	m_pStripMgr;
	BOOL		m_bGutterSelected;
									
	CString		m_strName;

	CArray<DWORD, DWORD> m_arrChannels; // channel array (must be kept sorted)
	ioMuteStripDesign m_StateData;		// design time  data
	bool		m_fLoadedStateData;		// flag to indicate if design data was loaded

	UINT		m_nScrollTimerID;
	BOOL		m_fLButtonDown;
	DWORD		m_dwTimerCount;

private:
	HRESULT		OnShowProperties(IDMUSProdTimeline*);

	UINT		m_cfMuteList;			// Clipboard format
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	int			m_nLastEdit;
	STRIPVIEW	m_svView;				// current strip view (minimized or maximized)

	// right click stuff
	long	m_lClickedMeasure;			// measure clicked on
	long	m_lClickedBeat;				// beat clicked on
	int		m_nClickedIdx;				// index of channel clicked (in m_arrChannels)

	// vertical scrollbar stuff
	long			m_lVScroll;			// current vertical scroll (in c_nChannelHeight units)
	CRect			m_rectVScroll;		// rectangle for location of vertical scrollbar
	CMuteScrollBar	m_VScrollBar;		// vertical scrollbar

	IDMUSProdPropPageManager*	m_pPropPageMgr;

};

#endif //__MUTEMGR_H_

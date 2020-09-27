#ifndef __PRIVATETIMELINECTL_H_
#define __PRIVATETIMELINECTL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PrivateTimelineCtl.h : Declaration of the CPrivateTimelineCtl

#include "resource.h"       // main symbols
#include "stripmgr.h"
#include "TimelineDraw.h"
#include <DMUSProd.h>
#include "ContextMenuHandler.h"
#include <afxext.h>
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )

#ifndef REFCLOCKS_PER_MINUTE
#define REFCLOCKS_PER_MINUTE 600000000
#endif

interface IDirectMusicPerformance;
class CContextMenuHandler;
class CTimeStrip;

struct StripMgrList
{
	struct StripMgrList*	m_pNext;
	IDMUSProdStripMgr*		m_pStripMgr;
	DWORD					m_dwGroupBits;

	StripMgrList(IDMUSProdStripMgr* pStripMgr, DWORD dwGroupBits)
	{
		ASSERT( pStripMgr );
		m_pStripMgr = pStripMgr;
		pStripMgr->AddRef();
		m_pNext = NULL;
		m_dwGroupBits = dwGroupBits;
	};

	~StripMgrList()
	{
		ASSERT( m_pStripMgr );
		m_pStripMgr->Release();
	};
};

struct StripList
{
	struct StripList*	m_pNext;
	IDMUSProdStrip*		m_pStrip;
	long				m_lHeight;	// height of strip
	long				m_lVScroll; // amount of vertical scroll
	long				m_lRestoreHeight;	// height of strip to retore to when maximizing it
	STRIPVIEW			m_sv;
	BOOL				m_fSelected;
	DWORD				m_dwGroupBits;
	DWORD				m_dwIndex;
	CLSID				m_clsidType;

	StripList(IDMUSProdStrip* pStrip, REFCLSID clsidype)
	{
		ASSERT( pStrip );
		m_pStrip = pStrip;
		pStrip->AddRef();
		m_pNext = NULL;
		m_lHeight = MIN_STRIP_HEIGHT;
		m_lVScroll = 0;
		m_lRestoreHeight = MIN_STRIP_HEIGHT;
		m_sv = SV_NORMAL;
		m_fSelected = FALSE;
		m_dwGroupBits = 1;
		m_dwIndex = 0;
		memcpy( &m_clsidType, &clsidype, sizeof(GUID) );
	};

	~StripList()
	{
		ASSERT( m_pStrip );
		m_pStrip->Release();
	}
};

struct NotifyListEntry
{
	IDMUSProdStripMgr*	pIStripMgr;
	DWORD				dwGroupBits;

	NotifyListEntry(IDMUSProdStripMgr* pINewStripMgr, DWORD dwNewGroupBits)
	{
		pIStripMgr = pINewStripMgr;
		dwGroupBits = dwNewGroupBits;
	}
};

struct NotifyEntry
{
	NotifyEntry( IDMUSProdStripMgr* pIStripMgr, REFGUID guid, DWORD dwGroupBits )
	{
		memcpy( &m_guid, &guid, sizeof(GUID) );

		NotifyListEntry* pNotifyListEntry = new NotifyListEntry( pIStripMgr, dwGroupBits);
		m_lstNotifyListEntry.AddHead( pNotifyListEntry );
	}

	~NotifyEntry()
	{
		while ( !m_lstNotifyListEntry.IsEmpty() )
		{
			NotifyListEntry* pNotifyListEntry = m_lstNotifyListEntry.RemoveHead();
			delete pNotifyListEntry;
		};
	}

	GUID		m_guid;
    CTypedPtrList<CPtrList, NotifyListEntry*> m_lstNotifyListEntry;
};

int CompareStrips( int nType1, DWORD dwGroups1, DWORD dwIndex1, BSTR bstrName1, int nType2, DWORD dwGroups2, DWORD dwIndex2, BSTR bstrName2 );
int StripCLSIDToInt( REFCLSID clsidType );
int StripCKIDsToInt( FOURCC ckid, FOURCC fccType );
void InsertMenuInMenu( CMenu* pMenu1, UINT nPos, CMenu* pMenu2);
const StripList *GetStripList( const class CPrivateTimelineCtl *pCPrivateTimelineCtl );
HRESULT WINAPI CPrivateTimelineCtlQI(void* pv, REFIID riid, LPVOID* ppv, DWORD dw);


/////////////////////////////////////////////////////////////////////////////
// CPrivateTimelineCtl
class ATL_NO_VTABLE CPrivateTimelineCtl : 
	public IDMUSProdTimeline,
	public IDMUSProdPrivateTimelineCtl,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPrivateTimelineCtl, &CLSID_PrivateTimelineCtl>
{
	friend CContextMenuHandler;
	friend CTimelineCtl;
	friend CTimeStrip;
	friend const StripList *GetStripList( const CPrivateTimelineCtl *pCPrivateTimelineCtl );
	friend HRESULT WINAPI CPrivateTimelineCtlQI(void* pv, REFIID riid, LPVOID* ppv, DWORD dw);

public:
	CPrivateTimelineCtl();
	~CPrivateTimelineCtl();

DECLARE_NO_REGISTRY()
	HRESULT FinalConstruct();

BEGIN_COM_MAP(CPrivateTimelineCtl)
	COM_INTERFACE_ENTRY(IDMUSProdTimeline)
	COM_INTERFACE_ENTRY_IID(IID_ICPrivateTimelineCtl, CPrivateTimelineCtl)
	COM_INTERFACE_ENTRY_IID(IID_IDMUSProdPrivateTimelineCtl, IDMUSProdPrivateTimelineCtl)
	COM_INTERFACE_ENTRY_FUNC_BLIND(0, CPrivateTimelineCtlQI)
END_COM_MAP()

// don't forget to add any new messages that might go to strips
// to the CallStripWMMessage function.
BEGIN_MSG_MAP(CPrivateTimelineCtl)
END_MSG_MAP()


// IDMUSProdTimeline
public:
    STDMETHODIMP AddStripMgr( 
        /* [in] */ IDMUSProdStripMgr* pIStripMgr,
		/* [in] */ DWORD			  dwGroupBits);
    
    STDMETHODIMP AddStrip( 
        /* [in] */ IDMUSProdStrip* pIStrip);
    
    STDMETHODIMP SetMarkerTime( 
        /* [in] */ MARKERID idMarkerType,
		/* [in] */ TIMETYPE	ttType,
        /* [in] */ long lTime);
    
    STDMETHODIMP GetMarkerTime( 
        /* [in] */ MARKERID idMarkerType,
		/* [in] */ TIMETYPE	type,
        /* [out] */ long *plTime);
    
    STDMETHODIMP ClocksToPosition( 
        /* [in] */ long lTime,
        /* [out] */ long *plPosition);
    
    STDMETHODIMP PositionToClocks( 
        /* [in] */ long lPosition,
        /* [out] */ long *plTime);

	STDMETHODIMP DrawMusicLines(
		/* [in] */ HDC					hdc,
		/* [in] */ MUSICLINE_PROPERTY	mlp,
		/* [in] */ DWORD				dwGroupBits,
		/* [in] */ DWORD				dwIndex,
		/* [in] */ LONG					lXOffset);

	STDMETHODIMP SetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [in] */ VARIANT			var
		);

	STDMETHODIMP Refresh(
			void
		);

	STDMETHODIMP GetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [out] */ VARIANT*			pvar
		);

    STDMETHODIMP ClocksToMeasureBeat( 
		/* [in] */  DWORD	dwGroupBits,
		/* [in] */  DWORD	dwIndex,
        /* [in] */  long	lTime,
        /* [out] */ long*	plMeasure,
        /* [out] */ long*	plBeat);
    
    STDMETHODIMP PositionToMeasureBeat( 
		/* [in] */  DWORD	dwGroupBits,
		/* [in] */  DWORD	dwIndex,
        /* [in] */  long	lPosition,
        /* [out] */ long*	plMeasure,
        /* [out] */ long*	plBeat);
    
    STDMETHODIMP MeasureBeatToClocks( 
		/* [in] */  DWORD	dwGroupBits,
		/* [in] */  DWORD	dwIndex,
        /* [in] */  long	lMeasure,
        /* [in] */  long	lBeat,
        /* [out] */ long*	plTime);
    
    STDMETHODIMP MeasureBeatToPosition( 
		/* [in] */  DWORD	dwGroupBits,
		/* [in] */  DWORD	dwIndex,
        /* [in] */  long	lMeasure,
        /* [in] */  long	lBeat,
        /* [out] */ long*	plPosition);

	STDMETHODIMP StripInvalidateRect(
		/* [in] */ IDMUSProdStrip*	pIStrip,
		/* [in] */ RECT*			pRect,
		/* [in] */ BOOL				fErase);

	STDMETHODIMP SetPropertyPage(
		/* [in] */ IUnknown* punkPropPageMgr,
		/* [in] */ IUnknown* punkPropPageObj);

	STDMETHODIMP RemovePropertyPageObject(
		/* [in] */ IUnknown* punkPropPageObj );

    STDMETHODIMP StripSetTimelineProperty( 
        /* [in] */ IDMUSProdStrip* punkStrip,
        /* [in] */ STRIP_TIMELINE_PROPERTY stp,
        /* [in] */ VARIANT variant);

	STDMETHODIMP OnDataChanged(
		/* [in] */ IUnknown* punk);

	STDMETHODIMP TrackPopupMenu(
		/* [in] */ HMENU			hMenu,
		/* [in] */ long				lXpos,
		/* [in] */ long				lYpos,
		/* [in] */ IDMUSProdStrip*	pIstrip,
		/* [in] */ BOOL				bEditMenu);
		
	STDMETHODIMP ClocksToRefTime(
		/* [in] */  long			lTime,
		/* [out] */ REFERENCE_TIME*	pRefTime);
	
	STDMETHODIMP PositionToRefTime(
		/* [in] */  long			lPosition,
		/* [out] */ REFERENCE_TIME*	pRefTime);
	
	STDMETHODIMP MeasureBeatToRefTime(
		/* [in] */  DWORD			dwGroupBits,
		/* [in] */  DWORD			dwIndex,
		/* [in] */  long			lMeasure,
		/* [in] */  long			lBeat,
		/* [out] */ REFERENCE_TIME*	pRefTime);
	
	STDMETHODIMP RefTimeToClocks(
		/* [in] */  REFERENCE_TIME	RefTime,
		/* [out] */ long*			plTime);
	
	STDMETHODIMP RefTimeToPosition(
		/* [in] */  REFERENCE_TIME	RefTime,
		/* [out] */ long*			plPosition);
	
	STDMETHODIMP RefTimeToMeasureBeat(
		/* [in] */  DWORD			dwGroupBits,
		/* [in] */  DWORD			dwIndex,
		/* [in] */  REFERENCE_TIME	RefTime,
		/* [out] */ long*			plMeasure,
		/* [out] */ long*			plBeat);

	STDMETHODIMP ScreenToStripPosition(
		/* [in] */  IDMUSProdStrip*	pIStrip,
		/* [in] [out] */ POINT*	pPoint);

	STDMETHODIMP StripGetTimelineProperty(
		/* [in] */	IDMUSProdStrip*	pIStrip,
		/* [in] */	STRIP_TIMELINE_PROPERTY stp,
		/* [out] */	VARIANT*	pvar);

	STDMETHODIMP RemoveStripMgr(
		/* [in] */	IDMUSProdStripMgr* pIStripMgr);

	STDMETHODIMP RemoveStrip(
		/* [in] */	IDMUSProdStrip* pIStrip);

	STDMETHODIMP GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */  DWORD		dwIndex,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData);

	STDMETHODIMP SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ DWORD		dwGroupBits,
		/* [in] */ DWORD		dwIndex,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData);

	STDMETHODIMP GetStripMgr(
		/* [in] */ REFGUID				guidType,
		/* [in] */ DWORD				dwGroupBits,
		/* [in] */ DWORD				dwIndex,
		/* [out,retval] */ IDMUSProdStripMgr**	ppStripMgr);

	STDMETHODIMP InsertStripAtDefaultPos(
		/* [in] */  IDMUSProdStrip*	pStrip,
		/* [in] */  REFCLSID		clsidType,
		/* [in] */  DWORD			dwGroupBits,
		/* [in] */  DWORD			dwIndex);

	STDMETHODIMP EnumStrip(
		/* [in] */  DWORD				dwEnum,
		/* [in] */  IDMUSProdStrip**	ppStrip);

	STDMETHODIMP InsertStripAtPos(
		/* [in] */  IDMUSProdStrip*		pStrip,
		/* [in] */  DWORD				dwPosition);

	STDMETHODIMP StripToWindowPos(
		/* [in] */  IDMUSProdStrip*		pStrip,
		/* [in,out] */ POINT*			pPoint);

	STDMETHODIMP AddToNotifyList(
		/* [in] */  IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */  REFGUID				rguidType,
		/* [in] */  DWORD				dwGroupBits);

	STDMETHODIMP RemoveFromNotifyList(
		/* [in] */  IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */  REFGUID				rguidType,
		/* [in] */  DWORD				dwGroupBits);

	STDMETHODIMP NotifyStripMgrs(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */  void*		pData);

	STDMETHODIMP AllocTimelineDataObject(
		/* [out,retval] */ IDMUSProdTimelineDataObject**	ppITimelineDataObject );

	STDMETHODIMP GetPasteType(
		/* [out,retval] */ TIMELINE_PASTE_TYPE*	ptlptPasteType );

	STDMETHODIMP SetPasteType(
		/* [in] */ TIMELINE_PASTE_TYPE tlptPasteType );

// IDMUSProdPrivateTimelineCtl functions
public:
	STDMETHODIMP SetTimelineOleCtl( IUnknown *punkTimelineOleCtl );
    STDMETHODIMP FinalCleanUp();

// private member variables
private:
	IDMUSProdFramework* m_pDMUSProdFramework;	// framework pointer set by SetTimelineProperty
	IDMUSProdTimelineCallback*	m_pTimelineCallback; // set by the component, this is an interface on the component
	StripMgrList*	m_pStripMgrList; // list of strip managers
	StripList*		m_pStripList;	// list of strips
	long			m_lLength;  // length of piece (zero is infinite length). In clocks for now.
	long			m_lXScrollPos;
	long			m_lYScrollPos;
	long			m_lFunctionBarWidth; // includes borders
	double			m_dblZoom; // zoom factor
	const StripList*m_pActiveStripList; // the currently active strip
	BOOL			m_fFreezeUndo; // true when a strip wants to prevent undo entries from being created
	BOOL			m_fMusicTime; // true when we're displaying in Music time.
	IDMUSProdTempoMapMgr* m_pTempoMapMgr;	// Pointer to the tempo-map manager, if there is one
	double			m_dblRefTimeConv; // number used to convert from RefTime to position
    CTypedPtrList<CPtrList, NotifyEntry*> m_lstNotifyEntry;
	TIMELINE_PASTE_TYPE m_ptPasteType;
	CRITICAL_SECTION m_csOnDataChanged;
	CTimeStrip*		m_pTimeStrip;
	CTimelineCtl	*m_pTimelineCtl;
	IDirectMusicPerformance* m_pIDMPerformance;
	bool			m_fInSetTimelineOleCtl;

// protected member methods
protected:
	long TotalStripHeight(void) const;
	void ActivateStripList( const StripList* pSL );
	void DeactivateStripList( void );
	void CallStripWMMessage( const StripList* pSL, UINT nMsg, WPARAM wParam, LPARAM lParam );
	StripList* FindStripList(IDMUSProdStrip*) const;
	HRESULT StripPointToClient( const StripList* pSL, POINT *pPoint) const;
	HRESULT StripRectToClient( const StripList* pSL, LPRECT pRect) const;
	HRESULT InternalInsertStripAtPos( IDMUSProdStrip* pStrip, DWORD dwPosition, REFCLSID clsidType, DWORD dwGroupBits, DWORD dwIndex );
	bool AnyGutterSelectedStrips( void ) const;
	bool ComputeEarlyAndLateTime( long &lEarlyTime, long &lLateTime ) const;
};

#endif //__PRIVATETIMELINECTL_H_

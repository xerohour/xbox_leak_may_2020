#ifndef __TIMELINECTL_H_
#define __TIMELINECTL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TimelineCtl.h : Declaration of the CTimelineCtl

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
#include "dmusici.h"
#include "dmusicf.h"

#ifndef REFCLOCKS_PER_MINUTE
#define REFCLOCKS_PER_MINUTE 600000000
#endif

interface IDirectMusicPerformance;

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
		m_lHeight = 100;
		m_lVScroll = 0;
		m_lRestoreHeight = 100;
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
};

struct NotifyEntry
{
	NotifyEntry( IDMUSProdStripMgr* pIStripMgr, REFGUID guid, DWORD dwGroupBits )
	{
		memcpy( &m_guid, &guid, sizeof(GUID) );
		m_lstNotifyListEntry.RemoveAll();

		NotifyListEntry* pNotifyListEntry;
		pNotifyListEntry = new NotifyListEntry;
		pNotifyListEntry->pIStripMgr = pIStripMgr;
		//pIStripMgr->AddRef();
		pNotifyListEntry->dwGroupBits = dwGroupBits;

		m_lstNotifyListEntry.AddHead( pNotifyListEntry );
	}

	~NotifyEntry()
	{
		NotifyListEntry* pNotifyListEntry = NULL;
		while ( !m_lstNotifyListEntry.IsEmpty() )
		{
			pNotifyListEntry = m_lstNotifyListEntry.RemoveHead();
			if ( pNotifyListEntry != NULL )
			{
				delete pNotifyListEntry;
			}
		};
	}

	GUID		m_guid;
    CTypedPtrList<CPtrList, NotifyListEntry*> m_lstNotifyListEntry;
};

int CompareStrips( int nType1, DWORD dwGroups1, DWORD dwIndex1, BSTR bstrName1, int nType2, DWORD dwGroups2, DWORD dwIndex2, BSTR bstrName2 );
int StripCLSIDToInt( REFCLSID clsidType );
int StripCKIDsToInt( FOURCC ckid, FOURCC fccType );
void InsertMenuInMenu( CMenu* pMenu1, UINT nPos, CMenu* pMenu2);

class CContextMenuHandler;
class CTimeStrip;

/////////////////////////////////////////////////////////////////////////////
// CTimelineCtl
class ATL_NO_VTABLE CTimelineCtl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineCtl, &CLSID_TimelineCtl>,
	public CComControl<CTimelineCtl>,
	public IDMUSProdTimeline,
	public IDMUSProdTimelineEdit,
	public IDropTarget,
	public IProvideClassInfo2Impl<&CLSID_TimelineCtl, NULL, &LIBID_TIMELINELib>,
	public IPersistStreamInitImpl<CTimelineCtl>,
	public IPersistStorageImpl<CTimelineCtl>,
	public IQuickActivateImpl<CTimelineCtl>,
	public IOleControlImpl<CTimelineCtl>,
	public IOleObjectImpl<CTimelineCtl>,
	public IOleInPlaceActiveObjectImpl<CTimelineCtl>,
	public IViewObjectExImpl<CTimelineCtl>,
	public IOleInPlaceObjectWindowlessImpl<CTimelineCtl>,
	public IDataObjectImpl<CTimelineCtl>,
	public ISpecifyPropertyPagesImpl<CTimelineCtl>,
	public IDMUSProdPropPageObject
{
	friend CContextMenuHandler;

public:
	CTimelineCtl();
	~CTimelineCtl();

static CWndClassInfo& GetWndClassInfo();


DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINECTL)

BEGIN_COM_MAP(CTimelineCtl)
	COM_INTERFACE_ENTRY(IDMUSProdTimeline)
	COM_INTERFACE_ENTRY(IDMUSProdTimelineEdit)
	COM_INTERFACE_ENTRY(IDropTarget)
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IQuickActivate)
	COM_INTERFACE_ENTRY_IMPL(IPersistStorage)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY_IMPL(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

BEGIN_PROPERTY_MAP(CTimelineCtl)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
END_PROPERTY_MAP()


// don't forget to add any new messages that might go to strips
// to the CallStripWMMessage function.
BEGIN_MSG_MAP(CTimelineCtl)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
	MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblclk)
	MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
	MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
	MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	MESSAGE_HANDLER(WM_CHAR, OnChar)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	COMMAND_HANDLER(BTN_ZOOMIN, BN_CLICKED, OnZoomIn)
	COMMAND_HANDLER(BTN_ZOOMOUT, BN_CLICKED, OnZoomOut)
	COMMAND_HANDLER(BTN_REALTIME, BN_CLICKED, OnRealTime)
	COMMAND_HANDLER(BTN_MUSICTIME, BN_CLICKED, OnMusicTime)
	//COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
	//COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
	//COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
	//COMMAND_ID_HANDLER(ID_EDIT_INSERT, OnEditInsert)
	//COMMAND_ID_HANDLER(ID_EDIT_DELETE, OnEditDelete)
	//COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
	//COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
	//COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectall)
END_MSG_MAP()

// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD* pdwStatus)
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}

// IDMUSProdTimeline
public:
	STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
	{
		ATLTRACE(_T("IOleObjectImpl::SetClientSite\n"));
		if( pClientSite == NULL )
		{
			StripMgrList* pList;
			for( pList = m_pStripMgrList; pList; pList = pList->m_pNext )
			{
				VARIANT varTimeline;
				varTimeline.vt = VT_UNKNOWN;
				V_UNKNOWN(&varTimeline) = NULL;
				pList->m_pStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
			}
		}
		return IOleObject_SetClientSite(pClientSite);
	}

	LRESULT OnCreate(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnDestroy(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnDrawItem(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnSize(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnHScroll(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnVScroll(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnMouseWheel(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);
	
	LRESULT OnMouseMove(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnKeyDown(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnChar(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnSetCursor(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);
	
	LRESULT OnLButtonDown(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnLButtonUp(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnRButtonDown(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnRButtonUp(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnLButtonDblclk(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnRealTime(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnMusicTime(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnEraseBkgnd(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnContextMenu(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);

	LRESULT OnTimer(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */);


//	LRESULT OnEditCopy(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

//	LRESULT OnEditCut(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

//	LRESULT OnEditPaste(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

//	LRESULT OnEditInsert(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

//	LRESULT OnEditDelete(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

//	LRESULT OnEditSelectall(WORD /* nMsg */, WORD /* wParam */,
//		HWND /* lParam */, BOOL& /* lResult */);

	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di);

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

// IDropTarget
public:
	HRESULT STDMETHODCALLTYPE DragEnter( 
        /* [in] */ IDataObject __RPC_FAR *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);
    
	HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);
    
	HRESULT STDMETHODCALLTYPE DragLeave(void);
    
	HRESULT STDMETHODCALLTYPE Drop( 
        /* [in] */ IDataObject __RPC_FAR *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);

// IDMUSProdTimelineEdit
public:
	HRESULT STDMETHODCALLTYPE Cut( /* out */ IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE Copy( /* out */ IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( /* in */ IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// IDMUSProdPropPageObject functions
public:
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

// private member variables
private:
	IDMUSProdFramework* m_pDMUSProdFramework;	// framework pointer set by SetTimelineProperty
	IDMUSProdTimelineCallback*	m_pTimelineCallback; // set by the component, this is an interface on the component
	StripMgrList*	m_pStripMgrList; // list of strip managers
	StripList*		m_pStripList;	// list of strips
	CWnd			m_wnd;				// the timeline window
	CScrollBar		m_ScrollHorizontal; // bottom scroll bar
	CScrollBar		m_ScrollVertical; // right scroll bar
	CButton			m_BtnZoomIn; // zoom in button
	CButton			m_BtnZoomOut; // zoom out button
	long			m_lLength;  // length of piece (zero is infinite length). In clocks for now.
	long			m_lBeginSelect; // clock-time of begin of selection (inclusive)
	long			m_lEndSelect; // clock-time of end of selection (not inclusive)
	long			m_lCursor; // clock-time of the cursor
	long			m_lXScrollPos;
	long			m_lYScrollPos;
	long			m_lFunctionBarWidth; // includes borders
	double			m_dblZoom; // zoom factor
	MOUSEMODE		m_MouseMode; // current Mouse Mode
	long			m_lResizeYPos; // current position of resize xor line
	long			m_lResizeOriginalYPos; // same as m_lResizeYPos when we first click with the mouse
	StripList*		m_pMouseStripList; // the strip getting resized or whatever
	StripList*		m_pActiveStripList; // the currently active strip
	StripList*		m_pLastDragStripList; // the strip which the cursor is currently over during a drag operation
	BOOL			m_fStripMouseCapture; // true when a strip wants all mouse events
	BOOL			m_fFreezeUndo; // true when a strip wants to prevent undo entries from being created
	BOOL			m_bPasting;	// true when pasting so the selection range doesn't change.
	BOOL			m_fMusicTime; // true when we're displaying in Music time.
	BOOL			m_fHScrollTracking; // true when we're dragging the HScroll thumb
	BOOL			m_fActiveGutterState; // true if gutters should be selected during MM_ACTIVEGUTTER movement
	COLORREF		m_colorMeasureLine;
	COLORREF		m_colorBeatLine;
	COLORREF		m_colorSubBeatLine;
	IDataObject*	m_pCurrentDataObject; // Pointer to the data object being dragged over the control
	IDataObject*	m_pCopyDataObject; // Pointer to the data object last copied.
	DWORD			m_dwDragScrollTick;		// Used to scroll timeline during OLE drag
	CContextMenuHandler	  m_MenuHandler;	// menu handler for the pop-up menus
	IDMUSProdTempoMapMgr* m_pTempoMapMgr;	// Pointer to the tempo-map manager, if there is one
	double			m_dblRefTimeConv; // number used to convert from RefTime to position
	static long		m_lBitmapRefCount;
	static CBitmap	m_BitmapZoomIn;
	static CBitmap	m_BitmapZoomOut;
	static CBitmap	m_BitmapMinimize;
	static CBitmap	m_BitmapMinimized;
	static CBitmap	m_BitmapMaximize;
	static CBitmap	m_BitmapMaximized;
	static CString	m_strZoomInChars;
	static CString	m_strZoomOutChars;
	SIZE			m_sizeMinMaxButton;
    CTypedPtrList<CPtrList, NotifyEntry*> m_lstNotifyEntry;
	TIMELINE_PASTE_TYPE m_ptPasteType;
	CRITICAL_SECTION m_csOnDataChanged;
	REFERENCE_TIME	m_rtSegmentStart;
	REFERENCE_TIME	m_rtLastElapsedTime;
	IDirectMusicPerformance* m_pIDMPerformance;
	HANDLE			m_hKeyRealTimeStatusBar;
	CTimeStrip*		m_pTimeStrip;
	long			m_lLastEarlyPosition;
	long			m_lLastLatePosition;
	long			m_lLastLateTime;
	static bool		m_fFilledColorArrays;
	static DWORD	m_adwNormalColor[256];
	static DWORD	m_adwActiveColor[256];
	/*
	static DWORD	m_adwSelectedColor[256];
	static DWORD	m_adwActiveSelectedColor[256];
	*/

// public helper methods
public:
	bool ShouldEnableDeleteTrack( void );

// protected member methods
protected:
	long ComputeXScrollPos(void); // returns, in screen coordinates, the horizontal scroll factor
	void ComputeDrawingArea( LPRECT pRect ) const;
	long PositionToXScroll(long); // converts from position to scroll bar
	void ScrollToTimeCursor( long lTime, long lLeftSide, long lRightSide );
	void SetTimeCursor( long lTime, BOOL fScroll, bool fUserSetCursor );
	void SetStripGutter( StripList* pSL, BOOL fSelect );
	void GetStripClientRect( StripList* pSL, LPRECT pRect );
	void GetEntireStripClientRect( StripList* pSL, LPRECT pRect );
	void InvalidateSelectedStrips(BOOL fErase);
	void ComputeScrollBars(void);
	long TotalStripHeight(void) const;
	void ActivateStripList( StripList* pSL );
	void DeactivateStripList( void );
	void SetTopStripList( StripList* pSL );
	StripList *GetTopStripList( void );
	void OptimizeZoomFactor(void);
	StripList* FindStripList(IDMUSProdStrip*) const;
	void CallStripWMMessage( StripList* pSL, UINT nMsg, WPARAM wParam, LPARAM lParam );
	void GetStripAndCoordinates( POINTL ptTimeline, POINTL *ptStrip, StripList **ppSL );
	void ScrollToPosition( long lPos );
	HRESULT CutCopy( BOOL bCut, IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT CanEdit( long lCommand );
	void SetMouseMode( long xPos, long yPos);
	POINT GetMousePoint() const;
	HRESULT StripPointToClient( StripList* pSL, POINT *pPoint) const;
	HRESULT StripRectToClient( StripList* pSL, LPRECT pRect) const;
	HRESULT InternalInsertStripAtPos( IDMUSProdStrip* pStrip, DWORD dwPosition, REFCLSID clsidType, DWORD dwGroupBits, DWORD dwIndex );
	bool AnyGutterSelectedStrips( void ) const;
	HRESULT CanStripEdit( IDMUSProdTimelineEdit* pITimelineEdit, long lCommand );
	void DisplayPropertySheet( void );
	void ScrollTimeline( POINTL point );
	bool ComputeEarlyAndLateTime( long &lEarlyTime, long &lLateTime );
	bool BumpTimeCursor( bool fMoveRight, DMUSPROD_TIMELINE_SNAP_TO stAmount );
	void GetTempoList( CList<DMUS_TEMPO_PARAM,DMUS_TEMPO_PARAM&> &lstTempos, long lTime );
	void GetTempoList( CList<DMUS_TEMPO_PARAM,DMUS_TEMPO_PARAM&> &lstTempos, REFERENCE_TIME rtTime );
};

#endif //__TIMELINECTL_H_

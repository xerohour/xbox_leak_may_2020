// SignPostMgr.h : Declaration of the CSignPostMgr

#ifndef __SIGNPOSTMGR_H_
#define __SIGNPOSTMGR_H_


#include "resource.h"       // main symbols
#include "DllJazzDataObject.h"
#include "timeline.h"
#include "SignPostPPGMgr.h"
#include "SignPostIO.h"
#include <dmusici.h>
#include <afxcmn.h>

#define CF_SIGNPOST "Microsoft Jazz v.1 SignPost"

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | DMUS_TRACKCONFIG_PLAY_COMPOSE | DMUS_TRACKCONFIG_LOOP_COMPOSE | DMUS_TRACKCONFIG_COMPOSING | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

class CSignPostStripMgrApp;
interface IDMUSProdFramework;

extern CSignPostStripMgrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSignPostStripMgrApp
class CSignPostStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

/////////////////////////////////////////////////////////////////////////////
// CSignPostMgr
class ATL_NO_VTABLE CSignPostMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSignPostMgr, &CLSID_SignPostMgr>,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend class CSignPostStrip;
public:
	CSignPostMgr()
	{
		m_pTimeline					= NULL;
		m_pSignPostList				= NULL;
		m_pMeasureArray				= NULL;
		m_pIPageManager				= NULL;
		m_pDragDataObject			= NULL;
		m_pCopyDataObject			= NULL;
		m_pSignPostStrip			= NULL;
		m_bSelected					= FALSE;
		m_PPGSignPost.lMeasure		= 0;
		m_PPGSignPost.dwValid		= 0;
		m_PPGSignPost.dwSignPost	= 0;
		m_nMeasures					= 0;
		m_lShiftFromMeasure			= 0;
		m_dwGroupBits				= 1;
		m_dwOldGroupBits			= 1;
		m_dwTrackExtrasFlags		= DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
		m_dwProducerOnlyFlags		= 0;
		m_pIDMTrack					= NULL;
		m_pTempIStream				= NULL;
		m_pIFramework				= NULL;
		m_dwActivityLevel			= 4;
	}
	~CSignPostMgr()
	{
		ClearSignPostList(m_pSignPostList);

		if(m_pMeasureArray != NULL)
		{
			delete [] m_pMeasureArray;
		}

		if(m_pIFramework != NULL)
		{
			m_pIFramework->Release();
			m_pIFramework = NULL;
		}
		if(m_pIPageManager != NULL)
		{
			m_pIPageManager->Release();
			m_pIPageManager = NULL;
		}
		if( m_pIDMTrack )
		{
			m_pIDMTrack->Release();
		}
		if( m_pTempIStream )
		{
			m_pTempIStream->Release();
		}
		if(m_pDragDataObject)
		{
			m_pDragDataObject->Release();
			m_pDragDataObject = NULL;
		}
		if(m_pCopyDataObject)
		{
			if(S_OK == OleIsCurrentClipboard(m_pCopyDataObject))
			{
				OleFlushClipboard();
			}
			m_pCopyDataObject->Release();
			m_pCopyDataObject = NULL;
		}
	}

protected:
	IDMUSProdTimeline			*m_pTimeline;
	struct SignPostExt			*m_pSignPostList;
	struct SignPostMeasureInfo	*m_pMeasureArray;	// SignPostMeasureInfo[m_nMeasures]
	IDMUSProdPropPageManager		*m_pIPageManager;
	BOOL						m_bSelected;
	PPGSignPost					m_PPGSignPost;
	CDllJazzDataObject			*m_pDragDataObject;
	IDataObject					*m_pCopyDataObject;
	long						m_lShiftFromMeasure;
	short						m_nMeasures;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	class CSignPostStrip		*m_pSignPostStrip;
	IStream						*m_pTempIStream;
	IDMUSProdFramework*			m_pIFramework;
	DWORD						m_dwActivityLevel;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_SIGNPOSTMGR)

BEGIN_COM_MAP(CSignPostMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()


// IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

// IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

// IDMUSProdStripMgr
public:
	HRESULT STDMETHODCALLTYPE IsParamSupported(
		/* [in] */ REFGUID		guidType);
	HRESULT STDMETHODCALLTYPE GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData);
	HRESULT STDMETHODCALLTYPE SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData);
	HRESULT STDMETHODCALLTYPE OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData);
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant);
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY smp,
		/* [in] */ VARIANT		variant);

protected:
	SignPostExt* GetFirstSelectedSignPost();
	void UnMarkAtMeasure(DWORD measure, WORD flags);
	HRESULT SaveDroppedSignPostList(IStream* pStream, BOOL bOnlySelected, short nOffsetMeasure);
	void UnMarkSignPosts(WORD flags);
	void DeleteMarkedSignPosts(DWORD flags);
	short MarkSelectedSignPosts(DWORD flags);

protected:
	HRESULT SyncWithDirectMusic(  );
	void UpdateSegment( void );
	HRESULT VerifyMeasureArray();
	HRESULT LoadSignPostList(IStream* pStream, long lDropPos, BOOL bDrop, BOOL &fChanged );
	HRESULT SaveSignPostList( IStream* pIStream, BOOL bOnlySelected, bool bNormalize=false );
	HRESULT InsertSignPost(long lMeasure);
	void	ClearSignPostList(SignPostExt *pSignPostList, BOOL bClearSelection = TRUE);
	void	SelectSegment(long lBeginTime, long lEndTime);
	void	ClearSelected();
	void	SelectAll();
	void	DeleteSelected();
	void	ToggleSelect(long lMeasure);
	BOOL	IsSelected(long lMeasure);
	short	GetFirstSelectedMeasure();
	void	GetBoundariesOfSelectedCommands( long *plStart, long *plEnd );
	BOOL	DeleteBetweenTimes( long lStart, long lEnd );

	SignPostExt*	GetSignPostAtMeasure(long lMeasure)
	{
		ASSERT(m_pMeasureArray);
		if(lMeasure >= 0 && lMeasure < m_nMeasures)
		{
			return m_pMeasureArray[lMeasure].pSignPost;
		}
		else
		{
			return 0;
		}
	}
private:
	void RecalculateMusicTimeValues();
	void RemoveOrphanSignposts();
	void SyncMeasureArray();
	HRESULT ResizeMeasureArray();
};


class CSignPostStrip : 
	public IDMUSProdStrip,
	public IDMUSProdTimelineEdit,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdPropPageObject,
	public IDropSource, 
	public IDropTarget
{
friend class CSignPostMgr;
protected:
	long				m_cRef;
	CSignPostMgr		*m_pSignPostMgr;
	IDMUSProdStripMgr	*m_pStripMgr;
	BOOL				m_fSelected;
	BOOL				m_bSelecting;
	BOOL				m_bContextMenuPaste;
	long				m_lBeginSelect;
	long				m_lEndSelect;
	UINT				m_cfFormat;
	CBitmap				m_bmCompose;
	BOOL				m_fLButtonDown;
	BOOL				m_fComposeButtonDown;
	BOOL				m_fInAllTracksAdded;

	// drag/drop support
	IDataObject*	m_pISourceDataObject;	// Object being dragged	
	IDataObject*	m_pITargetDataObject;
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	LONG			m_startDragPosition;	// xpos where drag was started
	UINT			m_nLastEdit;			// resource id of last edit
	short			m_nStripIsDragDropSource;
	bool			m_bWasSelected;
public:
	CSignPostStrip( CSignPostMgr* pSignPostMgr )
	{
		RECT rect;

		rect.top = 0;
		rect.bottom = 0;
		rect.left = 0;
		rect.right = 0;
		ASSERT( pSignPostMgr );
		m_pSignPostMgr = pSignPostMgr;
		m_pStripMgr = (IDMUSProdStripMgr*)pSignPostMgr;
		//if(pSignPostMgr)
		//{
		//	m_pStripMgr->AddRef();
		//}
		m_cRef = 0;
		AddRef();
		m_fSelected = FALSE;
		m_bSelecting = FALSE;
		m_bContextMenuPaste = FALSE;
		m_bWasSelected = FALSE;	// used as signal for toggle select
		m_cfFormat = RegisterClipboardFormat(CF_SIGNPOST);
		m_lBeginSelect = 0;
		m_lEndSelect = 0;
		m_pISourceDataObject = NULL;
		m_pITargetDataObject = NULL;
		m_dwStartDragButton = 0;
		m_dwOverDragButton = 0;
		m_dwOverDragEffect = 0;
		m_pDragImage = NULL;
		m_dwDragRMenuEffect = DROPEFFECT_NONE;
		m_fShowGroupProps = FALSE;
		m_fPropPageActive = FALSE;
		m_pPropPageMgr = NULL;
		m_fLButtonDown = FALSE;
		m_fInAllTracksAdded = FALSE;
		m_nStripIsDragDropSource = 0;
		m_nLastEdit = 0;
		m_fComposeButtonDown = FALSE;

		// Initialize bitmap
		m_bmCompose.LoadBitmap( IDB_COMPOSE );

		BITMAP bmParam;
		m_bmCompose.GetBitmap( &bmParam );
		m_bmCompose.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
	};

	~CSignPostStrip()
	{
		//ASSERT(m_pStripMgr);
		//if(m_pStripMgr)
		//{
		//	m_pStripMgr->Release();
		//}
		if(m_pPropPageMgr)
		{
			m_pPropPageMgr->Release();
		}

		m_bmCompose.DeleteObject();
	}

// IUnknown
public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

// IDMUSProdStrip
public:
	HRESULT	STDMETHODCALLTYPE	Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
    HRESULT STDMETHODCALLTYPE	GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
    HRESULT STDMETHODCALLTYPE	SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE	OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDropTarget
public:
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
// IDropTarget helpers
protected:
	CWnd* GetTimelineCWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteAt( IDataObject* pIDataObject, POINTL pt);

// IDropSource
public:
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );
// IDropSource helpers
protected:
	CImageList* CreateDragImage();
	HRESULT		CreateDataObject(IDataObject**, long measure);
	HRESULT		DoDragDrop(WPARAM mousekeybutton, LONG lXPos, long lmeasure);

// IDMUSProdStripFunctionBar
public:
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdTimelineEdit
public:
	HRESULT STDMETHODCALLTYPE Cut( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObjectt = NULL );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
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

// Other
protected:
	void SetEnabledItems(HMENU hMenu);
	void UnselectGutterRange( void );

	HRESULT OnRButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// Helper
private:
	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits
	long		m_lMouseXPos;			// second temp storage for keeping mouse position between
										// click states
	long		m_lMouseYPos;			// second temp storage for keeping mouse position between
										// click states
	BOOL		m_fShowGroupProps;		// if TRUE, show Group property page, else show SignPost property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	IDMUSProdPropPageManager*	m_pPropPageMgr;
};



#endif //__SIGNPOSTMGR_H_

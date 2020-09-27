// CommandMgr.h : Declaration of the CCommandMgr

#ifndef __COMMANDMGR_H_
#define __COMMANDMGR_H_

#include "resource.h"       // main symbols
#include <afxcmn.h>
//#include "JazzDataObject.h"
#include "timeline.h"
#include "CommandPPGMgr.h"
#include "FileIO.h"
#include "DllJazzDataObject.h"
#include <dmusici.h>
#include <mmsystem.h>

class CCommandStripMgrApp;
interface IDirectMusicTrack;

extern CCommandStripMgrApp theApp;

#define TRACKCONFIG_VALID_MASK ( DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

/////////////////////////////////////////////////////////////////////////////
// CCommandStripMgrApp
class CCommandStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

/////////////////////////////////////////////////////////////////////////////
// CCommandMgr
class ATL_NO_VTABLE CCommandMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCommandMgr, &CLSID_CommandMgr>,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend class CCommandStrip;
public:
	CCommandMgr();
	~CCommandMgr();
protected:
	IDMUSProdTimeline*			m_pTimeline;
	struct CommandExt*			m_pCommandList;
	struct CommandMeasureInfo*	m_pMeasureArray;	// CommandMeasureInfo[m_nMeasures]
	IDMUSProdPropPageManager*		m_pIPageManager;
	BOOL						m_bSelected;
	PPGCommand					m_PPGCommand;
	CDllJazzDataObject*			m_pDragDataObject;
	IDataObject*				m_pCopyDataObject;
	long						m_lShiftFromMeasure;
	short						m_nMeasures;
	CCommandStrip*				m_pCommandStrip;	// managed strip

public:

DECLARE_REGISTRY_RESOURCEID(IDR_COMMANDMGR)

BEGIN_COM_MAP(CCommandMgr)
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
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

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
	HRESULT SyncWithDirectMusic(  );
	HRESULT VerifyMeasureArray();
	HRESULT	LoadCommandList( IStream* pIStream );
	HRESULT	SaveCommandList( IStream* pIStream, BOOL bOnlySelected );
	HRESULT	InsertCommand( long lMeasure );
	void	ClearCommandList( CommandExt* pCommandList, BOOL bClearSelection = TRUE );
	void	SelectSegment( long lBeginTime, long lEndTime );
	void	ClearSelected();
	void	SelectAll();
	void	DeleteSelected();
	void	ToggleSelect( long lMeasure );
	BOOL	IsSelected( long lMeasure );
	short	GetFirstSelectedMeasure();
	CommandExt* GetFirstSelectedCommand();
	void	GetBoundariesOfSelectedCommands( long *plStart, long *plEnd );
	BOOL	DeleteBetweenTimes( long lStart, long lEnd );

	short	MarkSelectedCommands(DWORD flags);
	void	DeleteMarkedCommands(DWORD flags);
	void	UnMarkCommands(WORD flags);
	void	MarkAtMeasure(DWORD measure, WORD flags);
	void	UnMarkAtMeasure(DWORD measure, WORD flags);
	CommandExt*	GetCommandAtMeasure(long lMeasure = -1)
	{
		ASSERT(m_pMeasureArray);
		if(lMeasure >= 0 && lMeasure < m_nMeasures)
		{
			return m_pMeasureArray[lMeasure].pCommand;
		}
		else
		{
			return 0;
		}
	}
	HRESULT LoadDroppedCommandList( IStream* pStream, long lDropPos, bool fDragging, BOOL &fChanged );
	HRESULT	SaveDroppedCommandList( IStream* pIStream, BOOL bOnlySelected, short nOffsetMeasure );

	void UpdateSegment( void );

private:
	void RemoveOrphanCommands();
	void SyncMeasureArray();
	HRESULT ResizeMeasureArray();
	GUID		m_guidClassID;	// Class id of track.
	DWORD		m_dwPosition;	// Position in track list.
	DWORD		m_dwGroupBits;	// Group bits for track. 
	DWORD		m_dwOldGroupBits;
	DWORD		m_dwTrackExtrasFlags;
	DWORD		m_dwProducerOnlyFlags;

	FOURCC      m_ckid;			// chunk ID of track's data chunk if 0 fccType valid
	FOURCC      m_fccType;		// list type if NULL ckid valid
	IDirectMusicTrack*		m_pIDMTrack;
	IStream					*m_pTempIStream;
};

class CCommandStrip : 
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdPropPageObject,
	public IDMUSProdTimelineEdit,
	public IDropSource, 
	public IDropTarget
{
friend class CCommandMgr;
protected:
	long				m_cRef;
	CCommandMgr*		m_pCommandMgr;
	IDMUSProdStripMgr*	m_pStripMgr;
	BOOL				m_fSelected;
	BOOL				m_bSelecting;
	BOOL				m_bContextMenuPaste;
	long				m_lLastFirstMeasure;
	long				m_lBeginSelect;
	long				m_lEndSelect;
	CRect				m_rectInvalid;
	UINT				m_cfFormat;
	enum	{ off, source, target };
	int					m_nDragDropStatus;
	WPARAM				m_wParam;	// save message param for aborted drag/drop completion

	// drag/drop support
	IDataObject*	m_pISourceDataObject;			// Object being dragged	
	IDataObject*	m_pITargetDataObject;
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	UINT			m_cfCommandList;
	LONG			m_startDragPosition;	// xpos where drag was started

	// select support
	BOOL			m_bWasSelected;
	BOOL			m_fInAllTracksAdded;

public:
	CCommandStrip( CCommandMgr* pCommandMgr )
	{
		RECT rect;

		m_nDragDropStatus = off;

		m_bWasSelected = false;

		rect.top = 0;
		rect.bottom = 0;
		rect.left = 0;
		rect.right = 0;
		ASSERT( pCommandMgr );
		m_pCommandMgr = pCommandMgr;
		m_pStripMgr = (IDMUSProdStripMgr*)pCommandMgr;
		if( m_pStripMgr )
		{
			m_pStripMgr->AddRef();
		}
		m_cRef = 0;
		AddRef();
		m_fSelected = FALSE;
		m_bSelecting = FALSE;
		m_bContextMenuPaste = FALSE;
		m_cfFormat = RegisterClipboardFormat( CF_COMMANDLIST );
		m_lLastFirstMeasure = -1;
		m_lBeginSelect = 0;
		m_lEndSelect = 0;
		m_pISourceDataObject = NULL;
		m_pITargetDataObject = NULL;
		m_dwStartDragButton = 0;
		m_dwOverDragButton = 0;
		m_dwOverDragEffect = 0;
		m_pDragImage = NULL;
		m_dwDragRMenuEffect = DROPEFFECT_NONE;
		m_nLastEdit = 0;
		m_fShowGroupProps = FALSE;
		m_fPropPageActive = FALSE;
		m_pPropPageMgr = NULL;
		m_fLButtonDown = FALSE;
		m_fInAllTracksAdded = FALSE;
	};

	~CCommandStrip()
	{
		ASSERT( m_pStripMgr );
		if( m_pStripMgr )
		{
			m_pStripMgr->Release();
		}
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->Release();
		}
	}

// IUnknown
public:
    STDMETHODIMP QueryInterface( REFIID riid, LPVOID *ppv );
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
// IDMUSProdStrip
public:
	HRESULT	STDMETHODCALLTYPE	Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
    HRESULT STDMETHODCALLTYPE	GetStripProperty( STRIPPROPERTY sp, VARIANT* pvar );
    HRESULT STDMETHODCALLTYPE	SetStripProperty( STRIPPROPERTY sp, VARIANT var );
	HRESULT STDMETHODCALLTYPE	OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDropTarget
public:
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
// IDrop helpers
protected:
	HWND	GetTimelineHWND();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteAt( IDataObject* pIDataObject, long lPastePos, BOOL &fChanged );

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
	HRESULT STDMETHODCALLTYPE Cut( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject );
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
	void SetEnabledItems( HMENU hMenu );
	void UnselectGutterRange( void );

	HRESULT OnRButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos );
	HRESULT OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

private:
	HRESULT GiveTimelineFocus( void );
	UINT m_nLastEdit;		// holds string resource id of last edit operation
	HRESULT		OnShowProperties(IDMUSProdTimeline*);
	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits
	BOOL		m_fShowGroupProps;		// if TRUE, show Group property page, else show SignPost property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	BOOL		m_fLButtonDown;			// TRUE if the left mouse button is down

};
#endif //__COMMANDMGR_H_

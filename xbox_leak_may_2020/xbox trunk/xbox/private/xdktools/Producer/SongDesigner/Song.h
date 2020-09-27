#ifndef __SONG_H__
#define __SONG_H__

// Song.h : header file
//

#include <RiffStrm.h>
#include "SongComponent.h"
#include "SongDocType.h"
#include "FolderSegments.h"
#include "FolderToolGraphs.h"
#include "VirtualSegment.h"


// Next VirtualSegment has begun playing
#define WM_NEXT_VIRTUAL_SEGMENT	(WM_USER + 0xF0)
#define WM_STOP_VIRTUAL_SEGMENT	(WM_USER + 0xF1)

// VirtualSegment min/max length
#define MIN_VIRTUAL_SEGMENT_LENGTH		1
#define MAX_VIRTUAL_SEGMENT_LENGTH		999

// m_dwBitsUI flags
#define BF_SELECTED						0x00000001

// SyncSongEditor flags
#define SSE_SOURCE_SEGMENT_LIST				0x00000001
#define SSE_VIRTUAL_SEGMENT_LIST			0x00000002
#define SSE_TOOLGRAPH_LIST					0x00000004
#define SSE_TRACK_LIST						0x00000008
#define SSE_TRANSITION_LIST					0x00000010
#define SSE_REDRAW_VIRTUAL_SEGMENT_LIST		0x00000020
#define SSE_REDRAW_TRACK_LIST				0x00000040
#define SSE_REDRAW_TRANSITION_LIST			0x00000080
#define SSE_SELECTED_VIRTUAL_SEGMENT		0x00000100
#define SSE_INVALIDATE_VIRTUAL_SEGMENT_LIST	0x00000200
#define SSE_ALL								0x00FFFFFF
#define SSE_EMPTY_ALL_LISTS					0x01000000

// SyncChanges flags
#define SC_DIRECTMUSIC				0x00000001
#define SC_PROPERTIES				0x00000002
#define SC_EDITOR					0x00000004
#define SC_ALL						0xFFFFFFFF


// Flags for property tabs
#define	PROPF_NONE_SELECTED			0x00000001
#define	PROPF_ONE_SELECTED			0x00000002
#define	PROPF_MULTI_SELECTED		0x00000004
#define	PROPF_HAVE_AUDIOPATH		0x00001000
#define	PROPF_TIME_CONVERSIONS_OK	0x00002000


class CDirectMusicSong;
class CSongCtrl;


//////////////////////////////////////////////////////////////////////
//  CNotificationHandler

class CNotificationHandler : public CWnd
{
// Construction
public:
	CNotificationHandler();
	virtual ~CNotificationHandler();

private:
	LRESULT OnNextVirtualSegment( WPARAM wParam, LPARAM lParam );
	LRESULT OnStopVirtualSegment( WPARAM wParam, LPARAM lParam );

// Attributes
public:
	CDirectMusicSong*	m_pSong;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotificationHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CNotificationHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////
//  CSongPropPageManager

class CSongPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabSong;
friend class CTabInfo;

public:
	CSongPropPageManager();
	virtual ~CSongPropPageManager();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CTabSong*					m_pTabSong;
	CTabInfo*					m_pTabInfo;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicSong

class CDirectMusicSong : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject,
						 public IDMUSProdNotifySink, public IDMUSProdSortNode, public IDMUSProdTransport,
						 public IDMUSProdSecondaryTransport, public IDMUSProdNotifyCPt
{
friend class CVirtualSegmentPropPageObject;
friend class CTrackPropPageObject;
friend class CTransitionPropPageObject;
friend class CFolderToolGraphs;
friend class CFolder;
friend class CSongCtrl;
friend class CSongDlg;
friend class CTabSong;
friend class CTabInfo;

public:
    CDirectMusicSong();
	~CDirectMusicSong();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
    HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

    HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );
    HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
    HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
	HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate( IDMUSProdNode* pINode, GUID guidUpdateType, VOID* pData );

	// IDMUSProdSortNode : Sorting interface
	HRESULT STDMETHODCALLTYPE CompareNodes(IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult);

	// IDMUSProdTransport methods
	STDMETHOD(Play)(BOOL fPlayFromStart);
	STDMETHOD(Stop)(BOOL fStopImmediate);
	STDMETHOD(Transition)();
	STDMETHOD(Record)( BOOL fPressed );
	STDMETHOD(GetName)( BSTR* pbstrName );
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);

	// IDMUSProdSecondaryTransport methods
	STDMETHOD(GetSecondaryTransportSegment)( IUnknown **ppunkSegment );
	STDMETHOD(GetSecondaryTransportName)( BSTR* pbstrName );

	//IDMUSProdNotifyCPt method
	HRESULT STDMETHODCALLTYPE OnNotify( ConductorNotifyEvent *pConductorNotifyEvent );

	//Additional functions
protected:
	HRESULT LoadSong( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT SaveSong( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
    HRESULT SaveHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveVersion( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedVirtualSegments( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT PasteCF_CONTAINER( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_AUDIOPATH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	void EmptyVirtualSegmentList();
	void InsertVirtualSegment( CVirtualSegment* pVirtualSegment, int nIndex );
	void RemoveVirtualSegment( CVirtualSegment* pVirtualSegment );
	void RemoveToolGraphReferences( IDMUSProdNode* pIToolGraphNode );
	void RemoveSourceSegmentReferences( CSourceSegment* pSourceSegment );

public:
	HRESULT ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
    HRESULT GetObjectDescriptor( void* pObjectDesc );

	HRESULT OnNewContainer();
	HRESULT OnNewAudioPath();
	BOOL CreateUndoMgr();
	void SetModified( BOOL fModified );
	void SyncChanges( DWORD dwFlags, DWORD dwEditorFlags, IDMUSProdPropPageObject* pIPropPageObject );
	void ResolveVirtualSegmentIDs();

	CFolder* GetSegmentEmbedFolder();
	CFolder* GetSegmentRefFolder();

	BOOL IsVirtualSegmentNameUnique( CVirtualSegment* pVirtualSegment );
	void GetUniqueVirtualSegmentName( CVirtualSegment* pVirtualSegment );
	int VirtualSegmentToIndex( CVirtualSegment* pVirtualSegment );
	CVirtualSegment* GUIDToVirtualSegment( GUID guidVirtualSegment );
	CVirtualSegment* IndexToVirtualSegment( int nVirtualSegmentIndex );
	CVirtualSegment* FindVirtualSegmentByName( LPCTSTR pszName );
	CVirtualSegment* PtrToVirtualSegment( CVirtualSegment* pVirtualSegment );
	void RecomputeVirtualSegmentLengths();

	BOOL IsSourceSegmentInSong( IDMUSProdNode* pIDocRootNode );
	CSourceSegment* GetSourceSegmentFromDocRoot( IDMUSProdNode* pIDocRootNode );
	int SourceSegmentToIndex( CSourceSegment* pSourceSegment );
	CSourceSegment* IndexToSourceSegment( int nSourceSegmentIndex );
	CSourceSegment* PtrToSourceSegment( CSourceSegment* pSourceSegment );
	
	int ToolGraphToIndex( IDMUSProdNode* pIToolGraphNode );
	IDMUSProdNode* IndexToToolGraph( int nToolGraphIndex );
	IDMUSProdNode* PtrToToolGraph( IDMUSProdNode* pIToolGraphNode );

	HRESULT UnRegisterWithTransport();
	HRESULT RegisterWithTransport();
	HRESULT SetActiveTransport();
	HRESULT UpdateTransportName();
	HRESULT TransitionWithinSong();
	DWORD TransitionOptionsToDMUS_SEGF( ConductorTransitionOptions* pTransitionOptions );

	void GetGUID( GUID* pguidSong );
	void SetGUID( GUID guidSong);

private:
    DWORD					m_dwRef;
	BOOL					m_fModified;

	CJazzUndoMan*			m_pUndoMgr;
	HWND					m_hWndEditor;

	IDirectMusicSong*			m_pIDMSong;
	IDirectMusicSegmentState*	m_pIDMSegmentState;
	IDirectMusicSegmentState*	m_rpIDMStoppedSegmentState;
	IDirectMusicSegmentState*	m_pIDMTransitionSegmentState;

	bool					m_fInTransition;
	DWORD					m_dwTransitionPlayFlags;
	IDirectMusicSegment*	m_pIDMTransitionSegment;

	IDMUSProdNode*			m_pIDocRootNode;
	IDMUSProdNode*			m_pIParentNode;

	CFolderSegments			m_FolderSegments;
	CFolderToolGraphs		m_FolderToolGraphs;
	IDMUSProdNode*			m_pIContainerNode;
	IDMUSProdNode*			m_pIAudioPathNode;
	bool					m_fDeletingContainer;
	bool					m_fChangingFolder;

public:
	CString					m_strOrigFileName;
	CSongCtrl*				m_pSongCtrl;
	IDMUSProdNode*			m_pINodeBeingDeleted;
	IDMUSProdProject*		m_pIProject;
	CRITICAL_SECTION		m_csSegmentState;

	// A window so we can handle DMusic notification in a messaging thread
	CNotificationHandler	m_wndNotificationHandler;

private:
	// Persisted for DirectMusic
	GUID					m_guidSong;
	DMUS_VERSION			m_vVersion;
    CString					m_strName;
    CString					m_strAuthor;
    CString					m_strCopyright;
    CString					m_strSubject;
    CString					m_strInfo;

	DWORD					m_dwSongFlagsDM;
	DWORD					m_dwStartSegID;

	CTypedPtrList<CPtrList, CVirtualSegment*> m_lstVirtualSegments;
};

#endif // __SONG_H__

#ifndef __GRAPH_H__
#define __GRAPH_H__

// Graph.h : header file
//

#include "GraphComponent.h"
#include "GraphDocType.h"
#include "PChannelGroup.h"
#include "Tool.h"
#include <RiffStrm.h>


#define DMUS_FOURCC_GRAPH_UI_LIST			mmioFOURCC('t','g','l','u')
#define DMUS_FOURCC_GRAPH_UI_CHUNK			mmioFOURCC('t','g','c','u')
#define DMUS_FOURCC_PCHANNELGROUP_UI_LIST	mmioFOURCC('c','h','l','u')
#define DMUS_FOURCC_PCHANNELGROUP_UI_CHUNK	mmioFOURCC('c','h','c','u')
#define DMUS_FOURCC_TOOL_UI_LIST			mmioFOURCC('t','o','l','u')
#define DMUS_FOURCC_TOOL_UI_CHUNK			mmioFOURCC('t','o','c','u')

#define DEFAULT_TOOL_PANE_WIDTH			100
#define DEFAULT_PCHANNEL_COLUMN_WIDTH	90
#define GRAPH_HEADER_HEIGHT				20

// m_dwBitsUI
#define UD_DRAGSELECT		0x00000001
#define UD_MULTIPLESELECT	0x00000002

#pragma pack(2)

typedef struct GraphUI
{
	GraphUI()
	{
		nPChannelColumnWidth = DEFAULT_PCHANNEL_COLUMN_WIDTH;
		nRegisteredToolPaneWidth = DEFAULT_TOOL_PANE_WIDTH;
		dwFlagsUI = 0;
	}

	int		nPChannelColumnWidth;
	int		nRegisteredToolPaneWidth;
	DWORD	dwFlagsUI;
} GraphUI;

#pragma pack()

// Flags for GraphUI.dwFlagsUI
#define GRAPHUI_FROM_TOOL_PALETTE	0x00000001


class CTool;

//////////////////////////////////////////////////////////////////////
//  CGraphPropPageManager

class CGraphPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabGraph;
friend class CTabInfo;

public:
	CGraphPropPageManager();
	virtual ~CGraphPropPageManager();

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
	
	CTabGraph*					m_pTabGraph;
	CTabInfo*					m_pTabInfo;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicGraph

class CDirectMusicGraph : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject,
						  public IDMUSProdNotifySink, public IDMUSProdToolGraphInfo
{
friend class CGraphCtrl;
friend class CGraphDlg;
friend class CGraphListBox;
friend class CToolListBox;
friend class CTabGraph;
friend class CTabInfo;
friend class CTabTool;

public:
    CDirectMusicGraph();
	~CDirectMusicGraph();

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

    // IDMUSProdToolGraphInfo functions
    HRESULT STDMETHODCALLTYPE EnumTools( DWORD dwIndex, IUnknown** ppIDirectMusicTool );
    HRESULT STDMETHODCALLTYPE GetToolInfo( IUnknown* pIDirectMusicTool, DMUSProdToolInfo* pToolInfo );
    HRESULT STDMETHODCALLTYPE GetToolCount( DWORD* pdwNbrTools );
    HRESULT STDMETHODCALLTYPE AddToGraphUserList( IUnknown* pIUnknown );
    HRESULT STDMETHODCALLTYPE RemoveFromGraphUserList( IUnknown* pIUnknown );

	//Additional functions
protected:
	HRESULT LoadGraph( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT LoadUIState( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT SaveGraph( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveVersion( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveUIState( IDMUSProdRIFFStream* pIRiffStream );

    // Pertains to m_lstGraphUsers
	HRESULT SyncGraphUser_AddTools( IUnknown* pIUnknown );
    HRESULT SyncGraphUser_RemoveTools( IUnknown* pIUnknown );

	BOOL IsInSegment();
	BOOL IsInAudioPath();

	void InsertPChannelGroup( CPChannelGroup* pPChannelGroupToInsert );
	void RemovePChannelGroup( CPChannelGroup* pPChannelGroupToRemove );
	void RepositionPChannelGroup( CPChannelGroup* pPChannelGroupToReposition );

	void RemoveTool( CTool* pToolToRemove );
	void CreateToolPChannelList( CTool* pTool, CPChannelGroup* pChannelGroup );

public:
	BOOL IsMultipleSelectTools();
	BOOL IsToolNameUnique( CTool* pTool );
	CTool* MyEnumTools( int nToolIndex );
	CTool* IndexToTool( int nToolIndex );
	CTool* GetFirstSelectedTool();
	int ToolToIndex( CTool* pTool );
	void GetUniqueToolName( CTool* pTool );
	void InsertTool( CTool* pToolToInsert, int nIndex );
	void SelectAllTools();
	void UnselectAllTools();
	void MarkSelectedTools( DWORD flags );
	void UnMarkTools( DWORD flags );
	void DeleteMarkedTools( DWORD flags );
	void DeleteSelectedTools();
	void SelectToolsInRange( int nStartIndex, int nEndIndex );
	void UseGraphPChannelGroups( CTool* pTool );

public:
	HRESULT ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
    HRESULT GetObjectDescriptor( void* pObjectDesc );

public:
	BOOL CreateUndoMgr();
	void SetModified( BOOL fModified );
	void SyncGraphEditor();
	void ResetContentGraphEditor();
	HRESULT SyncGraphWithDirectMusic();
	void Refresh();
	void GetName( CString& strName );
	void GetGUID( GUID* pguidStyle );
	void SetGUID( GUID guidStyle);

private:
    DWORD				m_dwRef;
	BOOL				m_fModified;

	CJazzUndoMan*		m_pUndoMgr;

	IDirectMusicGraph*	m_pIDMGraph;

	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*	    m_pIParentNode;

	HWND				m_hWndEditor;

public:
	CGraphCtrl*			m_pGraphCtrl;
	CString				m_strOrigFileName;

private:
	// Persisted for UI
	GraphUI				m_GraphUI;
	CTypedPtrList<CPtrList, CPChannelGroup*> m_lstGraphPChannelGroups;

	// Persisted for DirectMusic
	GUID				m_guidGraph;
	DMUS_VERSION        m_vVersion;
    CString				m_strName;
    CString				m_strAuthor;
    CString				m_strCopyright;
    CString				m_strSubject;
    CString				m_strInfo;
	CTypedPtrList<CPtrList, CTool*> m_lstGraphTools;

	// Other
	CTypedPtrList<CPtrList, IUnknown*> m_lstGraphUsers;	// Segments and/or AudioPaths
};

#endif // __GRAPH_H__

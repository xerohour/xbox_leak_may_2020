#ifndef __AUDIOPATH_H__
#define __AUDIOPATH_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AudioPath.h : header file
//

#include "AudioPathDesignerDLL.h"
#include "AudioPathComponent.h"
#include "AudioPathDocType.h"
#include "AudioPathDesigner.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include "dmusici.h"
#pragma warning( pop )
#include "DllBasePropPageManager.h"
#include "dmusicf.h"

#define DMUSPROD_FOURCC_AUDIOPATH_DESIGN_LIST mmioFOURCC('p','a','p','d')
#define DMUSPROD_FOURCC_AUDIOPATH_UI_CHUNK mmioFOURCC('p','a','u','i')
#define DMUSPROD_FOURCC_PORT_DESIGN_LIST mmioFOURCC('p','d','l','d')
#define DMUSPROD_FOURCC_PORT_DESIGN_MIXGROUP_CHUNK mmioFOURCC('p','d','m','g')
#define DMUSPROD_FOURCC_EFFECT_GUID_CHUNK mmioFOURCC('p','e','g','d')
#define DMUSPROD_FOURCC_BUFFER_DESIGN_GUID_CHUNK mmioFOURCC('p','b','g','d')

class CDirectMusicAudioPath;
class CJazzUndoMan;
class ItemInfo;
class EffectInfo;
struct BufferOptions;
struct BusEffectInfo;
struct PortOptions;
struct ioPortConfig;
struct ioPChannelToBuffer;
struct ioDesignMixGroup;
struct ioDSoundBuffer;
struct ioDSoundEffect;
interface IDMUSProdRIFFStream;

bool ItemAndBufferSupportEffects( const ItemInfo *pItemInfo, const BufferOptions *pBufferOptions );
bool ItemSupportsBuffers( const ItemInfo *pItemInfo );

struct ioAudioPathUI
{
	WORD wSplitterPos;
	WORD wMixGroupWidth;
	WORD wBusWidth;
	WORD wBufferWidth;
	WORD wEffectWidth;
};

//////////////////////////////////////////////////////////////////////
//  CAudioPathPropPageManager

class CAudioPathPropPageManager : public CDllBasePropPageManager 
{
friend class CTabAudioPathAudioPath;
friend class CTabAudioPathInfo;

public:
	CAudioPathPropPageManager();
	virtual ~CAudioPathPropPageManager();

	/*
    // IUnknown functions
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
	*/

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    //HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    //HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	//void RemoveCurrentObject();

	// Member variables
private:
	/*
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	*/
	
	CTabAudioPathAudioPath*			m_pTabAudioPath;
	CTabAudioPathInfo*				m_pTabInfo;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicAudioPath

class CDirectMusicAudioPath : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject,
							  public IDMUSProdAudioPathInUse, public IDMUSProdNotifySink, public IDMUSProdDMOInfo
{
friend class CAudioPathCtrl;
friend class CAudioPathDlg;
friend class CEffectListDlg;
friend class CTabAudioPathAudioPath;
friend class CTabAudioPathInfo;

public:
    CDirectMusicAudioPath();
	~CDirectMusicAudioPath();

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

	// IDMUSProdAudioPathInUse
	HRESULT STDMETHODCALLTYPE UsingAudioPath( IUnknown *punkAudioPath, BOOL fActive );
        
	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

	// IDMUSProdDMOInfo
	HRESULT STDMETHODCALLTYPE EnumDMOInfo( DWORD dwIndex, DMUSProdDMOInfo *pDMOInfo );

	//Additional functions
protected:
	CString GetNewMixgroupName( void ) const;
	void	EnumeratePorts( void );
	PortOptions *FindPort( REFGUID rGUIDPort );
	void	GetUniqueEffectInstanceNameIfNecessary( EffectInfo *pEffectInfo );
	HRESULT SaveToConfigObject ( IUnknown *punkAudioPathConfig );
	HRESULT DM_SaveAudioPath( IStream *pIStream );
	HRESULT DM_SaveUIDesign( IDMUSProdRIFFStream *pIRiffStream );
	void	DM_SaveDSBC_Form( IDMUSProdRIFFStream *pRiffStream, ioDSoundBuffer *pioDSoundBuffer, DMUSProdStreamInfo &StreamInfo );
	HRESULT DM_LoadAudioPath( IStream *pIStream );
	HRESULT DM_LoadDesignInfo( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk );
	HRESULT DM_LoadPortConfig( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk, CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs );
	HRESULT DM_LoadBuffer( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk, DMUS_IO_BUFFER_ATTRIBUTES_HEADER &ioBufferHeader, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers );
	ioDSoundEffect *DM_LoadEffect( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk );
	void	DM_CreateItems( CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers );
	PortOptions *CreatePortOptions( const DMUS_IO_PORTCONFIG_HEADER &ioPortConfigHeader, const DMUS_PORTPARAMS8 &ioPortParams, const CString &strPortName );
	void	CreateItemInfos( ioPortConfig *pioPortConfig, PortOptions *pPortOptions, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstUsedDSoundBuffers, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers );
	void	CreateItemFromPChannelToBuffer( ioPChannelToBuffer *pioPChannelToBuffer, PortOptions *pPortOptions, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstUsedDSoundBuffers, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers );
	void	CreateItemFromDesignInfo( ioDesignMixGroup *pioDesignMixGroup, PortOptions *pPortOptions );
	void	CreateItemFromPChannelArray( CDWordArray &adwPChannelsToAdd, PortOptions *pPortOptions );
	void	InsertItemInfo( ItemInfo *pItemInfo );
	ItemInfo *GetItemForPChannel( DWORD dwPChannel );
	HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveVersion( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT AllAudioPaths_InsertToolGraph( void );
	HRESULT AllAudioPaths_RemoveToolGraph( void );
	HRESULT InsertToolGraph( IDirectMusicAudioPath* pIAudioPath );
	HRESULT RemoveToolGraph( IDirectMusicAudioPath* pIAudioPath );
	ItemInfo *CreateStandardItem( REFGUID rGuidBuffer1, REFGUID rGuidBuffer2, DWORD dwBufferFlags );

public:
    HRESULT GetObjectDescriptor( void* pObjectDesc );
	HRESULT CreateEmptyToolGraph();
	BSTR	GetNameForConductor( void );
	void	CopyFrom( CDirectMusicAudioPath *pAudioPath );
	void	CopyFrom( DWORD dwStandardID );
	HRESULT EnumBufferInfo( DWORD dwIndex, WCHAR wcstr[DMUS_MAX_NAME + 1], GUID *pGuid );
	void	ConnectAllSendEffects( void );
	void	UpdateConnectedSendEffects( const ItemInfo *pItem );
	void	DisconnectSendEffects( const ItemInfo *pItem );
	void	UpdateDisplayIndexes( void );

public:
	BOOL	CreateUndoMgr();
	void	SetModified( BOOL fModified );
	void	SyncAudioPathEditor();
	void	SyncAudiopathsInUse( DWORD dwFlags );
	void	UpdateDMOs( BusEffectInfo *pBusEffectInfo );
//	HRESULT UnRegisterWithTransport();
//	HRESULT RegisterWithTransport();
	void	SaveUndoState( bool &fIgnoreChange, const int nStringID, const bool fSetModified = true );
	void	SaveUndoState( const int nStringID, const bool fSetModified = true );
	EffectInfo *LoadEffect( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent );
	EffectInfo *LoadEffect( IStream *pIStream );
	PortOptions *GetDefaultPort( void );
	PortOptions *PastePortOptionsHelper( PortOptions *pPortOptions );

	void GetGUID( GUID* pguidAudioPath );
	void SetGUID( GUID guidAudioPath );

	// Dialog Sizing Functions
	inline WORD GetSplitterPos() const
	{
		return m_wSplitterPos;
	}
	void	SetSplitterPos( WORD pos, bool bMarkAsDirty=true );
	void	GetUniqueEffectInstanceName( EffectInfo *pEffectInfo );
	void	GetUniqueMixGroupName( ItemInfo *pItemInfo );

	bool	IsMixgroupNameUnique( const ItemInfo *pItemInfo, const CString cstrName ) const;
	bool	IsEffectNameUnique( const EffectInfo *pEffectInfo, const CString cstrName ) const;
	bool	IsInSegment( void );

private:
    DWORD				m_dwRef;
	BOOL				m_fModified;

	CJazzUndoMan*		m_pUndoMgr;

	CTypedPtrList< CPtrList, IDirectMusicAudioPath *> m_lstDMAudioPaths;

	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*	    m_pIParentNode;
	IDMUSProdNode*		m_pIToolGraphNode;

	HWND				m_hWndEditor;

public:
	CString				m_strOrigFileName;
	CAudioPathCtrl*		m_pAudioPathCtrl;
	IDMUSProdNode*		m_pINodeBeingDeleted;
	static CLIPFORMAT m_scfPChannel;
	static CLIPFORMAT m_scfEffect;
	static CLIPFORMAT m_scfBuffer;
	CTypedPtrList< CPtrList, ItemInfo *> m_lstItems;
	CTypedPtrList< CPtrList, PortOptions *> m_lstPortOptions;

private:
	WORD				m_wSplitterPos;
	WORD				m_wMixGroupWidth;
	WORD				m_wBusWidth;
	WORD				m_wBufferWidth;
	WORD				m_wEffectWidth;

    // AudioPath info block
public:
    CString				m_strName;
private:
	GUID				m_guidAudioPath;
	DMUS_VERSION        m_vVersion;
    CString				m_strAuthor;
    CString				m_strCopyright;
    CString				m_strSubject;
};

#endif // __AUDIOPATH_H__

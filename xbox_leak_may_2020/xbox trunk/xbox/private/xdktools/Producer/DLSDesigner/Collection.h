#ifndef __COLLECTION_H__
#define __COLLECTION_H__


// Collection.h : header file
//
/*
#define UPDATE_COLLECTION	1
#define UPDATE_INSTRUMENT	2
#define UPDATE_ARTICULATION	3
#define UPDATE_REGION		4
#define UPDATE_WAVE			5
#define UPDATE_ALL			6
*/
#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "CollectionInstruments.h"
#include "CollectionWaves.h"
#include "Info.h"
#include "DLS1.h"
#include "alist.h"
#include "unknownchunk.h"

class CCollectionPropPgMgr;


struct BInstr  
{
	UINT      nStringId;
	BYTE      bPatch;
	BYTE      bMSB;
	BYTE      bLSB;
	CString  *pstrName;
};

class AllocatedPortBufferPair
{

public:
	// Constructor
	AllocatedPortBufferPair(IDirectMusicPortDownload* pDownloadPort, IDirectMusicDownload* pAllocatedBuffer)
	{
		ASSERT(pDownloadPort);
		ASSERT(pAllocatedBuffer);

		m_pDownloadPort = pDownloadPort;
		m_pAllocatedBuffer = pAllocatedBuffer;
        m_dwDMID = 0;
	}

	// Destructor
	~AllocatedPortBufferPair()
	{
		m_pDownloadPort = NULL;
		m_pAllocatedBuffer = NULL;
	}

    void SetDMID(DWORD dwID)
    {
        m_dwDMID = dwID;
    }

    DWORD GetDMID()
    {
        return m_dwDMID;
    }

	IDirectMusicPortDownload*	m_pDownloadPort;            // what port is this allocated for?
	IDirectMusicDownload*		m_pAllocatedBuffer;		    // Where is the allocated buffer?
    DWORD                       m_dwDMID;                   // What's the download ID for this port?
};


class CCollection : public AListItem, public IDMUSProdNode, public IPersistStream, 
					public IDMUSProdPropPageObject, public IDMUSProdNotifySink
{
friend class CCollectionPropPg;
friend class CCollectionInstruments;
friend class CCollectionWaves;
friend class CWave;
friend class CMonoWave;
friend class CInstrumentCtrl;
friend class CWaveCtrl;
friend class CInstrument;
friend class CInstrumentRegions;
friend class CArticulation;
friend class CRegion;
friend class CDLSComponent;
friend class CInstrumentFVEditor;
friend class CWavePropPg;
friend class CCollectionRef;
friend class CPatchConflictDlg;
friend class CUserPatchConflictDlg;
public:
    CCollection( CDLSComponent* pComponent );
	~CCollection();
    CCollection * GetNext() {return(CCollection *)AListItem::GetNext();};


    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );

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

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnResourceId);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit(HMENU hMenu);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect(long lCommandId);

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE GetFirstChild(IDMUSProdNode** ppIFirstChildNode);
	HRESULT STDMETHODCALLTYPE GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode);
	HRESULT STDMETHODCALLTYPE GetNodeListInfo(DMUSProdListInfo* pListInfo);
	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );
    // IDMUSProdNode;drag & drop methods
	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject );

	// IDMUSProdNotifySink method
	STDMETHOD(OnUpdate)(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);
    
	// IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

	// IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

	//Additional functions
public:
	void SetDirtyFlag();

	DWORD   SampleCount();
	DWORD	RegionCount();
	DWORD	ArticulationCount();
	DWORD	WaveCount();
	DWORD	WaveNodeCount();
	DWORD	InstrumentCount();

	CCollectionInstruments* GetInstruments();
	HRESULT GetListOfConditions(CPtrList* pConditionList);
	void	OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode = true);

    HRESULT Load(IStream* pIStream, DWORD dwSize);
    HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT GetObjectDescriptor( void* pObjectDesc );

	void	SetNodeFileName(CString sFileName);
	CString	GetNodeFileName();
	HRESULT FindWaveNodeByGUID(GUID guidNode, CWaveNode** ppWaveNode);

	void	GetGUID( GUID* pguidCollection );
	void	SetGUID( GUID guidCollection );

	HRESULT SyncDMCollection();
	void	ReleaseDMCollection();

protected:
	void ClearDirtyFlag() {m_bIsDirty = false;}
    void ResolveConnections();
    void SetWaveIDs();
	void PrepareConnections();
	
private:
	HRESULT Download();	// Downloads collection to synth
	void Unload();		// Unloads a collection from the synth
	
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, BOOL fFullSave);
	HRESULT Save( const char* strFileName);

	HRESULT CreateDMCollection();

public:
	
	DWORD m_fPatchResolution;
	void  ValidateInstrumentPatches();
	BOOL  VerifyDLS();
	bool  IsWaveReferenced(CWave* pWave, IDMUSProdNode** ppReferenceNode);

	CDLSComponent*				m_pComponent;

private:

	IDirectMusicCollection8*	m_pIDMCollection;

	CString						m_sFileName;

	CCollectionInstruments		m_Instruments;
	CCollectionWaves			m_Waves;
    CUnknownChunk				m_UnknownChunk;
    
	long						m_lRef;
	IDMUSProdNode*				m_pIRootNode;
	IDMUSProdNode*				m_pIParentNode;

	CString						m_sTempName;		// Temp name used while saving etc.

	//	Data for DLS file...
	CInfo						m_Info;
	DLSHEADER					m_rDLSHeader;
	DLSVERSION					m_rVersion;

	CCollectionPropPgMgr*		m_pCollectionPropPgMgr;
	BOOL						m_bIsDirty;
	GUID						m_Guid;

	static BOOL					m_bNoMIDIPort;
};	

#endif // __COLLECTION_H__

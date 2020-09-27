#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

// Instrument.h : header file
//

#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "Info.h"
#include "alist.h"
#include "InstrumentRegions.h"
#include "DLSLoadSaveUtils.h"
#include "UnknownChunk.h"

class CInstrumentPropPgMgr;
class Collection;
class CUndoMan;
class AllocatedPortBufferPair;
class CArticulationList;

struct StopResetTimeEvent
{
	long		 lWaitTime;		// Length of the one shot wave in milliseconds
	CInstrument* pInstrument;	// Instrument playing
};

class CInstrument : public AListItem, public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject, public IDMUSProdTransport
{
friend class CDLSComponent;
friend class CCollection;
friend class CRegion;
friend class CArticulation;
friend class CWave;
friend class CMonoWave;
friend class CStereoWave;
friend class CWaveCtrl;
friend class CInstrumentRegions;
friend class CCollectionInstruments;
friend class CCollectionWaves;
friend class CInstrumentCtrl;
friend class CInstrumentFVEditor;
friend class CInstrumentPropPgMgr;
friend class CInstrumentPropPg;
friend class CWavePropPg;
friend class CCollectionRef;
friend class CPatchConflictDlg;
friend class CUserPatchConflictDlg;
public:
    CInstrument(CDLSComponent* pComponent);
	~CInstrument();
    CInstrument * GetNext() {return(CInstrument *)AListItem::GetNext();};

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
    // IDMUSProdNode : drag & drop methods
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

    // IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)(IStream* pIStream);
    STDMETHOD(Save)(IStream* pIStream, BOOL fClearDirty);
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData(void** ppData);
    HRESULT STDMETHODCALLTYPE SetData(void* pData);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

    //IDMUSProdTransport functions
	STDMETHOD(Play)(BOOL /*fPlayFromStart*/);
	STDMETHOD(Stop)(BOOL /*fStopImmediate*/);  
    STDMETHOD(Transition)( );
	STDMETHOD(Record)(BOOL fPressed);
	STDMETHOD(GetName)(BSTR* pbstrName);
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);


	//Additional functions
	void			UpdatePatch();
	bool			ValidatePatch();
	BOOL			VerifyDLS();
	HRESULT			CheckForOverlappedRegions();
	CRegion*		GetRegion(int nNote);
	DWORD			GetDLS1RegionCount();

    CCollection *	GetParentCollection() {return(m_pCollection);}
	HRESULT			Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT			UpdateInstrument();
	void			GetName();
	void			GetName(CString& sName);
	void			TurnOffMidiNotes();
	bool			IsDrum() { return (m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0; }
	
	HRESULT			OnViewProperties();
	void			RefreshUI(bool bWaveDataChanged);
	void			RefreshRegion();
	
	HRESULT			SaveStateForUndo(UINT uUndoTextID);
	void			PopUndoState();
	
	BOOL			SupportsNewFormat();
	
	CInstrumentFVEditor*	GetInstrumentEditor();
	CInstrumentRegions*		GetRegions();

	void			SetFirstGlobaArtAsCurrent();
	void			SetCurrentArticulation(CArticulation* pArticulation);
	CArticulation*	GetCurrentArticulation();
	UINT			GetArticulationCount();							// Goes throung all the lists os articulations and returns the number of global articulations
	void			CheckRegionsForLocalArts();
	BOOL			NoArtForEveryRegion();

	HRESULT			GetListOfConditions(CPtrList* pConditionList);
	CCollection*	GetCollection();

	bool			GetAuditionMode();
	void			SetAuditionMode(bool bAuditionMode);

protected:
	HRESULT Download(IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT Unload(IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad);
    HRESULT GetDMIDForPort(IDirectMusicPortDownload* pIDMDownloadPort, DWORD* pdwDMID);
	HRESULT Size(DWORD& dwSize);
	HRESULT Write(void* pvoid, IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, bool fFullSave);
	
	bool	SaveString(IStream* pIStream, CString sData);
	bool	LoadString(IStream* pIStream, CString& sData);

private:
	GCENT	GetMaxVolume();
	bool	IsPropertyPageShowing();
	
	AllocatedPortBufferPair* GetAllocatedBufferForPort(IDirectMusicPortDownload* pIDMPortDownLoad); // Gets the buffer allocated (in DM_Init) for this port
	void	RemoveFromAllocatedBufferList(AllocatedPortBufferPair* pAllocatedBuffer);
	void	CreateDefaultGlobalArticulation();	// Creates a deafulat list of global articulations
	void	DeleteAllRegions();			// Removes all regions from the instrument
	void	DeleteArticulationLists();	// Removes all global articulations lists

	void	OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode = true);	

	HRESULT	UpdateForAuditionMode();

public:
	CDLSComponent*				m_pComponent;

	CTypedPtrList<CPtrList, AllocatedPortBufferPair*> m_lstAllocatedBuffers; // List of allocated buffers to be unloaded
	
    WORD						m_nMIDINote;
	CCollection*				m_pCollection;		// Parent collection.
	CInstrumentCtrl*			m_pInstrumentCtrl;	// When instrument is displayed within a control this is set 
    CUnknownChunk				m_UnknownChunk;
	HANDLE						m_hStopEvent;
	CUndoMan*					m_pUndoMgr;

private:

	DWORD m_dwDLSize;
	BOOL						m_bNewFormat;
	bool						m_bAuditionMode;	// Audition mode can be solo layer or multiple layer
	int							m_cMidiNoteOns[128];
	CInstrumentRegions			m_Regions;			// List of regions.
    CInfo						m_Info;				// Info chunk.
	INSTHEADER					m_rInstHeader;		// DLS instrument chunk.
	
	CString						m_csName;			// Display name. Generated by GetName() method.

	DWORD						m_dwId;				//DownLoad ID (obtained from DM)
	DWORD						m_cbSizeOffsetTable;
	DWORD						m_dwRef;
	IDMUSProdNode*				m_pIRootNode;
	IDMUSProdNode*				m_pIParentNode;
	HWND						m_hWndEditor;

	StopResetTimeEvent			m_ResetCallBackParams;

	CInstrumentPropPgMgr*		m_pInstrumentPropPgMgr;

	CArticulation*				m_pCurArticulation;
	CTypedPtrList<CPtrList, CArticulationList*> m_lstArticulationLists;	// List of artiulation lists

	GUID						m_guidUndoStream;
};

#endif // __INSTRUMENT_H__

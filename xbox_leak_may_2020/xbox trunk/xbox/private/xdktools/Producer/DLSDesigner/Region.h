#ifndef __REGION_H__
#define __REGION_H__

// Region.h : header file
//

#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "dls1.h"
#include "UnknownChunk.h"
#include "ConditionalChunk.h"
#include <afxtempl.h>

class CRegionPropPgMgr;
class CInstrumentFVEditor;
class CMonoWave;
class CConditionalChunk;
class CArticulationList;

class CRegion : public AListItem, public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject
{
friend class CWave;
friend class CMonoWave;
friend class CStereoWave;
friend class CWaveCtrl;
friend class CArticulation;
friend class CInstrument;
friend class CInstrumentRegions;
friend class CCollection;
friend class CCollectionWaves;
friend class CInstrumentFVEditor;
friend class CRegionPropPg;
friend class CRegionExtraPropPg;
friend class CInstrumentCtrl;
friend class CWavePropPg;
friend class CCollectionInstruments;

public:
    CRegion(CDLSComponent* pComponent, bool bDLS1Region = false);
	~CRegion();
    CRegion* GetNext();

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
    CInstrument* GetInstrument() {return(m_pInstrument);}
	HRESULT		Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

	CRegion*	IsOverlappingRegion(CRegion* pRegion);
	CRegion*	IsOverlappingRegion(USHORT usStartNote, USHORT usEndNote);
    void		SetRangeValue(BOOL bUpper, CString csName);
    CString		GetRangeName(BOOL bUpper);
    USHORT		GetRangeValue(BOOL bUpper);
	CString		GetWaveName();
    void		GetName();
    void		SetInitialWave(CWave* pWave);		// This is called right after newing the region to set the default wave...
	int			GetLayer();							// Returns the visual layer of this region (Required for Instrument Editor)						
	void		SetLayer(int nLayer);
	bool		ValidateUIA();
	DWORD		Count();
	DWORD		Size();
	void		SetWave(CWave* pWave);				// Replaces the current wave with the passed wave for this region

	WAVELINK	GetWaveLink();
	void		SetWaveLink(WAVELINK waveLink);
	
	HRESULT				SetCondition(const CString& sCondition);
	HRESULT				GetListOfConditions(CPtrList* pConditionList);	// Gets the conditions set for the region and all the chldren articulation lists
	CConditionalChunk*	GetConditionalChunk();							// Gets the conditional chunk for this region
	
	HRESULT			CreateAndInsertArticulationList();
	void			SetCurrentArticulation(CArticulation* pArticulation);
	UINT			GetArticulationCount();
	CArticulation*	GetCurrentArticulation();

	void			OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode = true);
	void			CheckConfigAndRefreshNode();
	void			CheckArtsAndSetInstrumentArt();
	HRESULT			CopyProperties(CRegion* pRegion);	// Copies the proeprties from the passed region

	bool			IsOkayToPlay(USHORT usActiveLayer, USHORT nNote, USHORT usVelocity, bool bIgnoreLayer);

	CWaveNode*		GetWaveNode();	// REturns the wave node referred by this region

	/* returns the number of channels in the wave pointed to by this region */
	LONG			GetChannelCount();

	/* merges following regions that have the same m_usRegionSaved, with this one, and returns the
		"master" wave that should be used (regions with same saved ID are supposed to
		point to different channels of the same wave) */
	void			MergeNextRegions(CWave* &pWave);

protected:
	HRESULT Write(void* pv, LONG iChannel, DWORD* pdwCurOffset, DWORD* pDMIOffsetTable, DWORD* pdwCurIndex, DWORD dwIndexNextRegion, IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, bool fFullSave, USHORT usRegionSaved);

	BOOL m_bUseInstrumentArticulation;

private:
	void DeleteArticulationLists();
	UINT GetArticulationsUsed();			// Gets the number of articulations that will actually get downloaded
	void SetCurrentArtInEditor();

public:
	CDLSComponent*		m_pComponent;
    CInstrument*		m_pInstrument;		// Parent Instrument.
	bool				m_bLastDeleted;		// This node was deleted last; used during the undo

    CInstrumentFVEditor* m_pInstrumentFVEditor;

private:
	void				GetWave();

	// Articulation stuff
	CArticulation*		m_pArticulation;	// Optional Articulation child.
	CTypedPtrList<CPtrList, CArticulationList*> m_lstArticulationLists;

    CWave*				m_pWave;			// Link to wave (may be mono, stereo, or any channel)
    USHORT				m_usRegionSaved;	// regions that are really one share the same ID at save/load time, 1 based (0 means region ID wasn't loaded)
    
    CUnknownChunk		m_UnknownChunk;

    DWORD				m_dwRef;
	IDMUSProdNode*		m_pIRootNode;
	IDMUSProdNode*		m_pIParentNode;

	CString				m_csName;			// Display name. Generated by GetName() method.
	CConditionalChunk*	m_pConditionalChunk;	// Conditional chunk for the region
	RGNHEADER			m_rRgnHeader;		// Comes from a DLS file chunk
    WSMPL				m_rWSMP;			// Comes from a DLS file chunk
	WLOOP				m_rWLOOP;			// Comes from a DLS file chunk
	WAVELINK			m_rWaveLink;		// Comes from a DLS file chunk

	CRegionPropPgMgr*	m_pRegionPropPgMgr;

	// Flag that indicates a DLS1 region
	bool				m_bDLS1Region;

	// Flag that says if we're satifying the config condition
	BOOL				m_bConditionOK;

    // flags to remember whether we are overriding wave properties or not.
    BOOL m_bOverrideWaveAttenuation;
    BOOL m_bOverrideWaveFineTune;
    BOOL m_bOverrideWaveRootNote;
    BOOL m_bOverrideWaveMoreSettings;
	bool m_bDeletingRegion;//this node is being deleted

	USHORT	m_nLayer;
};

#define DLS_REGION_EXTRA_CHUNK			mmioFOURCC('d','m','p','r')
#endif // __REGION_H__

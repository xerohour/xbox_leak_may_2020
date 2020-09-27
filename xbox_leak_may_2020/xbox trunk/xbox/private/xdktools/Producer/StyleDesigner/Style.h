#ifndef __STYLE_H__
#define __STYLE_H__

// Style.h : header file
//

#include "StyleComponent.h"
#include "StyleDocType.h"
#include "StyleBands.h"
#include "Pattern.h"
#include "StylePatterns.h"
#include "StyleMotifs.h"
#include "StylePersonalities.h"


class CDirectMusicStyle;
interface IDMUSProdRIFFStream;

//////////////////////////////////////////////////////////////////////
//  CStylePropPageManager

class CStylePropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabStyleStyle;
friend class CTabStyleInfo;

public:
	CStylePropPageManager();
	virtual ~CStylePropPageManager();

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
    DWORD					m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CTabStyleStyle*			m_pTabStyle;
	CTabStyleInfo*			m_pTabInfo;

public:
	static short			sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicStyle

class CDirectMusicStyle : public IDMUSProdNode, public IDMUSProdStyleInfo, public IPersistStream,
	public IDMUSProdPropPageObject, public IDMUSProdNotifySink
{
friend class CStyleCtrl;
friend class CStyleDlg;
friend class CStyleBands;
friend class CStylePatterns;
friend class CStyleMotifs;
friend class CStylePersonalities;
friend class CTabStyleStyle;
friend class CTabStyleInfo;
friend class CDirectMusicPattern;
friend class CDirectMusicPart;
friend class CVarChoices;
friend class CVarChoicesPropPageManager;
friend class CTabVarChoices;
friend class CPatternCtrl;
friend class CDialogLinkExisting;

public:
    CDirectMusicStyle();
	~CDirectMusicStyle();

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

	// IDMUSProdStyleInfo functions
	HRESULT STDMETHODCALLTYPE GetActiveBandForStyle( IUnknown** ppIActiveBandNode );
	HRESULT STDMETHODCALLTYPE GetDefaultBand( IUnknown** ppIDefaultBandNode );
    HRESULT STDMETHODCALLTYPE GetTempo( double* pTempo );
    HRESULT STDMETHODCALLTYPE GetTimeSignature( DMUSProdTimeSignature* pTimeSignature );
	HRESULT STDMETHODCALLTYPE GetNotationType( DWORD *pdwType );
	HRESULT STDMETHODCALLTYPE GetActiveBandForObject( IUnknown* punkObject, IUnknown** ppIActiveBandNode );

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

	//Additional functions
protected:
    HRESULT DM_SaveStyle( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
    HRESULT DM_SaveStyleChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStyleUIChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStyleGUID( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStyleInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStyleDefaultBand( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStyleVersion( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveStylePersonalityList( IDMUSProdRIFFStream* pIRiffStream );

    HRESULT DM_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    
    HRESULT IMA25_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

    HRESULT UNDO_SaveStyle( IDMUSProdRIFFStream* pIRiffStream, FileType ftFileType );
    HRESULT UNDO_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

	HRESULT ContainsClipFormat( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, UINT cfClipFormat );
	HRESULT LoadCF_BANDTRACK( IStream* pIStream, IDMUSProdNode* pIPositionNode );

	HRESULT PasteCF_BAND( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, IDMUSProdNode* pIPositionNode );
	HRESULT PasteCF_BANDLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, IDMUSProdNode* pIPositionNode );
	HRESULT PasteCF_BANDTRACK( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, IDMUSProdNode* pIPositionNode );
	HRESULT PasteCF_TIMELINE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, IDMUSProdNode* pIPositionNode );
	HRESULT PasteCF_MOTIF( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, CDirectMusicPattern* pPositionMotif );
	HRESULT PasteCF_MOTIFLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, CDirectMusicPattern* pPositionMotif );
	HRESULT PasteCF_PATTERN( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, CDirectMusicPattern* pPositionPattern );
	HRESULT PasteCF_PATTERNLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, CDirectMusicPattern* pPositionPattern );

	BOOL IsBandNameUnique( IDMUSProdNode* pIBandNode, LPCTSTR szBandName );
	void GetUniqueBandName( IDMUSProdNode* pIBandNode );
	IDMUSProdNode* GetBandNodeByName( LPCTSTR szBandName );

public:
	HRESULT Initialize();
	BOOL CreateUndoMgr();
	BOOL IsPatternNameUnique( CDirectMusicPattern* pPattern );
	void GetUniquePatternName( CDirectMusicPattern* pPattern );
	void SetModified( BOOL fModified );
	void SyncStyleEditor( DWORD dwFlags );
	HRESULT SyncStyleWithDirectMusic();
	HRESULT ForceSyncStyleWithDirectMusic();
	HRESULT UnRegisterWithTransport();
	HRESULT RegisterWithTransport();
    HRESULT GetObjectDescriptor( void* pObjectDesc );
	IDirectMusicSegment* GetSegmentThatIsPlaying();
	IDMUSProdNode* GetActiveBand();
	IDirectMusicBand* GetDMBand( IDMUSProdNode* pIBandNode );
	void SetActiveBand( IDMUSProdNode* pIBandNode );
	void SendBand( IDMUSProdNode* pIBandNode );
	HRESULT SyncBand( IDMUSProdNode* pIBandNode );
	void SyncMidiStripMgrsThatUsePart( CDirectMusicPart* pDMPart );
	void RemoveActiveBand( IDMUSProdNode* pIBandNode );
	void UpdateLinkedParts( CDirectMusicPartRef* pPartRef, LPCTSTR pcstrText );
	void UpdateLinkFlags( void );
	void OnOutputPortsChanged( void );
	void OnOutputPortsRemoved( void );
	void UpdateMIDIMgrsAndChangePartPtrs( CDirectMusicPart* pOldPart, CDirectMusicPartRef* pNewPartRef, LPCTSTR pcstrText );
	void PreChangePartRef( CDirectMusicPartRef* pDMPartRef );
	BOOL IsInStyleBandList( IDMUSProdNode* pIBandNode );

	HRESULT AddPattern( CDirectMusicPattern* pPattern, CDirectMusicPattern* pPositionPattern );
	HRESULT RemovePattern( CDirectMusicPattern* pPattern );

	HRESULT AddMotif( CDirectMusicPattern* pMotif, CDirectMusicPattern* pPositionMotif );
	HRESULT RemoveMotif( CDirectMusicPattern* pMotif );

	CDirectMusicPart* FindPartByGUID( GUID guidPartID );
	CDirectMusicPart* AllocPart();
	void DeletePart( CDirectMusicPart* pPart );

	HRESULT ImportMidiFileAsPattern( BOOL fMotif );
	HRESULT CreatePatternFromMIDIStream(CDirectMusicPattern* pPattern, IDMUSProdNode* pBandNode, LPSTREAM pStream, CString strFile);

	// Methods to set Style data
	void SetTempo( double dblTempo, BOOL fInUndo );
	void SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fInUndo );
	void SetDefaultBand( IDMUSProdNode* pIDefaultBandNode, BOOL fInUndo );
	IDMUSProdNode* GetTheDefaultBand();

	void GetGUID( GUID* pguidStyle );
	void SetGUID( GUID guidStyle);

private:
    DWORD				m_dwRef;
	BOOL				m_fModified;

	CJazzUndoMan*		m_pUndoMgr;

	IDirectMusicStyle*	m_pIDMStyle;

	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*	    m_pIParentNode;
	IDMUSProdNode*		m_pIActiveBandNode;

	HWND				m_hWndEditor;

	DWORD				m_dwNextPatternID;
	BOOL				m_fPChannelChange;

public:
	CString				m_strOrigFileName;
	CStyleCtrl*			m_pStyleCtrl;
	IDMUSProdNode*		m_pINodeBeingDeleted;
	IDMUSProdProject*	m_pIProject;

public:
	CTypedPtrList<CPtrList, CDirectMusicPart*> m_lstStyleParts;
	CStyleBands			m_StyleBands;
	CStylePatterns		m_StylePatterns;
	CStyleMotifs		m_StyleMotifs;
	CStylePersonalities	m_StylePersonalities;

	// UI fields
	int					m_nSplitterYPos;
	DWORD				m_dwNotationType;

private:
    CString				m_strName;
    CString				m_strCategoryName;
    CString				m_strInfo;
    CString				m_strAuthor;
    CString				m_strCopyright;
	CString				m_strSubject;

    GUID				m_guidStyle;

	DWORD				m_dwVersionMS;		 // Version # high-order 32 bits
	DWORD				m_dwVersionLS;		 // Version # low-order 32 bits

	DirectMusicTimeSig	m_TimeSignature;	 // The style's time signature
	double				m_dblTempo;			 // The style's tempo

	// IMA 2.5 data
	WORD				m_wUseId;
};

#endif // __STYLE_H__

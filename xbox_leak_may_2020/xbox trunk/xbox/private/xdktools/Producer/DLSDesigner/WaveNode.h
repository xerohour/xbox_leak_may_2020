#ifndef __WAVENODE_H__
#define __WAVENODE_H__

// WaveNode.h : header file
//

#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "DMusicc.h"
#include "DSoundP.h"
#include <WaveTimelineDraw.h>


struct WaveStopResetTimeEvent
{
	long		 lWaitTime;		// Length of the one shot wave in milliseconds
	CWaveNode*	 pWaveNode;		// Playing wave node
};

// CWaveNode definition
//=========================================================================================================
class CWave;
class CWaveCtrl;
class CCollectionWaves;
class CWaveStream;

class CWaveNode : public AListItem, public IDMUSProdNode, public IPersistStream, 
					public IDMUSProdTransport, public IDMUSProdWaveTimelineDraw, public IDMUSProdNotifySink
{

public:
    
	// Named constructors : Create method should be the only way to create this object
	static HRESULT CreateWaveNode(CDLSComponent* pComponent, IStream* pIStream, CString sFileName, CWaveNode** ppWaveNode, CCollectionWaves* pWavesCollection = NULL, bool bInACollection = false);
	static HRESULT CreateWaveNode(CDLSComponent* pComponent, CCollectionWaves* pWavesCollection, IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, CWaveNode** ppWaveNode);
	static HRESULT CreateWaveNode(CDLSComponent* pComponent, CWaveNode** ppWaveNode); 

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );

    HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIRootNode );
    HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIRootNode );
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

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnMenuId);
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

	// Drag-Drop Methods
	HRESULT WriteDragChunkToStream(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT PasteWave(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckDragged);

	// Helper method to save waves as wave files
	HRESULT SaveAs(IStream* pIStream, bool bPromptForFileName = false); 

	// Helper method to retrieve object descriptor 
	HRESULT GetObjectDescriptor( void* pObjectDesc );

    // IDMUSProdTransport functions
	STDMETHOD(Play)(BOOL fPlayFromStart);
	STDMETHOD(Stop)(BOOL /*fStopImmediate*/);
    STDMETHOD(Transition)( );
	STDMETHOD(Record)(BOOL fPressed);
	STDMETHOD(GetName)(BSTR* pbstrName);
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);

	// IDMUSProdWaveTimelineDraw methods
	HRESULT STDMETHODCALLTYPE DrawWave(IDMUSProdTimeline* pITimeline, HDC hDC, const CRect& rcClient, const long lOffset, const WaveDrawParams* pWaveDrawParams, const WaveTimelineInfoParams* pWaveInfoParams);
	HRESULT STDMETHODCALLTYPE GetWaveInfo(WaveInfoParams* pWaveInfoParams);
	HRESULT STDMETHODCALLTYPE SampleToRefTime(DWORD dwSample, REFERENCE_TIME* prtSampleTime, LONG lFineTuneCents);
	HRESULT STDMETHODCALLTYPE RefTimeToSample(REFERENCE_TIME rtSampleTime, DWORD* pdwSample, LONG lFineTuneCents);

	// IDMUSProdNotifySink method
	STDMETHOD(OnUpdate)(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

	// AListItem method
	CWaveNode* GetNext() {return(CWaveNode*)AListItem::GetNext();}

	// Accessors And Mutators
	GUID			GetFileGUID();								// Gets the m_guidFile that's written to DMUS_FOURCC_GUID_CHUNK
	void			SetFileGUID(GUID guidNew);					// Sets the m_guidFile that's written to DMUS_FOURCC_GUID_CHUNK
	CWave*			GetWave() const;
	CDLSComponent*	GetComponent() const;
	CCollection*	GetCollection() const;
	void			SetCollection(CCollection* pCollection);
	
	CWaveCtrl*	GetWaveEditor() const;
	void		SetWaveEditor(CWaveCtrl* pWaveCtrl);

	HRESULT	InitAndDownload();										// Downloads itself to all the ports
	HRESULT UnloadFromAllPorts();									// Unloads itself from all the ports
	HRESULT DownloadToAllPorts();									// Downloads itself to all the ports without doing DM_Init
	HRESULT	DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad);	// Initializes the download buffers
	HRESULT	Download(IDirectMusicPortDownload* pIDMPortDownLoad);	// Downloads the Wave
	HRESULT	Unload(IDirectMusicPortDownload* pIDMPortDownLoad);		// Unloads the Wave

    BOOL    IsSourceInTempFile();
    void    SetSourceInTempFile(BOOL bSourceInTempFile);

	void	RegisterWithTransport();
	void	UnRegisterFromTransport();

	HRESULT	OnViewProperties();		// Shows the wave property page
	void	RefreshPropertyPage();	// Asks the wave to update it's property page

	bool	IsInACollection();		// Returns true if the node's in a collection
	bool	IsStereo();				// Does this nodes contain a stereo wave?

	HRESULT	GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime);	// Computes the time required for this wave to play once at the passed tempo
	void	DrawWave(CDC* pDC, const CRect& rcClient);			// Draws the wave on the passed DC within the passed bounding rect
	
	static void		CopyGUID(const GUID& guidSrc, GUID& guidDest);
	static HRESULT	IsLoadingStereoWave(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool& bStereo, bool& bTrueStereo, GUID& guidStereoWave, bool& bRightChannel);	// Returns true if we're trying to load a stereo wave else false
	
	void	SetFileName(CString sFileName);
	CString GetFileName();

	GUID	GetGUID();
	void	CreateNewVersionGUID();
	GUID	GetVersionGUID();

	void	SetTransportName();
	HRESULT SetSelected(BOOL bSelect = TRUE);

	// Helpers for DirectSoundWave object
	HRESULT				CreateDirectSoundWave();	
	HRESULT				ReloadDirectSoundWave();
	IDirectSoundWave*	GetDirectSoundWave();

	HRESULT	GetFineTunedSampleRate(long lFineTuneCents, DWORD& dwTunedSampleRate);

	void	SetPreviousRegionLinkIDs(CWaveNode* pSourceWaveNode);

	bool	GetDragCopy();
	void	SetDragCopy(bool bDragCopy);

	void	SetDirtyFlag();
	void	ClearDirtyFlag();


public:
	CRITICAL_SECTION	m_CriticalSection;			// Needed to guard wave data access calls
	GUID				m_guidFile;					// GUID written to DMUS_FOURCC_GUID_CHUNK
	HANDLE				m_hStopEvent;			// Stop event

private:
	
	CWaveNode( CDLSComponent* pComponent, bool bStereo = false);
	~CWaveNode();

	HRESULT				Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	static CWaveNode*	FindNodeForStereoGuid(CCollectionWaves* pWavesCollection, const GUID& guidStereoWave);
	bool				ConfirmWaveDelete(); // Confirms the delete action and deletes the wave

	GUID				m_guidVersion;				// GUID that identifies a particular version of the node
	IDirectSoundWave*	m_pIDirectSoundWave;		// DirectSoundWave object for the wave track
	CWaveStream*		m_pDirectSoundWaveStream;	// Stream that gets passed to the DirectSoundWave
	CPtrList			m_lstWaveTrackDownload;		// List of ports and pchannels that the wave is downloaded to

	WaveStopResetTimeEvent	m_ResetCallBackParams;  // Event to reset the play state for the playing wave

	long			m_lRef;			// Reference counter
	IDMUSProdNode*	m_pIRootNode;	// DocRoot node for this wave
	IDMUSProdNode*	m_pIParentNode;	// Parent node of this wave

	CDLSComponent*	m_pComponent;	// DLS Component
	bool			m_bInCollection;// Is the node in a DLS collection?
	CWave*			m_pWave;		// Wave object
	bool			m_bStereo;		// This wave is a stereo wave
	CWaveCtrl*		m_pWaveCtrl;	// The Wave Editor
	HWND			m_hWndEditor;	// Handle to the editor window

	bool			m_bTrackTimeCursor;	// Are we showing the cursor as the wave plays?

	CString			m_sFileName;		// The wave file name with the complete path

	bool			m_bDragCopy;		// To mark if wave node has been copied for drag-drop of instruments
	bool			m_bPastingWave;		// Flag to remember we're pasting a wave
	CString			m_sTempFileName;	// Temp file used during pasting waves
    BOOL            m_bSrcInATempFile;     // Flag to indicate the wave data lives in a temp source file; not saved to a WVP or a DLP file yet

	GUID			m_guidWaveNode;		// GUID that uniquelyu identifies this node

	BOOL			m_bIsDirty;			// Flag to mark this file as modified so user can be prompted...

	

};

#endif // __WAVENODE_H__
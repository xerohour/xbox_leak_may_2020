#if !defined(CCONDUCTOR_H__36F6DDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
#define CCONDUCTOR_H__36F6DDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CConductor.h : Declaration of the CConductor

#include "resource.h"
#include "DMUSProd.h"
#include "Conductor.h"
#include <dmusicc.h>
#include <dmusici.h>
#include <PrivateTransport.h>

class CConductor;
class COutputTool;
class CNotifyTool;
class MIDISaveTool;
class CStatusToolbarHandler;
class CSynthStatusToolbar;
class CPhoneyDSound;
interface ITransportRegEntry;
interface ISecondaryTransportRegEntry;

extern const IID IID_ITransportRegEntry;
extern const IID IID_ISecondaryTransportRegEntry;
extern CComModule _Module;
extern CConductor* g_pconductor;

BOOL SetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpszString );
BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite );
BOOL GetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize );
BOOL GetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD *pdwData );
BOOL GetRegData( HKEY hKey, LPCTSTR lpValueName, BYTE **ppbData, DWORD *pdwSize );

#define LATENCY_UNSUPPORTED 0x7fffffff

// Flag that we're transitioning to nothing
#define TRANS_TO_NOTHING 0x80000000
#define TRANS_PRIVATE_FLAGS (TRANS_TO_NOTHING)

#define RELEASE(x) { if (x) { x->Release(); x = NULL; } } 

// CAudiopathListItem
struct CAudiopathListItem
{
	CAudiopathListItem( IDMUSProdNode *pNewAudiopathNode, DWORD dwNewStandardID, DWORD dwXboxPathID )
	{
		pNode = pNewAudiopathNode;
		if( pNode )
		{
			pNode->AddRef();
		}

		dwStandardID = dwNewStandardID;
        dwXboxID = dwXboxPathID;
	}
	~CAudiopathListItem()
	{
		RELEASE( pNode );
	}

	IDMUSProdNode *pNode;
	DWORD dwStandardID;     // PC Audiopath ID.
    DWORD dwXboxID;         // Optional ID if this is playing on an Xbox.
	CString strName;
};

// SegStateAudioPath
struct SegStateAudioPath
{
	SegStateAudioPath( IDirectMusicAudioPath *pNewAudiopath, IDirectMusicSegmentState8 *pNewSegmentState )
	{
		pAudiopath = pNewAudiopath;
		if( pAudiopath )
		{
			pAudiopath->AddRef();
		}
		pSegmentState = pNewSegmentState;
		if( pSegmentState )
		{
			pSegmentState->AddRef();
		}
	}
	~SegStateAudioPath()
	{
		RELEASE( pAudiopath );
		RELEASE( pSegmentState );
	}
	IDirectMusicAudioPath *pAudiopath;
	IDirectMusicSegmentState8 *pSegmentState;
};


/////////////////////////////////////////////////////////////////////////////
// GetNewRegDWORD - Gets a DWORD from the new PortConfig registry key

inline BOOL GetNewRegDWORD( LPCTSTR lpValueName, DWORD *pdwData )
{
	return GetRegDWORD( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\"),
		lpValueName, pdwData );
}

/////////////////////////////////////////////////////////////////////////////
// GetNewRegString - Gets a string from the new PortConfig registry key

inline BOOL GetNewRegString( LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize )
{
	return GetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\"),
		lpValueName, lpszString, lpdwSize );
}

/////////////////////////////////////////////////////////////////////////////
// SetNewRegDWORD - Sets a DWORD in the new PortConfig registry key

inline BOOL SetNewRegDWORD( LPCTSTR lpValueName, DWORD dwData, BOOL fOverWrite )
{
	return SetRegDWORD( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\"),
		lpValueName, &dwData, fOverWrite );
}

/////////////////////////////////////////////////////////////////////////////
// SetNewRegString - Sets a string in the new PortConfig registry key

inline BOOL SetNewRegString( LPCTSTR lpValueName, LPTSTR lpszString )
{
	return SetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\"),
		lpValueName, lpszString );
}

typedef enum ODevice
{
    OD_ONE,
    OD_TWO
} ODevice;

class CDownloadedInstrumentList
{
public:
	CDownloadedInstrumentList();
	~CDownloadedInstrumentList();

 	IDirectMusicPort	*m_pDMPort; // Pointer to the IDirectMusicPort object.
	CTypedPtrList<CPtrList, IDirectMusicDownloadedInstrument*> m_lstDownloadedInstruments;
};

class CNotifyEntry
{
public:
	CNotifyEntry( IDMUSProdNotifyCPt* pINotifyCPt, REFGUID guid )
	{
		memcpy( &m_guid, &guid, sizeof(GUID) );
		m_lstNotifyCPt.RemoveAll();
		m_lstNotifyCPt.AddHead( pINotifyCPt );
	}

	~CNotifyEntry()
	{
		/*
		IDMUSProdNotifyCPt* pNotifyCPt = NULL;
		do
		{
			pNotifyCPt = m_lstNotifyCPt.RemoveHead();
			if ( pNotifyCPt != NULL )
			{
				pNotifyCPt->Release();
			}
		} while ( pNotifyCPt != NULL );
		*/
		m_lstNotifyCPt.RemoveAll();
	}

	GUID		m_guid;
    CTypedPtrList<CPtrList, IDMUSProdNotifyCPt*> m_lstNotifyCPt;
};

#define B_NOT_USING_ENGINE		0x001
#define B_PLAY_ENABLED			0x002
#define B_PLAY_CHECKED			0x004
#define B_STOP_ENABLED			0x008
#define B_STOP_CHECKED			0x010
#define B_REC_ENABLED			0x020
#define B_REC_CHECKED			0x040
#define B_TRANS_ENABLED			0x080
#define B_TRANS_CHECKED			0x100
#define B_TEMPO_ENABLED			0x200
#define B_TEMPO_RATIO_ENABLED	0x400
#define B_TEMPO_RATIO_CHECKED	0x800

#define B_USER_FLAGS (B_NOT_USING_ENGINE | B_PLAY_ENABLED | B_PLAY_CHECKED | \
				B_STOP_ENABLED | B_STOP_CHECKED | B_REC_ENABLED | B_REC_CHECKED)

#define FOURCC_2NDARY_TOOLBAR_LIST		mmioFOURCC('2','t','b','l')
#define FOURCC_2NDARY_TOOLBAR_CHUNK		mmioFOURCC('2','n','t','c')
#define FOURCC_2NDARY_TOOLBAR_SIZE		mmioFOURCC('2','n','t','s')
#define FOURCC_2NDARY_TOOLBAR_FLAGS		mmioFOURCC('2','n','t','f')

/////////////////////////////////////////////////////////////////////////////
// Conductor

class CConductor :
	public IDMUSProdConductor8,
	public IDMUSProdComponent,
	public IPrivateTransport,
	public IDMUSProdConductorPrivate,
	public CComObjectRoot,
	public CComCoClass<CConductor,&CLSID_CConductor>,
	public IConnectionPointContainerImpl<CConductor>,
	public IConnectionPointImpl<CConductor,&IID_IDMUSProdMidiInCPt>,
	public IPersistStream,
	public IDMUSProdNode,
	public IDMUSProdNotifySink
	{
	friend class CSecondaryToolbarHandler;
	friend class CToolbarHandler;
	friend class COptionsToolbarHandler;
	friend class DlgMIDI;
	friend class DlgMetronome;
	friend class DlgAltTempo;
	friend class DlgTransition;
	friend class CStatusToolbarHandler;
	friend class CWaveRecordToolbar;
	friend class COutputTool;
	friend class CRatioButton;
	friend class CMyButton;
	friend class CMIDIInputContainer;
	friend class CSynthButton;
	friend UINT AFX_CDECL MIDIInThreadProc( LPVOID pParam );
	friend UINT AFX_CDECL NotifyThreadProc( LPVOID pParam );
	friend void CALLBACK EXPORT TimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
	friend void CALLBACK EXPORT SynthStatusTimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
	friend void AddToolsAndSetupWaveSaveForSegState( IUnknown *punk );
	friend bool GetPChannelName( DWORD dwPChannel, CString &strName );


public:
	CConductor();
	~CConductor();
	HRESULT InitializeDirectMusic( CString &strError );
	void AddToolsToToolgraph(IDirectMusicGraph *pGraph, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool );
	void AddToolsToSegState(IDirectMusicSegmentState8 *pIDirectMusicSegmentState, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool );
	void AddTools(IDirectMusicAudioPath *pAudiopath, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool );
	//void AddTools(IDirectMusicPerformance* pPerf, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool );
	void SetupWaveSaveDMO( IDirectMusicAudioPath *pAudiopath );
	HRESULT DownloadGMCollection();
	void SendAllNotesOff( long lTime, BOOL fSendGMReset );
	HRESULT SendGMReset( DWORD dwPChannel );
	HRESULT SendMIDIMessage( DWORD dwPChannel, long lTime, BYTE bStatus, BYTE bByte1 );
	void StopAllNotesAndSegments( void );
	HRESULT CreateAudiopath( CAudiopathListItem *pAudiopathListItem, IDirectMusicAudioPath **ppIDirectMusicAudioPath );

	BEGIN_COM_MAP(CConductor)
		COM_INTERFACE_ENTRY(IDMUSProdConductor)
		COM_INTERFACE_ENTRY(IDMUSProdConductor8)
		COM_INTERFACE_ENTRY(IDMUSProdComponent)
		COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IPrivateTransport)
		COM_INTERFACE_ENTRY(IDMUSProdConductorPrivate)
		COM_INTERFACE_ENTRY(IPersistStream)
		COM_INTERFACE_ENTRY(IDMUSProdNode)
		COM_INTERFACE_ENTRY(IDMUSProdNotifySink)
	END_COM_MAP()

	DECLARE_NOT_AGGREGATABLE(CConductor)
	// Remove the comment from the line above if you don't want your object to
	// support aggregation or if you want to provide support for
	// Microsoft Transaction Server. The default is to support it

	DECLARE_REGISTRY_RESOURCEID(IDR_Conductor)

	// IDMUSProdConductor
	HRESULT STDMETHODCALLTYPE GetPerformanceEngine(
			/* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppPerformance );
	HRESULT STDMETHODCALLTYPE GetPort(
			/* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppPort );
	HRESULT STDMETHODCALLTYPE RegisterTransport(
			/* [in] */ IDMUSProdTransport *pTransport,
			/* [in] */ DWORD dwFlags );
	HRESULT STDMETHODCALLTYPE UnRegisterTransport(
			/* [in] */ IDMUSProdTransport *pTransport );
	HRESULT STDMETHODCALLTYPE SetActiveTransport(
			/* [in] */ IDMUSProdTransport *pTransport,
			/* [in] */ DWORD btnFlags);
	HRESULT STDMETHODCALLTYPE IsTransportPlaying(
			/* [in] */ IDMUSProdTransport *pTransport );
	HRESULT STDMETHODCALLTYPE PlayMIDIEvent(
			/* [in] */ BYTE bStatus,
			/* [in] */ BYTE bData1,
			/* [in] */ BYTE bData2,
			/* [in] */ DWORD dwTime );
	HRESULT STDMETHODCALLTYPE SetBtnStates(
			/* [in] */ IDMUSProdTransport *pTransport,
			/* [in] */ DWORD btnFlags );
	HRESULT STDMETHODCALLTYPE SetTempo(
			/* [in] */ IDMUSProdTransport *pTransport,
			/* [in] */ double dblTempo,
			/* [in] */ BOOL fEnable );
	HRESULT STDMETHODCALLTYPE RegisterNotify(
			/* [in] */ IDMUSProdNotifyCPt *pNotifyCPt,
			/* [in] */ REFGUID guidNotify );
	HRESULT STDMETHODCALLTYPE UnregisterNotify(
			/* [in] */ IDMUSProdNotifyCPt *pNotifyCPt,
			/* [in] */ REFGUID guidNotify );
	HRESULT STDMETHODCALLTYPE SetTransportName(
			/* [in] */ IDMUSProdTransport *pTransport,
			/* [in] */ BSTR bstrName );
	HRESULT STDMETHODCALLTYPE TransportStopped(
			/* [in] */ IDMUSProdTransport *pTransport );
	HRESULT STDMETHODCALLTYPE GetTransitionOptions(
			/* [out,retval] */ ConductorTransitionOptions *pTransitionOptions );
	HRESULT STDMETHODCALLTYPE RegisterSecondaryTransport(
			/* [in] */ IDMUSProdSecondaryTransport *pSecondaryTransport );
	HRESULT STDMETHODCALLTYPE UnRegisterSecondaryTransport(
			/* [in] */ IDMUSProdSecondaryTransport *pSecondaryTransport );
	HRESULT STDMETHODCALLTYPE SetSecondaryTransportName(
			/* [in] */ IDMUSProdSecondaryTransport *pSecondaryTransport,
			/* [in] */ BSTR bstrName );
    HRESULT STDMETHODCALLTYPE IsSecondaryTransportPlaying(
			/* [in] */ IDMUSProdSecondaryTransport *pSecondaryTransport );
	HRESULT STDMETHODCALLTYPE SetPChannelThru(
			/* [in] */ DWORD dwInputChannel,
			/* [in] */ DWORD dwPChannel );
	HRESULT STDMETHODCALLTYPE CancelPChannelThru(
			/* [in] */ DWORD dwInputChannel );
	HRESULT STDMETHODCALLTYPE IsTrackCursorEnabled( );

	// IDMUSProdConductor8
	HRESULT STDMETHODCALLTYPE RegisterAudiopath(
			/* [in] */ IDMUSProdNode *pAudiopathNode,
			/* [in] */ BSTR bstrName );
	HRESULT STDMETHODCALLTYPE UnRegisterAudiopath(
			/* [in] */ IDMUSProdNode *pAudiopathNode );
	HRESULT STDMETHODCALLTYPE SetAudiopathName(
			/* [in] */ IDMUSProdNode *pAudiopathNode,
			/* [in] */ BSTR bstrName );
	HRESULT STDMETHODCALLTYPE GetDownloadCustomDLSStatus(
			/* [out,retval] */ BOOL *pfDownloadCustomDLS );
	HRESULT STDMETHODCALLTYPE GetDownloadGMStatus(
			/* [out,retval] */ BOOL *pfDownloadGM );
	HRESULT STDMETHODCALLTYPE SetupMIDIAndWaveSave(
			/* [in] */ IUnknown *punkSegmentState );
	HRESULT STDMETHODCALLTYPE GetAudioParams(
			/* [out,retval] */ DMUS_AUDIOPARAMS *pDMUSAudioParams );

	// IDMUSProdComponent
	HRESULT STDMETHODCALLTYPE Initialize(
			/* [in] */ IDMUSProdFramework __RPC_FAR *pFramework, BSTR __RPC_FAR* pbstrErrorText );
	HRESULT STDMETHODCALLTYPE CleanUp();
	HRESULT STDMETHODCALLTYPE GetName(
			/* [retval][out] */ BSTR __RPC_FAR *pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp( BOOL fActivate );

	// IPrivateTransport
    HRESULT STDMETHODCALLTYPE PlayFromStart();
	HRESULT STDMETHODCALLTYPE PlayFromCursor();
	HRESULT STDMETHODCALLTYPE Record();
	HRESULT STDMETHODCALLTYPE Transition();
	HRESULT STDMETHODCALLTYPE SpaceBarPress();
	HRESULT STDMETHODCALLTYPE PlaySecondarySegment( int nSecondarySegment );

	// IDMUSProdConductorPrivate
	HRESULT STDMETHODCALLTYPE GetDefaultAudiopathNode( IDMUSProdNode **ppAudiopathNode );
	HRESULT STDMETHODCALLTYPE PleaseRedownload( void );

	// IPersistStream
	HRESULT STDMETHODCALLTYPE IsDirty(void);
	HRESULT STDMETHODCALLTYPE Load(
		/* [unique][in] */ IStream *pIStream);
	HRESULT STDMETHODCALLTYPE Save(
		/* [unique][in] */ IStream *pIStream,
		/* [in] */ BOOL fClearDirty);
	HRESULT STDMETHODCALLTYPE GetSizeMax(
		/* [out] */ ULARGE_INTEGER *pcbSize);
	HRESULT STDMETHODCALLTYPE GetClassID( 
		/* [out] */ CLSID __RPC_FAR *pClassID);

	// IDMUSProdNotifySink
    HRESULT STDMETHODCALLTYPE OnUpdate( 
        /* [in] */ IDMUSProdNode *pIDocRootNode,
        /* [in] */ GUID guidUpdateType,
        /* [in] */ void *pData);

    // IDMUSProdNode
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

private:
	void ReleaseAll();
	BOOL IsActiveUsingEngine( void );
	BOOL IsPlayingUsingEngine( void );
	ITransportRegEntry *GetDisplayedTransport( void );
	HRESULT SuspendOutput();
	HRESULT ResumeOutput();
	void BroadCastPortChangeNotification( void );
	void BroadCastPortRemovalNotification( void );
	void BroadCastWaveNotification( REFGUID rguidNotification );
	BOOL IsEnginePlaying( void );
	void ReadDefaultDirectMusicAudiopath( IDirectMusicAudioPath **ppAudiopath );
	void WriteDefaultDirectMusicAudiopath( void );
	HRESULT ActivateAudiopath( IDirectMusicAudioPath *pAudiopath, bool fSendNotificationsAndDownloadGM = true );
    HRESULT ActivateXboxPath( DWORD dwXboxPath);
    void UpdateMIDIThruChannels( void );
	void ClearThruConnections( void );
	void StopCursor( void );
	void StartCursor( void );
	void SetMIDIExportFields( IUnknown* pIUnknown );
	BOOL AllTransportsSetToPlay( void );
	void UpdateLatency( void );
	void UpdateAudioSettings( void );
	void DownOrUnLoadGM( void );
	void DownOrUnLoadDLS( void );

public:
	IDirectMusicPerformance8*			m_pDMPerformance;
	IDirectMusic8*						m_pDMusic;
	BOOL								m_fShutdown;
	BOOL								m_fOutputEnabled;
	IUnknown*							m_pIUnknownMIDIExport;
	IDirectMusicSegmentState*			m_pISegStateMIDIExport;
	BOOL								m_fLeadInMeasureMIDIExport;
	COutputTool*						m_pOutputTool;
	MIDISaveTool*						m_pMIDISaveTool;
	CNotifyTool*						m_pNotifyTool;

	// Audiopath stuff
	IDirectMusicAudioPath*				m_pDMAudiopath;
	CAudiopathListItem*					m_pAudiopathListItem;

	// Public phoney DSound stuff
	CPhoneyDSound						*m_pPhoneyDSound;
	void UsePhoneyDSound( bool fUsePhoneyDSound );

private:
	IDMUSProdFramework8* 				m_pFrameWork;
	HWND								m_hWndFramework;

	CTypedPtrList<CPtrList, CDownloadedInstrumentList*> m_lstDownloadedInstrumentList;
	HANDLE								m_hNotifyEvent;
	HANDLE								m_hNotifyExitEvent;
	bool								m_fDownloadGM;
	bool								m_fDownloadDLS;

	CTypedPtrList<CPtrList, CSecondaryToolbarHandler*> m_lstSecondaryToolbars;
	CStatusToolbarHandler*				m_pStatusToolbarHandler;
	CSynthStatusToolbar*				m_pSynthStatusToolbar;
	CToolbarHandler*					m_pToolbarHandler;
	COptionsToolbarHandler*				m_pOptionsToolbarHandler;
	CWaveRecordToolbar*					m_pWaveRecordToolbar;
	//bool								m_fAudiopathHasDumpDMOs;
    CTypedPtrList<CPtrList, ITransportRegEntry*> m_lstITransportRegEntry;
    CTypedPtrList<CPtrList, ISecondaryTransportRegEntry*> m_lstISecondaryTransportRegEntry;
	CTypedPtrList<CPtrList, CAudiopathListItem*> m_lstAudiopaths;
	CTypedPtrList<CPtrList, SegStateAudioPath*> m_lstSegStateAudioPaths;
	ITransportRegEntry*					m_rpActive;
	ITransportRegEntry*					m_rpPlaying;
	int 								m_nRatio;
	double								m_dblTempo;
	ConductorTransitionOptions			m_TransitionOptions;
	DWORD								m_dwLatency;
	DWORD								m_dwDefaultLatency;
	bool								m_fLatencyAppliesToAllAudiopaths;
	DWORD								m_dwSampleRate;
	DWORD								m_dwVoices;
	CLSID								m_clsidDefaultSynth;

	// Private Phoney DSound stuff
	bool								m_fUsePhoneyDSound;
	CLSID								m_clsidPhoneyOutput;

	BOOL								m_fMetronomeEnabled;
	BYTE								m_bMetronomeNoteOne;
	BYTE								m_bMetronomeVelocityOne;
	BYTE								m_bMetronomeNoteOther;
	BYTE								m_bMetronomeVelocityOther;
	DWORD								m_dwMetronomePChannel;

    CTypedPtrList<CPtrList, CNotifyEntry*> m_lstNotifyEntry;
	CRITICAL_SECTION					m_csNotifyEntry;
	CRITICAL_SECTION					m_csAudiopath;
	CRITICAL_SECTION					m_csAudiopathList;

	BEGIN_MSG_MAP(CConductor)
	END_MSG_MAP()

	// connection point for MIDI input sink
	BEGIN_CONNECTION_POINT_MAP( CConductor )
		CONNECTION_POINT_ENTRY( IID_IDMUSProdMidiInCPt )
	END_CONNECTION_POINT_MAP()
};

#endif // !defined(CCONDUCTOR_H__36F6DDF3_46CE_11D0_B9DB_00AA00C08146__INCLUDED)

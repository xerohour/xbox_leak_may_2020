// Segment.cpp : implementation file
//

/*-----------
@doc DMUSPROD
-----------*/

#include "stdafx.h"

#include "SegmentDesignerDLL.h"
#include "SegmentDesigner.h"
#include <DMUSProd.h>
#include "StyleDesigner.h"
#include "ChordMapDesigner.h"
#include "Segment.h"
#include "StyleRiffId.h"
#include <RiffStrm.h>
#include "SegmentDlg.h"
#include "SegmentPPGMgr.h"
#include "RiffStructs.h"
#include <mmreg.h>
#include "CommandList.h"
#include "ChordList.h"
#include "StyleRef.h"
#include "Track.h"
#include "resource.h"
#include "FileStructs.h"
#include "SegmentComponent.h"
#include "SegmentRiff.h"
#include <SegmentGuids.h>
#include "BandEditor.h"
#include "MuteList.h"
#include "UndoMan.h"
#include "SegmentIO.h"
#include "AudioPathDesigner.h"
#include "ToolGraphDesigner.h"
#include "ContainerDesigner.h"
#include "dmusicp.h"
#include "TabBoundaryFlags.h"

#define IDD_VELOCITY 1
#include "DialogVelocity.h"
#define MS_TO_REFTIME (10000i64)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// HACKHACK: Work around problem Zombie'ing segments during playback
/*
static const IID IID_CSegment =
{ 0xb06c0c21, 0xd3c7, 0x11d3, { 0x9b, 0xd1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
*/


// The DX7 Lyric track GUID is used to read legacy files
// {ee279463-b2e5-11d1-888F-00C04FBF8D15}
static const CLSID CLSID_DX7_LyricTrack =
{ 0xee279463, 0xb2e5, 0x11d1, { 0x88, 0x8f, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };


#define NOTE_TO_CLOCKS(note, ppq)	( (ppq) * 4 / (note) )
#define IMA_PPQ 192
#define IMA25_VOICEID_TO_PCHANNEL( id )  ( ( id + 4 ) & 0xf )
#define TRACKCONFIG_FLAGSTOCHECK (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED)
#define REFCLOCKS_PER_MINUTE (10000 * 1000 * 60)

extern HRESULT CreateSegmentFromMIDIStream(CSegment* pSegment, LPSTREAM pStream);
extern HRESULT WriteCombinedSeqTracks( IStream *pIStream, CTypedPtrList<CPtrList, CTrack*> &lstSeqTracks );

// FROM SuperJAM! Drum Map
static char achMappingsToDMUS[128] = {
         0,			//0 },
         35,		//1  },
         36,		//2  },
         38,		//3  },
         40,		//4  },
         41,		//5  },
         45,		//6  },
         48,		//7  },
         42,		//8  },
         44,		//9  },
         46,		//10 },
         39,		//11 },
         37,		//12 },
         51,		//13 },
         49,		//14 },
         54,		//15 },
         56,		//16 },
         61,		//17 },
         60,		//18 },
         64,		//19 },
         63,		//20 },
         66,		//21 },
         65,		//22 },
         69,		//23 },
         70,		//24 },
         71,		//25 },
         72,		//26 },
         73,		//27 },
         75,		//28 },
         47,		//29 },
         50,		//30 },
         53,		//31 },
         57,		//32 },
         52,		//33 },
         74,		//34 },
		 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
		 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
		 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
		 95, 96, 97, 98,
         87,		//99 },
         86,		//100 },
         85,		//101 },
         84,		//102 },
         83,		//103 },
         82,		//104 },
         81,		//105 },
         80,		//106 },
         79,		//107 },
         78,		//108 },
         77,		//109 },
         76,		//110 },
         68,		//111 },
         67,		//112 },
         62,		//113 },
         59,		//114 },
         58,		//115 },
         55,		//116 },
         43,		//117 },
         34,		//118 },
         33,		//119 },
         32,		//120 },
         31,		//121 },
         30,		//122 },
         29,		//123 },
         28,		//124 },
         27,		//125 },
         26,		//126 },
		 25,		//127 }
};

/////////////////////////////////////////////////////////////////////////////
// swaplong : switches byte order of a long

static void swaplong(char data[])

{
    char temp;
    temp = data[0];
    data[0] = data[3];
    data[3] = temp;
    temp = data[1];
    data[1] = data[2];
    data[2] = temp;
}

// This code is shared with Pattern.cpp in the Style Designer project
HRESULT DoAutoTransition( const ConductorTransitionOptions &TransitionOptions, IDirectMusicPerformance *pPerformance,
						IDirectMusicComposer8 *pIDMComposer, IDirectMusicSegment *pSegmentTo,
						IDirectMusicChordMap *pIDMChordMap, IDirectMusicSegmentState **ppTransitionSegmentState,
						IDirectMusicSegmentState **ppSegmentStateTo )
{
	IDirectMusicSegment *pITransitionSegment = NULL;

	// Set the transition pattern (embellishment)
	WORD wCommand = 0;
	if( !(TransitionOptions.dwFlags & TRANS_REGULAR_PATTERN) )
	{
		// if we have an embellishment, set it.
		wCommand = TransitionOptions.wPatternType;
	}

	// Set the transition flags
	DWORD dwFlags = TransitionOptions.dwBoundaryFlags;

	if( TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
	{
		// Can't transition at COMPOSEF_QUEUE, so just use COMPOSEF_SEGMENTEND
		dwFlags |= DMUS_COMPOSEF_SEGMENTEND;
	}

	HRESULT hr = pIDMComposer->AutoTransition(pPerformance, 
											   pSegmentTo, 
											   wCommand, 
											   dwFlags, 
											   pIDMChordMap, 
											   &pITransitionSegment, 
											   ppSegmentStateTo, 
											   ppTransitionSegmentState);

	if( pITransitionSegment )
	{
		pITransitionSegment->Release();
		pITransitionSegment = NULL;
	}

	if( ppSegmentStateTo
	&&	(NULL == *ppSegmentStateTo) )
	{
		hr = E_FAIL;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler

BEGIN_MESSAGE_MAP(CNotificationHandler, CWnd)
	//{{AFX_MSG_MAP(CSegmentDlg)
	ON_MESSAGE(WM_APP, OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler message handlers

LRESULT CNotificationHandler::OnApp( WPARAM wParam, LPARAM lParam )
{
	// Reset cursor to the start
	if( m_pSegment && (wParam==0) )
	{
		m_pSegment->OnNotificationCallback();

		DMUS_NOTIFICATION_PMSG* pNotifyEvent = reinterpret_cast<DMUS_NOTIFICATION_PMSG *>(lParam);
		if( pNotifyEvent )
		{
			// Notify the strip managers
			if( m_pSegment->m_pSegmentDlg
			&&  m_pSegment->m_pSegmentDlg->m_pTimeline )
			{
				m_pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( pNotifyEvent->guidNotificationType, 0xFFFFFFFF, pNotifyEvent );
			}
			else
			{
				m_pSegment->NotifyAllStripMgrs( pNotifyEvent->guidNotificationType, 0xFFFFFFFF, pNotifyEvent );
			}

			m_pSegment->m_pComponent->m_pIDMPerformance->FreePMsg( reinterpret_cast<DMUS_PMSG *>(pNotifyEvent) );
		}
	}
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment constructor/destructor

CSegment::CSegment( CSegmentComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;
	//GMaxRefTracker_StartTrack((ULONG *)&m_dwRef);
	AddRef();

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_pIDocRootNode	= this;
	m_pIParentNode	= NULL;
	m_pIContainerNode = NULL;
	m_pIAudioPathNode = NULL;
	m_pIToolGraphNode = NULL;
	m_hWndEditor	= NULL;
	m_fDeletingContainer = false;

	m_pSegmentDlg	= NULL;
	m_pIDMSegment	= NULL;
	m_pIDMSegment8	= NULL;
	m_pIDMCurrentSegmentState = NULL;
	::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicSegment, (void**)&m_pIDMSegment );
	ASSERT( m_pIDMSegment );
	if( m_pIDMSegment )
	{
		m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8, (void**)&m_pIDMSegment8 );
	}

	m_rtCurrentStartTime = 0;
	m_mtCurrentStartTime = 0;
	m_mtCurrentStartPoint = 0;
    m_rtCurrentLoopStart = 0;
    m_rtCurrentLoopEnd = 0;
	m_mtCurrentLoopStart = 0;
	m_mtCurrentLoopEnd = 0;
	m_dwCurrentMaxLoopRepeats = 0;

	// Segment Data
	m_fModified				= FALSE;
	m_fTransportRegistered	= FALSE;
	m_fSettingLength		= FALSE;
	m_fAddedToTree			= FALSE;
	m_fInUndo				= FALSE;
	m_fHaveTempoStrip		= FALSE;
	m_fRecordPressed		= FALSE;
	m_fTrackTimeCursor		= TRUE;
	m_fInTransition			= FALSE;
	m_fCtrlKeyDownWhenStopCalled = FALSE;
	m_dwTransitionPlayFlags = 0;
	m_pIDMTransitionSegment = NULL;
	m_pUndoMgr				= NULL;

	m_ProxyStripMgr.m_pSegment = this;

	m_dblZoom = 0.0;
	m_lVerticalScroll = 0;
	m_dblHorizontalScroll = 0.0;
	m_tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
	m_lFunctionbarWidth = 0;

	m_wLegacyActivityLevel = 5;
	m_fLoadedLegacyActivityLevel = false;

	m_SequenceQuantizeParms.m_wQuantizeTarget = QUANTIZE_TARGET_SELECTED;
	m_SequenceQuantizeParms.m_bResolution = 4;	
	m_SequenceQuantizeParms.m_bStrength = 100;
	m_SequenceQuantizeParms.m_dwFlags = SEQUENCE_QUANTIZE_START_TIME;		

	m_SequenceVelocitizeParams.m_wVelocityTarget = VELOCITY_TARGET_SELECTED;
	m_SequenceVelocitizeParams.m_bCompressMin = 0;
	m_SequenceVelocitizeParams.m_bCompressMax = 127;
	m_SequenceVelocitizeParams.m_lAbsoluteChangeStart = 0;
	m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd = 0;
	m_SequenceVelocitizeParams.m_dwVelocityMethod = SEQUENCE_VELOCITIZE_PERCENT;

	// Initialize the DirectMusic data
	m_dwLoopRepeats = 0;
	m_mtLength = 20 * DMUS_PPQ * 4; // Default to 20 measures of 4/4 time
	m_mtPlayStart = 0;
	m_mtLoopStart = 0;
	m_mtLoopEnd = 0;
	m_dwResolution = 0;//DMUS_SEGF_MEASURE; (Immediate)
	m_dwSegmentFlags = 0;
	m_rtLength = 0;
    m_rtLoopStart = 0;
    m_rtLoopEnd = 0;
	if( m_pIDMSegment )
	{
		m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
		m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		m_pIDMSegment->SetLength( m_mtLength );
		m_pIDMSegment->SetStartPoint( m_mtPlayStart );
		m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
		// No need to set m_dwSegmentFlags or rtLength, they're all 0 by default
	}
	::CoCreateGuid( &m_PPGSegment.guidSegment );

	InitializeCriticalSection( &m_csSegmentState );

	// Set the notification window to point to us
	m_wndNotificationHandler.m_pSegment = this;

	m_fBrandNew = false;
	m_fPropPageActive = false;

	// Create the notification window
	RECT rect;
	rect.top = 0;
	rect.bottom = 0;
	rect.left = 0;
	rect.right = 0;
	m_wndNotificationHandler.Create( NULL, _T("Hidden segment notification handler"), WS_CHILD, rect, CWnd::FromHandle(GetDesktopWindow()), 0 );
}

CSegment::~CSegment()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//GMaxRefTracker_StopTrack((ULONG *)&m_dwRef);
	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	if ( !m_lstTracks.IsEmpty() )
	{
		CTrack	*pTrack;
		while( !m_lstTracks.IsEmpty() )
		{
			pTrack = m_lstTracks.RemoveHead() ;
			delete pTrack;
		}
	}

	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		pIPropSheet->RemovePageManagerByObject(this);
		if(m_pComponent->m_pIPageManager)
		{
			m_pComponent->m_pIPageManager->RemoveObject(this);
		}
		pIPropSheet->Release();
	} 

	EnterCriticalSection( &m_csSegmentState );
	while( !m_lstSegStates.IsEmpty() )
	{
		delete m_lstSegStates.RemoveHead();
	}
	while( !m_lstTransSegStates.IsEmpty() )
	{
		m_lstTransSegStates.RemoveHead()->Release();
	}
	RELEASE( m_pIDMCurrentSegmentState );
	LeaveCriticalSection( &m_csSegmentState );

	if( m_pIDMTransitionSegment )
	{
		m_pIDMTransitionSegment->Release();
		m_pIDMTransitionSegment = NULL;
	}


	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	if( m_pIDMSegment8 )
	{
		IDirectMusicObjectP* pIDMObjectP;
		if( SUCCEEDED ( m_pIDMSegment8->QueryInterface( IID_IDirectMusicObjectP, (void**)&pIDMObjectP ) ) )
		{
			pIDMObjectP->Zombie();
			RELEASE( pIDMObjectP );
		}

		m_pIDMSegment8->Release();
		m_pIDMSegment8 = NULL;
	}

	if( m_pIDMSegment )
	{
		m_pIDMSegment->Release();
		m_pIDMSegment = NULL;
	}
	if ( m_pIAudioPathNode )
	{
		m_pIAudioPathNode->Release();
		m_pIAudioPathNode = NULL;
	}
	if ( m_pIContainerNode )
	{
		m_pIContainerNode->Release();
		m_pIContainerNode = NULL;
	}
	if ( m_pIToolGraphNode )
	{
		m_pIToolGraphNode->Release();
		m_pIToolGraphNode = NULL;
	}

	// m_PPGSegment will clean up after itself

	// Clean up the undo mgr.
	if(m_pUndoMgr)
	{
		delete m_pUndoMgr;
		m_pUndoMgr = NULL;
	}
	
	if( m_pComponent )
	{
		m_pComponent->Release();
		m_pComponent = NULL;
	}

	m_wndNotificationHandler.DestroyWindow();

	DeleteCriticalSection( &m_csSegmentState );
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::CreateEmptyContainer

HRESULT CSegment::CreateEmptyContainer( void )
{
	IDMUSProdNode* pIContainerNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Do not call if Segment already has a container!
	ASSERT( m_pIContainerNode == NULL );
	if( m_pIContainerNode )
	{
		return S_OK;
	}

	// Create the Container
	HRESULT hr = m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_ContainerNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_ContainerNode, &pIContainerNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_CONTAINER_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIContainerNode->SetNodeName( bstrName );

			InsertChildNode( pIContainerNode );
			pIContainerNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::CreateEmptyToolGraph

HRESULT CSegment::CreateEmptyToolGraph( void )
{
	IDMUSProdNode* pIToolGraphNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Do not call if Segment already has a tool graph!
	ASSERT( m_pIToolGraphNode == NULL );
	if( m_pIToolGraphNode )
	{
		return S_OK;
	}

	// Create the tool graph
	HRESULT hr = m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_ToolGraphNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_ToolGraphNode, &pIToolGraphNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_TOOLGRAPH_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIToolGraphNode->SetNodeName( bstrName );

			InsertChildNode( pIToolGraphNode );
			pIToolGraphNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::CreateEmptyAudioPath

HRESULT CSegment::CreateEmptyAudioPath( void )
{
	IDMUSProdNode* pIAudiopathNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Do not call if Segment already has a audio path!
	ASSERT( m_pIAudioPathNode == NULL );
	if( m_pIAudioPathNode )
	{
		return S_OK;
	}

	// Create the audio path
	HRESULT hr = m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_AudioPathNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_AudioPathNode, &pIAudiopathNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_AUDIOPATH_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIAudiopathNode->SetNodeName( bstrName );

			InsertChildNode( pIAudiopathNode );

			// Set the 'Use new instance of embedded audiopath by default' flag
			m_dwResolution |= DMUS_SEGF_USE_AUDIOPATH;
			m_PPGSegment.dwResolution = m_dwResolution;
			if(m_pIDMSegment != NULL)
			{
				m_pIDMSegment->SetDefaultResolution( m_dwResolution );
			}
			pIAudiopathNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}

HRESULT CSegment::Initialize()
{
	//HRESULT hr;

	// Initialize the iostructs

	// Create the undo manager
	m_pUndoMgr = new CSegmentUndoMan();
	if(m_pUndoMgr == NULL)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::SetModifiedFlag

void CSegment::SetModifiedFlag(BOOL fModified)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IUnknown implementation

HRESULT CSegment::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersist) )
    {
        AddRef();
        *ppvObj = (IPersist *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        AddRef();
        *ppvObj = (IPersistStream *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdTransport) )
    {
        AddRef();
        *ppvObj = (IDMUSProdTransport *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdNotifyCPt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNotifyCPt *)this;
        return S_OK;
    }

	if( ::IsEqualIID(riid, IID_IDMUSProdTimelineCallback) )
	{
		AddRef();
		*ppvObj = (IDMUSProdTimelineCallback *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink))
	{
		AddRef();
		*ppvObj = (IDMUSProdNotifySink*)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdConductorTempo))
	{
		AddRef();
		*ppvObj = (IDMUSProdConductorTempo*)this;
		return S_OK;
	}

 	if( ::IsEqualIID(riid, IID_IPrivateSegment))
	{
		AddRef();
		*ppvObj = (IPrivateSegment*)this;
		return S_OK;
	}

 	if( ::IsEqualIID(riid, IID_IDMUSProdSegmentEdit)
 	||  ::IsEqualIID(riid, IID_IDMUSProdSegmentEdit8))
	{
		AddRef();
		*ppvObj = (IDMUSProdSegmentEdit8*)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdSecondaryTransport))
	{
		AddRef();
		*ppvObj = (IDMUSProdSecondaryTransport*)this;
		return S_OK;
	}
	
	if(::IsEqualIID(riid, IID_IDMUSProdSortNode))
	{
		AddRef();
        *ppvObj = (IDMUSProdSortNode*) this;
        return S_OK;
	}

	if(::IsEqualIID(riid, IID_IDMUSProdGetReferencedNodes))
	{
		AddRef();
		*ppvObj = (IDMUSProdGetReferencedNodes*) this;
		return S_OK;
	}

	*ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CSegment::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CSegment::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
   --m_dwRef;

    if( m_dwRef == 0 )
    {
		TRACE( "SEGMENT: CSegment destroyed!\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNodeImageIndex

HRESULT CSegment::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return( m_pComponent->GetSegmentImageIndex(pnFirstImage) );
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetComponent

HRESULT CSegment::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetDocRootNode

HRESULT CSegment::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->AddRef();
		*ppIDocRootNode = m_pIDocRootNode;
		return S_OK;
	}

	*ppIDocRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::SetDocRootNode

HRESULT CSegment::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetParentNode

HRESULT CSegment::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(ppIParentNode == NULL)
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;
	return S_OK;	
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::SetParentNode

HRESULT CSegment::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;

	return S_OK;	
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNodeId

HRESULT CSegment::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_SegmentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNodeName

HRESULT CSegment::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pbstrName = m_PPGSegment.strSegmentName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNodeNameMaxLength

HRESULT CSegment::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::ValidateNodeName

HRESULT CSegment::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;

	// We accept any name the framework gives us which isn't too long
	strName = bstrName;
	::SysFreeString( bstrName );

	if(strName.GetLength() <= DMUS_MAX_NAME)
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::SetNodeName

HRESULT CSegment::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	CString strName = bstrName;
	::SysFreeString( bstrName );

	// The caller should have already validated the name!
	ASSERT(strName.GetLength() <= DMUS_MAX_NAME);

	if( strName != m_PPGSegment.strSegmentName )
	{
		m_PPGSegment.strSegmentName = strName;

		SetModifiedFlag( TRUE );

		OnNameChange();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetFirstChild

HRESULT CSegment::GetFirstChild( IDMUSProdNode **ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	if( m_pIContainerNode )
	{
		*ppIFirstChildNode = m_pIContainerNode;
		(*ppIFirstChildNode)->AddRef();
	}
	else if( m_pIAudioPathNode )
	{
		*ppIFirstChildNode = m_pIAudioPathNode;
		(*ppIFirstChildNode)->AddRef();
	}
	else if( m_pIToolGraphNode )
	{
		*ppIFirstChildNode = m_pIToolGraphNode;
		(*ppIFirstChildNode)->AddRef();
	}
	else
	{
		*ppIFirstChildNode = NULL;
	}

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNextChild

HRESULT CSegment::GetNextChild( IDMUSProdNode *pIChildNode, IDMUSProdNode **ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	*ppINextChildNode = NULL;

	if( pIChildNode == m_pIContainerNode )
	{
		if( m_pIAudioPathNode )
		{
			*ppINextChildNode = m_pIAudioPathNode;
		}
		else
		{
			*ppINextChildNode = m_pIToolGraphNode;
		}
	}
	else if( pIChildNode == m_pIAudioPathNode )
	{
		*ppINextChildNode = m_pIToolGraphNode;
	}

	if( *ppINextChildNode )
	{
		(*ppINextChildNode)->AddRef();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetNodeListInfo

HRESULT CSegment::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

    pListInfo->bstrName = m_PPGSegment.strSegmentName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_PPGSegment.guidSegment, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetEditorClsId

HRESULT CSegment::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pClsId = CLSID_SegmentDesigner;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetEditorTitle

HRESULT CSegment::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_SEGMENT_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_PPGSegment.strSegmentName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetEditorWindow

HRESULT CSegment::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::SetEditorWindow

HRESULT CSegment::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::UseOpenCloseImages

HRESULT CSegment::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetRightClickMenuId

HRESULT CSegment::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SEGMENT_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::OnRightClickMenuInit

HRESULT CSegment::OnRightClickMenuInit(HMENU hMenu)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::EnableMenuItem( hMenu, IDM_NEW_AUDIOPATH, m_pIAudioPathNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );
	::EnableMenuItem( hMenu, IDM_NEW_CONTAINER, m_pIContainerNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );
	::EnableMenuItem( hMenu, IDM_NEW_TOOLGRAPH, m_pIToolGraphNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::OnRightClickMenuSelect

HRESULT CSegment::OnRightClickMenuSelect(long lCommandId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	HRESULT hr = E_FAIL;

	switch(lCommandId)
	{
	case IDM_NEW_AUDIOPATH:
		hr = CreateEmptyAudioPath();
		if( m_pIAudioPathNode )
		{
			m_pComponent->m_pIFramework->OpenEditor( m_pIAudioPathNode );
		}
		break;

	case IDM_NEW_CONTAINER:
		hr = CreateEmptyContainer();
		break;

	case IDM_NEW_TOOLGRAPH:
		hr = CreateEmptyToolGraph();
		if( m_pIToolGraphNode )
		{
			m_pComponent->m_pIFramework->OpenEditor( m_pIToolGraphNode );
		}
		break;

	case IDM_RENAME:
		if( SUCCEEDED ( m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
		{
			hr = S_OK;
		}
		break;

	case ID_VIEW_PROPERTIES:
		{
			IDMUSProdPropSheet*	pIPropSheet;
			if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet) ) )
			{
				pIPropSheet->Show( TRUE );
				pIPropSheet->Release();
			}
		}

		OnShowProperties();
		hr = S_OK;
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::DeleteChildNode

HRESULT CSegment::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( m_fDeletingContainer )
	{
		// Nothing to do
		return S_OK;
	}

	CWaitCursor wait;

	if( pIChildNode == NULL 
	||  !(pIChildNode == m_pIContainerNode
		  || pIChildNode == m_pIAudioPathNode
		  || pIChildNode == m_pIToolGraphNode) )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( m_pComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Remove from Segment
	if( pIChildNode == m_pIContainerNode )
	{
		// Make sure that Containers cleanup properly.  Otherwise
		// Container that happens to be in the Framework's notification
		// list will never have its destructor called.
		if( m_pIContainerNode )
		{
			m_fDeletingContainer = true;
			m_pIContainerNode->DeleteNode( FALSE );
			m_fDeletingContainer = false;
		}
		RELEASE( m_pIContainerNode );
	}
	else if( pIChildNode == m_pIAudioPathNode )
	{
		RELEASE( m_pIAudioPathNode );

		// Clear the 'Use AudioPath' flag
		m_dwResolution &= ~DMUS_SEGF_USE_AUDIOPATH;
		m_PPGSegment.dwResolution = m_dwResolution;
		if(m_pIDMSegment != NULL)
		{
			m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		}

		// If available, refresh the property sheets
		if( m_pComponent && m_pComponent->m_pIPageManager)
		{
			m_pComponent->m_pIPageManager->RefreshData();
		}
	}
	else if( pIChildNode == m_pIToolGraphNode )
	{
		ASSERT( m_pIDMSegment != NULL );
		if( m_pIDMSegment )
		{
			m_pIDMSegment->SetGraph( NULL );
			RemoveSegmentFromGraphUserList();
		}
		RELEASE( m_pIToolGraphNode );
	}

	SetModifiedFlag( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::InsertChildNode

HRESULT CSegment::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	pIChildNode->AddRef();

	// Segments can only have one Container, Audio Path, or Tool Graph
	GUID guidNode;
	if( FAILED( pIChildNode->GetNodeId( &guidNode ) ) )
	{
		return E_INVALIDARG;
	}

	if( guidNode == GUID_ContainerNode )
	{
		if( m_pIContainerNode )
		{
			DeleteChildNode( m_pIContainerNode, FALSE );
		}

		// Set the Segment's Container
		ASSERT( m_pIContainerNode == NULL );
		m_pIContainerNode = pIChildNode;

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIContainerNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( m_pComponent->m_pIFramework->AddNode(m_pIContainerNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIContainerNode, FALSE );
			return E_FAIL;
		}
	}
	else if( guidNode == GUID_AudioPathNode )
	{
		const DWORD dwCurrentResolution = m_dwResolution;

		if( m_pIAudioPathNode )
		{
			DeleteChildNode( m_pIAudioPathNode, FALSE );
		}

		// Set the Segment's Audio Path
		ASSERT( m_pIAudioPathNode == NULL );
		m_pIAudioPathNode = pIChildNode;

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIAudioPathNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( m_pComponent->m_pIFramework->AddNode(m_pIAudioPathNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIAudioPathNode, FALSE );
			return E_FAIL;
		}

		if( m_dwResolution != dwCurrentResolution )
		{
			m_dwResolution = dwCurrentResolution;
			m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		}
	}
	else if( guidNode == GUID_ToolGraphNode )
	{
		if( m_pIToolGraphNode )
		{
			DeleteChildNode( m_pIToolGraphNode, FALSE );
		}

		// Set the Segment's Tool Graph
		ASSERT( m_pIDMSegment != NULL );
		ASSERT( m_pIToolGraphNode == NULL );
		m_pIToolGraphNode = pIChildNode;
		if( m_pIDMSegment )
		{
			IDirectMusicGraph* pIGraph;
			if( SUCCEEDED ( m_pIToolGraphNode->GetObject( CLSID_DirectMusicGraph, IID_IDirectMusicGraph, (void**)&pIGraph ) ) )
			{
				m_pIDMSegment->SetGraph( pIGraph );
				RELEASE( pIGraph );
			}

			AddSegmentToGraphUserList();
		}

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIToolGraphNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( m_pComponent->m_pIFramework->AddNode(m_pIToolGraphNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIToolGraphNode, FALSE );
			return E_FAIL;
		}
	}
	else
	{
		return E_INVALIDARG;
	}

	SetModifiedFlag( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::DeleteNode

HRESULT CSegment::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	// Remove from Project Tree
	if(S_FALSE == m_pComponent->m_pIFramework->RemoveNode((IDMUSProdNode *)this, fPromptUser))
	{
		return E_FAIL;
	}

	// Stop, unadvise ConnectionPoint and unregister the CSegment from the Transport.
	UnRegisterWithTransport();

	// Delete all tracks
	if ( !m_lstTracks.IsEmpty() )
	{
		CTrack	*pTrack;
		while( !m_lstTracks.IsEmpty() )
		{
			pTrack = m_lstTracks.RemoveHead() ;
			delete pTrack;
		}
	}

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	// Release the segment's container, audio path, and tool graph
	RELEASE( m_pIContainerNode );
	RELEASE( m_pIAudioPathNode );
	RELEASE( m_pIToolGraphNode );

	// Remove from Component Segment list
	m_pComponent->RemoveFromSegmentFileList( this );

	return S_OK;
}




/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::OnNodeSelChanged

HRESULT CSegment::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ( fSelected )
	{
		ASSERT( m_pComponent );
		ASSERT( m_pComponent->m_pIConductor );
		if( !m_pComponent || !m_pComponent->m_pIConductor )
		{
			return E_UNEXPECTED;
		}

		// Automatically updates tempo display
		m_pComponent->m_pIConductor->SetActiveTransport((IDMUSProdTransport *) this, GetButtonState() );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::GetObject

HRESULT CSegment::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DirectMusicSegment or CLSID_DirectMusicSegmentState object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicSegment ) )
	{
		if( m_pIDMSegment )
		{
			// Set the AudioPathConfig on the segment
			IDirectMusicSegment8P *pSegmentPrivate;
			if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **)&pSegmentPrivate ) ) )
			{
				// Try and get an AudioPathConfig object from the node
				IUnknown *pAudioPathConfig;
				if( (m_pIAudioPathNode == NULL)
				||	FAILED( m_pIAudioPathNode->GetObject( CLSID_DirectMusicAudioPathConfig, IID_IUnknown, (void**)&pAudioPathConfig ) ) )
				{
					pAudioPathConfig = NULL;
				}

				pSegmentPrivate->SetAudioPathConfig( pAudioPathConfig );
				pSegmentPrivate->Release();

				if( pAudioPathConfig )
				{
					pAudioPathConfig->Release();
				}
			}
			return m_pIDMSegment->QueryInterface( riid, ppvObject );
		}
	}
	else if( ::IsEqualCLSID( rclsid, GUID_AudioPathNode ) )
	{
		if( m_pIAudioPathNode )
		{
			return m_pIAudioPathNode->QueryInterface( riid, ppvObject );
		}
	}
	else if( ::IsEqualCLSID( rclsid, GUID_ToolGraphNode ) )
	{
		if( m_pIToolGraphNode )
		{
			return m_pIToolGraphNode->QueryInterface( riid, ppvObject );
		}
	}
	else if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicSegmentState ) )
	{
		HRESULT hr = E_FAIL;
		EnterCriticalSection( &m_csSegmentState );
		if( m_pIDMCurrentSegmentState )
		{
			hr = m_pIDMCurrentSegmentState->QueryInterface( riid, ppvObject );
		}
		LeaveCriticalSection( &m_csSegmentState );
		return hr;
	}
	else if( (::IsEqualCLSID( rclsid, GUID_TimeSignature ) && ::IsEqualIID( riid, GUID_TimeSignature )) ||
			 (::IsEqualCLSID( rclsid, GUID_ConductorCountInBeatOffset ) && ::IsEqualIID( riid, GUID_ConductorCountInBeatOffset )) )
	{
		ASSERT( ppvObject );
		if( !ppvObject )
		{
			return E_POINTER;
		}

		DMUS_TIMESIGNATURE TimeSig;

		// If not starting from the beginning, and displaying the Timeline, use the current cursor position
		MUSIC_TIME mtTime = 0;
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline &&
			(::IsEqualCLSID( rclsid, GUID_ConductorCountInBeatOffset ) ||
			 (::IsEqualCLSID( rclsid, GUID_TimeSignature ) &&
			  (((DMUS_TIMESIGNATURE *)ppvObject)->mtTime > 0) ) ) )
		{
			m_pSegmentDlg->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime);
		}

		// Look for a TimeSig in the segment
		if( m_pIDMSegment && SUCCEEDED( m_pIDMSegment->GetParam( GUID_TimeSignature, 0xFFFFFFFF, 0, mtTime, NULL, &TimeSig ) ) )
		{
			if( ::IsEqualCLSID( rclsid, GUID_TimeSignature ) )
			{
				DMUS_TIMESIGNATURE *pTimeSig = (DMUS_TIMESIGNATURE *)ppvObject;
				*pTimeSig = TimeSig;
			}
			else // GUID_ConductorCountInBeatOffset
			{
				if( mtTime )
				{
					long lMeasure, lBeat;
					ASSERT( m_pSegmentDlg->m_pTimeline );
					if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( 0xFFFFFFFF, 0, mtTime, &lMeasure, &lBeat ) ) &&
						SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( 0xFFFFFFFF, 0, lMeasure, lBeat, &lBeat ) ) )
					{
						MUSIC_TIME *pmtOffset = (MUSIC_TIME *)ppvObject;
						*pmtOffset = mtTime - lBeat;
					}
				}
			}
			return S_OK;
		}

		// Look for a TimeSig track in the segment
		//IDirectMusicTrack *pIDirectMusicTrack;
		if( m_pIDMSegment /*&& SUCCEEDED( m_pIDMSegment->GetTrack( CLSID_DirectMusicTimeSigTrack, 0xFFFFFFFF, 0, &pIDirectMusicTrack ) )*/ )
		{
			if( ::IsEqualCLSID( rclsid, GUID_TimeSignature ) )
			{
				DMUS_TIMESIGNATURE *pTimeSig = (DMUS_TIMESIGNATURE *)ppvObject;
				pTimeSig->bBeat = 4;
				pTimeSig->bBeatsPerMeasure = 4;
				pTimeSig->wGridsPerBeat = 2;
				pTimeSig->mtTime = 0;
			}
			else // GUID_ConductorCountInBeatOffset
			{
				if( mtTime )
				{
					MUSIC_TIME *pmtOffset = (MUSIC_TIME *)ppvObject;
					*pmtOffset = mtTime % DMUS_PPQ;
				}
			}
			//pIDirectMusicTrack->Release();
			return S_OK;
		}

		// Otherwise, fail
		return E_FAIL;
	}

	else if( ::IsEqualCLSID( rclsid, GUID_TempoParam ) && ::IsEqualIID( riid, GUID_TempoParam ) )
	{
		DMUS_TEMPO_PARAM *pTempo = (DMUS_TEMPO_PARAM *)ppvObject;
		ASSERT( pTempo );
		if( !pTempo )
		{
			return E_POINTER;
		}

		// If not starting from the beginning, and displaying the Timeline, use the current cursor position
		MUSIC_TIME mtTime = 0;
		if( (pTempo->mtTime > 0) && m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime);
		}

		// Look for a Tempo in the segment
		if( m_pIDMSegment && SUCCEEDED( m_pIDMSegment->GetParam( GUID_TempoParam, 0xFFFFFFFF, 0, mtTime, NULL, pTempo ) ) )
		{
			return S_OK;
		}

		// Otherwise, fail
		return E_FAIL;
		/*
		pTempo->mtTime = 0;
		pTempo->dblTempo = 120.0;

		return S_OK;
		*/
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////
// Drag / Drop Methods


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CreateDataObject

HRESULT CSegment::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Segment into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_SEGMENT into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfSegment, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_JAZZFILE format
		if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveClipFormat( m_pComponent->m_cfJazzFile, this, &pIStream ) ) )
		{
			// Style nodes represent files so we must also
			// place CF_JAZZFILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfJazzFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			pIStream->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanCut

HRESULT CSegment::CanCut( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CanDelete();
 
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanCopy

HRESULT CSegment::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanDelete

HRESULT CSegment::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanDeleteChildNode

HRESULT CSegment::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == m_pIAudioPathNode 
	||  pIChildNode == m_pIToolGraphNode 
	||  pIChildNode == m_pIContainerNode )
	{
		return S_OK;
	}

	return S_FALSE;	
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanPasteFromData

HRESULT CSegment::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	if( pDataObject->IsClipFormatAvailable(pIDataObject, m_pComponent->m_cfContainer) == S_OK 
	||  pDataObject->IsClipFormatAvailable(pIDataObject, m_pComponent->m_cfAudioPath) == S_OK 
	||  pDataObject->IsClipFormatAvailable(pIDataObject, m_pComponent->m_cfGraph) == S_OK )
	{
		RELEASE( pDataObject );
		return S_OK;
	}
	RELEASE( pDataObject );

	// Delegate to parent
	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			// Let Segment, for example, decide what can be dropped
			return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
		}
	}
	
	return S_FALSE; 	
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::PasteFromData

HRESULT CSegment::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfContainer ) ) )
	{
		// Handle CF_CONTAINER 
		hr = PasteCF_CONTAINER( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfAudioPath ) ) )
	{
		// Handle CF_AUDIOPATH 
		hr = PasteCF_AUDIOPATH( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfGraph ) ) )
	{
		// Handle CF_GRAPH 
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	
	if( hr == S_OK )
	{
		return hr;
	}

	// Let parent handle paste
	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::CanChildPasteFromData

HRESULT CSegment::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;
	pIChildNode->GetNodeId( &guidNodeId );

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfContainer ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ContainerNode ) )
		{
			hr = S_OK;
		}
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfAudioPath ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_AudioPathNode ) )
		{
			hr = S_OK;
		}
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfGraph ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ToolGraphNode ) )
		{
			hr = S_OK;
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNode::ChildPasteFromData

HRESULT CSegment::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfContainer ) ) )
	{
		// Handle CF_CONTAINER format
		hr = PasteCF_CONTAINER( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfAudioPath ) ) )
	{
		// Handle CF_AUDIOPATH format
		hr = PasteCF_AUDIOPATH( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfGraph ) ) )
	{
		// Handle CF_GRAPH format
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}




/////////////////////////////////////////////////////////////////////////////
// CSegment IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IPersist::GetClassID

HRESULT CSegment::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IPersistStream::IsDirty

HRESULT CSegment::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return (m_fModified) ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IPersistStream::Load

HRESULT CSegment::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}
	
	// Make sure the undo mgr exists
	ASSERT(m_pUndoMgr != NULL);
	if(m_pUndoMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If our window exists, disable drawing operations until Load() is completed
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		IOleWindow *pOleWindow;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->QueryInterface( IID_IOleWindow, (void **)&pOleWindow ) ) )
		{
			HWND hwnd;
			pOleWindow->GetWindow( &hwnd );
			::LockWindowUpdate( hwnd );
			pOleWindow->Release();
		}
	}

	// Save stream's current position
	LARGE_INTEGER li;
    ULARGE_INTEGER ul;

    li.HighPart = 0;
    li.LowPart = 0;

    HRESULT hr = pIStream->Seek(li, STREAM_SEEK_CUR, &ul);
    
	if(FAILED(hr))
	{
		return E_FAIL;	
    }
	
	// Clean up ourselves if we're doing an Undo/Redo.
	CleanUp();

	DWORD dwSavedPos = ul.LowPart;
    
	// Read first 4 bytes to determine what type of stream we
	// have been passed

	BOOL fFoundFormat = FALSE;
	FOURCC type;
	DWORD dwRead;
	hr = pIStream->Read(&type, sizeof(FOURCC), &dwRead);
	
	if(SUCCEEDED(hr) && dwRead == sizeof(FOURCC))
	{
		// Check for a RIFF file
		if( type == mmioFOURCC( 'R', 'I', 'F', 'F' ) )
		{
			// Check to see what type of RIFF file we have
			li.HighPart = 0;
			li.LowPart = 4; // Length needed to seek to form type of RIFF chunk

			hr = pIStream->Seek(li, STREAM_SEEK_CUR, &ul);
			if(SUCCEEDED(hr))
			{
				hr = pIStream->Read(&type, sizeof(FOURCC), &dwRead);
			}

			if(SUCCEEDED(hr) && dwRead == sizeof(FOURCC))
			{
				// Check to see if we have a Segment file
				if( type == DMUS_FOURCC_SEGMENT_FORM )
				{
					// Since we now know what type of stream we need to seek back to saved position
					li.HighPart = 0;
					li.LowPart = dwSavedPos;
					hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

					if ( SUCCEEDED(hr) )
					{
						hr = DM_LoadSegment( pIStream );
						fFoundFormat = TRUE;
					}
				}
				// Check to see if we have a Section file
				else if ( type == FOURCC_SECTION_FORM )
				{
					// Since we now know what type of stream we need to seek back to saved position
					li.HighPart = 0;
					li.LowPart = dwSavedPos;
					hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

					if ( SUCCEEDED(hr) )
					{
						hr = IMA25_LoadRIFFSection( pIStream );
						fFoundFormat = TRUE;
					}
				}
				// Check to see if we have a MIDI file
				else
				{
					li.HighPart = 0;
					li.LowPart = 8;  // Length needed to seek to start of normal MIDI file
									 // contained within the Riff chunck

					FOURCC type;	
					DWORD dwRead=0;

					memset(&type, 0, sizeof(FOURCC));

					hr = pIStream->Seek(li, STREAM_SEEK_CUR, &ul);
					
					if(SUCCEEDED(hr))
					{
						hr = pIStream->Read(&type, sizeof(FOURCC), &dwRead);
					}

					if(SUCCEEDED(hr) && dwRead == sizeof(FOURCC))
					{
						if(type == mmioFOURCC( 'M', 'T', 'h', 'd' ))
						{
							// Since we now know what type of stream we need to seek back to saved position
							li.HighPart = 0;
							li.LowPart = dwSavedPos;
							hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

							if ( SUCCEEDED(hr) )
							{
								hr = ReadRIFFMIDIFile( pIStream );
								fFoundFormat = TRUE;
							}
						}
						else
						{
							// Not a MIDI file or section or segment; unsupported
							hr = E_INVALIDARG;
						}
					
					}
					else
					{
						hr = E_FAIL;
					}
				}
			}
		}
		// Check for a template file
		else if(type == mmioFOURCC('L', 'P', 'T', 's'))
		{
			// Since we now know what type of stream we need to seek back to saved position
			li.HighPart = 0;
			li.LowPart = dwSavedPos;
			hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

			if ( SUCCEEDED(hr) )
			{
				hr = IMA25_LoadTemplate( pIStream );
				fFoundFormat = TRUE;
			}
		}
		// Check for section file
		else if( type == mmioFOURCC( 'N', 'T', 'C', 'S' ) ||
			type == mmioFOURCC( 'C', 'E', 'S', 'S' ) )
		{
			// Since we now know what type of stream we need to seek back to saved position
			li.HighPart = 0;
			li.LowPart = dwSavedPos;
			hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

			if ( SUCCEEDED(hr) )
			{
				hr = IMA25_LoadSection( pIStream );
				fFoundFormat = TRUE;
			}
		}
		// Check for normal MIDI file
		else if(type == mmioFOURCC('M', 'T', 'h', 'd'))
	    {
			// Since we now know what type of stream we need to seek back to saved position
			li.HighPart = 0;
			li.LowPart = dwSavedPos;
			hr = pIStream->Seek(li, STREAM_SEEK_SET, &ul);

			if ( SUCCEEDED(hr) )
			{
				hr = ReadMIDIFile( pIStream );
				fFoundFormat = TRUE;
			}
		}
		else
		{
			// Not a MIDI file or section or template; unsupported
			hr = E_INVALIDARG;
		}
	}

	if( fFoundFormat )
	{
		if( m_pIDocRootNode )
		{
			/*
			// Already in Project Tree so sync changes
			// Refresh Style editor (when open)
			SyncStyleEditor( SSE_ALL );

			// Sync change with property sheet
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
			pIPropSheet->RefreshTitle();
			pIPropSheet->RefreshActivePage();
			RELEASE( pIPropSheet );
			}
			*/
		}
	}

	/*
	// Check for "Undo" format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DM_FOURCC_STYLE_UNDO_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = UNDO_LoadStyle( pIRiffStream, &ckMain );
				fFoundFormat = TRUE;
			}

			if( fFoundFormat )
			{
				if( m_pIDocRootNode )
				{
					// Already in Project Tree so sync changes
					// Refresh Style editor (when open)
					SyncStyleEditor( SSE_STYLE );

					// Sync change with property sheet
					IDMUSProdPropSheet* pIPropSheet;
					if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
					{
						pIPropSheet->RefreshTitleByObject( this );
						pIPropSheet->RefreshActivePageByObject( this );
						RELEASE( pIPropSheet );
					}
				}
			}

			RELEASE( pIRiffStream );
		}
	}
	*/

	//	Update the Conductor if we've succeeded
	if( SUCCEEDED( hr ) )
	{
		// Update the disabled/enabled state of the tempo button
		UpdateConductorTempo();

		/* Don't create nodes by default
		if( m_pIContainerNode == NULL )
		{
			CreateEmptyContainer();
		}

		if( m_pIAudioPathNode == NULL )
		{
			CreateEmptyAudioPath();
		}

		if( m_pIToolGraphNode == NULL )
		{
			CreateEmptyToolGraph();
		}
		*/
	}

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		// Update the horizontal zoom, horizontal scroll, and vertical scroll
		m_pSegmentDlg->UpdateZoomAndScroll();

		// Notify the StripMgrs that all tracks have been added, so they can now
		// safely initialize.
		m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_AllTracksAdded, 0xFFFFFFFF, NULL );

		// Now, redraw the dialog
		::LockWindowUpdate( NULL );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::FindStyle

IDMUSProdNode* CSegment::FindStyle( CString strStyleName, IStream* pIStream )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( m_pComponent->m_pIFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for Styles
	hr = m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get the target directory
	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
		pPersistInfo->Release();
	}

	// See if there is a Style named 'strStyleName' in this Project
	if( !strStyleName.IsEmpty() )
	{
		BSTR bstrStyleName = strStyleName.AllocSysString();

		if( FAILED ( m_pComponent->m_pIFramework->GetBestGuessDocRootNode( pIDocType,
										 								   bstrStyleName,
																		   pITargetDirectoryNode,
																		   &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

ON_ERROR:
	if( pIDocType )
	{
		pIDocType->Release();
	}

	return pIDocRootNode;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::FindPersonality

IDMUSProdNode* CSegment::FindPersonality( CString strPersName, IStream* pIStream )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( m_pComponent->m_pIFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for personality
	hr = m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_PersonalityNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get the target directory
	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
		pPersistInfo->Release();
	}

	// See if there is a FindPersonality named 'strPersName' in this Project
	if( !strPersName.IsEmpty() )
	{
		BSTR bstrPersName = strPersName.AllocSysString();

		if( FAILED ( m_pComponent->m_pIFramework->GetBestGuessDocRootNode( pIDocType,
										 								   bstrPersName,
																		   pITargetDirectoryNode,
																		   &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

ON_ERROR:
	if( pIDocType )
	{
		pIDocType->Release();
	}

	return pIDocRootNode;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_LoadSegment

HRESULT CSegment::DM_LoadSegment( IStream* pIStream )
{
	CString strStyleName;
	CString strPersName;
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	BOOL fSegmentLengthChanged = FALSE;

	MMCKINFO	ckMain;
	MMCKINFO	ckList;
	MMCKINFO	ck;
	DWORD		dwByteCount;
	DWORD		dwSize;
	ckMain.fccType = DMUS_FOURCC_SEGMENT_FORM;

	if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) != 0 )
	{
		hr = E_FAIL;
	}
	else
	{
		// Save the current position
		DWORD dwPos = StreamTell( pIStream );

		// Load the Segment
		while( pIRiffStream->Descend( &ck, &ckMain, 0 ) == 0 )
		{
			switch( ck.ckid )
			{
			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_AUDIOPATH_FORM:
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = m_pComponent->m_pIAudioPathComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						if( m_pIAudioPathNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIAudioPathNode = pINode;
						}
						break;
					}

					case DMUS_FOURCC_TOOLGRAPH_FORM:
					if( m_pComponent->m_pIToolGraphComponent )
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = m_pComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						if( m_pIToolGraphNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIToolGraphNode = pINode;
							ASSERT( m_pIDMSegment != NULL );
							if( m_pIDMSegment )
							{
								IDirectMusicGraph* pIGraph;
								if( SUCCEEDED ( m_pIToolGraphNode->GetObject( CLSID_DirectMusicGraph, IID_IDirectMusicGraph, (void**)&pIGraph ) ) )
								{
									m_pIDMSegment->SetGraph( pIGraph );
									RELEASE( pIGraph );
								}

								AddSegmentToGraphUserList();
							}
						}
						break;
					}

					case DMUS_FOURCC_CONTAINER_FORM:
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = m_pComponent->m_pIContainerComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						if( m_pIContainerNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIContainerNode = pINode;
						}
						break;
					}
				}
				break;

				case DMUS_FOURCC_SEGMENT_CHUNK:
				{
					DMUS_IO_SEGMENT_HEADER_EX iDMSegment;
					ZeroMemory( &iDMSegment, sizeof( DMUS_IO_SEGMENT_HEADER_EX ) );

					dwSize = min( ck.cksize, sizeof( DMUS_IO_SEGMENT_HEADER_EX ) );
					hr = pIStream->Read( &iDMSegment, dwSize, &dwByteCount );
					if( FAILED( hr )
						||  dwByteCount != dwSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					m_dwLoopRepeats = iDMSegment.dwRepeats;
					if( m_mtLength != iDMSegment.mtLength )
					{
						fSegmentLengthChanged = TRUE;
					}
					m_mtLength = iDMSegment.mtLength;
					m_mtPlayStart = iDMSegment.mtPlayStart;
					m_mtLoopStart = iDMSegment.mtLoopStart;
					m_mtLoopEnd = iDMSegment.mtLoopEnd;
					m_dwResolution = iDMSegment.dwResolution;
					m_dwSegmentFlags = iDMSegment.dwFlags;
					m_rtLength = iDMSegment.rtLength;
                    m_rtLoopStart = iDMSegment.rtLoopStart;
                    m_rtLoopEnd = iDMSegment.rtLoopEnd;
					if( m_pIDMSegment )
					{
						m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
						m_pIDMSegment->SetDefaultResolution( m_dwResolution );
						m_pIDMSegment->SetLength( m_mtLength );
						m_pIDMSegment->SetStartPoint( m_mtPlayStart );
						m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );

					}
					SetSegmentHeaderChunk();
					if( m_pSegmentDlg )
					{
						m_pSegmentDlg->SetTimelineLength( m_mtLength );
					}
					break;
				}

				case DMUS_FOURCC_VERSION_CHUNK:
				{
					DMUS_IO_VERSION dmusSegmentVersionIO;
					ZeroMemory( &dmusSegmentVersionIO, sizeof( DMUS_IO_VERSION ) );

					dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
					hr = pIStream->Read( &dmusSegmentVersionIO, dwSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					m_PPGSegment.wVersion1 = WORD((dmusSegmentVersionIO.dwVersionMS & 0xFFFF0000) >> 16);
					m_PPGSegment.wVersion2 = WORD(dmusSegmentVersionIO.dwVersionMS & 0xFFFF);
					m_PPGSegment.wVersion3 = WORD((dmusSegmentVersionIO.dwVersionLS & 0xFFFF0000) >> 16);
					m_PPGSegment.wVersion4 = WORD(dmusSegmentVersionIO.dwVersionLS & 0xFFFF);
					break;
				}

				case DMUS_FOURCC_GUID_CHUNK:
					if( ck.cksize == sizeof(GUID) )
					{
						hr = pIStream->Read( &m_PPGSegment.guidSegment, sizeof(GUID), &dwByteCount );
						if( FAILED( hr ) || dwByteCount != sizeof(GUID ) )
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
					}
					else
					{
						TRACE("Segment: GUID chunk size (%d) != sizeof(GUID) (%d)\n", ck.cksize, sizeof(GUID) );
					}
					break;

				case FOURCC_LIST:
					switch( ck.fccType )
					{
						case DMUS_FOURCC_INFO_LIST:
						case DMUS_FOURCC_UNFO_LIST:
							while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
							{
								switch( ckList.ckid )
								{
									case DMUS_FOURCC_UNAM_CHUNK:
									case RIFFINFO_INAM:
										ReadMBSfromWCS( pIStream, ckList.cksize, &m_PPGSegment.strSegmentName );
										break;

									case RIFFINFO_IART:
									case DMUS_FOURCC_UART_CHUNK:
										ReadMBSfromWCS( pIStream, ckList.cksize, &m_PPGSegment.strAuthor );
										break;

									case RIFFINFO_ICOP:
									case DMUS_FOURCC_UCOP_CHUNK:
										ReadMBSfromWCS( pIStream, ckList.cksize, &m_PPGSegment.strCopyright );
										break;

									case RIFFINFO_ISBJ:
									case DMUS_FOURCC_USBJ_CHUNK:
										ReadMBSfromWCS( pIStream, ckList.cksize, &m_PPGSegment.strSubject );
										break;

									case RIFFINFO_ICMT:
									case DMUS_FOURCC_UCMT_CHUNK:
										ReadMBSfromWCS( pIStream, ckList.cksize, &m_PPGSegment.strInfo );
										break;
								}
								pIRiffStream->Ascend( &ckList, 0 );
							}
							break;
						case DMUS_FOURCC_TRACK_LIST:
							ckList.fccType = DMUS_FOURCC_TRACK_FORM;
							while( pIRiffStream->Descend( &ckList, &ck, MMIO_FINDRIFF ) == 0 )
							{
								DM_AddTrack( pIRiffStream, ckList.cksize - 4 );
								// Assume the load operation succeeded
								hr = S_OK;
						        pIRiffStream->Ascend( &ckList, 0 );
							}
							break;
						case DMUS_FOURCC_SEGMENT_DESIGN_LIST:
							while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
							{
								switch( ckList.ckid )
								{
								case DMUS_FOURCC_SEGMENT_DESIGN_CHUNK:
									{
										DMUS_IO_SEGMENT_DESIGN iDMSegmentDesign;
										ZeroMemory( &iDMSegmentDesign, sizeof( DMUS_IO_SEGMENT_DESIGN ) );

										// Init quantize fields for backward compatability
										iDMSegmentDesign.wQuantizeTarget = QUANTIZE_TARGET_SELECTED;
										iDMSegmentDesign.bQuantizeResolution = 4;	
										iDMSegmentDesign.bQuantizeStrength = 100;
										iDMSegmentDesign.dwQuantizeFlags = SEQUENCE_QUANTIZE_START_TIME;		

										// Init edit velocity fields for backward compatability
										iDMSegmentDesign.wVelocityTarget = VELOCITY_TARGET_SELECTED;
										iDMSegmentDesign.bCompressMin = 0;
										iDMSegmentDesign.bCompressMax = 127;
										iDMSegmentDesign.lAbsoluteChangeStart = 0;
										iDMSegmentDesign.lAbsoluteChangeEnd = 0;
										iDMSegmentDesign.dwVelocityMethod = SEQUENCE_VELOCITIZE_PERCENT;

										// Init snap-to field for backward compatability
										iDMSegmentDesign.dwSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;

										dwSize = min( ckList.cksize, sizeof( DMUS_IO_SEGMENT_DESIGN ) );
										hr = pIStream->Read( &iDMSegmentDesign, dwSize, &dwByteCount );
										if( FAILED( hr ) || dwByteCount != dwSize )
										{
											hr = E_FAIL;
											goto ON_ERROR;
										}
										//m_dblTempo		= iDMSegmentDesign.dblTempo;
										//m_fTempoModifier	= iDMSegmentDesign.fTempoModifier;
										// Only load zoom setting if we've never set it before
										if( m_dblZoom == 0.0 )
										{
											m_dblZoom = iDMSegmentDesign.dblZoom;
										}
										m_lVerticalScroll	= (signed)iDMSegmentDesign.dwVerticalScroll;
										m_dblHorizontalScroll = iDMSegmentDesign.dblHorizontalScroll;
										m_tlSnapTo			= (DMUSPROD_TIMELINE_SNAP_TO)iDMSegmentDesign.dwSnapTo;
										if( m_lFunctionbarWidth == 0 )
										{
											m_lFunctionbarWidth = iDMSegmentDesign.lFunctionbarWidth;
										}
										m_SequenceQuantizeParms.m_wQuantizeTarget = iDMSegmentDesign.wQuantizeTarget;
										m_SequenceQuantizeParms.m_bResolution = iDMSegmentDesign.bQuantizeResolution;	
										m_SequenceQuantizeParms.m_bStrength = iDMSegmentDesign.bQuantizeStrength;
										m_SequenceQuantizeParms.m_dwFlags = iDMSegmentDesign.dwQuantizeFlags;		
										m_SequenceVelocitizeParams.m_wVelocityTarget = iDMSegmentDesign.wVelocityTarget;
										m_SequenceVelocitizeParams.m_bCompressMin = iDMSegmentDesign.bCompressMin;
										m_SequenceVelocitizeParams.m_bCompressMax = iDMSegmentDesign.bCompressMax;
										m_SequenceVelocitizeParams.m_lAbsoluteChangeStart = iDMSegmentDesign.lAbsoluteChangeStart;
										m_SequenceVelocitizeParams.m_dwVelocityMethod = iDMSegmentDesign.dwVelocityMethod;
										if( dwByteCount <= DWORD(&(iDMSegmentDesign.lAbsoluteChangeEnd)) - DWORD(&iDMSegmentDesign) )
										{
											// Just copy the start value
											m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd = iDMSegmentDesign.lAbsoluteChangeStart;
										}
										else
										{
											m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd = iDMSegmentDesign.lAbsoluteChangeEnd;
										}
										m_PPGSegment.Import( &iDMSegmentDesign );
										m_wLegacyActivityLevel = iDMSegmentDesign.wReserved;
										m_fLoadedLegacyActivityLevel = true;
									}
									break;
								case DMUS_FOURCC_SEGMENT_DESIGN_STYLE_NAME:
									// Store Style name
									ReadMBSfromWCS( pIStream, ckList.cksize, &strStyleName );
									break;
								case DMUS_FOURCC_SEGMENT_DESIGN_STYLE_REF:
									{
										// Initialize the CStyleRef
										CStyleRef StyleRef;
										StyleRef.m_pIFramework = m_pComponent->m_pIFramework;

										// Load the ref chunk
										hr = StyleRef.Load( pIStream );
										if( FAILED ( hr ) )
										{
											if( strStyleName.IsEmpty() == FALSE )
											{
												StyleRef.m_pINode = FindStyle( strStyleName, pIStream );
												if( StyleRef.m_pINode )
												{
													hr = S_OK;
												}
											}
										}
										if( SUCCEEDED( hr ) && StyleRef.m_pINode )
										{
											AddAuditionStyleRefTrack( StyleRef.m_pINode );
										}
									}
									break;
								case DMUS_FOURCC_SEGMENT_DESIGN_PERS_NAME:
									// Store Personality name
									ReadMBSfromWCS( pIStream, ckList.cksize, &strPersName );
									break;
								case DMUS_FOURCC_SEGMENT_DESIGN_PERS_REF:
									{
										// Initialize the CStyleRef
										CStyleRef StyleRef;
										StyleRef.m_pIFramework = m_pComponent->m_pIFramework;

										// Load the ref chunk
										hr = StyleRef.Load( pIStream );
										if( FAILED ( hr ) )
										{
											if( strPersName.IsEmpty() == FALSE )
											{
												StyleRef.m_pINode = FindPersonality( strPersName, pIStream );
												if( StyleRef.m_pINode )
												{
													hr = S_OK;
												}
											}
										}
										if( SUCCEEDED( hr ) && StyleRef.m_pINode )
										{
											AddAuditionPersRefTrack( StyleRef.m_pINode );
										}
									}
									break;
								}
								pIRiffStream->Ascend( &ckList, 0 );
							}
							break;
					}
					break;
			}
			pIRiffStream->Ascend( &ck, 0 );
		    dwPos = StreamTell( pIStream );
		}
		pIRiffStream->Ascend( &ckMain, 0 );
	}

	// Update the Conductor with the new Tempo state
	UpdateConductorTempo();

	// If available, update the Dialog
	if( m_pSegmentDlg )
	{
		m_pSegmentDlg->UpdateZoomAndScroll();

		if( fSegmentLengthChanged )
		{
			if( m_pSegmentDlg->m_pTimeline )
			{
				// notify all groups
				m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Length_Change, 0xFFFFFFFF, NULL );
			}
		}
	}

	// If available, refresh the property sheets
	if( m_pComponent && m_pComponent->m_pIPageManager)
	{
		m_pComponent->m_pIPageManager->RefreshData();
	}

	// Our name may have changed, so update the project tree and property page titles
	OnNameChange();

ON_ERROR:
	pIRiffStream->Release();;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_AddTrack
HRESULT CSegment::DM_AddTrack( IDMUSProdRIFFStream* pIRiffStream, DWORD dwChunkSize )
{
    IStream* pIStream = NULL;
	CTrack* pTrack = NULL;
	IStream *pStreamCopy = NULL;

    HRESULT hr;
    MMCKINFO ckTrackHeader, ckMain, ckTrackExtras, ckProducerOnly;

	ASSERT( pIRiffStream != NULL );
	if ( pIRiffStream == NULL )
	{
		return E_INVALIDARG;
	}

	pTrack = new CTrack;
	ASSERT( pTrack != NULL );
	if ( pTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	ckMain.dwDataOffset = StreamTell( pIStream );
	ckMain.cksize = dwChunkSize;

	DWORD		dwByteCount;
	DWORD		dwSize;

	// Load the Track
	ckTrackHeader.ckid = DMUS_FOURCC_TRACK_CHUNK;
	if( pIRiffStream->Descend( &ckTrackHeader, &ckMain, MMIO_FINDCHUNK) != 0 )
	{
		hr = E_FAIL;
		TRACE("CSegment::DM_AddTrack: Unable to find track header chunk.\n");
		goto ON_ERROR;
	}

	DMUS_IO_TRACK_HEADER iDMTrack;

	ZeroMemory( &iDMTrack, sizeof( DMUS_IO_TRACK_HEADER ) );
	dwSize = min( ckTrackHeader.cksize, sizeof( DMUS_IO_TRACK_HEADER ) );
	hr = pIStream->Read( &iDMTrack, dwSize, &dwByteCount );
	if( FAILED( hr )
		||  dwByteCount != dwSize )
	{
		hr = E_FAIL;
		TRACE("CSegment::DM_AddTrack: Unable to read track header chunk.\n");
		goto ON_ERROR;
	}

	if( ::IsEqualGUID( iDMTrack.guidClassID, CLSID_DX7_LyricTrack ) )
	{
		// Lyric track - Replace DX7 CLSID with DirectMusic DX8 CLSID
		pTrack->m_guidClassID = CLSID_DirectMusicLyricsTrack;
	}
	else
	{
		memcpy( &pTrack->m_guidClassID, &iDMTrack.guidClassID, sizeof(GUID) );
	}
	pTrack->m_dwGroupBits = iDMTrack.dwGroup;
	pTrack->m_ckid = iDMTrack.ckid;
	pTrack->m_fccType = iDMTrack.fccType;
	pTrack->m_dwGroupBits = iDMTrack.dwGroup;
	pTrack->m_dwPosition = iDMTrack.dwPosition;
	// This is handled by AddTrack( pTrack ) below.
//	hr = GUIDToStripMgr( iDMTrack.guidClassID, &pTrack->m_guidEditorID );
	pIRiffStream->Ascend( &ckTrackHeader, 0 );

	// Save our current position
	DWORD dwPosition;
	dwPosition = StreamTell( pIStream );

	// Search for a TRACK_EXTRAS chunk
	ckTrackExtras.ckid = DMUS_FOURCC_TRACK_EXTRAS_CHUNK;
	if( pIRiffStream->Descend( &ckTrackExtras, &ckMain, MMIO_FINDCHUNK) == 0 )
	{
		// Found the track_extras chunk - now load it
		DMUS_IO_TRACK_EXTRAS_HEADER iTrackExtrasHeader;
		ZeroMemory( &iTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );

		// Only read in as much as we can
		dwSize = min( ckTrackExtras.cksize, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
		hr = pIStream->Read( &iTrackExtrasHeader, dwSize, &dwByteCount );

		// Check if the read failed
		if( FAILED( hr ) ||  dwByteCount != dwSize )
		{
			hr = E_FAIL;
			TRACE("CSegment::DM_AddTrack: Error reading track extras header chunk.\n");
			goto ON_ERROR;
		}

		// Copy the information
		pTrack->m_dwTrackExtrasFlags = iTrackExtrasHeader.dwFlags;
		pTrack->m_dwTrackExtrasPriority = iTrackExtrasHeader.dwPriority;

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ckTrackExtras, 0 );

		// Save the current position
		dwPosition = StreamTell( pIStream );
	}
	else
	{
		// Didn't find the track_extras chunk - reset our position to just after the track header chunk
		StreamSeek( pIStream, dwPosition, STREAM_SEEK_SET );
		ckTrackExtras.cksize = 0;
	}

	// Search for a PRODUCER_ONLY chunk
	ckProducerOnly.ckid = DMUS_FOURCC_SEGMENT_DESIGN_TRACK_PRODUCER_ONLY;
	if( pIRiffStream->Descend( &ckProducerOnly, &ckMain, MMIO_FINDCHUNK) == 0 )
	{
		// Found the PRODUCER_ONLY chunk - now load it
		IOProducerOnlyChunk iProducerOnly;
		ZeroMemory( &iProducerOnly, sizeof( IOProducerOnlyChunk ) );

		// Only read in as much as we can
		dwSize = min( ckProducerOnly.cksize, sizeof( IOProducerOnlyChunk ) );
		hr = pIStream->Read( &iProducerOnly, dwSize, &dwByteCount );

		// Check if the read failed
		if( FAILED( hr ) ||  dwByteCount != dwSize )
		{
			hr = E_FAIL;
			TRACE("CSegment::DM_AddTrack: Error reading producer only chunk.\n");
			goto ON_ERROR;
		}

		// Copy the information
		pTrack->m_dwProducerOnlyFlags = iProducerOnly.dwProducerOnlyFlags;

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ckProducerOnly, 0 );
	}
	else
	{
		// Didn't find the PRODUCER_ONLY chunk - reset our position to just after the track header or
		// track extras chunk
		StreamSeek( pIStream, dwPosition, STREAM_SEEK_SET );
		ckProducerOnly.cksize = 0;
	}

	// Copy the Track's data into pTrack->m_pIStream
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	hr = m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
	if( FAILED( hr ) )
	{
		TRACE("Segment: Unable to alloc memory stream\n");
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save the current position
	dwPosition = StreamTell( pIStream );

	ULARGE_INTEGER	uliSize;
	// Calculate how far it is from the current position to the end of the chunk
	uliSize.QuadPart = ckMain.dwDataOffset + ckMain.cksize - dwPosition;

	hr = pIStream->CopyTo( pStreamCopy, uliSize, NULL, NULL );

	pTrack->SetStream( pStreamCopy );

	if( ::IsEqualGUID( pTrack->m_guidClassID, CLSID_DirectMusicSeqTrack ) )
	{
		hr = AddSequenceTrack( pTrack );
	}
	else
	{
		hr = AddTrack( pTrack );
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}
	if( pStreamCopy )
	{
		pStreamCopy->Release();
	}

	if ( FAILED( hr ) )
	{
		delete pTrack;
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::AddTrack

HRESULT	CSegment::AddTrack( CTrack *pTrack )
{
	if( pTrack == NULL )
	{
		return E_INVALIDARG;
	}

    IStream* pIStream = NULL;
	IDirectMusicTrack *pIDMTrack = NULL;
	IDMUSProdStripMgr *pIStripMgr = NULL;
	IPersistStream* pIPersistStreamStrip = NULL;
	HRESULT hr;

	if( FAILED( GUIDToStripMgr( pTrack->m_guidClassID, &pTrack->m_guidEditorID ) ) )
	{
		TRACE("Segment::AddTrack: Unable to find Strip Editor for Track's CLSID.\n");
		ASSERT(FALSE); // This shouldn't happen, since GUIDToStripMgr should default
		// to the UnknownStripMgr if it can't find a match in the registry.
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = ::CoCreateInstance( pTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
							 IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
	if( FAILED( hr ) )
	{
		TRACE("Segment: Unable to CoCreate an IDMUSProdStripMgr - going to try the UnknownStripMgr\n");
		memcpy( &pTrack->m_guidEditorID, &CLSID_UnknownStripMgr, sizeof(GUID) );
		hr = ::CoCreateInstance( pTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
								 IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
		if( FAILED( hr ) )
		{
			TRACE("Segment: Unable to CoCreate an UnknownStripMgr\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	pTrack->SetStripMgr( pIStripMgr );

	// Try and set the groupbits for this new StripMgr
	DMUS_IO_TRACK_HEADER ioTrackHeader;
	memcpy( &ioTrackHeader.guidClassID, &pTrack->m_guidClassID, sizeof(GUID) );
	ioTrackHeader.dwPosition = pTrack->m_dwPosition;
	ioTrackHeader.dwGroup = pTrack->m_dwGroupBits;
	ioTrackHeader.ckid = pTrack->m_ckid;
	ioTrackHeader.fccType = pTrack->m_fccType;
	VARIANT varTrackHeader;
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioTrackHeader;

	hr = pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKHEADER, varTrackHeader );
	if( FAILED( hr ) )
	{
		//TRACE("Segment: Failed to set StripMgr's GroupBits\n");
		//TRACE("Segment: Failed to set StripMgr's FourCCIDs\n");
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now, try and get the groupbits for this StripMgr (in case the StripMgr
	// belongs to all groups, or can only be on group 1, etc.)
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioTrackHeader;
	hr = pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader );
	if( SUCCEEDED( hr ) )
	{
		pTrack->m_dwGroupBits = ioTrackHeader.dwGroup;
		pTrack->m_ckid = ioTrackHeader.ckid;
		pTrack->m_fccType = ioTrackHeader.fccType;
	}

	// Try and set the extras flags for this new StripMgr
	DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
	ioTrackExtrasHeader.dwFlags = pTrack->m_dwTrackExtrasFlags;
	ioTrackExtrasHeader.dwPriority = pTrack->m_dwTrackExtrasPriority;
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;

	hr = pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, varTrackHeader );
	if( FAILED( hr ) )
	{
		// Not a big deal - this is an 'Extra' chunk. after all.
		//TRACE("Segment: Failed to set StripMgr's TrackExtras flags\n");
		hr = S_OK;
	}

	// Now, try and get the extras flags for this StripMgr (in case the StripMgr
	// has a hardcoded priority value, or some default Extras flags)
	ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
	if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader ) ) )
	{
		if( pTrack->m_dwTrackExtrasPriority != ioTrackExtrasHeader.dwPriority )
		{
			pTrack->m_dwTrackExtrasPriority = ioTrackExtrasHeader.dwPriority;

			// Mark the segment as 'dirty'
			SetModifiedFlag( TRUE );
		}

		if( pTrack->m_dwTrackExtrasFlags != ioTrackExtrasHeader.dwFlags )
		{
			pTrack->m_dwTrackExtrasFlags = ioTrackExtrasHeader.dwFlags;

			// Mark the segment as 'dirty'
			SetModifiedFlag( TRUE );
		}
	}

	VARIANT varFramework;
	varFramework.vt = VT_UNKNOWN;
	V_UNKNOWN( &varFramework ) = m_pComponent->m_pIFramework;
	hr = pIStripMgr->SetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, varFramework );
	if( FAILED( hr ) )
	{
		//TRACE("Segment: Failed to set StripMgr's Framework pointer\n");
		hr = S_OK;
	}

	if( pIStripMgr->IsParamSupported( GUID_DocRootNode ) == S_OK )
	{
		hr = pIStripMgr->SetParam( GUID_DocRootNode, 0, (IDMUSProdNode *)this );
		if( FAILED( hr ) )
		{
			//TRACE("Segment: Failed to set StripMgr's Segment node pointer\n");
			hr = S_OK;
		}
	}

	hr = ::CoCreateInstance( pTrack->m_guidClassID, NULL, CLSCTX_INPROC,
							 IID_IDirectMusicTrack, (void**)&pIDMTrack );
	if( FAILED( hr ) )
	{
		TRACE("Segment: Unable to CoCreate an IDirectMusicTrack\n");
		hr = S_FALSE;
	}
	else
	{
		pTrack->SetDMTrack( pIDMTrack );

		hr = m_pIDMSegment->InsertTrack( pIDMTrack, pTrack->m_dwGroupBits );
		if( FAILED( hr ) )
		{
			TRACE("Segment: Failed to Insert pIDMTrack.\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}

		UpdateTrackConfig( pTrack );

		VARIANT varDMTrack;
		varDMTrack.vt = VT_UNKNOWN;
		V_UNKNOWN( &varDMTrack ) = pIDMTrack;
		hr = pIStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack );
		if( FAILED( hr ) )
		{
			TRACE("Segment: Failed to set StripMgr's IDMTrack pointer\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	IOProducerOnlyChunk ioProducerOnlyChunk;
	ioProducerOnlyChunk.dwProducerOnlyFlags = pTrack->m_dwProducerOnlyFlags;
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioProducerOnlyChunk;

	hr = pIStripMgr->SetStripMgrProperty( SMP_PRODUCERONLY_FLAGS, varTrackHeader );
	if( FAILED( hr ) )
	{
		// Not a big deal - this is an 'Extra' chunk. after all.
		//TRACE("Segment: Failed to set StripMgr's ProducerOnly flags\n");
		hr = S_OK;
	}

	pTrack->GetStream( &pIStream );
	if( pIStream )
	{
		hr = pIStripMgr->QueryInterface( IID_IPersistStream, (void **)&pIPersistStreamStrip );
		if( FAILED( hr ) )
		{
			TRACE("Segment: Unable to QI pIStripMgr's PersistStream\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}

		LARGE_INTEGER liTemp;
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );
		hr = pIPersistStreamStrip->Load( pIStream );
		if( FAILED( hr ) )
		{
			TRACE("Segment: pIStripMgr failed to Load().\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	else
	{
		TRACE("Segment: Track's IStream is NULL!\n");
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}
	if( pIPersistStreamStrip )
	{
		pIPersistStreamStrip->Release();
	}
	if( pIDMTrack )
	{
		pIDMTrack->Release();
	}
	if( pIStripMgr )
	{
		pIStripMgr->Release();
	}

	if ( SUCCEEDED( hr ) )
	{
		// Set m_fHaveTempoStrip (to disable the Trasnport's tempo edit box) if we have a StripMgr
		// that supports tempo tracks.
		if( IsEqualGUID(pTrack->m_guidClassID, CLSID_DirectMusicTempoTrack) &&
			!IsEqualGUID(pTrack->m_guidEditorID, CLSID_UnknownStripMgr) )
		{
			if( !m_fHaveTempoStrip )
			{
				m_fHaveTempoStrip = TRUE;
			}
		}

		InsertTrackAtDefaultPos( pTrack );
		//m_lstTracks.AddTail( pTrack );

		if(m_pSegmentDlg != NULL)
		{
			m_pSegmentDlg->AddTrack( pTrack );
		}

		// Update the disabled/enabled state of the tempo button
		UpdateConductorTempo();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::PrivRemoveStripMgr

void CSegment::PrivRemoveStripMgr( IDMUSProdStripMgr *pIStripMgr )
{
	CTrack* pTrack;
	POSITION position, pos2;
	IDMUSProdStripMgr* pTmpStripMgr;

	position = m_lstTracks.GetHeadPosition();
    while(position != NULL)
    {
		pos2 = position;
		pTrack = m_lstTracks.GetNext(position);
		pTrack->GetStripMgr( &pTmpStripMgr );
		if( pTmpStripMgr == pIStripMgr )
		{
			// Remove it from our internal list
			m_lstTracks.RemoveAt( pos2 );

			// Remove it from the dialog
			if( m_pSegmentDlg )
			{
				m_pSegmentDlg->RemoveTrack( pTrack );
			}

			// Remove it from the DirectMusic segment
			IDirectMusicTrack* pIDMTrack = NULL;
			pTrack->GetDMTrack( &pIDMTrack );
			if( pIDMTrack )
			{
				ASSERT( m_pIDMSegment );
				m_pIDMSegment->RemoveTrack( pIDMTrack );
				pIDMTrack->Release();
				pIDMTrack = NULL;
			}

			// Save the groupbits and CLSID of the strip manager
			DWORD dwGroupBits = pTrack->m_dwGroupBits;
			CLSID clsidStripMgr = pTrack->m_guidClassID;

			// Delete it
			delete pTrack;

			// Update m_fHaveTempoStrip
			if( clsidStripMgr == CLSID_DirectMusicTempoTrack )
			{
				if( !FindTrackByCLSID( CLSID_DirectMusicTempoTrack, NULL ) )
				{
					// We deleted the only tempo strip
					m_fHaveTempoStrip = FALSE;
				}
				else
				{
					// Keep m_fHaveTempoStrip at TRUE and keep the tempo button DISABLED
					ASSERT( m_fHaveTempoStrip );
				}
			}

			// Tell the transport to refresh the Tempo button state
			UpdateConductorTempo();

			// Notify the stripmgr it is being deleted
			pIStripMgr->OnUpdate( GUID_Segment_DeletedTrack, dwGroupBits, m_pSegmentDlg ? m_pSegmentDlg->m_pTimeline : NULL );

			// Notify all other strip managers that this track was removed
			if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
			{
				m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( clsidStripMgr, dwGroupBits, pIStripMgr );
			}
		}
		pTmpStripMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegment

HRESULT CSegment::DM_SaveSegment( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	HRESULT hr;

	hr = DM_SaveSegmentHeader( pIRiffStream );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = DM_SaveSegmentGUID( pIRiffStream );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = DM_SaveSegmentDesign( pIRiffStream );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = DM_SaveSegmentVersion( pIRiffStream );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = DM_SaveSegmentInfoList( pIRiffStream );
	if ( FAILED( hr ) )
	{
		return hr;
	}

// Save Segment's Container
	if( m_pIContainerNode )
	{
		IPersistStream* pIPersistStream;

		hr = m_pIContainerNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			return hr;
		}

		IStream *pIStream = pIRiffStream->GetStream();
		if( pIStream == NULL )
		{
			pIPersistStream->Release();
			return E_FAIL;
		}

		hr = pIPersistStream->Save( pIStream, fClearDirty );
		pIPersistStream->Release();
		pIStream->Release();
		if( FAILED( hr ) )
		{
			return hr;
		}
	}

	hr = DM_SaveTrackList( pIRiffStream, fClearDirty );
	if ( FAILED( hr ) )
	{
		return hr;
	}

// Save Segment's Tool Graph
	if( m_pIToolGraphNode )
	{
		IPersistStream* pIPersistStream;

		hr = m_pIToolGraphNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			return hr;
		}

		IStream *pIStream = pIRiffStream->GetStream();
		if( pIStream == NULL )
		{
			pIPersistStream->Release();
			return E_FAIL;
		}

		hr = pIPersistStream->Save( pIStream, fClearDirty );
		pIPersistStream->Release();
		pIStream->Release();
		if( FAILED( hr ) )
		{
			return hr;
		}
	}

// Save Segment's Audio Path
	if( m_pIAudioPathNode )
	{
		IPersistStream* pIPersistStream;

		hr = m_pIAudioPathNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			return hr;
		}

		IStream *pIStream = pIRiffStream->GetStream();
		if( pIStream == NULL )
		{
			pIPersistStream->Release();
			return E_FAIL;
		}

		hr = pIPersistStream->Save( pIStream, fClearDirty );
		pIPersistStream->Release();
		pIStream->Release();
		if( FAILED( hr ) )
		{
			return hr;
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegmentHeader

HRESULT CSegment::DM_SaveSegmentHeader( IDMUSProdRIFFStream* pIRiffStream ) const
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_SEGMENT_HEADER_EX oDMSegment;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Segment chunk header
    ck.ckid = DMUS_FOURCC_SEGMENT_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SEGMENT_HEADER_EX
	memset( &oDMSegment, 0, sizeof(DMUS_IO_SEGMENT_HEADER_EX) );

	oDMSegment.dwRepeats = m_dwLoopRepeats;
	oDMSegment.mtLength = m_mtLength;
	oDMSegment.mtPlayStart = m_mtPlayStart;
	oDMSegment.mtLoopStart = m_mtLoopStart;
	oDMSegment.mtLoopEnd = m_mtLoopEnd;
	oDMSegment.dwResolution = m_dwResolution;
	oDMSegment.dwFlags = m_dwSegmentFlags;
    oDMSegment.rtLoopStart = m_rtLoopStart;
    oDMSegment.rtLoopEnd = m_rtLoopEnd;
	if( m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH )
	{
		oDMSegment.rtLength = m_rtLength;
	}
	else
	{
		oDMSegment.rtLength = 0;
	}

	// Write Segment chunk data
	hr = pIStream->Write( &oDMSegment, sizeof(DMUS_IO_SEGMENT_HEADER_EX), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_SEGMENT_HEADER_EX) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();;
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegmentGUID

HRESULT CSegment::DM_SaveSegmentGUID( IDMUSProdRIFFStream* pIRiffStream ) const
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Segment chunk header
    ck.ckid = DMUS_FOURCC_GUID_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Segment GUID chunk data
	hr = pIStream->Write( &m_PPGSegment.guidSegment, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr ) || dwBytesWritten != sizeof(GUID) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();;
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegmentDesign

HRESULT CSegment::DM_SaveSegmentDesign( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckList, ck;
	DWORD dwBytesWritten;
	DMUS_IO_SEGMENT_DESIGN oDMSegmentDesign;

	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Get additional stream information
	DMUSProdStreamInfo	StreamInfo;
	IDMUSProdPersistInfo* pPersistInfo;

	StreamInfo.ftFileType = FT_DESIGN;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
	}

	// Exit early if this is a runtime save
	if( StreamInfo.ftFileType == FT_RUNTIME )
	{
		pIStream->Release();
		return S_FALSE;
	}

	// Write Segment design list header
	ckList.fccType = DMUS_FOURCC_SEGMENT_DESIGN_LIST;
	if( pIRiffStream->CreateChunk( &ckList, MMIO_CREATELIST ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Segment design chunk header
	ck.ckid = DMUS_FOURCC_SEGMENT_DESIGN_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SEGMENT_DESIGN
	memset( &oDMSegmentDesign, 0, sizeof(DMUS_IO_SEGMENT_DESIGN) );

	SyncTimelineSettings();

	m_PPGSegment.Export( &oDMSegmentDesign );
	//oDMSegmentDesign.dblTempo = m_dblTempo;
	//oDMSegmentDesign.fTempoModifier = m_fTempoModifier;
	oDMSegmentDesign.dblZoom = m_dblZoom;
	oDMSegmentDesign.dwVerticalScroll = m_lVerticalScroll;
	oDMSegmentDesign.dblHorizontalScroll = m_dblHorizontalScroll;
	oDMSegmentDesign.dwSnapTo = m_tlSnapTo;
	oDMSegmentDesign.lFunctionbarWidth = m_lFunctionbarWidth;

	oDMSegmentDesign.wQuantizeTarget = m_SequenceQuantizeParms.m_wQuantizeTarget;
	oDMSegmentDesign.bQuantizeResolution = m_SequenceQuantizeParms.m_bResolution;
	oDMSegmentDesign.bQuantizeStrength = m_SequenceQuantizeParms.m_bStrength;
	oDMSegmentDesign.dwQuantizeFlags = m_SequenceQuantizeParms.m_dwFlags;
	oDMSegmentDesign.wVelocityTarget = m_SequenceVelocitizeParams.m_wVelocityTarget;
	oDMSegmentDesign.bCompressMin = m_SequenceVelocitizeParams.m_bCompressMin;
	oDMSegmentDesign.bCompressMax = m_SequenceVelocitizeParams.m_bCompressMax;
	oDMSegmentDesign.lAbsoluteChangeStart = m_SequenceVelocitizeParams.m_lAbsoluteChangeStart;
	oDMSegmentDesign.lAbsoluteChangeEnd = m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd;
	oDMSegmentDesign.dwQuantizeFlags = m_SequenceVelocitizeParams.m_dwVelocityMethod;

	// Write Segment chunk data
	hr = pIStream->Write( &oDMSegmentDesign, sizeof(DMUS_IO_SEGMENT_DESIGN), &dwBytesWritten);
	if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SEGMENT_DESIGN) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend( &ckList, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegmentInfoList

HRESULT CSegment::DM_SaveSegmentInfoList( IDMUSProdRIFFStream* pIRiffStream ) const
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_PPGSegment.strSegmentName.IsEmpty()
	&&  m_PPGSegment.strAuthor.IsEmpty()
	&&  m_PPGSegment.strCopyright.IsEmpty()
	&&  m_PPGSegment.strSubject.IsEmpty()
	&&  m_PPGSegment.strInfo.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Segment name
	if( !m_PPGSegment.strSegmentName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_PPGSegment.strSegmentName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Segment author
	if( !m_PPGSegment.strAuthor.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_PPGSegment.strAuthor );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Segment copyright
	if( !m_PPGSegment.strCopyright.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCOP_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_PPGSegment.strCopyright );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Segment subject
	if( !m_PPGSegment.strSubject.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_USBJ_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_PPGSegment.strSubject );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Segment comments
	if( !m_PPGSegment.strInfo.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCMT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_PPGSegment.strInfo );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveSegmentVersion

HRESULT CSegment::DM_SaveSegmentVersion( IDMUSProdRIFFStream* pIRiffStream ) const
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusSegmentVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write SegmentVersion chunk header
	ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusSegmentVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusSegmentVersionIO.dwVersionMS = (m_PPGSegment.wVersion1 << 16) | m_PPGSegment.wVersion2;
	dmusSegmentVersionIO.dwVersionLS = (m_PPGSegment.wVersion3 << 16) | m_PPGSegment.wVersion4;

	// Write SegmentVersion chunk data
	hr = pIStream->Write( &dmusSegmentVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::DM_SaveTrackList

HRESULT CSegment::DM_SaveTrackList( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
    IStream* pIStream;
    HRESULT hr = S_OK;
    MMCKINFO ck;
	BOOL fRuntimeSave = FALSE;
	DWORD dwSeqTrackGroupBits = 0;
	CTypedPtrList<CPtrList, CTrack*> lstSeqTracks;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Track list chunk header
    ck.fccType = DMUS_FOURCC_TRACK_LIST;
    if( pIRiffStream->CreateChunk( &ck, MMIO_CREATELIST ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		DMUSProdStreamInfo	StreamInfo;

		if( SUCCEEDED( pPersistInfo->GetStreamInfo( &StreamInfo ) )
		&&	(StreamInfo.ftFileType == FT_RUNTIME) )
		{
			fRuntimeSave = TRUE;
		}

		pPersistInfo->Release();
	}

	POSITION position;
	position = m_lstTracks.GetHeadPosition();
    while(position != NULL)
    {
		CTrack	*pTrack = m_lstTracks.GetNext(position);

		if( fRuntimeSave )
		{
			// If this is an audition-only track, skip saving it during a runtime save
			if( pTrack->m_dwProducerOnlyFlags & SEG_PRODUCERONLY_AUDITIONONLY )
			{
				continue;
			}

			// Check if we're reading a sequence track
			if( ::IsEqualGUID( pTrack->m_guidClassID, CLSID_DirectMusicSeqTrack ) )
			{
				if( dwSeqTrackGroupBits != pTrack->m_dwGroupBits )
				{
					hr = WriteCombinedSeqTracks( pIStream, lstSeqTracks );
					ASSERT( lstSeqTracks.IsEmpty() );
					if( FAILED(hr) )
					{
						hr = E_FAIL;
						TRACE("CSegment::DM_SaveTrackList: Unable to combined sequence tracks.\n");
						goto ON_ERROR;
					}
				}

				// Add this track to the list
				lstSeqTracks.AddTail( pTrack );

				// Update dwSeqTrackGroupBits.  This is redundant if dwSeqTrackGroupBits == pTrack->m_dwGroupBits,
				// but it saves code space.
				dwSeqTrackGroupBits = pTrack->m_dwGroupBits;
				continue;
			}
			// Not reading a sequence track
			else if( !lstSeqTracks.IsEmpty() )
			{
				hr = WriteCombinedSeqTracks( pIStream, lstSeqTracks );
				ASSERT( lstSeqTracks.IsEmpty() );
				dwSeqTrackGroupBits = 0;
				if( FAILED(hr) )
				{
					hr = E_FAIL;
					TRACE("CSegment::DM_SaveTrackList: Unable to write combined sequence tracks.\n");
					goto ON_ERROR;
				}
			}
		}

		// Create the RIFF chunk that surrounds the track
		MMCKINFO ckRiff;
		ckRiff.fccType = DMUS_FOURCC_TRACK_FORM;
		if( pIRiffStream->CreateChunk( &ckRiff, MMIO_CREATERIFF ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write the Track Header chunk
		MMCKINFO ckTrkHeader;
		ckTrkHeader.ckid = DMUS_FOURCC_TRACK_CHUNK;
		if( pIRiffStream->CreateChunk( &ckTrkHeader, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		DMUS_IO_TRACK_HEADER oDMTrack;
		DWORD dwByteCount;
		ZeroMemory( &oDMTrack, sizeof( DMUS_IO_TRACK_HEADER ) );
		pTrack->FillTrackHeader( &oDMTrack );
		hr = pIStream->Write( &oDMTrack, sizeof( DMUS_IO_TRACK_HEADER ), &dwByteCount );
		if( FAILED( hr )
			||  dwByteCount != sizeof( DMUS_IO_TRACK_HEADER ) )
		{
			hr = E_FAIL;
			TRACE("CSegment::DM_SaveTrackList: Unable to write track header chunk.\n");
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend( &ckTrkHeader, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Check to see if the track extra's data is different from the defaults
		DMUS_IO_TRACK_EXTRAS_HEADER oDMTrackExtras;
		ZeroMemory( &oDMTrackExtras, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
		pTrack->FillTrackExtrasHeader( &oDMTrackExtras );
		if( oDMTrackExtras.dwFlags != DMUS_TRACKCONFIG_DEFAULT 
		||  oDMTrackExtras.dwPriority != 0 )
		{
			// Write the Track Extras Header chunk
			MMCKINFO ckTrkExtrasHeader;
			ckTrkExtrasHeader.ckid = DMUS_FOURCC_TRACK_EXTRAS_CHUNK;
			if( pIRiffStream->CreateChunk( &ckTrkExtrasHeader, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = pIStream->Write( &oDMTrackExtras, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ), &dwByteCount );
			if( FAILED( hr )
				||  dwByteCount != sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) )
			{
				hr = E_FAIL;
				TRACE("CSegment::DM_SaveTrackList: Unable to write track extras header chunk.\n");
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend( &ckTrkExtrasHeader, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Read the Track's Producer-Only state
		IOProducerOnlyChunk oProducerOnlyChunk;
		ZeroMemory( &oProducerOnlyChunk, sizeof( IOProducerOnlyChunk ) );
		pTrack->FillProducerOnlyChunk( &oProducerOnlyChunk );

		// Check to see if the track's Producer-Only data is different from the defaults
		if( oProducerOnlyChunk.dwProducerOnlyFlags != 0 )
		{
			// Write out the PRODUCER_ONLY chunk
			MMCKINFO ckProducerOnly;
			ckProducerOnly.ckid = DMUS_FOURCC_SEGMENT_DESIGN_TRACK_PRODUCER_ONLY;
			if( pIRiffStream->CreateChunk( &ckProducerOnly, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = pIStream->Write( &oProducerOnlyChunk, sizeof( IOProducerOnlyChunk ), &dwByteCount );
			if( FAILED( hr )
				||  dwByteCount != sizeof( IOProducerOnlyChunk ) )
			{
				hr = E_FAIL;
				TRACE("CSegment::DM_SaveTrackList: Unable to write PRODUCER_ONLY chunk.\n");
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend( &ckProducerOnly, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Write out the Track's data
		BOOL fWritten = FALSE;
		if( !m_fInUndo )
		{
			IDMUSProdStripMgr* pIStripMgr;
			pTrack->GetStripMgr( &pIStripMgr );
			if ( pIStripMgr )
			{
				IPersistStream* pIPersistStream;
				if ( SUCCEEDED( pIStripMgr->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
				{
					if ( pIPersistStream->Save( pIStream, fClearDirty ) == S_OK )
					{
						fWritten = TRUE;
					}
					pIPersistStream->Release();
				}
				pIStripMgr->Release();
				pIStripMgr = NULL;
			}
		}

		if ( !fWritten )
		{
			IStream *pITrackStream;
			pTrack->GetStream( &pITrackStream );

			if ( pITrackStream != NULL )
			{
				LARGE_INTEGER	liTemp;
				liTemp.QuadPart = 0;
				pITrackStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

				STATSTG StatStg;
				if ( SUCCEEDED( pITrackStream->Stat( &StatStg, STATFLAG_NONAME ) ) )
				{
					hr = pITrackStream->CopyTo( pIStream, StatStg.cbSize, NULL, NULL );
				}

				pITrackStream->Release();
			}
		}
		if( pIRiffStream->Ascend( &ckRiff, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// If necessary, write out any sequence tracks that are left
	if( !lstSeqTracks.IsEmpty() )
	{
		hr = WriteCombinedSeqTracks( pIStream, lstSeqTracks );
		ASSERT( lstSeqTracks.IsEmpty() );
		dwSeqTrackGroupBits = 0;
		if( FAILED(hr) )
		{
			hr = E_FAIL;
			TRACE("CSegment::DM_SaveTrackList: Unable to write combined sequence tracks.\n");
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();;
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSection

HRESULT CSegment::IMA25_LoadSection( IStream* /*pIStream*/ )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadRIFFSection

HRESULT CSegment::IMA25_LoadRIFFSection( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream *pIRiffStream = NULL;
	IDMUSProdNode		*pINode = NULL;
	MMCKINFO			ck, ckMain;
	DWORD				dwByteCount;
	DWORD				dwSize;
	DWORD				dwPos;
	HRESULT				hr = S_OK;
	ioIMASection		iSection;
	char				cSectionKey = 0;

    if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
		return E_FAIL;
	}

	ckMain.fccType = FOURCC_SECTION_FORM;
	if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) != 0 )
	{
		return E_FAIL;
	}

    dwPos = StreamTell( pIStream );

	while( pIRiffStream->Descend( &ck, &ckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
        case FOURCC_SECTION:
			dwSize = min( ck.cksize, sizeof( iSection ) );
            hr = pIStream->Read( &iSection, dwSize, &dwByteCount );
			if( FAILED( hr )
			||  dwByteCount != dwSize )
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			
			//m_lTime = iSection.lTime;
			m_mtPlayStart = 0;
			m_PPGSegment.strSegmentName = iSection.wstrName;

			// TODO: Implement
			//AddTempoTrackFromLegacy( iSection.wTempo + ((double)iSection.wTempoFract) / 65536.0 );

			m_dwResolution = 0;//DMUS_SEGF_MEASURE; (Immediate)
			m_dwLoopRepeats = iSection.wRepeats;
			m_mtLoopStart = 0;
			m_mtLoopEnd = 0;
			m_mtLength = iSection.wMeasureLength * iSection.wClocksPerMeasure * (DMUS_PPQ / IMA_PPQ);
			m_dwSegmentFlags = 0;
			m_rtLength = 0;

			// TODO: Implement
			//AddTimeSigTrackFromLegacy( iSection.wClocksPerMeasure, iSection.wClocksPerBeat );

			//m_dwFlags = iSection.dwFlags;
			cSectionKey = iSection.chKey;
			//memcpy( &(m_guidStyle), &(iSection.guidStyle), sizeof(m_guidStyle) );
			//memcpy( &(m_guidPersonality), &(iSection.guidPersonality), sizeof(m_guidPersonality) );
			m_PPGSegment.strInfo = iSection.wstrCategory;
			if( m_pIDMSegment )
			{
				m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
				m_pIDMSegment->SetDefaultResolution( m_dwResolution );
				m_pIDMSegment->SetLength( m_mtLength );
				m_pIDMSegment->SetStartPoint( m_mtPlayStart );
				m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
			}
			SetSegmentHeaderChunk();
			break;
        case FOURCC_STYLEREF:
            hr = IMA25_LoadSectionStyleRef( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
		case FOURCC_DMUSPROD_FILEREF:
			/*
			StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
			LoadJazzReferenceChunk(pIStream);
			*/
			break;
		case FOURCC_SECTIONUI:
			// Ignore it for now.  All the old Section Designer did was
			// read it into a byte array and then write the array back out.
			break;
        case FOURCC_PERSONALITYNAME:
            hr = IMA25_LoadSectionPersonalityRef( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
        case FOURCC_RIFF:
			switch( ck.fccType )
			{
				case FOURCC_BAND_FORM:
					hr = IMA25_LoadSectionBand( pIStream, &ck );
					if( FAILED( hr ) )
					{
						goto ON_ERR;
					}
					break;
			}
			break;
        case FOURCC_CHORD:
			hr = IMA25_LoadSectionChordList( pIStream, &ck, cSectionKey );
			if( FAILED( hr ) )
			{
				goto ON_ERR;
			}
            break;
        case FOURCC_COMMAND:
            hr = IMA25_LoadSectionCommandList( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
        case FOURCC_MUTE:
            hr = IMA25_LoadSectionMuteList( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
		case FOURCC_NOTE:
            hr = IMA25_LoadSectionNoteList( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
		case FOURCC_CURVE:
            hr = IMA25_LoadSectionCurveList( pIStream, &ck );
            if( FAILED( hr ) )
            {
				goto ON_ERR;
            }
            break;
        }
        pIRiffStream->Ascend( &ck, 0 );
		dwPos = StreamTell( pIStream );
    }

ON_ERR:
    if( FAILED( hr ) )
    {
		if( pINode != NULL )
		{
			pINode->Release();
		}
    }

	if ( pIRiffStream != NULL )
	{
		pIRiffStream->Release();
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionMuteList

HRESULT CSegment::IMA25_LoadSectionMuteList( IStream* pIStream, MMCKINFO* pckMain )
{
	ASSERT( pckMain );
	if( !pckMain )
	{
		return E_INVALIDARG;
	}

	HRESULT		hr = S_OK;
	DWORD		cb;
	long		lSize;
    WORD        wSizeMute;
	CMuteList	MuteList;
	MuteList.m_pIFramework = m_pComponent->m_pIFramework;

    lSize = pckMain->cksize;
	// load size of mute structure
    hr = pIStream->Read( &wSizeMute, sizeof( wSizeMute ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSizeMute ) )
	{
        return E_FAIL;
	}
	lSize -= cb;
	while( lSize > 0 )
	{
		hr = MuteList.IMA_AddMute( pIStream, wSizeMute );
		if(hr != S_OK)
		{
			if(FAILED(hr))
			{
				return hr;
			}
			return S_FALSE;
		}
		lSize -= wSizeMute;
    }

	CTrack *pTrack = NULL;
	if ( SUCCEEDED( MuteList.CreateMuteTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadSectionMuteList: Unable to add Mute track.\n");
			hr = E_FAIL;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionNoteList

HRESULT CSegment::IMA25_LoadSectionNoteList( IStream* pIStream, MMCKINFO* pckMain )
{
	ASSERT( pckMain );
	ASSERT( pIStream );
	if( !pckMain || !pIStream )
	{
		return E_INVALIDARG;
	}
	ASSERT( m_pComponent );
	ASSERT( m_pComponent->m_pIFramework );
	if( !m_pComponent || !m_pComponent->m_pIFramework )
	{
		return E_FAIL;
	}

	long lLeft;
	lLeft = pckMain->cksize;

	DWORD dwByteCount;
	WORD wSize, wRead, wSkip;
	HRESULT hr = pIStream->Read( &wSize, sizeof( wSize ), &dwByteCount );
	if( FAILED( hr ) || dwByteCount != sizeof( wSize ) )
	{
		return E_FAIL;
	}
	lLeft -= sizeof( wSize );

	ioIMANoteEvent iNote;
	wRead = (WORD)min( wSize, sizeof( iNote ) );
	wSkip = WORD(wSize - wRead);

	LARGE_INTEGER li;
	li.QuadPart = wSkip;

	IStream* pIMemStream;
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		return E_OUTOFMEMORY;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	CTrack *pTrack;
	pTrack = new CTrack;
	if( !pTrack )
	{
		pIMemStream->Release();
		return E_OUTOFMEMORY;
	}

	pTrack->m_fccType = 0;
	pTrack->m_ckid = DMUS_FOURCC_SEQ_TRACK;
	pTrack->m_dwGroupBits = 1;
	pTrack->m_dwPosition = 0;
	memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicSeqTrack, sizeof(GUID) );

	MMCKINFO ckTrack;
	ckTrack.ckid = DMUS_FOURCC_SEQ_TRACK;
	if( pIRiffStream->CreateChunk( &ckTrack, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	MMCKINFO ckSeqTrack;
	ckSeqTrack.ckid = DMUS_FOURCC_SEQ_LIST;
	if( pIRiffStream->CreateChunk( &ckSeqTrack, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	DWORD dwSize;
	dwSize = sizeof( DMUS_IO_SEQ_ITEM );
	hr = pIMemStream->Write( &dwSize, sizeof(dwSize), &dwByteCount );
	if( FAILED(hr) || dwByteCount != sizeof(dwSize) )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	DMUS_IO_SEQ_ITEM oSeqItem;
	while( lLeft >0 )
	{
		ZeroMemory( &iNote, sizeof( iNote ) );
		hr = pIStream->Read( &iNote, wRead, &dwByteCount );
		if( FAILED( hr ) || dwByteCount != wRead )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}

//		/* Sync with DMusic 388
		oSeqItem.mtTime = iNote.lTime * DMUS_PPQ / IMA_PPQ;
		oSeqItem.mtDuration = iNote.wDuration * DMUS_PPQ / IMA_PPQ;
		//oSeqItem.nOffset = 0;
		BYTE bTrack = (BYTE)IMA25_VOICEID_TO_PCHANNEL(iNote.bVoiceID + 15);
		oSeqItem.dwPChannel = bTrack;
		oSeqItem.bStatus = BYTE(iNote.bStatus & 0xF0);
		if( bTrack % 16 == 9 )
		{
			oSeqItem.bByte1 = achMappingsToDMUS[iNote.bNote];
		}
		else
		{
			oSeqItem.bByte1 = iNote.bNote;
		}
		oSeqItem.bByte2 = iNote.bVelocity;
//		*/
/* Pre DMusic 388
		oSeqItem.lTime = iNote.lTime * DMUS_PPQ / IMA_PPQ;
		oSeqItem.lDuration = iNote.wDuration * DMUS_PPQ / IMA_PPQ;
		oSeqItem.bEventType = iNote.bEventType;
		BYTE bTrack = (BYTE)IMA25_VOICEID_TO_PCHANNEL(iNote.bVoiceID + 15);
		oSeqItem.bStatus = BYTE( (iNote.bStatus & 0xF0) | ( bTrack & 0x0F));
		if( bTrack % 16 == 9 )
		{
			oSeqItem.bByte1 = achMappingsToDMUS[iNote.bNote];
		}
		else
		{
			oSeqItem.bByte1 = iNote.bNote;
		}
		oSeqItem.bByte2 = iNote.bVelocity;
		oSeqItem.bType = EVENT_VOICE;
*/
		hr = pIMemStream->Write( &oSeqItem, sizeof(oSeqItem), &dwByteCount );
		if( FAILED(hr) || dwByteCount != sizeof(oSeqItem) )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}

		lLeft -= wRead;
		if( wSkip )
		{
			pIStream->Seek( li, STREAM_SEEK_CUR, NULL );
			lLeft -= wSkip;
		}
	}

	if( pIRiffStream->Ascend( &ckSeqTrack, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	pTrack->SetStream( pIMemStream );

ON_ERR:
	if( FAILED( hr ) )
	{
		delete pTrack;
	}
	else
	{
		AddTrack( pTrack );
	}
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}
	if( pIMemStream )
	{
		pIMemStream->Release();
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionCurveList

HRESULT CSegment::IMA25_LoadSectionCurveList( IStream* pIStream, MMCKINFO* pckMain )
{
	ASSERT( pckMain );
	if( !pckMain )
	{
		return E_INVALIDARG;
	}

	long lLeft;
	lLeft = pckMain->cksize;

	DWORD dwRead;
	// Calculate Curve size and skip
	WORD wCurveSize, wCurveSkip;
	pIStream->Read( &wCurveSize, sizeof( wCurveSize ), &dwRead );
	lLeft -= sizeof( wCurveSize );

	ioIMACurveEvent iCurve;
	wCurveSkip = WORD(wCurveSize - min( wCurveSize, sizeof( iCurve ) ));
	wCurveSize = (WORD)min( wCurveSize, sizeof( iCurve ) );

	LARGE_INTEGER liCurveSkip;
	liCurveSkip.QuadPart = wCurveSkip;

	// Calculate SubCurve size and skip
	WORD wSubCurveSize, wSubCurveSkip;
	pIStream->Read( &wSubCurveSize, sizeof( wSubCurveSize ), &dwRead );
	lLeft -= sizeof( wSubCurveSize );

	ioIMASubCurve iSubCurve;
	wSubCurveSkip = WORD(wSubCurveSize - min( wSubCurveSize, sizeof( iSubCurve ) ));
	wSubCurveSize = (WORD)min( wSubCurveSize, sizeof( iSubCurve ) );

	LARGE_INTEGER liSubCurveSkip;
	liSubCurveSkip.QuadPart = wSubCurveSkip;

	WORD wNumSubCurves;
	while( lLeft > 0 )
	{
		ZeroMemory( &iCurve, sizeof( iCurve ) );
		pIStream->Read( &iCurve, wCurveSize, &dwRead );
		lLeft -= wCurveSize;
		if( wCurveSkip )
		{
			pIStream->Seek( liCurveSkip, STREAM_SEEK_CUR, NULL );
			lLeft -= wCurveSkip;
		}

		pIStream->Read( &wNumSubCurves, sizeof( wNumSubCurves ), &dwRead );
		lLeft -= sizeof( wNumSubCurves );
		while( (lLeft > 0) && (wNumSubCurves > 0) )
		{
			ZeroMemory( &iSubCurve, sizeof( iSubCurve ) );
			pIStream->Read( &iSubCurve, wSubCurveSize, &dwRead );
			lLeft -= wSubCurveSize;
			wNumSubCurves--;
			if( wSubCurveSkip )
			{
				pIStream->Seek( liSubCurveSkip, STREAM_SEEK_CUR, NULL );
				lLeft -= wSubCurveSkip;
			}
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionChordList

HRESULT CSegment::IMA25_LoadSectionChordList( IStream* pIStream, MMCKINFO* pckMain, char cKey )
{
	HRESULT		hr = S_OK;
	DWORD		cb;
	long		lSize;
    WORD        wSizeChord;
	CChordList	ChordList;
	ChordList.m_pIFramework = m_pComponent->m_pIFramework;

    lSize = pckMain->cksize;
	// load size of chord structure
    hr = pIStream->Read( &wSizeChord, sizeof( wSizeChord ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSizeChord ) )
	{
        return E_FAIL;
	}
	lSize -= cb;
	while( lSize > 0 )
	{
		hr = ChordList.IMA_AddChord( pIStream, wSizeChord, cKey );
		if(hr != S_OK)
		{
			if(FAILED(hr))
			{
				return hr;
			}
			return S_FALSE;
		}
		lSize -= wSizeChord;
    }

	CTrack *pTrack = NULL;
	if ( SUCCEEDED( ChordList.CreateTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadSectionChordList: Unable to add Chord track.\n");
			hr = E_FAIL;
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionCommandList

HRESULT CSegment::IMA25_LoadSectionCommandList( IStream* pIStream, MMCKINFO* pckMain )
{
	HRESULT		hr = S_OK;
	DWORD		cb;
	long		lSize;
    WORD        wSizeChord;
	CCommandList	CommandList;
	CommandList.m_pIFramework = m_pComponent->m_pIFramework;

    lSize = pckMain->cksize;
	// load size of chord structure
    hr = pIStream->Read( &wSizeChord, sizeof( wSizeChord ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSizeChord ) )
	{
        return E_FAIL;
	}
	lSize -= cb;
	while( lSize > 0 )
	{
		hr = CommandList.IMA_AddSectionCommand( pIStream, wSizeChord );
		if(hr != S_OK)
		{
			if(FAILED(hr))
			{
				return hr;
			}
			return S_FALSE;
		}
		lSize -= wSizeChord;
    }

	CTrack *pTrack = NULL;
	if ( SUCCEEDED( CommandList.CreateCommandTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadSectionCommandList: Unable to add Command track.\n");
			hr = E_FAIL;
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionStyleRef

HRESULT CSegment::IMA25_LoadSectionStyleRef( IStream* pIStream, MMCKINFO* pckMain )
{
	ASSERT( pckMain );
	ASSERT( pIStream );
	if( !pckMain || !pIStream )
	{
		return E_INVALIDARG;
	}
	ASSERT( m_pComponent );
	ASSERT( m_pComponent->m_pIFramework );
	if( !m_pComponent || !m_pComponent->m_pIFramework )
	{
		return E_FAIL;
	}

	DWORD	 dwByteCount;
	DWORD	 dwSize;
	wchar_t	 wstrName[MAX_PATH];
	
	dwSize = min( pckMain->cksize, sizeof( wstrName ) );
	
	HRESULT hr = pIStream->Read( wstrName, dwSize, &dwByteCount );
	if( FAILED( hr ) || dwByteCount != dwSize )
	{
		return E_FAIL;
	}

	IStream* pIMemStream;
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		return E_OUTOFMEMORY;
	}

	// Just sets target directory of pIMemStream to the target directory of pIStream 
    ULARGE_INTEGER ul;
    ul.HighPart = 0;
    ul.LowPart = 0;
	pIStream->CopyTo( pIMemStream, ul, NULL, NULL );

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	CTrack *pTrack;
	pTrack = new CTrack;
	if( !pTrack )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		return E_OUTOFMEMORY;
	}

	pTrack->m_ckid = 0;
	pTrack->m_fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
	pTrack->m_dwGroupBits = 1;
	pTrack->m_dwPosition = 0;
	memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicStyleTrack, sizeof(GUID) );

	MMCKINFO ckTrack;
	ckTrack.fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		delete pTrack;
		return E_FAIL;
	}
	{
		MMCKINFO ckStyleList;
		ckStyleList.fccType = DMUS_FOURCC_STYLE_REF_LIST;
		if( pIRiffStream->CreateChunk( &ckStyleList, MMIO_CREATELIST ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Write 'stmp-ck'
			MMCKINFO ck;
			ck.ckid = mmioFOURCC('s', 't', 'm', 'p');
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				DWORD dwTime;
				dwTime = 0;

				hr = pIMemStream->Write(&dwTime, sizeof(dwTime), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(dwTime))
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}

				if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}

			// Write Reference 'DMRF' chunk
			MMCKINFO ckRefList;
			ckRefList.fccType = DMUS_FOURCC_REF_LIST;
			if( pIRiffStream->CreateChunk( &ckRefList, MMIO_CREATELIST ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				// Write Reference header 'refh' chunk
				ck.ckid = DMUS_FOURCC_REF_CHUNK;
				if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
				{
					DMUS_IO_REFERENCE oDMRef;
					memcpy( &oDMRef.guidClassID, &CLSID_DirectMusicStyle, sizeof( GUID ) );
					oDMRef.dwValidData = DMUS_OBJ_NAME | DMUS_OBJ_CLASS;

					hr = pIMemStream->Write(&oDMRef, sizeof(oDMRef), &dwByteCount);
					if(FAILED(hr) || dwByteCount != sizeof(oDMRef))
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}
				}

				// Write Reference name 'name' chunk
				ck.ckid = DMUS_FOURCC_NAME_CHUNK;
				if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
				{
					hr = pIMemStream->Write( wstrName, dwSize, &dwByteCount);
					if(FAILED(hr) || dwByteCount != dwSize)
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}
				}
				if( pIRiffStream->Ascend( &ckRefList, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}
			if( pIRiffStream->Ascend( &ckStyleList, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
		}
		if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
	}
	pIRiffStream->Release();
	pTrack->SetStream( pIMemStream );
	pIMemStream->Release();
	AddTrack( pTrack );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionPersonalityRef

HRESULT CSegment::IMA25_LoadSectionPersonalityRef( IStream* pIStream, MMCKINFO* pckMain )
{
	ASSERT( pckMain );
	ASSERT( pIStream );
	if( !pckMain || !pIStream )
	{
		return E_INVALIDARG;
	}
	ASSERT( m_pComponent );
	ASSERT( m_pComponent->m_pIFramework );
	if( !m_pComponent || !m_pComponent->m_pIFramework )
	{
		return E_FAIL;
	}

	DWORD	 dwByteCount;
	DWORD	 dwSize;
	wchar_t	 wstrName[MAX_PATH];
	
	dwSize = min( pckMain->cksize, sizeof( wstrName ) );
	
	HRESULT hr = pIStream->Read( wstrName, dwSize, &dwByteCount );
	if( FAILED( hr ) || dwByteCount != dwSize )
	{
		return E_FAIL;
	}

	IStream* pIMemStream;
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		return E_OUTOFMEMORY;
	}

	// Just sets target directory of pIMemStream to the target directory of pIStream 
    ULARGE_INTEGER ul;
    ul.HighPart = 0;
    ul.LowPart = 0;
	pIStream->CopyTo( pIMemStream, ul, NULL, NULL );

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	CTrack *pTrack;
	pTrack = new CTrack;
	if( !pTrack )
	{
		pIMemStream->Release();
		pIRiffStream->Release();
		return E_OUTOFMEMORY;
	}

	pTrack->m_ckid = 0;
	pTrack->m_fccType = DMUS_FOURCC_PERS_TRACK_LIST;
	pTrack->m_dwGroupBits = 1;
	pTrack->m_dwPosition = 0;
	memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicChordMapTrack, sizeof(GUID) );

	MMCKINFO ckTrack;
	ckTrack.fccType = DMUS_FOURCC_PERS_TRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		delete pTrack;
		return E_FAIL;
	}
	{
		MMCKINFO ckPersList;
		ckPersList.fccType = DMUS_FOURCC_PERS_REF_LIST;
		if( pIRiffStream->CreateChunk( &ckPersList, MMIO_CREATELIST ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Write 'stmp-ck'
			MMCKINFO ck;
			ck.ckid = mmioFOURCC('s', 't', 'm', 'p');
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				DWORD dwTime;
				dwTime = 0;

				hr = pIMemStream->Write(&dwTime, sizeof(dwTime), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(dwTime))
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}

				if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}

			// Write Reference 'DMRF' chunk
			MMCKINFO ckRefList;
			ckRefList.fccType = DMUS_FOURCC_REF_LIST;
			if( pIRiffStream->CreateChunk( &ckRefList, MMIO_CREATELIST ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				// Write Reference header 'refh' chunk
				ck.ckid = DMUS_FOURCC_REF_CHUNK;
				if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
				{
					DMUS_IO_REFERENCE oDMRef;
					memcpy( &oDMRef.guidClassID, &CLSID_DirectMusicChordMap, sizeof( GUID ) );
					oDMRef.dwValidData = DMUS_OBJ_NAME;

					hr = pIMemStream->Write(&oDMRef, sizeof(oDMRef), &dwByteCount);
					if(FAILED(hr) || dwByteCount != sizeof(oDMRef))
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}
				}

				// Write Reference name 'name' chunk
				ck.ckid = DMUS_FOURCC_NAME_CHUNK;
				if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
				{
					hr = pIMemStream->Write( wstrName, dwSize, &dwByteCount);
					if(FAILED(hr) || dwByteCount != dwSize)
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						delete pTrack;
						return E_FAIL;
					}
				}
				if( pIRiffStream->Ascend( &ckRefList, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}
			if( pIRiffStream->Ascend( &ckPersList, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
		}
		if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
	}
	pIRiffStream->Release();
	pTrack->SetStream( pIMemStream );
	pIMemStream->Release();
	AddTrack( pTrack );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadSectionBand

HRESULT CSegment::IMA25_LoadSectionBand( IStream* pIStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(pckMain);
	HRESULT hr;
	IDMUSProdComponent	*pIBandComponent = NULL;
	IDMUSProdRIFFExt	*pIRIFFExt = NULL;
	IDMUSProdNode		*pIBandNode = NULL;
	IStream				*pIMemStream = NULL;
	IPersistStream		*pIFrom = NULL;
	CTrack				*pTrack = NULL;
	IDMUSProdRIFFStream *pIRiffStream = NULL;
	DWORD				dwByteCount;

	// Get the IDMUSProdComponent interface for the Band component...
	hr = m_pComponent->m_pIFramework->FindComponent( CLSID_BandComponent,  &pIBandComponent );
	if( FAILED ( hr ) )
	{
		goto ON_ERR;
	}

	// first add the changeable band
	hr = pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
	pIBandComponent->Release();
	pIBandComponent = NULL;
	if( FAILED( hr ) )
	{
		goto ON_ERR;
	}

	StreamSeek( pIStream, long(sizeof(DWORD)) * -3, STREAM_SEEK_CUR );
	hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pIBandNode );
	pIRIFFExt->Release();
	pIRIFFExt = NULL;
	if( FAILED( hr ) )
	{
		goto ON_ERR;
	}

	// Now, save the band to a memory stream
		// Create an empty IStream.
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		hr = E_OUTOFMEMORY;
		goto ON_ERR;
	}

	hr = pIBandNode->QueryInterface(IID_IPersistStream, (void**)&pIFrom);
	if(FAILED(hr))
	{
		goto ON_ERR;
	}

	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	MMCKINFO ckTrack;
	ckTrack.fccType = DMUS_FOURCC_BANDTRACK_FORM;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATERIFF ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	{	// Band track header
		MMCKINFO ckTrackHeader;
		ckTrackHeader.ckid = DMUS_FOURCC_BANDTRACK_CHUNK;
		if( pIRiffStream->CreateChunk( &ckTrackHeader, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}

		DMUS_IO_BAND_TRACK_HEADER oBandTrackHeader;
		oBandTrackHeader.bAutoDownload = TRUE;

		hr = pIMemStream->Write( &oBandTrackHeader, sizeof(oBandTrackHeader), &dwByteCount );
		if( FAILED(hr) || dwByteCount != sizeof(oBandTrackHeader) )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}

		if( pIRiffStream->Ascend( &ckTrackHeader, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}
	}

	{	// Band list
		MMCKINFO ckBandList;
		ckBandList.fccType = DMUS_FOURCC_BANDS_LIST;
		if( pIRiffStream->CreateChunk( &ckBandList, MMIO_CREATELIST ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}

		{	// Band list item
			MMCKINFO ckBandListItem;
			ckBandListItem.fccType = DMUS_FOURCC_BAND_LIST;

			if( pIRiffStream->CreateChunk( &ckBandListItem, MMIO_CREATELIST ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERR;
			}

			{	// Band list item header
				MMCKINFO ckBandListItemHeader;
				ckBandListItemHeader.ckid = DMUS_FOURCC_BANDITEM_CHUNK;

				if( pIRiffStream->CreateChunk( &ckBandListItemHeader, 0 ) != 0 )
				{
					hr = E_FAIL;
					goto ON_ERR;
				}

				DMUS_IO_BAND_ITEM_HEADER oBandItemHeader;
				oBandItemHeader.lBandTime = 0;
				hr = pIMemStream->Write( &oBandItemHeader, sizeof(oBandItemHeader), &dwByteCount );
				if( FAILED(hr) || dwByteCount != sizeof(oBandItemHeader) )
				{
					hr = E_FAIL;
					goto ON_ERR;
				}

				if( pIRiffStream->Ascend( &ckBandListItemHeader, 0 ) != 0 )
				{
					hr = E_FAIL;
					goto ON_ERR;
				}
			}	// Band list item header

			// Save band chunk
			pIFrom->Save(pIMemStream, FALSE);
			pIFrom->Release();
			pIFrom = NULL;
			pIBandNode->Release();
			pIBandNode = NULL;
			if( pIRiffStream->Ascend( &ckBandListItem, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
		}	// Band list item

		if( pIRiffStream->Ascend( &ckBandList, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}
	}	// Band list

	if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	pTrack = new CTrack;
	if( !pTrack )
	{
		hr = E_OUTOFMEMORY;
		goto ON_ERR;
	}

	pTrack->m_fccType = DMUS_FOURCC_BANDTRACK_FORM;
	pTrack->m_ckid = 0;
	pTrack->m_dwGroupBits = 1;
	pTrack->m_dwPosition = 0;
	memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicBandTrack, sizeof(GUID) );
	pTrack->SetStream( pIMemStream );

ON_ERR:
	if( FAILED( hr ) )
	{
		if( pTrack )
		{
			delete pTrack;
		}
	}
	else
	{
		AddTrack( pTrack );
	}
	if( pIMemStream )
	{
		pIMemStream->Release();
	}
	if( pIBandComponent )
	{
		pIBandComponent->Release();
	}
	if( pIRIFFExt )
	{
		pIRIFFExt->Release();
	}
	if( pIBandNode )
	{
		pIBandNode->Release();
	}
	if( pIFrom )
	{
		pIFrom->Release();
	}
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::IMA25_LoadTemplate

HRESULT CSegment::IMA25_LoadTemplate( IStream* pIStream )
{
	//IUnknown*				pIStyle;
	//IUnknown*				pIPersonality;
	//IDMUSProdProject*		pIProject;
	//IDMUSProdNode*			pIReferenceNode = NULL;
	CTemplateStruct			TemplateStruct;
	CCommandList			CommandList;
	CChordList				ChordList;
	CString					strError;
	HRESULT					hr;
	_LARGE_INTEGER			liTemp;
	ULONG					ulBytesRead;
	long					dwid;
	long					lRecordSize; //lFileSize;
	long					lFileSize = 0;

    ASSERT( pIStream != NULL );

	// Make sure the undo mgr exists
	ASSERT(m_pUndoMgr != NULL);
	if(m_pUndoMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	hr = pIStream->Read(&dwid, 4, &ulBytesRead);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	else if(hr == S_FALSE || ulBytesRead != 4)
	{
		return S_FALSE;
	}
	if(dwid != ID_TEMPLATE)
	{
		return S_FALSE;
	}
	hr = pIStream->Read((void *) &lFileSize, 4, &ulBytesRead);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	else if(hr == S_FALSE || ulBytesRead != 4)
	{
		return S_FALSE;
	}
	hr = pIStream->Read((char *) &lRecordSize, 4, &ulBytesRead);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	else if(hr == S_FALSE || ulBytesRead != 4)
	{
		return S_FALSE;
	}
	swaplong((char *) &lRecordSize);
	swaplong((char *) &lFileSize);

	lFileSize -= 4;
	hr = CSegment::ReadTemplate(pIStream, &TemplateStruct, lRecordSize);
	if(hr != S_OK)
	{
		return hr;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );
	CommandList.m_pIFramework = m_pComponent->m_pIFramework;
	ChordList.m_pIFramework = m_pComponent->m_pIFramework;

	m_PPGSegment.strSegmentName = TemplateStruct.m_szName;
	m_PPGSegment.strInfo = TemplateStruct.m_szType;
	m_mtLength = TemplateStruct.m_nMeasures * DMUS_PPQ * 4; // Assume 4/4 time
	if( TemplateStruct.m_bLoop )
	{
		m_PPGSegment.wSegmentRepeats = 32767;
	}
	m_dwLoopRepeats = 0;
	m_mtPlayStart = 0;
	m_mtLoopStart = 0;
	m_mtLoopEnd = 0;
	m_dwResolution = 0;// DMUS_SEGF_MEASURE; (Immediate)
	m_dwSegmentFlags = 0;
	m_rtLength = 0;
	if( m_pIDMSegment )
	{
		m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
		m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		m_pIDMSegment->SetLength( m_mtLength );
		m_pIDMSegment->SetStartPoint( m_mtPlayStart );
		m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
	}
	SetSegmentHeaderChunk();
	m_wLegacyActivityLevel = TemplateStruct.m_wActivityLevel;
	m_fLoadedLegacyActivityLevel = true;
	WORD wKey = TemplateStruct.m_wKey;

	lFileSize -= lRecordSize;
	while (lFileSize > 0)
	{
		hr = pIStream->Read((char *) &dwid, 4, &ulBytesRead);
		if(hr != S_OK || ulBytesRead != 4)
		{
			if(FAILED(hr))
			{
				hr = E_FAIL;
			}
			return hr;
		}
		hr = pIStream->Read((char *) &lRecordSize, 4, &ulBytesRead);
		if(hr != S_OK || ulBytesRead != 4)
		{
			if(FAILED(hr))
			{
				hr = E_FAIL;
			}
			return hr;
		}
		swaplong((char *) &lRecordSize);
		lFileSize -= 8;
		if (dwid == ID_COMMAND)
		{
			hr = CommandList.IMA_AddTemplateCommand( pIStream, lRecordSize );
			if(hr != S_OK)
			{
				if(hr == E_OUTOFMEMORY)
				{
					return E_OUTOFMEMORY;
				}
				if(FAILED(hr))
				{
					return hr;
				}
				return S_FALSE;
			}
		}
		else if (dwid == ID_CHORDEXT)
		{
			// BUGBUG: I'm not sure about using wKey here...
			hr = ChordList.IMA_AddChord( pIStream, lRecordSize, (BYTE)wKey );
			if(hr != S_OK)
			{
				if(FAILED(hr))
				{
					return hr;
				}
				return S_FALSE;
			}
		}
		else if(dwid == ID_STYLEREF)
		{
			// BUGBUG: Should ASSERT that we haven't already loaded a StyleRef.
			// Initialize the CStyleRef
			CStyleRef StyleRef;
			StyleRef.m_pIFramework = m_pComponent->m_pIFramework;
			hr = StyleRef.Load( pIStream );
			if( SUCCEEDED( hr ) && StyleRef.m_pINode )
			{
				AddAuditionStyleRefTrack( StyleRef.m_pINode );
			}
			else
			{
				// BUGBUG: No failure handler
			}
		}
		else if(dwid == ID_PERSREF)
		{
			// Initialize the CStyleRef
			CStyleRef StyleRef;
			StyleRef.m_pIFramework = m_pComponent->m_pIFramework;

			// Load the ref chunk
			hr = StyleRef.Load( pIStream );
			if( SUCCEEDED( hr ) && StyleRef.m_pINode )
			{
				AddAuditionPersRefTrack( StyleRef.m_pINode );
			}
			else
			{
				// BUGBUG: No failure handler
			}
		}
		else
		{
			liTemp.LowPart  = lRecordSize;
			liTemp.HighPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
		}
		lFileSize -= lRecordSize;
	}

	CTrack *pTrack = NULL;
	if ( SUCCEEDED( CommandList.CreateCommandTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadTemplate: Unable to add Command track.\n");
		}
		pTrack = NULL;
	}
	if ( SUCCEEDED( CommandList.CreateSignPostTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadTemplate: Unable to add SignPost track.\n");
		}
		pTrack = NULL;
	}
	if ( SUCCEEDED( ChordList.CreateTrack( &pTrack ) ) )
	{
		if( FAILED( AddTrack( pTrack ) ) )
		{
			TRACE("Segment: IMA25_LoadTemplate: Unable to add Chord track.\n");
		}
		pTrack = NULL;
	}
	/*
	// If we have a style and a personality, compose a new section and stick our chords in it.
	// Otherwise, the user will just be able to see the chords and not be able to hear them.
	if(m_pIStyle != NULL && m_pIPersonality != NULL)
	{
		// Create our section
		hr = SyncTemplateWithEngine(FALSE);
		if(FAILED(hr))
		{
			return E_FAIL;
		}

		// Update the chordlist of the section if there is one.
		UpdateSectionChords();
	}
	*/

    if (lFileSize > 0)
		TRACE("ERROR: Template read leaves filesize at %ld\n", lFileSize);

	if(m_pSegmentDlg != NULL)
	{
		m_pSegmentDlg->SetTimelineLength( m_mtLength );
	}

	// Only refresh if we are in the tree, or else GetData will fail when it tries to find
	// the project.
	if(m_fAddedToTree)
	{
		ASSERT(m_pComponent);
		if(m_pComponent->m_pIPageManager)
		{
			m_pComponent->m_pIPageManager->RefreshData();
		}
	}
	else
	{
		m_fAddedToTree = TRUE;
	}
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::ReadRIFFMIDIFile

HRESULT CSegment::ReadRIFFMIDIFile( IStream* pIStream )
{
	return CreateSegmentFromMIDIStream(this, pIStream);
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::ReadMIDIFile

HRESULT CSegment::ReadMIDIFile( IStream* pIStream )
{
	return CreateSegmentFromMIDIStream(this, pIStream);
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IPersistStream::Save

HRESULT CSegment::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_DESIGN;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Save the Segment
	if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )
	||  ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		// Direct Music format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_SEGMENT_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( DM_SaveSegment( pIRiffStream, fClearDirty ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModifiedFlag( FALSE );
				}
				hr = S_OK;
			}
			pIRiffStream->Release();;
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::WriteMIDIFile

HRESULT CSegment::WriteMIDIFile( IStream* /*pIStream*/ )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IPersistStream::GetSizeMax

HRESULT CSegment::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo::SetTempo

HRESULT CSegment::SetTempo( double dblTempo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Update the first tempo track we find that returns S_OK for the tempo at lTimeCursor
	if( m_fHaveTempoStrip && m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		HRESULT hr = E_UNEXPECTED;

		// Get the tempo at the current cursor position;
		long lTimeCursor;
		if( SUCCEEDED(m_pSegmentDlg->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTimeCursor) ) )
		{
			IDMUSProdStripMgr *pTempoStripMgrSave = NULL;
			MUSIC_TIME mtLatestTempoTime = LONG_MIN;
			DWORD dwIndex = 0;
			while( TRUE )
			{
				IDMUSProdStripMgr *pTempoStripMgr = NULL;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
				{
					if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
					{
						DMUS_TEMPO_PARAM tempo;
						if( S_OK == pTempoStripMgr->GetParam( GUID_TempoParam, lTimeCursor, NULL, &tempo ) )
						{
							if( mtLatestTempoTime < tempo.mtTime )
							{
								mtLatestTempoTime = tempo.mtTime;
								if( pTempoStripMgrSave )
								{
									pTempoStripMgrSave->Release();
								}
								pTempoStripMgrSave = pTempoStripMgr;
								pTempoStripMgrSave->AddRef();
							}
						}
					}

					pTempoStripMgr->Release();
				}
				else
				{
					// No more strips to check - exit
					break;
				}
				dwIndex++;
			}

			if( pTempoStripMgrSave )
			{
				// Found the tempo strip to modify - tell it of the changes
				ASSERT( mtLatestTempoTime != LONG_MIN );

				DMUS_TEMPO_PARAM tempo;
				tempo.mtTime = lTimeCursor;
				tempo.dblTempo = dblTempo;
				hr = pTempoStripMgrSave->OnUpdate(GUID_Segment_Set_Tempo, 0xFFFFFFFF, &tempo );
				pTempoStripMgrSave->Release();
				return S_OK;
			}
		}
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo::GetTempo

HRESULT CSegment::GetTempo( double *pdblTempo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate pdblTempo
	if( pdblTempo == NULL )
	{
		return E_POINTER;
	}

	// Enable the tempo edit box if we have a tempo track
	if( m_fHaveTempoStrip )
	{
		HRESULT hr = E_UNEXPECTED;

		// Get the tempo at the current cursor position;
		long lTimeCursor;
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline &&
			SUCCEEDED(m_pSegmentDlg->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTimeCursor) ) )
		{
			MUSIC_TIME mtLatestTempoTime = LONG_MIN;
			DWORD dwIndex = 0;
			while( TRUE )
			{
				IDMUSProdStripMgr *pTempoStripMgr = NULL;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
				{
					if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
					{
						DMUS_TEMPO_PARAM tempo;
						hr = pTempoStripMgr->GetParam( GUID_TempoParam, lTimeCursor, NULL, &tempo );

						// If we got a real tempo, and it's more recnet than any other tempos so far
						if( (hr == S_OK) && (tempo.mtTime > mtLatestTempoTime) )
						{
							// Save the tempo value and update the latest tempo time.
							*pdblTempo = tempo.dblTempo;
							mtLatestTempoTime = tempo.mtTime;
						}
					}

					pTempoStripMgr->Release();
				}
				else
				{
					// No more strips to check - exit
					break;
				}
				dwIndex++;
			}

			if( mtLatestTempoTime != LONG_MIN )
			{
				return S_OK;
			}
		}

		// Something failed - return an error code
		return E_UNEXPECTED;
	}

	// No Tempo strip - return E_NOTIMPL;
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo::SetTempoModifier

HRESULT CSegment::SetTempoModifier( double dblModifier )
{
	UNREFERENCED_PARAMETER(dblModifier);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo::GetTempoModifier

HRESULT CSegment::GetTempoModifier( double *pdblModifier )
{
	UNREFERENCED_PARAMETER(pdblModifier);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdConductorTempo::TempoModified

HRESULT CSegment::TempoModified( BOOL fModified )
{
	UNREFERENCED_PARAMETER(fModified);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::Play

HRESULT CSegment::Play( BOOL fPlayFromStart )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIDMPerformance != NULL );
	ASSERT( m_pIDMSegment != NULL );

	if( m_pComponent->m_pIDMPerformance == NULL )
	{
		return E_UNEXPECTED;
	}

	// If we're already playing, Stop()
	if( !m_fInTransition
	&&	(!m_lstSegStates.IsEmpty() || !m_lstTransSegStates.IsEmpty()) )
	{
		Stop( TRUE );
	}

	// m_lstTransSegStates and m_lstSegStates may not yet be empty, since segstates
	// are only removed in OnNotify()
	/*
	ASSERT( m_lstTransSegStates.IsEmpty() );
	ASSERT( m_lstSegStates.IsEmpty() );
	*/

	if( m_pIDMSegment == NULL )
	{
		return E_FAIL;
	}

	// If we're not in a transition, ensure m_pIDMTransitionSegment is NULL
	if( !m_fInTransition )
	{
		RELEASE( m_pIDMTransitionSegment );
	}

	HRESULT hr;
	hr = InitSegmentForPlayback();
	if( FAILED( hr ) )
	{
		AfxMessageBox( IDS_ERR_BAD_AUDIOPATH );
		return hr;
	}

	long lStartTime;
	if( fPlayFromStart
	||	!m_PPGSegment.pITimelineCtl
	||	FAILED( m_pSegmentDlg->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lStartTime ) ) )
	{
		lStartTime = m_mtPlayStart;
	}
	m_pIDMSegment->SetStartPoint( lStartTime );

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline && !m_fInTransition )
	{
		m_pSegmentDlg->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lStartTime );
	}

	m_pComponent->m_pIConductor->SetupMIDIAndWaveSave( m_pIDMSegment );

	EnterCriticalSection( &m_csSegmentState );
	IDirectMusicSegmentState *pNewSegmentState;
	hr = m_pComponent->m_pIDMPerformance->PlaySegmentEx( m_pIDMSegment, 0, m_fInTransition ? m_pIDMTransitionSegment : NULL,
		m_fInTransition ? m_dwTransitionPlayFlags : (DMUS_SEGF_DEFAULT | DMUS_SEGF_QUEUE),
		0, &pNewSegmentState, NULL, NULL );

	if( SUCCEEDED( hr ) )
	{
		SegStateParams *pSegStateParams = new SegStateParams( pNewSegmentState, m_mtLoopStart, m_mtLoopEnd, m_dwLoopRepeats);
		m_lstSegStates.AddHead( pSegStateParams );
		LeaveCriticalSection( &m_csSegmentState );

		{
			MUSIC_TIME mtNow;
			REFERENCE_TIME rtNow;
			m_pComponent->m_pIDMPerformance->GetTime( &rtNow, &mtNow );
			TRACE("Segment: Started playback at %ld | %I64d for %x\n", mtNow, rtNow, pNewSegmentState );
		}

		m_pComponent->m_pIConductor->SetupMIDIAndWaveSave( pNewSegmentState );

		// If we have an audiopath, notify it that we started playing
		RegisterSegementStateWithAudiopath( TRUE, pNewSegmentState );

		// If the timeline exists, notify all stripmgrs that we started playing
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Start, 0xFFFFFFFF, pNewSegmentState );
		}

		// Release our reference to the temporary segment state
		pNewSegmentState->Release();
	}
	else
	{
		LeaveCriticalSection( &m_csSegmentState );

		if( DMUS_E_NO_AUDIOPATH == hr )
		{
			DWORD dwResolution = m_fInTransition ? m_dwTransitionPlayFlags : DMUS_SEGF_DEFAULT;
			if( dwResolution & DMUS_SEGF_DEFAULT )
			{
				dwResolution |= m_dwResolution;
			}
			if( m_pIAudioPathNode
			&&	(dwResolution & DMUS_SEGF_USE_AUDIOPATH) )
			{
				AfxMessageBox( IDS_ERR_PLAY_BAD_AUDIOPATH );
			}
			else
			{
				AfxMessageBox( IDS_ERR_NODEFAULT_AUDIOPATH );
			}
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::Stop

HRESULT CSegment::Stop( BOOL fStopImmediate )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ( !m_pIDMSegment )
	{
		return E_FAIL;
	}

	// Capture the state of the control key
	m_fCtrlKeyDownWhenStopCalled = ((GetKeyState( VK_CONTROL ) & 0x8000) != 0);

	if( m_lstSegStates.IsEmpty()
	&&	m_lstTransSegStates.IsEmpty() )
	{
		// Whoa - something's not right.  Try to stop our segment anyways.
		TRACE("Segment: Stop() called when SegmentState is NULL!\n");

		if( m_pComponent->m_pIDMPerformance )
		{
#ifdef _DEBUG
			MUSIC_TIME mtNow;
			REFERENCE_TIME rtNow;
			m_pComponent->m_pIDMPerformance->GetTime( &rtNow, &mtNow );
			TRACE("Segment: Stopping playback at %ld | %I64d\n", mtNow, rtNow );
#endif
			// Stop immediately, since things aren't right
			if ( FAILED( m_pComponent->m_pIDMPerformance->Stop( m_pIDMSegment, NULL, 0, 0 ) ) )
			{
				return E_FAIL;
			}
		}

		// If the timeline exists, notify all stripmgrs that we stopped playing
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Stop, 0xFFFFFFFF, NULL );
		}

		m_fCtrlKeyDownWhenStopCalled = FALSE;
		return E_UNEXPECTED;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIDMPerformance != NULL );
	if( !m_pComponent || !m_pComponent->m_pIDMPerformance )
	{
		m_fCtrlKeyDownWhenStopCalled = FALSE;
		return E_UNEXPECTED;
	}

	MUSIC_TIME mtNow;
	m_pComponent->m_pIDMPerformance->GetTime( NULL, &mtNow );

	// Try and stop the transition segment state, if there is one
	MUSIC_TIME mtEarliestStartTime = 0;
	bool fStoppedSomething = false;

	EnterCriticalSection( &m_csSegmentState );

	// Iterate through all transition segment states, if there are any
	if( !m_lstTransSegStates.IsEmpty() )
	{
		POSITION pos = m_lstTransSegStates.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to the segment state
			IDirectMusicSegmentState *pSegState = m_lstTransSegStates.GetNext( pos );

			// Get the start time of this segment state
			MUSIC_TIME mtStartTime;
			if( SUCCEEDED( pSegState->GetStartTime( &mtStartTime ) )
			&&	mtStartTime < mtEarliestStartTime )
			{
				mtEarliestStartTime = mtStartTime;
			}

			// Try and stop this segment state
			if( SUCCEEDED( m_pComponent->m_pIDMPerformance->Stop( NULL, pSegState, 0, 
																  fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
			{
				fStoppedSomething = true;
			}
		}
	}
	// No transition segment states, so try and stop the transition segment
	/* Won't work, because the transition segment is composed and we never see its real pointer (bug #33274)
	else if( m_pIDMTransitionSegment )
	{
		if( SUCCEEDED( m_pComponent->m_pIDMPerformance->StopEx( m_pIDMTransitionSegment, 0, fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
		{
			fStoppedSomething = true;
		}
	}
	*/

	RELEASE( m_pIDMTransitionSegment );

	// Iterate through all of our segment states
	POSITION pos = m_lstSegStates.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the segment state structure
		SegStateParams *pSegStateParams = m_lstSegStates.GetNext( pos );

		// Get the start time of this segment state
		MUSIC_TIME mtStartTime;
		if( SUCCEEDED( pSegStateParams->pSegState->GetStartTime( &mtStartTime ) )
		&&	mtStartTime < mtEarliestStartTime )
		{
			mtEarliestStartTime = mtStartTime;
		}

		// Try and stop this segment state
		if( SUCCEEDED( m_pComponent->m_pIDMPerformance->Stop( NULL, pSegStateParams->pSegState, 0, 
															  fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
		{
			fStoppedSomething = true;
		}
	}

	LeaveCriticalSection( &m_csSegmentState );

	// If the timeline exists, notify all stripmgrs that we stopped playing
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Stop, 0xFFFFFFFF, NULL );
	}

	// If we're trying to stop the segment before it started playing, return an error code
	// since the current segment probably didn't stop playing.
	if( !fStoppedSomething
	||	(mtNow < mtEarliestStartTime) )
	{
		return E_UNEXPECTED;
	}
	else
	{
		return S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::Transition

HRESULT CSegment::Transition()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDMSegment );
	if( !m_pIDMSegment )
	{
		return E_FAIL;
	}

	ASSERT( m_pComponent->m_pIDMPerformance != NULL );
	if( !m_pComponent->m_pIDMPerformance )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pComponent );
	ASSERT( m_pComponent->m_pIConductor );
	if( !m_pComponent || !m_pComponent->m_pIConductor )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	ConductorTransitionOptions TransitionOptions;
	if( FAILED( m_pComponent->m_pIConductor->GetTransitionOptions( &TransitionOptions ) ) )
	{
		return E_NOTIMPL;
	}

	m_fCtrlKeyDownWhenStopCalled = FALSE;
	m_fInTransition = TRUE;

	RELEASE( m_pIDMTransitionSegment );

	if( TransitionOptions.dwFlags & TRANS_NO_TRANSITION )
	{
		// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
		if( TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_QUEUE;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
		{
			m_dwTransitionPlayFlags = 0;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_GRID )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_GRID;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_BEAT;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_MEASURE;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_SEGMENTEND;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_DEFAULT;
		}
		else
		{
			ASSERT( FALSE );
			m_dwTransitionPlayFlags = 0;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_ALIGN;

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_TICK;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_GRID;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_BEAT;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_MEASURE )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_MEASURE;
			}
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_MARKER;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_NOINVALIDATE )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_NOINVALIDATE;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_AFTERPREPARETIME;
		}

		if( TransitionOptions.pDMUSProdNodeSegmentTransition )
		{
			IDMUSProdNode *pNode;
			if( SUCCEEDED( TransitionOptions.pDMUSProdNodeSegmentTransition->QueryInterface( IID_IDMUSProdNode, (void **)&pNode ) ) )
			{
				if( SUCCEEDED( pNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&m_pIDMTransitionSegment ) ) )
				{
					m_dwTransitionPlayFlags |= DMUS_SEGF_AUTOTRANSITION;
				}
				pNode->Release();
			}
		}

		// Play the segment
		hr = Play(TRUE);

		// Reset transition flags
		m_fInTransition = FALSE;
		m_dwTransitionPlayFlags = 0;

		// Return result code
		return hr;
	}

	IDirectMusicChordMap *pIDMChordMap = NULL;
	// Get the segment's personality.
	hr = m_pIDMSegment->GetParam(GUID_IDirectMusicChordMap, 0xffffffff, 0, 0, NULL, &pIDMChordMap);
	if( FAILED(hr) || !pIDMChordMap )
	{
		TRACE("Segment Transition: Failed to get Personality from segment.\n");
		//return E_NOTIMPL;
	}

	IDirectMusicComposer8 *pIDMComposer;
	::CoCreateInstance( CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicComposer8, (void**)&pIDMComposer );
	if( !pIDMComposer )
	{
		if( pIDMChordMap )
		{
			pIDMChordMap->Release();
		}
		m_fInTransition = FALSE;
		return E_NOTIMPL;
	}

	// Necessary to set m_dwTransitionPlayFlags temporarily, so that
	// InitSegmentForPlayback can return a failure code if it is unable
	// to get and AudioPathConfig object
	m_dwTransitionPlayFlags = m_dwResolution;
	hr = InitSegmentForPlayback();
	m_dwTransitionPlayFlags = 0;
	if( FAILED( hr ) )
	{
		AfxMessageBox( IDS_ERR_BAD_AUDIOPATH );

		hr = E_FAIL;

		if(	pIDMChordMap )
		{
			pIDMChordMap->Release();
		}
		pIDMComposer->Release();

		m_fInTransition = FALSE;

		return hr;
	}

	ASSERT(m_pComponent->m_pIDMPerformance != NULL);
	m_pIDMSegment->SetStartPoint( m_mtPlayStart );

#ifdef _DEBUG
	MUSIC_TIME mtNow;
	REFERENCE_TIME rtNow;
	m_pComponent->m_pIDMPerformance->GetTime( &rtNow, &mtNow );
	TRACE("Segment: AutoTransition at %ld | %I64d\n", mtNow, rtNow );
#endif

	IDirectMusicSegmentState *pTransSegState = NULL;
	IDirectMusicSegmentState *pOurSegState = NULL;
	hr = DoAutoTransition( TransitionOptions, m_pComponent->m_pIDMPerformance,
					  pIDMComposer, m_pIDMSegment,
					  pIDMChordMap, &pTransSegState,
					  &pOurSegState );

	if( SUCCEEDED( hr ) )
	{
		m_pComponent->m_pIConductor->SetupMIDIAndWaveSave( pOurSegState );

		// Add our segment state and the transition segment state to the list of segment states
		SegStateParams *pSegStateParams = new SegStateParams( pOurSegState, m_mtLoopStart, m_mtLoopEnd, m_dwLoopRepeats);

		EnterCriticalSection( &m_csSegmentState );
		m_lstSegStates.AddHead( pSegStateParams );
		if( pTransSegState )
		{
			m_lstTransSegStates.AddHead( pTransSegState );
			pTransSegState->AddRef();
		}
		LeaveCriticalSection( &m_csSegmentState );

		// If we have an audiopath, notify it that we started playing
		RegisterSegementStateWithAudiopath( TRUE, pOurSegState );

		// If the timeline exists, notify all stripmgrs that we started playing
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Start, 0xFFFFFFFF, pOurSegState );
		}

		RELEASE( pTransSegState );
		RELEASE( pOurSegState );
	}
	else
	{
		// If autotransition failed, fallback to just playing the segment at the
		// specified boundary

		// Clean up
		TRACE("Segment AutoTransition: Failed with %x!.\n", hr);

		RELEASE( pTransSegState );
		RELEASE( pOurSegState );

		if(	pIDMChordMap )
		{
			pIDMChordMap->Release();
		}
		pIDMComposer->Release();

		// Now do the fallback transition
		// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
		if( TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_QUEUE;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
		{
			m_dwTransitionPlayFlags = 0;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_GRID )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_GRID;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_BEAT;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_MEASURE;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_SEGMENTEND;
		}
		else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT )
		{
			m_dwTransitionPlayFlags = DMUS_SEGF_DEFAULT;
		}
		else
		{
			ASSERT( FALSE );
			m_dwTransitionPlayFlags = 0;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_ALIGN;

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_TICK;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_GRID;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_BEAT;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_MEASURE )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_VALID_START_MEASURE;
			}
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_MARKER;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_NOINVALIDATE )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_NOINVALIDATE;
		}

		if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME )
		{
			m_dwTransitionPlayFlags |= DMUS_SEGF_AFTERPREPARETIME;
		}

		// Play the segment
		hr = Play(TRUE);

		// Reset transition flags
		m_fInTransition = FALSE;
		m_dwTransitionPlayFlags = 0;

		// Return result code
		return hr;
	}

	if(	pIDMChordMap )
	{
		pIDMChordMap->Release();
	}
	pIDMComposer->Release();

	m_fInTransition = FALSE;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::Record

HRESULT CSegment::Record(BOOL fPressed)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fRecordPressed != fPressed )
	{
		m_fRecordPressed = fPressed;

		// If the timeline exists, notify all stripmgrs that the record button was toggled
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_RecordButton, 0xFFFFFFFF, &fPressed );
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::GetName

HRESULT CSegment::GetName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	// Segment file name
	CString strName;
	BSTR bstrSegmentFileName;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->GetNodeFileName( this, &bstrSegmentFileName ) ) )
	{
		// Node is in the Project tree
		strName = bstrSegmentFileName;
		::SysFreeString( bstrSegmentFileName );
		int nFindPos = strName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strName = strName.Right( strName.GetLength() - nFindPos - 1 );
		}
		strName += _T(": ");
	}
	else
	{
		// Node has not yet been placed in the Project Tree
		// because the Segment is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}
	
	// Plus "name"
	strName += m_PPGSegment.strSegmentName;

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTransport::TrackCursor

HRESULT CSegment::TrackCursor(BOOL fTrackCursor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fTrackTimeCursor = fTrackCursor;

	if( m_pSegmentDlg && IsPlaying() )
	{
		m_pSegmentDlg->EnableTimer( fTrackCursor );
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdSecondaryTransport implementation

// CSegment IDMUSProdSecondaryTransport::GetSecondaryTransportSegment

HRESULT CSegment::GetSecondaryTransportSegment( IUnknown **ppunkSegment )
{
	if( ppunkSegment == NULL )
	{
		return E_POINTER;
	}

	if( m_pIDMSegment == NULL )
	{
		return E_FAIL;
	}

	// Set the AudioPathConfig on the segment
	IDirectMusicSegment8P *pSegmentPrivate;
	if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **)&pSegmentPrivate ) ) )
	{
		// Try and get an AudioPathConfig object from the node
		IUnknown *pAudioPathConfig;
		if( (m_pIAudioPathNode == NULL)
		||	FAILED( m_pIAudioPathNode->GetObject( CLSID_DirectMusicAudioPathConfig, IID_IUnknown, (void**)&pAudioPathConfig ) ) )
		{
			pAudioPathConfig = NULL;
		}

		pSegmentPrivate->SetAudioPathConfig( pAudioPathConfig );
		pSegmentPrivate->Release();

		if( pAudioPathConfig )
		{
			pAudioPathConfig->Release();
		}
	}

	return m_pIDMSegment->QueryInterface( IID_IUnknown, (void**)ppunkSegment );
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdSecondaryTransport::GetSecondaryTransportName

HRESULT CSegment::GetSecondaryTransportName( BSTR* pbstrName )
{
	return GetName( pbstrName );
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdPropPageObject::GetData

HRESULT CSegment::GetData(void **ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL
	||	*ppData == NULL )
	{
		return E_POINTER;
	}

	ASSERT(m_pComponent != NULL);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	// Check if this is the boundary tab
	if( *((DWORD *)(*ppData)) == 2 )
	{
		// Yes - use the GetData method for it
		return GetData_Boundary( ppData );
	}

	m_PPGSegment.mtSegmentLength = m_mtLength;

	if( m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH )
	{
		m_PPGSegment.rtSegmentLength = m_rtLength;
	}
	else if( m_PPGSegment.pITimelineCtl )
	{
		// If not in reference time, convert
		m_PPGSegment.pITimelineCtl->ClocksToRefTime( m_mtLength, &m_PPGSegment.rtSegmentLength );
	}

	if ( m_PPGSegment.pITimelineCtl )
	{
		LONG lMeasure, lBeat;
		if( SUCCEEDED( m_PPGSegment.pITimelineCtl->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtLength, &lMeasure, NULL ) ) )
		{
			if( SUCCEEDED( m_PPGSegment.pITimelineCtl->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, lMeasure, 0, &lBeat ) ) )
			{
				if( lBeat < m_mtLength )
				{
					lMeasure++;
				}
			}
			// Ensure the segment is at least one measure long
			m_PPGSegment.dwMeasureLength = max( 1, lMeasure );
		}

		// Get extension and pickup bar length
		GetNumExtraBars( &(m_PPGSegment.dwExtraBars), &(m_PPGSegment.fPickupBar) );

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtPlayStart, &lMeasure, &lBeat ) ) )
		{
			m_PPGSegment.dwPlayStartMeasure = (DWORD) lMeasure;
			m_PPGSegment.bPlayStartBeat = (BYTE) lBeat;

			MUSIC_TIME mt;
			if( SUCCEEDED( m_PPGSegment.pITimelineCtl->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, lMeasure, 0, &mt ) ) )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					// Convert mt into an offset from the start of the measure
					mt = m_mtPlayStart - mt;

					// Convert mt into an offset from the start of the beat
					mt -= lBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ);

					// Assume mt is less than one beat long
					ASSERT( mt < NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) );

					m_PPGSegment.wPlayStartGrid = mt / (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
					m_PPGSegment.wPlayStartTick = mt - m_PPGSegment.wPlayStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
				}
				else
				{
					TRACE("CSegment::GetData: For Start Time conversion, unable to get a TimeSig at %d.\n", mt);
				}
			}
			else
			{
				TRACE("CSegment::GetData: Unable to convert from a Measure start time (%d) to a music value.\n", lMeasure);
			}
		}
		else
		{
			TRACE("CSegment::GetData: Unable to convert from m_mtPlayStart (%d) to a Measure/beat.\n", m_mtPlayStart );
			ASSERT(FALSE);
		}

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtLoopStart, &lMeasure, &lBeat ) ) )
		{
			m_PPGSegment.dwLoopStartMeasure = (DWORD) lMeasure;
			m_PPGSegment.bLoopStartBeat = (BYTE) lBeat;

			MUSIC_TIME mt;
			if( SUCCEEDED( m_PPGSegment.pITimelineCtl->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, lMeasure, 0, &mt ) ) )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					// Convert mt into an offset from the start of the measure
					mt = m_mtLoopStart - mt;

					// Convert mt into an offset from the start of the beat
					mt -= lBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ);

					// Assume mt is less than one beat long
					ASSERT( mt < NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) );

					m_PPGSegment.wLoopStartGrid = mt / (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
					m_PPGSegment.wLoopStartTick = mt - m_PPGSegment.wLoopStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
				}
				else
				{
					TRACE("CSegment::GetData: For Loop Start Time conversion, unable to get a TimeSig at %d.\n", mt);
				}
			}
			else
			{
				TRACE("CSegment::GetData: Unable to convert from a Measure start loop time (%d) to a music value.\n", lMeasure);
			}
		}
		else
		{
			TRACE("CSegment::GetData: Unable to convert from m_mtLoopStart (%d) to a Measure/beat.\n", m_mtLoopStart );
			ASSERT(FALSE);
		}

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtLoopEnd, &lMeasure, &lBeat ) ) )
		{
			m_PPGSegment.dwLoopEndMeasure = (DWORD) lMeasure;
			m_PPGSegment.bLoopEndBeat = (BYTE) lBeat;

			MUSIC_TIME mt;
			if( SUCCEEDED( m_PPGSegment.pITimelineCtl->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, lMeasure, 0, &mt ) ) )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					// Convert mt into an offset from the start of the measure
					mt = m_mtLoopEnd - mt;

					// Convert mt into an offset from the start of the beat
					mt -= lBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ);

					// Assume mt is less than one beat long
					ASSERT( mt < NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) );

					m_PPGSegment.wLoopEndGrid = mt / (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
					m_PPGSegment.wLoopEndTick = mt - m_PPGSegment.wLoopEndGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat);
				}
				else
				{
					TRACE("CSegment::GetData: For Loop End Time conversion, unable to get a TimeSig at %d.\n", mt);
				}
			}
			else
			{
				TRACE("CSegment::GetData: Unable to convert from a Measure End loop time (%d) to a music value.\n", lMeasure);
			}
		}
		else
		{
			TRACE("CSegment::GetData: Unable to convert from m_mtLoopEnd (%d) to a Measure/beat.\n", m_mtLoopEnd );
			ASSERT(FALSE);
		}
	}

	if ( m_PPGSegment.pITimelineCtl == NULL )
	{
		// BUGBUG: Set the times from the stored design-time data
	}
	// Segment properties
	//m_PPGSegment.strSegmentName;
	//m_PPGSegment.dwMeasureLength; // Should set from saved value
	//m_PPGSegment.wSegmentRepeats;
	m_PPGSegment.dwResolution	= m_dwResolution;
	//m_PPGSegment.dwTrackGroup
	m_PPGSegment.dwSegmentFlags = m_dwSegmentFlags;

	// Loop properties
	m_PPGSegment.dwLoopRepeats	= m_dwLoopRepeats;
    m_PPGSegment.rtLoopEnd = m_rtLoopEnd;
    m_PPGSegment.rtLoopStart = m_rtLoopStart;

	// Info properties
	//m_PPGSegment.strSubject
	//m_PPGSegment.strAuthor
	//m_PPGSegment.strCopyright
	//m_PPGSegment.wVersion1
	//m_PPGSegment.wVersion2
	//m_PPGSegment.wVersion3
	//m_PPGSegment.wVersion4
	//m_PPGSegment.strInformation

	// Set the flags
	m_PPGSegment.dwFlags = m_pIAudioPathNode ? PPGT_HAVEAUDIOPATH : 0;

	*ppData = &m_PPGSegment;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment UpdateSavedState

void CSegment::UpdateSavedState( BOOL &fStateSaved, int nUndoID )
{
	// Make sure the state is saved.
	if(!fStateSaved)
	{
		m_fInUndo = TRUE;

		// Load the undo text, if any
		CString strUndo;
		if( nUndoID )
		{
			strUndo.LoadString( nUndoID );
		}
		else
		{
			strUndo.Empty();
		}
		m_pUndoMgr->SaveState(this, strUndo, m_pComponent->m_pIFramework);
		m_fInUndo = FALSE;
		m_fModified = TRUE;
		fStateSaved = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdPropPageObject::SetData

HRESULT CSegment::SetData(void* pData)
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	BOOL			fRefresh = FALSE;	// used to force update of prop page if data not validated
	BOOL			fStateSaved = FALSE;
	PPGSegment		*pSegment;
	HRESULT			hr = S_OK;
	HWND			hwndOld;
	CString			strWarning;

	ASSERT(pData != NULL);
	if(pData == NULL)
	{
		return E_INVALIDARG;
	}

	// Make sure the undo mgr exists
	ASSERT(m_pUndoMgr != NULL);
	if(m_pUndoMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// Make sure everything else exists
	ASSERT(m_pComponent);
	ASSERT(m_pComponent->m_pIFramework != NULL);
	ASSERT(m_pComponent->m_pIConductor != NULL);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}
	if( m_pComponent->m_pIFramework == NULL || m_pComponent->m_pIConductor == NULL)
	{
		return E_UNEXPECTED;
	}

	// Check if this is the boundary tab
	if( *((DWORD *)pData) == 2 )
	{
		// Yes - use the SetData method for it
		return SetData_Boundary( pData );
	}

	pSegment = (PPGSegment *)pData;

	/////////////////////////////////////////////////
	// Set Segment Properties

	if( !(pSegment->dwFlags & PPGT_VALIDSEGMENT) )
	{
		goto LOOP;
	}

	// Set Segment name
	if( m_PPGSegment.strSegmentName != pSegment->strSegmentName )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_NAME );
		m_PPGSegment.strSegmentName = pSegment->strSegmentName;

		OnNameChange();
		m_fModified = TRUE;
	}

	// Compute the old and new millisecond length
	REFERENCE_TIME rtOldMillisecondLength;
	rtOldMillisecondLength = (m_PPGSegment.rtSegmentLength + MS_TO_REFTIME / 2) / MS_TO_REFTIME;
	REFERENCE_TIME rtNewMillisecondLength;
	rtNewMillisecondLength = (pSegment->rtSegmentLength + MS_TO_REFTIME / 2) / MS_TO_REFTIME;

	// If the length has changed and we aren't in another call of SetData, try to set it
	if(	!m_fSettingLength
	&&	( (m_dwSegmentFlags != pSegment->dwSegmentFlags) ||
		  (!(m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH) && (pSegment->dwMeasureLength != m_PPGSegment.dwMeasureLength)) ||
		  ( (m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH) && (rtOldMillisecondLength != rtNewMillisecondLength) ) ) )
	{
		m_fSettingLength = TRUE;

		// Always refresh the property page so the loop tab is kept up to date
		fRefresh = TRUE;

		// The segment is currently playing - we can't change the length.
		if( IsPlaying() )
		{
			hwndOld = GetFocus();
			strWarning.LoadString(IDS_ERR_LENGTHNOCHANGE);
			AfxMessageBox( strWarning );
			SetFocus(hwndOld);
			m_fSettingLength = FALSE;
			pSegment->dwMeasureLength = m_PPGSegment.dwMeasureLength;
			goto END;
		}

		// Save an undo state and mark the segment as modifed
		UpdateSavedState( fStateSaved, IDS_UNDO_LENGTH );
		m_fModified = TRUE;

		// Update the segment's flags
		m_dwSegmentFlags = pSegment->dwSegmentFlags;
		m_PPGSegment.dwSegmentFlags = pSegment->dwSegmentFlags;

		// Save old length
		const MUSIC_TIME mtOldLength = m_mtLength;

		// Setting reference time length
		if( m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH )
		{
			ASSERT( m_pSegmentDlg );
			if ( m_pSegmentDlg )
			{
				ASSERT( m_pSegmentDlg->m_pTimeline );
				if ( m_pSegmentDlg->m_pTimeline )
				{
					m_pSegmentDlg->m_pTimeline->RefTimeToClocks( pSegment->rtSegmentLength, &m_mtLength );
					m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtLength, (long *)(&pSegment->dwMeasureLength), NULL );
				}
			}

			m_PPGSegment.rtSegmentLength = pSegment->rtSegmentLength;
			m_rtLength = pSegment->rtSegmentLength;

			// Ensure the segment is at least one measure long
			m_PPGSegment.dwMeasureLength = max( 1, pSegment->dwMeasureLength );
		}
		// Setting musictime length
		else
		{
			/*
			// The user is trying to shorten the segment, warn that data may be lost
			if( (pSegment->dwMeasureLength < m_PPGSegment.dwMeasureLength)
			&&	(pSegment->dwFlags & PPGT_WARNUSER) )
			{
				// Store the window with the focus so it gets it back.
				hwndOld = GetFocus();
				strWarning.LoadString(IDS_WARN_MEASURESLOST);
				if(IDNO == MessageBox(NULL, LPCSTR(strWarning), NULL, MB_YESNO |
					MB_ICONEXCLAMATION | MB_TASKMODAL))
				{
					SetFocus(hwndOld);
					pSegment->dwFlags |= PPGT_WARNUSER;
					m_fSettingLength = FALSE;
					pSegment->dwMeasureLength = m_PPGSegment.dwMeasureLength;
					goto END;
				}
				SetFocus(hwndOld);
				// We need to set WARNUSER to FALSE again because SetFocus will set it to TRUE, 
				// but it really should be FALSE.
				pSegment->dwFlags &= ~PPGT_WARNUSER;
			}
			*/

			ASSERT( m_pSegmentDlg );
			if ( m_pSegmentDlg )
			{
				ASSERT( m_pSegmentDlg->m_pTimeline );
				if ( m_pSegmentDlg->m_pTimeline )
				{
					m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwMeasureLength, 0, &m_mtLength );
				}
			}
		}

		// Update m_pIDMSegment and the Timeline
		if( mtOldLength != m_mtLength )
		{
			// Update m_pIDMSegment and the Timeline
			OnLengthChanged( mtOldLength );
		}
		else
		{
			// Just update the segment's header
			SetSegmentHeaderChunk();
		}

		m_fSettingLength = FALSE;
	}

	// Should never happen
	/*
	// Set number of design-time repeats of the entire segment
	if( m_PPGSegment.wSegmentRepeats != pSegment->wSegmentRepeats )
	{
		//UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_REPEAT );
		m_fModified = TRUE;
		m_PPGSegment.wSegmentRepeats = pSegment->wSegmentRepeats;
		//BUGBUG: Modify current repeat count based on the difference.
	}
	*/

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		// Set the extension bar length
		if( m_PPGSegment.dwExtraBars != pSegment->dwExtraBars )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_EXTRABARS );
			m_fModified = TRUE;
			m_PPGSegment.dwExtraBars = pSegment->dwExtraBars;

			// Notify the StripMgrs that the extension length has changed
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_ExtensionLength, 0xFFFFFFFF, &(pSegment->dwExtraBars) );

			// Need to refresh the property page, in case the extension length was rejected by a stripmgr
			fRefresh = TRUE;
		}

		// Set the pickup bar length
		if( m_PPGSegment.fPickupBar != pSegment->fPickupBar )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_PICKUPBAR );
			m_fModified = TRUE;
			m_PPGSegment.fPickupBar = pSegment->fPickupBar;

			// Notify the StripMgrs that the pickup length has changed
			m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_PickupLength, 0xFFFFFFFF, &(pSegment->fPickupBar) );

			// Need to refresh the property page, in case the extension length was rejected by a stripmgr
			fRefresh = TRUE;
		}
	}

	// Set the default resolution
	if(m_dwResolution != pSegment->dwResolution)
	{
		if( (m_dwResolution & DMUS_SEGF_USE_AUDIOPATH) != (pSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH) )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_USE_AUDIOPATH );
		}
		else if( (m_dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) != (pSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_TIMESIG_ALWAYS );
		}
		else
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_RESOLUTION );
		}
		m_fModified = TRUE;
		m_dwResolution = pSegment->dwResolution;
		m_PPGSegment.dwResolution = pSegment->dwResolution;

		if(m_pIDMSegment != NULL)
		{
			m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		}
	}


	// Set the play start time.
	if( (m_PPGSegment.dwPlayStartMeasure != pSegment->dwPlayStartMeasure) ||
		(m_PPGSegment.bPlayStartBeat != pSegment->bPlayStartBeat) ||
		(m_PPGSegment.wPlayStartGrid != pSegment->wPlayStartGrid) ||
		(m_PPGSegment.wPlayStartTick != pSegment->wPlayStartTick) )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_PLAY_STARTTIME );
		m_fModified = TRUE;
		m_PPGSegment.dwPlayStartMeasure = pSegment->dwPlayStartMeasure;
		m_PPGSegment.bPlayStartBeat = pSegment->bPlayStartBeat;
		m_PPGSegment.wPlayStartGrid = pSegment->wPlayStartGrid;
		m_PPGSegment.wPlayStartTick = pSegment->wPlayStartTick;

		MUSIC_TIME mt;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwPlayStartMeasure, 0, &mt ) ) )
		{
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
			{
				m_mtPlayStart = mt + m_PPGSegment.bPlayStartBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
								m_PPGSegment.wPlayStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
								m_PPGSegment.wPlayStartTick;
				if(m_pIDMSegment != NULL)
				{
					m_pIDMSegment->SetStartPoint( m_mtPlayStart );
				}
			}
		}
	}

	// Set the track group we get the TimeSig from
	if( m_PPGSegment.dwTrackGroup != pSegment->dwTrackGroup)
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_TIMESIG_GROUP );
		m_fModified = TRUE;
		m_PPGSegment.dwTrackGroup = pSegment->dwTrackGroup;

		if( m_pComponent->m_pIPageManager)
		{
			m_pComponent->m_pIPageManager->RefreshData();
		}
	}

	/////////////////////////////////////////////////
	// Set Loop Properties
LOOP:
	if( !(pSegment->dwFlags & PPGT_VALIDLOOP) )
	{
		goto INFO;
	}

    if ((m_rtLoopStart != pSegment->rtLoopStart) || (m_rtLoopEnd != pSegment->rtLoopEnd)
        || ((m_dwSegmentFlags & DMUS_SEGIOF_REFLOOP) != (pSegment->dwSegmentFlags & DMUS_SEGIOF_REFLOOP)))
    {
        UpdateSavedState( fStateSaved, IDS_UNDO_CHANGE_CLOCK_LOOPING );
        m_rtLoopStart = pSegment->rtLoopStart;
        m_rtLoopEnd = pSegment->rtLoopEnd;
        m_dwSegmentFlags = pSegment->dwSegmentFlags;
        SetSegmentHeaderChunk();
    }
    
    // Set the number of repeats
	if(m_dwLoopRepeats != pSegment->dwLoopRepeats)
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_LOOP_REPEATS );
		m_fModified = TRUE;
		m_dwLoopRepeats = pSegment->dwLoopRepeats;

		if(m_pIDMSegment != NULL)
		{
			m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
		}
	}

	// Set the loop start time.
	if( (m_PPGSegment.dwLoopStartMeasure != pSegment->dwLoopStartMeasure) ||
		(m_PPGSegment.bLoopStartBeat != pSegment->bLoopStartBeat) ||
		(m_PPGSegment.wLoopStartGrid != pSegment->wLoopStartGrid) ||
		(m_PPGSegment.wLoopStartTick != pSegment->wLoopStartTick) )
	{
		// Only update Undo if nothing else has already changed
		UpdateSavedState( fStateSaved, IDS_UNDO_LOOP_STARTTIME );
		m_fModified = TRUE;
		m_PPGSegment.dwLoopStartMeasure = pSegment->dwLoopStartMeasure;
		m_PPGSegment.bLoopStartBeat = pSegment->bLoopStartBeat;
		m_PPGSegment.wLoopStartGrid = pSegment->wLoopStartGrid;
		m_PPGSegment.wLoopStartTick = pSegment->wLoopStartTick;

		if( m_pSegmentDlg 
		&&  m_pSegmentDlg->m_pTimeline )
		{
			MUSIC_TIME mt;
			if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwLoopStartMeasure, 0, &mt ) ) )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					m_mtLoopStart = mt + m_PPGSegment.bLoopStartBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
									m_PPGSegment.wLoopStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
									m_PPGSegment.wLoopStartTick;
					if( m_mtLoopStart >= m_mtLength )
					{
						m_mtLoopStart = m_mtLength - 1;
						ASSERT(FALSE);
						// Refresh the property page
						fRefresh = TRUE;
					}
					if(m_pIDMSegment != NULL)
					{
						m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
					}
				}
			}
		}
	}

	// Set the loop end time.
	if( (m_PPGSegment.dwLoopEndMeasure != pSegment->dwLoopEndMeasure) ||
		(m_PPGSegment.bLoopEndBeat != pSegment->bLoopEndBeat) ||
		(m_PPGSegment.wLoopEndGrid != pSegment->wLoopEndGrid) ||
		(m_PPGSegment.wLoopEndTick != pSegment->wLoopEndTick) )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_LOOP_ENDTIME );
		m_fModified = TRUE;
		m_PPGSegment.dwLoopEndMeasure = pSegment->dwLoopEndMeasure;
		m_PPGSegment.bLoopEndBeat = pSegment->bLoopEndBeat;
		m_PPGSegment.wLoopEndGrid = pSegment->wLoopEndGrid;
		m_PPGSegment.wLoopEndTick = pSegment->wLoopEndTick;

		if( m_pSegmentDlg 
		&&  m_pSegmentDlg->m_pTimeline )
		{
			MUSIC_TIME mt;
			if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwLoopEndMeasure, 0, &mt ) ) )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					m_mtLoopEnd = mt + m_PPGSegment.bLoopEndBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
									m_PPGSegment.wLoopEndGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
									m_PPGSegment.wLoopEndTick;
					if( m_mtLoopEnd > m_mtLength )
					{
						m_mtLoopEnd = m_mtLength;
						ASSERT(FALSE);
						// Refresh the property page
						fRefresh = TRUE;
					}
					if(m_pIDMSegment != NULL)
					{
						m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
					}
				}
			}
		}
	}

INFO:
	if( !(pSegment->dwFlags & PPGT_VALIDINFO) )
	{
		goto END;
	}

	// Set subject string
	if( m_PPGSegment.strSubject != pSegment->strSubject )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_SUBJECT );
		m_fModified = TRUE;
		m_PPGSegment.strSubject = pSegment->strSubject;
	}

	// Set author string
	if( m_PPGSegment.strAuthor != pSegment->strAuthor )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_AUTHOR );
		m_fModified = TRUE;
		m_PPGSegment.strAuthor = pSegment->strAuthor;
	}

	// Set copyright string
	if( m_PPGSegment.strCopyright != pSegment->strCopyright )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_COPYRIGHT );
		m_fModified = TRUE;
		m_PPGSegment.strCopyright = pSegment->strCopyright;
	}

	// Set version1 value
	if( m_PPGSegment.wVersion1 != pSegment->wVersion1 )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_VERSION );
		m_fModified = TRUE;
		m_PPGSegment.wVersion1 = pSegment->wVersion1;
	}

	// Set version2 value
	if( m_PPGSegment.wVersion2 != pSegment->wVersion2 )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_VERSION );
		m_fModified = TRUE;
		m_PPGSegment.wVersion2 = pSegment->wVersion2;
	}

	// Set version3 value
	if( m_PPGSegment.wVersion3 != pSegment->wVersion3 )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_VERSION );
		m_fModified = TRUE;
		m_PPGSegment.wVersion3 = pSegment->wVersion3;
	}

	// Set version4 value
	if( m_PPGSegment.wVersion4 != pSegment->wVersion4 )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_VERSION );
		m_fModified = TRUE;
		m_PPGSegment.wVersion4 = pSegment->wVersion4;
	}

	// Set information string
	if( m_PPGSegment.strInfo != pSegment->strInfo )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_INFO );
		m_fModified = TRUE;
		m_PPGSegment.strInfo = pSegment->strInfo;
	}

	// Set GUID
	if( memcmp( &m_PPGSegment.guidSegment, &pSegment->guidSegment, sizeof(GUID) ) )
	{
		UpdateSavedState( fStateSaved, IDS_UNDO_SEGMENT_GUID );
		m_fModified = TRUE;
		memcpy( &m_PPGSegment.guidSegment, &pSegment->guidSegment, sizeof(GUID) );

		// Notify connected nodes that Style GUID has changed
		m_pComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
	}

END:
	if( fRefresh )
	{
		ASSERT(m_pComponent);
		if(m_pComponent->m_pIPageManager)
		{
			m_pComponent->m_pIPageManager->RefreshData();
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CSegment::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( m_pComponent->m_pIPageManager )
	{
		m_pComponent->m_pIPageManager->SetObject(NULL);
	}
	m_fPropPageActive = FALSE;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTimelineCallback implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdTimelineCallback::OnDataChanged

HRESULT CSegment::OnDataChanged(/* [in] */ LPUNKNOWN punkStripMgr)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(punkStripMgr == NULL)
	{
		// Nothing changed, so no need to try and update
		return S_OK;
	}

	// Make sure the undo mgr exists
	ASSERT(m_pUndoMgr != NULL);
	if(m_pUndoMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	BOOL fFreezeUndo = FALSE;

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		VARIANT var;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
		{
			fFreezeUndo = V_BOOL(&var);
		}
	}

	if( fFreezeUndo == FALSE )
	{
		// BUGBUG: We should be smart here and only save the strip that changed to the Undo Manager.
		m_fInUndo = TRUE;

		// Ask the StripMgr for its undo text
		CString strUndoText;
		strUndoText.Empty();
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
		{
			BSTR bstrUndoText = NULL;
			if( SUCCEEDED( pStripMgr->GetParam( GUID_Segment_Undo_BSTR, 0, NULL, &bstrUndoText ) ) )
			{
				if( bstrUndoText )
				{
					strUndoText = bstrUndoText;
					::SysFreeString( bstrUndoText );
				}
			}
			pStripMgr->Release();
		}

		// Save the state, with the corresponding undo text
		m_pUndoMgr->SaveState(this, strUndoText, m_pComponent->m_pIFramework);
		m_fInUndo = FALSE;
	}

	HRESULT hr = S_FALSE;
	POSITION position;
	position = m_lstTracks.GetHeadPosition();
    while(position != NULL)
    {
		CTrack	*pTrack = m_lstTracks.GetNext(position);
		IDMUSProdStripMgr* pIStripMgr;
		pTrack->GetStripMgr( &pIStripMgr );

		IUnknown* punk;
		if ( pIStripMgr && SUCCEEDED( pIStripMgr->QueryInterface(IID_IUnknown, (void**) &punk) ) )
		{
			if ( punkStripMgr == punk )
			{
				IStream *pStreamCopy;

				ASSERT( m_pComponent != NULL );
				ASSERT( m_pComponent->m_pIFramework != NULL );

				if(SUCCEEDED(m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy )))
				{
					IPersistStream* pIPersistStream;
					if( SUCCEEDED( pIStripMgr->QueryInterface( IID_IPersistStream, (void**) &pIPersistStream ) ) )
					{
						if( SUCCEEDED( pIPersistStream->Save( pStreamCopy, TRUE ) ) )
						{
							m_fModified = TRUE;
							pTrack->SetStream( pStreamCopy );
							hr = S_OK;
						}
						pIPersistStream->Release();
					}
					pStreamCopy->Release();
				}

				// Update the track's group bits
				DMUS_IO_TRACK_HEADER ioTrackHeader;
				ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
				VARIANT varTrackHeader;
				varTrackHeader.vt = VT_BYREF;
				V_BYREF(&varTrackHeader) = &ioTrackHeader;
				;
				if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
				{
					TRACE("CSegment: Failed to get get StripMgr's GroupBits\n");
				}
				else
				{
					if( ioTrackHeader.dwGroup != pTrack->m_dwGroupBits )
					{
						UpdateTrackGroupBits( pTrack, pIStripMgr, ioTrackHeader.dwGroup );
					}
				}

				// Update the track's extras flags
				DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
				ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
				varTrackHeader.vt = VT_BYREF;
				V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;

				// Flag whether we need to update the functionbar or not
				BOOL fRefreshFunctionbar = FALSE;

				if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader ) ) )
				{
					//TRACE("CSegment: Failed to get get StripMgr's Track Extras flags\n");
				}
				else
				{
					// Check if the TrackExtras header has changed
					bool fChanged = false;
					if( ioTrackExtrasHeader.dwFlags != pTrack->m_dwTrackExtrasFlags )
					{
						// Check if the function bar needs to be redrawn.
						fRefreshFunctionbar = ((pTrack->m_dwTrackExtrasFlags & TRACKCONFIG_FLAGSTOCHECK)
											   != (ioTrackExtrasHeader.dwFlags & TRACKCONFIG_FLAGSTOCHECK));

						// It changed - update the bits
						pTrack->m_dwTrackExtrasFlags = ioTrackExtrasHeader.dwFlags;
						fChanged = true;
					}
					// Check if the TrackExtras priority has changed
					if( ioTrackExtrasHeader.dwPriority != pTrack->m_dwTrackExtrasPriority )
					{
						// It changed - update the bits
						pTrack->m_dwTrackExtrasPriority = ioTrackExtrasHeader.dwPriority;
						fChanged = true;
					}
					if( fChanged )
					{
						// Update the state of the track in the segment
						UpdateTrackConfig( pTrack );
					}
				}

				// Update the track's PRODUCER_ONLY flags
				IOProducerOnlyChunk ioProducerOnlyChunk;
				ZeroMemory( &ioProducerOnlyChunk, sizeof( IOProducerOnlyChunk ) );
				varTrackHeader.vt = VT_BYREF;
				V_BYREF(&varTrackHeader) = &ioProducerOnlyChunk;

				if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_PRODUCERONLY_FLAGS, &varTrackHeader ) ) )
				{
					//TRACE("CSegment: Failed to get get StripMgr's PRODUCER_ONLY flags\n");
				}
				else
				{
					// Check if the ProducerOnly flags have changed
					if( ioProducerOnlyChunk.dwProducerOnlyFlags != pTrack->m_dwProducerOnlyFlags )
					{
						// Flag that the function bar needs to be redrawn.
						fRefreshFunctionbar = TRUE;

						// It changed - update the bits
						pTrack->m_dwProducerOnlyFlags = ioProducerOnlyChunk.dwProducerOnlyFlags;
					}
				}

				// Redraw the strips for this StripMgr, if necessary
				if( fRefreshFunctionbar && m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
				{
					VARIANT varFunctionbarWidth;
					m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &varFunctionbarWidth );

					long lLeftDisplay;
					m_pSegmentDlg->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
					m_pSegmentDlg->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

					RECT rectToInvalidate;
					rectToInvalidate.left = lLeftDisplay - V_I4(&varFunctionbarWidth);
					rectToInvalidate.right = lLeftDisplay;
					rectToInvalidate.top = 0;
					rectToInvalidate.bottom = 0x10000000;

					DWORD dwIndex = 0;
					IDMUSProdStrip *pStrip;
					while( m_pSegmentDlg->m_pTimeline->EnumStrip( dwIndex, &pStrip ) == S_OK )
					{
						// Get the strip's strip manager
						if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varTrackHeader ) ) 
						&&	V_UNKNOWN(&varTrackHeader) )
						{
							// Get an IDMUSProdStripMgr interface
							IDMUSProdStripMgr *pStripMgr;
							if( SUCCEEDED( V_UNKNOWN(&varTrackHeader)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
							{
								if( pIStripMgr == pStripMgr )
								{
									m_pSegmentDlg->m_pTimeline->StripInvalidateRect( pStrip, &rectToInvalidate, FALSE );
								}
								pStripMgr->Release();
							}
							V_UNKNOWN(&varTrackHeader)->Release();
						}
						pStrip->Release();
						dwIndex++;
					}
				}

				pIStripMgr->Release();
				punk->Release();
				break;
			}
			punk->Release();
		}
		if( pIStripMgr )
		{
			pIStripMgr->Release();
		}
	}

	// HACKHACK: Modify the segment to clear its internal PChannel array.
	/*
	if( m_pIDMSegment8 )
	{
		void *pCSegment;
		if( SUCCEEDED( m_pIDMSegment8->QueryInterface( IID_CSegment, &pCSegment ) ) )
		{
			DWORD baseAddress = (DWORD)pCSegment;
			DWORD *pdwPChannels = (DWORD *)(baseAddress + 0x68);
			DWORD **padwPChannels = (DWORD **)(baseAddress + 0x6c);

			*pdwPChannels = 0;
			if( *padwPChannels != NULL )
			{
				delete [] *padwPChannels;
				*padwPChannels = NULL;
			}
			m_pIDMSegment8->Release();
		}

		// Re-initialize all the tracks
		POSITION posTrack = m_lstTracks.GetHeadPosition();
		while( posTrack )
		{
			// Get a pointer to each track
			CTrack *pTrack = m_lstTracks.GetNext( posTrack );

			// Get a pointer to each DirectMusic track
			IDirectMusicTrack *pIDMTrack = NULL;
			pTrack->GetDMTrack( &pIDMTrack );
			if( pIDMTrack )
			{
				// Insert the track
				pIDMTrack->Init( m_pIDMSegment );
				pIDMTrack->Release();
			}
		}
	}
	*/

	// Fix 32337: Release, then recreate the segment and reload it
	if( m_pIDMSegment8 )
	{
		IDirectMusicObjectP* pIDMObjectP;
		if( SUCCEEDED ( m_pIDMSegment8->QueryInterface( IID_IDirectMusicObjectP, (void**)&pIDMObjectP ) ) )
		{
			pIDMObjectP->Zombie();
			RELEASE( pIDMObjectP );
		}

		m_pIDMSegment8->Release();
		m_pIDMSegment8 = NULL;
	}

	if( m_pIDMSegment )
	{
		m_pIDMSegment->Release();
		m_pIDMSegment = NULL;
	}

	::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicSegment, (void**)&m_pIDMSegment );
	if( m_pIDMSegment )
	{
		m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8, (void**)&m_pIDMSegment8 );

		// Set the segment parameters
		m_pIDMSegment->SetRepeats( m_dwLoopRepeats );
		m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		m_pIDMSegment->SetLength( m_mtLength );
		m_pIDMSegment->SetStartPoint( m_mtPlayStart );
		m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );

		// Update the segment header chunk
		SetSegmentHeaderChunk();

		// Set the tool graph
		if( m_pIToolGraphNode )
		{
			IDirectMusicGraph* pIGraph;
			if( SUCCEEDED ( m_pIToolGraphNode->GetObject( CLSID_DirectMusicGraph, IID_IDirectMusicGraph, (void**)&pIGraph ) ) )
			{
				m_pIDMSegment->SetGraph( pIGraph );
				pIGraph->Release();
			}

			AddSegmentToGraphUserList();
		}

		// Re-insert all the tracks
		POSITION posTrack = m_lstTracks.GetHeadPosition();
		while( posTrack )
		{
			// Get a pointer to each track
			CTrack *pTrack = m_lstTracks.GetNext( posTrack );

			// Get a pointer to each DirectMusic track
			IDirectMusicTrack *pIDMTrack = NULL;
			pTrack->GetDMTrack( &pIDMTrack );
			if( pIDMTrack )
			{
				// Insert the track
				m_pIDMSegment->InsertTrack( pIDMTrack, pTrack->m_dwGroupBits );
				pIDMTrack->Release();

				UpdateTrackConfig( pTrack );
			}
		}

		// Set the AudioPathConfig on the segment
		IDirectMusicSegment8P *pSegmentPrivate;
		if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **)&pSegmentPrivate ) ) )
		{
			// Try and get an AudioPathConfig object from the node
			IUnknown *pAudioPathConfig;
			if( (m_pIAudioPathNode == NULL)
			||	FAILED( m_pIAudioPathNode->GetObject( CLSID_DirectMusicAudioPathConfig, IID_IUnknown, (void**)&pAudioPathConfig ) ) )
			{
				pAudioPathConfig = NULL;
			}

			pSegmentPrivate->SetAudioPathConfig( pAudioPathConfig );
			pSegmentPrivate->Release();

			if( pAudioPathConfig )
			{
				pAudioPathConfig->Release();
			}
		}
	}

	// Notify connected nodes to sync the updated segment
	// Necessary since we have just CoCreated a new m_pIDMSegment
	m_pComponent->m_pIFramework->NotifyNodes( this, DOCROOT_SyncDirectMusicObject, NULL );

	return hr;
}

///////////////////////////////////////////////////////////////////////////
// CSegment :: UpdateTrackGroupBits

void CSegment::UpdateTrackGroupBits( CTrack *pTrack, IDMUSProdStripMgr* pIStripMgr, DWORD dwGroupBits )
{
	pTrack->m_dwGroupBits = dwGroupBits;

	// Update DirectMusic
	if( m_pIDMSegment )
	{
		IDirectMusicTrack *pIDMTrack = NULL;
		pTrack->GetDMTrack( &pIDMTrack );
		if( pIDMTrack )
		{
			m_pIDMSegment->RemoveTrack( pIDMTrack );
			m_pIDMSegment->InsertTrack( pIDMTrack, dwGroupBits );
			pIDMTrack->Release();

			UpdateTrackConfig( pTrack );
		}
	}

	// Update the Timeline
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		m_pSegmentDlg->m_pTimeline->RemoveStripMgr( pIStripMgr );
		m_pSegmentDlg->m_pTimeline->AddStripMgr( pIStripMgr, dwGroupBits );

		// The above RemoveStripMgr caused the track group property page to disappear.
		// Now, enumerate through all strips to find the one to display the property page of.
		DWORD dwEnum = 0;
		IDMUSProdStrip* pIStrip;
		IDMUSProdStripMgr* pITmpStripMgr;
		while( m_pSegmentDlg->m_pTimeline->EnumStrip( dwEnum, &pIStrip ) == S_OK )
		{
			ASSERT( pIStrip );
			dwEnum++;
			VARIANT varStripMgr;
			if( SUCCEEDED( pIStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
			&& (V_UNKNOWN(&varStripMgr) != NULL) )
			{
				if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pITmpStripMgr ) ) )
				{
					if( pITmpStripMgr == pIStripMgr )
					{
						BOOL fSucceeded = FALSE;
						IDMUSProdPropPageObject* pIDMUSProdPropPageObject;

						// Try displaying the strip's property page
						if( SUCCEEDED( pIStrip->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIDMUSProdPropPageObject ) ) )
						{
							if( SUCCEEDED( pIDMUSProdPropPageObject->OnShowProperties() ) )
							{
								fSucceeded = TRUE;
							}
							pIDMUSProdPropPageObject->Release();
						}

						// If that failed, try displaying the stripmgr's property page
						if( !fSucceeded )
						{
							if( SUCCEEDED( pITmpStripMgr->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIDMUSProdPropPageObject ) ) )
							{
								pIDMUSProdPropPageObject->OnShowProperties();
								pIDMUSProdPropPageObject->Release();
							}
						}
					}
					pITmpStripMgr->Release();
				}
				V_UNKNOWN(&varStripMgr)->Release();
			}
			pIStrip->Release();
		}
	}

	// Update position in our track list
	POSITION pos = m_lstTracks.Find( pTrack );
	if( pos )
	{
		m_lstTracks.RemoveAt( pos );
		InsertTrackAtDefaultPos( pTrack );
	}
}

///////////////////////////////////////////////////////////////////////////
// CSegment :: OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)

HRESULT CSegment::OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	if( pIDocRootNode == m_pIContainerNode )
	{
		// CONTAINER_ChangeNotification
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_ChangeNotification ) )
		{
			SetModifiedFlag( TRUE );
			// No need to sync segment with DMusic
			// because containers do not affect performance
			return S_OK;
		}

		// CONTAINER_FileLoadFinished
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_FileLoadFinished ) )
		{
			// No need to sync segment with DMusic
			// because containers do not affect performance
			return S_OK;
		}
	}
	else if( pIDocRootNode == m_pIAudioPathNode )
	{
		// AUDIOPATH_DirtyNotification
		if( ::IsEqualGUID(guidUpdateType, AUDIOPATH_DirtyNotification ) )
		{
			SetModifiedFlag( TRUE );
			// TODO:
			//SyncSegmentWithDirectMusic();
			return S_OK;
		}
	}
	else if( pIDocRootNode == m_pIToolGraphNode )
	{
		// TOOLGRAPH_ChangeNotification
		if( ::IsEqualGUID(guidUpdateType, TOOLGRAPH_ChangeNotification ) )
		{
			SetModifiedFlag( TRUE );
			// No need to sync segment with DMusic
			// because segment uses the IDirectMusicGraph pointer
			// retrieved from m_pIToolGraphNode and the toolgraph handles the sync
			return S_OK;
		}
	}

	else if( pIDocRootNode == this )
	{
		// Notification is from this Segment node
		if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileNameChange ) 
		||  ::IsEqualGUID( guidUpdateType, FRAMEWORK_AfterFileSave ) )
		{
			OnNameChange();
			hr = S_OK;
		}
	}

	// Prepare DMUSProdFrameworkMsg structure
	DMUSProdFrameworkMsg fm;
	fm.guidUpdateType = guidUpdateType;
	fm.punkIDMUSProdNode = pIDocRootNode;
	fm.pData = pData;

	// Notify the strip managers
	if( m_pSegmentDlg
	&&  m_pSegmentDlg->m_pTimeline )
	{
		hr = m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_FrameworkMsg, 0xFFFFFFFF, &fm );
	}
	else
	{
		hr = NotifyAllStripMgrs( GUID_Segment_FrameworkMsg, 0xFFFFFFFF, &fm );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////
// CSegment :: NotifyAllStripMgrs

HRESULT CSegment::NotifyAllStripMgrs( GUID guidUpdateType, DWORD dwGroupBits, VOID* pData )
{
	CTrack* pTrack;
	IDMUSProdStripMgr* pIStripMgr;

	// The dialog is not open, otherwise we should use the Timeline.
	ASSERT( !m_pSegmentDlg || !m_pSegmentDlg->m_pTimeline );

	// Freeze undo queue - there is not Timeline available, if this method has been called, so we can't do this
//	VARIANT var;
//	var.vt = VT_BOOL;
//	V_BOOL(&var) = TRUE;
//	SetTimelineProperty( TP_FREEZE_UNDO, var );

	POSITION pos = m_lstTracks.GetHeadPosition();
    while( pos )
    {
		pTrack = m_lstTracks.GetNext( pos );

		pTrack->GetStripMgr( &pIStripMgr );
		if( pIStripMgr )
		{
			pIStripMgr->OnUpdate( guidUpdateType, dwGroupBits, pData );
			pIStripMgr->Release();
		}
	}

	// Restore undo queue - there is not Timeline available, if this method has been called, so we can't do this
//	var.vt = VT_BOOL;
//	V_BOOL(&var) = FALSE;
//	SetTimelineProperty( TP_FREEZE_UNDO, var );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment additional functions

/////////////////////////////////////////////////////////////////////////////
// CSegment::ReadTemplate

HRESULT CSegment::ReadTemplate( IStream* pIStream, CTemplateStruct *pTemplateStruct, long lRecSize )
{
	long			lBufSize;
	char			*szBuffer;

	_LARGE_INTEGER	liTemp;

	ULONG			ulBytesRead;
	HRESULT			hr;

	lBufSize = pTemplateStruct->GetSize();
	szBuffer = new char[lBufSize];
	if(!szBuffer) {
		return E_OUTOFMEMORY;
	}

	if (lRecSize > lBufSize)
	{
		hr = pIStream->Read(szBuffer, (UINT)lBufSize, &ulBytesRead);
		if(FAILED(hr))
		{
			delete [] szBuffer;
			return E_FAIL;
		}
		else if(hr == S_FALSE || ulBytesRead != (ULONG) lBufSize)
		{
			delete [] szBuffer;
			return S_FALSE;
		}
		liTemp.LowPart = lRecSize - lBufSize;
		liTemp.HighPart = 0;
    	pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
    }
    else
    {
		memset((void *) szBuffer,0,(UINT)lBufSize);
		hr = pIStream->Read(szBuffer, lRecSize, &ulBytesRead);
		if(FAILED(hr))
		{
			delete [] szBuffer;
			return E_FAIL;
		}
		else if(hr == S_FALSE || ulBytesRead != (ULONG) lRecSize)
		{
			delete [] szBuffer;
			return S_FALSE;
		}
    }
	hr = pTemplateStruct->StringToStruct(szBuffer, ulBytesRead);
	delete [] szBuffer;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::RegisterWithTransport()

HRESULT CSegment::RegisterWithTransport()
{
	if( m_fTransportRegistered )
	{
		return S_OK;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIConductor != NULL );
	if ( !m_pComponent || !m_pComponent->m_pIConductor )
	{
		return E_UNEXPECTED;
	}

	// Register Segment with Conductor's Secondary segment Transport system
	if( FAILED( m_pComponent->m_pIConductor->RegisterSecondaryTransport( this ) ) )
	{
		return E_FAIL;
	}

	// Register Segment with Conductor's Transport and notification system
	if( FAILED( m_pComponent->m_pIConductor->RegisterTransport( this, 0 ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( m_pComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_SEGMENT ) ) )
	{
		return E_FAIL;
	}
	m_pComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_RECOMPOSE );

	m_fTransportRegistered = TRUE;
	return S_OK;
 }

/////////////////////////////////////////////////////////////////////////////
// CSegment::UnRegisterWithTransport()

HRESULT CSegment::UnRegisterWithTransport()
{
	if(m_fTransportRegistered == FALSE)
	{
		return S_OK;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIConductor != NULL );
	if ( !m_pComponent || !m_pComponent->m_pIConductor )
	{
		return E_UNEXPECTED;
	}

	// Make sure Segment is not playing - we don't care about the return result
	Stop( TRUE );

	HRESULT hr = S_OK;

	// Unregister Segment with Conductor's Secondary segment Transport system
	if( FAILED( m_pComponent->m_pIConductor->UnRegisterSecondaryTransport( this ) ) )
	{
		hr = E_FAIL; // Don't return early - all these calls need to happen
	}

	// Unregister the Transport's Play, Stop, Transition functionality.
	if( FAILED( m_pComponent->m_pIConductor->UnRegisterTransport( this ) ) )
	{
		hr = E_FAIL; // Don't return early - all these calls need to happen
	}

	// Unregister notification messages
	if( FAILED( m_pComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_SEGMENT ) ) )
	{
		hr = E_FAIL; // Don't return early - all these calls need to happen
	}
	m_pComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_RECOMPOSE );

	if( SUCCEEDED( hr ) )
	{
		m_fTransportRegistered = FALSE;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::SetDialog()

void CSegment::SetDialog(CSegmentDlg *pDialog)
{
	m_pSegmentDlg = pDialog;

	// If the editor is not open then we're done.
	if(!m_pSegmentDlg)
	{
		RemoveDialog();
		return;
	}

	// If this dialog is already playing, then Enable the Dialog's Timer.
	if( IsPlaying() )
	{
		EnableDialogTimer(TRUE);
	}

	if ( m_PPGSegment.pITimelineCtl )
	{
		m_PPGSegment.pITimelineCtl->Release();
		m_PPGSegment.pITimelineCtl = NULL;
	}

	ASSERT( m_pSegmentDlg->m_pTimeline );
	m_PPGSegment.pITimelineCtl = m_pSegmentDlg->m_pTimeline;
	m_PPGSegment.pITimelineCtl->AddRef();

	ASSERT( m_pComponent );
	if( m_pComponent->m_pIPageManager )
	{
		m_pComponent->m_pIPageManager->RefreshData();
	}

	// Make sure we're using the correct MusicTime length, if we're in clocktime
	UpdateConductorTempo();

	// Always set our measure length.
	long lMeasure;
	if( SUCCEEDED( m_PPGSegment.pITimelineCtl->ClocksToMeasureBeat( 0xFFFFFFFF, 0, m_mtLength, &lMeasure, NULL ) ) )
	{
		// Ensure the segment is at least one measure long
		m_PPGSegment.dwMeasureLength = max( 1, lMeasure );
	}

	// Need to update the property page, since the number of measures may have changed.
	if( m_pComponent->m_pIPageManager )
	{
		m_pComponent->m_pIPageManager->RefreshData();
	}

	// Get extension and pickup bar length
	GetNumExtraBars( &(m_PPGSegment.dwExtraBars), &(m_PPGSegment.fPickupBar) );
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::RemoveDialog()

void CSegment::RemoveDialog( void )
{
	m_pSegmentDlg = NULL;
	ASSERT( m_pComponent );
	if ( m_PPGSegment.pITimelineCtl )
	{
		m_PPGSegment.pITimelineCtl->Release();
		m_PPGSegment.pITimelineCtl = NULL;
	}
	if( m_pComponent->m_pIPageManager )
	{
		m_pComponent->m_pIPageManager->RefreshData();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegment::EnableDialogTimer()

void CSegment::EnableDialogTimer(BOOL fEnable) const
{
	// Caller must have called CSegment::SetDialog() prior to EnableDialogTimer()
	if(!m_pSegmentDlg)
	{
		return;
	}

	m_pSegmentDlg->EnableTimer( fEnable && m_fTrackTimeCursor );
}

/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdPropPageObject::OnShowProperties

HRESULT CSegment::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Show the Segment properties
	IDMUSProdPropSheet* pIPropSheet = NULL;

	if( FAILED ( hr = m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) )
	||	(pIPropSheet == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//  If the property sheet is hidden, exit
	// ????
	if( pIPropSheet->IsShowing() != S_OK )
	{
		pIPropSheet->Release();
		return S_OK;
	}

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pComponent->m_pIPageManager != NULL );
		pIPropSheet->Release();
		return S_OK;
	}

	//hr = pIPropSheet->Show( TRUE );
	if( SUCCEEDED ( hr = pIPropSheet->SetPageManager(m_pComponent->m_pIPageManager) ) )
	{
		short nActiveTab = CSegmentPPGMgr::sm_nActiveTab;

		hr = m_pComponent->m_pIPageManager->SetObject( this );
		m_fPropPageActive = TRUE;
		pIPropSheet->SetActivePage( nActiveTab ); 
	}

	pIPropSheet->Release();

	return hr;
}


////////////////////////////////// Additional Segment helpers
//

BOOL CSegment::IsPlaying()
{
	BOOL fResult = FALSE;

	if( m_pComponent->m_pIDMPerformance )
	{
		EnterCriticalSection( &m_csSegmentState );

		POSITION pos = m_lstSegStates.GetHeadPosition();
		while( pos )
		{
			SegStateParams *pSegStateParams = m_lstSegStates.GetNext( pos );
			fResult |= ( m_pComponent->m_pIDMPerformance->IsPlaying( NULL, pSegStateParams->pSegState ) == S_OK ); 
		}

		LeaveCriticalSection( &m_csSegmentState );
	}

	return fResult;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNotifyCPt implementation

/////////////////////////////////////////////////////////////////////////////
// CSegment::OnNotify

HRESULT CSegment::OnNotify( ConductorNotifyEvent *pConductorNotifyEvent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pConductorNotifyEvent != NULL );
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIConductor != NULL );

	switch( pConductorNotifyEvent->m_dwType )
	{
		case DMUS_PMSGT_NOTIFICATION:
		{
			DMUS_NOTIFICATION_PMSG* pNotifyEvent = (DMUS_NOTIFICATION_PMSG *)pConductorNotifyEvent->m_pbData;

			TRACE("SegmentNotification %d for State %x at time %d\n", pNotifyEvent->dwNotificationOption, pNotifyEvent->punkUser, pNotifyEvent->mtTime );

			EnterCriticalSection( &m_csSegmentState );

			SegStateParams *pSegStateParams = NULL;
			IDirectMusicSegmentState *pNotifySegState = NULL;
			if( pNotifyEvent->punkUser
			&&	SUCCEEDED( pNotifyEvent->punkUser->QueryInterface( IID_IDirectMusicSegmentState, (void **)&pNotifySegState ) ) )
			{
				POSITION pos = m_lstSegStates.GetHeadPosition();
				while( pos )
				{
					SegStateParams *pTmpSegStateParams = m_lstSegStates.GetNext( pos );
					if( pNotifySegState == pTmpSegStateParams->pSegState )
					{
						pSegStateParams = pTmpSegStateParams;
						break;
					}
				}

			// Handle GUID_NOTIFICATION_SEGMENT notifications if they are for our segment state
			if( pSegStateParams
			&&	::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
			{
				switch( pNotifyEvent->dwNotificationOption )
				{
					case DMUS_NOTIFICATION_SEGSTART:
						m_rtCurrentStartTime = pNotifyEvent->rtTime;
						m_mtCurrentStartTime = pNotifyEvent->mtTime;
						m_mtCurrentStartPoint = 0;
						pNotifySegState->GetStartPoint( &m_mtCurrentStartPoint );
						m_mtCurrentLoopStart = pSegStateParams->mtLoopStart;
						m_mtCurrentLoopEnd = pSegStateParams->mtLoopEnd;
                        m_rtCurrentLoopStart = m_rtLoopStart;
                        m_rtCurrentLoopEnd = m_rtLoopEnd;
						m_dwCurrentMaxLoopRepeats = pSegStateParams->dwMaxLoopRepeats;

						if( m_pIDMCurrentSegmentState )
						{
							m_pIDMCurrentSegmentState->Release();
						}

						m_pIDMCurrentSegmentState = pNotifySegState;
						m_pIDMCurrentSegmentState->AddRef();

						// If the dialog exists, notify it that we started playing
						if( m_pSegmentDlg )
						{
							::PostMessage( m_pSegmentDlg->m_hWnd, WM_APP, pNotifyEvent->dwNotificationOption, pNotifyEvent->mtTime );
						}
						break;

					case DMUS_NOTIFICATION_SEGABORT:
						// Fix 29099: When the primary segment receives a SEGABORT notification, and Ctrl+click was not used to hit the stop button, call pDMPerf->Stop( 0, 0, 0, 0 ) to ensure that everything in the performance stops.
						if( !m_fCtrlKeyDownWhenStopCalled
						&&	m_pComponent && m_pComponent->m_pIDMPerformance )
						{
							// Only stop if there is no primary segment playing
							MUSIC_TIME mtNow;
							IDirectMusicSegmentState *pSegState = NULL;
							if( SUCCEEDED( m_pComponent->m_pIDMPerformance->GetTime( NULL, &mtNow ) )
							&&	FAILED( m_pComponent->m_pIDMPerformance->GetSegmentState( &pSegState, mtNow ) ) )
							{
								m_pComponent->m_pIDMPerformance->Stop( NULL, NULL, 0, 0 );
							}

							if( pSegState )
							{
								pSegState->Release();
							}
						}

						// Save the current start point and start time, etc.
						{
							MUSIC_TIME mtStartPoint = 0;
							MUSIC_TIME mtStartTime = 0;
							pNotifySegState->GetStartPoint( &mtStartPoint );
							pNotifySegState->GetStartTime( &mtStartTime );
							if( mtStartTime > m_mtCurrentStartTime )
							{
								m_mtCurrentStartTime = mtStartTime;
                                m_rtCurrentStartTime = 0;
                                if (m_pComponent && m_pComponent->m_pIDMPerformance)
                                {
                                    m_pComponent->m_pIDMPerformance->MusicToReferenceTime(mtStartTime,&m_rtCurrentStartTime);
                                }
								m_mtCurrentStartPoint = mtStartPoint;
								m_mtCurrentLoopStart = pSegStateParams->mtLoopStart;
								m_mtCurrentLoopEnd = pSegStateParams->mtLoopEnd;
                                m_rtCurrentLoopStart = m_rtLoopStart;
                                m_rtCurrentLoopEnd = m_rtLoopEnd;
								m_dwCurrentMaxLoopRepeats = pSegStateParams->dwMaxLoopRepeats;
							}
						}

					case DMUS_NOTIFICATION_SEGEND:
						m_rtCurrentStartTime = 0;

						// If the dialog exists, notify it that we stopped playing
						if( m_pSegmentDlg )
						{
							::PostMessage( m_pSegmentDlg->m_hWnd, WM_APP, pNotifyEvent->dwNotificationOption, pNotifyEvent->mtTime );
						}

						// Notify the Conductor we stopped playing
						if( m_pComponent && m_pComponent->m_pIConductor )
						{
							m_pComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*) this );
						}

						m_fCtrlKeyDownWhenStopCalled = FALSE;

						RELEASE( m_pIDMTransitionSegment );

						if( pNotifySegState == m_pIDMCurrentSegmentState )
						{
							RELEASE( m_pIDMCurrentSegmentState );
						}

						// Remove all Segment States that started before the time of this notification
						POSITION pos = m_lstSegStates.GetHeadPosition();
						while( pos )
						{
							POSITION posToRemove = pos;
							SegStateParams *pTmpSegStateParams = m_lstSegStates.GetNext( pos );
							if( pTmpSegStateParams == pSegStateParams )
							{
								// If we have an audiopath, notify it that we stopped playing
								RegisterSegementStateWithAudiopath( FALSE, pNotifySegState );

								m_lstSegStates.RemoveAt( posToRemove );
								delete pSegStateParams;
								pSegStateParams = NULL;
							}
							else
							{
								// Work-around 38773: DMusic: DMIME: No notifications sent for segments that are "dequeued".
								MUSIC_TIME mtStart;
								if( SUCCEEDED( pTmpSegStateParams->pSegState->GetStartTime( &mtStart ) ) )
								{
									if( mtStart < pNotifyEvent->mtTime )
									{
										// If we have an audiopath, notify it that we stopped playing
										RegisterSegementStateWithAudiopath( FALSE, pTmpSegStateParams->pSegState );

										m_lstSegStates.RemoveAt( posToRemove );
										delete pTmpSegStateParams;
									}
								}
							}
						}

						break;
				}
			}
			// Handle recompose notifications
			else if( pSegStateParams
				 &&  ::IsEqualGUID( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_RECOMPOSE ) )
			{
				DMUS_PMSG* pNewNotifyEvent;
				if( SUCCEEDED( m_pComponent->m_pIDMPerformance->ClonePMsg( reinterpret_cast<DMUS_PMSG *>(pNotifyEvent), &pNewNotifyEvent ) ) )
				{
					// This recompose is for our segment - handle it
					::PostMessage( m_wndNotificationHandler.GetSafeHwnd(), WM_APP, 0, LPARAM(pNewNotifyEvent) );
				}
			}
				pNotifySegState->Release();
			}

			LeaveCriticalSection( &m_csSegmentState );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::Compose

HRESULT CSegment::Compose( DWORD dwFlags, DWORD dwActivity )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDMSegment );
	if( !m_pIDMSegment )
	{
		return E_UNEXPECTED;
	}

	IDirectMusicComposer8P *pIDMComposer;
	::CoCreateInstance( CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicComposer8P, (void**)&pIDMComposer );
	if( !pIDMComposer )
	{
		return E_NOINTERFACE;
	}

	// Try and find a SignPostTrack
	IDirectMusicTrack *pDMSignPostTrack;
	if( FAILED( m_pIDMSegment->GetTrack( CLSID_DirectMusicSignPostTrack, 0xFFFFFFFF, 0, &pDMSignPostTrack) ) )
	{
		ASSERT(FALSE);
		TRACE("CSegment: Whoops, can't find a SignPost track in a template.\n");
		pIDMComposer->Release();
		return E_UNEXPECTED;
	}

	/* Not necessary
	// Get the group bits for the signpost track
	DWORD dwSignPostGroupBits = 0;
	m_pIDMSegment->GetTrackGroup( pDMSignPostTrack, &dwSignPostGroupBits );
	*/

	pDMSignPostTrack->Release();
	pDMSignPostTrack = NULL;

	HRESULT hr = S_OK;

	/* Not necessary
	// Close the existing segment
	IDirectMusicSegment *pIDMComposedSegment = NULL;
	if( SUCCEEDED( m_pIDMSegment->Clone( 0, 0, &pIDMComposedSegment ) ) )
	{
		// Remove the existing ChordTrack, if any
		IDirectMusicTrack* pIDMTrack = NULL;
		if( SUCCEEDED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicChordTrack, dwSignPostGroupBits, 0, &pIDMTrack ) ) )
		{
			pIDMComposedSegment->RemoveTrack( pIDMTrack );
			pIDMTrack->Release();
			pIDMTrack = NULL;
		}

		// Compose the segment in-place
		dwFlags &= ~DMUS_COMPOSE_TEMPLATEF_CLONE;
		hr = pIDMComposer->ComposeSegmentFromTemplateEx( NULL, pIDMComposedSegment, dwFlags, dwActivity, NULL, &pIDMComposedSegment );
		if( FAILED( hr ) )
		{
			if( pIDMComposedSegment )
			{
				pIDMComposedSegment->Release();
			}
			return hr;
		}

		if( FAILED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicChordTrack, dwSignPostGroupBits, 0, &pIDMTrack ) ) )
		{
			pIDMComposedSegment->Release();
			return S_FALSE;
		}
		pIDMTrack->Release();
		RELEASE( pIDMComposedSegment );
	}
	*/

	// Compose the segment in-place
	dwFlags &= ~DMUS_COMPOSE_TEMPLATEF_CLONE;

	// Compose a segment with a new Chord Track
	hr = pIDMComposer->ComposeSegmentFromTemplateEx( NULL, m_pIDMSegment, dwFlags, dwActivity, NULL, NULL );
	pIDMComposer->Release();

	// ComposeSegmentFromTemplateEx will return S_FALSE if it didn't compose any chords
	if( S_OK != hr )
	{
		return hr;
	}

	return UpdateChordStripFromSegment();
}



/*======================================================================================
METHOD:  IDMUSPRODSEGMENTEDIT::ADDSTRIP
========================================================================================
@method HRESULT | IDMUSProdSegmentEdit8 | AddStrip | Adds a track of type <p clsidTrackType>
		to this Segment.
 
@rvalue S_OK | A track of type <p clsidTrackType> was created successfully and a pointer
	to its IUnknown interface was returned in <p ppIStripMgr>.
@rvalue E_POINTER | The address in <p ppIStripMgr> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the track was not created.

@comm	When this method succeeds, the Segment Designer broadcasts a notification with
	the CLSID of the track that was added.

@ex The following excerpt creates a Band track in track group 1: |

	IDMUSProdNode* pISegmentNode;
	IDMUSProdSegmentEdit8* pISegmentEdit8;
	IUnknown *punkStripMgr;
	IDMUSProdStripMgr* pIStripMgr;
	
	...
	...
	...

	if( pISegmentNode )
	{
		if( SUCCEEDED( pISegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void**)&pISegmentEdit8 ) ) )
		{
			if( SUCCEEDED ( pISegmentEdit8->AddStrip( CLSID_DirectMusicBandTrack, 0x01, &punkStripMgr ) ) )
			{
				if( SUCCEEDED ( punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
				{
					...
					...
					...
					pIStripMgr->Release();
				}
				punkStripMgr->Release();
			}
			pISegmentEdit8->Release();
		}
	}
	
@xref <i IDMUSProdSegmentEdit8>, <i IDMUSProdStripMgr>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CSegment::AddStrip

HRESULT CSegment::AddStrip(
	CLSID clsidTrackType,	// @parm [in] DirectMusic CLSID of the track to be created.
	DWORD dwGroupBits,		// @parm [in] Track group(s) of the newly created strip.
							//		Each bit in <p dwGroupBits> corresponds to a track group.
							//		Up to 32 track groups can be specified.
	IUnknown** ppIStripMgr	// @parm [out,retval] Address of a variable to receive the 
							//		IUnknown interface for the newly created
							//		strip.
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIStripMgr == NULL )
	{
		return E_POINTER;
	}

	*ppIStripMgr = NULL;

	CTrack* pTrack = new CTrack;
	if ( pTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pTrack->m_guidClassID = clsidTrackType;
	pTrack->m_dwGroupBits = dwGroupBits;
	if( ::IsEqualGUID( pTrack->m_guidClassID, CLSID_DirectMusicTempoTrack ) )
	{
		pTrack->m_dwGroupBits = 0xFFFFFFFF;
	}

	if( SUCCEEDED ( AddTrack( pTrack ) ) )
	{
		IDMUSProdStripMgr *pIStripMgr = NULL;
		pTrack->GetStripMgr( &pIStripMgr );
		if( pIStripMgr )
		{
			// Freeze undo queue
			BOOL fOrigFreezeState = FALSE;
			VARIANT var;
			if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
			{
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
				{
					fOrigFreezeState = V_BOOL(&var);
				}

				if( !fOrigFreezeState )
				{
					var.vt = VT_BOOL;
					V_BOOL(&var) = TRUE;
					m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
				}
			}

			pIStripMgr->OnUpdate( GUID_Segment_CreateTrack, pTrack->m_dwGroupBits, NULL );

			// Notify all StripMgrs that this track was created
			if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
			{
				m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( pTrack->m_guidClassID, pTrack->m_dwGroupBits, pIStripMgr );
			}

			// Re-enable undo queue, if neccessary 
			if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline && !fOrigFreezeState )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pSegmentDlg->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}

			*ppIStripMgr = pIStripMgr;	// AddRef'd by pTrack->GetStripMgr()
			return S_OK;
		}
	}

	delete pTrack;
	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODSEGMENTEDIT::CONTAINSTEMPOSTRIP
========================================================================================
@method HRESULT | IDMUSProdSegmentEdit8 | ContainsTempoStrip | Determines whether this
		Segment contains a tempo track.
 
@rvalue S_OK | Always succeeds.

@xref <i IDMUSProdSegmentEdit8>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CSegment::ContainsTempoStrip

HRESULT CSegment::ContainsTempoStrip(
	BOOL* pfTempoStripExists	// @parm [out,retval] Address of a variable to receive the
								// flag specifying whether or not this Segment contains a 
								// tempo track. TRUE = Segment contains a tempo track.
								// FALSE = Segment does not contain a tempo track.
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfTempoStripExists == NULL )
	{
		return E_POINTER;
	}

	*pfTempoStripExists = m_fHaveTempoStrip;
	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODSEGMENTEDIT8::GETOBJECTDESCRIPTOR
========================================================================================
@method HRESULT | IDMUSProdSegmentEdit8 | GetObjectDescriptor | Fills a DMUS_OBJECTDESC
		structure with information about this Segment.

@comm	Check <p dwValidData> in the returned DMUS_OBJECTDESC structure to know which
		members are valid.
 
@rvalue S_OK | Information about this Segment was returned successfully.
@rvalue E_POINTER | <p pObjectDesc> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | <p dwSize> in the supplied DMUS_OBJECTDESC structure was not
		initialized.

@xref <i IDMUSProdSegmentEdit8>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CSegment::GetObjectDescriptor

HRESULT CSegment::GetObjectDescriptor(
	void *pObjectDesc		// @parm [out] Address of a DMUS_OBJECTDESC structure
							//		to be filled with data about this Segment.
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	DWORD dwOrigSize = pDMObjectDesc->dwSize;

	memset( pDMObjectDesc, 0, dwOrigSize );
	
	pDMObjectDesc->dwSize = dwOrigSize;
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	memcpy( &pDMObjectDesc->guidObject, &m_PPGSegment.guidSegment, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicSegment, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = (m_PPGSegment.wVersion1 << 16) | m_PPGSegment.wVersion2;
	pDMObjectDesc->vVersion.dwVersionLS = (m_PPGSegment.wVersion3 << 16) | m_PPGSegment.wVersion4;
	MultiByteToWideChar( CP_ACP, 0, m_PPGSegment.strSegmentName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODSEGMENTEDIT8::REMOVESTRIPMGR
========================================================================================
@method HRESULT | IDMUSProdSegmentEdit8 | RemoveStripMgr | Removes the <p pIStripMgr>
		from the segment and adds an item to the undo queue.

@rvalue S_OK | The track contained by <p pIStripMgr> was successfully removed
		from the segment.
@rvalue E_POINTER | <p pIStripMgr> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | <p pIStripMgr> does not point to an <i IDMUSProdStripMgr> in
		this segment.

@xref <i IDMUSProdSegmentEdit8>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CSegment::RemoveStripMgr

HRESULT CSegment::RemoveStripMgr(
	IUnknown* pIStripMgr	// @parm [in] The IUnknown interface of the strip manager
							// to remove.
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check for NULL pointer
	if( pIStripMgr == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Check for an IDMUSProdStripMgr interface
	IDMUSProdStripMgr *pDMPStripMgr;
	if( FAILED( pIStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pDMPStripMgr ) ) )
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Search for the pDMPStripMgr
	CTrack *pTrackToRemove = NULL;
	IDMUSProdStripMgr *pTmpStripMgr;
	POSITION pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		CTrack *pTrack = m_lstTracks.GetNext( pos );
		pTrack->GetStripMgr( &pTmpStripMgr );
		if( pTmpStripMgr == pDMPStripMgr )
		{
			pTrackToRemove = pTrack;
		}
		pTmpStripMgr->Release();
	}

	// Check if we found the pDMPStripMgr
	if( !pTrackToRemove )
	{
		// Didn't find it - cleanup and return E_INVALIDARG
		pDMPStripMgr->Release();
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	// Save the undo state
	BOOL fStateSaved = FALSE;
	UpdateSavedState( fStateSaved, IDS_UNDO_DELETE_TRACK );

	// Remove the stripmgr from our list
	PrivRemoveStripMgr( pDMPStripMgr );

	// Release the stripmgr
	pDMPStripMgr->Release();

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODSEGMENTEDIT8::ENUMTRACK
========================================================================================
@method HRESULT | IDMUSProdSegmentEdit8 | EnumTrack | Enumerates the tracks
		that are within the segment.

@rvalue S_OK | The information for track <p dwIndex> was successfully returned in
		<p pioTrackHeader> and <p pioTrackExtrasHeader>.
		from the segment.
@rvalue E_POINTER | <p pioTrackHeader> or <p pioTrackExtrasHeader> is not valid.  For example, they may be NULL.
@rvalue DMUS_E_NOT_FOUND | <p dwIndex> is greater than the number of tracks in the segment.

@xref <i IDMUSProdSegmentEdit8>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CSegment::EnumTrack

HRESULT CSegment::EnumTrack(
	DWORD dwIndex,				// @parm [in] The zero-based index of the track to get
	DWORD dwTrackHeaderSize,	// @parm [in] The size of the memory block passed as <p pioTrackHeader>
	void *pioTrackHeader,		// @parm [out] A pointer to a <t DMUS_IO_TRACK_HEADER> to
								// return information about the track in
	DWORD dwTrackExtrasHeaderSize,	// @parm [in] The size of the memory block passed as <p pioTrackExtrasHeader>
	void *pioTrackExtrasHeader	// @parm [out] A pointer to a <t DMUS_IO_TRACK_EXTRAS_HEADER> to
								// return information about the track in
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check for NULL pointers
	if( pioTrackHeader == NULL
	||	pioTrackExtrasHeader == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	CTrack *pTmpTrack = NULL;
	POSITION position;

	position = m_lstTracks.GetHeadPosition();
    while( position )
    {
		pTmpTrack = m_lstTracks.GetNext(position);
		if( pTmpTrack->m_dwProducerOnlyFlags & SEG_PRODUCERONLY_AUDITIONONLY )
		{
			continue;
		}

		if( dwIndex == 0 )
		{
			DMUS_IO_TRACK_HEADER ioTrackHeader;
			pTmpTrack->FillTrackHeader( &ioTrackHeader );
			memcpy( pioTrackHeader, &ioTrackHeader, min( dwTrackHeaderSize, sizeof( DMUS_IO_TRACK_HEADER ) ) );

			DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
			pTmpTrack->FillTrackExtrasHeader( &ioTrackExtrasHeader );
			memcpy( pioTrackExtrasHeader, &ioTrackExtrasHeader, min( dwTrackExtrasHeaderSize, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) ) );

			return S_OK;
		}

		dwIndex--;
	}

	return DMUS_E_NOT_FOUND;
}

HRESULT CSegment::ReplaceTrackInStrip(
    IUnknown* pIStripMgr,	// @parm [in] The IUnknown interface of the strip manager
    IUnknown* pITrack   	// @parm [in] The IUnknown interface of the track to replace in the strip manager
)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check for NULL pointer
	if( pIStripMgr == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Check for an IDMUSProdStripMgr interface
	IDMUSProdStripMgr *pDMPStripMgr;
	if( FAILED( pIStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pDMPStripMgr ) ) )
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Search for the pDMPStripMgr
	CTrack *pTrackToReplace = NULL;
	IDMUSProdStripMgr *pTmpStripMgr;
	POSITION pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		CTrack *pTrack = m_lstTracks.GetNext( pos );
		pTrack->GetStripMgr( &pTmpStripMgr );
		if( pTmpStripMgr == pDMPStripMgr )
		{
			pTrackToReplace = pTrack;
		}
		pTmpStripMgr->Release();
	}

	// Check if we found the pDMPStripMgr
	if( !pTrackToReplace )
	{
		// Didn't find it - cleanup and return E_INVALIDARG
		pDMPStripMgr->Release();
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

    IDirectMusicTrack* pIDMTrack = NULL;
	HRESULT hr = pITrack->QueryInterface( IID_IDirectMusicTrack, (void**)&pIDMTrack );
	if( SUCCEEDED( hr ) )
	{
        IDirectMusicTrack* pOldTrack = NULL;
        pTrackToReplace->GetDMTrack(&pOldTrack);
        if (pOldTrack)
        {
		    hr = m_pIDMSegment->RemoveTrack( pOldTrack );
            pOldTrack->Release();
        }
        else
        {
            hr = E_FAIL;
        }

		if( SUCCEEDED( hr ) )
		{
            pTrackToReplace->SetDMTrack( pIDMTrack );
            hr = m_pIDMSegment->InsertTrack( pIDMTrack, pTrackToReplace->m_dwGroupBits );
		}

		if( SUCCEEDED( hr ) )
		{
		    UpdateTrackConfig( pTrackToReplace );

            // Make sure that this track is actually associated with this strip manager
		    VARIANT varDMTrack;
		    varDMTrack.vt = VT_UNKNOWN;
		    V_UNKNOWN( &varDMTrack ) = pIDMTrack;
		    hr = pDMPStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack );
		}
        pIDMTrack->Release();
	}
	else
	{
		TRACE("Segment: Unable to QI for an IDirectMusicTrack\n");
		hr = S_FALSE;
    }

    if (SUCCEEDED(hr))
    {
	    // Save the undo state
	    BOOL fStateSaved = FALSE;
	    UpdateSavedState( fStateSaved, IDS_UNDO_ADD_TRACK );
    }

	// Release the stripmgr
	pDMPStripMgr->Release();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::GUIDToStripMgr

HRESULT CSegment::GUIDToStripMgr( REFGUID guidCLSID, GUID* pguidStripMgr )
{
	if ( pguidStripMgr == NULL )
	{
		return E_INVALIDARG;
	}

	LONG	lResult;

	TCHAR    szGuid[MID_BUFFER];
	LPOLESTR psz;
	
	if( FAILED( StringFromIID(guidCLSID, &psz) ) )
	{
		TRACE("Segment: Unable to convert GUID to string.\n");
		goto ON_ERR;
	}

	WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
	CoTaskMemFree( psz );

	TCHAR	szRegPath[MAX_BUFFER];
	HKEY	hKeyTrack;

	_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
	_tcscat( szRegPath, szGuid );
	lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hKeyTrack );
	if ( lResult != ERROR_SUCCESS )
	{
		TRACE("Segment: Unable to find track GUID.\n");
		goto ON_ERR;
	}

	_TCHAR	achClsId[MID_BUFFER];
	DWORD	dwType, dwCbData;
	dwCbData = MID_BUFFER;
	lResult  = ::RegQueryValueEx( hKeyTrack, _T("StripManager"), NULL,
				&dwType, (LPBYTE)achClsId, &dwCbData );
	::RegCloseKey( hKeyTrack );
	if( (lResult != ERROR_SUCCESS) || (dwType != REG_SZ) )
	{
		TRACE("Segment: Unable to find Strip Manager CLSID in Track's key.\n");
		goto ON_ERR;
	}

	wchar_t awchClsId[80];

	if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchClsId, sizeof(awchClsId) / sizeof(wchar_t) ) != 0 )
	{
		IIDFromString( awchClsId, pguidStripMgr );
		return S_OK;
	}

ON_ERR:
	TRACE("Segment: Using UnknownStripMgr as default\n");
	memcpy( pguidStripMgr, &CLSID_UnknownStripMgr, sizeof(GUID) );
	return S_OK;
}

void CSegment::RegisterSegementStateWithAudiopath( BOOL fRegister, IDirectMusicSegmentState *pSegState )
{
	// If we have an audiopath and segment state, notify it that we stopped playing
	if( m_pIAudioPathNode && pSegState )
	{
		// Get the IDirectMusicSegmentState8 interface
		IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
		if( SUCCEEDED( pSegState->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
		{
			IDMUSProdAudioPathInUse *pIDMUSProdAudioPathInUse;
			if( SUCCEEDED( m_pIAudioPathNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void**)&pIDMUSProdAudioPathInUse ) ) )
			{
				IDirectMusicAudioPath *pSegmentAudioPath;
				if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pSegmentAudioPath ) ) )
				{
					IDirectMusicAudioPath *pPerformanceAudioPath = NULL;
					if( FAILED( m_pComponent->m_pIDMPerformance->GetDefaultAudioPath( &pPerformanceAudioPath ) )
					||	(pPerformanceAudioPath != pSegmentAudioPath) )
					{
						TRACE("Register %d %x\n", fRegister, pSegState );
						pIDMUSProdAudioPathInUse->UsingAudioPath( pSegmentAudioPath, fRegister );
					}

					if( pPerformanceAudioPath )
					{
						pPerformanceAudioPath->Release();
					}

					pSegmentAudioPath->Release();
				}
				pIDMUSProdAudioPathInUse->Release();
			}
			pIDirectMusicSegmentState8->Release();
		}
	}
}

void CSegment::CleanUp()
{
	Stop( TRUE );

	m_pComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*) this );

	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	// Following strings only saved when they have values
	// So make sure they are initialized!!
	m_PPGSegment.strAuthor.Empty();
	m_PPGSegment.strCopyright.Empty();
	m_PPGSegment.strSubject.Empty();
	m_PPGSegment.strInfo.Empty();

	m_wLegacyActivityLevel = 5;
	m_fLoadedLegacyActivityLevel = false;

	if ( !m_lstTracks.IsEmpty() )
	{
		CTrack	*pTrack;
		while( !m_lstTracks.IsEmpty() )
		{
			pTrack = m_lstTracks.RemoveHead() ;
			if( m_pSegmentDlg )
			{
				m_pSegmentDlg->RemoveTrack( pTrack );
			}
			delete pTrack;
		}
	}

	m_fHaveTempoStrip = FALSE;

	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		IDMUSProdStrip* pStrip;
		// 1 because we don't want to remove the top strip. ----\|/
		while( SUCCEEDED( m_pSegmentDlg->m_pTimeline->EnumStrip( 1, &pStrip ) ) )
		{
			m_pSegmentDlg->m_pTimeline->RemoveStrip( pStrip );
			pStrip->Release();
		}
	}

	EnterCriticalSection( &m_csSegmentState );
	while( !m_lstSegStates.IsEmpty() )
	{
		SegStateParams *pSegStateParams = m_lstSegStates.RemoveHead();
		RegisterSegementStateWithAudiopath( FALSE, pSegStateParams->pSegState );
		delete pSegStateParams;
	}
	while( !m_lstTransSegStates.IsEmpty() )
	{
		m_lstTransSegStates.RemoveHead()->Release();
	}
	RELEASE( m_pIDMCurrentSegmentState );
	LeaveCriticalSection( &m_csSegmentState );

	if( m_pIDMSegment8 )
	{
		IDirectMusicObjectP* pIDMObjectP;
		if( SUCCEEDED ( m_pIDMSegment8->QueryInterface( IID_IDirectMusicObjectP, (void**)&pIDMObjectP ) ) )
		{
			pIDMObjectP->Zombie();
			RELEASE( pIDMObjectP );
		}

		m_pIDMSegment8->Release();
		m_pIDMSegment8 = NULL;
	}

	if( m_pIDMSegment )
	{
		m_pIDMSegment->Release();
		m_pIDMSegment = NULL;
	}

	RELEASE( m_pIDMTransitionSegment );

	::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicSegment, (void**)&m_pIDMSegment );
	ASSERT( m_pIDMSegment );
	if( m_pIDMSegment )
	{
		m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8, (void**)&m_pIDMSegment8 );
	}
}


HRESULT CSegment::InitSegmentForPlayback()
{
	ASSERT( m_pIDMSegment != NULL );

	if( m_pIDMSegment )
	{
		// Set the AudioPathConfig on the segment
		IDirectMusicSegment8P *pSegmentPrivate;
		if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **)&pSegmentPrivate ) ) )
		{
			// Try and get an AudioPathConfig object from the node
			IUnknown *pAudioPathConfig;
			if( (m_pIAudioPathNode == NULL)
			||	FAILED( m_pIAudioPathNode->GetObject( CLSID_DirectMusicAudioPathConfig, IID_IUnknown, (void**)&pAudioPathConfig ) ) )
			{
				pAudioPathConfig = NULL;
			}

			pSegmentPrivate->SetAudioPathConfig( pAudioPathConfig );
			pSegmentPrivate->Release();

			if( pAudioPathConfig )
			{
				pAudioPathConfig->Release();
				// Do not clear pAudioPathConfig - it is used below
			}

			DWORD dwResolution;
			if( m_fInTransition )
			{
				dwResolution = m_dwTransitionPlayFlags;
				if( m_dwTransitionPlayFlags & DMUS_SEGF_DEFAULT )
				{
					dwResolution |= m_dwResolution;
				}
			}
			else
			{
				dwResolution = m_dwResolution;
			}

			if( (dwResolution & DMUS_SEGF_USE_AUDIOPATH)
			&&	(pAudioPathConfig == NULL) )
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

BOOL CSegment::FindTrackByCLSID( REFCLSID clsidTrackId, CTrack **ppTrack )
{
	CTrack *pTmpTrack = NULL;
	POSITION position;

	position = m_lstTracks.GetHeadPosition();
    while( position )
    {
		pTmpTrack = m_lstTracks.GetNext(position);
		if( IsEqualCLSID( pTmpTrack->m_guidClassID, clsidTrackId ) )
		{
			if( ppTrack )
			{
				*ppTrack = pTmpTrack;
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CSegment::OnTimeSigChange( void )
{
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		MUSIC_TIME mtNewLength;
		// Ignore if we're using reference time length
		if( !(m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH)
		&&	SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, m_PPGSegment.dwMeasureLength, 0, &mtNewLength ) ) )
		{
			if( m_mtLength != mtNewLength )
			{
				BOOL fLoopEndIsLength = (m_mtLoopEnd == m_mtLength);

				const MUSIC_TIME mtOldLength = m_mtLength;
				m_mtLength = mtNewLength;

				if( fLoopEndIsLength )
				{
					m_mtLoopEnd = m_mtLength;
					//OnLengthChanged will update m_pIDMSegment
				}
				/*
				// Update Start Time from m_PPGSegment
				MUSIC_TIME mt;
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwPlayStartMeasure, 0, &mt ) ) )
				{
					DMUS_TIMESIGNATURE TimeSig;
					if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
					{
						m_mtPlayStart = mt + m_PPGSegment.bPlayStartBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
										m_PPGSegment.wPlayStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
										m_PPGSegment.wPlayStartTick;
						if(m_pIDMSegment != NULL)
						{
							m_pIDMSegment->SetStartPoint( m_mtPlayStart );
						}
					}
				}

				// Update Loop Start from m_PPGSegment
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwLoopStartMeasure, 0, &mt ) ) )
				{
					DMUS_TIMESIGNATURE TimeSig;
					if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
					{
						m_mtLoopStart = mt + m_PPGSegment.bLoopStartBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
										m_PPGSegment.wLoopStartGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
										m_PPGSegment.wLoopStartTick;
						if( m_mtLoopStart >= m_mtLength )
						{
							m_mtLoopStart = m_mtLength - 1;
							//ASSERT(FALSE);
						}
					}
				}

				// Update Loop End from m_PPGSegment
				if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->MeasureBeatToClocks( m_PPGSegment.dwTrackGroup, 0, pSegment->dwLoopEndMeasure, 0, &mt ) ) )
				{
					DMUS_TIMESIGNATURE TimeSig;
					if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetParam( GUID_TimeSignature, m_PPGSegment.dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
					{
						m_mtLoopEnd = mt + m_PPGSegment.bLoopEndBeat * NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) +
										m_PPGSegment.wLoopEndGrid * (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) +
										m_PPGSegment.wLoopEndTick;
						if( m_mtLoopEnd > m_mtLength )
						{
							m_mtLoopEnd = m_mtLength;
							//ASSERT(FALSE);
						}
					}
				}

				if(m_pIDMSegment != NULL)
				{
					m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
				}
				*/

				// Update m_pIDMSegment and the Timeline
				OnLengthChanged( mtOldLength );
			}
		}

		// Refresh the property page
		ASSERT(m_pComponent);
		ASSERT(m_pComponent->m_pIFramework);
		IDMUSProdPropSheet *pIPropSheet;
		if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **) &pIPropSheet)))
		{
			pIPropSheet->RefreshTitleByObject(this);
			pIPropSheet->Release();
		}
	}

	// Notify connected nodes that Segment time signature has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, GUID_TimeSignature, NULL );
}

HRESULT CSegment::InsertTrackAtDefaultPos( CTrack *pTrackToInsert )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pTrackToInsert )
	{
		return E_POINTER;
	}

	// determine the position
	POSITION pos, pos2 = NULL;
	CTrack *pTrackTmp;
	int nMyId;
	nMyId = TrackCLSIDToInt( pTrackToInsert->m_guidClassID );
	pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		pTrackTmp = m_lstTracks.GetNext( pos );
		// if the strip being insert should go BEFORE pTrackTmp, break
		if( CompareTracks( TrackCLSIDToInt( pTrackTmp->m_guidClassID ), pTrackTmp->m_dwGroupBits,
						   nMyId, pTrackToInsert->m_dwGroupBits ) == 2 )
		{
			m_lstTracks.InsertBefore( pos2, pTrackToInsert );
			return S_OK;
		}
	}

	m_lstTracks.AddTail( pTrackToInsert );

	return S_OK;
}

int TrackCLSIDToInt( REFCLSID clsidType )
{
	if( memcmp( &clsidType, &CLSID_DirectMusicTempoTrack, sizeof(GUID) ) == 0 )
	{
		return 0;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicTimeSigTrack, sizeof(GUID) ) == 0 )
	{
		return 10;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordMapTrack, sizeof(GUID) ) == 0 )
	{
		return 20;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSignPostTrack, sizeof(GUID) ) == 0 )
	{
		return 30;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordTrack, sizeof(GUID) ) == 0 )
	{
		return 40;
	}
	else if( (memcmp( &clsidType, &CLSID_DirectMusicMotifTrack, sizeof(GUID) ) == 0)
		 ||	 (memcmp( &clsidType, &CLSID_DirectMusicSegmentTriggerTrack, sizeof(GUID) ) == 0) )
	{
		return 50;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicCommandTrack, sizeof(GUID) ) == 0 )
	{
		return 60;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicStyleTrack, sizeof(GUID) ) == 0 )
	{
		return 70;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMelodyFormulationTrack, sizeof(GUID) ) == 0 )
	{
		return 80;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicPatternTrack, sizeof(GUID) ) == 0 )
	{
		return 90;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSeqTrack, sizeof(GUID) ) == 0 )
	{
		return 100;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicWaveTrack, sizeof(GUID) ) == 0 )
	{
		return 110;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicBandTrack, sizeof(GUID) ) == 0 )
	{
		return 120;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicParamControlTrack, sizeof(GUID) ) == 0 )
	{
		return 130;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMuteTrack, sizeof(GUID) ) == 0 )
	{
		return 140;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicScriptTrack, sizeof(GUID) ) == 0 )
	{
		return 150;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicLyricsTrack, sizeof(GUID) ) == 0 )
	{
		return 160;
	}
	else
	{
		return 170;
	}
}

int CompareTracks( int nType1, DWORD dwGroups1, int nType2, DWORD dwGroups2 )
{
	// If the group bits are equal, check the types
	if( dwGroups1 == dwGroups2 )
	{
		return nType1 <= nType2 ? 1 : 2;
	}

	// Continue while both group bits have at least one bit still set
	while( dwGroups1 && dwGroups2 )
	{
		if( (dwGroups1 & dwGroups2 & 1) == 0 )
		{
			// One of the first bits of dwGroups1 or dwGroups2 is zero
			if( dwGroups1 & 1 )
			{
				// The first bit of dwGroups1 is one and at least one bit in dwGroups2 is set -
				// #1 should go first
				return 1;
			}
			else if( dwGroups2 & 1 )
			{
				// The first bit of dwGroups2 is one and at least one bit in dwGroups1 is set -
				// #2 should go first
				return 2;
			}
			// else both of the first bits are zero - try the next bit
		}
		else
		{
			// Both of the first bits of dwGroups1 and dwGroups2 are one
			if( nType1 < nType2 )
			{
				return 1; // #1 should go first
			}
			else if ( nType1 > nType2 )
			{
				return 2; // #2 should go first
			}
			// else both are the same type - check the next bit
		}
		dwGroups1 = dwGroups1 >> 1;
		dwGroups2 = dwGroups2 >> 1;
	}

	if( dwGroups1 )
	{
		// Some of the bits in dwGroups1 are set - #2 should go first
		return 2;
	}
	else if( dwGroups2 )
	{
		// Some of the bits in dwGroups2 are set - #1 should go first
		return 1;
	}
	// dwGroups1 == dwGroups2 == 0 (Shouldn't happen!)
	ASSERT( FALSE );
	return 1;
}

void CSegment::OnNameChange( void )
{
	// Our File name changed - Get new Segment file name
	CString strName;
	BSTR bstrName;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->GetNodeFileName( this, &bstrName ) ) )
	{
		// Node is in the Project tree
		strName = bstrName;
		::SysFreeString( bstrName );
		int nFindPos = strName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strName = strName.Right( strName.GetLength() - nFindPos - 1 );
		}
		strName += _T(": ");
	}
	else
	{
		// Node has not yet been placed in the Project Tree
		// because the Segment is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}

	// Plus "name"
	strName += m_PPGSegment.strSegmentName;

	// Update the Conductor's Toolbars
	bstrName = strName.AllocSysString();
	m_pComponent->m_pIConductor->SetTransportName( this, bstrName );
	bstrName = strName.AllocSysString();
	m_pComponent->m_pIConductor->SetSecondaryTransportName( this, bstrName );

	// Update the Framework
	m_pComponent->m_pIFramework->RefreshNode( this );

	// Update our property sheet
	ASSERT(m_pComponent);
	ASSERT(m_pComponent->m_pIFramework);
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **) &pIPropSheet)))
	{
		pIPropSheet->RefreshTitleByObject(this);
		pIPropSheet->Release();
	}

	// Notify connected nodes that Style name has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, GUID_Segment_Name_Change, NULL );
}

DWORD CSegment::GetButtonState() const
{
	if( m_pSegmentDlg != NULL )
	{
		if( m_fRecordPressed )
		{
			return BS_RECORD_ENABLED | BS_RECORD_CHECKED;
		}
		return BS_RECORD_ENABLED;
	}
	else
	{
		return BS_AUTO_UPDATE;
	}
}

void GetTimelineTempoState( IDMUSProdTimeline *pTimeline, double &dblTempo, BOOL &fEnableTempoEdit )
{
	ASSERT( pTimeline );

	// Get the tempo at the current cursor position;
	long lTimeCursor;
	if( SUCCEEDED(pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTimeCursor) ) )
	{
		MUSIC_TIME mtLatestTempoTime = LONG_MIN;
		DWORD dwIndex = 0;
		while( TRUE )
		{
			IDMUSProdStripMgr *pTempoStripMgr = NULL;
			if( SUCCEEDED( pTimeline->GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
			{
				if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
				{
					DMUS_TEMPO_PARAM tempo;
					HRESULT hr = pTempoStripMgr->GetParam( GUID_TempoParam, lTimeCursor, NULL, &tempo );

					// If we got a real tempo, and it's more recent than any other tempos so far
					if( (hr == S_OK) && (tempo.mtTime > mtLatestTempoTime) )
					{
						// Save the tempo value and update the latest tempo time.
						dblTempo = tempo.dblTempo;
						mtLatestTempoTime = tempo.mtTime;
					}
				}

				pTempoStripMgr->Release();
			}
			else
			{
				// No more strips to check - exit
				break;
			}
			dwIndex++;
		}

		if( mtLatestTempoTime != LONG_MIN )
		{
			fEnableTempoEdit = TRUE;
			return;
		}
	}

	// Something failed, or unable to find a tempo - disable the edit box
	/* No need - already diabled
	dblTempo = -1.0;
	fEnableTempoEdit = FALSE;
	*/
}

HRESULT CSegment::SetQuantizeParams( SequenceQuantize* pSequenceQuantize )
{
	if( pSequenceQuantize == NULL )
	{
		return E_INVALIDARG;
	}

	m_SequenceQuantizeParms.m_wQuantizeTarget = pSequenceQuantize->m_wQuantizeTarget;
	m_SequenceQuantizeParms.m_bResolution = pSequenceQuantize->m_bResolution;	
	m_SequenceQuantizeParms.m_bStrength = pSequenceQuantize->m_bStrength;
	m_SequenceQuantizeParms.m_dwFlags = pSequenceQuantize->m_dwFlags;	
	
	return S_OK;
}

HRESULT CSegment::GetQuantizeParams( SequenceQuantize* pSequenceQuantize )
{
	if( pSequenceQuantize == NULL )
	{
		return E_INVALIDARG;
	}

	pSequenceQuantize->m_wQuantizeTarget = m_SequenceQuantizeParms.m_wQuantizeTarget;
	pSequenceQuantize->m_bResolution = m_SequenceQuantizeParms.m_bResolution;	
	pSequenceQuantize->m_bStrength = m_SequenceQuantizeParms.m_bStrength;
	pSequenceQuantize->m_dwFlags = m_SequenceQuantizeParms.m_dwFlags;	
	
	return S_OK;
}

HRESULT CSegment::SetVelocitizeParams( SequenceVelocitize* pSequenceVelocitize )
{
	if( pSequenceVelocitize == NULL )
	{
		return E_INVALIDARG;
	}

	m_SequenceVelocitizeParams.m_wVelocityTarget = pSequenceVelocitize->m_wVelocityTarget;
	m_SequenceVelocitizeParams.m_bCompressMin = pSequenceVelocitize->m_bCompressMin;	
	m_SequenceVelocitizeParams.m_bCompressMax = pSequenceVelocitize->m_bCompressMax;
	m_SequenceVelocitizeParams.m_lAbsoluteChangeStart = pSequenceVelocitize->m_lAbsoluteChangeStart;
	m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd = pSequenceVelocitize->m_lAbsoluteChangeEnd;
	m_SequenceVelocitizeParams.m_dwVelocityMethod = pSequenceVelocitize->m_dwVelocityMethod;	
	
	return S_OK;
}

HRESULT CSegment::GetVelocitizeParams( SequenceVelocitize* pSequenceVelocitize )
{
	if( pSequenceVelocitize == NULL )
	{
		return E_INVALIDARG;
	}

	pSequenceVelocitize->m_wVelocityTarget = m_SequenceVelocitizeParams.m_wVelocityTarget;
	pSequenceVelocitize->m_bCompressMin = m_SequenceVelocitizeParams.m_bCompressMin;	
	pSequenceVelocitize->m_bCompressMax = m_SequenceVelocitizeParams.m_bCompressMax;
	pSequenceVelocitize->m_lAbsoluteChangeStart = m_SequenceVelocitizeParams.m_lAbsoluteChangeStart;
	pSequenceVelocitize->m_lAbsoluteChangeEnd = m_SequenceVelocitizeParams.m_lAbsoluteChangeEnd;
	pSequenceVelocitize->m_dwVelocityMethod = m_SequenceVelocitizeParams.m_dwVelocityMethod;	
	
	return S_OK;
}

void CSegment::UpdateConductorTempo( void )
{
	double dblTempo = -1.0;
	BOOL fEnableTempoEdit = FALSE;

	// Enable the tempo edit box if we have a tempo track
	if( m_fHaveTempoStrip )
	{
		if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
		{
			GetTimelineTempoState( m_pSegmentDlg->m_pTimeline, dblTempo, fEnableTempoEdit );
		}
	}
	else
	{
		// No Tempo strip - leave disabled
	}

	// If we're a clocktime segment, update the musictime length
	if( m_pSegmentDlg
	&&	m_pSegmentDlg->m_pTimeline
	&&	(m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH) )
	{
		const MUSIC_TIME mtOldLength = m_mtLength;

		m_pSegmentDlg->m_pTimeline->RefTimeToClocks( m_rtLength, &m_mtLength );
		m_pSegmentDlg->m_pTimeline->ClocksToMeasureBeat( m_PPGSegment.dwTrackGroup, 0, m_mtLength, (long *)(&m_PPGSegment.dwMeasureLength), NULL );

		// Ensure the segment is at least one measure long
		m_PPGSegment.dwMeasureLength = max( 1, m_PPGSegment.dwMeasureLength );

		if( m_mtLength != mtOldLength )
		{
			m_fModified = TRUE;

			// Update m_pIDMSegment and the Timeline
			OnLengthChanged( mtOldLength );
		}
	}

	m_pComponent->m_pIConductor->SetTempo( (IDMUSProdTransport *)this, dblTempo, fEnableTempoEdit );
}

void CSegment::OnLengthChanged( MUSIC_TIME mtOldLength )
{
	// Due to DirectMusic restriction, we must update the loop points first
	if( m_mtLoopStart > m_mtLength )
	{
		m_mtLoopStart = 0;
		m_mtLoopEnd = 0;
		m_dwLoopRepeats = 0;
		if(m_pIDMSegment)
		{
			m_pIDMSegment->SetLoopPoints( 0, 0 );
			m_pIDMSegment->SetRepeats( 0 );
		}
	}
	else if( m_mtLoopEnd > m_mtLength )
	{
		m_mtLoopEnd = m_mtLength;
		if(m_pIDMSegment)
		{
			m_pIDMSegment->SetLoopPoints( m_mtLoopStart, m_mtLoopEnd );
		}
	}

	if(m_pIDMSegment)
	{
		ASSERT( m_mtLoopEnd <= m_mtLength );
		VERIFY( SUCCEEDED( m_pIDMSegment->SetLength( m_mtLength) ) );
	}

	SetSegmentHeaderChunk();

	// Update the timeline with the new length.
	if(m_pSegmentDlg)
	{
		m_pSegmentDlg->SetTimelineLength( m_mtLength );

		// notify all groups
		m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( GUID_Segment_Length_Change, 0xFFFFFFFF, &mtOldLength );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UpdateTrackConfig - updated the track configuration settings for the passed in track and segment

HRESULT CSegment::UpdateTrackConfig( CTrack *pTrack )
{
	if( (pTrack == NULL) || (m_pIDMSegment8 == NULL) )
	{
		// Shouldn't happen
		ASSERT(FALSE);
		return E_POINTER;
	}

	// Compute the index of this track
	DWORD dwIndex = 0;

	// Get a pointer to the DirectMusic track to look for
	IDirectMusicTrack *pIDMTrackToLookFor = NULL;
	pTrack->GetDMTrack( &pIDMTrackToLookFor );

	if( pIDMTrackToLookFor == NULL )
	{
		return S_OK;	// Nothing to do
	}

	// Initialize the temporary DirectMusic track pointer
	IDirectMusicTrack *pIDMTrack = NULL;

	// Enumerate through all tracks until we find the one we want
	while( SUCCEEDED( m_pIDMSegment8->GetTrack( pTrack->m_guidClassID, pTrack->m_dwGroupBits, dwIndex, &pIDMTrack ) ) )
	{
		// Release the pointer that was returned by GetTrack
		pIDMTrack->Release();

		// Check if this is track we're looking for
		if( pIDMTrack == pIDMTrackToLookFor )
		{
			// Yep
			break;
		}

		// Nope - increment the index and look again
		dwIndex++;
	}

	// Release our reference on the track we're looking for
	pIDMTrackToLookFor->Release();

	// Update the track's trackconfig flags
	HRESULT hr = m_pIDMSegment8->SetTrackConfig( pTrack->m_guidClassID, pTrack->m_dwGroupBits, dwIndex, pTrack->m_dwTrackExtrasFlags, ~pTrack->m_dwTrackExtrasFlags );
	if( FAILED( hr ) )
	{
		TRACE("Segment: Failed to set TrackConfig flags.\n");
	}
	else
	{
		// Set track priority
		IDirectMusicSegment8P *pSegmentPrivate;
		if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **)&pSegmentPrivate ) ) )
		{
			hr = pSegmentPrivate->SetTrackPriority( pTrack->m_guidClassID, pTrack->m_dwGroupBits, dwIndex, pTrack->m_dwTrackExtrasPriority );
			if( FAILED( hr ) )
			{
				TRACE("Segment: Failed to set TrackConfig priority.\n");
			}
			pSegmentPrivate->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// FindCTrackFromDMTrack - Seach for a CTrack object that matches the give pIDMTrack

CTrack *CSegment::FindCTrackFromDMTrack( IDirectMusicTrack* pIDMTrack )
{
	// Iterate through m_lstTracks
	POSITION pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current CTrack
		CTrack *pTmpTrack = m_lstTracks.GetNext( pos );

		// Get a pointer to the associated IDirectMusicTrack
		IDirectMusicTrack *pIDMTmpTrack = NULL;
		pTmpTrack->GetDMTrack( &pIDMTmpTrack );

		// Check if the IDMTrack is equal to the one we're searching for
		if( pIDMTmpTrack == pIDMTrack )
		{
			// Is equal - return CTrack pointer
			return pTmpTrack;
		}
	}
	
	// Didn't find pIDMTrack - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::OnNotificationCallback - Handle a DirectMusic notification in a message thread

void CSegment::OnNotificationCallback( void )
{
	UpdateChordStripFromSegment();
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::UpdateChordStripFromSegment

HRESULT CSegment::UpdateChordStripFromSegment( void )
{
	// Update the Chord strip with the new chord track

	//DWORD dwIndex = 0;

	// Find the SignPost track
	IDirectMusicTrack *pDMSignPostTrack;
	//while( SUCCEEDED( m_pIDMSegment->GetTrack( CLSID_DirectMusicSignPostTrack, 0xFFFFFFFF, dwIndex, &pDMSignPostTrack) ) )
	if( SUCCEEDED( m_pIDMSegment->GetTrack( CLSID_DirectMusicSignPostTrack, 0xFFFFFFFF, 0, &pDMSignPostTrack) ) )
	{
		// Get the group bits of the SignPost track
		DWORD dwGroupBits;
		if( FAILED( m_pIDMSegment->GetTrackGroup( pDMSignPostTrack, &dwGroupBits) ) )
		{
			TRACE("CSegment: Whoops, can't find group bits of the SignPost track in a composed segment.\n");
			pDMSignPostTrack->Release();
			return E_FAIL;
		}

		// Release the signpost track
		pDMSignPostTrack->Release();
		pDMSignPostTrack = NULL;

		// Find the new Chord Track
		IDirectMusicTrack *pIDMComposedChordTrack;
		if( FAILED( m_pIDMSegment->GetTrack( CLSID_DirectMusicChordTrack, dwGroupBits, 0, &pIDMComposedChordTrack) ) )
		{
			TRACE("CSegment: Whoops, can't find a Chord track in a composed segment.\n");
			return S_FALSE;
		}

		// Find the CTrack for this track
		CTrack *pTmpTrack = FindCTrackFromDMTrack( pIDMComposedChordTrack );

		// Set if we create a new CTrack
		BOOL fNeedToAddTrack = FALSE;

		// Double-check that we found a CTrack
		if( pTmpTrack == NULL )
		{
			// Composition engine must have created a new track - create a new CTrack to correspond to it
			pTmpTrack = new CTrack();
			if( pTmpTrack == NULL )
			{
				pIDMComposedChordTrack->Release();
				return E_UNEXPECTED;
			}

			memcpy( &pTmpTrack->m_guidClassID, &CLSID_DirectMusicChordTrack, sizeof( GUID ) );
			if( FAILED( GUIDToStripMgr( pTmpTrack->m_guidClassID, &pTmpTrack->m_guidEditorID ) ) )
			{
				TRACE("Segment::AddTrack: Unable to find Strip Editor for Track's CLSID.\n");
				ASSERT(FALSE); // This shouldn't happen, since GUIDToStripMgr should default
				// to the UnknownStripMgr if it can't find a match in the registry.
				delete pTmpTrack;
				pIDMComposedChordTrack->Release();
				return E_UNEXPECTED;
			}

			IDMUSProdStripMgr *pITmpStripMgr;
			HRESULT hr = ::CoCreateInstance( pTmpTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
									 IID_IDMUSProdStripMgr, (void**)&pITmpStripMgr );
			if( FAILED( hr ) )
			{
				TRACE("Segment: Unable to CoCreate an IDMUSProdStripMgr - going to try the UnknownStripMgr\n");
				memcpy( &pTmpTrack->m_guidEditorID, &CLSID_UnknownStripMgr, sizeof(GUID) );
				hr = ::CoCreateInstance( pTmpTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
										 IID_IDMUSProdStripMgr, (void**)&pITmpStripMgr );
				if( FAILED( hr ) )
				{
					TRACE("Segment: Unable to CoCreate an UnknownStripMgr\n");
					delete pTmpTrack;
					pIDMComposedChordTrack->Release();
					return E_UNEXPECTED;
				}
			}

			pTmpTrack->SetStripMgr( pITmpStripMgr );
			pTmpTrack->SetDMTrack( pIDMComposedChordTrack );

			// Get the group bits for this track
			if( FAILED( m_pIDMSegment->GetTrackGroup( pIDMComposedChordTrack, &dwGroupBits) ) )
			{
				TRACE("CSegment: Whoops, can't find group bits of the chord track in a composed segment.\n");
				delete pTmpTrack;
				pIDMComposedChordTrack->Release();
				pITmpStripMgr->Release();
				return E_UNEXPECTED;
			}

			// Set the group bits for this track
			pTmpTrack->m_dwGroupBits = dwGroupBits;

			// Set the chunk ids for this track
			pTmpTrack->m_ckid = NULL;
			pTmpTrack->m_fccType = DMUS_FOURCC_CHORDTRACK_LIST;

			// Give the strip manager a pointer to its DirectMusic Track.
			VARIANT varDMTrack;
			varDMTrack.vt = VT_UNKNOWN;
			V_UNKNOWN( &varDMTrack ) = pIDMComposedChordTrack;
			if( FAILED( pITmpStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack ) ) )
			{
				delete pTmpTrack;
				pIDMComposedChordTrack->Release();
				pITmpStripMgr->Release();
				return E_UNEXPECTED;
			}

			// Give the strip manager a pointer to the framework.
			varDMTrack.vt = VT_UNKNOWN;
			V_UNKNOWN( &varDMTrack ) = m_pComponent->m_pIFramework;
			if( FAILED( pITmpStripMgr->SetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, varDMTrack ) ) )
			{
				delete pTmpTrack;
				pIDMComposedChordTrack->Release();
				pITmpStripMgr->Release();
				return E_UNEXPECTED;
			}

			// If requested, try and set the strip manager's DocRootNode pointer
			if( pITmpStripMgr->IsParamSupported( GUID_DocRootNode ) == S_OK )
			{
				pITmpStripMgr->SetParam( GUID_DocRootNode, 0, (IDMUSProdNode *)this );
			}

			// Tell the strip manager what its group bits are
			DMUS_IO_TRACK_HEADER ioTrackHeader;
			memcpy( &ioTrackHeader.guidClassID, &pTmpTrack->m_guidClassID, sizeof(GUID) );
			ioTrackHeader.dwPosition = pTmpTrack->m_dwPosition;
			ioTrackHeader.dwGroup = pTmpTrack->m_dwGroupBits;
			ioTrackHeader.ckid = pTmpTrack->m_ckid;
			ioTrackHeader.fccType = pTmpTrack->m_fccType;
			varDMTrack.vt = VT_BYREF;
			V_BYREF(&varDMTrack) = &ioTrackHeader;

			hr = pITmpStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKHEADER, varDMTrack );
			if( FAILED( hr ) )
			{
				delete pTmpTrack;
				pIDMComposedChordTrack->Release();
				pITmpStripMgr->Release();
				return E_UNEXPECTED;
			}

			// Release our reference to the strip manager
			pITmpStripMgr->Release();

			// Flag that we should add pTmpTrack to the segment later on
			fNeedToAddTrack = TRUE;
		}

		// Release the chord track
		pIDMComposedChordTrack->Release();

		// Get the associated strip manager
		IDMUSProdStripMgr *pIStripMgr = NULL;
		pTmpTrack->GetStripMgr( &pIStripMgr );
		if( pIStripMgr == NULL )
		{
			if( fNeedToAddTrack ) delete pTmpTrack;
			return E_UNEXPECTED;
		}

		// Query the strip manager for its IPersistStream interface
		IPersistStream *pIPersistStream;
		if( FAILED( pIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
		{
			pIStripMgr->Release();
			if( fNeedToAddTrack ) delete pTmpTrack;
			return E_UNEXPECTED;
		}

		// Release the strip manager
		pIStripMgr->Release();

		// This causes the ChordStrip to load chords from its attached IDirectMusicTrack.
		if( FAILED( pIPersistStream->Load( NULL ) ) )
		{
			pIPersistStream->Release();
			if( fNeedToAddTrack ) delete pTmpTrack;
			return E_UNEXPECTED;
		}

		// Allocate a memory stream to save the new chords in
		ASSERT(m_pComponent != NULL);
		ASSERT(m_pComponent->m_pIFramework != NULL);
		IStream *pIStream;
		if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIStream ) ) )
		{
			pIPersistStream->Release();
			if( fNeedToAddTrack ) delete pTmpTrack;
			return E_UNEXPECTED;
		}

		// Save the chords into pIStream
		if( FAILED( pIPersistStream->Save( pIStream, TRUE ) ) )
		{
			pIPersistStream->Release();
			pIStream->Release();
			if( fNeedToAddTrack ) delete pTmpTrack;
			return E_UNEXPECTED;
		}

		// Release the strip manager's persist stream interface
		pIPersistStream->Release();

		// Add an undo event to the queue
		BOOL fSaved = FALSE;
		UpdateSavedState(fSaved, IDS_UNDO_COMPOSE);

		// Update the CTrack with the new stream
		pTmpTrack->SetStream( pIStream );

		// Release the stream
		pIStream->Release();

		// Check if we need to add pTmpTrack to the segment
		if( fNeedToAddTrack )
		{
			// Add the track to our list and the dialog (if it exists)
			InsertTrackAtDefaultPos( pTmpTrack );
			if(m_pSegmentDlg != NULL)
			{
				m_pSegmentDlg->AddTrack( pTmpTrack );
			}
		}
		//dwIndex++;
	}
	/*
	if( dwIndex == 0 )
	*/
	else
	{
		TRACE("CSegment: Whoops, can't find a SignPost track in a composed segment.\n");
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::GetNumExtraBars

void CSegment::GetNumExtraBars( DWORD *pdwExtraBars, BOOL *pfPickupBars )
{
	DWORD dwExtraBars = 0;
	BOOL fPickupBars = FALSE;

	bool fGotExtras = false;
	bool fGotPickup = false;
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		IDMUSProdStripMgr* pIStripMgr;
		DWORD dwTmp;
		POSITION position = m_lstTracks.GetHeadPosition();
		while(position != NULL)
		{
			CTrack *pTrack = m_lstTracks.GetNext(position);
			pTrack->GetStripMgr( &pIStripMgr );
			if( pIStripMgr )
			{
				if( SUCCEEDED( pIStripMgr->GetParam( GUID_Segment_ExtensionLength, 0, NULL, &dwTmp ) ) )
				{
					if( fGotExtras )
					{
						if( dwExtraBars != dwTmp )
						{
							dwExtraBars = 0xFFFFFFFF;
						}
					}
					else
					{
						dwExtraBars = dwTmp;
						fGotExtras = true;
					}
				}
				if( SUCCEEDED( pIStripMgr->GetParam( GUID_Segment_PickupLength, 0, NULL, &dwTmp ) ) )
				{
					if( fGotPickup )
					{
						if( unsigned(fPickupBars) != dwTmp )
						{
							fPickupBars = 2;
						}
					}
					else
					{
						fPickupBars = (dwTmp > 1) ? 2 : dwTmp;
						fGotPickup = true;
					}
				}
				pIStripMgr->Release();
			}
		}
	}

	*pdwExtraBars = dwExtraBars;
	*pfPickupBars = fPickupBars;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::AddSegmentToGraphUserList

HRESULT CSegment::AddSegmentToGraphUserList( void )
{
	if( m_pIToolGraphNode == NULL 
	||  m_pIDMSegment == NULL )
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Now get the IDMUSProdToolGraphInfo interface for m_pIToolGraphNode
	IDMUSProdToolGraphInfo* pIToolGraphInfo;
	if( SUCCEEDED ( m_pIToolGraphNode->QueryInterface( IID_IDMUSProdToolGraphInfo, (void**)&pIToolGraphInfo ) ) )
	{
		if( SUCCEEDED ( pIToolGraphInfo->AddToGraphUserList( m_pIDMSegment ) ) )
		{
			hr = S_OK;
		}

		RELEASE( pIToolGraphInfo );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::RemoveSegmentFromGraphUserList

HRESULT CSegment::RemoveSegmentFromGraphUserList( void )
{
	if( m_pIToolGraphNode == NULL 
	||  m_pIDMSegment == NULL )
	{
		// Should not happen!
		ASSERT( 0 );
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Get the IDMUSProdToolGraphInfo interface for m_pIToolGraphNode
	IDMUSProdToolGraphInfo* pIToolGraphInfo;
	if( SUCCEEDED ( m_pIToolGraphNode->QueryInterface( IID_IDMUSProdToolGraphInfo, (void**)&pIToolGraphInfo ) ) )
	{
		if( SUCCEEDED ( pIToolGraphInfo->RemoveFromGraphUserList( m_pIDMSegment ) ) )
		{
			hr = S_OK;
		}

		RELEASE( pIToolGraphInfo );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode methods
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode::CompareNodes

HRESULT CSegment::CompareNodes( IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult )
{
	ASSERT(pINode1);
	ASSERT(pINode2);
	
	if( pINode1 == NULL
	||  pINode2 == NULL )
	{
		return E_INVALIDARG;
	}

	if( pnResult == NULL )
	{
		return E_POINTER;
	}
	*pnResult = 0;

	GUID guidNodeId1;
	pINode1->GetNodeId( &guidNodeId1 );

	GUID guidNodeId2;
	pINode2->GetNodeId( &guidNodeId2 );

	if( ::IsEqualGUID(guidNodeId1, GUID_ContainerNode) )
	{
		// Containers always first
		*pnResult = -1;
	}
	else if( ::IsEqualGUID(guidNodeId1, GUID_AudioPathNode) )
	{
		if( ::IsEqualGUID(guidNodeId2, GUID_ToolGraphNode) )
		{
			*pnResult = -1;
		}
		else
		{
			*pnResult = 1;
		}
	}
	else if( ::IsEqualGUID(guidNodeId1, GUID_ToolGraphNode) )
	{
		// ToolGraphs always last
		*pnResult = 1;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDMUSProdGetReferencedNodes methods
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdGetReferencedNodes::GetReferencedNodes

HRESULT STDMETHODCALLTYPE CSegment::GetReferencedNodes(DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// The size must be supplied
	if( pdwArraySize == NULL )
	{
		return E_POINTER;
	}

	// Query for how many array entries are required
	DWORD dwArraySizeRequired = 0;
	DWORD dwIndex = 0;
	DMUSProdReferencedNodes dmpReferencedNodes;
	ZeroMemory( &dmpReferencedNodes, sizeof(DMUSProdReferencedNodes) );

	while( true )
	{
		dmpReferencedNodes.dwErrorLength = dwErrorLength;
		dmpReferencedNodes.wcstrErrorText = wcstrErrorText;
		HRESULT hr = GetParam(GUID_Segment_ReferencedNodes, 0xFFFFFFFF, dwIndex, 0, NULL, &dmpReferencedNodes );

		if( S_OK == hr )
		{
			dwArraySizeRequired += dmpReferencedNodes.dwArraySize;
		}
		else if( S_FALSE != hr )
		{
			break;
		}

		// Increment the stripmgr index and zero the structure
		ZeroMemory( &dmpReferencedNodes, sizeof(DMUSProdReferencedNodes) );
		dwIndex++;
	}

	// Check if there are no referenced nodes
	if( 0 == dwArraySizeRequired )
	{
		*pdwArraySize = 0;
		return S_FALSE;
	}

	// If an insufficient array size is supplied
	if( *pdwArraySize < dwArraySizeRequired )
	{
		// Return the required size
		*pdwArraySize = dwArraySizeRequired;

		// Return S_OK if the array pointer is NULL, otherwise return E_OUTOFMEMORY
		return (ppIDMUSProdNode == NULL) ? S_OK : E_OUTOFMEMORY;
	}

	// Initialize the starting array index to 0, and re-initialize the structure and stripmgr index
	DWORD dwArrayIndex = 0;
	ZeroMemory( &dmpReferencedNodes, sizeof(DMUSProdReferencedNodes) );
	dwIndex = 0;

	// Query the number of entries needed by this stripmgr
	while( true )
	{
		// Set up the structure to show the actual number of entries left, and to point to
		// where in the array to start writing from, and to initialize the error information
		ZeroMemory( &dmpReferencedNodes, sizeof(DMUSProdReferencedNodes) );
		dmpReferencedNodes.dwArraySize = *pdwArraySize - dwArrayIndex;
		dmpReferencedNodes.apIDMUSProdNode = &(ppIDMUSProdNode[dwArrayIndex]);

		// Get the entries
		HRESULT hr = GetParam(GUID_Segment_ReferencedNodes, 0xFFFFFFFF, dwIndex, 0, NULL, &dmpReferencedNodes );
		if( S_OK == hr )
		{
			// Increment the stripmgr index, the array index by the amount filled in, and zero the structure
			dwIndex++;
			dwArrayIndex += dmpReferencedNodes.dwArraySize;
		}
		else if( S_FALSE != hr )
		{
			break;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::PasteCF_CONTAINER

HRESULT CSegment::PasteCF_CONTAINER( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, m_pComponent->m_cfContainer, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Container
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( m_pComponent->m_pIContainerComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::PasteCF_AUDIOPATH

HRESULT CSegment::PasteCF_AUDIOPATH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, m_pComponent->m_cfAudioPath, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new AudioPath
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( m_pComponent->m_pIAudioPathComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );

				if( m_pIAudioPathNode
				&&	(m_dwResolution &= DMUS_SEGF_USE_AUDIOPATH) == 0 )
				{
					m_dwResolution |= DMUS_SEGF_USE_AUDIOPATH;
					m_pIDMSegment->SetDefaultResolution( m_dwResolution );
				}

				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::PasteCF_GRAPH

HRESULT CSegment::PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( m_pComponent->m_pIToolGraphComponent
	&&	SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, m_pComponent->m_cfGraph, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Graph
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( m_pComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::AddAuditionStyleRefTrack

HRESULT CSegment::AddAuditionStyleRefTrack( IDMUSProdNode *pINode )
{
	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	////////////// Add Style track /////////////////////
	IStream* pIMemStream;
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		return E_OUTOFMEMORY;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	CTrack* pTrack = new CTrack;
	ASSERT( pTrack != NULL );
	if ( pTrack == NULL )
	{
		pIMemStream->Release();
		pIRiffStream->Release();
		return E_OUTOFMEMORY;
	}

	//pTrack->m_ckid = 0;
	//pTrack->m_dwGroupBits = 1;
	//pTrack->m_dwPosition = 0;
	pTrack->m_dwProducerOnlyFlags = SEG_PRODUCERONLY_AUDITIONONLY;
	//pTrack->m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
	pTrack->m_fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
	pTrack->m_guidClassID = CLSID_DirectMusicStyleTrack;

	HRESULT hr = E_FAIL;

	MMCKINFO ckTrack;
	ckTrack.fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		delete pTrack;
		return E_FAIL;
	}
	{
		MMCKINFO ckStyleList;
		ckStyleList.fccType = DMUS_FOURCC_STYLE_REF_LIST;
		if( pIRiffStream->CreateChunk( &ckStyleList, MMIO_CREATELIST ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Write 'stmp-ck'
			MMCKINFO ck;
			ck.ckid = mmioFOURCC('s', 't', 'm', 'p');
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				DWORD dwByteCount, dwTime = 0;

				hr = pIMemStream->Write(&dwTime, sizeof(dwTime), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(dwTime))
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}

				if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}


			// Write Reference chunks
			IDMUSProdLoaderRefChunk* pIRefChunkLoader;
			if( SUCCEEDED( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
			{
				pIRefChunkLoader->SaveRefChunkForLoader( pIMemStream,
														 pINode,
														 CLSID_DirectMusicStyle,
														 NULL,
														 WL_DIRECTMUSIC );
				pIRefChunkLoader->Release();
			}

			IDMUSProdFileRefChunk* pIFileRefChunk;
			if ( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
			{
				pIFileRefChunk->SaveRefChunk( pIMemStream, pINode );
				pIFileRefChunk->Release();
			}
		}
		if( pIRiffStream->Ascend( &ckStyleList, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
	}
	if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		delete pTrack;
		return E_FAIL;
	}
	pIRiffStream->Release();
	pTrack->SetStream( pIMemStream );
	pIMemStream->Release();

	hr = AddTrack( pTrack );
	if( FAILED( hr ) )
	{
		delete pTrack;
	}


	////////////// Add Tempo track /////////////////////
	double dblTempo = -1.0;
	IDMUSProdStyleInfo* pIStyleInfo;
	if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
	{
		pIStyleInfo->GetTempo( &dblTempo );
		pIStyleInfo->Release();
	}
	if( dblTempo > 0.0 )
	{
		if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
		{
			return E_OUTOFMEMORY;
		}

		if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
		{
			pIMemStream->Release();
			return E_FAIL;
		}

		pTrack = new CTrack;
		ASSERT( pTrack != NULL );
		if ( pTrack == NULL )
		{
			pIMemStream->Release();
			pIRiffStream->Release();
			return E_OUTOFMEMORY;
		}

		pTrack->m_ckid = DMUS_FOURCC_TEMPO_TRACK;
		pTrack->m_dwGroupBits = 0xFFFFFFFF;
		//pTrack->m_dwPosition = 0;
		pTrack->m_dwProducerOnlyFlags = SEG_PRODUCERONLY_AUDITIONONLY;
		//pTrack->m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
		//pTrack->m_fccType = 0;
		pTrack->m_guidClassID = CLSID_DirectMusicTempoTrack;

		// Create the chunk to store the Tempo data
		ckTrack.ckid = DMUS_FOURCC_TEMPO_TRACK;
		if( pIRiffStream->CreateChunk( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Write the structure size
			DWORD dwBytesWritten;
			DWORD dwTempoSize;
			dwTempoSize = sizeof(DMUS_IO_TEMPO_ITEM);
			hr = pIMemStream->Write( &dwTempoSize, sizeof(DWORD), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}

			DMUS_IO_TEMPO_ITEM oTempo;
			// Clear out the structure (clears out the padding bytes as well).
			ZeroMemory( &oTempo, sizeof(DMUS_IO_TEMPO_ITEM) );
			oTempo.lTime = 0;
			oTempo.dblTempo = dblTempo;

			hr = pIMemStream->Write( &oTempo, sizeof(DMUS_IO_TEMPO_ITEM), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_TEMPO_ITEM) )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
		}
		// Ascend out of the Tempo data chunk.
		if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		pIRiffStream->Release();
		pTrack->SetStream( pIMemStream );
		pIMemStream->Release();

		hr = AddTrack( pTrack );
		if( FAILED( hr ) )
		{
			delete pTrack;
		}
	}


	////////////// Add Band track /////////////////////
	IPersistStream *pIDefaultBandNodeStream = NULL;
	if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
	{
		IUnknown *punkBandNode;
		if( SUCCEEDED( pIStyleInfo->GetDefaultBand( &punkBandNode ) ) )
		{
			punkBandNode->QueryInterface( IID_IPersistStream, (void**)&pIDefaultBandNodeStream );
			punkBandNode->Release();
		}
		pIStyleInfo->Release();
	}
	if( pIDefaultBandNodeStream )
	{
		if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
		{
			pIDefaultBandNodeStream->Release();
			return E_OUTOFMEMORY;
		}

		if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
		{
			pIMemStream->Release();
			pIDefaultBandNodeStream->Release();
			return E_FAIL;
		}

		pTrack = new CTrack;
		ASSERT( pTrack != NULL );
		if ( pTrack == NULL )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			pIDefaultBandNodeStream->Release();
			return E_OUTOFMEMORY;
		}

		//pTrack->m_ckid = 0;
		//pTrack->m_dwGroupBits = 1;
		//pTrack->m_dwPosition = 0;
		pTrack->m_dwProducerOnlyFlags = SEG_PRODUCERONLY_AUDITIONONLY;
		//pTrack->m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
		pTrack->m_fccType = DMUS_FOURCC_BANDTRACK_FORM;
		pTrack->m_guidClassID = CLSID_DirectMusicBandTrack;

		// Create the chunk to store the Band data
		ckTrack.fccType = DMUS_FOURCC_BANDTRACK_FORM;
		if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATERIFF ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			pIDefaultBandNodeStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Create a LIST chunk to wrap the Bands
			MMCKINFO ckList;
			ckList.fccType = DMUS_FOURCC_BANDS_LIST;
			if( pIRiffStream->CreateChunk( &ckList, MMIO_CREATELIST ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				pIDefaultBandNodeStream->Release();
				delete pTrack;
				return E_FAIL;
			}

			{
				// Create a LIST chunk for the Band
				MMCKINFO ckBandList;
				ckBandList.fccType = DMUS_FOURCC_BAND_LIST;
				if( pIRiffStream->CreateChunk( &ckBandList, MMIO_CREATELIST ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					pIDefaultBandNodeStream->Release();
					delete pTrack;
					return E_FAIL;
				}

				{
					// Write 'bd2h-ck'
					MMCKINFO ck2h;
					ck2h.ckid = DMUS_FOURCC_BANDITEM_CHUNK2;
					if( pIRiffStream->CreateChunk( &ck2h, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						pIDefaultBandNodeStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					DWORD dwByteCount;
					DMUS_IO_BAND_ITEM_HEADER2 header2;

					header2.lBandTimePhysical = -1;
					header2.lBandTimeLogical = 0;

					hr = pIMemStream->Write(&header2, sizeof(header2), &dwByteCount);
					if(FAILED(hr) || dwByteCount != sizeof(header2))
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						pIDefaultBandNodeStream->Release();
						delete pTrack;
						return E_FAIL;
					}

					if( pIRiffStream->Ascend( &ck2h, 0 ) != 0 )
					{
						pIRiffStream->Release();
						pIMemStream->Release();
						pIDefaultBandNodeStream->Release();
						delete pTrack;
						return E_FAIL;
					}
				}

				// Now, save the band
				pIDefaultBandNodeStream->Save(pIMemStream, FALSE);

				// Ascend out of the Band LIST chunk.
				if( pIRiffStream->Ascend( &ckBandList, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					pIDefaultBandNodeStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}
			// Ascend out of the Bands list chunk.
			if( pIRiffStream->Ascend( &ckList, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				pIDefaultBandNodeStream->Release();
				delete pTrack;
				return E_FAIL;
			}
		}
		// Ascend out of the Band data chunk.
		if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			pIDefaultBandNodeStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		pIRiffStream->Release();
		pTrack->SetStream( pIMemStream );
		pIMemStream->Release();
		pIDefaultBandNodeStream->Release();

		hr = AddTrack( pTrack );
		if( FAILED( hr ) )
		{
			delete pTrack;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::AddAuditionPersRefTrack

HRESULT CSegment::AddAuditionPersRefTrack( IDMUSProdNode *pINode )
{
	ASSERT(m_pComponent != NULL);
	ASSERT(m_pComponent->m_pIFramework != NULL);

	////////////// Add PersRef track /////////////////////
	IStream* pIMemStream;
	if( FAILED( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIMemStream) ) )
	{
		return E_OUTOFMEMORY;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIMemStream, &pIRiffStream ) ) )
	{
		pIMemStream->Release();
		return E_FAIL;
	}

	CTrack* pTrack = new CTrack;
	ASSERT( pTrack != NULL );
	if ( pTrack == NULL )
	{
		pIMemStream->Release();
		pIRiffStream->Release();
		return E_OUTOFMEMORY;
	}

	//pTrack->m_ckid = 0;
	//pTrack->m_dwGroupBits = 1;
	//pTrack->m_dwPosition = 0;
	pTrack->m_dwProducerOnlyFlags = SEG_PRODUCERONLY_AUDITIONONLY;
	//pTrack->m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
	pTrack->m_fccType = DMUS_FOURCC_PERS_TRACK_LIST;
	pTrack->m_guidClassID = CLSID_DirectMusicChordMapTrack;

	HRESULT hr = E_FAIL;

	MMCKINFO ckTrack;
	ckTrack.fccType = DMUS_FOURCC_PERS_TRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		pIRiffStream->Release();
		pIMemStream->Release();
		delete pTrack;
		return E_FAIL;
	}
	{
		MMCKINFO ckPersList;
		ckPersList.fccType = DMUS_FOURCC_PERS_REF_LIST;
		if( pIRiffStream->CreateChunk( &ckPersList, MMIO_CREATELIST ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
		{
			// Write 'stmp-ck'
			MMCKINFO ck;
			ck.ckid = mmioFOURCC('s', 't', 'm', 'p');
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
			{
				DWORD dwByteCount, dwTime = 0;

				hr = pIMemStream->Write(&dwTime, sizeof(dwTime), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(dwTime))
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}

				if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
				{
					pIRiffStream->Release();
					pIMemStream->Release();
					delete pTrack;
					return E_FAIL;
				}
			}


			// Write Reference chunks
			IDMUSProdLoaderRefChunk* pIRefChunkLoader;
			if( SUCCEEDED( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
			{
				pIRefChunkLoader->SaveRefChunkForLoader( pIMemStream,
														 pINode,
														 CLSID_DirectMusicChordMap,
														 NULL,
														 WL_DIRECTMUSIC );
				pIRefChunkLoader->Release();
			}

			IDMUSProdFileRefChunk* pIFileRefChunk;
			if ( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
			{
				pIFileRefChunk->SaveRefChunk( pIMemStream, pINode );
				pIFileRefChunk->Release();
			}

			if( pIRiffStream->Ascend( &ckPersList, 0 ) != 0 )
			{
				pIRiffStream->Release();
				pIMemStream->Release();
				delete pTrack;
				return E_FAIL;
			}
		}
		if( pIRiffStream->Ascend( &ckTrack, 0 ) != 0 )
		{
			pIRiffStream->Release();
			pIMemStream->Release();
			delete pTrack;
			return E_FAIL;
		}
	}
	pIRiffStream->Release();
	pTrack->SetStream( pIMemStream );
	pIMemStream->Release();

	hr = AddTrack( pTrack );
	if( FAILED( hr ) )
	{
		delete pTrack;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::GetGUID

void CSegment::GetGUID( GUID* pguidSegment )
{
	if( pguidSegment )
	{
		*pguidSegment = m_PPGSegment.guidSegment;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::SetGUID

void CSegment::SetGUID( GUID guidSegment )
{
	m_PPGSegment.guidSegment = guidSegment;

	m_fModified = TRUE;

	// Notify connected nodes that Segment GUID has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::GetData_Boundary

#define APATH_AND_ALWAYS (DMUS_SEGF_USE_AUDIOPATH | DMUS_SEGF_TIMESIG_ALWAYS)

HRESULT CSegment::GetData_Boundary( void** ppData )
{
	PPGTabBoundaryFlags *pPPGTabBoundaryFlags = (PPGTabBoundaryFlags *)*ppData;
	pPPGTabBoundaryFlags->dwPlayFlags = m_dwResolution & ~APATH_AND_ALWAYS;
	pPPGTabBoundaryFlags->dwFlagsUI = PROPF_HAVE_VALID_DATA | PROPF_OMIT_SEGF_DEFAULT;
	pPPGTabBoundaryFlags->strPrompt = m_PPGSegment.strSegmentName;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::SetData_Boundary

HRESULT CSegment::SetData_Boundary( void* pData )
{
	PPGTabBoundaryFlags *pPPGTabBoundaryFlags = (PPGTabBoundaryFlags *)pData;

	// Set the default resolution
	if((m_dwResolution & ~APATH_AND_ALWAYS) != (pPPGTabBoundaryFlags->dwPlayFlags & ~APATH_AND_ALWAYS))
	{
		BOOL fStateSaved = FALSE;
		if( (m_dwResolution & DMUS_SEGF_AFTERPREPARETIME) != (pPPGTabBoundaryFlags->dwPlayFlags & DMUS_SEGF_AFTERPREPARETIME) )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_RESPONSE );
		}
		else if( (m_dwResolution & DMUS_SEGF_VALID_ALL) != (pPPGTabBoundaryFlags->dwPlayFlags & DMUS_SEGF_VALID_ALL) )
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_DEFAULT_ALIGNMENT );
		}
		else
		{
			UpdateSavedState( fStateSaved, IDS_UNDO_RESOLUTION );
		}
		m_fModified = TRUE;
		m_dwResolution &= APATH_AND_ALWAYS;
		m_dwResolution |= pPPGTabBoundaryFlags->dwPlayFlags & ~APATH_AND_ALWAYS;
		m_PPGSegment.dwResolution = m_dwResolution;

		if(m_pIDMSegment != NULL)
		{
			m_pIDMSegment->SetDefaultResolution( m_dwResolution );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::SetSegmentHeaderChunk

void CSegment::SetSegmentHeaderChunk( void )
{
	IDirectMusicSegment8P *pIDirectMusicSegment8P;
	if( m_pIDMSegment
	&&	SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicSegment8P, (void **) &pIDirectMusicSegment8P ) ) )
	{
		DMUS_IO_SEGMENT_HEADER_EX oDMSegment;
		// Prepare DMUS_IO_SEGMENT_HEADER_EX
		memset( &oDMSegment, 0, sizeof(DMUS_IO_SEGMENT_HEADER_EX) );

		oDMSegment.dwRepeats = m_dwLoopRepeats;
		oDMSegment.mtLength = m_mtLength;
		oDMSegment.mtPlayStart = m_mtPlayStart;
		oDMSegment.mtLoopStart = m_mtLoopStart;
		oDMSegment.mtLoopEnd = m_mtLoopEnd;
		oDMSegment.dwResolution = m_dwResolution;
		oDMSegment.dwFlags = m_dwSegmentFlags;
        oDMSegment.rtLoopStart = m_rtLoopStart;
        oDMSegment.rtLoopEnd = m_rtLoopEnd;
		if( m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH )
		{
			oDMSegment.rtLength = m_rtLength;
		}
		else
		{
			oDMSegment.rtLength = 0;
		}

		pIDirectMusicSegment8P->SetHeaderChunk( sizeof( oDMSegment ), (DMUS_IO_SEGMENT_HEADER *) &oDMSegment );
		pIDirectMusicSegment8P->Release();
	}
}


/*
/////////////////////////////////////////////////////////////////////////////
// CSegment::InternalGetStripMgr

HRESULT CSegment::InternalGetStripMgr( GUID guidType, DWORD dwGroupBits, DWORD dwIndex, IDMUSProdStripMgr **ppStripMgr )
{
	IDMUSProdStripMgr *pStripMgr = NULL;
	POSITION pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		CTrack *pTrack = m_lstTracks.GetNext( pos );
		if( pTrack
		&&	(pTrack->m_dwGroupBits & dwGroupBits) )
		{
			pStripMgr = NULL;
			pTrack->GetStripMgr( &pStripMgr );
			if( pStripMgr )
			{
				if ( pStripMgr->IsParamSupported( guidType ) == S_OK )
				{
					if ( dwIndex == 0 )
					{
						*ppStripMgr = pStripMgr;
						return S_OK;
					}

					dwIndex--;
				}
				pStripMgr->Release();
			}
		}
	}

	pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		CTrack *pTrack = m_lstTracks.GetNext( pos );
		if( pTrack
		&&	(pTrack->m_dwGroupBits & dwGroupBits)
		&&	(guidType == pTrack->m_guidClassID) )
		{
			pStripMgr = NULL;
			pTrack->GetStripMgr( &pStripMgr );
			if( pStripMgr )
			{
				if ( dwIndex == 0 )
				{
					*ppStripMgr = pStripMgr;
					return S_OK;
				}

				dwIndex--;
				pStripMgr->Release();
			}
		}
	}

	return E_INVALIDARG;
}
*/


/////////////////////////////////////////////////////////////////////////////
// CSegment::SyncTimelineSettings

void CSegment::SyncTimelineSettings( void )
{
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		VARIANT var;
		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_ZOOM, &var ) ) )
		{
			m_dblZoom = V_R8(&var);
		}

		if( (m_dblZoom > 0.0)
		&&	SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			m_dblHorizontalScroll = V_I4(&var) / m_dblZoom;
		}

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var ) ) )
		{
			m_lVerticalScroll = V_I4(&var);
		}

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
		{
			m_tlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO)V_I4(&var);
		}

		if( SUCCEEDED( m_pSegmentDlg->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
		{
			m_lFunctionbarWidth = V_I4(&var);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegment::GetParam

HRESULT CSegment::GetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pData)
{
	if( m_pSegmentDlg && m_pSegmentDlg->m_pTimeline )
	{
		return m_pSegmentDlg->m_pTimeline->GetParam( rguidType, dwGroupBits, dwIndex, mtTime, pmtNext, pData );
	}

	POSITION position = m_lstTracks.GetHeadPosition();
    while( position )
    {
		CTrack *pTmpTrack = m_lstTracks.GetNext(position);
		if( pTmpTrack->m_dwGroupBits & dwGroupBits )
		{
			IDMUSProdStripMgr *pStripMgr = NULL;
			pTmpTrack->GetStripMgr( &pStripMgr );
			if( pStripMgr )
			{
				if(S_OK == pStripMgr->IsParamSupported( rguidType ) )
				{
					if( dwIndex == 0 )
					{
						HRESULT hr = pStripMgr->GetParam( rguidType, mtTime, pmtNext, pData );
						pStripMgr->Release();
						return hr;
					}
					dwIndex--;
				}
				pStripMgr->Release();
			}
		}
	}

	return E_INVALIDARG;
}

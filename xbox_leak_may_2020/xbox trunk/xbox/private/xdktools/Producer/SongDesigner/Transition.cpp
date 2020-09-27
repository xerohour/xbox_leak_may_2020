// Transition.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SongCtl.h"
#include "SourceSegment.h"
#include "VirtualSegment.h"
#include "Transition.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CTransition Constructor/Destructor

CTransition::CTransition( CDirectMusicSong* pSong )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSong != NULL );
	m_pSong = pSong;
//	m_pSong->AddRef();			intentionally missing

	// UI
	m_dwToSegmentFlag = DMUS_SONG_ANYSEG;
	m_fResolveToSegmentID = false;
	m_dwToSegmentID = 0;
	m_fResolveTransitionSegmentID = false;
	m_dwTransitionSegmentID = 0;
	m_dwBitsUI = 0;

	// DMUS_IO_TRANSITION_DEF
	m_pToSegment = NULL;
	m_pTransitionSegment = NULL;
	m_dwPlayFlagsDM = 0;
}

CTransition::~CTransition( void )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	RELEASE( m_pToSegment );
	RELEASE( m_pTransitionSegment );
}


/////////////////////////////////////////////////////////////////////////////
// CTransition::Copy

void CTransition::Copy( CTransition* pTransition )
{
	ASSERT( pTransition != NULL );
	if ( pTransition == NULL )
	{
		return;
	}

	if( pTransition == this )
	{
		return;
	}

	// UI
	m_dwToSegmentFlag = pTransition->m_dwToSegmentFlag;
	m_fResolveToSegmentID = pTransition->m_fResolveToSegmentID;
	m_dwToSegmentID = pTransition->m_dwToSegmentID;
	m_fResolveTransitionSegmentID = pTransition->m_fResolveTransitionSegmentID;
	m_dwTransitionSegmentID = pTransition->m_dwTransitionSegmentID;
	m_dwBitsUI = pTransition->m_dwBitsUI;

	//		DMUS_IO_Transition_EXTRAS_HEADER
	m_pToSegment = pTransition->m_pToSegment;
	if( m_pToSegment )
	{
		m_pToSegment->AddRef();
	}

	m_pTransitionSegment = pTransition->m_pTransitionSegment;
	if( m_pTransitionSegment )
	{
		m_pTransitionSegment->AddRef();
	}

	m_dwPlayFlagsDM = pTransition->m_dwPlayFlagsDM;
}


/////////////////////////////////////////////////////////////////////////////
// CTransition IUnknown implementation

HRESULT CTransition::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IUnknown *)this;
    }
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CTransition::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CTransition::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


//////////////////////////////////////////////////////////////////////
//	Additional methods

/////////////////////////////////////////////////////////////////////////////
// CTransition FormatTextUI

void CTransition::FormatTextUI( CString& strText )
{
	// Determine "Tran:" Text (or "Intro:" text)
	CString strTemp;
	if( m_dwToSegmentFlag == DMUS_SONG_NOFROMSEG )
	{
		strText.LoadString( IDS_INTRO_TEXT );
	}
	else
	{
		strText.LoadString( IDS_TRAN_TEXT );
	}
	strText += _T(" ");
	if( m_pTransitionSegment )
	{
		m_pTransitionSegment->GetName( strTemp );
	}
	else
	{
		strTemp.LoadString( IDS_NONE_TEXT );
	}
	strText += strTemp;
	strText += _T("   ");

	// Determine "To:" Text
	strTemp.LoadString( IDS_TO_TEXT );
	strText += strTemp;
	strText += _T(" ");
	if( m_pToSegment )
	{
		m_pToSegment->GetName( strTemp );
	}
	else
	{
		if( m_dwToSegmentFlag == DMUS_SONG_ANYSEG )
		{
			strTemp.LoadString( IDS_ANY_TEXT );
		}
		else if( m_dwToSegmentFlag == DMUS_SONG_NOSEG )
		{
			strTemp.LoadString( IDS_NONE_TEXT );
		}
		else if( m_dwToSegmentFlag == DMUS_SONG_NOFROMSEG )
		{
			strTemp.Empty();
			if( m_pSong
			&&  m_pSong->m_pSongCtrl
			&&  m_pSong->m_pSongCtrl->m_pSongDlg )
			{
				CVirtualSegment* pVirtualSegmentToEdit = m_pSong->m_pSongCtrl->m_pSongDlg->GetVirtualSegmentToEdit();
				if( pVirtualSegmentToEdit )
				{
					pVirtualSegmentToEdit->GetName( strTemp );
				}
			}
		}
	}
	strText += strTemp;
}
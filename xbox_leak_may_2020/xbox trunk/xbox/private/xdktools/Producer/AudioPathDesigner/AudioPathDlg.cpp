// AudioPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include <windowsx.h>
#include "dsoundp.h"
#include "AudioPathDesignerDLL.h"
#include "AudioPath.h"
#include "AudioPathCtl.h"
#include "AudioPathDlg.h"
#include "ItemInfo.h"
#include "EffectInfo.h"
#include "RiffStrm.h"
#include "PChannelName.h"
#include "DlgAddPChannel.h"
#include "EffectListDlg.h"
#include "EffectPPGMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ItemInfo *CreateEnvironmentItem( void );

bool BufferSendsTo( const BufferOptions *pBufferOptions, const ItemInfo *pItemInfo )
{
	if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
	{
		return false;
	}

	POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
	while( posEffect )
	{
		BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
		if( pItemInfo == pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup )
		{
			return true;
		}
	}

	return false;
}

int CALLBACK ComparePChannelsInTree(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNREFERENCED_PARAMETER(lParamSort);
	const ItemInfoWrapper *pItemInfoWrapper1 = (ItemInfoWrapper *)lParam1;
	const ItemInfoWrapper *pItemInfoWrapper2 = (ItemInfoWrapper *)lParam2;

	// If the same item
	if( pItemInfoWrapper1->pItemInfo == pItemInfoWrapper2->pItemInfo )
	{
		// Order by buffer index
		int nBufferIndex1 = 0;
		POSITION pos = pItemInfoWrapper1->pItemInfo->lstBuffers.GetHeadPosition();
		while( pos )
		{
			if( pItemInfoWrapper1->pBufferOptions == pItemInfoWrapper1->pItemInfo->lstBuffers.GetNext( pos ) )
			{
				break;
			}
			nBufferIndex1++;
		}

		int nBufferIndex2 = 0;
		pos = pItemInfoWrapper2->pItemInfo->lstBuffers.GetHeadPosition();
		while( pos )
		{
			if( pItemInfoWrapper2->pBufferOptions == pItemInfoWrapper2->pItemInfo->lstBuffers.GetNext( pos ) )
			{
				break;
			}
			nBufferIndex2++;
		}

		return nBufferIndex1 - nBufferIndex2;
	}

	// Otherwise, order by the display index
	// If item1's index is greater than item2's index, this returns a positive result,
	// which means that item1 should go last
	return pItemInfoWrapper1->pItemInfo->dwDisplayIndex - pItemInfoWrapper2->pItemInfo->dwDisplayIndex;

	/*
	if( pItemInfoWrapper1->pItemInfo->nNumPChannels == 0 )
	{
		if( pItemInfoWrapper2->pItemInfo->nNumPChannels == 0 )
		{
			if( BufferSendsTo( pItemInfoWrapper1->pBufferOptions, pItemInfoWrapper2->pItemInfo ) )
			{
				// #1 sends to #2, so it should go last
				return 1;
			}
			if( BufferSendsTo( pItemInfoWrapper2->pBufferOptions, pItemInfoWrapper1->pItemInfo ) )
			{
				// #2 sends to #1, so it should go last
				return -1;
			}

			// Neither have any PChannels - just compare their names
			return _tcscmp( pItemInfoWrapper1->pItemInfo->strBandName, pItemInfoWrapper2->pItemInfo->strBandName );
		}
		else
		{
			// Only #2 has PChannels, so it should go last
			return -1;
		}
	}
	else
	{
		if( pItemInfoWrapper2->pItemInfo->nNumPChannels == 0 )
		{
			// Only #1 has PChannels, so it should go last
			return 1;
		}
		else
		{
			// Both have PChannels - just compare the first PChannel in the sorted array
			return pItemInfoWrapper1->pItemInfo->adwPChannels[0] - pItemInfoWrapper2->pItemInfo->adwPChannels[0];
		}
	}
	*/
}

void PChannelArrayToString( DWORD adwPChannels[], int nNumPChannels, CString &strText )
{
	if( adwPChannels
	&&	nNumPChannels )
	{
		DWORD dwLastPChannel = adwPChannels[0];
		bool fInRange = false;

		char strTemp2[20];
		char strNum[10];
		_itoa( adwPChannels[0] + 1, strNum, 10 );
		strText = strNum;

		for( int iPChannelIndex = 1 ;  iPChannelIndex < nNumPChannels;  iPChannelIndex++ )
		{
			if( (dwLastPChannel + 1) == adwPChannels[iPChannelIndex] )
			{
				dwLastPChannel++;
				fInRange = true;
			}
			else
			{
				if( fInRange )
				{
					// No longer in range
					_itoa( dwLastPChannel + 1, strNum, 10 );
					strcpy( strTemp2, "-" );
					strcat( strTemp2, strNum );
					strcat( strTemp2, ", " );
					_itoa( adwPChannels[iPChannelIndex] + 1, strNum, 10 );
					strcat( strTemp2, strNum );
					strText += strTemp2;
					dwLastPChannel = adwPChannels[iPChannelIndex];
					fInRange = false;
				}
				else
				{
					// No longer in range
					_itoa( adwPChannels[iPChannelIndex] + 1, strNum, 10 );
					strcpy( strTemp2, ", " );
					strcat( strTemp2, strNum );
					strText += strTemp2;
					dwLastPChannel = adwPChannels[iPChannelIndex];
				}
			}
		}

		if( fInRange )
		{
			_itoa( dwLastPChannel + 1, strNum, 10 );
			strcpy( strTemp2, "-" );
			strcat( strTemp2, strNum );
			strText += strTemp2;
		}
	}
}

void BufferToString( const ItemInfo *pItem, const BufferOptions *pBufferOptions, CString &strText )
{
	if( !ItemSupportsBuffers( pItem ) )
	{
		// Try and load the text 'Unsupported'
		strText.LoadString( IDS_BUFFER_UNSUPPORTED );
	}
	else if( !pBufferOptions )
	{
		// Try and load the text 'None'
		strText.LoadString( IDS_BUFFER_NONE );
	}
	else if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
	{
		// Initialize the bus ID text from the GUID of the standard buffer
		if( pBufferOptions->guidBuffer == GUID_Buffer_Reverb )
		{
			strText.LoadString( IDS_BUFFER_REVERB );
		}
		else if( pBufferOptions->guidBuffer == GUID_Buffer_EnvReverb )
		{
			strText.LoadString( IDS_BUFFER_ENVREVERB );
		}
		/*
		else if( pBufferOptions->guidBuffer == GUID_Buffer_3D )
		{
			strText.LoadString( IDS_BUFFER_3D );
		}
		*/
		else if( pBufferOptions->guidBuffer == GUID_Buffer_3D_Dry )
		{
			strText.LoadString( IDS_BUFFER_3D_DRY );
		}
		else if( pBufferOptions->guidBuffer == GUID_Buffer_Mono )
		{
			strText.LoadString( IDS_BUFFER_MONO );
		}
		else if( pBufferOptions->guidBuffer == GUID_Buffer_Stereo )
		{
			strText.LoadString( IDS_BUFFER_STEREO );
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		// Try and load the text 'User Defined'
		strText.LoadString(IDS_BUFFER_USER_DEFINED);
	}
}

void BusListToString( const ItemInfo *pItem, const BufferOptions *pBufferOptions, CString &strText )
{
	if( !ItemSupportsBuffers( pItem ) )
	{
		// Try and load the text 'Unsupported'
		strText.LoadString( IDS_BUFFER_UNSUPPORTED );
	}
	else if( pBufferOptions
	&&	pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
	{
		// Initialize the bus ID text with 'Default'
		strText.LoadString( IDS_BUSID_STANDARD );
	}
	else if( pBufferOptions
	&&	pItem->nNumPChannels > 0
	&&	pBufferOptions->lstBusIDs.GetSize() > 0 )
	{
		if( pBufferOptions->lstBusIDs.GetSize() == 1 )
		{
			// Try and load an engligh version of the bus ID
			if( strText.LoadString( IDS_BUSID0 + pBufferOptions->lstBusIDs[0] ) == 0 )
			{
				// No english version available, just use the number
				strText.Format( _T("%u"), pBufferOptions->lstBusIDs[0] );
			}
		}
		else if( (pBufferOptions->lstBusIDs.GetSize() == 2)
		&&	(pBufferOptions->lstBusIDs[0] == DSBUSID_LEFT)
		&&	(pBufferOptions->lstBusIDs[1] == DSBUSID_RIGHT) )
		{
			strText.LoadString(IDS_BUSID_STEREO);
		}
		else if( (pBufferOptions->lstBusIDs.GetSize() == 2)
		&&	(pBufferOptions->lstBusIDs[0] == DSBUSID_RIGHT)
		&&	(pBufferOptions->lstBusIDs[1] == DSBUSID_LEFT) )

		{
			strText.LoadString( IDS_BUSID_REV_STEREO );
		}
		else
		{
			strText.LoadString(IDS_BUSID_MULTIPLE);
		}

		/*
		CString strTmp;
		bool fHaveOne = false;
		for( int i=0; i < pBufferOptions->lstBusIDs.GetSize(); i++ )
		{
			// Add a comma separator, if necessary
			if( fHaveOne )
			{
				strText += CString(_T(", "));
			}
			else
			{
				fHaveOne = true;
			}

			// Try and load an engligh version of the bus ID
			if( strTmp.LoadString( IDS_BUSID0 + pBufferOptions->lstBusIDs[i] ) == 0 )
			{
				// No english version available, just use the number
				strTmp.Format( _T("%u"), pBufferOptions->lstBusIDs[i] );
			}

			// Add the Bus ID on to the end of the string
			strText += strTmp;
		}
		*/
	}
	else
	{
		// Try and load the text 'none'
		strText.LoadString(IDS_BUSID_NONE);
	}
}

void EffectListToString( const ItemInfo *pItem, const BufferOptions *pBufferOptions, CString &strText )
{
	// Clear the string
	strText.Empty();

	// Check if the item and buffer suppors effects, and if the effects list is non-empty
	if( ItemAndBufferSupportEffects( pItem, pBufferOptions )
	&&	!pBufferOptions->lstEffects.IsEmpty() )
	{
		// String to temporarily store the effect text
		CString strEffects;

		// Iterate through all the effects
		POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each effect
			BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( pos );

			// Check if this is a sending effect
			if( (GUID_DSFX_SEND == pBusEffectInfo->m_EffectInfo.m_clsidObject)
			/*||	(GUID_DSFX_STANDARD_I3DL2SOURCE == pBusEffectInfo->m_EffectInfo.m_clsidObject)*/ )
			{
				if( GUID_DSFX_SEND == pBusEffectInfo->m_EffectInfo.m_clsidObject )
				{
					// Load the "Send" text
					strEffects.LoadString( IDS_SEND_TEXT );
				}
				else
				{
					// Use the instance name
					strEffects = pBusEffectInfo->m_EffectInfo.m_strInstanceName;
				}

				if( NULL != pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup )
				{
					strEffects += _T(" (") + pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup->strBandName + _T(")");
				}
				else if( GUID_Buffer_EnvReverb == pBusEffectInfo->m_EffectInfo.m_clsidSendBuffer )
				{
					CString strReverb;
					strReverb.LoadString( IDS_BUFFER_ENVREVERB );
					strEffects += _T(" (") + strReverb + _T(")");
				}
				else
				{
					CString strExt;
					strExt.LoadString( IDS_SEND_EXTENAL );
					strEffects += strExt;
				}
			}
			else
			{
				strEffects = pBusEffectInfo->m_EffectInfo.m_strInstanceName;
			}

			strText += strEffects;
			if( pos )
			{
				strText += CString(" | ");
			}
		}
	}
}

bool IsItemEnvReverb( const ItemInfo *pItemInfo )
{
	return pItemInfo
		&&	(pItemInfo->lstBuffers.GetCount() == 1)
		&&	(pItemInfo->lstBuffers.GetHead()->guidBuffer == GUID_Buffer_EnvReverb);
}

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg dialog


CAudioPathDlg::CAudioPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioPathDlg::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CAudioPathDlg)
	//}}AFX_DATA_INIT

	m_pAudioPathCtrl = NULL;
	m_pAudioPath = NULL;
	m_pISourceDataObject = NULL;
	m_fEnteredSwitchTo = false;
	m_dwStartDragButton = 0;
	m_cRef = 0;
	AddRef();
}

CAudioPathDlg::~CAudioPathDlg()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
}

HRESULT CAudioPathDlg::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject)
	||	::IsEqualIID(riid, IID_IUnknown) )
	{
		*ppv = (IDMUSProdPropPageObject *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDropSource) )
	{
		*ppv = (IDropSource *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdUpdateObjectNow) )
	{
		*ppv = (IDMUSProdUpdateObjectNow *)this;
	}
	else
	{
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CAudioPathDlg::AddRef( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return InterlockedIncrement( &m_cRef );
}

ULONG CAudioPathDlg::Release( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	const long lRes = InterlockedDecrement( &m_cRef );
	if( lRes == 0 )
	{
		delete this;
	}

	return lRes;
}

HRESULT CAudioPathDlg::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	PPGItemBase *pPPGItemBase = (PPGItemBase *)ppData;

	if( pPPGItemBase == NULL )
	{
		ASSERT(FALSE);
		return E_POINTER;
	}

	switch( pPPGItemBase->m_ppgIndex )
	{
	case PPG_MIX_GROUP:
		{
			MixGroupInfoForPPG *pMixGroupInfoForPPG = (MixGroupInfoForPPG *)pPPGItemBase;
			pMixGroupInfoForPPG->Copy( m_MixGroupInfoForPPG );
		}
		break;
	case PPG_BUFFER:
		{
			BufferInfoForPPG *pBufferInfoForPPG = (BufferInfoForPPG *)pPPGItemBase;
			pBufferInfoForPPG->Copy( m_BufferInfoForPPG );
		}
		break;
	case PPG_EFFECT:
		{
			EffectInfoForPPG *pEffectInfoForPPG = (EffectInfoForPPG *)pPPGItemBase;
			pEffectInfoForPPG->Copy( m_EffectInfoForPPG );
		}
		break;
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
		break;
	}

	return S_OK;
}

HRESULT CAudioPathDlg::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	PPGItemBase *pPPGItemBase = (PPGItemBase *)pData;

	if( pPPGItemBase == NULL )
	{
		ASSERT(FALSE);
		return E_POINTER;
	}

	switch( pPPGItemBase->m_ppgIndex )
	{
	case PPG_MIX_GROUP:
		return SetData_MixGroup( (MixGroupInfoForPPG *)pPPGItemBase );
		break;
	case PPG_BUFFER:
		return SetData_Buffer( (BufferInfoForPPG *)pPPGItemBase );
		break;
	case PPG_EFFECT:
		return SetData_Effect( (EffectInfoForPPG *)pPPGItemBase );
		break;
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
		break;
	}

	return E_FAIL;
}

HRESULT CAudioPathDlg::SetData_MixGroup( MixGroupInfoForPPG *pMixGroupInfoForPPG )
{
	// Get a pointer to the item to change
	ItemInfo *pItemInfoToChange = NULL;
	int nItemSelected = -1;
	POSITION pos = m_tcTree.GetFirstSelectedItemPosition();
	if( pos )
	{
		// Get the index of the selected item
		nItemSelected = m_tcTree.GetNextSelectedItem( pos );
		if( nItemSelected >= 0 )
		{
			// Get a pointer to the selected item
			pItemInfoToChange = m_tcTree.GetItemInfo( nItemSelected );
		}
	}

	if( pItemInfoToChange == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Set to true once an undo state has been saved
	bool fChanged = false;

	// Set to true if the property page needs to be refreshed
	bool fRefreshPPG = false;

	if( pMixGroupInfoForPPG->m_dwChanged & CH_MIXGROUP_NAME )
	{
		// Check if the name is unique
		if( m_pAudioPath->IsMixgroupNameUnique( NULL, pMixGroupInfoForPPG->m_strMixGroupName ) )
		{
			// Name is unique - save undo state
			m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_MIXGROUP_NAME, true );

			// Update the name
			pItemInfoToChange->strBandName = pMixGroupInfoForPPG->m_strMixGroupName;
			m_MixGroupInfoForPPG.m_strMixGroupName = pMixGroupInfoForPPG->m_strMixGroupName;

			UpdateTreeItemName( pItemInfoToChange );

			// Sync change with property sheet title
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				UpdateMixGroupForPPG();
				pIPropSheet->RefreshTitleByObject( this );
				pIPropSheet->Release();
			}
		}
		else
		{
			// Name is not unique - need to refresh property page
			fRefreshPPG = true;
		}
	}

	if( pMixGroupInfoForPPG->m_dwChanged & CH_MIXGROUP_SYNTH )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_MIXGROUP_SYNTH, true );

		// Update the synth settings
		if( pItemInfoToChange->pPortOptions )
		{
			// Check if we changed the port
			if( pItemInfoToChange->pPortOptions->m_guidPort != pMixGroupInfoForPPG->m_PortOptions.m_guidPort )
			{
				pItemInfoToChange->pPortOptions = m_pAudioPath->FindPort( pMixGroupInfoForPPG->m_PortOptions.m_guidPort );

				// If the port supports AudioPaths, ensure it has a buffer
				if( pItemInfoToChange->pPortOptions->m_fAudioPath
				&&	pItemInfoToChange->lstBuffers.IsEmpty() )
				{
					// Create a buffer
					BufferOptions *pBufferOptions = new BufferOptions;

					// Add the buffer to the item
					pItemInfoToChange->lstBuffers.AddTail( pBufferOptions );

					// Point this item at the buffer
					ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( nItemSelected ));
					pItemInfoWrapper->pBufferOptions = pBufferOptions;
				}

				RECT rectItem;
				if( m_tcTree.GetItemRect( nItemSelected, &rectItem, LVIR_BOUNDS ) )
				{
					m_tcTree.InvalidateRect( &rectItem, TRUE );
				}

				// Need to refresh property page so it gets the correct flags set
				fRefreshPPG = true;
			}
			else
			{
				// Update the port's settings
				pItemInfoToChange->pPortOptions->Copy( &pMixGroupInfoForPPG->m_PortOptions );
			}

			m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );
		}
		else
		{
			// Shouldn't happen, since you can't set the port for a mix group with no PChannels
			ASSERT(FALSE);
		}
	}

	if( fRefreshPPG )
	{
		UpdateMixGroupForPPG();
		theApp.m_pAudioPathComponent->m_pIMixGroupPageManager->RefreshData();
	}

	return S_OK;
}

HRESULT CAudioPathDlg::SetData_Buffer( BufferInfoForPPG *pBufferInfoForPPG )
{
	// Get a pointer to the first selected buffer
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	if( m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL ) != BUS_BUFFER_COL )
	{
		// No buffer selected, fail
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Set to true once an undo state has been saved
	bool fChanged = false;

	// Set to true if the property page title should be refreshed
	bool fRefreshTitle = false;

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_HEADERFLAGS )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_SETTINGS, true );

		pBufferOptions->dwHeaderFlags = pBufferInfoForPPG->m_dwHeaderFlags;

		// If the buffer is no longer using a defined buffer type,
		// and the buffer has zero Bus IDs
		if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED)
		&&	(pBufferOptions->lstBusIDs.GetSize() == 0) )
		{
			// Ensure it has at least one Bus
			pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
			pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
			pBufferOptions->wChannels = max( WORD(2), pBufferOptions->wChannels );
		}

		// Resort the display
		m_pAudioPath->UpdateDisplayIndexes();
		m_tcTree.SortItems( ComparePChannelsInTree, 0 );

		// Sync change with property sheet title
		fRefreshTitle = true;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_STANDARDGUID )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_STANDARD_BUFFER_TYPE, true );

		pBufferOptions->guidBuffer = pBufferInfoForPPG->m_guidBufferID;
		pBufferOptions->dwHeaderFlags = pBufferInfoForPPG->m_dwHeaderFlags;

		// Sync change with property sheet title
		fRefreshTitle = true;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_USERGUID )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_GUID, true );

		pBufferOptions->guidBuffer = pBufferInfoForPPG->m_guidBufferID;

		// Update any connected send effects
		m_pAudioPath->UpdateConnectedSendEffects( pItemInfo );
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFERFLAGS )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_SETTINGS, true );

		pBufferOptions->dwBufferFlags = pBufferInfoForPPG->m_dwBufferFlags;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFER3DALG )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_3DALGORITHM, true );

		pBufferOptions->guid3DAlgorithm = pBufferInfoForPPG->m_clsid3dAlgorithm;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFER3DPOS )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_3DPOSITION, true );

		pBufferOptions->ds3DBuffer = pBufferInfoForPPG->m_ds3DBuffer;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFERCHANNELS )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_CHANNELS, true );

		ASSERT( pBufferInfoForPPG->m_dwNumChannels <= USHRT_MAX );
		pBufferOptions->wChannels = WORD(pBufferInfoForPPG->m_dwNumChannels);
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFERPAN )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_PAN, true );

		pBufferOptions->lPan = pBufferInfoForPPG->m_lPan;
	}

	if( pBufferInfoForPPG->m_dwChanged & CH_BUFFER_BUFFERVOLUME )
	{
		// Save undo state
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_BUFFER_VOLUME, true );

		pBufferOptions->lVolume = pBufferInfoForPPG->m_lVolume;
	}

	if( fRefreshTitle )
	{
		// Sync change with property sheet title
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			UpdateBufferForPPG();
			pIPropSheet->RefreshTitleByObject( this );
			pIPropSheet->Release();
		}
	}

	// If anything changed
	if( fChanged )
	{
		// Find the item we changed
		int nItem = FindItem( pItemInfo, pBufferOptions );
		ASSERT( nItem >= 0 );
		if( nItem >= 0 )
		{
			// Get the rect for the item we changed
			RECT rectInvalidate;
			m_tcTree.GetItemRect( nItem, &rectInvalidate, LVIR_BOUNDS );

			// Redraw the item we changed
			m_tcTree.InvalidateRect( &rectInvalidate, TRUE );
		}

		m_pAudioPath->SyncAudiopathsInUse( 0 );	
	}

	return S_OK;
}

HRESULT CAudioPathDlg::SetData_Effect( EffectInfoForPPG *pEffectInfoForPPG )
{
	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	BusEffectInfo *pBusEffectInfo;
	if( m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, &pBusEffectInfo ) != EFFECT_COL )
	{
		// No effect selected, fail
		ASSERT(FALSE);
		return E_FAIL;
	}


	bool fUndoSave = false;
	EffectInfo *pEffectInfoToChange = &(pBusEffectInfo->m_EffectInfo);

	if( pEffectInfoForPPG->m_dwChanged & CH_EFFECT_NAME )
	{
		if( pEffectInfoToChange->m_strInstanceName != pEffectInfoForPPG->m_strInstanceName )
		{
			// Ensure we're not changing the name of the default effect
			ASSERT(pEffectInfoToChange->m_strInstanceName != pEffectInfoToChange->m_strName);
			// Ensure the instance name is unique
			if( m_pAudioPath->IsEffectNameUnique( pEffectInfoToChange, pEffectInfoForPPG->m_strInstanceName ) )
			{
				// Save an undo state
				m_pAudioPath->SaveUndoState( fUndoSave, IDS_UNDO_EFFECT_NAME, true );

				// Copy the instance name
				pEffectInfoToChange->m_strInstanceName = pEffectInfoForPPG->m_strInstanceName;

				// Redraw the PChannel tree with the new instance name
				// TODO: Redraw less of the screen
				InvalidateRect( NULL, TRUE );

				// Sync change with property sheet title
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
				{
					UpdateEffectForPPG();
					pIPropSheet->RefreshTitleByObject( this );
					pIPropSheet->Release();
				}
			}
			else
			{
				// Name already used - update the property page with the old name
				UpdateEffectForPPG();
				theApp.m_pAudioPathComponent->m_pIEffectPageManager->RefreshData();
			}
		}
	}

	if( pEffectInfoForPPG->m_dwChanged & CH_EFFECT_FLAGS )
	{
		if( pEffectInfoToChange->m_dwFlags != pEffectInfoForPPG->m_dwFlags )
		{
			// Rename the EffectInfo, if necessary
			CString strOldName = pEffectInfoToChange->m_strInstanceName;
			if( !m_pAudioPath->IsEffectNameUnique( pEffectInfoToChange, pEffectInfoToChange->m_strInstanceName ) )
			{
				m_pAudioPath->GetUniqueEffectInstanceName( pEffectInfoToChange );
			}

			// Save an undo state
			m_pAudioPath->SaveUndoState( fUndoSave, IDS_UNDO_EFFECT_FLAGS, true );

			// Copy the flags
			pEffectInfoToChange->m_dwFlags = pEffectInfoForPPG->m_dwFlags;

			// Update the PChannel tree, if necessary
			if( strOldName != pEffectInfoToChange->m_strInstanceName )
			{
				int nItem = FindItem( pItemInfo, pBufferOptions );
				if( nItem >= 0 )
				{
					RECT rectItem;
					if( m_tcTree.GetItemRect( nItem, &rectItem, LVIR_BOUNDS ) )
					{
						m_tcTree.InvalidateRect( &rectItem, TRUE );
					}
				}
			}

			m_pAudioPath->SyncAudiopathsInUse( 0 ); 
		}
	}

	if( pEffectInfoForPPG->m_dwChanged & CH_EFFECT_DATA )
	{
		if( pEffectInfoForPPG->m_pIStream
		&&	(pEffectInfoToChange->m_pIStream != pEffectInfoForPPG->m_pIStream) )
		{
			// Rename the EffectInfo, if necessary
			CString strOldName = pEffectInfoToChange->m_strInstanceName;
			if( !m_pAudioPath->IsEffectNameUnique( pEffectInfoToChange, pEffectInfoToChange->m_strInstanceName ) )
			{
				m_pAudioPath->GetUniqueEffectInstanceName( pEffectInfoToChange );
			}

			// Save an undo state
			m_pAudioPath->SaveUndoState( fUndoSave, IDS_UNDO_EFFECT_DATA, true );

			// Release our existing stream, if we have any
			RELEASE( pEffectInfoToChange->m_pIStream );
			
			// Clone the stream
			pEffectInfoForPPG->m_pIStream->Clone( &pEffectInfoToChange->m_pIStream );

			// Update the PChannel tree, if necessary
			if( strOldName != pEffectInfoToChange->m_strInstanceName )
			{
				int nItem = FindItem( pItemInfo, pBufferOptions );
				if( nItem >= 0 )
				{
					RECT rectItem;
					if( m_tcTree.GetItemRect( nItem, &rectItem, LVIR_BOUNDS ) )
					{
						m_tcTree.InvalidateRect( &rectItem, TRUE );
					}
				}
			}

			// Update any DMOs that are in use
			m_pAudioPath->UpdateDMOs( pBusEffectInfo );
		}
	}

	if( pEffectInfoForPPG->m_dwChanged & CH_EFFECT_SEND )
	{
		if( pEffectInfoToChange->m_clsidSendBuffer != pEffectInfoForPPG->m_clsidSendBuffer )
		{
			/* Never necessary to rename the effectinfo
			// Rename the EffectInfo, if necessary
			CString strOldName = pEffectInfoToChange->m_strInstanceName;
			GetUniqueEffectInstanceNameIfNecessary( pEffectInfoToChange );
			*/

			// Save an undo state
			m_pAudioPath->SaveUndoState( fUndoSave, IDS_UNDO_EFFECT_SEND, true );

			// Copy the send GUID
			pEffectInfoToChange->m_clsidSendBuffer = pEffectInfoForPPG->m_clsidSendBuffer;

			// Clear the mix group pointer
			pEffectInfoToChange->m_pSendDestinationMixGroup = NULL;

			// Check if we're not using the standard Env. Reverb GUID
			if( GUID_Buffer_EnvReverb != pEffectInfoToChange->m_clsidSendBuffer )
			{
				// Check to see if the effect is now pointed at any of our mix groups
				POSITION pos = m_pAudioPath->m_lstItems.GetHeadPosition();
				while( pos )
				{
					// Get a pointer to each item
					ItemInfo *pTmpItem = m_pAudioPath->m_lstItems.GetNext( pos );

					// Check if this item is the one with the buffer we want to use
					if( (pTmpItem->nNumPChannels == 0)
					&&	(pTmpItem->lstBuffers.GetCount() > 0)
					&&	(pTmpItem->lstBuffers.GetHead()->guidBuffer == pEffectInfoToChange->m_clsidSendBuffer) )
					{
						// Yes - set the pointer and break out of the loop
						pEffectInfoToChange->m_pSendDestinationMixGroup = pTmpItem;
						break;
					}
				}
			}

			// Update the PChannel tree to display the new send destination
			int nItem = FindItem( pItemInfo, pBufferOptions );
			if( nItem >= 0 )
			{
				RECT rectItem;
				if( m_tcTree.GetItemRect( nItem, &rectItem, LVIR_BOUNDS ) )
				{
					m_tcTree.InvalidateRect( &rectItem, TRUE );
				}
			}

			// Resort the display
			m_pAudioPath->UpdateDisplayIndexes();
			m_tcTree.SortItems( ComparePChannelsInTree, 0 );

			m_pAudioPath->SyncAudiopathsInUse( 0 );	
		}
	}

	return S_OK;
}

HRESULT CAudioPathDlg::OnShowProperties( void )
{
	return E_NOTIMPL;
}

HRESULT CAudioPathDlg::OnRemoveFromPageManager( void )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg IDMUSProdUpdateObjectNow implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg IDMUSProdUpdateObjectNow::UpdateObjectNow

HRESULT CAudioPathDlg::UpdateObjectNow( LPUNKNOWN punkObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate the object pointer
	if( punkObject == NULL )
	{
		return E_POINTER;
	}

	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	BusEffectInfo *pBusEffectInfo;
	if( m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, &pBusEffectInfo ) != EFFECT_COL )
	{
		// No effect selected, fail
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get an EffectInfo pointer
	EffectInfo *pEffectInfoToChange = &pBusEffectInfo->m_EffectInfo;
	if( pEffectInfoToChange == NULL )
	{
		return E_FAIL;
	}

	IPersistStream *pIPersistStream;
	if( SUCCEEDED( punkObject->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
	{
		// Create a stream to store the effect's data in
		IStream *pIStream;
		if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
		{
			// Try and save the effect to a stream
			if( SUCCEEDED( pIPersistStream->Save( pIStream, TRUE ) ) )
			{
				// Seek back to the start of the stream
				if( SUCCEEDED( StreamSeek( pIStream, 0 , STREAM_SEEK_SET ) ) )
				{
					// Always update the object(s) with the new stream

					// Save a pointer to the existing stream, if there is one
					IStream *pIOldStream = pEffectInfoToChange->m_pIStream;
					
					// Point the effect at the new stream
					pEffectInfoToChange->m_pIStream = pIStream;

					// Update only the DMOs that are in use for this BusEffectInfo
					m_pAudioPath->UpdateDMOs( pBusEffectInfo );

					// Point the effect back to the old stream
					pEffectInfoToChange->m_pIStream = pIOldStream;
				}
			}

			// Release the stream
			pIStream->Release();
		}

		// Release the object's IPersistStream interface
		pIPersistStream->Release();
	}

	return S_OK;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::QueryContinueDrag

HRESULT CAudioPathDlg::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton & MK_LBUTTON )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}

		if( !(grfKeyState & MK_LBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	if( m_dwStartDragButton & MK_RBUTTON )
	{
		if( grfKeyState & MK_LBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}
		
		if( !(grfKeyState & MK_RBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::GiveFeedback

HRESULT CAudioPathDlg::GiveFeedback( DWORD dwEffect )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwEffect);

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

void CAudioPathDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioPathDlg)
	DDX_Control(pDX, IDC_TREE_PCHANNELS, m_tcTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioPathDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioPathDlg)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TREE_PCHANNELS, OnSelchangedTreePchannels)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_TREE_PCHANNELS, OnBegindragTreePchannels)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	/*
	ON_NOTIFY(HDN_BEGINTRACK, 0, OnBeginTrackHeader)
	*/
	ON_NOTIFY(HDN_ENDTRACK, 0, OnEndTrackHeader)
	ON_NOTIFY(HDN_DIVIDERDBLCLICK, 0, OnDividerDblClickHeader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::RefreshControls

void CAudioPathDlg::RefreshControls( void )
{
	ASSERT( m_pAudioPath != NULL );

	// Clear our item data pointers
	for( int i=0; i < m_tcTree.GetItemCount(); i++ )
	{
		// Delete wrappers
		m_tcTree.SetItemData( i, 0 );
		delete reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( i ));
	}

	// Remove all items from the tree
	m_tcTree.DeleteAllItems();

	// Repopulate the tree with the new items
	POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( posItem )
	{
		ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );
		InsertItemIntoTree( pItemInfo );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg message handlers

BOOL CAudioPathDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	ASSERT( m_pAudioPath != NULL );

	m_CTreeDropTarget.m_pAudioPathDlg = this;
	::RegisterDragDrop( m_tcTree.GetSafeHwnd(), &m_CTreeDropTarget );

	m_tcTree.SetDMAudioPathDlg( this );

	// Set up the header control
	CDC *pDC = GetDC();

	CString strTitle;
	CSize size;
	size.cx = 80;
	long lWidth;

	// Mix Group column
	strTitle.LoadString( IDS_MIXGROUP_COLUMN );
	if( pDC )
	{
		size = pDC->GetTextExtent( strTitle );
	}
	if( m_pAudioPath->m_wMixGroupWidth )
	{
		lWidth = m_pAudioPath->m_wMixGroupWidth;
	}
	else
	{
		lWidth = max( 115, size.cx );
		m_pAudioPath->m_wMixGroupWidth = WORD(lWidth);
	}
	m_tcTree.InsertColumn( 0, strTitle, LVCFMT_LEFT, lWidth, -1 );

	// Bus column
	strTitle.LoadString( IDS_BUS_COLUMN );
	if( pDC )
	{
		size = pDC->GetTextExtent( strTitle );
	}
	if( m_pAudioPath->m_wBusWidth )
	{
		lWidth = m_pAudioPath->m_wBusWidth;
	}
	else
	{
		lWidth = max( 65, size.cx );
		m_pAudioPath->m_wBusWidth = WORD(lWidth);
	}
	m_tcTree.InsertColumn( 1, strTitle, LVCFMT_LEFT, lWidth, 1 );

	// Buffer column
	strTitle.LoadString( IDS_BUFFER_COLUMN );
	if( pDC )
	{
		size = pDC->GetTextExtent( strTitle );
	}
	if( m_pAudioPath->m_wBufferWidth )
	{
		lWidth = m_pAudioPath->m_wBufferWidth;
	}
	else
	{
		lWidth = max( 110, size.cx );
		m_pAudioPath->m_wBufferWidth = WORD(lWidth);
	}
	m_tcTree.InsertColumn( 2, strTitle, LVCFMT_LEFT, lWidth, 2 );

	// Effects column
	strTitle.LoadString( IDS_EFFECTS_COLUMN );
	if( pDC )
	{
		size = pDC->GetTextExtent( strTitle );
	}
	if( m_pAudioPath->m_wEffectWidth )
	{
		lWidth = m_pAudioPath->m_wEffectWidth;
	}
	else
	{
		lWidth = max( 70, size.cx );
		m_pAudioPath->m_wEffectWidth = WORD(lWidth);
	}
	m_tcTree.InsertColumn( 3, strTitle, LVCFMT_LEFT, lWidth, 3 );

	if( pDC )
	{
		ReleaseDC( pDC );
	}

	POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( posItem )
	{
		ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );
		InsertItemIntoTree( pItemInfo );
	}

	ResizeEffectColumn();

	// If the audiopath is empty, open up the add/remove PChannels dialog
	if( m_pAudioPath->m_lstItems.IsEmpty() )
	{
		::PostMessage( m_tcTree.GetSafeHwnd(), WM_COMMAND, ID_EDIT_INSERT, 0 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnDestroy

void CAudioPathDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Clear our item data pointers
	for( int i=0; i < m_tcTree.GetItemCount(); i++ )
	{
		// Delete wrappers
		ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( i ));
		m_tcTree.SetItemData( i, 0 );
		delete pItemInfoWrapper;
	}

	// Remove MixGroup, Buffer, and Effect from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		if(theApp.m_pAudioPathComponent->m_pIMixGroupPageManager)
		{
			theApp.m_pAudioPathComponent->m_pIMixGroupPageManager->RemoveObject(this);
		}
		if(theApp.m_pAudioPathComponent->m_pIBufferPageManager)
		{
			theApp.m_pAudioPathComponent->m_pIBufferPageManager->RemoveObject(this);
		}
		if(theApp.m_pAudioPathComponent->m_pIEffectPageManager)
		{
			theApp.m_pAudioPathComponent->m_pIEffectPageManager->RemoveObject(this);
		}
		pIPropSheet->Release();
	}

	CDialog::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnSize

void CAudioPathDlg::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnSize(nType, cx, cy);
    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow( m_tcTree.GetSafeHwnd() ) )
	{
		return;
	}

	RECT rect;
	GetClientRect(&rect);
	m_tcTree.MoveWindow(&rect);

	ResizeEffectColumn();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnUpdateEditCut

void CAudioPathDlg::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	if( (tColumn == PCHANNEL_COL)
	||	(tColumn == EFFECT_COL) )
	{
		pCmdUI->Enable( TRUE );
	}
	else //if( tColumn == BUS_BUFFER_COL )
	{
		pCmdUI->Enable( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnEditCut

void CAudioPathDlg::OnEditCut() 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	switch( tColumn )
	{
	case PCHANNEL_COL:
		m_tcTree.OnEditPChannelCut();
		break;
	case EFFECT_COL:
		m_tcTree.OnEditEffectCut();
		break;
	case BUS_BUFFER_COL:
	default:
		ASSERT(FALSE);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnUpdateEditCopy

void CAudioPathDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	if( (tColumn == PCHANNEL_COL)
	||	(tColumn == EFFECT_COL) )
	{
		pCmdUI->Enable( TRUE );
	}
	else //if( tColumn == BUS_BUFFER_COL )
	{
		pCmdUI->Enable( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnEditCopy

void CAudioPathDlg::OnEditCopy() 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	switch( tColumn )
	{
	case PCHANNEL_COL:
		m_tcTree.OnEditPChannelCopy();
		break;
	case EFFECT_COL:
		m_tcTree.OnEditEffectCopy();
		break;
	case BUS_BUFFER_COL:
	default:
		ASSERT(FALSE);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnUpdateEditPaste

void CAudioPathDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( (::IsClipboardFormatAvailable( CDirectMusicAudioPath::m_scfPChannel )
					 || ::IsClipboardFormatAvailable( CDirectMusicAudioPath::m_scfEffect ))? TRUE : FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnEditPaste

void CAudioPathDlg::OnEditPaste() 
{
	ASSERT( m_pAudioPath != NULL );

	if( ::IsClipboardFormatAvailable( CDirectMusicAudioPath::m_scfPChannel ) )
	{
		m_tcTree.OnEditPChannelPaste();
	}
	else if( ::IsClipboardFormatAvailable( CDirectMusicAudioPath::m_scfEffect ) )
	{
		m_tcTree.OnEditEffectPaste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnUpdateEditInsert

void CAudioPathDlg::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( TRUE );

	// Find out what is selected in the tree
	ItemInfo *pItemInfo = NULL;
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( &pItemInfo, NULL, NULL );
	switch( tColumn )
	{
	case BUS_BUFFER_COL:
		// Only can insert a new buffer if the # of pchannels is greater than zero
		pCmdUI->Enable( (pItemInfo->nNumPChannels > 0) ? TRUE : FALSE );
		break;
#ifndef DMP_XBOX
	// Always can insert a new mix group or effect
	default:
	/*
	case EFFECT_COL:
	case PCHANNEL_COL:
	*/
		pCmdUI->Enable( TRUE );
		break;
#else
	case EFFECT_COL:
		pCmdUI->Enable( FALSE );
		break;
	default:
	/*
	case PCHANNEL_COL:
	*/
		pCmdUI->Enable( TRUE );
		break;
#endif
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnEditInsert

void CAudioPathDlg::OnEditInsert() 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	switch( tColumn )
	{
#ifndef DMP_XBOX
	case EFFECT_COL:
		m_tcTree.OnEditEffectInsert();
		break;
#endif
	case PCHANNEL_COL:
	default:
		// Map to inserting a MixGroup, not add/remove PChannels
		m_tcTree.OnEditInsertMixGroup();
		break;
	case BUS_BUFFER_COL:
		m_tcTree.OnEditBufferInsert();
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnUpdateEditDelete

void CAudioPathDlg::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	ItemInfo *pItemInfo;
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( &pItemInfo, NULL, NULL );
	if( (tColumn == PCHANNEL_COL)
	||	(tColumn == EFFECT_COL) )
	{
		pCmdUI->Enable( TRUE );
	}
	else if( tColumn == BUS_BUFFER_COL )
	{
		if(	IsItemEnvReverb( pItemInfo ) )
		{
			pCmdUI->Enable( FALSE );
		}
		else
		{
			pCmdUI->Enable( pItemInfo->lstBuffers.GetCount() > 0 );
		}
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnEditDelete

void CAudioPathDlg::OnEditDelete() 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	switch( tColumn )
	{
	case PCHANNEL_COL:
		m_tcTree.OnEditPChannelDelete();
		break;
	case EFFECT_COL:
		m_tcTree.OnEditEffectDelete();
		break;
	case BUS_BUFFER_COL:
		m_tcTree.OnEditBufferDelete();
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg::OnViewProperties

BOOL CAudioPathDlg::OnViewProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pAudioPath != NULL );
	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	if( FAILED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;
	if( SUCCEEDED ( m_pAudioPath->OnShowProperties() ) )
	{
		fSuccess = TRUE;
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}

void CAudioPathDlg::OnSelchangedTreePchannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 0;

	static bool fEntered;
	if( fEntered )
	{
		return;
	}

	NMLISTVIEW *pNMLISTVIEW = (NMLISTVIEW*)pNMHDR;
	if( pNMLISTVIEW->iItem >= 0 )
	{
		if( pNMLISTVIEW->uChanged & LVIF_STATE )
		{
			if( (pNMLISTVIEW->uNewState & LVIS_SELECTED) != (pNMLISTVIEW->uOldState & LVIS_SELECTED) )
			{
				m_tcTree.InvalidateItem( m_tcTree.GetItemInfo( pNMLISTVIEW->iItem ) );
			}
		}
	}

	fEntered = true;

	m_pAudioPathCtrl->SwitchToCorrectPropertyPage();

	fEntered = false;
}

void CAudioPathDlg::OnBegindragTreePchannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pNMHDR);

	ASSERT(m_tcTree.GetSelectedCount() > 0);

	if( SUCCEEDED( m_tcTree.CreateDataObject( &m_pISourceDataObject ) ) )
	{
		// Begin Drag operation
		DWORD dwEffect = DROPEFFECT_NONE;
		m_dwStartDragButton = MK_LBUTTON;
		HRESULT hr = ::DoDragDrop( m_pISourceDataObject, this, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect );

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					// TODO:
					//m_pTimeSigMgr->DeleteMarked( UD_DRAGSELECT );
				}
				break;

			default:
				dwEffect = DROPEFFECT_NONE;
				break;
		}

		RELEASE( m_pISourceDataObject );

		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CAudioPathDlg::UpdateTreeItemName( const ItemInfo *pItemInfo )
{
	if( pItemInfo == NULL )
	{
		return;
	}

	// Look for any effects that send to this item
	POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pTmpItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );

		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pTmpItemInfo ) )
		{
			continue;
		}

		// Iterate through the list of buffers
		POSITION posBuffer = pTmpItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pTmpItemInfo->lstBuffers.GetNext( posBuffer );

			// If the buffer does not support effects, skip it
			if( !ItemAndBufferSupportEffects( pTmpItemInfo, pBufferOptions ) )
			{
				continue;
			}

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this effect sends to the mix group that changed
				if( pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup == pItemInfo )
				{
					// Redraw this item
					int nItem = FindItem( pTmpItemInfo, pBufferOptions );
					if( nItem >= 0 )
					{
						// Get the rect for the item we changed
						RECT rectInvalidate;
						m_tcTree.GetItemRect( nItem, &rectInvalidate, LVIR_BOUNDS );

						// Redraw the item we changed
						m_tcTree.InvalidateRect( &rectInvalidate, TRUE );
					}
					break;
				}
			}
		}
	}

	CString strItemName;

	if( pItemInfo->nNumPChannels )
	{
		PChannelArrayToString( pItemInfo->adwPChannels, pItemInfo->nNumPChannels, strItemName );

		strItemName += CString(_T(": "));

		strItemName += pItemInfo->strBandName;
	}
	else
	{
		strItemName = pItemInfo->strBandName;
	}

	// If the item has zero buffers
	if( pItemInfo->lstBuffers.IsEmpty() )
	{
		// Update the item's name
		int nItem = FindItem( pItemInfo, NULL );
		ASSERT( nItem >= 0 );
		m_tcTree.SetItemText( nItem, 0, strItemName );

		// Set the bus text
		CString strTemp;
		BusListToString( pItemInfo, NULL, strTemp );
		m_tcTree.SetItemText( nItem, 1, strTemp );

		// Set the buffer text
		BufferToString( pItemInfo, NULL, strTemp );
		m_tcTree.SetItemText( nItem, 2, strTemp );

		// Set the effect text
		m_tcTree.SetItemText( nItem, 3, NULL );
	}
	else
	{
		POSITION pos = pItemInfo->lstBuffers.GetHeadPosition();
		while( pos )
		{
			const BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( pos );
			int nItem = FindItem( pItemInfo, pBufferOptions );
			ASSERT( nItem >= 0 );
			m_tcTree.SetItemText( nItem, 0, strItemName );

			// Set the bus text
			CString strTemp;
			BusListToString( pItemInfo, pBufferOptions, strTemp );
			m_tcTree.SetItemText( nItem, 1, strTemp );

			// Set the buffer text
			BufferToString( pItemInfo, pBufferOptions, strTemp );
			m_tcTree.SetItemText( nItem, 2, strTemp );

			// Set the effect text
			EffectListToString( pItemInfo, pBufferOptions, strTemp );
			m_tcTree.SetItemText( nItem, 3, strTemp );
		}
	}
}

void CAudioPathDlg::UpdateTreeItemNames( void )
{
	// Iterate through all items
	POSITION pos = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( pos )
	{
		// Update the name for each item
		UpdateTreeItemName( m_pAudioPath->m_lstItems.GetNext( pos ) );
	}
}

HRESULT ReadInItems( IStream *pStream, CTypedPtrList< CPtrList, ItemInfo *> &lstNewItems, CDirectMusicAudioPath *pAudioPath )
{
	STATSTG statStg;
	long lStreamSize = 0;
	if( FAILED( pStream->Stat( &statStg, STATFLAG_NONAME ) ) )
	{
		return E_FAIL;
	}

	lStreamSize = statStg.cbSize.LowPart;

	// Seek back to the start of the stream
	if( FAILED( StreamSeek( pStream, 0 , STREAM_SEEK_SET ) ) )
	{
		return E_FAIL;
	}

	// Allocate a RIFF stream
	IDMUSProdRIFFStream *pRiffStream;
	AllocRIFFStream( pStream, &pRiffStream );

	// Initialize the result to return
	HRESULT hr = S_OK;

	// Now, load in all items to paste
	while( lStreamSize > 0 )
	{
		// Find the DMUSPROD_FOURCC_ITEMINFO_LIST chunk
		MMCKINFO ckItemList;
		ckItemList.fccType = DMUSPROD_FOURCC_ITEMINFO_LIST;
		if( 0 == pRiffStream->Descend( &ckItemList, NULL, MMIO_FINDLIST ) )
		{
			ItemInfo *pItemInfo = new ItemInfo();
			hr = pItemInfo->Read( pRiffStream, &ckItemList, pAudioPath );
			if( SUCCEEDED( hr ) )
			{
				lstNewItems.AddTail( pItemInfo );
			}
			else
			{
				delete pItemInfo;
				break;
			}

			lStreamSize -= ckItemList.cksize + 8;

			pRiffStream->Ascend( &ckItemList, 0 );
		}
		else
		{
			hr = E_FAIL;
			break;
		}
	}

	pRiffStream->Release();

	if( FAILED(hr) )
	{
		while( !lstNewItems.IsEmpty() )
		{
			delete lstNewItems.RemoveHead();
		}
	}

	return hr;
}

HRESULT CAudioPathDlg::DropOnTree( IDataObject* pIDataObject, DROPEFFECT dropEffect, POINT point)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(dropEffect);

	// BUGBUG: Use dropEffect (creating a send effect)

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDllDataObject = new CDllJazzDataObject();
	if( !pDllDataObject )
	{
		return E_OUTOFMEMORY;
	}

	// Try and get data in the the PChannel format
	IStream *pIStream = NULL;
	if( SUCCEEDED( pDllDataObject->AttemptRead( pIDataObject, CDirectMusicAudioPath::m_scfPChannel, &pIStream ) ) )
	{
		// Check to see if the PChannel items is dropped on an existing item
		UINT flags;
		int nTargetItem = m_tcTree.HitTest(point, &flags);
		if( nTargetItem < 0 )
		{
			// Dropping a MixGroup from this tree into a blank spot in the tree - no result
			if( m_pISourceDataObject == pIDataObject )
			{
				delete pDllDataObject;
				pIStream->Release();
				return S_FALSE;
			}
		}

		PastePChannels( pIStream, point );
	}
	else if( SUCCEEDED( pDllDataObject->AttemptRead( pIDataObject, CDirectMusicAudioPath::m_scfEffect, &pIStream ) ) )
	{
		UINT flags;
		int nTargetItem = m_tcTree.HitTest(point, &flags);
		if( nTargetItem >= 0 )
		{
			// Save an undo state
			m_pAudioPath->SaveUndoState( IDS_UNDO_DROP_EFFECTS, true );

			BusEffectInfo *pDroppedOnBusEffectInfo;
			RECT rectDroppedOnEffect;
			m_tcTree.GetEffectRect( point, nTargetItem, &pDroppedOnBusEffectInfo, &rectDroppedOnEffect );

			ItemInfo *pItemInfo = m_tcTree.GetItemInfo( nTargetItem );

			CTypedPtrList<CPtrList, EffectInfo *> lstEffects;
			EffectInfo *pNewEffectInfo = m_pAudioPath->LoadEffect( pIStream );
			while( pNewEffectInfo )
			{
				//SetI3DL2SrcDestinationIfNecessary( pNewEffectInfo );
				lstEffects.AddTail( pNewEffectInfo );
				pNewEffectInfo = m_pAudioPath->LoadEffect( pIStream );
			}

			if( !lstEffects.IsEmpty() )
			{
				// Ensure only the dropped effects are selected
				m_tcTree.UnselectAll();
			}

			if( pItemInfo && pDroppedOnBusEffectInfo && !lstEffects.IsEmpty() )
			{
				POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
				while( posBuffer )
				{
					BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

					POSITION pos = pBufferOptions->lstEffects.Find( pDroppedOnBusEffectInfo );
					if( pos )
					{
						while( !lstEffects.IsEmpty() )
						{
							// Create a BusEffectInfo to wrap the dropped EffectInfo
							pNewEffectInfo = lstEffects.RemoveHead();
							BusEffectInfo *pBusEffectInfo = new BusEffectInfo(pNewEffectInfo);

							// Make this effect selected
							pBusEffectInfo->m_beiSelection = BEI_NORMAL;

							if( point.x + GetScrollPos(SB_HORZ) <= rectDroppedOnEffect.left + pDroppedOnBusEffectInfo->m_lPixelWidth / 2 )
							{
								pBufferOptions->lstEffects.InsertBefore( pos, pBusEffectInfo );
							}
							else
							{
								pBufferOptions->lstEffects.InsertAfter( pos, pBusEffectInfo );
							}

							// Ensure the effect name is really unique
							m_pAudioPath->GetUniqueEffectInstanceNameIfNecessary( &pBusEffectInfo->m_EffectInfo );

							// Only need the EffectInfo during the load
							delete pNewEffectInfo;
						}
						break;
					}
				}
			}
			else if( pItemInfo && !pItemInfo->lstBuffers.IsEmpty() && !lstEffects.IsEmpty() )
			{
				// Get the wrapper structure for the item
				// lstOldItems
				ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( nTargetItem ));
				if( pItemInfoWrapper )
				{
					// Pointer to each buffer
					BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

					// Add the effect to the buffer, if it supports effects,
					// and we can get the column widths
					long lMixGroupWidth, lBusWidth, lBufferWidth;
					if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions )
					&&	GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth ) )
					{
						// Determine whether to add to the start or end of the effect list
						const bool fAddToStart = (point.x + GetScrollPos(SB_HORZ)) <= lMixGroupWidth + lBusWidth + lBufferWidth;

						while( !lstEffects.IsEmpty() )
						{
							// Create a BusEffectInfo to wrap the dropped EffectInfo
							if( fAddToStart )
							{
								pNewEffectInfo = lstEffects.RemoveTail();
							}
							else
							{
								pNewEffectInfo = lstEffects.RemoveHead();
							}
							BusEffectInfo *pBusEffectInfo = new BusEffectInfo(pNewEffectInfo);

							// Make this effect selected
							pBusEffectInfo->m_beiSelection = BEI_NORMAL;

							if( fAddToStart )
							{
								pBufferOptions->lstEffects.AddHead( pBusEffectInfo );
							}
							else
							{
								pBufferOptions->lstEffects.AddTail( pBusEffectInfo );
							}

							// Ensure the effect name is really unique
							m_pAudioPath->GetUniqueEffectInstanceNameIfNecessary( &pBusEffectInfo->m_EffectInfo );

							delete pNewEffectInfo;
						}
					}
				}
			}

			while( !lstEffects.IsEmpty() )
			{
				// Ensure we don't leak any memory
				delete lstEffects.RemoveHead();
			}

			if( pItemInfo )
			{
				// Update the internal names, so that column resize works correctly
				UpdateTreeItemName( pItemInfo );

				// Resize the effect column, since an effect may have been added
				ResizeEffectColumn();
			}

			m_tcTree.InvalidateItem( nTargetItem );

			// Resort the display
			m_pAudioPath->UpdateDisplayIndexes();
			m_tcTree.SortItems( ComparePChannelsInTree, 0 );

			m_pAudioPath->SyncAudiopathsInUse( 0 );	

			// Set the focus to the mix group
			m_pAudioPathCtrl->SetLastFocus( FOC_MIXGROUP );

			// Switch to the correct property page
			m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
		}
	}

	// If necessary, release our reference to the stream
	if( pIStream )
	{
		pIStream->Release();
	}

	// Delete the DLLDataObject we used to read in the data
	delete pDllDataObject;

	return S_OK;
}

void CAudioPathDlg::InsertPChannel( POINT pointInsert )
{
	// Convert from screen to client coordinates
	m_tcTree.ScreenToClient( &pointInsert );

	// Get the item inserted on
	UINT uFlags;
	int nItem = m_tcTree.HitTest( pointInsert, &uFlags );

	// Check if we found an item
	ItemInfo *pItemInfoToEdit = NULL;
	if( nItem >= 0  )
	{
		pItemInfoToEdit = m_tcTree.GetItemInfo( nItem );
	}

	// Initialize the add PChannel dialog
	CDlgAddPChannel dlgAddPChannel;

	// Set the PChannelName pointer
	IDMUSProdProject *pIDMUSProdProject;
	if( SUCCEEDED( theApp.m_pAudioPathComponent->m_pIFramework->FindProject( m_pAudioPath, &pIDMUSProdProject ) ) )
	{
		pIDMUSProdProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&dlgAddPChannel.m_pIPChannelName );
		pIDMUSProdProject->Release();
	}

	// Set up the array of existing PChannels
	POSITION pos = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		const ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( pos );

		// Skip the item we're editing
		if( pItemInfo != pItemInfoToEdit )
		{
			// Iterate through all PChannels
			for( int i=0; i < pItemInfo->nNumPChannels; i++ )
			{
				// Add each PChannel to the array (order is irrelevant)
				dlgAddPChannel.m_adwExistingPChannels.Add( pItemInfo->adwPChannels[i] );
			}
		}
	}

	// Set up the array of PChannels for the item we're adding
	if( pItemInfoToEdit )
	{
		// Itrate through all PChannels
		for( int i=0; i < pItemInfoToEdit->nNumPChannels; i++ )
		{
			// Add each PChannel to the array (order is irrelevant)
			dlgAddPChannel.m_adwPChannels.Add( pItemInfoToEdit->adwPChannels[i] );
		}
	}

	if( dlgAddPChannel.DoModal() == IDOK )
	{
		bool fChanged = false;
		if( !pItemInfoToEdit )
		{
			// Create a new Mix Group with these PChannels
			m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_INSERT_MIXGROUP, true );

			// Create a new item
			pItemInfoToEdit = new ItemInfo();
			if( pItemInfoToEdit )
			{
				// Get a mix group name for the PChannel
				pItemInfoToEdit->strBandName = m_pAudioPath->GetNewMixgroupName();

				// Get the default port
				pItemInfoToEdit->pPortOptions = m_pAudioPath->GetDefaultPort();

				// Create a buffer
				BufferOptions *pBufferOptions = new BufferOptions;

#ifdef DMP_XBOX
				// If the buffer will have zero PChannels
				if( 0 == dlgAddPChannel.m_adwPChannels.GetSize() )
				{
					// Must be shared I3DL2 Reverb
					pBufferOptions->dwHeaderFlags |= DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_SHARED;

					// Change the buffer's GUID
					pBufferOptions->guidBuffer = GUID_Buffer_EnvReverb;

					// Verify that either DSBCAPS_CTRL3D or DSBCAPS_CTRLPAN is set
					if( !(pBufferOptions->dwBufferFlags & DSBCAPS_CTRL3D)
					&&	!(pBufferOptions->dwBufferFlags & DSBCAPS_CTRLPAN) )
					{
						pBufferOptions->dwBufferFlags |= DSBCAPS_CTRLPAN;
					}

					// Ensure neither DSBCAPS_STATIC nor DSBCAPS_LOCDEFER are set
					pBufferOptions->dwBufferFlags &= ~(DSBCAPS_STATIC | DSBCAPS_LOCDEFER);

					// Remove all Bus IDs
					pBufferOptions->lstBusIDs.RemoveAll();

					// Ensure it has two channels
					pBufferOptions->wChannels = 2;
				}
#endif //DMP_XBOX

				// Add the buffer to the item
				pItemInfoToEdit->lstBuffers.AddTail( pBufferOptions );

				// Add the item to the AudioPath
				m_pAudioPath->InsertItemInfo( pItemInfoToEdit );

				// Add the item to the tree
				InsertItemIntoTree( pItemInfoToEdit );
			}
		}

		// This handles both inserting a new MixGroup and inserting PChannels
		if( pItemInfoToEdit )
		{
			// Get the new number of PChannels
			const long lNewPChannelCount = dlgAddPChannel.m_adwPChannels.GetSize();

			// Check if anything changed
			if( (pItemInfoToEdit->nNumPChannels != lNewPChannelCount)
			||	(memcmp( pItemInfoToEdit->adwPChannels, dlgAddPChannel.m_adwPChannels.GetData(), sizeof(DWORD) * pItemInfoToEdit->nNumPChannels )) )
			{
				// Save an undo state
				m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_ADDREMOVE_PCHANNELS, true );

				// Create a new array to store the PChannels in
				DWORD *adwNewPChannels = NULL;
				if( lNewPChannelCount )
				{
					adwNewPChannels = new DWORD[lNewPChannelCount];
				}
				if( (lNewPChannelCount == 0)
				||	adwNewPChannels )
				{
					// Copy the PChannels into the new array
					if( lNewPChannelCount )
					{
						memcpy( adwNewPChannels, dlgAddPChannel.m_adwPChannels.GetData(), sizeof(DWORD) * lNewPChannelCount );
					}

					// Delete the old array
					delete[] pItemInfoToEdit->adwPChannels;

					// Save the old number of PChannels
					const long lOldNumPChannels = pItemInfoToEdit->nNumPChannels;

					// Set the size of, and point to the new array
					pItemInfoToEdit->adwPChannels = adwNewPChannels;
					pItemInfoToEdit->nNumPChannels = lNewPChannelCount;

					// Flag if we need to refresh the entire mixgroup list
					bool fRefreshControls = false;

					if( 0 == lOldNumPChannels )
					{
						// Going from zero PChannels - update all send effects in case this
						// mix group was sent to
						m_pAudioPath->DisconnectSendEffects( pItemInfoToEdit );
					}
					else if( 0 == lNewPChannelCount )
					{
						// Going to zero PChannels

						// Ensure that there are no standard buffers
						POSITION posBuffer = pItemInfoToEdit->lstBuffers.GetHeadPosition();
						while( posBuffer )
						{
							BufferOptions *pBufferOptions = pItemInfoToEdit->lstBuffers.GetNext( posBuffer );
							if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
							{
#ifdef DMP_XBOX
								// Must be shared I3DL2 Reverb
								pBufferOptions->dwHeaderFlags |= DMUS_BUFFERF_SHARED;

								// Change the buffer's GUID
								pBufferOptions->guidBuffer = GUID_Buffer_EnvReverb;

								// Verify that either DSBCAPS_CTRL3D or DSBCAPS_CTRLPAN is set
								if( !(pBufferOptions->dwBufferFlags & DSBCAPS_CTRL3D)
								&&	!(pBufferOptions->dwBufferFlags & DSBCAPS_CTRLPAN) )
								{
									pBufferOptions->dwBufferFlags |= DSBCAPS_CTRLPAN;
								}

								// If the buffer has zero Bus IDs
								if( pBufferOptions->lstBusIDs.GetSize() == 0 )
								{
									// Ensure it has at least two Buses
									pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
									pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
									pBufferOptions->wChannels = 2;
								}
#else //DMP_XBOX
								pBufferOptions->dwHeaderFlags &= ~(DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_SHARED);

								// Change the buffer's GUID
								CoCreateGuid( &pBufferOptions->guidBuffer );

								// Verify that either DSBCAPS_CTRL3D or DSBCAPS_CTRLPAN is set
								if( !(pBufferOptions->dwBufferFlags & DSBCAPS_CTRL3D)
								&&	!(pBufferOptions->dwBufferFlags & DSBCAPS_CTRLPAN) )
								{
									pBufferOptions->dwBufferFlags |= DSBCAPS_CTRLPAN;
								}

								// If the buffer has zero Bus IDs
								if( pBufferOptions->lstBusIDs.GetSize() == 0 )
								{
									// Ensure it has at least two Buses
									pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
									pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
									pBufferOptions->wChannels = max( WORD(2), pBufferOptions->wChannels );
								}
#endif //DMP_XBOX
							}
						}

						// Split off any buffers (other than the first one) into new mix groups
						posBuffer = pItemInfoToEdit->lstBuffers.GetHeadPosition();
						pItemInfoToEdit->lstBuffers.GetNext( posBuffer );
						while( posBuffer )
						{
							POSITION posCurrent = posBuffer;
							BufferOptions *pBufferOptions = pItemInfoToEdit->lstBuffers.GetNext( posBuffer );

							// Create a new item
							ItemInfo *pNewItemInfo = new ItemInfo();
							if( pNewItemInfo )
							{
								// Get a mix group name for the PChannel
								pNewItemInfo->strBandName = m_pAudioPath->GetNewMixgroupName();

								// Get the default port
								pNewItemInfo->pPortOptions = m_pAudioPath->GetDefaultPort();

								// Add the buffer to the item
								pNewItemInfo->lstBuffers.AddTail( pBufferOptions );

								// Add the item to the AudioPath
								m_pAudioPath->InsertItemInfo( pNewItemInfo );

								// Add the item to the tree
								InsertItemIntoTree( pNewItemInfo );

								// Remove the buffer from the main item
								pItemInfoToEdit->lstBuffers.RemoveAt( posCurrent );

								fRefreshControls = true;
							}
						}

						// Connect any send effects that were broken
						m_pAudioPath->ConnectAllSendEffects();
					}

					// If the item has PChannels
					if( pItemInfoToEdit->nNumPChannels )
					{
						// Ensure that all its buffers have BusIDs
						POSITION posBuffer = pItemInfoToEdit->lstBuffers.GetHeadPosition();
						while( posBuffer )
						{
							BufferOptions *pBufferOptions = pItemInfoToEdit->lstBuffers.GetNext( posBuffer );

							// If not using a defined buffer
							if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
							{
								if( pBufferOptions->lstBusIDs.GetSize() == 0 )
								{
									pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
									pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
									pBufferOptions->wChannels = max( WORD(2), pBufferOptions->wChannels );
								}
							}
						}

						// Ensure that it has a port
						if( pItemInfoToEdit->pPortOptions == NULL )
						{
							pItemInfoToEdit->pPortOptions = m_pAudioPath->GetDefaultPort();
						}
					}
					// If the item has zero PChannels
					else
					{
						// Ensure it doesn not have a port
						pItemInfoToEdit->pPortOptions = NULL;

						// Ensure all its buffers have zero Bus IDs
						POSITION posBuffer = pItemInfoToEdit->lstBuffers.GetHeadPosition();
						while( posBuffer )
						{
							BufferOptions *pBufferOptions = pItemInfoToEdit->lstBuffers.GetNext( posBuffer );
							
							// If the buffer has some Bus IDs
							if( pBufferOptions->lstBusIDs.GetSize() > 0 )
							{
								// Set the # of channels in the buffer
								pBufferOptions->wChannels = WORD(pBufferOptions->lstBusIDs.GetSize());

								// Remove all Bus IDs
								pBufferOptions->lstBusIDs.RemoveAll();
							}

							// Ensure neither DSBCAPS_STATIC nor DSBCAPS_LOCDEFER are set
							pBufferOptions->dwBufferFlags &= ~(DSBCAPS_STATIC | DSBCAPS_LOCDEFER);
						}
					}

					if( fRefreshControls )
					{
						RefreshControls();
					}
					else
					{
						// Update the name of the item
						UpdateTreeItemName( pItemInfoToEdit );

						// Resort the tree
						m_pAudioPath->UpdateDisplayIndexes();
						m_tcTree.SortItems( ComparePChannelsInTree, 0 );
					}
				}
			}
		}

		m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );
	}
}

void CAudioPathDlg::InsertEnvReverb()
{
	if( m_pAudioPath )
	{
		// Save an undo state
		bool fChanged = false;
		m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_INSERT_MIXGROUP, true );

		// Create a new item
		ItemInfo *pItemInfo = CreateEnvironmentItem();

		if( pItemInfo )
		{
			// Add the item to the AudioPath
			m_pAudioPath->InsertItemInfo( pItemInfo );

			// Add the item to the tree
			InsertItemIntoTree( pItemInfo );

			m_pAudioPath->SyncAudiopathsInUse( 0 );
		}
	}
}

void CAudioPathDlg::PastePChannels( IStream *pIStream, POINT pointPaste )
{
	// The list of PChannels to insert
	CTypedPtrList< CPtrList, ItemInfo *> lstNewItems;

	// Try and read in the list of PChannel items to insert (make sure the list is non-empty).
	if( SUCCEEDED( ReadInItems( pIStream, lstNewItems, m_pAudioPath ) )
	&&	!lstNewItems.IsEmpty() )
	{
		// Save an undo state
		m_pAudioPath->SaveUndoState( IDS_UNDO_DROP_PCHANNELS, true );

		// Check to see if the PChannel items is dropped on an existing item
		UINT flags;
		int nTargetItem = m_tcTree.HitTest(pointPaste, &flags);
		if( nTargetItem >= 0 )
		{
			ItemInfo *pTargetItem = m_tcTree.GetItemInfo( nTargetItem );
			ASSERT( pTargetItem );

			// Yes - add the PChannel items to this item
			while( !lstNewItems.IsEmpty() )
			{
				// Get a pointer to each item
				ItemInfo *pNewItem = lstNewItems.RemoveHead();

				// Iterate through all PChannels in each new item
				for( int i=0; i < pNewItem->nNumPChannels; i++ )
				{
					// Check if the PChannel already exists in the target item
					bool fAlreadyInTargetItem = false;
					if( pTargetItem->adwPChannels != NULL
					&&	pTargetItem->nNumPChannels > 0 )
					{
						for( long lIndex = 0; lIndex < pTargetItem->nNumPChannels; lIndex++ )
						{
							if( pTargetItem->adwPChannels[lIndex] == pTargetItem->adwPChannels[i] )
							{
								fAlreadyInTargetItem = true;
								break;
							}
						}
					}

					if( !fAlreadyInTargetItem )
					{
						// Delete the PChannel, if it exists
						DeletePChannel( pNewItem->adwPChannels[i] );

						// Add each new PChannel to nTargetItem
						InsertPChannelIntoItem( pTargetItem, pNewItem->adwPChannels[i] );
					}
				}

				// Delete the new item, since it's no longer needed
				delete pNewItem;
			}
		}
		else
		{
			// No - just add the PChannel items to the main part of the tree
			while( !lstNewItems.IsEmpty() )
			{
				// Remove the item from the list of new items
				ItemInfo *pNewItem = lstNewItems.RemoveHead();

				// Ensure we don't insert Env. Reverb more than once.
				if( IsItemEnvReverb( pNewItem )
				&&	HasEnvReverb() )
				{
					continue;
				}

				// Iterate through all PChannels in each new item
				for( int i=0; i < pNewItem->nNumPChannels; i++ )
				{
					// Delete the PChannel, if it exists
					DeletePChannel( pNewItem->adwPChannels[i] );
				}

				// Iterate through the buffers and give new GUIDs to all non-shared and non-defined buffers
				POSITION posBuffer = pNewItem->lstBuffers.GetHeadPosition();
				while( posBuffer )
				{
					BufferOptions *pBufferOptions = pNewItem->lstBuffers.GetNext( posBuffer );
					if( 0 == (pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
					{
						CoCreateGuid( &pBufferOptions->guidBuffer );
					}
				}

				// Add the item to the list of items in the AudioPath
				m_pAudioPath->InsertItemInfo( pNewItem );

				// Add the item to the PChannel tree
				InsertItemIntoTree( pNewItem );
			}
		}

		m_pAudioPath->UpdateDisplayIndexes();
		m_tcTree.SortItems( ComparePChannelsInTree, 0 );

		m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );
	}
}

void CAudioPathDlg::PastePChannelFromClipboard( POINT pointPaste )
{
	// Get the IDataObject from the clipboard
	IDataObject *pIDataObject = NULL;
	if( FAILED(OleGetClipboard(&pIDataObject))
	||	(pIDataObject == NULL) )
	{
		return;
	}

	FORMATETC formatEtc;
	STGMEDIUM stgMedium;

	formatEtc.cfFormat = CDirectMusicAudioPath::m_scfPChannel;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_ISTREAM;

	// Try and get data in the the PChannel format, ensuring that it is of a stream type
	if( SUCCEEDED( pIDataObject->GetData( &formatEtc, &stgMedium ) )
	&&	(stgMedium.tymed == TYMED_ISTREAM) )
	{
		m_tcTree.ScreenToClient( &pointPaste );
		PastePChannels( stgMedium.pstm, pointPaste );
	}

	pIDataObject->Release();
}

void CAudioPathDlg::PasteEffectFromClipboard( POINT pointPaste )
{
	// Get the IDataObject from the clipboard
	IDataObject *pIDataObject = NULL;
	if( FAILED(OleGetClipboard(&pIDataObject))
	||	(pIDataObject == NULL) )
	{
		return;
	}

	// Convert from screen to client coordinates
	m_tcTree.ScreenToClient( &pointPaste );

	DropOnTree( pIDataObject, DROPEFFECT_COPY, pointPaste );

	pIDataObject->Release();
}

int CAudioPathDlg::FindItem( const ItemInfo *pItemInfo, const BufferOptions *pBufferOptions )
{
	ASSERT( pItemInfo );

	// Iterate through our items
	for( int i=0; i < m_tcTree.GetItemCount(); i++ )
	{
		// Check if this is the item we're looking for
		const ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( i ));
		if( pItemInfoWrapper )
		{
			if( (pItemInfoWrapper->pItemInfo == pItemInfo)
			&&	(!pBufferOptions || (pItemInfoWrapper->pBufferOptions == pBufferOptions)) )
			{
				return i;
			}
		}
	}

	return -1;
}

void CAudioPathDlg::InsertItemIntoTree( ItemInfo *pItem )
{
	ASSERT( pItem );

	if( pItem->lstBuffers.IsEmpty() )
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM | LVIF_TEXT;
		lvItem.iItem = 0;
		lvItem.iSubItem = 0;
		lvItem.pszText = pItem->strBandName.GetBuffer( 0 );
		lvItem.lParam = DWORD(new ItemInfoWrapper(pItem, 0));
		m_tcTree.InsertItem( &lvItem );
		pItem->strBandName.ReleaseBuffer( -1 );
	}
	else
	{
		POSITION pos = pItem->lstBuffers.GetHeadPosition();
		while( pos )
		{
			BufferOptions *pBufferOptions = pItem->lstBuffers.GetNext( pos );
			LVITEM lvItem;
			lvItem.mask = LVIF_PARAM | LVIF_TEXT;
			lvItem.iItem = 0;
			lvItem.iSubItem = 0;
			lvItem.pszText = pItem->strBandName.GetBuffer( 0 );
			lvItem.lParam = DWORD(new ItemInfoWrapper(pItem, pBufferOptions));
			m_tcTree.InsertItem( &lvItem );
			pItem->strBandName.ReleaseBuffer( -1 );
		}
	}

	UpdateTreeItemName( pItem );
	m_pAudioPath->UpdateDisplayIndexes();
	m_tcTree.SortItems( ComparePChannelsInTree, 0 );
}

void CAudioPathDlg::InsertPChannelIntoItem( ItemInfo *pItemInfo, DWORD dwPChannel )
{
	// Verify the pointer to the band item
	if( pItemInfo == NULL )
	{
		return;
	}

	// Create a new array of PChannels (since we added one)
	DWORD *adwPChannels = new DWORD[pItemInfo->nNumPChannels + 1];

	// Copy the old array of PChannels into the new array, inserting the new PChannel
	// where appropriate
	for( long i=0; i < pItemInfo->nNumPChannels; i++ )
	{
		if( pItemInfo->adwPChannels[i] > dwPChannel )
		{
			memcpy( adwPChannels, pItemInfo->adwPChannels, sizeof(DWORD) * max( 0, i ) );
			adwPChannels[i] = dwPChannel;
			memcpy( &(adwPChannels[i+1]), &(pItemInfo->adwPChannels[i]), sizeof(DWORD) * max( 0, pItemInfo->nNumPChannels - i ) );
			break;
		}
	}

	// New PChannel goes at the end of the array
	if( i == pItemInfo->nNumPChannels )
	{
		memcpy( adwPChannels, pItemInfo->adwPChannels, sizeof(DWORD) * i );
		adwPChannels[i] = dwPChannel;
	}

	// Delete old array
	delete[] pItemInfo->adwPChannels;

	// Point the parent to the new array
	pItemInfo->adwPChannels = adwPChannels;
	pItemInfo->nNumPChannels++;

	// Ensure that all the buffers of this item have BusIDs
	POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
	while( posBuffer )
	{
		BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

		// If not using a defined buffer
		if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
		{
			if( pBufferOptions->lstBusIDs.GetSize() == 0 )
			{
				pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
				pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
				pBufferOptions->wChannels = max( WORD(2), pBufferOptions->wChannels );
			}
		}
	}

	// Ensure that it has a port
	if( pItemInfo->pPortOptions == NULL )
	{
		pItemInfo->pPortOptions = m_pAudioPath->GetDefaultPort();
	}

	// Need to update the name of the parent
	UpdateTreeItemName( pItemInfo );

	m_pAudioPath->UpdateDisplayIndexes();
	m_tcTree.SortItems( ComparePChannelsInTree, 0 );
}

void CAudioPathDlg::DeletePChannel( DWORD dwPChannel )
{
	// Get a handle to the first item
	for( int iItemToDelete = 0; iItemToDelete < m_tcTree.GetItemCount(); iItemToDelete++ )
	{
		// Get a pointer to this mix group item
		ItemInfo *pItemRootInfo = m_tcTree.GetItemInfo( iItemToDelete );

		if( pItemRootInfo == NULL )
		{
			// Move on to check the next mix group item
			continue;
		}

		for( int i=0; i < pItemRootInfo->nNumPChannels; i++ )
		{
			if( pItemRootInfo->adwPChannels[i] == dwPChannel )
			{
				// Check if this is the only PChannel in the mix group
				if( pItemRootInfo->nNumPChannels == 1 )
				{
					// Only PChannel in mix group - remove the entire group.
					const POSITION posToRemove = m_pAudioPath->m_lstItems.Find( pItemRootInfo );
					ASSERT( posToRemove );
					m_pAudioPath->m_lstItems.RemoveAt( posToRemove );

					// Delete item from list control

					// Iterate through our items
					for( int iToDel=m_tcTree.GetItemCount() - 1; iToDel >= 0; iToDel-- )
					{
						// Check if this is the item we're looking for
						ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(m_tcTree.GetItemData( iToDel ));
						if( pItemInfoWrapper
						&&	pItemInfoWrapper->pItemInfo == pItemRootInfo )
						{
							// Yes - delete the item
							m_tcTree.DeleteItem( iToDel );
							delete pItemInfoWrapper;
						}
					}

					iItemToDelete = 0;
					delete pItemRootInfo;
				}
				else
				{
					// More than one PChannel in this mix group - remove only this PChannel

					// Decrement the number of PChannels in the mix group
					pItemRootInfo->nNumPChannels--;

					// Create a new array of PChannels
					DWORD *adwPChannels = new DWORD[pItemRootInfo->nNumPChannels];

					// Copy all but the PChannel we're deleting to the new array
					int j=0;
					for( int k=0; k < pItemRootInfo->nNumPChannels + 1; k++ )
					{
						if( pItemRootInfo->adwPChannels[k] != dwPChannel )
						{
							adwPChannels[j] = pItemRootInfo->adwPChannels[k];
							j++;
						}
					}

					// Delete the parent's existing array
					delete[] pItemRootInfo->adwPChannels;

					// Replace the parent's PChannel array
					pItemRootInfo->adwPChannels = adwPChannels;

					// Update the name of the parent
					UpdateTreeItemName( pItemRootInfo );
				}

				// Can only have one instance of a PChannel, so return
				return;
			}
		}
	}
}

void CAudioPathDlg::UpdateBufferForPPG( void )
{
	// Point the item at the correct audio path
	m_BufferInfoForPPG.m_pAudioPath = m_pAudioPath;
	m_BufferInfoForPPG.m_strAudioPathName = m_pAudioPath->m_strName;
	m_BufferInfoForPPG.m_dwChanged = 0;

	// Get a pointer to the first selected buffer
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	if( m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL ) == BUS_BUFFER_COL )
	{
		// If the item doesn't support buffers
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			m_BufferInfoForPPG.m_fValid = false;
		}
		else
		{
			m_BufferInfoForPPG.Import( pBufferOptions, pItemInfo->nNumPChannels > 0 );
			m_BufferInfoForPPG.m_fValid = true;
		}
	}
	else
	{
		m_BufferInfoForPPG.m_fValid = false;
	}
}

void CAudioPathDlg::SwitchToBufferPPG( void ) 
{
	// Fix 30394: Keep this method from being called from itself (or another SwitchTo() method)
	if( m_fEnteredSwitchTo )
	{
		return;
	}

	m_fEnteredSwitchTo = true;

	UpdateBufferForPPG();

	// Change to the buffer property page, if the property sheet is visible
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		// Check if property sheet is visible
		if( pIPropSheet->IsShowing() == S_OK )
		{
			// Change to the Buffer property page manager
			short nActiveTab = CBufferPPGMgr::sm_nActiveTab;
			if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIBufferPageManager) ) )
			{
				// Point the Buffer property page manager back at this
				theApp.m_pAudioPathComponent->m_pIBufferPageManager->SetObject( this );

				// Set the correct active property page tab
				pIPropSheet->SetActivePage( nActiveTab ); 
			}

			// Refresh the property sheet title
			pIPropSheet->RefreshTitleByObject( this );

			// Refresh the property sheet page
			pIPropSheet->RefreshActivePageByObject( this );
		}

		pIPropSheet->Release();
	}

	m_fEnteredSwitchTo = false;
}

void CAudioPathDlg::UpdateEffectForPPG( void )
{
	// Point the item at the correct audio path
	m_EffectInfoForPPG.m_pAudioPath = m_pAudioPath;
	m_EffectInfoForPPG.m_strAudioPathName = m_pAudioPath->m_strName;
	m_EffectInfoForPPG.m_dwChanged = 0;

	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	BusEffectInfo *pBusEffectInfo;
	if( m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, &pBusEffectInfo ) == EFFECT_COL )
	{
		// If the item doesn't support effects
		if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
		{
			m_EffectInfoForPPG.m_fValid = false;
		}
		else
		{
			m_EffectInfoForPPG.Import( pBusEffectInfo->m_EffectInfo );
			m_EffectInfoForPPG.m_guidMyBuffer = pBufferOptions->guidBuffer;
			m_EffectInfoForPPG.m_fValid = true;
		}
	}
	else
	{
		m_EffectInfoForPPG.m_fValid = false;
	}
}

void CAudioPathDlg::SwitchToEffectPPG( void ) 
{
	// Fix 30394: Keep this method from being called from itself (or another SwitchTo() method)
	if( m_fEnteredSwitchTo )
	{
		return;
	}

	m_fEnteredSwitchTo = true;

	UpdateEffectForPPG();

	// Change to the effect property page, if the property sheet is visible
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		// Check if property sheet is visible
		if( pIPropSheet->IsShowing() == S_OK )
		{
			// Change to the Effect property page manager
			short nActiveTab = CEffectPPGMgr::sm_nActiveTab;
			if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIEffectPageManager) ) )
			{
				// Point the Effect property page manager back at this
				theApp.m_pAudioPathComponent->m_pIEffectPageManager->SetObject( this );

				// Set the correct active property page tab
				pIPropSheet->SetActivePage( nActiveTab ); 
			}

			// Refresh the property sheet title
			pIPropSheet->RefreshTitleByObject( this );

			// Refresh the property sheet page
			pIPropSheet->RefreshActivePageByObject( this );
		}

		pIPropSheet->Release();
	}

	m_fEnteredSwitchTo = false;
}

void CAudioPathDlg::UpdateMixGroupForPPG( void )
{
	// Get a pointer to the first selected item
	const ItemInfo *pItemInfo = m_tcTree.GetFirstSelectedMixGroupItem();

	// Point the item at the correct audio path
	m_MixGroupInfoForPPG.m_pAudioPath = m_pAudioPath;
	m_MixGroupInfoForPPG.m_strAudioPathName = m_pAudioPath->m_strName;
	m_MixGroupInfoForPPG.m_dwChanged = 0;

	// Check if there is a selected item
	if( pItemInfo )
	{
		// Copy the selected item to m_MixGroupInfoForPPG
		m_MixGroupInfoForPPG.m_strMixGroupName = pItemInfo->strBandName;
		m_MixGroupInfoForPPG.m_PortOptions.Copy( pItemInfo->pPortOptions );
		m_MixGroupInfoForPPG.m_dwFlags = pItemInfo->pPortOptions ? MGI_HAS_SYNTH : 0;
		m_MixGroupInfoForPPG.m_dwFlags |= ItemSupportsBuffers( pItemInfo ) ? 0 : MGI_CAN_EDIT_SYNTH;
		m_MixGroupInfoForPPG.m_dwFlags |= IsItemEnvReverb( pItemInfo ) ? MGI_DONT_EDIT_NAME : 0;
		m_MixGroupInfoForPPG.m_fValid = true;
	}
	else
	{
		m_MixGroupInfoForPPG.m_fValid = false;
	}
}

void CAudioPathDlg::SwitchToMixGroupPPG( void ) 
{
	// Fix 30394: Keep this method from being called from itself (or another SwitchTo() method)
	if( m_fEnteredSwitchTo )
	{
		return;
	}

	m_fEnteredSwitchTo = true;

	UpdateMixGroupForPPG();

	// Change to the mix group property page, if the property sheet is visible
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		// Check if property sheet is visible
		if( pIPropSheet->IsShowing() == S_OK )
		{
			// Change to the MixGroup property page manager
			short nActiveTab = CMixGroupPPGMgr::sm_nActiveTab;
			if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIMixGroupPageManager) ) )
			{
				// Point the MixGroup property page manager back at this
				theApp.m_pAudioPathComponent->m_pIMixGroupPageManager->SetObject( this );

				// Set the correct active property page tab
				pIPropSheet->SetActivePage( nActiveTab ); 
			}

			// Refresh the property sheet title
			pIPropSheet->RefreshTitleByObject( this );

			// Refresh the property sheet page
			pIPropSheet->RefreshActivePageByObject( this );
		}

		pIPropSheet->Release();
	}

	m_fEnteredSwitchTo = false;
}

void CAudioPathDlg::OnEditSelectAll() 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL );
	if( tColumn == EFFECT_COL )
	{
		// Make only these effects selected
		m_tcTree.UnselectAll();

		POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
		while( pos )
		{
			pBufferOptions->lstEffects.GetNext( pos )->m_beiSelection = BEI_NORMAL;
		}

		m_tcTree.InvalidateItem( pItemInfo );
	}
	else //if( (tColumn == PCHANNEL_COL) || (tColumn == BUS_BUFFER_COL) )
	{
		m_tcTree.SelectAll();
	}
}

void CAudioPathDlg::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	ASSERT( m_pAudioPath != NULL );

	// Find out what is selected in the tree
	TreeColumn tColumn = m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL );
	if( tColumn == EFFECT_COL )
	{
		pCmdUI->Enable( TRUE );
	}
	else //if( (tColumn == PCHANNEL_COL) || (tColumn == BUS_BUFFER_COL) )
	{
		pCmdUI->Enable( (m_tcTree.GetItemCount() > 0) ? TRUE : FALSE );
	}
}

bool CAudioPathDlg::HasEnvReverb( void ) 
{
	// Search for EnvReverb
	POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( posItem )
	{
		ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );

		// Only need to check the first buffer
		if( pItemInfo->lstBuffers.GetCount() > 0 )
		{
			if( pItemInfo->lstBuffers.GetHead()->guidBuffer == GUID_Buffer_EnvReverb )
			{
				return true;
			}
		}
	}

	return false;
}

void CAudioPathDlg::SwitchToCorrectPropertyPage( void )
{
	// Find out what is selected in the tree
	switch( m_tcTree.GetFirstSelectedThing( NULL, NULL, NULL ) )
	{
	case PCHANNEL_COL:
		SwitchToMixGroupPPG();
		break;
	case EFFECT_COL:
		SwitchToEffectPPG();
		break;
	case BUS_BUFFER_COL:
		SwitchToBufferPPG();
		break;
	default:
		// Remove MixGroup, Buffer, and Effect from property sheet
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RemovePageManagerByObject( this );
			pIPropSheet->Release();
		}
		break;
	}
}

bool CAudioPathDlg::GetColumnWidths( long &lMixGroup, long &lBus, long &lBuffer )
{
	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_WIDTH;
	if( m_tcTree.GetColumn( 0, &lvColumn ) )
	{
		lMixGroup = lvColumn.cx;
		if( m_tcTree.GetColumn( 1, &lvColumn ) )
		{
			lBus = lvColumn.cx;
			if( m_tcTree.GetColumn( 2, &lvColumn ) )
			{
				lBuffer = lvColumn.cx;
				return true;
			}
		}
	}

	return false;
}

/*
void CAudioPathDlg::OnBeginTrackHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NMHEADER *pNMHEADER = (NMHEADER *)pNMHDR;

	// Disable resizing Effects list header
	*pResult = (pNMHEADER->iItem == 3) ? 1 : 0;
}
*/

void CAudioPathDlg::OnEndTrackHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NMHEADER *pNMHEADER = (NMHEADER *)pNMHDR;

	ResizeEffectColumn();

	*pResult = 0;

	if( pNMHEADER->pitem
	&&	(pNMHEADER->pitem->mask & HDI_WIDTH) )
	{
		switch( pNMHEADER->iItem )
		{
		case 0:
			if( m_pAudioPath->m_wMixGroupWidth != WORD(pNMHEADER->pitem->cxy) )
			{
				m_pAudioPath->SetModified( TRUE );
				m_pAudioPath->m_wMixGroupWidth = WORD(pNMHEADER->pitem->cxy);
			}
			break;
		case 1:
			if( m_pAudioPath->m_wBusWidth != WORD(pNMHEADER->pitem->cxy) )
			{
				m_pAudioPath->SetModified( TRUE );
				m_pAudioPath->m_wBusWidth = WORD(pNMHEADER->pitem->cxy);
			}
			break;
		case 2:
			if( m_pAudioPath->m_wBufferWidth != WORD(pNMHEADER->pitem->cxy) )
			{
				m_pAudioPath->SetModified( TRUE );
				m_pAudioPath->m_wBufferWidth = WORD(pNMHEADER->pitem->cxy);
			}
			break;
		case 3:
			if( m_tcTree.GetColumnWidth( 3 ) != pNMHEADER->pitem->cxy )
			{
				pNMHEADER->pitem->cxy = m_tcTree.GetColumnWidth( 3 );
			}
			*pResult = 1;
			break;
		}
	}
}

void CAudioPathDlg::OnDividerDblClickHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NMHEADER *pNMHEADER = (NMHEADER *)pNMHDR;

	*pResult = 0;

	WORD wWidth = WORD(m_tcTree.GetColumnWidth( pNMHEADER->iItem ));

	switch( pNMHEADER->iItem )
	{
	case 0:
		if( m_pAudioPath->m_wMixGroupWidth != wWidth )
		{
			m_pAudioPath->SetModified( TRUE );
			m_pAudioPath->m_wMixGroupWidth = wWidth;
		}
		break;
	case 1:
		if( m_pAudioPath->m_wBusWidth != wWidth )
		{
			m_pAudioPath->SetModified( TRUE );
			m_pAudioPath->m_wBusWidth = wWidth;
		}
		break;
	case 2:
		if( m_pAudioPath->m_wBufferWidth != wWidth )
		{
			m_pAudioPath->SetModified( TRUE );
			m_pAudioPath->m_wBufferWidth = wWidth;
		}
		break;
		/*
	case 3:
		if( m_pAudioPath->m_wEffectWidth != wWidth )
		{
			m_pAudioPath->SetModified( TRUE );
			m_pAudioPath->m_wEffectWidth = wWidth;
		}
		break;
		*/
	}

	ResizeEffectColumn();
}

/*
void CAudioPathDlg::SetI3DL2SrcDestinationIfNecessary( EffectInfo *pEffectInfoToChange )
{
	if( pEffectInfoToChange
	&&	(pEffectInfoToChange->m_clsidObject == GUID_DSFX_STANDARD_I3DL2SOURCE )
	&&	(pEffectInfoToChange->m_clsidSendBuffer == GUID_NULL) )
	{
		// Iterate through the list of items
		POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
		while( posItem )
		{
			// Get a pointer to each item
			ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );

			// If the port doesn't support buffers, skip it
			if( !ItemSupportsBuffers( pItemInfo ) )
			{
				continue;
			}

			// If the port has any PChannels, skip it
			if( pItemInfo->nNumPChannels > 0 )
			{
				continue;
			}

			// Iterate through the list of buffers
			POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
			DWORD dwBufferNum = 0;
			while( posBuffer )
			{
				// Get a pointer to each buffer
				BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

				// Iterate through the list of effects
				POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
				DWORD dwEffectNum = 0;

				// If the buffer does not support effects, don't check any of the effects in the buffer
				if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
				{
					posEffect = NULL;
				}

				while( posEffect )
				{
					// Get a pointer to each effect
					BusEffectInfo *pBusEffectInfoInList = pBufferOptions->lstEffects.GetNext( posEffect );

					// Check if this is the effect we're looking for
					if( pBusEffectInfoInList->m_EffectInfo.m_clsidObject == GUID_DSFX_STANDARD_I3DL2REVERB )
					{
						// Found an I3DL2 reverb effect
						pEffectInfoToChange->m_clsidSendBuffer = pBufferOptions->guidBuffer;
						pEffectInfoToChange->m_pSendDestinationMixGroup = pItemInfo;
						return;
					}

					// Increment the effect index
					dwEffectNum++;
				}

				// Increment the buffer index
				dwBufferNum++;
			}
		}

		// Didn't find an I3DL2 reverb effect, just use the standard env. reverb buffer GUID
		pEffectInfoToChange->m_clsidSendBuffer = GUID_Buffer_EnvReverb;
		pEffectInfoToChange->m_pSendDestinationMixGroup = NULL;
	}
}
*/

void CAudioPathDlg::ResizeEffectColumn( void )
{
	// Resize the effect column to fill up the empty space
	long lMixGroup, lBus, lBuffer;
	if( GetColumnWidths( lMixGroup, lBus, lBuffer ) )
	{
		// Get the dialog's client rect
		RECT rect;
		GetClientRect(&rect);

		// Set the effect column's width
		m_tcTree.SetColumnWidth( 3, max( GetMinEffectWidth(), rect.right - rect.left + GetScrollPos(SB_HORZ) - lMixGroup - lBus - lBuffer ) );
	}
}

int CAudioPathDlg::GetMinEffectWidth( void )
{
	int nMaxWidth = 1;
	CString strEffectText;

	POSITION posItem = m_pAudioPath->m_lstItems.GetHeadPosition();
	while( posItem )
	{
		ItemInfo *pItemInfo = m_pAudioPath->m_lstItems.GetNext( posItem );

		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			EffectListToString( pItemInfo, pBufferOptions, strEffectText );

			const int nEffectWidth = m_tcTree.GetStringWidth( strEffectText );
			if( nEffectWidth > nMaxWidth )
			{
				nMaxWidth = nEffectWidth;
			}
		}
	}

	return nMaxWidth;
}

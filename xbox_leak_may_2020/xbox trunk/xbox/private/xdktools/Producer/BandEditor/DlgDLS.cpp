// DlgDLS.cpp : implementation file
//

#include "stdafx.h"
#include "bandeditordll.h"
#include "BandDlg.h"
#include "DlgDLS.h"
#include "DLSDesigner.h"
#include "Band.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDLS dialog

CDlgDLS::CDlgDLS(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDLS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDLS)
	m_nLSB = 0;
	m_nMSB = 0;
	m_nPC = 0;
	//}}AFX_DATA_INIT
	m_pIFramework = theApp.m_pIFramework;
	m_pICurrentRefNode = NULL;

	// Init the collection list now...
	// Don't do this in the init dialog since the 
	// loading of DLS collections may throw up other
	// modal dialogs resulting in problematic behaviour...
	InitCollectionList();
}

CDlgDLS::~CDlgDLS()
{
	if( m_pICurrentRefNode )
	{
		m_pICurrentRefNode->Release();
		m_pICurrentRefNode = NULL;
	}

	COLLECTION_LISTITEM*	pNextItem;
	
	while ( !m_CollectionList.IsEmpty() )
	{
		pNextItem = m_CollectionList.RemoveHead();
		pNextItem->pRefNode->Release();
		delete pNextItem;
	}

	CleanInstrumentList();

}

void CDlgDLS::CleanInstrumentList()
{
	INSTRUMENT_LISTITEM* pInstrument = NULL;
	while(!m_InstrumentList.IsEmpty())
	{
		pInstrument = m_InstrumentList.RemoveHead();
		if(pInstrument)
			delete pInstrument;
	}
}

void CDlgDLS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDLS)
	DDX_Control(pDX, IDC_DLSLIST, m_cList);
	DDX_Control(pDX, IDC_COLLECTIONS_COMBO, m_DLS_Combo);
	DDX_Text(pDX, IDC_DLSLSB, m_nLSB);
	DDV_MinMaxInt(pDX, m_nLSB, 0, 127);
	DDX_Text(pDX, IDC_DLSMSB, m_nMSB);
	DDX_Text(pDX, IDC_DLSPC, m_nPC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDLS, CDialog)
	//{{AFX_MSG_MAP(CDlgDLS)
	ON_LBN_DBLCLK(IDC_DLSLIST, OnDblclkDlslist)
	ON_LBN_SELCHANGE(IDC_DLSLIST, OnSelchangeDlslist)
	ON_CBN_SELCHANGE(IDC_COLLECTIONS_COMBO, OnSelchangeCollectionsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDLS message handlers

//To Do: correct this
void CDlgDLS::OnDblclkDlslist() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	DWORD dwData = m_cList.GetItemData( m_cList.GetCurSel() );
	m_nMSB = (dwData >> 16) & 0x7F;
	m_nLSB = (dwData >> 8) & 0x7F;
	m_nPC = dwData & 0x7F;
	EndDialog( IDOK );
}

void CDlgDLS::GetCurrentCollection() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set current collection
	int nCurSel = m_DLS_Combo.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		IDMUSProdReferenceNode* pIRefNode;
		
		// Current Collection RefNode
		pIRefNode = (IDMUSProdReferenceNode *)m_DLS_Combo.GetItemDataPtr( nCurSel );
		if( pIRefNode != m_pICurrentRefNode )
		{
			if( m_pICurrentRefNode )
			{
				m_pICurrentRefNode->Release();
			}
			m_pICurrentRefNode = pIRefNode;
			if( m_pICurrentRefNode )
			{
				m_pICurrentRefNode->AddRef();
			}
		}

		// Current Collection name
		m_DLS_Combo.GetLBText( nCurSel, m_csCurrentCollection );
	}
}

void CDlgDLS::OnSelchangeDlslist() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Update Collection
	GetCurrentCollection();

	// Update instrument
	if(m_cList.GetCurSel() != LB_ERR)
	{
		DWORD dwData = m_cList.GetItemData( m_cList.GetCurSel() );
		m_nMSB = (dwData >> 16) & 0x7F;
		m_nLSB = (dwData >> 8) & 0x7F;
		m_nPC = dwData & 0x7F;
		UpdateData( FALSE );
	}
}

BOOL CDlgDLS::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Fill the list box with the instruments in the current collection
	CDialog::OnInitDialog();

	m_cList.ResetContent();
    static int tabstops[1] = { 12*4 };
    m_cList.SetTabStops(1,tabstops);

	InitCollectionCombo();
	InitInstrumentList();
	
	
	return TRUE;  
}

void CDlgDLS::InitCollectionList()
{
	IDMUSProdDocType*		pIDocType;
	IDMUSProdProject*		pIProject;
	GUID					guidProject;
	IDMUSProdNode*			pIDocRootNode;
	IDMUSProdProject*		pINextProject;
	IDMUSProdNode*			pIFileNode;
	IDMUSProdNode*			pINextFileNode;
	IDMUSProdNode*			pIReturnNode=NULL;
	DMUSProdListInfo		ListInfo;
	CString					csCollectionName;
	COLLECTION_LISTITEM*	pNewItem=NULL;


	HRESULT hr = m_pIFramework->FindDocTypeByNodeId( GUID_CollectionNode, &pIDocType );
	if (SUCCEEDED(hr))
	{
		hr = m_pIFramework->GetFirstProject( &pINextProject );
		while( SUCCEEDED( hr )  &&  pINextProject )
		{
			pIProject = pINextProject;
			pIProject->GetGUID(&guidProject);

			HRESULT hr = pIProject->GetFirstFileByDocType( pIDocType, &pINextFileNode );

			while( hr == S_OK )
			{
				pIFileNode = pINextFileNode;
				ZeroMemory( &ListInfo, sizeof(ListInfo) );
				ListInfo.wSize = sizeof(ListInfo);
				if( SUCCEEDED ( pIFileNode->GetNodeListInfo ( &ListInfo ) ) )
				{
					if( ListInfo.bstrName )
					{
						csCollectionName = ListInfo.bstrName;
						::SysFreeString( ListInfo.bstrName );
					}

					if( ListInfo.bstrDescriptor )
					{
						::SysFreeString( ListInfo.bstrDescriptor );
					}

					GUID guidFile;
					if( SUCCEEDED(m_pIFramework->GetNodeFileGUID( pIFileNode, &guidFile ) ) )
					{
						if ( SUCCEEDED(m_pIFramework->FindDocRootNodeByFileGUID( guidFile, &pIDocRootNode ) ) )
						{
							pNewItem = new COLLECTION_LISTITEM;
							pNewItem->csCollectionName = csCollectionName;
							pNewItem->pRefNode = CreateReferenceNode(pIDocRootNode);
							m_CollectionList.AddTail(pNewItem);
							pIDocRootNode->Release();
						}
					}
				}
				hr = pIProject->GetNextFileByDocType( pIFileNode, &pINextFileNode );
				pIFileNode->Release();
			}
			hr = m_pIFramework->GetNextProject( pIProject, &pINextProject );
			pIProject->Release();
		}
		pIDocType->Release();
	}//FindDocTypeByNodeId
}

void CDlgDLS::InitInstrumentList()
{
	IDLSQueryInstruments*					pIDLSQuery;
	IDMUSProdReferenceNode*					pIRefNode;
	int										iComboIndex;
	
	if ( m_CollectionList.IsEmpty() )
	{
		return;
	}
	m_cList.ResetContent();
	iComboIndex = m_DLS_Combo.GetCurSel();
	pIRefNode = (IDMUSProdReferenceNode*) m_DLS_Combo.GetItemDataPtr(iComboIndex);
	if ( SUCCEEDED(pIRefNode->QueryInterface(IID_IDLSQueryInstruments,(void**)&pIDLSQuery)) )
	{	
		if( SUCCEEDED(pIDLSQuery->ResetInstrumentList()) )
		{
			BYTE bMSB, bLSB, bPC;
			BOOL fDrums = FALSE;
			signed char szName[200];

			// Clear the list
			CleanInstrumentList();

			while( pIDLSQuery->GetNextInstrument( &bMSB, &bLSB, &bPC, &fDrums, szName, 200 ) )
			{
				// only display drums for the drum track
				if( (fDrums  &&  (m_nTrack == 9 || m_nTrack%16 == 9))  ||  (!fDrums  &&  (m_nTrack != 9 && m_nTrack%16 != 9)) )
				{
					INSTRUMENT_LISTITEM* pInstrument = new INSTRUMENT_LISTITEM;
					pInstrument->dwParams = (DWORD) (bMSB << 16 | bLSB << 8 | bPC);
					pInstrument->sInstrumentName.Format("%d,%d,%d\t%s", (long)bMSB, (long)bLSB, (long)bPC, szName); 
					m_InstrumentList.AddTail(pInstrument);
				}	
			}	

			SortInstrumentsAndAddToList();
		}
		pIDLSQuery->Release();
		// find the instrument in the list by its number and select it
		char szFind[32];
		sprintf( szFind, "%d,%d,%d\t", m_nMSB, m_nLSB, m_nPC );
		if( CB_ERR == m_cList.SelectString( -1, szFind ) )
		{
			m_cList.SetCurSel( -1 );
		}
	}
}

// Quicksort the instrument list
void CDlgDLS::SortInstrumentsAndAddToList()
{
	POSITION position = m_InstrumentList.GetHeadPosition();
	int nElems = m_InstrumentList.GetCount();

	QuickSort(position, nElems);

	// Reset the instrument list
	m_cList.ResetContent();

	position = m_InstrumentList.GetHeadPosition();
	while(position)
	{
		INSTRUMENT_LISTITEM* pInstrument = (INSTRUMENT_LISTITEM*)m_InstrumentList.GetNext(position);
		ASSERT(pInstrument);
		if(pInstrument == NULL)
			return;

		m_cList.SetItemData(m_cList.AddString(pInstrument->sInstrumentName), pInstrument->dwParams);
	}
}


// Implements classic QuickSort
void CDlgDLS::QuickSort(POSITION position, int nElems)
{
	// Nothing to do?
	if(nElems <= 1)
		return;

	// Get a pivot position
	POSITION pivotPosition = position;
	for(int nCount = 0; nCount < nElems/2; nCount++)
		m_InstrumentList.GetNext(pivotPosition);

	// Swap the pivot to the start of this list
	swap(position, pivotPosition);
	
	POSITION lastPosition = position;
	POSITION indexPosition = position;
	
	// Start with index = 1 (we swapped the pivot to 0)
	m_InstrumentList.GetNext(indexPosition);

	INSTRUMENT_LISTITEM* pInstrument0 = (INSTRUMENT_LISTITEM*)m_InstrumentList.GetAt(position);
	ASSERT(pInstrument0);
	if(pInstrument0 == NULL)
		return;

	int nPC0 = pInstrument0->dwParams & 0x7F;
	int nMSB0 = (pInstrument0->dwParams >> 16) & 0x7F;
	int nLSB0 = (pInstrument0->dwParams >> 8) & 0x7F;

	for(int nIndex = 1; nIndex < nElems && indexPosition != NULL; nIndex++)
	{
		// Remember the old position as GetNext will auto-increment the position
		POSITION oldPosition = indexPosition;
		
		INSTRUMENT_LISTITEM* pInstrument1 = (INSTRUMENT_LISTITEM*)m_InstrumentList.GetNext(indexPosition);
		ASSERT(pInstrument1);
		if(pInstrument1 == NULL)
			return;

		int nPC1 = pInstrument1->dwParams & 0x7F;
		int nMSB1 = (pInstrument1->dwParams >> 16) & 0x7F;
		int nLSB1 = (pInstrument1->dwParams >> 8) & 0x7F;

		// Patch sorts first, MSB sorts second and LSB sorts last
		if((nPC1 < nPC0) || (nPC1 == nPC0 && nMSB1 < nMSB0) || 
		   (nPC1 == nPC0 && nMSB1 == nMSB0 && nLSB1 < nLSB0))
		{
			m_InstrumentList.GetNext(lastPosition);
			swap(lastPosition, oldPosition);
		}
	}

	// Reset the pivot element
	swap(position, lastPosition);

	int nPartitionElems = 0;
	POSITION tempPosition = position;
	while(tempPosition != lastPosition)
	{
		nPartitionElems++;
		m_InstrumentList.GetNext(tempPosition);
	}

	// Recursively sort the subarrays
	QuickSort(position, nPartitionElems);

	// sort starting from last position + 1
	m_InstrumentList.GetNext(lastPosition);
	QuickSort(lastPosition, nElems - nPartitionElems - 1);
}

#ifdef _DEBUG
void CDlgDLS::_PrintTrace(POSITION startPosition, int nElems)
{
	TRACE("================\n");
	for(int nIndex = 0; nIndex < nElems; nIndex++)
	{
		INSTRUMENT_LISTITEM* pInstrument = (INSTRUMENT_LISTITEM*)m_InstrumentList.GetNext(startPosition);
		int nPC = pInstrument->dwParams & 0x7F;

		TRACE("PC = %d\n", nPC);
	}
	TRACE("================\n");
}
#endif



void CDlgDLS::swap(POSITION& position1, POSITION& position2)
{
	ASSERT(position1);
	ASSERT(position2);

	INSTRUMENT_LISTITEM* pTempInstrument = (INSTRUMENT_LISTITEM*)m_InstrumentList.GetAt(position1);
	ASSERT(pTempInstrument);
	if(pTempInstrument == NULL)
		return;

	m_InstrumentList.SetAt(position1, m_InstrumentList.GetAt(position2));
	m_InstrumentList.SetAt(position2, pTempInstrument);
}


void CDlgDLS::InitCollectionCombo()
{
	COLLECTION_LISTITEM*	pNextItem;
	POSITION				posIndex=NULL;
	int						iComboIndex;
	int						iCurrentCollection=0;
	
	if ( m_CollectionList.IsEmpty() )
	{
		return;
	}

	posIndex = m_CollectionList.GetHeadPosition();
	m_DLS_Combo.ResetContent();
	while(posIndex)
	{
		pNextItem = m_CollectionList.GetNext(posIndex);
		iComboIndex = m_DLS_Combo.AddString(pNextItem->csCollectionName);
		m_DLS_Combo.SetItemDataPtr(iComboIndex, pNextItem->pRefNode);
	}

	iCurrentCollection = m_DLS_Combo.FindStringExact( -1, m_csCurrentCollection );
	if( iCurrentCollection == CB_ERR )
	{
		iCurrentCollection = 0;
		m_nLastSelectedCollection = -1;
	}
	if(m_nLastSelectedCollection == -1)
		m_DLS_Combo.SetCurSel(iCurrentCollection);
	else
		m_DLS_Combo.SetCurSel(m_nLastSelectedCollection);

	m_DLS_Combo.SetTopIndex(iCurrentCollection);
}

IDMUSProdReferenceNode* CDlgDLS::CreateReferenceNode(IDMUSProdNode * pCollectionDocRoot)
{
	HRESULT						hr;
	IDMUSProdNode*				pINode;
	IDMUSProdComponent*			pIComponent;
	IDMUSProdReferenceNode*		pIRefNode;
	bool						bCollectionFound=false;


	// Allocate a new reference node
	hr = m_pIFramework->FindComponent( CLSID_DLSComponent,  &pIComponent );
	if(FAILED(hr))
	{
		return NULL;
	}

	hr = pIComponent->AllocReferenceNode(GUID_CollectionRefNode, &pINode);
	pIComponent->Release();
	if(FAILED(hr))
	{
		return NULL;
	}

	hr = pINode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIRefNode);
	pINode->Release();
	if (FAILED(hr))
	{
		TRACE("Failed query for Reference interface\n");
		return NULL;
	}
	
	hr = pIRefNode->SetReferencedFile(pCollectionDocRoot);
	if (FAILED(hr))
	{
		TRACE("Failed to SetReferencedFile\n");
		pIRefNode->Release();
		return NULL;
	}
	return pIRefNode;
}

void CDlgDLS::OnSelchangeCollectionsCombo() 
{
	// Update Collection
	GetCurrentCollection();

	m_nLastSelectedCollection = m_DLS_Combo.GetCurSel();

	// Rebuild instrument list
	InitInstrumentList();
}

void CDlgDLS::OnOK() 
{
	UpdateData(TRUE);

	int nCollections = m_DLS_Combo.GetCount();

	// Don't try to look for a collection that has this MSB, LSB and Patch...
	// ...if there are no collections...duh!
	if(nCollections == CB_ERR || nCollections == 0)
		EndDialog(IDOK);

	BOOL bFound = FALSE;
	IDMUSProdReferenceNode* pIRefNode = NULL;
	IDLSQueryInstruments*	pIDLSQuery = NULL;
	for(int nCollectionCount = 0; nCollectionCount < nCollections; nCollectionCount++)
	{
		pIRefNode = (IDMUSProdReferenceNode *)m_DLS_Combo.GetItemDataPtr(nCollectionCount);

		if (SUCCEEDED(pIRefNode->QueryInterface(IID_IDLSQueryInstruments,(void**)&pIDLSQuery)))
		{
			signed char sInstrumentName[DMUS_MAX_NAME];

			bFound = pIDLSQuery->GetInstrumentName(BYTE(m_nMSB), BYTE(m_nLSB), BYTE(m_nPC), false, sInstrumentName, DMUS_MAX_NAME);
			// Look for Drums
			if(!bFound)
				bFound = pIDLSQuery->GetInstrumentName(BYTE(m_nMSB), BYTE(m_nLSB), BYTE(m_nPC), true, sInstrumentName, DMUS_MAX_NAME);

			if(bFound)
			{
				pIDLSQuery->Release();
				break;
			}
			
			pIDLSQuery->Release();
		}
		
	}

	if(bFound)
	{
		m_DLS_Combo.SetCurSel(nCollectionCount);
		InitInstrumentList();
		OnSelchangeDlslist();
	}

	EndDialog(IDOK);
	//CDialog::OnOK();
}

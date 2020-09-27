// CollectionCtl.cpp : Implementation of the CCollectionCtrl ActiveX Control class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "jazz.h"
#include "Collection.h"
#include "CollectionCtl.h"
#include "CollectionPpg.h"
#include "CollectionEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCollectionCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CCollectionCtrl, COleControl)
	//{{AFX_MSG_MAP(CCollectionCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CCollectionCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CCollectionCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CCollectionCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CCollectionCtrl, COleControl)
	//{{AFX_EVENT_MAP(CCollectionCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl Interface map

BEGIN_INTERFACE_MAP(CCollectionCtrl, COleControl)
    INTERFACE_PART(CCollectionCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CCollectionCtrl, IID_IJazzEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CCollectionCtrl, 1)
	PROPPAGEID(CCollectionPropPage::guid)
END_PROPPAGEIDS(CCollectionCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CCollectionCtrl, "DLSDESIGNER.CollectionCtrl.1",
	0xbc964e86, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CCollectionCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DCollection =
		{ 0xbc964e84, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DCollectionEvents =
		{ 0xbc964e85, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwCollectionOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CCollectionCtrl, IDS_COLLECTION, _dwCollectionOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::CCollectionCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CCollectionCtrl

BOOL CCollectionCtrl::CCollectionCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_COLLECTION,
			IDB_COLLECTION,
			afxRegApartmentThreading,
			_dwCollectionOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::CCollectionCtrl - Constructor

CCollectionCtrl::CCollectionCtrl() : m_pCollectionEditor(NULL), m_pCollection(NULL)
{
	InitializeIIDs(&IID_DCollection, &IID_DCollectionEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::~CCollectionCtrl - Destructor

CCollectionCtrl::~CCollectionCtrl()
{
	if(m_pCollection)
	{
		m_pCollection->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl IJazzEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CCollectionCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CCollectionCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CCollectionCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CCollectionCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::QueryInterface

STDMETHODIMP CCollectionCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CCollectionCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::AttachObjects implementation

HRESULT CCollectionCtrl::XEditor::AttachObjects( IJazzNode* pNode, IOleUndoManager* pUndoManager )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	pThis->m_pCollection = (CCollection *)pNode;
	pThis->m_pCollection->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::OnInitMenuFilePrint

HRESULT CCollectionCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::OnFilePrint

HRESULT CCollectionCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CCollectionCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XEditor::OnFilePrintPreview

HRESULT CCollectionCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CCollectionCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CCollectionCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CCollectionCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CCollectionCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::OnDraw - Drawing function

void CCollectionCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::DoPropExchange - Persistence support

void CCollectionCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::OnResetState - Reset control to default state

void CCollectionCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	IJazzFramework *pFramework = theApp.GetFramework();
	if (pFramework)
	{
		IJazzComponent *pDLS;
		pFramework->FindComponent(CLSID_DLSComponent,&pDLS);
		if (pDLS)
		{
			IDLSQueryInstruments *pDLSQuery;
			pDLS->QueryInterface(IID_IDLSQueryInstruments,(void **)&pDLSQuery);
			if (pDLSQuery)
			{
				signed char szCollectionName[200];
				pDLSQuery->ResetCollectionList();
				while (pDLSQuery->GetNextCollection(szCollectionName,200))
				{
					char szOutput[300];
					sprintf(szOutput,"%s:\n",szCollectionName);
					OutputDebugString(szOutput);
					pDLSQuery->ResetInstrumentList(szCollectionName);
					BYTE bMSB, bLSB, bPC;
					BOOL fDrums;
					signed char szName[200];
					while (pDLSQuery->GetNextInstrument(
									&bMSB, &bLSB, &bPC, &fDrums, szName, 200))
					{
						sprintf(szOutput,"\t%s: %ld,%ld,%ld, Drums: %ld, ",
							szName,(long)bMSB,(long)bLSB,(long)bPC,fDrums);
						pDLSQuery->GetInstrumentName(szCollectionName, 
									  bMSB,bLSB,bPC,fDrums,szName,200); 
						OutputDebugString(szOutput);
						sprintf(szOutput,"GetName returns %s\n", szName);
						OutputDebugString(szOutput);
					}
				}
				pDLSQuery->Release();
			}
			pDLS->Release();
		}
	}
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl::AboutBox - Display an "About" box to the user

void CCollectionCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_COLLECTION);
	dlgAbout.DoModal();

}


/////////////////////////////////////////////////////////////////////////////
// CCollectionCtrl message handlers

int CCollectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create Collection's view
	m_pCollectionEditor = new CCollectionEditor(this);

	if( m_pCollectionEditor == NULL )
	{
		return -1;
	}

	if(m_pCollectionEditor->Create(NULL, 
								"WindowName", 
								WS_CHILD | WS_VISIBLE, 
								CRect( 0, 0, 800, 400 ), 
								this, 
								888, 
								NULL) == 0)
	{
		return -1;
	}


	m_pCollectionEditor->OnInitialUpdate(); 
	return 0;
}



	


void CCollectionCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
	
	m_pCollectionEditor->MoveWindow( 0, 0, cx, cy );
}

void CCollectionCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCollectionCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}

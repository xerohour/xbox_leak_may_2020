// OlePropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "OlePropSheet.h"
#include "OlePropPage.h"

#include "comhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COlePropSheet

IMPLEMENT_DYNAMIC(COlePropSheet, CPropertySheet)

COlePropSheet::COlePropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pUnkServer = NULL;
	m_pIDMUSProdUpdateObjectNow = NULL;
	EnableAutomation();
}

COlePropSheet::COlePropSheet(LPUNKNOWN lpUnk, LPCTSTR pszCaption, IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pUnkServer = lpUnk;
	m_pIDMUSProdUpdateObjectNow = pIDMUSProdUpdateObjectNow;
	EnableAutomation();
}

COlePropSheet::~COlePropSheet()
{
	for (UINT i = 0; i < m_iPageCnt; i++)
	{
		delete ((COlePropPage *)m_pageArray[i]);			
	}
}


BEGIN_MESSAGE_MAP(COlePropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COlePropSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COlePropSheet, CPropertySheet)
	//{{AFX_DISPATCH_MAP(COlePropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOlePropSheet to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {828CE900-15EE-11D3-AE81-00105AA22F40}
static const IID IID_IOlePropSheet =
{ 0x828ce900, 0x15ee, 0x11d3, { 0xae, 0x81, 0x0, 0x10, 0x5a, 0xa2, 0x2f, 0x40 } };

BEGIN_INTERFACE_MAP(COlePropSheet, CPropertySheet)
	INTERFACE_PART(COlePropSheet, IID_IOlePropSheet, Dispatch)
END_INTERFACE_MAP()


void COlePropSheet::PostNcDestroy() 
{
	CPropertySheet::PostNcDestroy();
	delete this;	
}


int COlePropSheet::DoModal() 
{
	// Check that we have a pointer to the object to display properties for
	ASSERT(m_pUnkServer != NULL);
	if( m_pUnkServer == NULL )
	{
		return FALSE;
	}

	// Remove the apply button
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Get number of property pages that reside in object
	ISpecifyPropertyPages * lpSPP;
	HRESULT hRes = m_pUnkServer->QueryInterface(IID_ISpecifyPropertyPages, (LPVOID*)&lpSPP);
	if( FAILED( hRes ) )
	{
		return FALSE;
	}

	// Get the array of property sheet pages
	CAUUID cauuid;
	if( FAILED( lpSPP->GetPages( &cauuid ) ) )
	{
		lpSPP->Release();
		return FALSE;
	}
	lpSPP->Release();

	// Set the number of pages to display
	m_iPageCnt = cauuid.cElems;

	for (UINT i = 0; i < m_iPageCnt; i++)
	{
		// Create CPropertyPages and Property page sites.
		COlePropPage * pPage = new COlePropPage(cauuid.pElems[i], m_pUnkServer, m_pIDMUSProdUpdateObjectNow );

		// Check if construction failed
		if( (pPage == NULL)
		||	!pPage->m_fConstructionSucceeded )
		{
			// construction failed - delete the object, if necessary
			if( pPage )
			{
				delete pPage;
			}

			// Fail
			return FALSE;
		}

		// Add the page to our array of pages
		m_pageArray.Add(pPage);

		// Add the page to the dialog
		AddPage((COlePropPage*)m_pageArray[i]);
	}

	// Now, actually display the dialog
	return CPropertySheet::DoModal();
}

// OlePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "comhelp.h"
#include <comdef.h>
#include <initguid.h>
#include "OlePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Extended dialog template (from atlhost.h)
#pragma pack(push, 1)
struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;

	// Everything else in this structure is variable length,
	// and therefore must be determined dynamically

	// sz_Or_Ord menu;			// name or ordinal of a menu resource
	// sz_Or_Ord windowClass;	// name or ordinal of a window class
	// WCHAR title[titleLen];	// title string of the dialog box
	// short pointsize;			// only if DS_SETFONT is set
	// short weight;			// only if DS_SETFONT is set
	// short bItalic;			// only if DS_SETFONT is set
	// WCHAR font[fontLen];		// typeface name, if DS_SETFONT is set
};
#pragma pack(pop)


//*********************************************************************
// GetPropSheetFont
//*********************************************************************

#define IDD_PROPSHEET	1006

static HFONT GetPropSheetFont()
{
	// Create a LOGFONTA structure
    LOGFONTA logfont;

	// Initialize it
    memset(&logfont, 0, sizeof(LOGFONTA));
    logfont.lfHeight = 8;
    logfont.lfWeight = FW_NORMAL;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName, "MS Sans Serif");

	// Try and load ComCtrl32.dll
    HINSTANCE hInst = LoadLibraryA("COMCTL32");
    if (hInst != NULL)
	{
		// Find the standard property sheet template
		HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDD_PROPSHEET), RT_DIALOG);

		// Load the standard property sheet template
		HGLOBAL hTemplate = LoadResource(hInst, hResource);
		DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LockResource(hTemplate);
		if (pTemplate != NULL)
		{
			// Check if this is an extended dialogm template
			bool bDialogEx = ((pTemplate->style & 0xFFFF0000) == 0xFFFF0000);

			// A pointer used to iterate through the dialog template
			WORD* pw;

			// Skip over the dialog template
			if (bDialogEx)
			{
				pw = (WORD*)((DLGTEMPLATEEX*)pTemplate + 1);
			}
			else
			{
				pw = (WORD*)(pTemplate + 1);
			}

			if (*pw == (WORD)-1)		// Skip menu name string or ordinal
				pw += 2; // WORDs
			else
				while(*pw++);
				
			if (*pw == (WORD)-1)		// Skip class name string or ordinal
				pw += 2; // WORDs
			else
				while(*pw++);

			while (*pw++);			// Skip caption string

			// Get the default device context
			HDC hdc = ::GetDC(NULL);

			// Convert *pw into 1/72ths of an inch
			logfont.lfHeight = -MulDiv(*pw, GetDeviceCaps(hdc, LOGPIXELSY), 72);

			// Release the default device context
			::ReleaseDC(NULL, hdc);

			pw += (bDialogEx ? 3 : 1);

			// Convert the font name from a WCHAR string to an ASCII string
			WideCharToMultiByte(CP_ACP, 0, pw, -1, logfont.lfFaceName, sizeof(logfont.lfFaceName), NULL, NULL);
		}

		// Release our lock on the ComCtl32.dll library
		FreeLibrary(hInst);
    }

	// Try and create the font
    return CreateFontIndirectA(&logfont);
}

COlePropPageTemplate::COlePropPageTemplate()
{
	// Initialize the property page template
	style = WS_CHILD | WS_TABSTOP | DS_CONTROL;
	dwExtendedStyle = WS_EX_CONTROLPARENT;
	cdit = 0;
	x = 0;
	y = 0;
	cx = 0;
	cy = 0;
	MenuName = 0;
	ClassName = 0;
	szCaption = 0;
	wPointSize = 0;
	szFontName = 0;
}

void COlePropPageTemplate::SetSize(SIZE& size)
{
	// Variables to store the size of characters in the system font
	static UINT cxSysChar = 0;
	static UINT cySysChar = 0;

	// If the size is not yet initialized
	if ((cxSysChar == 0) && (cySysChar == 0))
	{
		// Get the default device context
		HDC hdc = ::GetDC(NULL);

		// Get the property sheet font
		HFONT hfontSys;
		if ((hfontSys = GetPropSheetFont()) != NULL)
		{
			// Select the property sheet font
			HFONT hfontOld = (HFONT)SelectObject(hdc, hfontSys);

			// Get the metrics for this font
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);

			// Get the size of the entire alphabet, both upper and lower cases
			SIZE sizeText;
			::GetTextExtentPointA(hdc, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &sizeText);

			// Compute the average width of a character (rounding to the nearest value)
			cxSysChar = (sizeText.cx + 26) / 52;

			// compute the height of the characters, including the white space between rows
			cySysChar = tm.tmHeight + tm.tmExternalLeading;

			// Select the old font
			SelectObject(hdc, hfontOld);

			// Delete the property sheet font
			DeleteObject((HGDIOBJ)hfontSys);
		}
		else
		{
			// Could not create the font so just use the system's values
			LONG l = GetDialogBaseUnits();
			cxSysChar = LOWORD(l);
			cySysChar = HIWORD(l);
		}

		// Release the default device context
		::ReleaseDC(NULL, hdc);
	}

	// Ensure cxSysChar and cySysChar are at least 1
	cxSysChar = max( cxSysChar, 1 );
	cySysChar = max( cySysChar, 1 );

	// Translate pixels to dialog units
	cx = short(MulDiv(size.cx, 4, cxSysChar));
	cy = short(MulDiv(size.cy, 8, cySysChar));
}

/////////////////////////////////////////////////////////////////////////////
// COlePropPage property page

IMPLEMENT_DYNCREATE(COlePropPage, CPropertyPage)

COlePropPage::COlePropPage() : CPropertyPage(COlePropPage::IDD)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	EnableAutomation();
	//{{AFX_DATA_INIT(COlePropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pPropPage = NULL;
	m_pObject = NULL;
	m_pIDMUSProdUpdateObjectNow = NULL;
	m_hwndProducer = NULL;
}

COlePropPage::COlePropPage(CLSID clsidPage, LPUNKNOWN lpUnkPage, IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow) : CPropertyPage(COlePropPage::IDD),
	m_clsidPage(clsidPage), m_pObject(lpUnkPage), m_pIDMUSProdUpdateObjectNow(pIDMUSProdUpdateObjectNow)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Create COM Property page and get IPropertyPage interface

	// Enable automation for this dialog
	EnableAutomation();

	// Initialize m_fConstructionSucceeded to true
	m_fConstructionSucceeded = true;

	// Initialize the property page pointer to NULL
	m_pPropPage = NULL;

	// Try and initialize everything
	try
	{
		// A variable to store the results
		HRESULT hRes;

		// Try and create the property page
		hRes = CoCreateInstance( m_clsidPage, NULL, CLSCTX_INPROC, IID_IPropertyPage, (void**)&m_pPropPage );
		if( FAILED( hRes ) )
		{
			AfxThrowOleException( hRes );
		}

		// Point the property page back to us
		hRes = m_pPropPage->SetPageSite( (IPropertyPageSite*) GetInterface( &IID_IPropertyPageSite ) );
		if( FAILED( hRes ) )
		{
			AfxThrowOleException( hRes );
		}

		// Point the property page to its object
		hRes = m_pPropPage->SetObjects( 1, &m_pObject );
		if( FAILED( hRes ) )
		{
			AfxThrowOleException( hRes );
		}

		// Get a pointer to the IMalloc allocator
		IMalloc *pIMalloc;
		if( FAILED( CoGetMalloc( MEMCTX_TASK, &pIMalloc ) ) )
		{
			AfxThrowOleException(E_FAIL);
		}

		// Allocate a PROPPAGEINFO structure
		PROPPAGEINFO* pPPI = (PROPPAGEINFO*) pIMalloc->Alloc(sizeof(PROPPAGEINFO));

		// If memory allocated failed
		if( NULL == pPPI )
		{
			AfxThrowOleException(E_OUTOFMEMORY);
		}

		// Initialize the PROPPAGEINFO structure
		ZeroMemory( pPPI, sizeof( PROPPAGEINFO ) );

		// Initialize the size of the PROPPAGEINFO structure
		pPPI->cb = sizeof(PROPPAGEINFO);

		// Get the property page information (from the propety page)
		hRes = m_pPropPage->GetPageInfo(pPPI);
		if( SUCCEEDED( hRes ) )
		{
			// Set the dialog title
			m_strCaption = pPPI->pszTitle;
			m_psp.pszTitle = m_strCaption;
			m_psp.dwFlags |= PSP_USETITLE;

			// Free the memory allocated by the strings in pPPI
			CoTaskMemFree( pPPI->pszDocString );
			CoTaskMemFree( pPPI->pszHelpFile );
			CoTaskMemFree( pPPI->pszTitle );

			// set the dialog template size
			m_dlgTemplate.SetSize( pPPI->size );
		}

		// Make sure the property sheet uses pResource
		m_psp.dwFlags |= PSP_DLGINDIRECT;
		m_psp.pResource = &m_dlgTemplate;

		// Free the PROPPAGEINFO structure
		pIMalloc->Free(pPPI);

		// Release the IMalloc allocator
		pIMalloc->Release();
	} catch (COleException * e)
	{
		// Just delete the exception
		delete e;

		// And set the construction succeeded flag to false
		m_fConstructionSucceeded = false;
	}

	m_hwndProducer = NULL;

	//{{AFX_DATA_INIT(COlePropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


COlePropPage::~COlePropPage()
{
	// Release COM Property Page
	if (m_pPropPage)
	{
		m_pPropPage->Release();
		m_pPropPage = NULL;
	}
}

void COlePropPage::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CPropertyPage::OnFinalRelease();
}

void COlePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COlePropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COlePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(COlePropPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COlePropPage, CPropertyPage)
	//{{AFX_DISPATCH_MAP(COlePropPage)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOlePropPage to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {469F345A-1A8B-11D3-B009-00105AA22F40}
static const IID IID_IOlePropPage =
{ 0x469f345a, 0x1a8b, 0x11d3, { 0xb0, 0x9, 0x0, 0x10, 0x5a, 0xa2, 0x2f, 0x40 } };

BEGIN_INTERFACE_MAP(COlePropPage, CPropertyPage)
	INTERFACE_PART(COlePropPage, IID_IOlePropPage, Dispatch)
	INTERFACE_PART(COlePropPage, IID_IPropertyPageSite, PropertySite)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COlePropPage message handlers
IMPLEMENT_IUNKNOWN(COlePropPage, PropertySite)


STDMETHODIMP COlePropPage::XPropertySite::GetLocaleID(LCID* pLocaleID)
{
	//METHOD_PROLOGUE(COlePropPage, PropertySite);     // Creates a pThis pointer 
	//TRACE("COlePropPage::PropertySite::GetLocaleID()\n");
	*pLocaleID = ::GetThreadLocale();

	return (HRESULT)NOERROR;   
}

STDMETHODIMP COlePropPage::XPropertySite::OnStatusChange(DWORD dwFlags)
{
	METHOD_PROLOGUE(COlePropPage, PropertySite);     // Creates a pThis pointer 
	//TRACE("COlePropPage::PropertySite::OnStatusChange()\n");

	if (dwFlags & PROPPAGESTATUS_DIRTY)
	{
		// Update the object
		pThis->m_pPropPage->Apply();

		// Notify the editor that the object was updated
		if( pThis->m_pIDMUSProdUpdateObjectNow )
		{
			pThis->m_pIDMUSProdUpdateObjectNow->UpdateObjectNow( pThis->m_pObject );
		}
	}

	return (HRESULT)NOERROR;   
}

STDMETHODIMP COlePropPage::XPropertySite::GetPageContainer(IUnknown **ppUnk)
{
	UNREFERENCED_PARAMETER(ppUnk);
	//METHOD_PROLOGUE(COlePropPage, PropertySite);     // Creates a pThis pointer 
	//TRACE("COlePropPage::PropertySite::GetPageContainer()\n");

	return (HRESULT)E_NOTIMPL;   
}

STDMETHODIMP COlePropPage::XPropertySite::TranslateAccelerator(MSG *pMsg)
{
	UNREFERENCED_PARAMETER(pMsg);
	//METHOD_PROLOGUE(COlePropPage, PropertySite);     // Creates a pThis pointer to 
	//TRACE("COlePropPage::PropertySite::TranslateAccelerator()\n");

	return (HRESULT)E_NOTIMPL;   
}

/////////////////////////////////////////////////////////////////////////////
// COlePropPage message handlers


BOOL COlePropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();

	// Initialize the property page
	try {
		// Get the rectangle of our window
		CRect pgrect;
		GetWindowRect(&pgrect);

		// Convert it to client coordinates
		ScreenToClient(pgrect);

		// Try and activate the property page
		HRESULT hRes = m_pPropPage->Activate( GetSafeHwnd(), pgrect, TRUE );
		if( FAILED( hRes ) )
			AfxThrowOleException( hRes );

		hRes = m_pPropPage->Show( SW_SHOW );
		if( FAILED( hRes ) )
			AfxThrowOleException( hRes );

	} catch (COleException * e)
	{
		throw (e);
	}

	// Add WS_EX_CONTROLPARENT style to property page
	// necessary to allow tabbing from page to sheet.

	// Get COM Prop Page
	CWnd * pWnd = GetWindow(GW_CHILD);

	// Get the class name
	CString str;
	::GetClassName(pWnd->GetSafeHwnd(), str.GetBuffer(128), 128);
    str.ReleaseBuffer();

	// Check if the class name is #32770 (dialog box)
	if (str == (CString)_T("#32770"))
	{
		// If so, add the WS_EX_CONTROLPARENT flag
		pWnd->ModifyStyleEx(0,WS_EX_CONTROLPARENT,0);
	}

	m_hwndProducer = ::FindWindow("JzApBR", NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL COlePropPage::OnKillActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// The window was deactivated - update the object
	m_pPropPage->Apply();

	// Notify the editor that the object was updated
	if( m_pIDMUSProdUpdateObjectNow )
	{
		m_pIDMUSProdUpdateObjectNow->UpdateObjectNow( m_pObject );
	}

	return CPropertyPage::OnKillActive();
}

void COlePropPage::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Try and de-activate the property page
	if( m_pPropPage )
	{
		m_pPropPage->Deactivate();
	}

	CPropertyPage::OnDestroy();
}

BOOL COlePropPage::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// This allows the Transport accelerators to continue working while the property page is open
	if( pMsg->message == WM_SYSCHAR )
	{
		if( m_hwndProducer )
		{
			HWND hWndHasFocus = ::GetFocus();
			::SendMessage( m_hwndProducer, WM_APP, WPARAM(pMsg), 0 );
			if( hWndHasFocus != ::GetFocus() )
			{
				::SetFocus( hWndHasFocus );
			}
		}
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

// mfcawex2Doc.cpp : implementation of the CMfcawex2Doc class
//

#include "stdafx.h"
#include "mfcawex2.h"

#include "mfcawex2Doc.h"
#include "CntrItem.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc

IMPLEMENT_DYNCREATE(CMfcawex2Doc, COleServerDoc)

BEGIN_MESSAGE_MAP(CMfcawex2Doc, COleServerDoc)
	//{{AFX_MSG_MAP(CMfcawex2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleServerDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleServerDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleServerDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMfcawex2Doc, COleServerDoc)
	//{{AFX_DISPATCH_MAP(CMfcawex2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMfcawex2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {1DEE14A8-CC4B-11D0-84BF-00AA00C00848}
static const IID IID_IMfcawex2 =
{ 0x1dee14a8, 0xcc4b, 0x11d0, { 0x84, 0xbf, 0x0, 0xaa, 0x0, 0xc0, 0x8, 0x48 } };

BEGIN_INTERFACE_MAP(CMfcawex2Doc, COleServerDoc)
	INTERFACE_PART(CMfcawex2Doc, IID_IMfcawex2, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc construction/destruction

CMfcawex2Doc::CMfcawex2Doc()
{
	// Use OLE compound files
	EnableCompoundFile();

	// TODO: add one-time construction code here

	EnableAutomation();

	AfxOleLockApp();
}

CMfcawex2Doc::~CMfcawex2Doc()
{
	AfxOleUnlockApp();
}

BOOL CMfcawex2Doc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc server implementation

COleServerItem* CMfcawex2Doc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	CMfcawex2SrvrItem* pItem = new CMfcawex2SrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}



/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc serialization

void CMfcawex2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc diagnostics

#ifdef _DEBUG
void CMfcawex2Doc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CMfcawex2Doc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2Doc commands

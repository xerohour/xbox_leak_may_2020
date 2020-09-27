// ecmfcappDoc.cpp : implementation of the CEcmfcappDoc class
//

#include "stdafx.h"
#include "ecmfcapp.h"

#include "ecmfcappDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappDoc

IMPLEMENT_DYNCREATE(CEcmfcappDoc, CDocument)

BEGIN_MESSAGE_MAP(CEcmfcappDoc, CDocument)
	//{{AFX_MSG_MAP(CEcmfcappDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappDoc construction/destruction

CEcmfcappDoc::CEcmfcappDoc()
{
	// TODO: add one-time construction code here

}

CEcmfcappDoc::~CEcmfcappDoc()
{
}

BOOL CEcmfcappDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEcmfcappDoc serialization

void CEcmfcappDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappDoc diagnostics

#ifdef _DEBUG
void CEcmfcappDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEcmfcappDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappDoc commands

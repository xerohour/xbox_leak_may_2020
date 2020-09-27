// mfcawexeDoc.cpp : implementation of the CMfcawexeDoc class
//

#include "stdafx.h"
#include "mfcawexe.h"

#include "mfcawexeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeDoc

IMPLEMENT_DYNCREATE(CMfcawexeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMfcawexeDoc, CDocument)
	//{{AFX_MSG_MAP(CMfcawexeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeDoc construction/destruction

CMfcawexeDoc::CMfcawexeDoc()
{
	// TODO: add one-time construction code here

}

CMfcawexeDoc::~CMfcawexeDoc()
{
}

BOOL CMfcawexeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	int i = 99;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMfcawexeDoc serialization

void CMfcawexeDoc::Serialize(CArchive& ar)
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
// CMfcawexeDoc diagnostics

#ifdef _DEBUG
void CMfcawexeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfcawexeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeDoc commands

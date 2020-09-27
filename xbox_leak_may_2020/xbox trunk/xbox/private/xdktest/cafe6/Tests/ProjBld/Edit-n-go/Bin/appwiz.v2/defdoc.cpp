// defdoc.cpp : implementation of the CDefDoc class
//

#include "stdafx.h"
#include "def.h"

#include "defdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefDoc

IMPLEMENT_DYNCREATE(CDefDoc, CDocument)

BEGIN_MESSAGE_MAP(CDefDoc, CDocument)
	//{{AFX_MSG_MAP(CDefDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefDoc construction/destruction

CDefDoc::CDefDoc()
{
	// TODO: add one-time construction code here

}

CDefDoc::~CDefDoc()
{
}

BOOL CDefDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDefDoc serialization

void CDefDoc::Serialize(CArchive& ar)
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
// CDefDoc diagnostics

#ifdef _DEBUG
void CDefDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDefDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDefDoc commands

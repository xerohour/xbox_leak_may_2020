// docobdoc.cpp : implementation of the CDocObjectDoc class
//

#include "stdafx.h"
#pragma hdrstop

#include "docobdoc.h"
#include "docobtpl.h"
#include "docobctr.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc

IMPLEMENT_DYNCREATE(CDocObjectDoc, CPartDoc)

BEGIN_MESSAGE_MAP(CDocObjectDoc, CPartDoc)
	//{{AFX_MSG_MAP(CDocObjectDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, CPartDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, CPartDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CPartDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CPartDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, CPartDoc::OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, CPartDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, CPartDoc::OnEditConvert)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc construction/destruction

CDocObjectDoc::CDocObjectDoc()
{
	// For most containers, using compound files is a good idea.
	EnableCompoundFile();

	m_clsid = GUID_NULL;
	m_pItem = NULL;
}

CDocObjectDoc::~CDocObjectDoc()
{
}

BOOL CDocObjectDoc::IsModified()
{
	if (m_pItem != NULL)
		return(m_pItem->IsModified());
	return(FALSE);

//	return(CPartDoc::IsModified());
}

BOOL CDocObjectDoc::NewDocumentHook()
{
	// set up the clsid for the document, now that template is attached
	CPartTemplate *pTemplate = (CPartTemplate *)GetDocTemplate();
	ASSERT_KINDOF(CPartTemplate, pTemplate);
	m_clsid = pTemplate->GetTemplateClsid();

	m_pItem = new CDocObjectContainerItem(this);
	if (m_pItem->CreateNewUntitledFile(m_clsid))
		return TRUE;

	// it didn't work ... clean up
	DoClose();
	MsgBox(Error, IDS_DOCOBJ_CANT_CREATE) ;
	return FALSE;
}

BOOL CDocObjectDoc::OpenDocumentHook(LPCTSTR lpszPathName)
{
	// set up the clsid for the document, now that template is attached
	CPartTemplate *pTemplate = (CPartTemplate *)GetDocTemplate();
	ASSERT_KINDOF(CPartTemplate, pTemplate);
	m_clsid = pTemplate->GetTemplateClsid();

	// The document is implemented as a single OLE client
	// item, opened from the specified file.
	m_pItem = new CDocObjectContainerItem(this);
	if (m_pItem->CreateFromExternalFile(lpszPathName, m_clsid))
	{
		SetReadOnlyOnDisk(m_pItem->IsReadOnlyOnDisk()) ;
		return TRUE;
	}

	BOOL fIssueError = TRUE;

	if ((m_pItem->m_pOleDocument == (LPOLEDOCUMENT)0xFFFFFFFF) && (m_pItem->m_pOleDocumentView == (LPOLEDOCUMENTVIEW)0xFFFFFFFF))
	{
		// [patbr] hack! we attempted standalone open of file, so shell should issue
		// error message if the file could not be opened, and we should silently fail.
		// we want to return FALSE, though, so no document or view is created.
		m_pItem->m_pOleDocument = (LPOLEDOCUMENT)NULL;
		m_pItem->m_pOleDocumentView = (LPOLEDOCUMENTVIEW)NULL;
		fIssueError = FALSE;
	}

	// it didn't work ... clean up
	DoClose();

	if (fIssueError)
	{
		CString str ;
		MsgBox(Error, MsgText(str, IDS_DOCOBJ_CANT_OPEN, lpszPathName));
	}

	return FALSE;
}

BOOL CDocObjectDoc::SaveDocumentHook(const char* pszPathName, UINT nSaveType)
{
	ASSERT(m_pItem != NULL);
	return(m_pItem->SaveToExternalFile(pszPathName));
}

void CDocObjectDoc::DoClose()
{
	if (m_pItem != NULL)
	{
		POSITION pos = GetFirstViewPosition();
		CView *pView = GetNextView(pos);
		if(pView)
		{
			CFrameWnd *pFrame = (CFrameWnd *)pView->GetParent();
			// The only possible pending activation here is our own frame
			ASSERT(!CPartFrame::s_pFrmHookActivate  || !pFrame || 
				CPartFrame::s_pFrmHookActivate == pFrame->m_pNotifyHook);
			
			// If we are in the process of activating, let's ignore it, it will
			// cancel out when it gets to CPartFrame::MDIActivate
			if(!CPartFrame::s_pFrmHookActivate && pFrame)
			{
				// if we are the active frame now, let's de-activate
				if(CPartFrame::s_pLastActiveFrame == pFrame)
				{
					CPartFrame::s_pFrmHookDeactivate = pFrame->m_pNotifyHook;
					CPartFrame::s_pLastActiveFrame = NULL;
					CPartFrame::FinishActivation();
				}
			}
		}
		m_pItem->DoClose();
		CDocObjectContainerItem *pTempItem = m_pItem;
		m_pItem = NULL;
		pTempItem->InternalRelease();
		OleRelease(m_lpRootStg);
	}
}


BOOL CDocObjectDoc::CanCloseFrame(CFrameWnd *pFrame)
{
	BOOL fRet = TRUE;
	// if pFrame is NULL, only check whether we can close the container item,
	// do NOT call CanCloseFrame which will cause prompting to the user
	if (pFrame)
		fRet = COleDocument::CanCloseFrame(pFrame);

	return fRet;
}

void CDocObjectDoc::CloseDocumentHook()
{
	// delete the contained item if any...this will close the doc object view.
	DoClose();
	CPartDoc::CloseDocumentHook();	// base implementation
}

HRESULT	CDocObjectDoc::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	// we offer not services at this time
	if (ppvObj == NULL)
		return E_INVALIDARG;

	*ppvObj = NULL;
	return E_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc serialization

void CDocObjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CPartDoc enables serialization
	//  of the container document's COleClientItem objects.
	CPartDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc diagnostics

#ifdef _DEBUG
void CDocObjectDoc::AssertValid() const
{
	CPartDoc::AssertValid();
}

void CDocObjectDoc::Dump(CDumpContext& dc) const
{
	CPartDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc commands

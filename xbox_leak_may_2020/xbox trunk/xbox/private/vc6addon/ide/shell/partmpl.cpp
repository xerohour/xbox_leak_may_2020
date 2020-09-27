// partmpl.cpp : Part document template class definition
//

#include "stdafx.h"
#include <io.h>
#include <dde.h>
#include "main.h"
#include "prjapi.h"
#include "utilauto.h"

IMPLEMENT_DYNAMIC(CPartTemplate, CMultiDocTemplate)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPartTemplate

CPartTemplate::CPartTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,
	CPackage* pPackage, CPacket* pPacket, REFCLSID clsid,
	LPCTSTR szAutoType /* = NULL */)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
	ASSERT(m_docList.IsEmpty());
	ASSERT(pDocClass != NULL);

	m_hMenuShared = NULL;
	m_hAccelTable = NULL;
	m_nUntitledCount = 0;   // start at 1
	
	m_pPackage = pPackage;
	m_pPacket = pPacket;

	// CLSID_NULL is reserved for DocObject types.
	ASSERT(clsid != CLSID_NULL);
	m_clsid = clsid;

	m_strAutoType = szAutoType;
	m_nIconID = nIDResource;
	m_TabName = Files;
}

CPartTemplate::~CPartTemplate()
{
}

void CPartTemplate::SetContainerInfo()
{
	m_hMenuInPlace = theCmdCache.GetOleMenu();
}

CDocument* CPartTemplate::OpenDocumentFile(const TCHAR* pszPathName,
		BOOL bMakeVisible /*=TRUE*/)
{
	// NOTE: The only difference between this function and the one in
	// CMultiTemplate is this one initializes the appropriate Packet
	// for the type.
	
	CDocument* pDocument = CreateNewDocument();

#ifndef NEWPROJ
	if (pDocument == NULL)
	{
		// HACK_M2: Here we differ from MFC by adding paths of failed opens 
		// to the recent file list.  This was done because old-style project
		// files did not have part doc's so CreateNewDocument() always
		// "failed"  This hack gets those projects onto the MRU...
		if (pszPathName != NULL)
			AfxGetApp()->AddToRecentFileList(pszPathName);
		
		TRACE0("CDocTemplate::CreateNewDocument returned NULL\n");
//		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
#endif
	ASSERT_VALID(pDocument);

	BOOL bOldAuto = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE; // don't destroy if something goes wrong
	CMDIChildWnd* pFrame = (CMDIChildWnd*) CreateNewFrame(pDocument, NULL);
	pDocument->m_bAutoDelete = bOldAuto;

	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);
	ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

	if (pszPathName == NULL)
	{
		// create a new document - with default document name
		UINT nUntitled = m_nUntitledCount + 1;

		CString strDocName;
		if (GetDocString(strDocName, CDocTemplate::docName) &&
			!strDocName.IsEmpty())
		{
			char szNum[8];
			wsprintf(szNum, "%d", nUntitled);
			strDocName += szNum;
		}
		else
		{
			// use generic 'untitled' - ignore untitled count
			VERIFY(strDocName.LoadString(AFX_IDS_UNTITLED));
		}
		pDocument->SetTitle(strDocName);

		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE\n");
			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;
	}
	else
	{
		// open an existing document
		if (!pDocument->OnOpenDocument(pszPathName))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE\n");
			pFrame->DestroyWindow();
			return NULL;
		}
	}

	// Initialize the view's Packet pointer
	POSITION pos = pDocument->GetFirstViewPosition();
	CPartView* pPartView = (CPartView*) pDocument->GetNextView(pos);
	ASSERT(pPartView && pPartView->IsKindOf( RUNTIME_CLASS(CPartView) ));
	CPartTemplate* pTemplate = (CPartTemplate*) pDocument->GetDocTemplate();
	ASSERT(pTemplate && pTemplate->IsKindOf( RUNTIME_CLASS(CPartTemplate) ));
	pPartView->m_pPacket = pTemplate->m_pPacket;

	InitialUpdateFrame(pFrame, pDocument);
	pFrame->OnUpdateFrameMenu(TRUE, pFrame, NULL);

	return pDocument;
}

void CPartTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
	BOOL bMakeVisible /* = TRUE */)
{
	ASSERT_VALID(pFrame);

	// if the frame does not have an active view, set to first pane
	if (pFrame->GetActiveView() == NULL)
	{
		CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			pFrame->SetActiveView((CView*)pWnd,
				!CPartFrame::s_bLockFrame && !CPartFrame::IsWorkspaceLocked());
		}
	}

	if (pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
		((CPartFrame*) pFrame)->OnToolbarStatus();

	if (!CPartFrame::IsWorkspaceLocked())
	{
		// send initial update to all views (and other controls) in the frame
		pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE);

		// finally, activate the frame
		// (send the default show command unless the main desktop window)
		int nCmdShow = -1;      // default
		if (pFrame == AfxGetApp()->m_pMainWnd)
			nCmdShow = AfxGetApp()->m_nCmdShow; // use the parameter from WinMain
		pFrame->ActivateFrame(nCmdShow);

		// now that the frame is visible - update frame counts
		if (pDoc != NULL)
		{
			pDoc->UpdateFrameCounts();
			pFrame->OnUpdateFrameTitle(TRUE);
		}
	}

	// at this point the frame should be properly linked to the document
	ASSERT(pDoc == NULL || pFrame->GetActiveDocument() == pDoc);
}

BOOL CPartTemplate::SaveAll( 
						BOOL bQuery /*= FALSE*/, 
						UINT nFlags /*= FILTER_NONE*/)
{
	CPartDoc* pDoc;

	POSITION pos = GetFirstDocPosition();
	while (pos)
	{
		pDoc = (CPartDoc *) GetNextDoc(pos);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
		
		// Give the document a chance to filter itself.
		if (pDoc->IsFiltered(nFlags))
			continue;

		// Save:
		if (bQuery) 
		{
			if (!pDoc->SaveModified ()) return FALSE;
		}	
		else if (pDoc->IsModified())
		{
			BSTR	bstrLang = NULL;
			
			// If the Doc is untitled,
			// then activate one of its views and call DoSave (NULL) which
			// will bring a a file name dialog.  This is just like OnFileSave/
			// The caller is responsible for restoring the origninal view:
			//
			// skip this code if, we get a valid language name that is "Microsoft TSQL",
			// "Microsoft SQL", or "Oracle SQL+" -- in that case we want to call DoSave
			//	with an empty pathname, which will appropriately save. these strings are
			//	guaranteed to be not localized (DTG package).
			//
			if (pDoc->GetPathName().IsEmpty() && pDoc->GetFirstViewPosition() != NULL &&
				!(SUCCEEDED(pDoc->GetLanguageName(&bstrLang))
					&& bstrLang
					&& (!wcscmp(L"Microsoft TSQL", bstrLang) ||
						!wcscmp(L"Microsoft SQL", bstrLang) ||
						!wcscmp(L"Oracle PL/SQL", bstrLang) ||
						!wcscmp(L"Oracle SQL+", bstrLang)
					   )
				 )
			   )
			{
				POSITION pos = pDoc->GetFirstViewPosition();
				CView* pView = pDoc->GetNextView(pos);
				CPartFrame* pFrame = (CPartFrame*) pView->GetParent();
				while (pFrame != NULL && !pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
					pFrame = (CPartFrame*) pFrame->GetParent();

				if (pFrame != NULL)
				{
					((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIActivate(pFrame);

					if (!pDoc->DoSave(NULL))
						return FALSE;
				}
			}
			else if (!pDoc->DoSave(pDoc->GetPathName() )) return FALSE;

			if (bstrLang)
				::SysFreeString(bstrLang);
		}
	}
	return TRUE;
}

CDocTemplate::Confidence CPartTemplate::MatchDocType(const TCHAR* pszPathName,
					CDocument*& rpDocMatch)
{
	Confidence confidence = CDocTemplate::MatchDocType(pszPathName, rpDocMatch);
	if (confidence < yesAttemptNative)
		confidence = noAttempt; // you have to positive!
	return confidence;
}

BOOL CPartTemplate::CanBeAddedToProject(IPkgProject *pProject)
{
	return TRUE;
}

// Default implementation creates a new file, gives it a name and adds it to
// a project if pProject is not NULL
CPartDoc *CPartTemplate::NewDocumentFile(LPCTSTR szFileName, IPkgProject *pProject, LPCTSTR szFolder)
{
	USES_CONVERSION;
	CString strFileName = szFileName;
	CPartDoc *pDoc = (CPartDoc *)OpenDocumentFile(NULL);
	ASSERT(pDoc == NULL || pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

	if (pDoc != NULL)
	{
		if (!strFileName.IsEmpty())
		{
			BOOL bSave = pDoc->DoSave(strFileName);
			if ( bSave && pProject != NULL)
			{
				LPCOLESTR wstr = T2OLE(szFileName);
				LPOLESTR wstrcopy = (LPOLESTR)malloc((wcslen(wstr)+1) * sizeof(wchar_t));
				wcscpy(wstrcopy, wstr);

				pProject->AddFiles((LPCOLESTR *)&wstrcopy, 1, T2OLE(szFolder));
				free(wstrcopy);
			} 
			if (!bSave){				
				//REVIEW: This is probably not enough if we failed to create the doc,
				//do we need to take it out of some lists or something?
				pDoc->OnCloseDocument();
				
			}
		}
	}
	return pDoc;
}

HICON CPartTemplate::GetIcon() 
{
   return ::LoadIcon(GetPackage()->HInstance(), MAKEINTRESOURCE(GetIconID()));
}

CPartTemplate::FileNewTabType CPartTemplate::GetTabType(void)
{
   CString strName;
   if(GetDocString(strName, CDocTemplate::fileNewName))
      return strName.IsEmpty() ? newnone : newfile;
   return newnone;
}


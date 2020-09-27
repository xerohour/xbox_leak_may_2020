// docauto.cpp: OLE automation methods for CPartDoc class
//
#include "stdafx.h"
#include "errno.h"
#include <utilauto.h>

#include "autostat.h" // g_AutomationState - Internal automation code.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Disable warning for the bool keyword which is needed for variants.
#pragma warning(disable: 4237) //bool

//
// Helper function.
//
BOOL frameBelongsToDoc(CFrameWnd* pFrame, CDocument* pDoc) ;

/////////////////////////////////////////////////////////////////////////////
// CPartDoc OLE automation methods

// Property queries
LPDISPATCH CPartDoc::GetParent()
{
  return GetApplication(); 
}

LPDISPATCH CPartDoc::GetApplication()
{
  return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

BSTR CPartDoc::GetFullName()
{
	CString strResult(GetPathName()); // call MFC's CDocument member

	return strResult.AllocSysString();
}

//
//
//
BSTR CPartDoc::GetName()
{
  CString strResult;
  TCHAR ext[_MAX_EXT];
   
  _tsplitpath(  // use generic mapping to _splitpath or _wsplitpath
    GetPathName(), // full path from CDocument
    0, // drive
    0, // dir
    strResult.GetBuffer(_MAX_FNAME), // file title
    ext); // extension

  strResult.ReleaseBuffer();
  strResult += ext;

  return strResult.AllocSysString();
}

//
//
//
BSTR CPartDoc::GetPath()
{
  CString strResult;
  TCHAR drive[_MAX_DRIVE];
   
  _tsplitpath(  // use generic mapping to _splitpath or _wsplitpath
    GetPathName(), // full path from CDocument
    drive, // drive
    strResult.GetBuffer(_MAX_DIR), // dir
    0, // file title
    0); // extension

  strResult.ReleaseBuffer();

  ASSERT(strResult.GetLength() != 0) ;

  int newLength = strResult.GetLength() - 1; 
  if (strResult[newLength] == _T('\\'))
  {
	strResult = drive + strResult.Left(newLength); // remove the trailing backslash
  }
  else
  {
	  // No Trailing backslash so don't remove.
	strResult = drive + strResult;
  }

  return strResult.AllocSysString();
}

BOOL CPartDoc::GetSaved()
{
  return !IsModified(); // since "saved" & "modified" are opposite
}

void CPartDoc::SetSaved(BOOL bNewValue)
{
  SetModifiedFlag(!bNewValue); // since "saved" & "modified" are opposite
}

LPDISPATCH CPartDoc::NewWindow(void)
{
  // NOTE: this method may not be appropriate for all document types

	CMDIChildWnd* pActiveChild = static_cast<CMDIFrameWnd*>(theApp.m_pMainWnd)->MDIGetActive();

  if (pActiveChild)
	{
		CDocument *pDoc = pActiveChild->GetActiveDocument();

		if (static_cast<CDocument*>(this) != pDoc) // if a view on our document isn't currently active
			pActiveChild = 0; // don't send a 'pOther' param to CreateNewFrame
  }

	CDocTemplate* pTemplate = GetDocTemplate();
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(this, pActiveChild);
	if (pFrame == NULL)
	{
		DsThrowShellOleDispatchException(DS_E_CANNOTCREATENEWWINDOW) ;
	}

	pTemplate->InitialUpdateFrame(pFrame, this);

	return static_cast<CPartFrame*>(pFrame)->GetAutomationObject(TRUE);
}

IDispatch* CPartDoc::ActiveWindow()
{
	// To get the most recently active window for
	// this document, we traverse all of the MDI children
	// checking to see if they below to this documnet.
	// The first one to do so should be the most recently active doc.

		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT_KINDOF(CMainFrame, pMainFrame);

		CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
		if (pMDIActiveChild == NULL)
		{
			return NULL ;
		}

		CPartFrame* pMDIChild = (CPartFrame *)pMDIActiveChild->GetWindow(GW_HWNDFIRST);
		while( pMDIChild != NULL )
		{
			if(!IsValidMDICycleMember(pMDIChild))
			{
				continue;
			}
		
			// Check to see if it belongs to the current document.
			if (frameBelongsToDoc(pMDIChild, this)) // see helper function aboves
			{
				// We found a window which belongs to us.
				return pMDIChild->GetAutomationObject(TRUE);
			}			
			pMDIChild = (CPartFrame *)pMDIChild->GetWindow(GW_HWNDNEXT);
		} // while
		return NULL ;
}

BOOL CPartDoc::ReadOnly()
{
	return (IsReadOnly() || IsReadOnlyOnDisk()) ;
}

// Methods

// Helper function which returns TRUE if the frame belongs to
// the document.
BOOL frameBelongsToDoc(CFrameWnd* pFrame, CDocument* pDoc)
{
	// Get the active view for this frame.
	CView* pActiveView = pFrame->GetActiveView() ;
	if (pActiveView != NULL)
	{
		// Get the document for the active window
		CDocument* pActiveDoc = pActiveView->GetDocument() ;
		if (pActiveDoc != NULL)
		{
			// Is the ActiveDocument the same as this document?
			return (pDoc == pActiveDoc) ;
		}
	}
	return FALSE ;
}

BOOL CPartDoc::GetActive() 
{
	// Get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT_KINDOF(CMainFrame, pMainFrame);

	// Get the active MDI window.
	CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
	if (pMDIActiveChild != NULL)
	{
		ASSERT_KINDOF(CPartFrame, pMDIActiveChild) ;
		return frameBelongsToDoc(pMDIActiveChild, this) ; // see above.
	}
	// We aren't the active document.
	return FALSE ;
}

void CPartDoc::SetActive(BOOL bNewValue) 
{
	if (bNewValue == GetActive())
	{
		// Don't change anything if the current state is okay.
		return ;
	}

	if (bNewValue)
	{
		// Activate this document.
		ActivateDoc() ;
	}
	else
	{
		// To deactivate the document, we need to activate another window.
		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT_KINDOF(CMainFrame, pMainFrame);
		
		// Get the currently active MDI window. This should belong to us,
		// because of the GetActive check above.
		CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
		CPartFrame* pMDIChild = pMDIActiveChild;
		while( TRUE )
		{
			pMDIChild = (CPartFrame *)pMDIChild->GetWindow(GW_HWNDNEXT);
			if(pMDIChild == NULL) // we got to the end of the list
			{
				pMDIChild = (CPartFrame *)pMDIActiveChild->GetWindow(GW_HWNDFIRST);
			}

			if(pMDIChild == pMDIActiveChild) // we completed the loop
			{
				break;
			}

			if(!IsValidMDICycleMember(pMDIChild))
			{
				continue;
			}
		
			// Check to see if it belongs to the current document.
			if (!frameBelongsToDoc(pMDIChild, this)) // see helper function aboves
			{
				//Okay, activate the pMDIChild frame...
				pMDIChild->ActivateFrame();
				// we are done...
				break ;
			}			
		} // while
	} // else
}


DsSaveStatus CPartDoc::Close(const VARIANT FAR& vtLongSaveChanges)
{
	g_AutomationState.SaveChanges(vtLongSaveChanges);
	
	// Attempt to close the document
	//OnCloseDocument() ;
	OnFileClose() ;

	// Return if the save was canceled. (HandleError resets status.)
	DsSaveStatus status = g_AutomationState.SavePromptResult() ;		

	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;

	// Return if the save was canceled.
	return status ;		
}

DsSaveStatus CPartDoc::Save(const VARIANT FAR& vtFilename, const VARIANT FAR& vtBoolPrompt)
{
	// Get Filename paramter
	CString filename;
	::ConvertVariantToCString(/*in*/vtFilename, /*out*/filename, m_strPathName/*Default*/) ;
	
	// Set up the automation state.
	g_AutomationState.SetPromptState(vtBoolPrompt) ;

	// Attempt saving the file.
	BOOL bResult = DoSave(filename) ;

	// Return if the save was canceled.
	DsSaveStatus status = g_AutomationState.SavePromptResult() ;		
	
	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;
	
	if (!bResult && status == dsSaveSucceeded)
	{
		// No error reported, but there is no document.
		TRACE0("Shell Automation Save: We should be reporting an error!") ;
		DsThrowShellOleDispatchException(DS_E_UNKNOWN) ;		
	}

	return status ;
}

IDispatch* CPartDoc::GetWindows()
{
	CAutoWindows *pAW = new CAutoWindows(this);
	return pAW->GetIDispatch(FALSE); // creation is an implicit AddRef

}


// partdoc.cpp : implementation of the CPartDoc class
//

#include "stdafx.h"
#include "io.h" // for access
#include "errno.h"
#include "resource.h"

#include <bldapi.h>
#include <bldguid.h>

#include "AutoDocD.h" //Default Document Automation Object.

#define DOC_CLASS COleDocument

IMPLEMENT_DYNCREATE(CPartDoc, DOC_CLASS)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartDoc

BEGIN_MESSAGE_MAP(CPartDoc, DOC_CLASS)
	//{{AFX_MSG_MAP(CPartDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_READ, OnUpdateIndicator)
	//}}AFX_MSG_MAP
	//
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//
// Creation events. See FireCreationEvent
//
typedef enum
{
	eEventFired = 0,
	eFireOpen = 1,
	eFireNew = 2
} enumFireCreationEvent ;

/////////////////////////////////////////////////////////////////////////////
// CPartDoc construction/destruction

CPartDoc* CPartDoc::s_pSavableDoc = NULL;

BOOL CPartDoc::CanSaveAll()
{
	return (s_pSavableDoc != NULL);
}

void CPartDoc::UpdateSaveAll(BOOL bCreating)
{
	if ((bCreating && s_pSavableDoc != NULL) ||
		(!bCreating && s_pSavableDoc == NULL))
		return;

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CPartTemplate* pTemplate = (CPartTemplate*)theApp.GetNextDocTemplate(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));

		POSITION posDoc =pTemplate->GetFirstDocPosition();
		while (posDoc != NULL)
		{
			CPartDoc* pDoc = (CPartDoc*) pTemplate->GetNextDoc(posDoc);
			ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
			
			// Give the document a chance to filter itself.
			if (!pDoc->IsFiltered(FILTER_DEBUG))
			{
				s_pSavableDoc = pDoc;
				return;
			}
		}
	}

	s_pSavableDoc = NULL;
}

CPartDoc::CPartDoc()
{
	m_pAutoObj = NULL ; // Pointer to the associated automation object.

	m_bReadOnly = FALSE;
	m_bReadOnlyOnDisk = FALSE;
	m_pProxyTemplate = NULL;
	m_nIDString = 0;
	m_FireCreationEvent = eEventFired ; // See FireCreationEvent.

#ifndef NO_VBA
	if (theApp.m_fOleAutomation)
	{
		EnableAutomation();	// enables this->GetIDispatch() for access via OLE automation
	}
#endif	// NO_VBA
}

CPartDoc::~CPartDoc()
{
	//OLEAUTO: Inform the associated automation object that we are gone.
	if (m_pAutoObj != NULL)
	{
		m_pAutoObj->AssociatedObjectReleased() ;
	}


	if (s_pSavableDoc == this)
	{
		if (m_pDocTemplate != NULL)
			m_pDocTemplate->RemoveDocument(this);
		ASSERT(m_pDocTemplate == NULL);     // must be detached

		UpdateSaveAll(FALSE);
	}
}

void CPartDoc::OnFileSave()
{
	LPCTSTR szFilename ;
	if (IsReadOnly())
	{
		szFilename = NULL ;
	}
	else
	{
		szFilename = m_strPathName ;
	}

	if (!DoSave(szFilename))
	{
		TRACE0("Warning: File save failed\n");
	}
}

void CPartDoc::OnFileSaveAs()
{
	if (!DoSave(NULL))
		TRACE0("Warning: File save-as failed\n");
}

// Document closing became complicated because we wanted to fire an OLE Event before the document was invalid.
// So we needed a separate virtual. But since functions overrode OnCloseDocument directly, and MFC calls
// OnClose document directly, we needed to change something. I decided to stop us overriding OnCloseDocument, 
// which is now broken down into two overridables - PreCloseDocument and CloseDocument. Override PreCloseDocument
// to take some action before saving happens, Override CloseDocument to actually save stuff.

// This is the one that gets called to actually make the close happen
void CPartDoc::OnCloseDocument()
{
	PreCloseDocument();
	CloseDocumentHook() ;

	return;
}

void CPartDoc::PreCloseDocument(void)
{
	// Fire the appropriate event
	IDispatch* pAssociatedAutoObj = GetAutomationObject(TRUE) ;
	if(pAssociatedAutoObj  != NULL)
	{
		theApp.m_pAutoApp->FireBeforeDocumentClose(pAssociatedAutoObj);
		pAssociatedAutoObj->Release();
	}
}

void CPartDoc::CloseDocumentHook()
{
#ifdef _AUTOSAVE
	// Check for an autosave file & nuke it before closing the file
	if (!GetPathName().IsEmpty())
	{
		CPath pathAutosaveClobber;
		pathAutosaveClobber.Create(GetPathName());
		gAutosaver.RemoveAutosaveFile(pathAutosaveClobber);
	}
#endif

	// Not a bug
	ParentCloseDocumentHook() ; // Call base class.	
}

//
// This function calls the base class for
// CloseDocumentHook which happens to be OnCloseDocument.
//
void CPartDoc::ParentCloseDocumentHook()
{
	DOC_CLASS::OnCloseDocument(); 
}

BOOL CPartDoc::CanCloseFrame(CFrameWnd *pFrame)
{
	if (DOC_CLASS::CanCloseFrame(pFrame))
	{
		// If the file isn't modified, we don't get a chance to remove
		// an autosave file when the frame gets closed, so we have to
		// do it here.
#ifdef _AUTOSAVE
		if (!GetPathName().IsEmpty())
		{
			CPath pathAutosaveClobber;
			pathAutosaveClobber.Create(GetPathName());
			gAutosaver.RemoveAutosaveFile(pathAutosaveClobber);
		}
#endif
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPartDoc::IsFiltered(UINT nFlags)
{
	// Get the doc's frame (if there is one):
	POSITION pos = GetFirstViewPosition();
	if (pos == NULL)
		return TRUE;	// Filter docs with no views.

	if ((nFlags & (FILTER_DEBUG | FILTER_NON_PROJECT)) != 0)
	{
		CView* pView = GetNextView(pos);
		ASSERT(pView);
		CPartFrame* pFrame = (CPartFrame*) pView->GetParent();
		while (pFrame != NULL && !pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
			pFrame = (CPartFrame*) pFrame->GetParent();

		if ((nFlags & FILTER_DEBUG) != 0 &&
			(pFrame == NULL || pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildDock))))
			return TRUE;

		if ((nFlags & FILTER_NON_PROJECT) != 0)
		{
			extern CPartFrame* GetProjectFrame();

			CPath cp;
			LPBUILDSYSTEM pBldSysIFace;
			theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace);
			CPartFrame* pProjFrame = GetProjectFrame();
		 
			if ((pFrame == NULL || pFrame != pProjFrame) &&
				(pBldSysIFace == NULL || !cp.Create(GetPathName()) ||
				(pBldSysIFace->IsActiveBuilderValid() == S_OK && pBldSysIFace->IsProjectFile(&cp, FALSE, ACTIVE_BUILDER) != S_OK)))
			{
				if (pBldSysIFace)
					pBldSysIFace->Release();
				return TRUE;
			}

			if (pBldSysIFace)
				pBldSysIFace->Release();

			/*
			CPath cp;
			if ( !cp.Create( GetPathName() ) || !GetBldSysIFace()->IsProjectFile(&cp, TRUE ACTIVE_BUILDER) )
			{
				return TRUE;
			}
			*/
		}
	}

	if ((nFlags & FILTER_NEW_FILE) != 0	&& GetPathName().IsEmpty())
		return TRUE;

	return FALSE;
}

// Save as MFC 2.1, but doesn't delete files when the save fails!
//
BOOL CPartDoc::DoSave(const char* pszPathName, BOOL bReplace /*=TRUE*/)
	// Save the document data to a file
	// pszPathName = path name where to save document file
	// if pszPathName is NULL then the user wi..\ll be prompted (SaveAs)
	// note: pszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
	//
	// If the file name changes, then the autosave file [if it exists] for
	// the old name is deleted.
{
	CString newName = pszPathName;
	BOOL bPromptForName = newName.IsEmpty();
	CDocTemplate* pTemplate;
	UINT nSaveType = 0;

	if (!g_pAutomationState->DisplayUI() && bPromptForName) //ShellOM:State
	{
		// We cannot prompt the name because we are being called by an OLE Automation method.
		g_pAutomationState->SetError(DS_E_NOFILENAME) ;
		return FALSE ;
	};

	// We need to check to see if the file is accessable. newName may be different from m_strPathName.
	if (!bPromptForName && _access(newName, 6) != 0 && errno == EACCES)
//	if (!bPromptForName && _access(m_strPathName, 6) != 0 && errno == EACCES)
	{

		if (g_pAutomationState->DisplayUI()) // Can we show UI? ShellOM:State.
		{
			CString strComment;
			strComment.LoadString(IDS_ROCOMMENT);
			CString strMessage = m_strPathName + strComment;
			int nRet = AfxMessageBox(strMessage, MB_OKCANCEL | MB_ICONEXCLAMATION);

			// create a messagebox-like dialog, with Save As (default), Overwrite,
			// and Cancel buttons.
	//		int nRet = CMessageBox::DoMessageBox(strMessage, theApp.m_pszAppName, 
	//			strButtons, MB_ICONEXCLAMATION, 0, 2, NULL, NULL);	// FUTURE: need a parent as the last argument

			if( nRet == IDCANCEL )		// was 2: cancel button
			{
				g_pAutomationState->SetSavePromptCanceled() ; // Inform automation engine that operation was canceled
				return FALSE;
			}
			if( nRet == IDOK )			// was 0: save as button
			{
				bPromptForName = TRUE;
			}
	/*		else		// IDYES		// was overwrite button
			{
				if( !_chmod(m_strPathName, _S_IWRITE) == 0 )
				{
					// unable to affect read-only
					strComment.LoadString(IDS_ROCOMMENT);
					strMessage = m_strPathName + strComment;
					if( CMessageBox::DoMessageBox(strMessage, theApp.m_pszAppName, 
						strButtons, MB_ICONEXCLAMATION, 0, 1, NULL, NULL) == 1 )	// FUTURE: need a parent as the last argument
					{
						return FALSE;		// cancel button
					}
					bPromptForName = TRUE;	// save as button
				}
			}
	*/
		}
		else
		{
			// The Automation Object is not allowing UI, so return an error.
			g_pAutomationState->SetError(DS_E_READONLY) ;
			return FALSE ;
		}
	}

	if (bPromptForName)
	{
		ASSERT(g_pAutomationState->DisplayUI()) ; // Only prompt if not automated. ShellOM:State

		pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (newName.IsEmpty())
		{
			newName = GetDefaultTitle();
			int iBad = newName.FindOneOf("<>:\"/\\|");    // dubious filename
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

			newName.TrimRight();
						
			// append the default suffix if there is one
			CString strExt;
			
#if 0 // [paulde] removed GetDefaultFileExtension()
			// give the package a chance to set it if it's not already set:
			if (m_nIDString == 0)
				SetDocString(GetPackage()->GetDefaultFileExtension());
#endif

			if (GetDocString(strExt, CDocTemplate::filterExt)
				&& !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += GetExtensionCase(newName, strExt);
			}
		}

		if (!theApp.DoPromptFileSave(newName, AFX_IDS_SAVEFILE,
			OFN_PATHMUSTEXIST, this, &nSaveType))
		{
			g_pAutomationState->SetSavePromptCanceled() ; // Inform automation engine that operation was canceled
			return FALSE;       // don't even try to save
		}

		if (IsReadOnly() && (newName.CompareNoCase(m_strPathName) == 0))
		{
			MsgBox(Error, IDS_ERROR_CANT_SAVERO2);
			return FALSE;
		}
	}

	// It is crazy to be recording undo around a save, since you would only be
	// undoing the side effects of the save, but not the save itself.  Also, it 
	// was causing a crash in the RES editor when undoing something that did a save.
	BOOL bUndoPause = FALSE;
	if( theUndoSlob.IsRecording() )
	{
		theUndoSlob.Pause();
		bUndoPause = TRUE;
	}

	if (!OnSaveDocument(newName, nSaveType))
		return FALSE;

	if( bUndoPause )
		theUndoSlob.Resume();

	if (bReplace)
	{
		Replace(newName, nSaveType);
	}
	return TRUE;        // success
}

BOOL CPartDoc::Replace(const char* pszPathName, UINT nSaveType)
{
	// Reset the title and change the document name
	// Clobber an existing autosave file for the old name.
#ifdef _WIN32
	if (!GetPathName().IsEmpty())
	{
		CPath pathAutosaveClobber;
		pathAutosaveClobber.Create(GetPathName());
		gAutosaver.RemoveAutosaveFile(pathAutosaveClobber);
	}
#endif
	CString newName = pszPathName;
	GetActualFileCase( newName );
	SetPathName(newName);
	//
	// NB: can't really assert this because VCPP documents
	// may not have a name.  they override SetPathName and
	// do nothing with it.	they are pseudo documents.
	// [18-Oct-1993 dans]
	//ASSERT(m_strPathName == newName); 	  // must be set
	return TRUE;
}

BOOL CPartDoc::GetValidFilters(UINT** paIDS, UINT* pnInitial)
{
	*paIDS = NULL;
	*pnInitial = 0;
	return FALSE;		// if this isn't overridden, don't expect anything
}

//
// OnSaveDocument 
// DO NOT OVERRIDE THIS FUNCTION! 
// Instead, override SaveDocumentHook.
//
BOOL CPartDoc::OnSaveDocument(const char* pszPathName, UINT nSaveType)
{
	BOOL b = SaveDocumentHook(pszPathName, nSaveType) ;
	return PostSaveDocument(b) ;
};

//
// PostSaveDocument
// Fires the automation event after the document has been saved.
//
BOOL CPartDoc::PostSaveDocument(BOOL bResult)
{
	if (bResult)
	{
		// Fire the appropriate event
		IDispatch* pAssociatedAutoObj = GetAutomationObject(TRUE) ;

		if(pAssociatedAutoObj  != NULL)
		{
			theApp.m_pAutoApp->FireDocumentSave(pAssociatedAutoObj);
			pAssociatedAutoObj->Release();
		}
	}
	return bResult ;
};

//
// SaveDocumentHook
// Packages can override this function.
//
BOOL CPartDoc::SaveDocumentHook(const char* pszPathName, UINT nSaveType)
{
	TRACE("CPartDoc::SaveDocumentHook: %s\n", pszPathName);

#ifdef _AUTOSAVE
	if (*pszPathName != '\0')
	{
		CPath pathAutosaveClobber;
		pathAutosaveClobber.Create(pszPathName);
		gAutosaver.RemoveAutosaveFile(pathAutosaveClobber);
	}
#endif

	SetModifiedFlag(FALSE);

	return TRUE ;
}

//
// Call the base class version of SaveDocumentHook which
// is really OnSaveDocument
//
BOOL CPartDoc::ParentSaveDocumentHook(const char* pszPathName, UINT nSaveType)
{
	return DOC_CLASS::OnSaveDocument(pszPathName) ;
}	


//
// OnNewDocument 
// DO NOT OVERRIDE THIS FUNCTION! 
// Instead, override NewDocumentHook.
//
BOOL CPartDoc::OnNewDocument()
{
	BOOL b = NewDocumentHook() ;
	return PostNewDocument(b) ;
};

//
// PostNewDocument
// Fires the automation event after the document has been created.
//
BOOL CPartDoc::PostNewDocument(BOOL bResult)
{
	if (bResult)
	{
		// See FireCreationEvent
		m_FireCreationEvent = eFireNew ;
	}

	return bResult;
};

//
// NewDocumentHook
// Packages can override this function to customize creating
// new documents.
//
BOOL CPartDoc::NewDocumentHook()
{
	if (!ParentNewDocumentHook())
		return FALSE;

	UpdateSaveAll(TRUE);

	return TRUE;
}

//
// This function calls the base class for
// NewDocumentHook which happens to be OnNewDocument.
//
BOOL CPartDoc::ParentNewDocumentHook()
{
	return DOC_CLASS::OnNewDocument() ;
}


//
// OnOpenDocument 
// DO NOT OVERRIDE THIS FUNCTION! 
// Instead, override OpenDocumentHook.
//
BOOL CPartDoc::OnOpenDocument(const char* pszPathName)
{
	BOOL b = OpenDocumentHook(pszPathName) ;
	return PostOpenDocument(b) ;
}

//
// PostOpenDocument
// Fires the automation event after the document has been opened.
//
BOOL CPartDoc::PostOpenDocument(BOOL bResult)
{
	if (bResult)
	{
		// See FireCreationEvent
		m_FireCreationEvent = eFireOpen ;
	}

	return bResult ;
}

//
// OpenDocumentHook
// Packages can override this function to customize creating
// new documents.
//
BOOL CPartDoc::OpenDocumentHook(const char* pszPathName)
{
	CString strPathName = pszPathName;
	GetActualFileCase( strPathName );
	SetPathName( strPathName );
	SetModifiedFlag(FALSE);

#ifdef _AUTOSAVE
	// Check for an autosaved backup copy.  Returns FALSE if the user cancels.
	CPath pathToOpen;
	pathToOpen.Create( strPathName );
	if (!gAutosaver.RecoverAutosaveFileCheck(pathToOpen))
		return FALSE;
#endif

	UpdateSaveAll(TRUE);

	return TRUE;
}

//
// This function calls the base class for
// OpenDocumentHook which happens to be OnOpenDocument.
//
BOOL CPartDoc::ParentOpenDocumentHook(const char* pszPathName)
{
	return DOC_CLASS::OnOpenDocument(pszPathName) ;
}


void CPartDoc::ActivateDoc()
{
	POSITION pos = GetFirstViewPosition();
	if (pos == NULL)
		return;

	CView* pView = GetNextView(pos);
	if (pView == NULL)
		return;

	CFrameWnd* pFrame = pView->GetParentFrame();
	if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
		pFrame->ActivateFrame();
}

BOOL CPartDoc::AttemptCloseDoc()
{
	InformationBox(ERR_Other_Type_Open);
	return FALSE;
}

BOOL CPartDoc::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

	CString name = m_strPathName;
	if (name.IsEmpty())
		name = m_strTitle;

#ifdef _AUTOSAVE
	CPath pathAutosave;
#endif

	int iOption = IDYES ;
	if (g_pAutomationState->DisplayUI()) //ShellOM:State
	{
		// Display UI
		CString prompt;
		AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
		iOption = AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE);
	}
	else
	{
		// Don't Display UI
		iOption = g_pAutomationState->SaveChanges() ? IDYES : IDNO ;
	}

	// Save or Loose changes.
	switch (iOption)
	{
	case IDCANCEL:
		g_pAutomationState->SetSavePromptCanceled() ; // Inform automation engine that operation was canceled
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate.
		// Somewhere down DoSave() we do autosave file clobbering.
		if (!DoSave(m_strPathName))
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document & clobber autosave file.
		// Don't do this if the document is untitled though!
		// FUTURE:  In here we must remove autosave files for untitled documents!!

#ifdef _AUTOSAVE
		if (!m_strPathName.IsEmpty())
		{
			pathAutosave.Create(m_strPathName);
			gAutosaver.RemoveAutosaveFile(pathAutosave);
		}
#endif
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

/////////////////////////////////////////////////////////////////////////////
// CPartDoc serialization

void CPartDoc::Serialize(CArchive& ar)
{
	ASSERT(FALSE); // we don't do archives!
}

/////////////////////////////////////////////////////////////////////////////
// CPartDoc diagnostics

#ifdef _DEBUG
void CPartDoc::AssertValid() const
{
	DOC_CLASS::AssertValid();
}

void CPartDoc::Dump(CDumpContext& dc) const
{
	DOC_CLASS::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPartDoc commands

CSlob* CPartDoc::GetSlob()
{
	return NULL;
}


BOOL CPartDoc::IsReadOnly()
{
	return m_bReadOnly;
}

// override this to supply more interesting handling of R/O condions (check out)
BOOL CPartDoc::IsReadOnlyEx(BOOL bHandleError /* = TRUE */)
{
	if ((m_bReadOnly) && (bHandleError))
	{
		MessageBeep(0);
	}
	return m_bReadOnly;
}

void CPartDoc::SetReadOnly(BOOL bReadOnly /* = TRUE */)
{
	m_bReadOnly = bReadOnly;
}

void CPartDoc::OnUpdateIndicator(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID == ID_INDICATOR_READ)
		pCmdUI->Enable(IsReadOnly() || IsReadOnlyOnDisk());

#ifdef _DEBUG
	else
		ASSERT(FALSE);
#endif
}

void CPartDoc::SetModifiedFlag(BOOL bModified)
{
	if (bModified == m_bModified)
		return;
	
	DOC_CLASS::SetModifiedFlag(bModified);
	UpdateFrameCounts();
}

BOOL CPartDoc::GetDocString(CString& rString, enum CDocTemplate::DocStringIndex i) const
{
	if( m_nIDString != 0 )
	{
		CString strDocString;
		strDocString.LoadString(m_nIDString);
		return AfxExtractSubString(rString, strDocString, (int)i);
	}
	else if (m_pProxyTemplate != NULL)
	{
		return m_pProxyTemplate->GetDocString(rString, i);
	}

	ASSERT(GetDocTemplate() != NULL);
	return GetDocTemplate()->GetDocString(rString, i);
}

BOOL CPartDoc::OnFileChange(int nPackage, FCItem* pFCItem)
{
	// derived document classes that handle file change notifications
	// can override this function, and see if the file that changed is yours.
	return FALSE;
}

long CPartDoc::GetDocLong(int nVar)
{
	switch (nVar)
	{
	case GDL_PROXYTEMPLATE:
		return (long) m_pProxyTemplate;
	default:
		ASSERT(FALSE);	// unsupported value
		break;
	}

	return 0x0;		// there is no good error code
}

long CPartDoc::SetDocLong(int nVar, long lData)
{
	long lRet = GetDocLong(nVar);	// get previous value, to return later

	switch (nVar)
	{
	case GDL_PROXYTEMPLATE:
		m_pProxyTemplate = (CPartTemplate*)lData;
		break;
	default:
		ASSERT(FALSE);	// unsupported value
		break;
	}

	return lRet;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  OLE Automation support functions.
//

//
// Override to return the automation object corresponding to this object.
// Default creates an automation object and returns it.
//
IDispatch* CPartDoc::GetAutomationObject(BOOL bAddRef)
{
	if (m_pAutoObj)
	{
		return m_pAutoObj->GetIDispatch(bAddRef);
	}

	m_pAutoObj = new CAutoDocumentDefault(this) ;
	return m_pAutoObj->GetIDispatch(FALSE); // creation is an implicit AddRef
}

void CPartDoc::AutomationObjectReleased()
{
	m_pAutoObj = NULL ;
}


///////////////////////////////////////////////////////////
//
// Orion 96 Bug 12176 - We must fire the New and Open events
// after the windows have been created, but before the activation
// events for the windows. Therefore, we set a variable and wait
// for the window to get activated. This funciton is called by
// the code which fires the window activation. 
//
void CPartDoc::FireCreationEvent()
{
	//WARNING: The automation method may delete this document
	// during this call. So, we can't expect to do anything which
	// uses member variables after this point!

	// Save the member variable to a temp var on the stack.
	UINT FireCreationEventTemp = m_FireCreationEvent ;

	// Reset the member variable.
	m_FireCreationEvent = eEventFired ;

	// From now on use the temp.
	if (FireCreationEventTemp != eEventFired )
	{
		// Fire the appropriate event
		IDispatch* pAssociatedAutoObj = GetAutomationObject(TRUE) ;
		if(pAssociatedAutoObj  != NULL)
		{
			if (FireCreationEventTemp == eFireOpen)
			{
				theApp.m_pAutoApp->FireDocumentOpen(pAssociatedAutoObj);
			}
			else
			{
				theApp.m_pAutoApp->FireNewDocument(pAssociatedAutoObj);
			}
			pAssociatedAutoObj->Release();
		}
	}
}
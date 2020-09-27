// ipcmpdoc.cpp : implementation of the CIPCompDoc class
//

#include "stdafx.h"
#pragma hdrstop

#include <afxodlgs.h>
#include <ocdevo.h>
#include "shldocs_.h"
#include "ipcmpctr.h"
#include "ipcfrmhk.h"
#include "ipcmpvw.h"
#include "ipcslob.h"
#include "ipcundo.h"
#include "docobfrm.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CIPCompDoc, CPartDoc)
IMPLEMENT_DYNAMIC(CIPCompDocTemplate, CPartTemplate)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPCompDoc


BEGIN_MESSAGE_MAP(CIPCompDoc, CPartDoc)
	//{{AFX_MSG_MAP(CIPCompDoc)

	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)

	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)

	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCompDoc construction/destruction

CIPCompDoc::CIPCompDoc()
{
	m_pIpCompSlob = NULL;
	m_pItem = NULL;
	m_fDocObjViewUIActive = FALSE;
	m_clsid = GUID_NULL;
}

CIPCompDoc::~CIPCompDoc()
{
	ASSERT_VALID(this);
	if(NULL != m_pIpCompSlob)
	{
		delete m_pIpCompSlob;
		m_pIpCompSlob = NULL;
	}
}

BOOL CIPCompDoc::NewOrLoadStorage(LPCTSTR pszPathName, IStorage ** ppStg, BOOL fNew)
{
	BOOL fRet = TRUE;
	if (fNew)
	{
		if (FAILED(StgCreateDocfile(NULL, STGM_READWRITE | STGM_DIRECT | STGM_SHARE_EXCLUSIVE | STGM_DELETEONRELEASE,
									 0, ppStg)))
			fRet = FALSE;
	}
	else
	{
		USES_CONVERSION;
		ASSERT(pszPathName != NULL);
		if (FAILED(StgOpenStorage(T2W(pszPathName), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 
									 NULL, 0, ppStg)))
			fRet = FALSE;
	}
	return fRet;
}

BOOL CIPCompDoc::NewDocumentHook()
{
	ASSERT(m_clsid != GUID_NULL);

	// IS Bug 6274 (to update SaveAll UI status)
	if (!CPartDoc::NewDocumentHook())
		return FALSE;

	CWaitCursor wc;
	COleRef<IStorage> srpStorage;
	NewOrLoadStorage(NULL, &srpStorage, TRUE);
		
	m_srpUndoMgr = new COleUndoManager;
	m_pItem = new CIPCompContainerItem(this);
	m_pIpCompSlob = new CIPCompSlob(this);
	BOOL fRet = FALSE;

	if(m_pItem && m_srpUndoMgr && m_pIpCompSlob && m_pIpCompSlob->Create() &&
		m_pItem->CreateNewFromClsid(m_clsid, srpStorage))
		fRet = InitOrLoadPersistence(srpStorage, TRUE);

	// it didn't work ... clean up
	if(!fRet)
	{
		::AfxMessageBox(IDS_ERROR_CANT_CREATE_IPCMPDOC);
		DoClose();
	}

	return fRet;
}

BOOL CIPCompDoc::OpenDocumentHook(LPCTSTR lpszPathName)
{
	ASSERT(m_clsid != GUID_NULL);

	CWaitCursor wc;
	// Note that COleUndoManager's constructor sets the
	// object's reference count to 1.
	//
	COleRef<IStorage> srpStorage;
	BOOL fRet = FALSE;

	if (!NewOrLoadStorage(lpszPathName, &srpStorage, FALSE))
	{
		// NewOrLoadStorage may not display the error.
		//
		::AfxMessageBox(IDS_ERROR_CANT_CREATE_IPCMPDOC);
	}
	else
	{
		m_srpUndoMgr = new COleUndoManager;
		m_pItem = new CIPCompContainerItem(this);
		m_pIpCompSlob = new CIPCompSlob(this);
	
		if(m_pItem && m_srpUndoMgr && m_pIpCompSlob && m_pIpCompSlob->Create() &&
			m_pItem->CreateNewFromClsid(m_clsid, srpStorage))
		{
			// If this fails, it will display an error message of its own
			//
			fRet = InitOrLoadPersistence(srpStorage, FALSE);
		}
		else
		{
			// CreateNewFromClsid doesn't display errors
			//
			::AfxMessageBox(IDS_ERROR_CANT_CREATE_IPCMPDOC);
		}
	}

	// it didn't work ... clean up
	if (!fRet)
	{
		DoClose();
	}
	else //$UNDONE, this is added because CPartTemplate incorrectly left it out.  
	{
		// Set Beautified path name
		CString strPathName(lpszPathName);
		GetActualFileCase(strPathName);
		SetPathName(strPathName);

		// IS Bug 6274 (to update SaveAll UI status)
		if (!CPartDoc::OpenDocumentHook(lpszPathName))
			return FALSE;

		// Check for ReadOnly file
		DWORD dwAttrib = GetFileAttributes(lpszPathName);
		if(-1 != dwAttrib && (dwAttrib & FILE_ATTRIBUTE_READONLY))
			// File is ReadOnly so set the flag
			SetReadOnlyOnDisk(TRUE);
	}

	return fRet;
}

void CIPCompDoc::CloseDocumentHook()
{
	// delete the contained item if any...this will close the doc object view.
	DoClose();
	CPartDoc::CloseDocumentHook();
}

void CIPCompDoc::DoClose()
{
	m_fDocObjViewUIActive = FALSE;
	
	if (m_pItem != NULL)
	{
		m_pItem->DoClose();
		CIPCompContainerItem * pTempItem = m_pItem;
		m_pItem = NULL;
		pTempItem->InternalRelease();
		OleRelease(m_lpRootStg);
	}
}


BOOL CIPCompDoc::CanCloseFrame(CFrameWnd *pFrame)
{
	BOOL fRet = TRUE;
	// if pFrame is NULL, only check whether we can close the container item,
	// do NOT call CanCloseFrame which will cause prompting to the user
	if (pFrame && m_fDocObjViewUIActive)
		fRet = COleDocument::CanCloseFrame(pFrame);

	if (fRet && m_pItem != NULL)
		fRet = m_pItem->CanClose();

	return fRet;
}

BOOL CIPCompDoc::InitOrLoadPersistence(IStorage * pStg, BOOL fNew)
{
	ASSERT(m_pItem != NULL);
	ASSERT(m_pItem->m_lpObject != NULL);
	
	BOOL fRet = FALSE;
	if (m_pItem && m_pItem->m_lpObject)
	{
		COleRef<IPersistStorage> srpPersistStorage;
		
		if (SUCCEEDED(m_pItem->m_lpObject->QueryInterface(IID_IPersistStorage,
					(void **)&srpPersistStorage)))
		{
			ASSERT(srpPersistStorage != NULL);
			HRESULT hr = E_FAIL;
			
			// Initialize the storage
			if (fNew)
				hr = srpPersistStorage->InitNew(pStg);
			else
				hr = srpPersistStorage->Load(pStg);
				
			if (SUCCEEDED(hr))
				fRet = TRUE;
		}
	}

	return fRet;
}

// OleCmdFromId maps an MFC command ID (i.e. command ID's routed
// to OnCmdMsg) to a (Guid, cmdid) pair recognized by IMsoCommandTarget.
// Derived classes should override this function to give access to
// the commands recognized by the components they host.
//
BOOL CIPCompDoc::OleCmdFromId(UINT, GUID **, DWORD *, DWORD *)
{
	// Base class implementation doesn't have any mapping information.
	//
	return FALSE;
}

// PopupDescFromGuidId maps a (Guid, MenuId) pair (as used by
// IOleComponentUIManager::ShowContextMenu) to a popup-menu
// description which can be passed to ::ShowContextPopupMenu.
// Derived classes should override this function using their
// hardwired knowledge about the components they host.
//
POPDESC * CIPCompDoc::PopupDescFromGuidId(REFGUID rguid, UINT nMenuId)
{
	// Base class implementation doesn't have any mapping information.
	//
	return FALSE;
}

IUnknown *CIPCompDoc::GetTrackSelection()
{
	ASSERT(NULL != m_pIpCompSlob);
	if(NULL == m_pIpCompSlob)
		return NULL;
	else
		return m_pIpCompSlob->GetTrackSelection();
}

IUnknown *CIPCompDoc::GetUndoActionManager()
{
	return m_srpUndoMgr;
}

IMsoCommandTarget * CIPCompDoc::GetSupportingTarget(GUID * pguid,
		DWORD cmdid, DWORD rgf)
{
	if (m_pItem == NULL)
		return NULL;

	CIPCompFrameHook *pFrameHook = (CIPCompFrameHook *)(m_pItem->m_pInPlaceFrame);

	return (pFrameHook != NULL) ?
				pFrameHook->GetSupportingTarget(pguid, cmdid, rgf) :
				NULL;
}

void CIPCompDoc::SetIPCComboEditText(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf)
{
	if (m_pItem == NULL)
		return;

	CIPCompFrameHook *pFrameHook = (CIPCompFrameHook *)(m_pItem->m_pInPlaceFrame);

	if (pFrameHook != NULL)
	{
		pFrameHook->SetIPCComboEditText(pcombo, pguid, cmdid, rgf);
	}
}

void CIPCompDoc::FillIPCCombo(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf)
{
	if (m_pItem == NULL)
		return;

	CIPCompFrameHook *pFrameHook = (CIPCompFrameHook *)(m_pItem->m_pInPlaceFrame);

	if (pFrameHook != NULL)
	{
		pFrameHook->FillIPCCombo(pcombo, pguid, cmdid, rgf);
	}
}

void CIPCompDoc::OnIPCComboSelect(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf)
{
	if (m_pItem == NULL)
		return;

	CIPCompFrameHook *pFrameHook = (CIPCompFrameHook *)(m_pItem->m_pInPlaceFrame);

	if (pFrameHook != NULL)
	{
		pFrameHook->OnIPCComboSelect(pcombo, pguid, cmdid, rgf);
	}
}

void CIPCompDoc::OnIPCComboEnter(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf)
{
	if (m_pItem == NULL)
		return;

	CIPCompFrameHook *pFrameHook = (CIPCompFrameHook *)(m_pItem->m_pInPlaceFrame);

	if (pFrameHook != NULL)
	{
		pFrameHook->OnIPCComboEnter(pcombo, pguid, cmdid, rgf);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CIPCompDoc serialization

void CIPCompDoc::Serialize(CArchive& ar)
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
// CIPCompDoc diagnostics

#ifdef _DEBUG
void CIPCompDoc::AssertValid() const
{
	CPartDoc::AssertValid();
}

void CIPCompDoc::Dump(CDumpContext& dc) const
{
	CPartDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIPCompDoc commands
VOID
CIPCompDoc::OnEditUndo()
{
	if (m_srpUndoMgr == NULL)
		return;

	int cReps = max(1, theApp.GetCmdArg());

	for (int i = 0; i < cReps && !m_srpUndoMgr->IsUndoEmpty(); i++)
	{
		m_srpUndoMgr->UndoTo(NULL);
	}
	theApp.ClearRepCount();
}

VOID
CIPCompDoc::OnEditRedo()
{
	if (m_srpUndoMgr == NULL)
		return;

	int cReps = max(1, theApp.GetCmdArg());

	for (int i = 0; i < cReps && !m_srpUndoMgr->IsRedoEmpty(); i++)
	{
		m_srpUndoMgr->RedoTo(NULL);
	}
	theApp.ClearRepCount();
}

void CIPCompDoc::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	// Enable/disable the entry
	pCmdUI->Enable(m_srpUndoMgr != NULL	&& !m_srpUndoMgr->IsUndoEmpty());
}

void CIPCompDoc::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	// Enable/disable the entry
	pCmdUI->Enable(m_srpUndoMgr != NULL && !m_srpUndoMgr->IsRedoEmpty());
}

LRESULT
CIPCompDoc::GetUndoRedoString(WPARAM nLevel, LPARAM lpstr,
		UINT cchBuf, BOOL fUndo)
{
	USES_CONVERSION;
	HRESULT hr;

	if (m_srpUndoMgr == NULL)
		return FALSE;

	COleRef<IEnumOleUndoUnits> srpEnum;
	COleRef<IOleUndoUnit> srpUndoAction;

	if (fUndo)
	{
		hr = m_srpUndoMgr->EnumUndoable(&srpEnum);
	}
	else
	{
		hr = m_srpUndoMgr->EnumRedoable(&srpEnum);
	}

	if (!SUCCEEDED(hr) || srpEnum == NULL)
		return FALSE;

	//nLevel is zero based.
	//
	hr = srpEnum->Skip(nLevel);
	if (!SUCCEEDED(hr))
		return FALSE;

	hr = srpEnum->Next(1, &srpUndoAction, NULL);

	if (!SUCCEEDED(hr) || hr == S_FALSE || srpUndoAction == NULL)
		return FALSE;

	BSTR bstr;
	hr = srpUndoAction->GetDescription(&bstr);

	if (!SUCCEEDED(hr))
		return FALSE;

	// convert the bstr into an lpstr and copy it into lpstr
	//
	lstrcpyn((LPSTR)lpstr, OLE2T((WCHAR *)bstr), cchBuf);
	SysFreeString(bstr);

	return (lstrlen((LPSTR)lpstr) != 0);
}

IOleObject * CIPCompDoc::GetOleObject()
{
	return (m_pItem ? m_pItem->m_lpObject : NULL);
}

IStorage * CIPCompDoc::GetStorage()
{
	return (m_pItem ? m_pItem->m_lpStorage : NULL);
}

CSlob *	CIPCompDoc::GetSlob()
{
	return m_pIpCompSlob;
}

void CIPCompDoc::BeginUndo(LPCTSTR szDescription)
{
	USES_CONVERSION;

	ASSERT(szDescription);
	ASSERT(m_srpUndoMgr);

	COleParentUndoUnit * pCUA = 
		new COleParentUndoUnit(atUndoAction, T2OLE(szDescription));
	pCUA->AddRef();
	m_UndoStack.AddHead(pCUA);
	m_srpUndoMgr->Open(pCUA);
	pCUA->Release();
}

BOOL CIPCompDoc::SetUndoStringMgr(IPropertyPageUndoString *pPPUS)
{
	ASSERT(NULL != pPPUS);
	ASSERT(m_srpUndoStrMgr == NULL);
	if(NULL == pPPUS || m_srpUndoStrMgr != NULL)
		return FALSE;

	pPPUS->AddRef();
	m_srpUndoStrMgr = pPPUS;
	return TRUE;
}

void CIPCompDoc::EndUndo(BOOL bAbort /*= FALSE*/)
{
	ASSERT(m_srpUndoMgr);
	ASSERT(!m_UndoStack.IsEmpty());

	LPOLESTR pStr = NULL;
	COleParentUndoUnit *pCUA = m_UndoStack.RemoveHead();
	ASSERT(NULL !=pCUA);
	if(!pCUA->IsUndoActionListEmpty() && !bAbort)
	{
		if(m_srpUndoStrMgr != NULL && SUCCEEDED(m_srpUndoStrMgr->GetUndoString(&pStr)))
		{
 			USES_CONVERSION;
			ASSERT(NULL != pStr);
			pCUA->SetUndoDesc(OLE2T(pStr));
			AfxFreeTaskMem(pStr);
		}
		m_srpUndoMgr->Close(pCUA, !bAbort);
	}
	else
		m_srpUndoMgr->Close(pCUA, FALSE /*don't commit*/);

	m_srpUndoStrMgr.SRelease();
}

HRESULT	CIPCompDoc::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	IUnknown *punk = NULL;

	if(NULL == ppvObj)
		return E_INVALIDARG;

	*ppvObj = NULL;

	if(SID_STrackSelection == guidService)
	{
		// this is an non-addref'ed pointer
		punk = GetTrackSelection();
	}
	// the Service ID for the Undo Action Manager is the same
	// as the Interface ID for IOleUndoManager.
	else if(IID_IOleUndoManager == guidService)
	{
		// this is an non-addref'ed pointer
		punk = m_srpUndoMgr;
	}

	if(NULL != punk)
		return punk->QueryInterface(riid, ppvObj);
	else
		return E_NOINTERFACE;
}

HRESULT CIPCompDoc::ShowIPCHelp(DWORD dwCompRole, REFCLSID rclsidComp,
		POINT posMouse, DWORD dwHelpCmd, LPOLESTR pwszHelpFile, DWORD dwData)
{
	return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CDocObjectTemplate commands

CIPCompDocTemplate::CIPCompDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CPackage* pPackage, CPacket* pPacket, REFCLSID clsid)
	: CPartTemplate(nIDResource,
		pDocClass,
		RUNTIME_CLASS(CDocObjectFrame),
		RUNTIME_CLASS(CIPCompView),
		pPackage,
		pPacket,
		clsid)
{
}

void CIPCompDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible)
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

	CIPCompView * pView = (CIPCompView *)pFrame->GetActiveView();
	ASSERT(pView);
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CIPCompView)));
	
	HRESULT hr = NOERROR;
	if (SUCCEEDED(hr = pView->OnActivateDocObjectView()))
		CPartTemplate::InitialUpdateFrame(pFrame, pDoc, bMakeVisible);
	else
	{
		if (hr != OLE_E_PROMPTSAVECANCELLED)
			::AfxMessageBox(IDS_ERROR_CANT_CREATE_IPCMPDOC);
			
		::PostMessage(pFrame->m_hWnd, WM_CLOSE, NULL, NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////
// LoadErrorInfo diagnostics
void LoadErrorInfo(IErrorInfo * pei, CString& strDescription, CString& strHelpFile, DWORD * pdwHelpContext)
{
	ASSERT(pei != NULL);

	BSTR bstr = NULL;
	if (SUCCEEDED(pei->GetDescription(&bstr) && bstr))
	{
		strDescription = bstr;
		::SysFreeString(bstr);
		bstr = NULL; 
	}

	if (SUCCEEDED(pei->GetHelpFile(&bstr) && bstr))
	{
		strHelpFile = bstr;
		::SysFreeString(bstr);
		bstr = NULL; 
	}
	
	pei->GetHelpContext(pdwHelpContext);
}


void ShowErrorForInterface(REFIID riid, IUnknown * pUnk, HRESULT hr)
{
	ASSERT(pUnk);

	if (hr == OLE_E_PROMPTSAVECANCELLED)
		return;
		
	COleRef<ISupportErrorInfo> srpSupportErrInfo;
	
	if (pUnk->QueryInterface(IID_ISupportErrorInfo, (void**)&srpSupportErrInfo) == S_OK)
	{
		if (srpSupportErrInfo->InterfaceSupportsErrorInfo(riid) == S_OK)
		{
			COleRef<IErrorInfo> srpErrInfo;
			
			::GetErrorInfo(0, &srpErrInfo);

			if (srpErrInfo != NULL)
			{
				CString strDesc;
				CString strHelpFile;
				DWORD   dwContextId;

				::LoadErrorInfo(srpErrInfo, strDesc, strHelpFile, &dwContextId);
				::AfxMessageBox(strDesc);
			}
		}
	}
}

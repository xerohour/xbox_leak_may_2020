// ipcundo.cpp : implementation of the COleUndoManager and
// CEnumOleUndoUnit classes.
//

#include "stdafx.h"
#pragma hdrstop

#include "ipcundo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//$UNDONE -- these Guids are defined in ocx96.lib; once we have
// versions of that library for all packages, we can link to it
// and remove these definitions.
//
DEFINE_GUID(IID_IOleUndoManager,	0xd001f200, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IOleUndoUnit, 	0x894ad3b0, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IOleParentUndoUnit, 0xa1faf330, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IEnumOleUndoUnits,	0xb3e7c340, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);

// Macros ---------------------------------------------------------------------
inline HRESULT MapAbortResult(HRESULT hr)
	{ return ((hr == E_ABORT) ? E_FAIL : hr); }

/////////////////////////////////////////////////////////////////////////////
// COleUndoManager implementation
//

// Note that the constructor sets the reference count to one.
//
COleUndoManager::COleUndoManager() :
m_cRef(1),
m_cRollbackCtr(0),
m_fEnable(TRUE),
m_fUndoStackIsEmpty(TRUE),
m_fRedoStackIsEmpty(TRUE),
m_usManagerState(usBaseState),
m_pOpenUndoAction(NULL),
m_pOpenRedoAction(NULL)

{
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::~COleUndoManager

Description: Destructor for COleUndoManager Object

Return Values: None

-----------------------------------------------------------------------------*/
COleUndoManager::~COleUndoManager()
{
DiscardFrom(NULL); // Release everything in the Undo/Redo stacks
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::QueryInterface

Description:
	Query the object for an OLE interface.

Return Values:
	NO_ERROR to indicate the interface is supported, in which case
	*ppvObj is filled in; otherwise, *ppvObj is set to NULL.
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::QueryInterface (REFIID riid,
		void ** ppvObj)
{
	if (ppvObj == NULL)
		return E_INVALIDARG;
		
	*ppvObj = NULL;

	if (riid==IID_IUnknown)
		*ppvObj = (void *)(IUnknown *)this;
	else if (riid==IID_IOleUndoManager)
		*ppvObj = (void *)(IOleUndoManager *)this;
	else
		return E_NOINTERFACE;

	((IUnknown *)*ppvObj)->AddRef();
	return NOERROR;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::AddRef

Description:
	Add a reference to this object.

Return Values:
	The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleUndoManager::AddRef()
{
	return( ++m_cRef );
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::Release

Description:
	Release a reference to this object.

Return Values:
	The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleUndoManager::Release()
{
	ASSERT( m_cRef > 0 );
	int cRef = --m_cRef;
	if (cRef==0)
		delete this;
	return cRef;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::Open

Description: Open a new empty compound action onto the Undo/Redo stack

Return Values:	S_OK, if OK
				E_UNEXPECTED, if not enabled
				E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::Open
(
IOleParentUndoUnit *pCUA // Pointer to Compound Undo Action
)
{
	HRESULT hr = S_OK;

	TRY

	if(!pCUA)
		{
		ASSERT(FALSE);
		return E_INVALIDARG;
		}
	if(!m_fEnable)
		return S_OK;

	POSITION posUndo;

	if(m_pOpenUndoAction||m_pOpenRedoAction)
		{
		switch(m_usManagerState)
			{
			  case usBaseState:
			  case usRedoState:
					ASSERT(m_pOpenUndoAction);
					if(m_usManagerState==usBaseState)
						m_pOpenUndoAction->OnNextAdd();
					return m_pOpenUndoAction->Open(pCUA);
					break;
			  case usUndoState:
					ASSERT(m_pOpenRedoAction);
					return m_pOpenRedoAction->Open(pCUA);
					break;
			  default:
					ASSERT(FALSE);
					return E_FAIL;
					break;
			}
		}
	switch(m_usManagerState)
		{
		  case usBaseState:				
				// Call OnNextAdd on the top of stack
				posUndo = m_UndoStack.GetHeadPosition();
				if(posUndo != NULL)
					m_UndoStack.GetAt(posUndo)->OnNextAdd();
				m_pOpenUndoAction=pCUA;
				break;
		  case usRedoState:
				m_pOpenUndoAction=pCUA;
				break;
		  case usUndoState:
				m_pOpenRedoAction=pCUA;
				break;
		  default:
				ASSERT(FALSE);
				return E_FAIL;
				break;
		}
	// We're keeping the interface pointer pUA so AddRef it!
	pCUA->AddRef();

	CATCH_ALL(pe)
	{
		hr = COleException::Process(pe);
		pe->Delete();
	}
	END_CATCH_ALL

	return hr;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::Close

Description: Close the currently open compound undo action list

Return Values: S_OK, if ok or disabled
			   E_FALSE, if no open child
			   E_INVALIDARG, if pCUA is not the currently open object
			   E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::Close
(
IOleParentUndoUnit *pCUA, // Pointer to Compound Undo Action
BOOL fCommit // Flag to keep or discard the completed compound undo action list
)
{
	HRESULT hr = S_OK;

	if(!m_fEnable)
		return S_OK;

	TRY

		if(m_pOpenUndoAction||m_pOpenRedoAction)
		{
			switch(m_usManagerState)
			{
			case usBaseState:
			case usRedoState:
				ASSERT(m_pOpenUndoAction);
				hr=m_pOpenUndoAction->Close(pCUA, fCommit);
				if(hr!=S_FALSE)
					return hr;
				if(pCUA!=m_pOpenUndoAction)
					return E_INVALIDARG;
				if(!fCommit)
				{
					m_pOpenUndoAction->Release();
					m_pOpenUndoAction=NULL;
					return S_OK;
				}
#ifdef _DEBUG
				if(m_usManagerState==usBaseState)
				{
					BSTR bstrDesc;
					m_pOpenUndoAction->GetDescription(&bstrDesc);
					ASSERT(wcslen(bstrDesc) > 0);
					SysFreeString(bstrDesc);
				}
#endif
				// If we're in the base state, committing a new
				// Undo unit wipes out the Redo stack.
				//
				if (m_usManagerState==usBaseState)
				{
					// Clear out the Redo Stack
					if(m_pOpenRedoAction)
					{
						m_pOpenRedoAction->Release();
						m_pOpenRedoAction=NULL;
					}
					POSITION posRedo = m_RedoStack.GetHeadPosition();
					while(posRedo != NULL)
					{
						m_RedoStack.GetNext(posRedo)->Release();
					}
					m_RedoStack.RemoveAll();
				}

				m_UndoStack.AddHead(m_pOpenUndoAction);
				m_pOpenUndoAction=NULL;
				if(m_usManagerState==usRedoState)
					m_cRollbackCtr++;
				OnStackChange();
				return S_OK;
				break;
			case usUndoState:
				ASSERT(m_pOpenRedoAction);
				hr=m_pOpenRedoAction->Close(pCUA, fCommit);
				if(hr!=S_FALSE)
					return hr;
				if(pCUA!=m_pOpenRedoAction)
					return E_INVALIDARG;
				if(!fCommit)
				{
					m_pOpenRedoAction->Release();
					m_pOpenRedoAction=NULL;
					return S_OK;
				}
				m_RedoStack.AddHead(m_pOpenRedoAction);
				m_pOpenRedoAction=NULL;
				m_cRollbackCtr++;
				OnStackChange();
				return S_OK;
				break;
			default:
				ASSERT(FALSE);
				return E_FAIL;
				break;
			}
		}
		else
			return S_FALSE;

	CATCH_ALL(pe)
	{
		hr = COleException::Process(pe);
		pe->Delete();
	}
	END_CATCH_ALL

	return hr;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::Add

Description:
		Add a simple undo action to the Undo/Redo stack
		AddAtomic can be called with a compound action via Add()

Return Values:	S_OK
				E_UNEXPECTED, The Manager is not enabled
				E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::Add
(
IOleUndoUnit *pUA // Pointer to Undo Action
)
{
	if(!pUA)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	if(!m_fEnable)
		return S_OK;

	//Case I:  There is an open Compound action on the stack
	if(m_pOpenUndoAction||m_pOpenRedoAction)
		{
		switch(m_usManagerState)
			{
			  case usBaseState:
			  case usRedoState:
					ASSERT(m_pOpenUndoAction);
					if(m_usManagerState==usBaseState)
						m_pOpenUndoAction->OnNextAdd();
					return m_pOpenUndoAction->Add(pUA);
					break;
			  case usUndoState:
					ASSERT(m_pOpenRedoAction);
					return m_pOpenRedoAction->Add(pUA);
					break;
			  default:
					ASSERT(FALSE);
					return E_FAIL;
					break;
			}
		}

	HRESULT hr = S_OK;
	POSITION posUndo, posRedo;

	TRY
		//Case II:	There is no open Compound action on the stack
		switch(m_usManagerState)
		{
		case usBaseState:
			// Clear out the Redo Stack
			if(m_pOpenRedoAction)
			{
				m_pOpenRedoAction->Release();
				m_pOpenRedoAction=NULL;
			}
			posRedo = m_RedoStack.GetHeadPosition();
			while(posRedo != NULL)
			{
				m_RedoStack.GetNext(posRedo)->Release();
			}
			m_RedoStack.RemoveAll();
			// Call OnNextAdd on the top of stack
			posUndo = m_UndoStack.GetHeadPosition();
			if(posUndo != NULL)
				m_UndoStack.GetAt(posUndo)->OnNextAdd();
			m_UndoStack.AddHead(pUA);
#ifdef _DEBUG
			BSTR bstrDesc;
			pUA->GetDescription(&bstrDesc);
			ASSERT(wcslen(bstrDesc) > 0);
			SysFreeString(bstrDesc);
#endif
			break;
		case usUndoState:
			m_RedoStack.AddHead(pUA);
			m_cRollbackCtr++;
			break;
		case usRedoState:
			m_UndoStack.AddHead(pUA);
			m_cRollbackCtr++;
			break;
		default:
			ASSERT(FALSE);
			return E_FAIL;
			break;
		}

		// We're keeping the interface pointer pUA so AddRef it!
		pUA->AddRef();
		OnStackChange();

	CATCH_ALL(pe)
	{
		hr = COleException::Process(pe);
		pe->Delete();
	}
	END_CATCH_ALL

	return hr;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::DiscardFrom

Description: Discard all Undo stack entries prior to a given entry

Return Values:	S_OK
				E_INVALIDARG, if Action not found in stack
				E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::DiscardFrom
(
IOleUndoUnit *pUA // Pointer to Undo Action
)
{
	POSITION posUndo, posRedo, posTemp;

	if(pUA==NULL) // if pUA NULL, discard both Undo/Redo Stacks entirely
	{
		// Traverse UndoStack from Head to Tail, releasing each member, then delete stack
		posUndo = m_UndoStack.GetHeadPosition();
		while(posUndo != NULL)
		{
			m_UndoStack.GetNext(posUndo)->Release();
		}
		m_UndoStack.RemoveAll();
		if(m_pOpenUndoAction)
		{
			m_pOpenUndoAction->Release();
			m_pOpenUndoAction=NULL;
		}
		// Traverse RedoStack from Head to Tail, releasing each member, then delete stack
		posRedo = m_RedoStack.GetHeadPosition();
		while(posRedo != NULL)
		{
			m_RedoStack.GetNext(posRedo)->Release();
		}
		m_RedoStack.RemoveAll();
		if(m_pOpenRedoAction)
		{
			m_pOpenRedoAction->Release();
			m_pOpenRedoAction=NULL;
		}
		OnStackChange();
		return S_OK;
	}

	// Caller wishes to discard from a particular item to the end.
	// Traverse UndoStack from Head to Tail, searching for the particular Action
	for(posUndo = m_UndoStack.GetHeadPosition();
		(posUndo != NULL) && m_UndoStack.GetAt(posUndo)!=pUA;
		m_UndoStack.GetNext(posUndo));

	if(!posUndo)
		return E_INVALIDARG; // if Action not found, return E_INVALIDARG

	// Remove this item and everything after it.
	//
	while (posUndo != NULL)
	{
		posTemp = posUndo;
		m_UndoStack.GetNext(posUndo)->Release();
		m_UndoStack.RemoveAt(posTemp);
	}

	OnStackChange();
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::UndoTo

Description: Perform undo actions back thru the Undo Stack up to & including
the specified object

Return Values:
		S_OK,	 OK
		E_UNEXPECTED, if disabled
		E_INVALIDARG,	 The specified object is not in the stack
		E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::UndoTo
(
IOleUndoUnit *pUA // Pointer to Undo Action
)
{
	IOleUndoUnit *pUA1;
	HRESULT	hr;

	ASSERT(m_usManagerState==usBaseState);

	if(!m_fEnable)
		return E_UNEXPECTED;

	if(m_pOpenUndoAction||m_pOpenRedoAction) // Operation not allowed if open child object
		return E_UNEXPECTED;

	POSITION pos;
	if(pUA==NULL)
	{
		// No target object specified; undo the last action on the stack.
		//
		pos = m_UndoStack.GetHeadPosition();
		if(pos != NULL)
			pUA=m_UndoStack.GetAt(pos);
		else
			return E_UNEXPECTED;
	}
	else
	{
		// Make sure that the specified action is in the stack.
		//
		for(pos = m_UndoStack.GetHeadPosition();
			pos != NULL && m_UndoStack.GetAt(pos) != pUA;
			m_UndoStack.GetNext(pos));

		// If not found, return error
		if(pos == NULL)
			return E_INVALIDARG;
	}

	// Put Undo Manager into Undo State
	m_usManagerState=usUndoState;

	// Traverse stack, call Do methods, release Iface ptrs, remove from stack
	// Knowns: Stack is non-empty and element has been found in stack
	do
	{
		m_cRollbackCtr=0;
		pUA1 = m_UndoStack.RemoveHead();
		hr = pUA1->Do(this);
		pUA1->Release();
	}
	while(m_UndoStack.GetHeadPosition() != NULL	&& (pUA1!=pUA) && SUCCEEDED(hr));

	// Put Undo Manager into Base State
	m_usManagerState=usBaseState;

	if(SUCCEEDED(hr))
	{
		OnStackChange();
		return S_OK;
	}
	else
	{
		//This handles the rollback case where an Undo action fails
		//If Undo fails & nothing added to Redo stack, only return error code & map E_ABORT to E_FAIL
		if(m_cRollbackCtr<=0)
		{
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr);
		}
		if(m_pOpenRedoAction) // Operation not allowed if open child object
		{
			ASSERT(FALSE);
			goto Abort;
		}

		POSITION posRedo;
		posRedo = m_RedoStack.GetHeadPosition();
		if(posRedo == NULL)
		{
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr);
		}
		// Put Undo Manager into Redo State
		m_usManagerState=usRedoState;

		//Preserve the error code & rollback the top of the redo stack
		HRESULT hr1=hr;
		pUA1=m_RedoStack.GetAt(posRedo);
		m_RedoStack.RemoveAt(posRedo);
		hr=pUA1->Do(NULL);
		pUA1->Release();

		// Put Undo Manager back into Base State
		m_usManagerState=usBaseState;

		if(FAILED(hr))
			goto Abort;
		else
		{
			// Rollback succeeded!
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr1);
		}
	}

Abort:
	DiscardFrom(NULL);
	OnStackChange();
	return E_ABORT;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::RedoTo

Description: Perform redo actions back thru the Redo Stack up to & including
the specified object

Return Values:
		S_OK,	 OK
		E_UNEXPECTED, if disabled
		E_INVALIDARG,	 The specified object is not in the stack
		E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::RedoTo
(
IOleUndoUnit *pUA // Pointer to Undo Action
)
{
	IOleUndoUnit *pUA1;
	HRESULT	hr;

	ASSERT(m_usManagerState==usBaseState);

	if(!m_fEnable)
		return E_UNEXPECTED;

	if(m_pOpenUndoAction||m_pOpenRedoAction) // Operation not allowed if open child object
		return E_UNEXPECTED;

	POSITION pos;
	if(pUA==NULL)
	{
		pos = m_RedoStack.GetHeadPosition();
		if(pos != NULL)
			pUA=m_RedoStack.GetAt(pos);
		else
			return E_UNEXPECTED;
	}
	else
	{
		// Search for input Action in Redo stack
		for(pos = m_RedoStack.GetHeadPosition();
			pos != NULL && m_RedoStack.GetAt(pos) != pUA;
			m_RedoStack.GetNext(pos)
		);
		// If not found, return error
		if(pos == NULL)
			return E_INVALIDARG;
	}
	// Put Undo Manager into Redo State
	m_usManagerState=usRedoState;

	// Traverse stack, call Do methods, release Iface ptrs, remove from stack
	// Knowns: Stack is non-empty and element has been found in stack
	//
	pos = m_RedoStack.GetHeadPosition();
	do
	{
		// Zero Rollback counter to count actions added to Redo stack
		m_cRollbackCtr=0;
		pUA1 = m_RedoStack.GetAt(pos);
		m_RedoStack.RemoveAt(pos);
		hr=pUA1->Do(this);
		pUA1->Release();
		pos = m_RedoStack.GetHeadPosition();
	}
	while(pos != NULL && (pUA1!=pUA)&&SUCCEEDED(hr));

	// Put Undo Manager into Base State
	m_usManagerState=usBaseState;

	if(SUCCEEDED(hr))
	{
		OnStackChange();
		return S_OK;
	}
	else
	{
		//This handles the rollback case where a Redo action fails
		//If Redo fails & nothing added to Undo stack, only return error code & map E_ABORT to E_FAIL
		if(m_cRollbackCtr<=0)
		{
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr);
		}
		if(m_pOpenUndoAction) // Operation not allowed if open child object
		{
			ASSERT(FALSE);
			goto Abort;
		}

		POSITION posUndo;
		posUndo = m_UndoStack.GetHeadPosition();
		if(posUndo == NULL)
		{
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr);
		}
		// Put Undo Manager into Undo State
		m_usManagerState=usUndoState;

		//Preserve the error code & rollback the top of the undo stack
		HRESULT hr1=hr;
		pUA1 = m_UndoStack.GetAt(posUndo);
		m_UndoStack.RemoveAt(posUndo);
		hr=pUA1->Do(NULL);
		pUA1->Release();

		// Put Undo Manager back into Base State
		m_usManagerState=usBaseState;

		if(FAILED(hr))
			goto Abort;
		else
		{
			// Rollback succeeded!
			DiscardFrom(NULL);
			OnStackChange();
			return MapAbortResult(hr1);
		}
	}

Abort:
	DiscardFrom(NULL);
	OnStackChange();
	return E_ABORT;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::EnumUndoable

Description: Make an enumeration list of the top-level undoable actions

Return Values: S_OK
			   E_UNEXPECTED, if no open child object
			   E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::EnumUndoable
(
IEnumOleUndoUnits **ppEnum // Pointer to Enumeration Interface to return
)
{
	if(!ppEnum)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*ppEnum=NULL;
	if(m_pOpenUndoAction||m_pOpenRedoAction) // Operation not allowed if open child object
		return E_UNEXPECTED;

	*ppEnum=(IEnumOleUndoUnits *) new CEnumOleUndoUnit(this, &m_UndoStack);
	if (*ppEnum != NULL)
	{
		// Note that CEnumOleUndoUnit's constructor sets the reference
		// count to 1.
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::EnumRedoable

Description: Make an enumeration list of the top-level redoable actions

Return Values: S_OK
			   E_UNEXPECTED, if no open child object
			   E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::EnumRedoable
(
IEnumOleUndoUnits **ppEnum // Pointer to Enumeration Interface to return
)
{
	if(!ppEnum)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	*ppEnum=NULL;
	if(m_pOpenUndoAction||m_pOpenRedoAction) // Operation not allowed if open child object
		return E_UNEXPECTED;

	*ppEnum=(IEnumOleUndoUnits *) new CEnumOleUndoUnit(this, &m_RedoStack);
	if (*ppEnum != NULL)
	{
		// Note that CEnumOleUndoUnit's constructor sets the reference
		// count to 1.
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::GetLastUndoDescription

Description: Get description of top-level Action object on top of Undo Stack

Return Values:
		S_OK, if, OK
		E_FAIL, Error:	Stack is Empty
		E_UNEXPECTED,	Error:	open child action
		pstr points to the description string
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::GetLastUndoDescription
(
BSTR *pbstr  // Pointer to string buffer to return description
)
{
	if(!pbstr)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	*pbstr=NULL;
	if(m_pOpenUndoAction||m_pOpenRedoAction||!m_fEnable) // Operation not allowed if open child object
		return E_UNEXPECTED;

	POSITION pos;
	pos = m_UndoStack.GetHeadPosition();
	if(pos == NULL)
		return E_FAIL;
	else
		return m_UndoStack.GetAt(pos)->GetDescription(pbstr);
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::GetLastRedoDescription

Description: Get description of top-level Action object on top of Redo Stack

Return Values:
		S_OK, if, OK
		E_FAIL, Error:	Stack is Empty
		E_UNEXPECTED,	Error:	open child action
		pstr points to the description string
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::GetLastRedoDescription
(
BSTR *pbstr  // Pointer to string buffer to return description
)
{
	if(!pbstr)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	*pbstr=NULL;
	if(m_pOpenUndoAction||m_pOpenRedoAction||!m_fEnable) // Operation not allowed if open child object
		return E_UNEXPECTED;

	POSITION pos;
	pos = m_RedoStack.GetHeadPosition();
	if(pos == NULL)
		return E_FAIL;
	else
		return m_RedoStack.GetAt(pos)->GetDescription(pbstr);
}


/*-----------------------------------------------------------------------------
Name: COleUndoManager::Enable

Description: Enables/Disables the Undo/Redo Manager

Return Values:	S_OK

-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::Enable
(
BOOL fEnable // Enable or Disable the UndoManager
)
{
	//Disallow enable processing during UndoTo/RedoTo operation
	if(m_usManagerState!=usBaseState)
		return E_UNEXPECTED;
	//Disallow enable processing if any open child object
	if(m_pOpenUndoAction||m_pOpenRedoAction)
		return E_UNEXPECTED;
	m_fEnable=fEnable;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::GetOpenParentState

Description: Get State of Open Compound Undo Action

Return Values:	S_OK, if there is an open action
				S_FALSE, if no open action

-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoManager::GetOpenParentState
(
DWORD *pdwState // Pointer to State Flags to return
)
{
	*pdwState=NULL;

	switch(m_usManagerState)
	{
		case usBaseState:
		case usRedoState:
			if(m_pOpenUndoAction)
			{
				m_pOpenUndoAction->GetParentState(pdwState);
				return S_OK;
			}
			else
				return S_FALSE;

		  case usUndoState:
			if(m_pOpenRedoAction)
			{
				m_pOpenRedoAction->GetParentState(pdwState);
				return S_OK;
			}
			else
				return S_FALSE;

		  default:
				ASSERT(FALSE);
				return E_FAIL;
	}
}

/*-----------------------------------------------------------------------------
Name: COleUndoManager::OnStackChange

Description: Processes notification that stack has changed

Return Values:	none

-----------------------------------------------------------------------------*/
void COleUndoManager::OnStackChange
(
)
{
	int cUndoStackCount=m_UndoStack.GetCount(),
		cRedoStackCount=m_RedoStack.GetCount();

	// If either stack changes from empty to non-empty or vice-versa, dirty
	// The menus to make the undo/redo dropdown controls enable/disable themselves
	if((m_fUndoStackIsEmpty&&(cUndoStackCount>0)) ||
	   (!m_fUndoStackIsEmpty&&(cUndoStackCount==0)) ||
	   (m_fRedoStackIsEmpty&&(cRedoStackCount>0)) ||
	   (!m_fRedoStackIsEmpty&&(cRedoStackCount==0)))
	{
		// Dirty the menus
		theApp.m_bMenuDirty = TRUE;
		m_fUndoStackIsEmpty=(cUndoStackCount==0);
		m_fRedoStackIsEmpty=(cRedoStackCount==0);
	}
}


///////////////////////////////////////////////////////////////////////////////
// CEnumOleUndoUnit
/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::CEnumOleUndoUnit

Description: Constructor for CEnumOleUndoUnit Object

Return Values:  None

-----------------------------------------------------------------------------*/
CEnumOleUndoUnit::CEnumOleUndoUnit
(
IOleUndoManager * pUndoMgr,
CTypedPtrList<CPtrList, IOleUndoUnit *>  *pActionList // Undo/Redo stack to enumerate
) :
m_cRef(1),
m_cPos(0),
m_pActionList(pActionList)
{
	m_srpUndoMgr = pUndoMgr;
	if (m_srpUndoMgr)
		m_srpUndoMgr->AddRef();
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::CEnumOleUndoUnit

Description: Overloaded Constructor for CEnumOleUndoUnit Object

Return Values:  None

-----------------------------------------------------------------------------*/
CEnumOleUndoUnit::CEnumOleUndoUnit
(
IOleUndoManager * pUndoMgr,
CTypedPtrList<CPtrList, IOleUndoUnit *>  *pActionList, // Undo/Redo stack to enumerate
ULONG cPos //  Position of current Action in enumeration object
) :
m_cRef(1),
m_cPos(cPos),
m_pActionList(pActionList)
{
	m_srpUndoMgr = pUndoMgr;
	if (m_srpUndoMgr)
		m_srpUndoMgr->AddRef();
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::QueryInterface

Description:
    Query the object for an OLE interface.

Return Values:	 
    NO_ERROR to indicate the interface is supported, in which case
    *ppvObj is filled in; otherwise, *ppvObj is set to NULL.
-----------------------------------------------------------------------------*/
STDMETHODIMP CEnumOleUndoUnit::QueryInterface
(
REFIID riid,        // requested interface
void ** ppvObj  // receives pointer to interface
)
{
	if (ppvObj == NULL)
		return E_INVALIDARG;
		
	*ppvObj = NULL;

	if (riid==IID_IUnknown)
		*ppvObj = (void *)(IUnknown *)this;
	else if (riid==IID_IEnumOleUndoUnits)
		*ppvObj = (void *)(IEnumOleUndoUnits *)this;
	else
		return E_NOINTERFACE;

	((IUnknown *)*ppvObj)->AddRef();
	return NOERROR;
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::AddRef

Description:
    Add a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) CEnumOleUndoUnit::AddRef()
{
	return( ++m_cRef );
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::Release

Description:
    Release a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) CEnumOleUndoUnit::Release()
{
	ASSERT( m_cRef > 0 );
	int cRef = --m_cRef;
	if (cRef==0)
		delete this;
	return cRef;
}


/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::Next

Description: Returns next item in Action enumeration list

Return Values:
        S_OK       OK, the number of elements returned is celt
		S_FALSE    The number of elements returned is less than celt
		E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP CEnumOleUndoUnit::Next
(
ULONG celt, // Number of Undo/Redo Actions requested from enumeration object
IOleUndoUnit **rgelt, // Array to put Undo Actions that were fetched
ULONG *pceltFetched // Actual number of Actions returned from enumeration object
)
{
	DWORD i;
	ULONG cFetched;

	// NULL output parameters.	Note that the specification for IEnumX
	// states that unused slots in the rgelt array are not set to NULL.
	//
	if (pceltFetched != NULL)
	{
		*pceltFetched = 0;
	}

	if (rgelt == NULL || (celt > 1 && pceltFetched == NULL))
	{
		return E_INVALIDARG;
	}

	POSITION pos = m_pActionList->GetHeadPosition();
	for (i = 0; pos != NULL && i < m_cPos; i++)
	{
		m_pActionList->GetNext(pos);
	}

	for (cFetched = 0; pos != NULL && cFetched < celt; cFetched++)
	{
		rgelt[cFetched] = m_pActionList->GetNext(pos);
		rgelt[cFetched]->AddRef();
	}

	m_cPos += cFetched;

	if (pceltFetched != NULL)
	{
		*pceltFetched = cFetched;
	}

	// If we fetched as many as requested, return S_OK; if
	// we fetched fewer than requested, return S_FALSE.  (Note
	// that if we fetched zero items, possibly because we're
	// at the end of the list, we return S_FALSE.)
	//
	return (cFetched == celt) ? S_OK : S_FALSE;
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::Skip

Description: Returns Skip item in Action enumeration list

Return Values:
        S_OK       OK, the number of elements returned is celt
		S_FALSE    The number of elements returned is less than celt

-----------------------------------------------------------------------------*/
STDMETHODIMP CEnumOleUndoUnit::Skip
(
ULONG celt //  Number of Undo/Redo Actions requested from enumeration object
)
{
	if(((unsigned)m_pActionList->GetCount())<=m_cPos+celt)
		return E_INVALIDARG;
	m_cPos+=celt;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::Reset

Description: Returns Reset item in Action enumeration list

Return Values:  S_OK

-----------------------------------------------------------------------------*/
STDMETHODIMP CEnumOleUndoUnit::Reset()
{
	m_cPos=0;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: CEnumOleUndoUnit::Clone

Description: Returns Clone item in Action enumeration list

Return Values:
        S_OK       OK, the number of elements returned is celt
		S_FALSE    The number of elements returned is less than celt

-----------------------------------------------------------------------------*/
STDMETHODIMP CEnumOleUndoUnit::Clone
(
IEnumOleUndoUnits** ppEnum // Pointer to cloned Enumeration Interface to return
)
{
	if(!ppEnum)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	*ppEnum=NULL;
	
	*ppEnum=(IEnumOleUndoUnits *) new CEnumOleUndoUnit(m_srpUndoMgr, m_pActionList, m_cPos);
	if (*ppEnum != NULL)
	{
		// Note that CEnumOleUndoUnit's constructor sets the reference
		// count to 1.
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Class COleUndoUnit
///////////////////////////////////////////////////////////////////////////////
// This Class is Sample Code for Undo Implementators and is used for testing

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::COleUndoUnit

Description: Constructor for COleUndoUnit Object

Return Values:  None

-----------------------------------------------------------------------------*/
COleUndoUnit::COleUndoUnit
(
LPOLESTR strDescription, // String to initialize the UndoAction description
BOOL fUndoFail, // Flag to force Undo action to fail
BOOL fRedoFail // Flag to force Redo action to fail
) :
m_cRef(0),
m_atActionType(atUndoAction),
m_fUndoFail(fUndoFail),
m_fRedoFail(fRedoFail)
{
	USES_CONVERSION;
	m_strDesc = OLE2T(strDescription);
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::Do

Description:  Perform this Undo Action and put on the other Undo/Redo Stack

Return Values:  S_OK

-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoUnit::Do
(
IOleUndoManager *pUndoActionManager // Pointer to containing Undo Manager
)
{
	BOOL fFail=FALSE;

	if(m_atActionType==atUndoAction)
	{
		m_atActionType=atRedoAction;
		fFail=m_fUndoFail;
	}
	else
	{
		m_atActionType=atUndoAction;
		fFail=m_fRedoFail;
	}

	if(fFail)
		return E_FAIL;
	if(pUndoActionManager) // if NULL, pretend to do it, i.e. only return S_OK
		pUndoActionManager->Add((IOleUndoUnit *)this);
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::GetDescription

Description:  Get description of the UndoAction Object

Return Values:  S_OK
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoUnit::GetDescription
(
BSTR *pbstr  // Pointer to string buffer to return description
)
{
	USES_CONVERSION;

	*pbstr=::SysAllocString(T2OLE(m_strDesc));
	if(*pbstr==NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::GetUnitType

Description: Returns a Class ID and Type ID for this object

Return Values:  S_OK
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoUnit::GetUnitType
(
CLSID *pclsid,
LONG *pnID
)
{
	//$COMMENT Implementers should put in their own GUIDs here
	*pclsid=CLSID_NULL;
	*pnID=0;
    return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::OnNextAdd

Description:  Sends notification to its creating object to not continue
              inserting data through private interfaces to it

Return Values:	None
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoUnit::OnNextAdd()
{
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::QueryInterface

Description:
    Query the object for an OLE interface.

Return Values:	 
    NOERROR, to indicate the interface is supported, in which case
        *ppvObj is filled in; otherwise, *ppvObj is set to NULL.
    E_NOINTERFACE to indicate the interface is not supported
-----------------------------------------------------------------------------*/
STDMETHODIMP COleUndoUnit::QueryInterface
(
REFIID riid,        // requested interface
void ** ppvObj  // receives pointer to interface
)
{
	if (ppvObj == NULL)
		return E_INVALIDARG;
		
	*ppvObj = NULL;

	if (riid==IID_IUnknown)
        *ppvObj = (void *)(IUnknown *)(IUnknown *)this;
    else if (riid==IID_IOleUndoUnit)
        *ppvObj = (void *)(IOleUndoUnit *)this;
	else
        return E_NOINTERFACE;

    ((IUnknown *)*ppvObj)->AddRef();
    return NOERROR;
}


/*-----------------------------------------------------------------------------
Name: COleUndoUnit::AddRef

Description:
    Add a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleUndoUnit::AddRef()
{
	return( ++m_cRef );
}

/*-----------------------------------------------------------------------------
Name: COleUndoUnit::Release

Description:
    Release a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleUndoUnit::Release()
{
	ASSERT( m_cRef > 0 );
	int cRef = --m_cRef;
	if (cRef==0)
		delete this;
	return cRef;
}

///////////////////////////////////////////////////////////////////////////////
// Class COleParentUndoUnit
///////////////////////////////////////////////////////////////////////////////
// This Class is Sample Code for Undo Implementators and is used for testing

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::COleParentUndoUnit

Description: Constructor for COleParentUndoUnit Object

Return Values:  None

-----------------------------------------------------------------------------*/
COleParentUndoUnit::COleParentUndoUnit
(
UNDOACTIONTYPE atActionType, // Type of compound action (Undo or Redo)
LPOLESTR strDescription // String to initialize the UndoAction description
) :
m_cRef(0),
m_fActionBlocked(FALSE),
m_atActionType(atActionType),
m_pOpenAction(NULL)
{
	USES_CONVERSION;
	m_strDesc = OLE2T(strDescription);
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::Open

Description: Open a new empty compound action to the top of this compound action

Return Values:
    S_OK, OK
	E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::Open
(
IOleParentUndoUnit *pCUA // Pointer to Compound Undo Action
)
{
	if(!pCUA)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	if(m_fActionBlocked) // if blocked, return S_OK
		return S_OK;

	if(m_pOpenAction)
		return m_pOpenAction->Open(pCUA);
	else
	{
		m_pOpenAction=pCUA;
		m_pOpenAction->AddRef();
		return S_OK;
	}
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::Close

Description: Close the currently open compound undo action list

Return Values: S_OK, if ok
			   S_FALSE, if no open child
			   E_INVALIDARG, if pCUA is not the last currently open object
			   E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::Close
(
IOleParentUndoUnit *pCUA, // Pointer to Compound Undo Action
BOOL fCommit // Flag to keep or discard the completed compound undo action list
)
{
	HRESULT hr;

	if(m_fActionBlocked)
	{
		if(pCUA!=(IOleParentUndoUnit *)this)
			return S_OK;
		else
		{
			ASSERT(m_pOpenAction==NULL);
			m_fActionBlocked=FALSE;
			return FALSE;
		}
	}	
	else if(m_pOpenAction)
	{
		hr=m_pOpenAction->Close(pCUA, fCommit);
		if(hr!=S_FALSE)
			return hr;
		if(pCUA!=m_pOpenAction)
			return E_INVALIDARG;
		if(!fCommit)
		{
			m_pOpenAction->Release();
			m_pOpenAction=NULL;
			return S_OK;
		}
		m_culCompoundActionList.AddHead(m_pOpenAction);
		m_pOpenAction=NULL;
		return S_OK;
	}
	else
	{
		m_fActionBlocked=FALSE;
		return S_FALSE;
	}
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::Add

Description: Add a simple undo action to this compound action

Return Values:  S_OK, if ok
                E_FAIL, Thrown Error

-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::Add
(
IOleUndoUnit *pUA // Pointer to Undo Action
)
{
	if(!pUA)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}
	if(m_fActionBlocked) // if blocked, return S_OK
		return S_OK;

	if(m_pOpenAction)
		return m_pOpenAction->Add(pUA);
	m_culCompoundActionList.AddHead(pUA);
	    // We're keeping the interface pointer pUA so AddRef it!
    pUA->AddRef();
    return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::FindUnit

Description: Not Implemented yet!

Return Values:

-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::FindUnit
(
IOleUndoUnit *pUA // Pointer to Undo Action to find
)
{
	IOleUndoUnit *pUA1;
	void * pvDummy;

	if(pUA==(IOleUndoUnit *)this)
		return S_OK;

	for(POSITION Pos = m_culCompoundActionList.GetHeadPosition();
		NULL != Pos;   )
	{		
		pUA1 = m_culCompoundActionList.GetNext(Pos);
		if(pUA1 == pUA)
			return S_OK;
		if((pUA1->QueryInterface(IID_IOleParentUndoUnit, &pvDummy)!=
			E_NOINTERFACE) &&
		   ((IOleParentUndoUnit *)pUA1)->FindUnit(pUA)==S_OK)
			return S_OK;
	}
    return S_FALSE;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::GetParentState

Description: Get State of Open Compound Undo Action

Return Values:

-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::GetParentState
(
DWORD *pdwState // Pointer to State Flags to return
)
{
	if(m_fActionBlocked)
		*pdwState=UAS_BLOCKED;
	else if(m_pOpenAction)
		m_pOpenAction->GetParentState(pdwState);
	else
		*pdwState=UAS_NORMAL;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::Do

Description:  Perform this Undo Action and put on the other Undo/Redo Stack

Return Values:  S_OK, if OK
                E_FAIL, Thrown Error
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::Do
(
IOleUndoManager *pUndoActionManager // Pointer to containing Undo Manager
)
{
	USES_CONVERSION;

	COleParentUndoUnit *cuaCAction;
	IOleUndoUnit *pUA;
	HRESULT hr = NOERROR;

	ASSERT(m_pOpenAction==NULL);
	ASSERT(!m_fActionBlocked);

	if(m_atActionType==atUndoAction)
	{
		// For debugging only write a debug string out to the debug window
		m_atActionType=atRedoAction;
	}
	else
	{
		// For debugging only write a debug string out to the debug window
		m_atActionType=atUndoAction;
	}

	if(pUndoActionManager)
	{
		cuaCAction=new COleParentUndoUnit(m_atActionType, T2OLE(m_strDesc));
		hr=pUndoActionManager->Open(cuaCAction);
		ASSERT(SUCCEEDED(hr));
	}


	POSITION Pos;
	// Traverse stack, call Do methods, release Iface ptrs, remove from stack
	int cStateChange=0;
	while(!m_culCompoundActionList.IsEmpty() && SUCCEEDED(hr))
	{
		pUA = m_culCompoundActionList.RemoveHead();
		ASSERT(NULL != pUA);
		hr = pUA->Do(pUndoActionManager);
		//Note:  this also applies to RedoTo below!
		if(SUCCEEDED(hr))
			cStateChange++;
		pUA->Release();
	}

	if(!pUndoActionManager)
	{
		if(SUCCEEDED(hr))
			return S_OK;
		else
			return hr;
	}

	if(SUCCEEDED(hr))
	{
		pUndoActionManager->Close(cuaCAction, TRUE);
		return S_OK;
	}
	else if(cStateChange>0)
	{
		pUndoActionManager->Close(cuaCAction, TRUE);
		return hr;
	}
	else
	{
		pUndoActionManager->Close(cuaCAction, FALSE);
		return hr;
	}
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::GetDescription

Description: Get description of the UndoAction Object

Return Values:  S_OK
                pstr points to the description string
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::GetDescription
(
BSTR *pbstr  // Pointer to string buffer to return description
)
{
	USES_CONVERSION;

	*pbstr=::SysAllocString(T2OLE(m_strDesc));
	if(*pbstr==NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::GetUnitType

Description: Returns a Class ID and Type ID for this object

Return Values:  S_OK
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::GetUnitType
(
CLSID *pclsid,
LONG *pnID
)
{
	//$COMMENT Implementers should put in their own GUIDs here
	*pclsid=CLSID_NULL;
	*pnID=0;
    return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::OnNextAdd

Description:  Sends notification to its creating object to not continue
              inserting data through private interfaces to it

Return Values:	None
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::OnNextAdd()
{
	//$CONSIDER I don't think the spec is clear on what this should do for blocked Actions!
	if(m_pOpenAction)
		m_pOpenAction->OnNextAdd();
	return S_OK;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::QueryInterface

Description:
    Query the object for an OLE interface.

Return Values:	 
    NO_ERROR to indicate the interface is supported, in which case
    *ppvObj is filled in; otherwise, *ppvObj is set to NULL.
-----------------------------------------------------------------------------*/
STDMETHODIMP COleParentUndoUnit::QueryInterface
(
REFIID riid,        // requested interface
void ** ppvObj  // receives pointer to interface
)
{
	if (ppvObj == NULL)
		return E_INVALIDARG;
		
	*ppvObj = NULL;

	if (riid==IID_IUnknown)
		//$CONSIDER Should I give out the IUnknown of the CCompDocHost here instead?
        *ppvObj = (void *)(IUnknown *)(IUnknown *)this;
    else if (riid==IID_IOleParentUndoUnit)
        *ppvObj = (void *)(IOleParentUndoUnit *)this;
	else
        return E_NOINTERFACE ;

    ((IUnknown *)*ppvObj)->AddRef();
    return NOERROR;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::AddRef

Description:
    Add a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleParentUndoUnit::AddRef()
{
	return( ++m_cRef );
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::Release

Description:
    Release a reference to this object.

Return Values:
    The number of outstanding references.
-----------------------------------------------------------------------------*/
STDMETHODIMP_(ULONG) COleParentUndoUnit::Release()
{
	ASSERT( m_cRef > 0 );
    int cRef = --m_cRef;
    if (cRef==0)
	{
		// Traverse Compound Stack from Head to Tail, releasing each member, then delete stack
		POSITION Pos = m_culCompoundActionList.GetHeadPosition();
		IOleUndoUnit *pUA;
		while(NULL != Pos)
		{
			pUA = m_culCompoundActionList.GetNext(Pos);
			pUA->Release();
		}
		m_culCompoundActionList.RemoveAll();
		delete this;
	}
	return cRef;
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::IsEmpty

Description:
    Determine if the undo action list is empty

Return Values:
    TRUE == empty, FALSE == not empty
-----------------------------------------------------------------------------*/
BOOL COleParentUndoUnit::IsUndoActionListEmpty()
{ 
	return ((m_culCompoundActionList.GetCount() == 0) ? TRUE : FALSE);
}

/*-----------------------------------------------------------------------------
Name: COleParentUndoUnit::IsEmpty

Description:
    Allow the undo description to be modified

Return Values:
-----------------------------------------------------------------------------*/
void COleParentUndoUnit::SetUndoDesc(LPCTSTR szDesc)
{ 
	ASSERT(NULL != szDesc); 
	m_strDesc = szDesc; 
}

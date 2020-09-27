/*
AutoFrm.cpp -

This file contains the default implementations of the OLE Automation Windows
Objects for CPartFrame. See shldocs.h for the declarations. Each CPartFrame 
derived class has its own CAutoObj automation object. These automation objects
implement the necessary code. Most of these automation objects can simply call
these functions in CPartFrm. Those that need different implementations of these
functions should NOT override these. These functions are NOT virtual. Instead 
change the implementation in your CAutoObj derived class.

12 Apr 96 Created Dale
*/
#include "stdafx.h"

#include "utilauto.h"
#include "autostat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//
//
//
BSTR CPartFrame::GetCaption()
{
	CString strResult;
	GetWindowText(strResult) ;
	return strResult.AllocSysString();
}

/* Do not implement
void CPartFrame::SetCaption(LPCTSTR lpszNewValue)
{
	ASSERT(lpszNewValue == NULL) ;
	if (lpszNewValue != NULL)
	{
		SetWindowText(lpszNewValue) ;
	}
}
*/

//
//
//

long CPartFrame::GetLeft()
{
	// Get client rectangle
	CRect rect;
	GetWindowRect(&rect);

	// Convert to client co-ordinate relative to frame.
	CWnd* pMdiClient = GetParent();
	pMdiClient->ScreenToClient(&rect) ;

	return rect.left;
}

void CPartFrame::SetLeft(long nNewValue)
{
	// TODO: Docking

	// Get parent frame to check for valid values.
	CWnd* pMdiClient = GetParent();
	CRect rectMdiClient;
	pMdiClient->GetClientRect(&rectMdiClient) ; //DER - What's the real space I have...

	// Check for valid values.
	if ((nNewValue >= 0)  && 
		(nNewValue < rectMdiClient.Width()))
	{
		// Get the existing dimensions of the window.
		CRect rect ;
		GetWindowRect(&rect) ;		

		// Convert to client co-ordinates.
		pMdiClient->ScreenToClient(&rect) ;

		// Move
		MoveWindow(nNewValue, rect.top, 
					rect.Width(), rect.Height(), TRUE);
	}

}

long CPartFrame::GetTop()
{
	// Get client rectangle
	CRect rect;
	GetWindowRect(&rect);

	// Convert to client co-ordinate relative to frame.
	CWnd* pMdiClient = GetParent();
	pMdiClient->ScreenToClient(&rect) ;

	return rect.top;
}

void CPartFrame::SetTop(long nNewValue)
{
	// TODO: Docking

	// Get parent frame to check for valid values.
	CWnd* pMdiClient = GetParent();
	CRect rectMdiClient;
	pMdiClient->GetClientRect(&rectMdiClient) ; //DER - What's the real space I have...

	// Check for valid values.
	if ((nNewValue >= 0)  && 
		(nNewValue < rectMdiClient.Height()))
	{
		// Get the existing dimensions of the window.
		CRect rect ;
		GetWindowRect(&rect) ;		

		// Convert to client co-ordinates.
		pMdiClient->ScreenToClient(&rect) ;

		// Move
		MoveWindow(rect.left, nNewValue, 
					rect.Width(), rect.Height(), TRUE);
	}
}

long CPartFrame::GetHeight()
{
	// Get client rectangle
	CRect rect;
	GetWindowRect(&rect);

	// Convert to client co-ordinate relative to frame.
	CWnd* pMdiClient = GetParent();
	pMdiClient->ScreenToClient(&rect) ;

	return rect.Height();
}

void CPartFrame::SetHeight(long nNewValue)
{
	// TODO: Docking

	// Get parent frame to check for valid values.
	//
	// NOTE - Use GetParent and not GetParentFrame.
	// We want to get the MDICLIENT window.
	//
	CWnd* pMdiClient = GetParent();
	CRect rectMdiClient;
	pMdiClient->GetClientRect(&rectMdiClient) ; //DER - What's the real space I have...	

	// Check for valid values.
	if ((nNewValue >= 0)  && 
		(nNewValue < rectMdiClient.Height())) 
	{
		// Get the existing dimensions of the window.
		CRect rect ;
		GetWindowRect(&rect) ;		

		// Convert to client co-ordinates.
		pMdiClient->ScreenToClient(&rect) ; 

		// Move
		MoveWindow(rect.left, rect.top, 
					rect.Width(), nNewValue, TRUE);
	}
}

long CPartFrame::GetWidth()
{
	// Get rectangle for window.
	CRect rect;
	GetWindowRect(&rect);

	// Convert to client co-ordinate relative to frame.
	CWnd* pMdiClient = GetParent();
	pMdiClient->ScreenToClient(&rect) ;

	return rect.Width();
}

void CPartFrame::SetWidth(long nNewValue)
{
	// TODO: Docking

	// Get parent frame to check for valid values.
	CWnd* pMdiClient = GetParent();
	CRect rectMdiClient;
	pMdiClient->GetClientRect(&rectMdiClient) ; //DER - What's the real space I have...

	// Check for valid values.
	if ((nNewValue >= 0)  && 
		(nNewValue < rectMdiClient.Width()))
	{
		// Get the existing dimensions of the window.
		CRect rect ;
		GetWindowRect(&rect) ;		

		// Convert to client co-ordinates.
		pMdiClient->ScreenToClient(&rect) ;

		// Move
		MoveWindow(rect.left, rect.top, 
					nNewValue, rect.Height(), TRUE);
	}
}

long CPartFrame::GetIndex()
{
	const long cBadCount = -1;
	long count = cBadCount;

	IDispatch* pDispatch = NULL;
	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
	ASSERT_KINDOF(CMainFrame, pMainFrame) ;

#ifdef _DEBUG
	BOOL bFound = FALSE ;
#endif
	CPartFrame* pMDIChild = (CPartFrame*)GetWindow(GW_HWNDFIRST);
	ASSERT_KINDOF(CPartFrame, pMDIChild) ;
	while(pMDIChild != NULL)
	{
		if(!IsValidMDICycleMember(pMDIChild))
			continue;

		// Make sure that it is automatable
		pDispatch = pMDIChild->GetAutomationObject(TRUE); //ToDo...Fix the False case
		if (pDispatch == NULL)
		{
			continue ;
		}
		else
		{
			pDispatch->Release() ;
		}

		// We have a window. Increment the count.
		count++ ;
		if(pMDIChild == this) // we completed the loop
		{
			// We have found our current window, so quit.
#ifdef _DEBUG
			bFound = TRUE ;
#endif
			break;
		}

		// Find next window.
		pMDIChild = (CPartFrame*)pMDIChild->GetWindow(GW_HWNDNEXT);
		ASSERT_KINDOF(CPartFrame, pMDIChild) ;
	}
	ASSERT(bFound = TRUE) ;
	ASSERT(count != cBadCount) ;

	return count;
}

static LPDISPATCH NextWin(CPartFrame* pMDIChildStart, BOOL goUp)
{
	IDispatch* pDispatch = NULL;
	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	DWORD dwNextWin = goUp ? GW_HWNDPREV : GW_HWNDNEXT;
	DWORD dwFirstWin = goUp ? GW_HWNDLAST : GW_HWNDFIRST;
	CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
	CPartFrame* pMDIChild = pMDIChildStart;
	while( TRUE )
	{
		pMDIChild = (CPartFrame *)pMDIChild->GetWindow(dwNextWin);
		if(pMDIChild == NULL) // we got to the end of the list
			pMDIChild = (CPartFrame *)pMDIActiveChild->GetWindow(dwFirstWin);

		if(pMDIChild == pMDIActiveChild) // we completed the loop
			break;

		if(!IsValidMDICycleMember(pMDIChild))
			continue;
				// Get the automation object for this document.
		pDispatch = pMDIChild->GetAutomationObject(TRUE);
		if (pDispatch != NULL)
			break;
	}
	if(pDispatch == NULL)
		pDispatch = pMDIActiveChild->GetAutomationObject(TRUE) ;

	return pDispatch;
}

LPDISPATCH CPartFrame::GetNext()
{
	return NextWin(this, FALSE);
}

LPDISPATCH CPartFrame::GetPrevious()
{
	return NextWin(this, TRUE);
}

DsWindowState CPartFrame::GetWindowState() 
{
	// Main window can't docking or float.
	DsWindowState state = dsWindowStateNormal ;

	if (IsIconic())
	{
		state = dsWindowStateMinimized ;
	}
	else if (IsZoomed())
	{
		state = dsWindowStateMaximized ;
	}
	return state;
}


void CPartFrame::SetWindowState(DsWindowState nNewValue) 
{
	int nCmdShow ;

	switch(nNewValue)
	{
	case dsWindowStateMaximized :
	    nCmdShow = SW_SHOWMAXIMIZED ;
		break;
	case dsWindowStateMinimized :
		nCmdShow = SW_MINIMIZE ;
		break;
	case dsWindowStateNormal:
		nCmdShow = SW_SHOWNORMAL;
		break;
	default:
		// Main window cannot be docked or floated.
		// TODO: ERROR
		return ;
	}
	ShowWindow(nCmdShow) ;
}

DsSaveStatus CPartFrame::AutomationClose(const VARIANT FAR& vtLongSaveChanges)
{
	g_AutomationState.SaveChanges(vtLongSaveChanges);
	
	// Attempt to close the frame
	CloseFrame();

	// Return if the save was canceled. (HandleError resets status.)
	DsSaveStatus status = g_AutomationState.SavePromptResult() ;		

	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;

	// Return if the save was canceled.
	return status ;		
};

BOOL CPartFrame::GetActive()
{
	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
	ASSERT(pMDIActiveChild != NULL) ;
	return (pMDIActiveChild == this) ;
}

void CPartFrame::SetActive(BOOL bActive)
{
	BOOL bCurrentlyActive = GetActive() ;

	if (bActive == bCurrentlyActive) 
	{
		// Optimization. Current state is already
		// correct so don't do any work.
		return ;
	}

	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	if (bActive)	// Make this window active.
	{
		pMainFrame->MDIActivate(this) ;		
	}
	else			// Make this window in-active.
	{
		// We will activate the next window.
		CPartFrame* pMDIChild = (CPartFrame*)GetWindow(GW_HWNDNEXT);
		ASSERT_KINDOF(CPartFrame, pMDIChild) ;
		if(pMDIChild == NULL) // we got to the end of the list
		{
			// Get the first window.
			pMDIChild = (CPartFrame*)GetWindow(GW_HWNDFIRST);
			ASSERT_KINDOF(CPartFrame, pMDIChild) ;
		}

		// Only activate if it is different than the current window.
		if (pMDIChild != this)
		{
			pMainFrame->MDIActivate(pMDIChild) ;		
		}
	}
}

// cafemdi.cpp : implementation file
//

#include "stdafx.h"
#include "cafemdi.h"
#include "logview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCafeDrvMDIChild

IMPLEMENT_DYNCREATE(CCafeDrvMDIChild, CMDIChildWnd)

CCafeDrvMDIChild::CCafeDrvMDIChild()
{
}

CCafeDrvMDIChild::~CCafeDrvMDIChild()
{
}


BEGIN_MESSAGE_MAP(CCafeDrvMDIChild, CMDIChildWnd)
	//{{AFX_MSG_MAP(CCafeDrvMDIChild)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCafeDrvMDIChild message handlers

BOOL CCafeDrvMDIChild::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// CG: The following block was added by the Split Bars component.
	if (!m_wndSplitter.CreateStatic(this,
		                          2, 2 ))          // TODO: adjust the number of rows, columns
	//	                          CSize(10, 10), // TODO: adjust the minimum pane size
	//	                          pContext))
		{
			TRACE0("Failed to create split bar ");
			return FALSE;    // failed to create
		}

		// Suite tree pane. 
		if(!m_wndSplitter.CreateView(0,0,pContext->m_pNewViewClass,CSize(190,150),pContext))
		{
			return FALSE ;
		}
		// ConsoleViewPort pane; Use the viewport document
		if(!m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CLogView),CSize(190,150),pContext))
		{
			return FALSE ;
		}
		//LogTreeView pane
		if(!m_wndSplitter.CreateView(1,0,RUNTIME_CLASS(CLogView) /*pContext->m_pNewViewClass */,CSize(190,150),pContext))
		{
			return FALSE ;
		}

		//Logview (Text view) pane.
		if(!m_wndSplitter.CreateView(1,1,RUNTIME_CLASS(CViewportView),CSize(190,150),pContext))
		{
			return FALSE ;
		}
		return TRUE;
}


BOOL CCafeDrvMDIChild::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL ret = CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
	MDIMaximize() ;
	return ret; 

}

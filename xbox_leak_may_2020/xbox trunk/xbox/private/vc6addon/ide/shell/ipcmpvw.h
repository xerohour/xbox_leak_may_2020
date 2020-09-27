// ipcmpvw.h : interface of the CIPCompView class
//

#ifndef __IPCMPVW_H__
#define __IPCMPVW_H__

#include <docobvw.h>

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
// CIPCompView

class CIPCompView : public CDocObjectView
{
	DECLARE_DYNCREATE(CIPCompView)

public:
	CIPCompView();
	CIPCompDoc *GetDocument();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate(); // called first time after construct
	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	CSlob* GetSelection();
	HRESULT OnActivateDocObjectView();
	virtual void ShowContextPopupMenu(CPoint pt);
	// we don't want to defer activation for inplace activation
	// 
	virtual BOOL DeferActivation()
	{
		return FALSE;
	}
protected:

	DECLARE_MESSAGE_MAP()
	
	afx_msg LRESULT OnGetUndoString(WPARAM, LPARAM);
	afx_msg LRESULT OnGetRedoString(WPARAM, LPARAM);
	afx_msg LRESULT OnWmHelp(WPARAM, LPARAM);
	afx_msg LRESULT OnResetCommandTarget(WPARAM, LPARAM);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnHelp();
};

#ifndef _DEBUG	// debug version in ipcmpvw.cpp
inline CIPCompDoc * CIPCompView::GetDocument()
   { return (CIPCompDoc *) m_pDocument; }
#endif

#undef AFX_DATA
#define AFX_DATA NEAR

#endif	// __IPCMPVW_H__

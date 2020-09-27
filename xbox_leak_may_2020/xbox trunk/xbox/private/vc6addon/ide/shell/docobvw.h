// docobvw.h : interface of the CDocObjectView class
//

#ifndef __DOCOBVW_H__
#define __DOCOBVW_H__

class CDocObjectDoc;
class CDocObjectContainerItem;

#undef AFX_DATA
#define AFX_DATA

/////////////////////////////////////////////////////////////////////////////
// CDocObjectView

class AFX_EXT_CLASS CDocObjectView : public CPartView
{
	DECLARE_DYNCREATE(CDocObjectView)

protected:
	CDocObjectView();

public:
	CDocObjectDoc *GetDocument();
	// m_pSelection holds the selection to the current CDocobCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CDocobCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CDocObjectContainerItem* m_pSelection;

public:
	//{{AFX_VIRTUAL(CDocObjectView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	//}}AFX_VIRTUAL

public:
	virtual ~CDocObjectView();
	virtual BOOL CanClose();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// The following variable is used to avoid recursing in ActivateView.
	static BOOL s_bInActivateView;

	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);

protected:
	afx_msg void CDocObjectView::OnUpdateTRUE(CCmdUI* pCmdUI);

// Generated message map functions
protected:
	//{{AFX_MSG(CDocObjectView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnFileClose();
	afx_msg void OnFilePrint();
	afx_msg void OnPageSetup();
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);
	//}}AFX_MSG

protected:
	IOleCommandTarget *m_pOleCmdTarget;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __DOCOBVW_H__

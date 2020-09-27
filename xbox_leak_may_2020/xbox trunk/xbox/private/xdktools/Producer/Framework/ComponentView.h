#ifndef __COMPONENTVIEW_H__
#define __COMPONENTVIEW_H__

// ComponentView.h : header file
//

class CComponentDoc;
class CComponentCntrItem;

/////////////////////////////////////////////////////////////////////////////
// CComponentView view

class CComponentView : public CView
{
public:
	CComponentView();           // protected constructor used by dynamic creation
	virtual ~CComponentView();
	DECLARE_DYNCREATE(CComponentView)

// Attributes
public:
	IDMUSProdNode*	  m_pINode;
	IDMUSProdProject* m_pIProject;
	IDMUSProdEditor*  m_pIEditor;
	BOOL			  m_fSeed;	// view created from File Open/New
								// this view needs to stick around
								// until document is being destroyed
// Operations
public:
	CComponentDoc* GetDocument();
	// m_pSelection holds the selection to the current CBassCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CBassCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	CComponentCntrItem* m_pEditorCtrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComponentView)
	public:
	virtual BOOL IsSelected(const CObject* pDocItem) const;
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CComponentView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __COMPONENTVIEW_H__

#ifndef __JAZZTOOLBAR_H__
#define __JAZZTOOLBAR_H__

// JazzToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar window

class CBookmarkToolBar : public CToolBar
{
// Construction
public:
	CBookmarkToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBookmarkToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBookmarkToolBar();

	BOOL AddBookmark( CBookmark* pBookmark, BOOL fSelect );
	void RemoveAllBookmarks();
	void RemoveSelectedBookmark();
	void RemoveBookmark( CBookmark* pBookmark );
	void ApplySelectedBookmark();
	BOOL IsBookmarkSelected();
	BOOL BookmarkNameExists( LPCTSTR szName );
	void OnBookmarkRemove();

private:
	CFont	  m_font;
	CComboBox m_ctlBookmarkComboBox;

	// Generated message map functions
protected:
	//{{AFX_MSG(CBookmarkToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __JAZZTOOLBAR_H__

#ifndef __COMPONENTDOC_H__
#define __COMPONENTDOC_H__

// ComponentDoc.h : header file
//

#ifndef OPENFILENAME_NT4
#define OPENFILENAME_NT4 OPENFILENAME
#endif

class CComponentView;

/////////////////////////////////////////////////////////////////////////////
// CComponentDoc document

class CComponentDoc : public CCommonDoc
{
friend class CFileNode;

protected:
	CComponentDoc();			// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CComponentDoc)

// Attributes
public:
	CFileNode*		  m_pFileNode;
	IDMUSProdDocType* m_pIDocType;
	BYTE			  m_bAction;	// Doc being 1=deleted or 2=reverted

// Overrides
public:
	virtual BOOL DoSave( LPCTSTR szPathName, BOOL bReplace = TRUE );

// Operations
public:
	void OpenEditorWindow( IDMUSProdNode* pINode );
	void CloseAllViews( BOOL fAutoDelete );
	BOOL CloseViewsByNode( IDMUSProdNode* pINode );
	BOOL SaveAndCloseDoc();
	BOOL SaveComponentFileIfModified();
	BOOL RevertDocument();

private:
	CFrameWnd* CreateNewWindow( IDMUSProdNode* pINode );
	void AppendFilterSuffix( CString& filter, OPENFILENAME_NT4& ofn, CString* pstrDefaultExt );
	BOOL DoPromptFileSave( CJazzDocTemplate* pDocTemplate, UINT nResourceID, CString& fileName );
	void DetermineDefaultFileName( CString& strName );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComponentDoc)
	public:
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR szPathName);
	virtual BOOL OnSaveDocument(LPCTSTR szPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL IsModified();
	protected:
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComponentDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CComponentDoc)
	afx_msg void OnFileRuntimeSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __COMPONENTDOC_H__

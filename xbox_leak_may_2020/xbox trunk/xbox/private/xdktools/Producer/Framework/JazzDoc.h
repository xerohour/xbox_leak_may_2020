#if !defined(JAZZDOC_H__B6AED010_3BDF_11D0_89AC_00A0C9054129__INCLUDED_)
#define JAZZDOC_H__B6AED010_3BDF_11D0_89AC_00A0C9054129__INCLUDED_

// JazzDoc.h : interface of the CJazzDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CProject;

class CJazzDoc : public CCommonDoc
{
friend class CProjectPropTabGeneral;
friend class CProjectPropTabFolders;

protected: // create from serialization only
	CJazzDoc();
	DECLARE_DYNCREATE(CJazzDoc)

// Attributes
public:
	CProject* m_pProject;		// will end up being in CommonDoc (protected?)

// Overrides
public:
	virtual BOOL DoFileSave();
	virtual BOOL DoSave( LPCTSTR szPathName, BOOL bReplace = TRUE );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJazzDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR szPathName);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR szPathName);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJazzDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	BOOL LoadProject( LPCTSTR szPathName ); 
	BOOL SaveProject( LPCTSTR szPathName );
	void SetOpenEditorGUIDInFile( LPCTSTR szPathName, GUID guid );

// Generated message map functions
protected:
	//{{AFX_MSG(CJazzDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CJazzDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(JAZZDOC_H__B6AED010_3BDF_11D0_89AC_00A0C9054129__INCLUDED)

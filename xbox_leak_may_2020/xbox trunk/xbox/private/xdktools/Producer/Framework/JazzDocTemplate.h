#ifndef __JAZZDOCTEMPLATE_H__
#define __JAZZDOCTEMPLATE_H__

// JazzDocTemplate.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CJazzDocTemplate multiple doc template

class CJazzDocTemplate : public CMultiDocTemplate
{
friend class CComponentDoc;

	DECLARE_DYNAMIC(CJazzDocTemplate)

public:
	CJazzDocTemplate( IDMUSProdDocType* pIDocType,
					  HINSTANCE hInstance,
					  UINT nIDResource,
					  CRuntimeClass* pDocClass,
					  CRuntimeClass* pFrameClass,
					  CRuntimeClass* pViewClass );

// Attributes
public:

private:
	IDMUSProdDocType*	m_pIDocType;
	HINSTANCE			m_hInstance;
	short				m_nTreeImageIndex;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJazzDocTemplate)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsEqualDocType( IDMUSProdDocType* pIDocType );
	BOOL IsProjectDocType();
	HINSTANCE GetInstance();
	short GetTreeImageIndex();
	virtual BOOL GetDocString( CString& rString,
							enum DocStringIndex index ) const; // get one of the info strings
	virtual CDocument* OpenDocumentFile(
							LPCTSTR szPathName, BOOL bMakeVisible = TRUE);
	virtual Confidence MatchDocType( LPCTSTR lpszPathName, CDocument*& rpDocMatch );

protected:
	virtual ~CJazzDocTemplate();
	virtual void LoadTemplate();

	// Generated message map functions
	//{{AFX_MSG(CJazzDocTemplate)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __JAZZDOCTEMPLATE_H__

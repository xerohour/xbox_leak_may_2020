#if !defined(AFX_TABFILEDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABFILEDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabFileDesign.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabFileDesign dialog

class CTabFileDesign : public CPropertyPage
{
// Construction
public:
	CTabFileDesign( CFilePropPageManager* pPageManager );
	~CTabFileDesign();
	void SetFile( CFileNode* pFileNode );

// Dialog Data
	//{{AFX_DATA(CTabFileDesign)
	enum { IDD = IDD_TAB_FILE_DESIGN };
	CEdit	m_editDesignFileName;
	CStatic	m_staticDesignCreate;
	CStatic	m_staticDesignSize;
	CStatic	m_staticDesignModified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabFileDesign)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CFileNode*				m_pFileNode;
	CFilePropPageManager*	m_pPageManager;

// Implementation
protected:
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabFileDesign)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFILEDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)

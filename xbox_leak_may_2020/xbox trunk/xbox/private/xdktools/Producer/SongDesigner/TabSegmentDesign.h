#if !defined(AFX_TABSEGMENTDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABSEGMENTDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabSegmentDesign.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSegmentDesign dialog

class CTabSegmentDesign : public CPropertyPage
{
// Construction
public:
	CTabSegmentDesign( CSegmentPropPageManager* pPageManager );
	~CTabSegmentDesign();
	void SetSourceSegment( CSourceSegment* pSourceSegment );

// Dialog Data
	//{{AFX_DATA(CTabSegmentDesign)
	enum { IDD = IDD_TAB_SEGMENT_DESIGN };
	CEdit	m_editDesignFileName;
	CStatic	m_staticDesignCreate;
	CStatic	m_staticDesignSize;
	CStatic	m_staticDesignModified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabSegmentDesign)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CSourceSegment*				m_pSourceSegment;
	CSegmentPropPageManager*	m_pPageManager;
	BOOL						m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabSegmentDesign)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSEGMENTDESIGN_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)

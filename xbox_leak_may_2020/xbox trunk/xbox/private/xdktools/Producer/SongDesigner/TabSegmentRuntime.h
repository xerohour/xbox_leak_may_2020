#if !defined(AFX_TABSEGMENTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABSEGMENTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabSegmentRuntime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSegmentRuntime dialog

class CTabSegmentRuntime : public CPropertyPage
{
// Construction
public:
	CTabSegmentRuntime( CSegmentPropPageManager* pPageManager );
	~CTabSegmentRuntime();
	void SetSourceSegment( CSourceSegment* pSourceSegment );

// Dialog Data
	//{{AFX_DATA(CTabSegmentRuntime)
	enum { IDD = IDD_TAB_SEGMENT_RUNTIME };
	CEdit	m_editRuntimeFile;
	CStatic	m_staticRuntimeCreate;
	CStatic	m_staticRuntimeSize;
	CStatic	m_staticRuntimeModified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabSegmentRuntime)
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
	//{{AFX_MSG(CTabSegmentRuntime)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSEGMENTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)

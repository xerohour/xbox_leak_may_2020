#ifndef __PATTERNDLG_H__
#define __PATTERNDLG_H__

// PatternDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatternDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPatternCtrl;
interface IDMUSProdTimeline;
interface IDMUSProdStripMgr;
interface IDMUSProdStrip;
class CDirectMusicStyle;

class CPatternDlg : public CFormView
{
public:
	CPatternDlg();
	virtual ~CPatternDlg();

	// Additional methods
	void EnableTimer( BOOL fEnable );
	void Activate( BOOL fActive );
	void UpdateZoomAndScroll( void );

protected:
	DECLARE_DYNCREATE(CPatternDlg)

// Form Data
public:
	//{{AFX_DATA(CPatternDlg)
	enum { IDD = IDD_DLG_PATTERN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
private:
	short				m_nTimerID;			// Used to display transport cursor
	BOOL				m_fActive;

public:
	CPatternCtrl*		m_pPatternCtrl;
	IDMUSProdTimeline*	m_pITimelineCtl;
	IDMUSProdStripMgr*	m_pIMIDIStripMgr;
	IUnknown*			m_punkMIDIStripMgr;
	CDirectMusicStyle*	m_pStyle;
	IDMUSProdStrip*		m_pActiveStripWhenWindowDeactivated;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternDlg)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPatternDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __PATTERNDLG_H__

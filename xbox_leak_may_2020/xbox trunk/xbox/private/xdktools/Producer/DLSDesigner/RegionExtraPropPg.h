#if !defined(AFX_REGIONEXTRAPROPPG_H__6E263AA2_ECD6_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONEXTRAPROPPG_H__6E263AA2_ECD6_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RegionExtraPropPg.h : header file
//

#include "resource.h"
#include "myslider.h"

class CRegion;
class CRegionPropPgMgr;

#define DM_VALIDATE (WM_USER + 1000)

/////////////////////////////////////////////////////////////////////////////
// CRegionExtraPropPg dialog

class CRegionExtraPropPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegionExtraPropPg)

// Construction
public:
	CRegionExtraPropPg();
	~CRegionExtraPropPg();

	void SetObject(CRegion* pRegion) {m_pRegion = pRegion;}
	void SetPropMgr(CRegionPropPgMgr* pPropMgr) {m_pPropMgr = pPropMgr;}	

	void EnableControls(bool fEnable);
	void EnableAST(bool bNewState);
	void EnablePlayBack(bool bNewState);
	void EnableASC(bool bNewState);
	void EnableLoopCtrls(bool bNewState);
    void RevertToWaveASC(void);
    void RevertToWaveAST(void);
    void RevertToWavePlaybackSettings(void);

// Dialog Data
	//{{AFX_DATA(CRegionExtraPropPg)
	enum { IDD = IDD_REGION_EXTRA_PROP_PAGE };
	CEdit	m_SampleLengthEdit;
	CEdit	m_LoopStartEdit;
	CEdit	m_LoopLengthEdit;
	BOOL	m_fAllowCompress;
	BOOL	m_fAllowTruncate;
	//}}AFX_DATA

	DWORD	m_dwLoopLength;
	DWORD	m_dwLoopStart;

CMenu* m_pContextMenu;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegionExtraPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegionExtraPropPg)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOwsLoop();
	afx_msg void OnOwsAllowTruncate();
	afx_msg void OnOwsOverride();
	afx_msg void OnOwsAllowCompress();
	afx_msg void OnOwsOneShot();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRevertToWave();
	afx_msg void OnKillfocusOwsLoopstart();
	afx_msg void OnKillfocusOwsLooplength();
	afx_msg void OnChangeLooplength();
	afx_msg void OnChangeLoopstart();
	afx_msg LRESULT OnValidate(UINT wParam, long lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	enum { fLOOP_START = 0x01,	fLOOP_LENGTH = 0x02, fROOT_NOTE = 0x04 };
private:
	HRESULT ValidateAndSetLoopValues();
	void PostValidate(UINT	nControl);
	BOOL		m_fOneShot;
	CRegion*	m_pRegion;
	bool		m_fInOnInitialUpdate;
	bool		m_fActivateOverride;
	bool		m_fActivateAST;
	bool		m_fActivateASC;
	bool		m_fActivateLoop;

	DWORD m_bfTouched; //bit field for "has control been changed by user?"
	bool m_bTouchedByProgram; //Did program call SetWindowText() on this control?
	CRegionPropPgMgr*  m_pPropMgr;

	BOOL m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONEXTRAPROPPG_H__6E263AA2_ECD6_11D0_876A_00AA00C08146__INCLUDED_)

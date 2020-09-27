#if !defined(AFX_REGIONEDITOR_H__D0620362_CB9E_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONEDITOR_H__D0620362_CB9E_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RegionEditor.h : header file
//

#include "myslider.h"

/////////////////////////////////////////////////////////////////////////////
// CRegionEditor form view

class CWave;

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "RiffStrm.h"
#include "CollectionWaves.h"
#include "Region.h"

class CRegionEditor : public CFormView
{
friend class CRegionCtrl;

protected:
	CRegionEditor(CRegionCtrl *parent = NULL);	// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRegionEditor)

// Form Data
public:
	MySlider			m_msUpperRange;
    MySlider			m_msLowerRange;
    MySlider			m_msGroup;
    MySlider			m_msTune;
    MySlider			m_msAttenuation;
    MySlider			m_msUnityNote;
    DWORD				m_dwUpperRange;
	DWORD				m_dwLowerRange;
    DWORD				m_dwGroup;
    long				m_lFineTune;
    long				m_lAttenuation;
    long				m_lUnityNote;

	CRegion*			m_pRegion;
	CCollectionWaves*	m_pWaves;
	CWave*				m_pWave;
	CCollection*		m_pCollection;

	//{{AFX_DATA(CRegionEditor)
	enum { IDD = IDD_REGION };
	DWORD	m_dwLoopStart;
	DWORD	m_dwLoopLength;
	BOOL	m_fOneShot;
	BOOL	m_fAllowOverlap;
	BOOL	m_fAllowCompress;
	BOOL	m_fAllowTruncate;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

private:
CRegionCtrl*	m_parent;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionEditor)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRegionEditor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRegionEditor)
	afx_msg void OnGetwave();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeWavelink();
	afx_msg void OnAllowTruncate();
	afx_msg void OnAllowCompress();
	afx_msg void OnOverlap();
	afx_msg void OnChangeLooplength();
	afx_msg void OnChangeLoopstart();
	afx_msg void OnOneshot();
	afx_msg void OnChangeDgroup();
	afx_msg void OnChangeDtune();
	afx_msg void OnChangeDattenuation();
	afx_msg void OnChangeDunitynote();
	afx_msg void OnChangeElrange();
	afx_msg void OnChangeEurange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONEDITOR_H__D0620362_CB9E_11D0_876A_00AA00C08146__INCLUDED_)

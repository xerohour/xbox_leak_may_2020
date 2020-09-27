#if !defined(AFX_INSTRUMENTEDITOR_H__D0620363_CB9E_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_INSTRUMENTEDITOR_H__D0620363_CB9E_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InstrumentEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInstrumentEditor form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "Collection.h"

class CInstrument;
class CInstrumentCtrl;

class CInstrumentEditor : public CFormView
{
friend class CInstrumentCtrl;

protected:
	CInstrumentEditor(CInstrumentCtrl *parent = NULL);	// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInstrumentEditor)

// Form Data
public:
	//{{AFX_DATA(CInstrumentEditor)
	enum { IDD = IDD_INSTRUMENT };
	UINT	m_wBank;
	UINT	m_wBank2;
	UINT	m_wPatch;
	BOOL	m_fIsDrumKit;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentEditor)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInstrumentEditor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CInstrumentCtrl* m_parent;
	CInstrument* m_pInstrument;
	CCollection* m_pCollection;

	// Generated message map functions
	//{{AFX_MSG(CInstrumentEditor)
	afx_msg void OnChangeBank();
	afx_msg void OnChangeBank2();
	afx_msg void OnDrums();
	afx_msg void OnChangePatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTEDITOR_H__D0620363_CB9E_11D0_876A_00AA00C08146__INCLUDED_)

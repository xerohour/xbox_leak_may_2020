#if !defined(AFX_CONDITIONEDITOR_H__C1589783_EB68_4B7B_84A9_F587596DB4CC__INCLUDED_)
#define AFX_CONDITIONEDITOR_H__C1589783_EB68_4B7B_84A9_F587596DB4CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "Region.h"
#include "Instrument.h"
#include "SCriptEdit.h"

// ConditionEditor.h : header file
//
class CDLSComponent;
class CScriptEdit;

/////////////////////////////////////////////////////////////////////////////
// CConditionEditor dialog

class CConditionEditor : public CDialog
{
// Construction
public:
	CConditionEditor(CWnd* pParent = NULL, CDLSComponent* pComponent = NULL, CConditionalChunk* pConditionalChunk = NULL, CInstrument* pInstrument = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConditionEditor)
	enum { IDD = IDD_CONDITION_EDITOR };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	void ShowPopupMenu(CWnd* pWnd, CPoint point);

private:
	void WriteToExpression(WORD wID);

private:
	CDLSComponent*		m_pComponent;
	CConditionalChunk*	m_pConditionalChunk;
	CScriptEdit			m_ScriptEdit;
	CInstrument*		m_pInstrument;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConditionEditor)
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONEDITOR_H__C1589783_EB68_4B7B_84A9_F587596DB4CC__INCLUDED_)

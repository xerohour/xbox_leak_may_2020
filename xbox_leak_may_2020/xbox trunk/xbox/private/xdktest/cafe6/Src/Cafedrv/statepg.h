///////////////////////////////////////////////////////////////////////////////
//	statepg.h
//
//	Created by :			Date :
//		BrianCr				08/07/95
//
//	Description :
//		Definition of the CStatePage class
//

#ifndef __STATEPG_H__
#define __STATEPG_H__

#include "caferes.h"
#include "suitedoc.h"

/////////////////////////////////////////////////////////////////////////////
// CStatePage dialog

class CStatePage: public CPropertyPage
{
	DECLARE_DYNCREATE(CStatePage)
// Construction
public:
	CStatePage(CSuiteDoc* pSuiteDoc = NULL);
	~CStatePage();

// Dialog Data
	//{{AFX_DATA(CStatePage)
	enum { IDD = IDD_CurrentState };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatePage)
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatePage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillStateList(void);

// data
protected:
	CSuiteDoc* m_pSuiteDoc;
};

#endif // __STATEPG_H__

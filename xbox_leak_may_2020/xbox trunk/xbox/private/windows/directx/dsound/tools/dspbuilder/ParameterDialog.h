/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ParameterDialog.h

Abstract:

	Dialog to handle Effect parameters

Author:

	Robert Heitkamp (robheit) 30-Nov-2001

Revision History:

	30-Nov-2001 robheit
		Initial Version

--*/
#ifndef PARAMETER_DIALOG_H
#define PARAMETER_DIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxtempl.h>
#include "dspbuilder.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CKnob;
class CModule;

//------------------------------------------------------------------------------
//	CParameterDialog
//------------------------------------------------------------------------------
class CParameterDialog : public CDialog  
{
public:

	CParameterDialog(CModule* pModule=NULL, LPCTSTR pName=NULL, int width=5);
	virtual ~CParameterDialog(void);

	void AddKnob(CKnob* pKnob);	// Knobs will be freed with the dialog exits
//	inline void SetModule(CModule* pModule) { m_pModule = pModule; };
//	inline void SetName(LPCTSTR pName) { m_name = pName; };
	void UpdateKnobs(void);
	void SetRealtime(BOOL bFlag);

	inline BOOL GetRealtime(void) const { return m_bRealtime; };
		
protected:

	virtual BOOL OnInitDialog(void);
	virtual void OnOK(void);
	virtual void OnCancel(void);
	virtual void PostNcDestroy(void);

protected:
	//{{AFX_MSG(CParameterDialog)
	afx_msg void OnReset();
	afx_msg void OnApply();
	afx_msg void OnDefault();
	afx_msg void OnRealtime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	CString					m_name;
	int						m_width;
	CArray<CKnob*, CKnob*&>	m_knobs;
	CModule*				m_pModule;
	BOOL					m_bRealtimeEnabled;
	BOOL					m_bRealtime;
};

#endif


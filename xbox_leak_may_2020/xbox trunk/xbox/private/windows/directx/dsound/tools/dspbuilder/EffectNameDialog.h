/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	EffectNameDialog.h

Abstract:

	Dialog to handle Effect names

Author:

	Robert Heitkamp (robheit) 12-Feb-2002

Revision History:

	12-Feb-2002 robheit
		Initial Version

--*/
#ifndef EFFECT_NAME_DIALOG_H
#define EFFECT_NAME_DIALOG_H

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

//------------------------------------------------------------------------------
//	CEffectNameDialog
//------------------------------------------------------------------------------
class CEffectNameDialog : public CDialog  
{
public:

	CEffectNameDialog(LPCTSTR pEffect, LPCTSTR pName);
	virtual ~CEffectNameDialog(void);

	inline LPCTSTR GetName(void) const { return m_name; };

protected:

	virtual BOOL OnInitDialog(void);
	virtual void OnOK(void);

protected:

	//{{AFX_MSG(CEffectNameDialog)
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	CString	m_name;
	CString	m_effect;
};

#endif


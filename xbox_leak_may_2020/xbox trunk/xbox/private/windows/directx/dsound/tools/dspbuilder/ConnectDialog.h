/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ConnectDialog.h

Abstract:

	Dialog to handle connection to Xbox

Author:

	Robert Heitkamp (robheit) 13-Jun-2001

Revision History:

	13-Jun-2001 robheit
		Initial Version

--*/
#if !defined(AFX_CONNECTDIALOG_H__98670C4C_7B34_4304_9BFA_F51E9C7F7E80__INCLUDED_)
#define AFX_CONNECTDIALOG_H__98670C4C_7B34_4304_9BFA_F51E9C7F7E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"

//------------------------------------------------------------------------------
//	CConnectDialog
//------------------------------------------------------------------------------
class CConnectDialog : public CDialog  
{
public:

	CConnectDialog(LPCTSTR pName = NULL);
	virtual ~CConnectDialog(void);

	inline void SetName(LPCTSTR pName) { m_name = pName; };
	inline LPCTSTR GetName(void) const { return m_name; };

protected:

	virtual void OnOK(void);
	virtual BOOL OnInitDialog(void);

private:

	CString	m_name;
};

#endif // !defined(AFX_CONNECTDIALOG_H__98670C4C_7B34_4304_9BFA_F51E9C7F7E80__INCLUDED_)

/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ExportDialog.h

Abstract:

	Export options

Author:

	Robert Heitkamp (robheit) 14-Jun-2001

Revision History:

	14-Jun-2001 robheit
		Initial Version

--*/
#if !defined(AFX_EXPORTDIALOG_H__F52849B5_9623_4305_899A_06AB112B6A88__INCLUDED_)
#define AFX_EXPORTDIALOG_H__F52849B5_9623_4305_899A_06AB112B6A88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"

//------------------------------------------------------------------------------
//	CExportDialog 
//------------------------------------------------------------------------------
class CExportDialog : public CDialog  
{
public:

	CExportDialog(void);
	CExportDialog(const CStringArray& configs);
	virtual ~CExportDialog(void);

	LPCTSTR GetI3DL2(void) const { return m_i3dl2; };
	BOOL GetPatchXTalkToLFE(void) const { return m_bPatchXtalkToLFE; };
	BOOL GetIncludeXTalk(void) const { return m_bIncludeXtalk; };
	void SetPatchXTalkToLFE(BOOL bFlag) { m_bPatchXtalkToLFE = bFlag; };
	void SetI3DL2(LPCTSTR pReverb) { m_i3dl2 = pReverb; };
	void SetIncludeXTalk(BOOL bFlag) { m_bIncludeXtalk = bFlag; };
	void SetConfigs(const CStringArray& configs) { m_configs.Append(configs); };

protected:

	virtual void OnOK(void);
	virtual BOOL OnInitDialog(void);

private:

	CStringArray	m_configs;
	CString			m_i3dl2;
	BOOL			m_bPatchXtalkToLFE;
	BOOL			m_bIncludeXtalk;
};

#endif


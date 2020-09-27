// cmdlinebld.h: interface for the CPrjCommandLineBldTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDLINEBLD_H__611D08C2_8065_11D1_9EE4_00C04FAC5A7C__INCLUDED_)
#define AFX_CMDLINEBLD_H__611D08C2_8065_11D1_9EE4_00C04FAC5A7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sniff.h"

class CPrjCommandLineBldTest : public CTest  
{
	DECLARE_TEST(CPrjCommandLineBldTest, CSniffDriver)
public:
	CPrjCommandLineBldTest();
	virtual ~CPrjCommandLineBldTest();
	virtual void Run(void);
	// test methods
	void TestCleanOption(void);

private:
	CString		m_strMSDEVPath;
	COProject	m_prj;
	BOOL SpawnConsoleApp(CString cpEXE, CString cpCmdLine);
	void DoGetLastMessageDlg(char *strFile, int iLineNum);

};

#endif // !defined(AFX_CMDLINEBLD_H__611D08C2_8065_11D1_9EE4_00C04FAC5A7C__INCLUDED_)

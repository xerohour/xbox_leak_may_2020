///////////////////////////////////////////////////////////////////////////////
//	DlgWalk.H
//
//	Created by :					Date :
//		DougMan							1/7/97
//
//	Description :
//		Declaration of a DlgWalk class
//		This test verifies that all the IV5 dialogs are accessable
//

#ifndef __DLGWALK_H__
#define __DLGWALK_H__

#include "sniff.h"
#include "ivutil.h"
#include "resource.h"

// Used by DLGWALK
const int TOTALDIALOGS = 18;

struct CASES {
	UINT	iDlg_Mnemonic;
	UINT	iVerification;
	CString	aAdditionalKey;
	CString PreTest;
	CString PostTest;
}; // End TestCases Structure


///////////////////////////////////////////////////////////////////////////////
//	BOOL CGeneric:: class

class CDlgWalk : public CTest 
{
	DECLARE_TEST(CDlgWalk, CSniffDriver)

public:
// Overrides
	virtual void Run(void);
// Operations
	
// Data

protected:
	
private:
int CDlgWalk::FindDialog(CString strPreCase, UINT iMnemonic, UINT iVerify, CString strAddKeys, CString strPostCase);
void CDlgWalk::InitalCases(CASES *ptTestCases);
};


#endif // __DLGWALK_H__
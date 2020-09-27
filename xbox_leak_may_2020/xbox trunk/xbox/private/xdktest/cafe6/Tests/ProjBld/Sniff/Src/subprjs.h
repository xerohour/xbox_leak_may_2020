///////////////////////////////////////////////////////////////////////////////
//	SUBPRJS.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __SUBPRJS_H__
#define __SUBPRJS_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"
///////////////////////////////////////////////////////////////////////////////
//	CPrjSubPrjTest class

class CPrjSubPrjTest : public CTest
{
	DECLARE_TEST(CPrjSubPrjTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	CString szProjDLLLoc;
	CString szDLLProj;
	CString szDLLCPPFiles;
	CString szProjEXELoc;
	COProject prj;

	BOOL InsertProject( void );
	BOOL AddFilesToDLLProject( void );
	BOOL VerifySubproject( void );
	BOOL VerifyBLDs ( void );
	BOOL CloseProject( void );
	BOOL OpenProject( void  );
	BOOL BuildProject( void );
};

#endif  //__SUBPRJS_H__ 
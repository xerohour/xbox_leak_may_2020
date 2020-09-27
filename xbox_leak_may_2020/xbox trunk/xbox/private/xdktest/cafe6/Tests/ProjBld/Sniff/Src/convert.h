///////////////////////////////////////////////////////////////////////////////
//	SUBPRJS.H
//
//	Created by :			Date :
//              ThuyT                                   07/05/96
//
//	Description :
//		Declaration of the CPrjAppTest class
//

#ifndef __CONVERT_H__
#define __CONVERT_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"
///////////////////////////////////////////////////////////////////////////////
//	CPrjConversionTest class

class CPrjConversionTest : public CTest
{
	DECLARE_TEST(CPrjConversionTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	CString szProjEXELoc;
	COProject prj;

	BOOL VerifyBuild( BOOL Build );
	BOOL CloseProject( void );
	BOOL OpenProject( CString ProjectName );
	BOOL BuildProject( void );
	BOOL VerifyBLD ( CString szFileName);
	BOOL VerifyConversion( void );
};

#endif  //__CONVERT_H__ 

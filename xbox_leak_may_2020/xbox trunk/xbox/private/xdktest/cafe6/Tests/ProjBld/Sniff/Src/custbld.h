///////////////////////////////////////////////////////////////////////////////
//	CUSTBLD.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __CUSTBLD_H__
#define __CUSTBLD_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CPrjCustomBuildTest class

class CPrjCustomBuildTest : public CTest
{
	DECLARE_TEST(CPrjCustomBuildTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	COProject prj;
	CString szProjEXELoc;

	BOOL SetCustomBuild( void);
	BOOL VerifyCustomBuild( void );
	BOOL CloseProject( void );
	BOOL OpenProject( void  );
	BOOL BuildProject( void );
};

#endif //__CUSTBLD_H__
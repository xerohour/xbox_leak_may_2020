///////////////////////////////////////////////////////////////////////////////
//	SCANDEP.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __SCANDEP_H__
#define __SCANDEP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"
///////////////////////////////////////////////////////////////////////////////
//	CPrjScanDepTest class

class CPrjScanDepTest : public CTest
{
	DECLARE_TEST(CPrjScanDepTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	COProject prj;
	CString szProjEXELoc; 

	BOOL VerifyResults( CString szFileName );	
	BOOL CloseProject( void );
	BOOL OpenProject( void  );
	BOOL BuildProject( void );
};

#endif //__SCANDEP_H__

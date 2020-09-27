///////////////////////////////////////////////////////////////////////////////
//	BLDOPTS.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __BLDOPTS_H__
#define __BLDOPTS_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"
///////////////////////////////////////////////////////////////////////////////
//	CPrjBuildSettingsTest class

class CPrjBuildSettingsTest : public CTest
{
	DECLARE_TEST(CPrjBuildSettingsTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	CString szDummyLibrary;
	COProject prj;
	CString szBuildOutputFile;

	BOOL AddLibrary( void );
	BOOL SetIntermediateDir( void );
	BOOL SetOutDir( void );
	BOOL VerifyIntermediateDirectory( void );
	BOOL VerifyOutDirectory( void );
	BOOL VerifyLinkerLibrary( void );
	BOOL CloseProject( void );
	BOOL OpenProject( void  );
	BOOL BuildProject( void );
};
#endif //__BLDOPTS_H__
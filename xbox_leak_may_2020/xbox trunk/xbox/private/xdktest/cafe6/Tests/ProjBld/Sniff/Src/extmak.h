///////////////////////////////////////////////////////////////////////////////
//	EXTMAK.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __EXTMAK_H__
#define __EXTMAK_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"
///////////////////////////////////////////////////////////////////////////////
//	CPrjExternalMAKTest class

class CPrjExternalMAKTest : public CTest
{
	DECLARE_TEST(CPrjExternalMAKTest, CSniffDriver)

// Operations
public:
	virtual void Run(void);

protected:
	CString szExternalProject;
	CString szNewExternalProject;
	COProject prj;

	BOOL PrepareExtProject( void );
	BOOL OpenExtProject( void );
	BOOL SetExtDebugOption( void );
	BOOL SetExtBuildCmdLine( void );
	BOOL CloseProject( void );
	BOOL BuildProject( void );
};

#endif __EXTMAK_H__
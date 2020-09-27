/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	XFApiTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 05-Nov-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	05-Nov-2001 jeffsul
		Initial Version

--*/

#ifndef _XFAPITEST_H_
#define _XFAPITEST_H_

#include "Test.h"
#include <xfont.h>

class CXFApiTest : public CTest
{
public:
	CXFApiTest();
	~CXFApiTest();

protected:
	HRESULT ReadIni();

private:
	VOID ExecuteParameterChecks();

};

#endif //#ifndef _XFAPITEST_H_

///////////////////////////////////////////////////////////////////////////////
//  INTSUB.CPP
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      implementation of the CIntSubSuite class
//

#include "stdafx.h"
#include "intsub.h"
#include "afxdllx.h"
#include "..\..\core\cleanup.h"

#include "i1cases.h"
#include "i2cases.h"
#include "i3cases.h"
#include "i4cases.h"
#include "i5cases.h"
#include "i6cases.h"
#include "i7cases.h"
#include "i8cases.h"
#include "i9cases.h"
#include "i10cases.h"
#include "i11cases.h"
#include "i12cases.h"
#include "i13cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CIntSubSuite

IMPLEMENT_SUBSUITE(CIntSubSuite, CIDESubSuite, "EE - Integer", "VCQA Debugger")

BEGIN_TESTLIST(CIntSubSuite)
	TEST(CInt1Cases, RUN)
	TEST(CInt2Cases, RUN)
	TEST(CInt3Cases, RUN)
	TEST(CInt4Cases, RUN)
	TEST(CInt5Cases, RUN)
	TEST(CInt6Cases, RUN)
	TEST(CInt7Cases, RUN)
	TEST(CInt8Cases, RUN)
	TEST(CInt9Cases, RUN)
	TEST(CInt10Cases, RUN)
	TEST(CInt11Cases, RUN)
	TEST(CInt12Cases, RUN)
	TEST(CInt13Cases, RUN)
END_TESTLIST()

void CIntSubSuite::CleanUp(void)
	{
	::CleanUp();
	}

BOOL fProjectReady = FALSE;

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization






///////////////////////////////////////////////////////////////////////////////
//  MIXSUB.CPP
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      implementation of the CMixSubSuite class
//

#include "stdafx.h"
#include "mixsub.h"
#include "afxdllx.h"
#include "..\..\core\cleanup.h"

#include "m1cases.h"
#include "m2cases.h"
#include "m3cases.h"
#include "m4cases.h"
#include "m5cases.h"
#include "m6cases.h"
#include "m7cases.h"
#include "m8cases.h"
#include "m9cases.h"
#include "m10cases.h"
#include "m11cases.h"
#include "m12cases.h"
#include "m13cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CMixSubSuite

IMPLEMENT_SUBSUITE(CMixSubSuite, CIDESubSuite, "EE - Mixed", "VCQA Debugger")

BEGIN_TESTLIST(CMixSubSuite)
	TEST(CMix1Cases, RUN)
	TEST(CMix2Cases, RUN)
	TEST(CMix3Cases, RUN)
	TEST(CMix4Cases, RUN)
	TEST(CMix5Cases, RUN)
	TEST(CMix6Cases, RUN)
	TEST(CMix7Cases, RUN)
	TEST(CMix8Cases, RUN)
	TEST(CMix9Cases, RUN)
	TEST(CMix10Cases, RUN)
	TEST(CMix11Cases, RUN)
	TEST(CMix12Cases, RUN)
	TEST(CMix13Cases, RUN)
END_TESTLIST()

void CMixSubSuite::CleanUp(void)
	{
	::CleanUp();
	}

BOOL fProjectReady = FALSE;

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization






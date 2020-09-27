///////////////////////////////////////////////////////////////////////////////
//  MISCSUB.CPP
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      implementation of the CMiscSubSuite class
//

#include "stdafx.h"
#include "miscsub.h"
#include "afxdllx.h"
#include "..\..\core\cleanup.h"

#include "aicases.h" 
#include "asncases.h"
#include "baecases.h"
#include "bancases.h"
#include "cmpcases.h"
#include "cstcases.h"
#include "divcases.h"
#include "eqcases.h"
#include "gecases.h"
#include "gtcases.h"
#include "lecases.h"
#include "lsecases.h"
#include "lshcases.h"
#include "ltcases.h"
#include "mincases.h"
#include "neqcases.h"
#include "notcases.h"
#include "oelcases.h"
#include "oeqcases.h"
#include "orcases.h"
#include "pdccases.h"
#include "piccases.h"
#include "plucases.h"
#include "qstcases.h"
#include "rsecases.h"
#include "remcases.h"
#include "rmecases.h"
#include "rshcases.h"
#include "topcases.h"
#include "umncases.h"
#include "xorcases.h"
#include "xrecases.h"
					
#undef THIS_FILE	
static char BASED_CODE THIS_FILE[] = __FILE__;
					
					
/////////////////////////////////////////////////////////////////////////////
// CMiscSubSuite

IMPLEMENT_SUBSUITE(CMiscSubSuite, CIDESubSuite, "EE - Misc", "VCQA Debugger")

BEGIN_TESTLIST(CMiscSubSuite)
	TEST(CAndIfCases, RUN)
	TEST(CAssignCases, RUN)
	TEST(CBitAndEqCases, RUN)
	TEST(CBitAndCases, RUN)
	TEST(CComplCases, RUN)
	TEST(CCastCases, RUN)
	TEST(CDivCases, RUN)
	TEST(CEqCases, RUN)
	TEST(CGreaterEqCases, RUN)
	TEST(CGreaterCases, RUN)
	TEST(CLessThanEqCases, RUN)
	TEST(CLShiftEqCases, RUN)
	TEST(CLShiftCases, RUN)
	TEST(CLessThanCases, RUN)
	TEST(CMinusCases, RUN)
	TEST(CNotEqCases, RUN)
	TEST(CNotCases, RUN)
	TEST(COrElseCases, RUN)
	TEST(COrEqCases, RUN)
	TEST(COrCases, RUN)
	TEST(CPostDecCases, RUN)
	TEST(CPreIncCases, RUN)
	TEST(CPlusCases, RUN)
	TEST(CQuestCases, RUN)
	TEST(CRemainCases, RUN)
	TEST(CRemainEqCases, RUN)
	TEST(CRShiftEqCases, RUN)
	TEST(CRShiftCases, RUN)
	TEST(CTimesOpCases, RUN)
	TEST(CUMinusCases, RUN)
	TEST(CXOrCases, RUN)
	TEST(CXOrEqCases, RUN)
END_TESTLIST()

void CMiscSubSuite::CleanUp(void)
	{
	::CleanUp();
	}

BOOL fProjectReady = FALSE;

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization






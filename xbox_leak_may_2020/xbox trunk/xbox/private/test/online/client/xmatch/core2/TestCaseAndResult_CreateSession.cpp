#include <common.h>
#include "TestCaseAndResult_CreateSession.h"
#include "CTestCase_CreateSession.h"

using namespace MatchCoreTestNamespace;

namespace MatchCoreTestNamespace {

TestCaseAndExpected_CreateSession g_TCAE_CreateSession[]=
{
/*
	{	
		{PubCurr, PubAvail, PriCurr, PriAvail, Attribute, Event, TaskCreate}	For Creation
		{HrExpected, HTaskExpected} For Creation
		UpdateType 
		{PubCurr, PubAvail, PriCurr, PriAvail, Attribute, Event, TaskCreate}	For Update
		{HrExpected, HTaskExpected}	For Update 
	}
*/
	{	
		{DWZero, DWZero, DWZero, DWZero, ZeroNum_Null, NullEvent, ValildTask},
		{SY_FAIL, SY_NULL}, 
		SY_NOUPDATE
	},
	{	
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_SUCCESS, SY_NONNULL}, 
		SY_NOUPDATE
	},
	{	
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_SUCCESS, SY_NONNULL}, 
		SY_VALIDSESSIONID, 
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_SUCCESS, SY_NONNULL}	
	},
	{	
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_SUCCESS, SY_NONNULL},
		SY_INVALIDSESSIONID,
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_FAIL, SY_NULL},
	}
	{	
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_SUCCESS, SY_NONNULL},
		SY_NULLSESSIONID,
		{DWOne, DWOne, DWOne, DWOne, One_Int, NullEvent, ValildTask},
		{SY_FAIL, SY_NULL},
	}
};

int	g_nCase_CreateSession = sizeof g_TCAE_CreateSession / sizeof(TestCaseAndExpected_CreateSession);

}


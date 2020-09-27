///////////////////////////////////////////////////////////////////////////////
//	CPICASES.CPP
//
//	Created by :			Date :
//		MikePie				2/28/95
//
//	Description :
//		Implementation of the CCpiTestCases class
//

#include "stdafx.h"
#include "Cpicases.h"

// for _stat CRT function
#include "sys\types.h"
#include "sys\stat.h"

// for rmdir CRT function
#include "direct.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCpiTestCases, CTest, "Code Page Resource Tests", -1, CSniffDriver)

void CCpiTestCases::Run(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CCpiTestCases::CpiNewRC(void)
{
	HWND hwndRC;

	if ((hwndRC = UIWB.CreateNewFile(GetLocString(IDSS_NEW_RC))) == NULL)
	{
		m_pLog->RecordFailure("Could not create RC script");
		UIWB.CloseAllWindows();
		return FALSE;
	}
	else
	{
		// Worked
	}
	UIWB.CloseAllWindows();
	return TRUE;
}
 
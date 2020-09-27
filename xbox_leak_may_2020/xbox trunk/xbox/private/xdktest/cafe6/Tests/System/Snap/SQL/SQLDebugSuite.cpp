///////////////////////////////////////////////////////////////////////////////
//	SQLDebugSuite.CPP
//
//	Created by :			Date :			
//		CHRISKO					5/21/97
//
//	Description :
//		implementation of the CSQLDebugSuite class
//

#include "stdafx.h"
#include "SQLDebugSuite.h"
#include "afxdllx.h"

#include "SQLDebugCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CSQLDebugSuite, CIDESubSuite, "SQL debugging", "VCQA")

BEGIN_TESTLIST(CSQLDebugSuite)
	char buff[1024];
TEST(CSQLDebugCase, (GetLang()==CAFE_LANG_ENGLISH?RUN:DONTRUN))
	strcpy(buff,"SVR- ");
	strcat(buff,m_SuiteParams->GetTextValue("SERVER", "GALSQL1"));
	strcat(buff,"; DB- ");
	strcat(buff,m_SuiteParams->GetTextValue("DATABASE", "pubs"));
	strcat(buff,"; USER- ");
	strcat(buff,m_SuiteParams->GetTextValue("USER", "sa"));
	strcat(buff,"; PWD- ");
	strcat(buff,m_SuiteParams->GetTextValue("PWD"));
	pTest->SetName(buff);
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

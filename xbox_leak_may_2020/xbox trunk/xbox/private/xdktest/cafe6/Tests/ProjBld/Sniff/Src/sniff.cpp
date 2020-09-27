///////////////////////////////////////////////////////////////////////////////
//      SNIFF.CPP
//
//      Created by :                    Date :
//              TomWh                                   10/18/93
//
//      Description :
//              implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "prjcases.h"
#include "bldopts.h"
#include "subprjs.h"
#include "custbld.h"
#include "scandep.h"
#include "extmak.h"
#include "convert.h"
#include "cmdlinebld.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "ProjBld Sniff Test", "brickb x62132")
BEGIN_TESTLIST(CSniffDriver)
	TEST(CPrjAppTest, RUN)
	TEST(CPrjBuildSettingsTest, RUN) DEPENDENCY(CPrjAppTest)
	TEST(CPrjSubPrjTest, RUN)		DEPENDENCY(CPrjAppTest)
	TEST(CPrjScanDepTest, RUN)		DEPENDENCY(CPrjAppTest)
	TEST(CPrjCustomBuildTest, RUN)	DEPENDENCY(CPrjAppTest)
	TEST(CPrjConversionTest, DONTRUN)
	TEST(CPrjExternalMAKTest, RUN)
	TEST(CPrjCommandLineBldTest, RUN) DEPENDENCY(CPrjAppTest)
END_TESTLIST()

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	CleanUpVSSDroppings();
	ExpectedMemLeaks(0);
}

void CSniffDriver::CleanUp(void)
{
	// close all windows
	UIWB.CloseAllWindows();

}


void CSniffDriver::CleanUpVSSDroppings()
{
	// lets make sure there are none of those damm *.scc files in the folders....
	// subdirectory recursion would be better here, didn't have time
	// will get back to this down the road, plus I didn't want to go deleting everything
	// who knows what test this would screw up.
	CString strTemp;
	CString strCWD;

	// wack it from the current directory
	::GetCurrentDirectory(MAX_PATH, strTemp.GetBuffer(MAX_PATH));
	GetShortPathName(strTemp, strCWD.GetBuffer(MAX_PATH), MAX_PATH);

	strTemp.Format("%s\\", strCWD);
	
	DeleteFileByWildCard(strTemp);

	// look in the SRC folder
	strTemp.Format("%s\\src\\", strCWD);
	DeleteFileByWildCard(strTemp);

	// go one more level into the Projdir
	strTemp.Format("%s\\src\\%s\\", strCWD, V4PROJDIR);
	DeleteFileByWildCard(strTemp);

	// one more in the DLL folder
	strTemp.Format("%s\\src\\%s\\%s\\", strCWD, V4PROJDIR, _T("DLL"));
	DeleteFileByWildCard(strTemp);

}


void CSniffDriver::DeleteFileByWildCard(CString strWildCard)
{
	WIN32_FIND_DATA pFFD;
	HANDLE hFileFind;
	CString strTemp = strWildCard + _T("*.scc");

	if ((hFileFind = ::FindFirstFile(strTemp, &pFFD)) != INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordInfo("Doing a little clean up of Source Safe droppings %s", strWildCard);

		DeleteFile(strWildCard + pFFD.cFileName);
		while (FindNextFile(hFileFind, &pFFD))
		{
			::DeleteFile(pFFD.cFileName);
		}
		::FindClose(hFileFind);
	}

}

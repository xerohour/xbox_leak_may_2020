///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		RickKr					8/30/93
//
//	Description :
//		implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "navig.h"
#include "queries.h"
#include "filters.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

static CString szBackslash	= "\\";
static CString szSrcDir		= "src";
static CString szProjName	= "browse";

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "Browser Sniff Test", "Scot Forbes x64054")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CNavigate, RUN)
	TEST(CQueries, RUN)
	TEST(CFilters, DONTRUN)
END_TESTLIST()

COWorkSpace ws;

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	fProjReady = FALSE;
	CString szSourceDir(GetCWD() + szSrcDir);
	CString szOrigWorkspace(szSourceDir + szBackslash + "original" + WSPEXTENSION);
	CString szNewWorkspace(szSourceDir + szBackslash + szProjName + WSPEXTENSION);
	CString szOrigBuildfile(szSourceDir + szBackslash + "original" + PRJEXTENSION);
	CString szNewBuildfile(szSourceDir + szBackslash + szProjName + PRJEXTENSION);
	
	CSubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);

	::CopyFile( szOrigWorkspace,  szNewWorkspace, FALSE );
	::SetFileAttributes( szNewWorkspace , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag
	::CopyFile( szOrigBuildfile,  szNewBuildfile, FALSE );
	::SetFileAttributes( szNewBuildfile , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag

}

void CSniffDriver::CleanUp(void)
{
	KillFile("src", "browse.ncb");
	KillFile("src", "browse.plg");
	KillFile("src", "browse.mak");
	KillFile("src", "browse.opt");
	KillFile("src", "browse.dsp");
	KillFile("src", "browse.dsw");
	KillAllFiles("src\\Debug");
	ws.CloseAllWindows();
}

int CSniffDriver::InitProject(void)
	
	{
	if(!fProjReady)
		
		{
		COProject prj;
		
		if(prj.Open(GetCWD() + szSrcDir + szBackslash + szProjName + PRJEXTENSION) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("SNIFF ERROR: could not open browse.dsp");
			return ERROR_ERROR;
			}

		if(prj.ToggleBSCGeneration(TOGGLE_ON) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("SNIFF ERROR: could not toggle on BSC generation");
			return ERROR_ERROR;
			}

		if(prj.Build(5) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("SNIFF ERROR: could not build project");
			return ERROR_ERROR;
			}

		//fProjReady = TRUE;
		}

	// REVIEW (michma): stupid work-around to make sure case sensitivity is on for navigations.
	UIBrowse uibrowse;
	uibrowse.Query(DefRef, "WinMain");
	uibrowse.CloseBrowser();

	return ERROR_SUCCESS;
	}

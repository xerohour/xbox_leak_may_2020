///////////////////////////////////////////////////////////////////////////////
//	InsertCallToATLCOMServerInAppCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CInsertCallToATLCOMServerInAppCase class

#include "stdafx.h"
#include "InsertCallToATLCOMServerInAppCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInsertCallToATLCOMServerInAppCase, CSystemTestSet, "Insert Call To ATL COM Server In App", -1, CInsertCallToATLCOMServerInAppSubsuite)

void CInsertCallToATLCOMServerInAppCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CInsertCallToATLCOMServerInAppCase::Run(void)

{	 	
	// open the AppWiz App's main source file.
	LOG->RecordCompare(src.Open(GetCWD() + m_strWorkspaceLoc + m_strAppProjName + ".CPP") == ERROR_SUCCESS, 
		"Open file '%s' for editing", m_strAppProjName + ".CPP");
	
	// find the line where we are supposed to add construction code.
	LOG->RecordCompare(src.Find(GetLocString(IDSS_ADD_CONSTRUCT_CODE_HERE)),
		"Search file '%s' for place to add call to ATL COM server", m_strAppProjName + ".CPP");
	
	// insert the includes for the ATL COM server.
	src.TypeTextAtCursor("{UP 2}{HOME}");
	src.TypeTextAtCursor("#include \"ATL COM Server\\ATL COM Server.h\"{ENTER}");
	src.TypeTextAtCursor("#include \"ATL COM Server\\ATL COM Server_i.c\"{ENTER}");
	
	// insert the CoInitialize and CoCreateInstance calls.
	src.TypeTextAtCursor("{ENTER}{DOWN 2}{TAB}");
	src.TypeTextAtCursor("CoInitialize(NULL);", TRUE);
	src.TypeTextAtCursor("{ENTER}IUnknown *pIUnknown;{ENTER}");

	CString strCoCreateInstanceCall[] = 
	{
		"HRESULT hr = CoCreateInstance(",
		m_strATLCOMServerClsID + ",", 
		"NULL,", 
		"CLSCTX_INPROC_SERVER,", 
		m_strATLCOMServerIID + ",",
		"(void**)&pIUnknown);"
	};

	for(int i = 0; i < 6; i++)
	{
		src.TypeTextAtCursor("{ENTER}");
		src.TypeTextAtCursor(strCoCreateInstanceCall[i], TRUE);
	}

	src.TypeTextAtCursor("{ENTER 2}");
	src.TypeTextAtCursor("ASSERT(SUCCEEDED(hr));", TRUE);
	src.TypeTextAtCursor("{ENTER 2}");

	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the AppWiz App project so we can rebuild with the changes.
	prj.SetTarget((CString)m_strAppProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	// build the project.
	LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, 
		"Build '%s' project with call to ATL COM server.", m_strAppProjName);
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
		"Verify build of '%s' project with call to ATL COM server", m_strAppProjName);

}

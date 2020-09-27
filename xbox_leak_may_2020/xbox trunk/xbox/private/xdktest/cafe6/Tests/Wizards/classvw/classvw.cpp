///////////////////////////////////////////////////////////////////////////////
//  CLASSVW.CPP
//
//  Created by: Jim Griesmer    Date: 6/29/95
//
//  Description:  Implementation of CComprehensiveTest.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ClassVw.h"
#include "rawstrng.h"
#include "support.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_LISTTEST(CComprehensiveTest, CTest, "Comprehensive Class Verification", "comprhsv.lst", CClassViewSubSuite)

void CComprehensiveTest::PreRun(CRawListString &strRaw)
{
    CTest::PreRun(strRaw);

    CString strProjectName;

    // Parse out the raw line into component variables.
    strRaw >> strProjectName >> m_strHFile >> m_strCPPFile >> m_strBaseLineFile;

    LOG->RecordInfo("Project: %s", (LPCSTR)strProjectName);
    LOG->RecordInfo("Header: %s", (LPCSTR)m_strHFile);
    LOG->RecordInfo("CPP: %s", (LPCSTR)m_strCPPFile);
    LOG->RecordInfo("BaseLine: %s", (LPCSTR)m_strBaseLineFile);
	
	//Kill all files in the current directory
	KillAllFiles(m_strCWD + strProjectName);
    // Create a new project.
    //m_proj.New(strProjectName,              // Project Name
    m_proj.NewProject(UIAW_PT_APP,			// Project Type
            strProjectName,					// Project Name
            m_strCWD + strProjectName,		// Project Directory
            NULL);							// Platforms
//			NULL,							// New Subdirectory
//          CNP_ADD_FILES_MYSELF,          // Which files added?
//			NULL,                          // ???? Don't know
//			NULL,                          // ????
//			NULL);                         // ????

	m_strProjBaseName = m_proj.GetName();
	m_strProjBaseName = m_strProjBaseName.Left(m_strProjBaseName.Find('.'));

	CString strDestFile = m_strCWD + '\\' + strProjectName + '\\' + m_strHFile;

	if (!CopyFile(m_strCWD + m_strHFile, strDestFile, FALSE))
		LOG->RecordFailure("Couldn't make copy of %s", m_strHFile);

	if (!SetFileAttributes(strDestFile, FILE_ATTRIBUTE_NORMAL))
		LOG->RecordFailure("Couldn't set the attributes of %s", strDestFile);

	strDestFile = m_strCWD + '\\' + strProjectName + '\\' + m_strCPPFile;

	if (!CopyFile(m_strCWD + m_strCPPFile, strDestFile, FALSE))
		LOG->RecordFailure("Couldn't make copy of %s", m_strCPPFile);

	if (!SetFileAttributes(strDestFile, FILE_ATTRIBUTE_NORMAL))
		LOG->RecordFailure("Couldn't set the attributes of %s", strDestFile);
}

void CComprehensiveTest::Run(CRawListString& strRaw)
{    
    CTest::Run(strRaw);

    // Test for activation and readiness of IDE.
    GetSubSuite()->GetIDE()->IsActive();

    // Add the CPP file to the project.
    m_proj.AddFiles(m_strCPPFile);
    m_proj.AddFiles(m_strHFile);

    // Verify existence of classes in the class view.
    m_proj.VerifyClasses(m_strBaseLineFile);

    m_proj.Close();
}

void CComprehensiveTest::PostRun(CRawListString &strRaw)
{
    CTest::PostRun(strRaw);

    // Do all this just to set the current directory for the IDE session...
    UIFileOpenDlg dlgFileOpen;
    dlgFileOpen.Display();
    dlgFileOpen.SetPath(m_strCWD);
    dlgFileOpen.Cancel();

    // Delete the project directory.
    KillAllFiles(m_proj.GetDir());
}

#ifdef DONT_KNOW_EXACTLY_WHERE_THIS_CAME_FROM
CString CComprehensiveTest::GetSelectedItemName()
{

// DOES NOT WORK YET!!!
    MST.DoKeys("%0");   // Activate Workspace window

    CString str = "Empty";

    UINT msg = RegisterWindowMessage("GetItemProp");


    int i = 0;
    ATOM atom;

    do
    {
        MST.DoKeys("{Down}");
        i++;
        atom = (ATOM)SendMessage( GetSubSuite()->GetIDE()->m_hWnd, msg, P_ProjItemName, 0);
        if (atom != 0)
        {
            UINT ret = GlobalGetAtomName( atom, str.GetBufferSetLength(512), 512 );
            ret = GlobalDeleteAtom(atom);   // this atom created by the Workbench
            str.ReleaseBuffer(-1);

            TRACE("STRING = %s\n", (LPCSTR)str);
        }
    }
    while (atom != 0 && i < 10);

    return str;
}

#endif

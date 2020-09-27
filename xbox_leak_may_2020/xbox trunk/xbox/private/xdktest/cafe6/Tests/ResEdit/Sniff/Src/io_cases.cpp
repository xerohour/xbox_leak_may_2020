///////////////////////////////////////////////////////////////////////////////
//	IO_CASES.CPP
//
//	Created by :			Date :
//		DougT				8/13/93
//		RonPih				7/4/97		-Fixed reliance on RO file attribute
//										 for io_open.h
//
//	Description :
//		Implementation of the CIO_TestCases class
//
//		Assumes io_open.rc & io_open.h are in the baseln subdirectory.
//

#include "stdafx.h"
#include "io_cases.h"

// for _stat CRT function
#include "sys\types.h"
#include "sys\stat.h"

// for rmdir CRT function
#include "direct.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CIO_TestCases, CTest, "Resource I/O Tests", -1, CSniffDriver)

void CIO_TestCases::Run(void)
{
	//Delete all io_ sniff files from any previous test
	DeleteFile(m_strCWD + "IO_TMPSV.RC");
	DeleteFile(m_strCWD + "IO_OPEN.APS");
	SetFileAttributes(m_strCWD + "IO_OPEN.RC",FILE_ATTRIBUTE_NORMAL);
	DeleteFile(m_strCWD + "IO_OPEN.RC");
	SetFileAttributes(m_strCWD + "IO_OPEN.H",FILE_ATTRIBUTE_NORMAL);
	DeleteFile(m_strCWD + "IO_OPEN.H");
	DeleteFile(m_strCWD + "INC\\IO_TMPSV.H");
	RemoveDirectory(m_strCWD + "INC");

	// copy io_open.* from baseln subdirectory
	if (!CopyFile(m_strCWD + "baseln\\io_open.h",m_strCWD + "io_open.h",FALSE) ||
		!CopyFile(m_strCWD + "baseln\\io_open.rc",m_strCWD + "io_open.rc",FALSE))
	{
		m_pLog->RecordFailure("Copy io_open.* to working directory failed.  Subsequent tests may also fail.");
		UIWB.CloseAllWindows();
		return;
	}

	XSAFETY;
	IO_OpenRC();
	XSAFETY;
	IO_SaveAsRC();
	XSAFETY;
	IO_CompareHFile();
	IO_CompareRCFile();
	XSAFETY;
	IO_CloseRC();
	XSAFETY;

	// copy io_open.* from baseln subdirectory
	// (HACK: until all other vres tests assume io_open.* starts in baseln subdirectory)
	SetFileAttributes(m_strCWD + "IO_OPEN.RC",FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(m_strCWD + "IO_OPEN.H",FILE_ATTRIBUTE_NORMAL);
	if (!CopyFile(m_strCWD + "baseln\\io_open.h",m_strCWD + "io_open.h",FALSE) ||
		!CopyFile(m_strCWD + "baseln\\io_open.rc",m_strCWD + "io_open.rc",FALSE))
	{
		m_pLog->RecordInfo("Copy io_open.* to working directory failed.  Subsequent tests may fail.");
		UIWB.CloseAllWindows();
		return;
	}
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CIO_TestCases::IO_OpenRC(void)
{
	HWND hwnd = UIWB.OpenFile(m_strCWD + "io_open.rc");
	if (hwnd == NULL) {
		m_pLog->RecordFailure("File-Open: hwnd == %x", hwnd);
		return FALSE;
	}
	return TRUE;
}

BOOL CIO_TestCases::IO_SaveAsRC(void)
{
	UIWB.DoCommand(IDM_FILE_SETINCLUDES, DC_MNEMONIC);
	MST.DoKeys("io_tmpsv.h");
	MST.DoKeys("{ENTER}");

  	BOOL bSaveResult;
	bSaveResult = UIWB.SaveFileAs(m_strCWD + "io_tmpsv.rc", TRUE /* Overwrite? */);
//	Sleep(500);
	UIWB.WaitAttachActive(15000);

	if (bSaveResult == NULL) {
		m_pLog->RecordFailure("File-Save As: bSaveResult == %i", bSaveResult);
		return FALSE;
	}
	return TRUE;
}

BOOL CIO_TestCases::IO_CompareRCFile(void)
{
	m_pLog->Comment("Attempting compare of RC files.");

	int nCompareRCResult = CompareFiles(m_strCWD + "io_tmpsv.rc", m_strCWD + "io_base.rc", 0);
	switch(nCompareRCResult)
	{
		case CF_SAME:
			m_pLog->RecordInfo("io_tmpsv.rc and io_base.rc compare fine");
			break;
		case CF_DUPNAME:
			ASSERT(FALSE);		// how could this happen?
			break;
		case CF_CANTOPEN1:
			m_pLog->RecordFailure("can't open io_tmpsv.rc");
			break;
		case CF_CANTOPEN2:
			m_pLog->RecordFailure("can't open io_base.rc");
			break;
/*		case CF_DIFFSIZE:
			m_pLog->RecordFailure("io_tmpsv.rc and io_base.rc are different sizes"); // REVIEW(enriquep): fix special NT J case
			break;	  */
		case CF_NOTSAME:
			m_pLog->RecordFailure("io_tmpsv.rc and io_base.rc are different at the byte level");
			break;
		default:
			m_pLog->RecordInfo("io_tmpsv.rc and io_base.rc compare fine");	 // REVIEW(enriquep): fix special NT J case
			break;

	}

	return TRUE;
}

BOOL CIO_TestCases::IO_CompareHFile(void)	
{
	m_pLog->Comment("Attempting compare of H files.");

	int nCompareHResult  = CompareFiles(m_strCWD + "io_tmpsv.h", m_strCWD + "io_base.h", 0);

	switch(nCompareHResult)
	{
		case CF_SAME:
			m_pLog->RecordInfo("io_tmpsv.h and io_base.h compare fine");
			break;
		case CF_DUPNAME:
			ASSERT(FALSE);		// how could this happen?
			break;
		case CF_CANTOPEN1:
			m_pLog->RecordFailure("can't open io_tmpsv.h");
			break;
		case CF_CANTOPEN2:
			m_pLog->RecordFailure("can't open io_base.h");
			break;
		case CF_DIFFSIZE:
			m_pLog->RecordFailure("io_tmpsv.h and io_base.h are different sizes");
			break;
		case CF_NOTSAME:
		default:
			m_pLog->RecordFailure("io_tmpsv.h and io_base.h are different at the byte level");
			break;
	}

	UIWB.DoCommand(IDM_FILE_SETINCLUDES, DC_MNEMONIC);

	MST.DoKeys(m_strCWD + "INC\\io_tmpsv.h");
	MST.DoKeys("{ENTER}");

	struct _stat buf;
	int statresult;
	char buffer[] = "A line to output";

	int bSaveAlteredHResult;
	bSaveAlteredHResult = UIWB.SaveFileAs(m_strCWD + "io_tmpsv.rc", TRUE /* Overwrite? */);
	Sleep(500);
	if (bSaveAlteredHResult == NULL) {
		m_pLog->RecordFailure("File-Save As: bSaveAlteredHResult == %i", bSaveAlteredHResult);
	}

	// Get data associated with "INC\io_tmpsv.h"
	statresult = _stat(m_strCWD + "INC\\io_tmpsv.h", &buf );

	// Check if statistics are valid: */
	if( statresult != 0 )
	{
		m_pLog->RecordFailure(m_strCWD + "INC\\io_tmpsv.h does not exist.");
	}

	return TRUE;
}

BOOL CIO_TestCases::IO_CloseRC(void)
{
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
	}
	return TRUE;
}

BOOL CIO_TestCases::IO_DirtyRC(void)
{
	HWND hwnd = UIWB.OpenFile(m_strCWD + "io_open.rc");

	if (hwnd == NULL) {
		m_pLog->RecordFailure("File-Open: hwnd == %x", hwnd);
	}

//REVIEW(dougt): Convert constants to variable-driven commands

	MST.DoKeys("{DOWN 3}{TAB}");				// locate a string table
	MST.DoKeys(" {ENTER}");						// select it, open the string table
	MST.DoKeys("{DOWN 2}");						// go to the end of the string table
	MST.DoKeys("%{ENTER}");						// open a prop page
	MST.DoKeys("{TAB}This is a test.");			// enter a caption for the new string table entry

  	BOOL bSaveDirtyRCResult;
	bSaveDirtyRCResult = UIWB.SaveFileAs(m_strCWD + "io_tmpdt.rc", TRUE /* Overwrite? */);
	if (bSaveDirtyRCResult == NULL) {
		m_pLog->RecordFailure("Dirty RC -- File-Save As: bSaveDirtyRCResult == %i", bSaveDirtyRCResult);
	}

	MST.DoKeys("{DEL}");						// delete the new entry

	BOOL bSaveRestoredRCResult;
	bSaveRestoredRCResult = UIWB.SaveFileAs(m_strCWD + "io_tmpdt.rc", TRUE /* Overwrite? */);
	if (bSaveRestoredRCResult == NULL) {
		m_pLog->RecordFailure("Restored RC -- File-Save As: bSaveRestoredRCResult == %i", bSaveRestoredRCResult);
	}

	m_pLog->Comment("Attempting compare of original and dirtied RC files.");
	int nCompareDirtyResult  = CompareFiles(m_strCWD + "io_open.rc", m_strCWD + "io_tmpdt.rc", 0);
	switch(nCompareDirtyResult)
	{
		case CF_SAME:
			m_pLog->RecordInfo("io_open.rc and io_tmpdt.rc compare fine");
			break;
		case CF_DUPNAME:
			ASSERT(FALSE);		// how could this happen?
			break;
		case CF_CANTOPEN1:
			m_pLog->RecordFailure("can't open io_open.rc");
			break;
		case CF_CANTOPEN2:
			m_pLog->RecordFailure("can't open io_tmpdt.rc");
			break;
		case CF_DIFFSIZE:
			m_pLog->RecordFailure("io_open.rc and io_tmpdt.rc are different sizes");
			break;
		case CF_NOTSAME:
		default:
			m_pLog->RecordFailure("io_open.rc and io_tmpdt.rc are different at the byte level");
			break;
	}

	HWND hwnd2 = UIWB.OpenFile(m_strCWD + "io_tmpdt.rc");

	if (hwnd2 == NULL) {
		m_pLog->RecordFailure("File-Open: hwnd == %x", hwnd2);
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//      GRDCASES.CPP
//
//      Created by :                Date :
//              MikePie               4/13/95
//
//      Description :
//              Implementation of the CGrdTestCases class
//
//              Repaired / Modified by:         Date:                   Description:
//                              MikePie                           5/01/95                  Fixed non-graceful exit of last munged test
//
//              Repaired / Modified by:         Date:                   Description:
//                              ejugovic                          5/29/96                  Fixed compare to CWD while adding entries
///////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "grdcases.h"
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// CString MBCEntry = "GRIDTESTAˆŸ‚`ˆŸ•\";
CString MBCEntry = "GRIDTEST_MBC_Disabled";

IMPLEMENT_TEST(CGrdTestCases, CTest, "Grid Control Tests", -1, CSniffDriver)

void CGrdTestCases::Run(void)
{
	XSAFETY;
	Sleep(2000);
	EXPECT(AddToGrid());
	EXPECT(ReorderGrid());
	EXPECT(ReopenGrid());
	EXPECT(CheckMBC());
	EXPECT(CloseGrid());
}


///////////////////////////////////////////////////////////////////////////////
//      Test Cases
///////////////////////////////////////////////////////////////////////////////
//
BOOL CGrdTestCases::AddToGrid(void)
{
	m_pLog->RecordInfo("Opening grid control in Tools.Options.Directories...");
	UIOptionsTabDlg uioptdlg;
	uioptdlg.Display();

	uioptdlg.AddDirectory("GRIDTEST1");
	MST.DoKeys("{DOWN}");
	Sleep(500);

	DWORD sysinfo = (DWORD) GetSystem();
	if ((sysinfo & SYSTEM_DBCS) || (sysinfo & SYSTEM_JAPAN))
	{
		m_pLog->RecordInfo("Adding false direcory (%s) to grid...",MBCEntry);
		uioptdlg.AddDirectory(MBCEntry);
	}
	else
	{
		m_pLog->RecordInfo("Adding false direcory (GRIDTEST2) to grid...");
		uioptdlg.AddDirectory("GRIDTEST1" ) ;
	}
	MST.DoKeys("{ENTER}");  // Hit Enter
	Sleep(500);
	MST.DoKeys("{TAB}");    // Tab to yes
	MST.DoKeys("{ENTER}");
	Sleep(800);
	MST.DoKeys("{DOWN}");
	Sleep(500); 

	CString GridEntry;      
	m_pLog->RecordInfo("Adding currrent directory (%s) to grid...",m_strCWD);
    MST.DoKeys(m_strCWD);       // Enter the current working directory
	MST.DoKeys("{ENTER}");  // Hit Enter
	Sleep(600);
	MST.DoKeys("{UP}");             // Go back up to the new entry
	EmptyClip();                    // Empty the clipboard--we're going to do a compare...

	m_pLog->RecordInfo("Copying grid entry text to clipboard...");
//	MST.DoKeys("^(C)");             // Ctrl-C:Copy the new entry into clipboard     
	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
	Sleep(600);
	MST.DoKeys("{ESC}");    // Get out of edit mode
	Sleep(600);

	m_pLog->RecordInfo("Getting clipped text from clipboard...");
	GetClipText(GridEntry); // Get the grid entry out of the clipboard

// workaround for grid control problem in retail IDE (06/07/96)
	if (GridEntry[GridEntry.GetLength() - 1] != '\\')	// check for existing backslash
		GridEntry += "\\";								// need to append backslash to match CWD

	if (GridEntry == m_strCWD)
	{
		m_pLog->RecordInfo("Got current directory from grid...");
		return TRUE;
	}
	else
	{
		m_pLog->RecordInfo("Expected (%s), got (%s).",m_strCWD,GridEntry);
		return FALSE;
	}                               
}

 
BOOL CGrdTestCases::ReorderGrid(void)
{
	m_pLog->RecordInfo("Moving last entry to top...");
	MST.DoKeys("{END}");    // Move to bottom of list
	MST.DoKeys("{UP}");             // Move up one - this should be CWD
//	MST.DoKeys("^(X)");             // Ctrl-X:Get the last entry and put it in clipboard
	UIWB.DoCommand(ID_EDIT_CUT, DC_ACCEL);
	Sleep(600);
	MST.DoKeys("{ESC}");
//	MST.DoKeys("{DEL}");
	UIWB.DoCommand(ID_EDIT_CLEAR, DC_ACCEL);
	Sleep(600);

	m_pLog->RecordInfo("Moving to top of grid...");
	MST.DoKeys("{HOME}");   // Go to top of list
	Sleep(600);
//	MST.DoKeys("^(V)");             // Ctrl-V:Paste
	UIWB.DoCommand(ID_EDIT_PASTE, DC_ACCEL);
	MST.DoKeys("{ENTER}");

	CString GridEntry;
	EmptyClip();                    // Empty clipboard
//	MST.DoKeys("^(C)");
	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
	MST.DoKeys("{ENTER}");
	GetClipText(GridEntry);

// workaround for grid control problem in retail IDE (06/07/96)
	if (GridEntry[GridEntry.GetLength() - 1] != '\\')	// check for existing backslash
		GridEntry += "\\";								// need to append backslash to match CWD

	if (GridEntry == m_strCWD)
	{
		m_pLog->RecordInfo("Reorder successful.");
		return TRUE;
	}
	else
	{
		m_pLog->RecordInfo("Got (%s), but expected (%s).",GridEntry,m_strCWD);
		return FALSE;
	}
}


BOOL CGrdTestCases::ReopenGrid(void)
{
	m_pLog->RecordInfo("Closing grid...");
	MST.DoKeys("{ENTER}");
	Sleep(1000);

	m_pLog->RecordInfo("Opening grid control...");
	UIOptionsTabDlg uioptdlg;
	uioptdlg.ShowDirectoryTab();
/*
	MST.DoKeys("%(TO)");			// Tools.Options
	Sleep(1000);					// Wait for dialog to open

//	REVIEW: This no longer works with the new focus 
//	(there's a better way to do it anyway) CHRISSH
	MST.DoKeys("C");				// Go to Compatibility tab before
	Sleep(200);                     // going to Directories tab. This is the only
	MST.DoKeys("D");                // way we can be sure we are getting the Directories
	Sleep(200);						// tab and not the Debug tab.
	MST.DoKeys("%(D)");             // Tab to Directories 
	Sleep(200);
*/
	MST.DoKeys("{HOME}");			// Go to top of grid                    
	Sleep(200);

	CString GridEntry;
	EmptyClip();                    // Empty clipboard
//	MST.DoKeys("^(C)");
	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
	MST.DoKeys("{ENTER}");
	Sleep(600);
	GetClipText(GridEntry);

// workaround for grid control problem in retail IDE (06/07/96)
	if (GridEntry[GridEntry.GetLength() - 1] != '\\')	// check for existing backslash
		GridEntry += "\\";              // need to append backslash to match CWD

	if (GridEntry == m_strCWD)
	{
		m_pLog->RecordInfo("Reopen successful.");
		return TRUE;
	}
	else
	{
		m_pLog->RecordInfo("Got (%s), but expected (%s).",GridEntry,m_strCWD);
		return FALSE;
	}

}


BOOL CGrdTestCases::CheckMBC(void)
{

	DWORD sysinfo = (DWORD) GetSystem();

	if ((sysinfo & SYSTEM_DBCS) || (sysinfo & SYSTEM_JAPAN))
	{
		MST.DoKeys("{END}");
		MST.DoKeys("{UP}");

		CString GridEntry;
		EmptyClip();                    // Empty clipboard
//		MST.DoKeys("^(C)");             //
		UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
		MST.DoKeys("{ESC}");    //
		Sleep(1000);
		GetClipText(GridEntry);
		GridEntry.MakeUpper();
		MBCEntry = m_strCWD + MBCEntry;
		MBCEntry.MakeUpper();
		if (GridEntry == MBCEntry)
		{
			m_pLog->RecordInfo("Got MBC entry expected.");
			return TRUE;
		}
		else
		{
			MST.DoKeys("{ESC}");
//			MST.DoKeys("^({F4})");
			UIWB.DoCommand(ID_FILE_CLOSE, DC_ACCEL);
			m_pLog->RecordInfo("Expected (%s), but got (%s) instead.",MBCEntry,GridEntry);
			return FALSE;
		}
	}
	else
	{
		m_pLog->RecordInfo("Not an MBC system. Continuing test...");
		return TRUE;
	}
}

BOOL CGrdTestCases::CloseGrid(void)
{
	MST.DoKeys("{END}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("{ENTER}");
	return TRUE;
}

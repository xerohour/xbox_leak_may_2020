///////////////////////////////////////////////////////////////////////////////
//	LYTCASES.CPP
//
//	Created by :			Date :
//		Enriquep					10/25/93
//
//	Description :
//		Dockable views and toolbars layout tests (Design and Debug Modes)
//

#include "stdafx.h"
#include "lytcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CLytTestCases, CTest, "Layout Tests", -1, CSniffDriver)

void CLytTestCases::Run(void)
{
	// get the current working directory for this test
	::GetCurrentDirectory(MAX_PATH, m_strCWD.GetBuffer(MAX_PATH));
	m_strCWD.ReleaseBuffer();
	m_strCWD += '\\';

	XSAFETY;
	DesignModeLyt();
	DebugModeLyt();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CLytTestCases::DesignModeLyt(void)
{
	BOOL bResult = TRUE;   // Flag used to indicate if test passed. TRUE == PASSED

	COProject proj;

	// Open an internal makefile according to platform being tested

#ifdef _X86_
        proj.Open(m_strCWD + "projects\\x86\\layout.mak");
#else
#ifdef _MIPS_
		proj.Open(m_strCWD + "projects\\mips\\layout.mak");
#endif
#endif

	//Check Standard toolbar is docked on upper border
	if( !WS.IsToolbarDocked(IDTB_SUSHI_STANDARD, DW_TOP) )
	{
		m_pLog->Comment("(1) Standard Toolbar should be docked on upper border");	
		bResult = FALSE;
	}


	// Check Resource Toolbar isn't docked
    if ( !WS.IsToolbarVisible(IDTB_VRES_RESOURCE) )
	{
        m_pLog->Comment("(1) Resource Toolbar is not visible");
		bResult = FALSE;
	}			
	if( WS.IsToolbarDocked(IDTB_VRES_RESOURCE, DW_DONT_CARE) )
	{
		m_pLog->Comment("(1) Resource Toolbar should not be docked");	
		bResult = FALSE;
	}

	// Check Output Window
	if (!WS.IsDockWindowVisible(IDW_OUTPUT_WIN) )
	{
        m_pLog->Comment("(1) Output Window is not visible");
		bResult = FALSE;
	}	 
	if( WS.IsWindowDocked(IDW_OUTPUT_WIN, DW_DONT_CARE) )
	{
		m_pLog->Comment("(1) Output Window should not be docked");	
		bResult = FALSE;
	}


	if (bResult == FALSE)
	{
		m_pLog->RecordFailure("(1) Initial Design Mode Layout");
		return FALSE;
	}	
    
    m_pLog->RecordInfo("(1) Initial Design Mode Layout passed");

	return TRUE;
}


BOOL CLytTestCases::DebugModeLyt(void)
{
	UIWB.StepInto(3000);

	UIWBMessageBox mbAlert;
	while( mbAlert.WaitAttachActive(1000) )	// dismiss any alerts that comes up
	{
		if (mbAlert.ButtonExists(MSG_NO))
			mbAlert.ButtonClick(MSG_NO);
		else
			mbAlert.ButtonClick();	
	}					


	// Verify Design mode toolbars aren't visible when in Debug mode. Output Window is still visible.

	int nSleep = 0;
	while ( ((WS.IsToolbarVisible(IDTB_VRES_RESOURCE)) || 
			(!WS.IsDockWindowVisible(IDW_OUTPUT_WIN))) &&
			(nSleep++ < 30) )
		Sleep(1000);

	if ( (WS.IsToolbarVisible(IDTB_VRES_RESOURCE)) || (!WS.IsDockWindowVisible(IDW_OUTPUT_WIN)) )
	{
		m_pLog->RecordFailure("(1) Initial Debug Mode Layout:Design Mode toolbars are still visible");

		UIWB.StopDebugging();	  // Exit Debug mode
		
		return FALSE;
	}

	// Check Debug windows are present

	BOOL bResult = TRUE;	 		// Flag used to indicate pass or failure of test. TRUE == PASS
	
	//  **** Check Debug Toolbar
	if(!WS.IsToolbarVisible(IDTB_VCPP_DEBUG))
	{
		m_pLog->Comment("(2) Debug Toolbar is not visible");
		bResult = FALSE;
	}

	if ( !WS.IsWindowDocked(IDTB_VCPP_DEBUG, DW_RIGHT) )
	{
        m_pLog->Comment("(2) Debug Toolbar should be docked on right border");
		bResult = FALSE;
	}

	//  **** Check Watch window
	if(!WS.IsDockWindowVisible(IDW_WATCH_WIN))
	{
		m_pLog->Comment("(2) Watch window is not visible");
		bResult = FALSE;
	}

	if ( WS.IsWindowDocked(IDW_WATCH_WIN, DW_DONT_CARE) )
	{
        m_pLog->Comment("(2) Watch window should not be docked");
		bResult = FALSE;
	}
	
	//  **** Check Memory window
	if(!WS.IsDockWindowVisible(IDW_MEMORY_WIN))
	{
		m_pLog->Comment("(2) Memory window is not visible");
		bResult = FALSE;
	}
	
	if ( WS.IsWindowDocked(IDW_MEMORY_WIN, DW_DONT_CARE) )
	{
        m_pLog->Comment("(2) Memory window should not be docked");
		bResult = FALSE;
	}
	
	//  **** Check Locals window
	if(!WS.IsDockWindowVisible(IDW_LOCALS_WIN))
	{
		m_pLog->Comment("(2) Locals window is not visible");
		bResult = FALSE;
	}

	if ( !WS.IsWindowDocked(IDW_LOCALS_WIN, DW_BOTTOM) )
	{
        m_pLog->Comment("(2) Locals window should be docked on bottom");
		bResult = FALSE;
	}

	//  **** Check Registers window
	if(!WS.IsDockWindowVisible(IDW_CPU_WIN))
	{
		m_pLog->Comment("(2) Registers window is not visible");
		bResult = FALSE;
	}

	if ( !WS.IsWindowDocked(IDW_CPU_WIN, DW_BOTTOM) )
	{
        m_pLog->Comment("(2) Registers window should be docked on bottom");
		bResult = FALSE;
	}


	//  **** Check Call Stack window
	if(!WS.IsDockWindowVisible(IDW_CALLS_WIN))
	{
		m_pLog->Comment("(2) Call Stack window is not visible");
		bResult = FALSE;
	}

	if ( !WS.IsWindowDocked(IDW_CALLS_WIN, DW_BOTTOM) )
	{
        m_pLog->Comment("(2) Call Stack window should be docked on bottom");
		bResult = FALSE;
	}

	if (bResult == FALSE)
	{
		m_pLog->RecordFailure("(2) Initial Debug Mode Layout");
		UIWB.StopDebugging();	  // Exit Debug mode
	}	

	m_pLog->RecordInfo("(2) Initial Debug Mode Layout passed");

	UIWB.StopDebugging();	  // Exit Debug mode
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CTabTestCases class

IMPLEMENT_TEST(CTabTestCases, CTest, "Tab Dialog Tests", -1, CSniffDriver)

void CTabTestCases::Run(void)
{
	UIOptionsTabDlg uiOptDlg;
	uiOptDlg.Display();		   
	uiOptDlg.ShowPage(TAB_WORKSPACE, 8);
	int k = 0;
	for (int i=1; i < 8; i++)
	{
		if (i % 2 == 0)
		{
			for(k = 1; k <= i; k++)
			{
				uiOptDlg.ActivateNextPage();
			}
		}
		else
		{
			for(k = 1; k <= i; k++)
			{
				uiOptDlg.ActivateNextPage(FALSE);
			}
		}
		
	}  	

	MST.WButtonClick(GetLabel(IDOK) );		 //Close Dialog	  REVIEW(Enriquep): Use uiOptDlg.OK() function

	m_pLog->RecordInfo("(3) Tabbed thru Options Dialog without problems");
}

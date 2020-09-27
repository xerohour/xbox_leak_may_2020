///////////////////////////////////////////////////////////////////////////////
//      macrcase.CPP
//
//      Created by :                    Date :
//        Craigs                          7/17/96
//
//      Description :
//              Implementation of the CMacroTestCases class
//

#include "stdafx.h"
#include "macrcase.h"
#include "w32repl.h"
#include "support.h"
										 
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;        

IMPLEMENT_TEST(CMacroTestCases, CTest, "Macro Tests", -1, EditorSniff)

void CMacroTestCases::Run(void)
{
  COClipboard ClipBoard;
	XSAFETY;
  UICustomizeTabDlg CstmzDlg ;

  CstmzDlg.ShowAddIn_MacroPg() ;
  CstmzDlg.BrowseForMacroFiles();
    
  MST.DoKeys(m_strCWD+"111__V36.vbs");
  Sleep(6000);
  MST.WButtonClick(GetLabel(IDOK)) ; 

  //Give VBS time to parse (more than enough)
  WaitForInputIdle(g_hTargetProc, 6000);

  MST.DoKeys("{TAB}");
  MST.DoKeys("{TAB}");
  MST.DoKeys("{TAB}");
  MST.DoKeys("{DOWN}");
  MST.DoKeys("111__V36");
  MST.DoKeys(" ");

  MST.DoKeys("{ESC}");   

  //Create a new file
  COSource sourc ;
  
  sourc.Create() ;
  //Add Some text to the file
  ClipBoard.Empty();
  MST.DoKeys("This is a test of the VBS system, this is only a test: (Beeeeeep)");
  MST.DoKeys("+{HOME}");
  MST.DoKeys("^c");  

  CString ClipText = ClipBoard.GetText();
  
  //Bring up the tools->macro dialog
  UIMacrosDlg macroDlg ;
  macroDlg.Display() ;
  macroDlg.SelectMacroFile("111_V36");
  macroDlg.SelectMacro("TestActive");
  ClipBoard.Empty();
  macroDlg.RunMacro() ;

   if (ClipText != ClipBoard.GetText())
  {
    m_pLog->RecordCriticalError("Expected and actual output do not match");
  }
}

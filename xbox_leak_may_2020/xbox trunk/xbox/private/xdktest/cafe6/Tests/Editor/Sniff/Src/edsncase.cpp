///////////////////////////////////////////////////////////////////////////////
//	edsncase.CPP
// 
//	Created by
//		GeorgeCh
//
//	Description : 
//		Editor Sniff 01 
//	Date		Person		Comments
//	--------	---------	-----------
//	6/26/95		mikepie		Smart Indent added as an UNDOable
//							command. Modified sniff accordingly.
//				


#include "stdafx.h"
#include "edsncase.h"
#include "support.h"
// #include "emulcase.h"


#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

#define NOTCARE 0
#define WINDOUT 1					 
#define FILEOUT	2
#define NUMERAL 1
#define TEXTUAL 2
#define ARGLINE 3

#define SMART_INDENT_ON  1
#define SMART_INDENT_OFF 0

void SmartIndentMode(int mode)
{
	UIOptionsTabDlg tabdlg ;
	
	COSource  sourc;
	sourc.Create() ; 
	tabdlg.ShowTabsTab() ;
	MST.DoKeys("{TAB}");	// Move focus into window

	if (mode == SMART_INDENT_ON)
	{		  
		tabdlg.SetAutoIndentMode(SMART);	// S(m)art Indent
		//tabdlg.SetSmartIndentOption(CLOSING,100);	// Indent (C)losing brace on
	}
	else
	{
			tabdlg.SetAutoIndentMode(DEFAULT); // (D)efault
	}

	MST.DoKeys("{ENTER}");	// OK
	MST.DoKeys("^({F4})");	// Close new window
}	

UICustomizeTabDlg TCT;

static BOOL FirstTest = TRUE;


IMPLEMENT_TEST(SmartIndent, CTest, "Smart Indent", -1, EditorSniff)
void SmartIndent::Run(void)
{
	COSource  sourc;
	SmartIndentMode(SMART_INDENT_ON);
	
	sourc.Create();

	PoundCases();
	Code1();
	Class1();
	Code2();
	Strings();
	MultiLineComments();
	CaseSwitch();

	int removeold = 0;
	removeold = remove(m_strCWD + "testout\\SmartOut.cpp");	
	if (removeold != 0)
		m_pLog->RecordInfo("Couldn't remove %stestout\\SmartOut.cpp",m_strCWD);
	
	UIWB.SaveFileAs(m_strCWD + "testout\\SmartOut.cpp") ;

	int iresult = 0;
	m_pLog->RecordInfo("Comparing test output to %sbaseln\\SmartOk.cpp...",m_strCWD);
	iresult = CompareFiles(m_strCWD + "testout\\SmartOut.cpp",m_strCWD +"baseln\\SmartOk.cpp",1);

	m_pLog->RecordCompare(iresult == CF_SAME, 
		"Compare of output file %stestout\\SmartOut.cpp.",m_strCWD);

	// Close the window
	//MST.DoKeys("^{F4}");
	UIWB.CloseAllWindows();
	
}

BOOL SmartIndent::PoundCases()
{
CString temp;
	temp.LoadString(IDS_STRING1); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING2); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING3); MST.DoKeys(temp); 
	temp.LoadString(IDS_STRING4); MST.DoKeys(temp);
	
	return(TRUE);
}

BOOL SmartIndent::Code1()
{
	CString temp;
	temp.LoadString(IDS_STRING5); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING6); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING7); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING8); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING9); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING10); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING11); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING12); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING13); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING14); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING15); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING16); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING17); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING18); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING19); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING20); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING21); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING22); MST.DoKeys(temp);

	return(TRUE);
}

BOOL SmartIndent::Class1()
{
	CString temp;
	temp.LoadString(IDS_STRING23); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING24); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING25); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING26); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING27); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING28); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING29); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING30); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING31); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING32); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING33); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING34); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING35); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING36); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING37); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING38); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING39); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING40); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING41); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING42); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING43); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING44); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING45); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING46); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING47); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING48); MST.DoKeys(temp);
	return(TRUE);
}

BOOL SmartIndent::Code2()
{
	CString temp;
	temp.LoadString(IDS_STRING49); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING50); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING51); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING52); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING53); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING54); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING55); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING56); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING57); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING58); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING59); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING60); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING61); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING62); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING63); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING64); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING65); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING66); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING67); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING68); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING69); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING70); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING71); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING72); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING73); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING74); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING75); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING76); MST.DoKeys(temp);
	
	return(TRUE);
}

BOOL SmartIndent::Strings()
{
	CString temp;
	temp.LoadString(IDS_STRING77); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING78); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING79); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING80); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING81); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING82); MST.DoKeys(temp);
	
	return(TRUE);
}

BOOL SmartIndent::MultiLineComments()
{
	CString temp;
	temp.LoadString(IDS_STRING83); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING84); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING85); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING86); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING87); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING88); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING89); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING90); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING91); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING92); MST.DoKeys(temp);
		
	return(TRUE);
}

BOOL SmartIndent::CaseSwitch()
{
	CString temp;
	temp.LoadString(IDS_STRING93); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING94); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING95); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING96); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING97); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING98); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING99); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING100); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING101); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING102); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING103); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING104); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING105); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING106); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING107); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING108); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING109); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING110); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING111); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING112); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING113); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING114); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING115); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING116); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING117); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING118); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING119); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING120); MST.DoKeys(temp);
	temp.LoadString(IDS_STRING121); MST.DoKeys(temp);

	return(TRUE);
}


IMPLEMENT_TEST(EdsnifSelection, CTest, "Selection Tests", -1, EditorSniff)
void EdsnifSelection::Run(void)
{

	COSource  sourc;
	COSource  sourc2;

	SmartIndentMode(SMART_INDENT_OFF);
	
	// copy over writable versions of some baseline files
	if (FirstTest)
	{
		KillAllFiles("TESTOUT", FALSE);
		sourc.Open(m_strCWD + "baseln\\EdBase0.cpp" );
		sourc.SaveAs(m_strCWD + "testout\\EdBase0.cpp", TRUE);
		sourc.Close();
    }
	FirstTest = FALSE;

	sourc2.Open(m_strCWD + "baseln\\EdBase1.cpp" );
	sourc2.SaveAs(m_strCWD + "testout\\EdBase1.cpp", TRUE);
	sourc2.Close();

	// CutCpyDelPst01( );	// stream cut, paste, undo, redo
	// CutCpyDelPst02( );  // stream copy, paste, undo, redo
	CutCpyDelPst03( );
	// CutCpyDelPst04( ); CAFE problems prevent use of this test yet

}

IMPLEMENT_TEST(EdsnifTabIndent, CTest, "Tab/Indent tests", -1, EditorSniff)

void EdsnifTabIndent::Run(void)
{
	COSource  sourc;

	SmartIndentMode(SMART_INDENT_OFF);

	// Needs to be set again here for looping
	sourc.SelectEmulations(EMULATE_VC2);

	if (FirstTest)
	{
		KillAllFiles("TESTOUT", FALSE);
		sourc.Open(m_strCWD + "baseln\\EdBase0.cpp" );
		sourc.SaveAs(m_strCWD + "testout\\EdBase0.cpp", TRUE);
		sourc.Close();
    }
	FirstTest = FALSE;

	EnterTabs01( );     // enter tabs 	// Wait for new drop to reenable

}

IMPLEMENT_TEST(EdsnifOvrInsert, CTest, "Overtype/Insert tests", -1, EditorSniff)

void EdsnifOvrInsert::Run(void)
{

	SmartIndentMode(SMART_INDENT_OFF);

	COSource  sourc;
	
	// Needs to be set again here for looping
	sourc.SelectEmulations(EMULATE_VC2);

	if (FirstTest)
	{
		KillAllFiles("TESTOUT", FALSE);
		sourc.Open(m_strCWD + "baseln\\EdBase0.cpp" );
		sourc.SaveAs(m_strCWD + "testout\\EdBase0.cpp", TRUE);
		sourc.Close();
    }
	FirstTest = FALSE;

	OvrInsert01( );		// Overtype and insert modes

}

IMPLEMENT_TEST(EdsnifToolsMenu, CTest, "Tools menu tests", -1, EditorSniff)

void EdsnifToolsMenu::Run(void)
{
	SmartIndentMode(SMART_INDENT_OFF);
	
	COProject proj1;
	CString hellofiles;

	hellofiles = "copy " + m_strCWD + "baseln\\hello.* " + m_strCWD + "testout";

	if (FirstTest)
		KillAllFiles("TESTOUT", FALSE);
    FirstTest = FALSE;

	system (hellofiles);

	proj1.Open(m_strCWD + "testout\\hello.mak" );
// TODO UIProjOptionsTabDlg::SetDebugOption when Enrique implements it
	MST.DoKeys("%PS");
	//MST.DoKeys("{RIGHT}");
	MST.DoKeys("^({TAB})");
	MST.DoKeys("%U");
	MST.DoKeys("Argument");
	MST.DoKeys("{ENTER}");
	proj1.Close();

	General();

}

 // create source with jagged right edge for selection tests
 
BOOL EdsnifSelection::CutCpyDelPst01( )
{
	COSource  sourc;
	COSource  sourc2;
	COSource  sourc3;
	int iresult= 0;

	// SetTestName("CCDP01_2");
	// UNDO and compare, 
	// UNDO buffer lost when file is saved so must restore and cut first
    // This is no longer true - streamline
	sourc2.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc2.SelectText( 1,2,118,2 );
	UIWB.DoCommand(ID_EDIT_CUT,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	sourc2.SaveAs(m_strCWD + "testout\\CCDP01_3.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP01_3.cpp",m_strCWD + "baseln\\EdBase1.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP01_3/EdBase1.cpp REDO Stream Cut result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO Stream Cut result = %d", iresult);
	sourc2.Close();

return(TRUE);
}

BOOL EdsnifSelection::CutCpyDelPst02( )
{
	COSource  sourc;
	COSource  sourc2;
	COSource  sourc3;
	COSource  sourc4;
	int iresult= 0;

	// UNDO paste and compare
	// SetTestName("CCDP02_5");
	sourc3.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc3.SelectText( 1,2,120,22 );
	UIWB.DoCommand(ID_EDIT_COPY,DC_MENU);
	sourc3.SetCursor ( 1,2 );
	UIWB.DoCommand(ID_EDIT_PASTE,DC_MENU);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_MENU);
	UIWB.DoCommand(ID_EDIT_REDO,DC_MENU);
	sourc3.SaveAs(m_strCWD + "testout\\CCDP02_6.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP02_6.cpp",m_strCWD + "baseln\\EdBase0.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP02_6/EdBase0 REDO Stream Paste result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO Stream Paste result = %d", iresult);
	sourc3.Close();

	return(TRUE);
}

BOOL EdsnifSelection::CutCpyDelPst03( )
{
	COSource  sourc;
	COSource  sourc2;
	COSource  sourc3;
	int iresult= 0;
	 
	// UNDO and compare  TODO change this to a shortcut
	// SetTestName("CCDP03_8");
	sourc2.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc2.SelectText( 50,50,10,10 );
	UIWB.DoCommand(ID_EDIT_CLEAR,DC_MNEMONIC);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	sourc3.SaveAs(m_strCWD + "testout\\CCDP03_9.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP03_9.cpp",m_strCWD + "baseln\\EdBase9.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP03_9/EdBase9 REDO Stream Delete result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO Stream Delete result = %d", iresult);
	sourc3.Close();
	
return(TRUE);
}

BOOL EdsnifSelection::CutCpyDelPst04( )
{

	COSource  sourc;
	COSource  sourc2;
	COSource  sourc3;
	COSource  sourc4;
	COSource  sourc5;
	COSource  sourc6;
	int iresult= 0;
	 
	// Select a column, CUT, compare to baseline
	// SetTestName("CCDP04_A");
	sourc.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc.SetCursor( 3, 4 );
	MST.DoKeys(ID_EDIT_COLUMN); // enter column mode
	sourc.SetCursor( 90, 70 );
	UIWB.DoCommand(ID_EDIT_CUT,DC_ACCEL);
	sourc.SaveAs(m_strCWD + "testout\\CCDP04_A.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_A.cpp",m_strCWD + "baseln\\EdBase10.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_A/EdBase10 Column Select and Cut result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED Column Select and Cut result = %d", iresult);

	// UNDO and compare
	// SetTestName("CCDP04_B");
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	sourc.SaveAs(m_strCWD + "testout\\CCDP04_B.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_B.cpp",m_strCWD +"testout\\EdBase0.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_B/EdBase0 UNDO Column Cut result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED UNDO Column Cut result = %d", iresult);

	// SetTestName("CCDP04_C");
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	sourc.SaveAs(m_strCWD + "testout\\CCDP04_C.cpp", TRUE);
	sourc.Close();
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_C.cpp",m_strCWD + "baseln\\EdBase12.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_C/EdBase12 REDO Column Cut result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO Column Cut result = %d", iresult);

	// now paste in the original cut and compare
	// SetTestName("CCDP04_D");
	sourc4.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc4.SetCursor( 3, 4 );
	MST.DoKeys(ID_EDIT_COLUMN); // enter column mode
	sourc4.SetCursor( 90, 70 );
	UIWB.DoCommand(ID_EDIT_CUT,DC_ACCEL);
	sourc4.SetCursor( 3, 4 );
	UIWB.DoCommand(ID_EDIT_PASTE,DC_ACCEL);
	sourc4.SaveAs(m_strCWD + "testout\\CCDP04_D.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_D.cpp",m_strCWD +"testout\\EdBase0.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_D/EdBase0 Column Paste result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED Column Paste result = %d", iresult);
	 
	// UNDO paste and compare						
	// SetTestName("CCDP04_E");
	sourc4.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc4.SetCursor( 3, 4 );
	MST.DoKeys(ID_EDIT_COLUMN); // enter column mode
	sourc4.SetCursor( 90, 70 );
	UIWB.DoCommand(ID_EDIT_CUT,DC_ACCEL);
	sourc4.SetCursor( 3, 4 );
	UIWB.DoCommand(ID_EDIT_PASTE,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	sourc4.SaveAs(m_strCWD + "testout\\CCDP04_E.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_E.cpp",m_strCWD + "baseln\\EdBase12.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_E/EdBase12 UNDO Column Paste result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED UNDO Column Paste result = %d", iresult);

	// REDO paste and compare
	// SetTestName("CCDP04_F");
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	sourc4.SaveAs(m_strCWD + "testout\\CCDP04_F.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\CCDP04_F.cpp",m_strCWD +"testout\\EdBase0.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED CCDP04_F/EdBase0 REDO Column Paste result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO Column Paste result = %d", iresult);
	sourc4.Close();
	 
	return(TRUE);
}


BOOL EdsnifTabIndent::EnterTabs01( )
{

	COSource  sourc2;
	COSource  sourc3;
	COSource  sourc4;
	COSource  sourc5;
	COSource  sourc6;
	COSource  sourc7;
	COSource  sourc8;
	COSource  sourc9;
	COSource  sourc10;
	int iresult= 0;

	// Open file copy text, insert text, compare to baseline
	sourc2.Create();
	
	CString strTest("\ttabs\t\tspaces     more tabs\t\t\tend.");
	CString strTest2("No leading tabs\t\tjust here.");
	CString strClip;
	CString strEmpty("");
	CString strTwoTabs("\t\t");

	// SetTestName("ET01_1");
	EmptyClip();
	MST.DoKeys( strTest );	
	UIWB.DoCommand( ID_EDIT_SELECT_ALL,DC_MENU );
	UIWB.DoCommand( ID_EDIT_CUT,DC_MENU );
	GetClipText( strClip );
	if (strClip != strTest) 	  // see if tabs are entered correctly
	{
		m_pLog->RecordFailure("FAILED ET01_1 Enter tabs");
	}
	else m_pLog->RecordInfo("PASSED Enter tabs 1");
	
	// SetTestName("ET01_2");
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( "{HOME}{HOME}" );
	MST.DoKeys( strTest );
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest );
	MST.DoKeys( strTest );

	MST.DoKeys( "{HOME}{HOME}" );
	MST.DoKeys( strTest2 );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest2 );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( "{HOME}{HOME}" );
	MST.DoKeys( strTest2 );
	MST.DoKeys( strTest );
	MST.DoKeys( "{ENTER}" );
	MST.DoKeys( strTest2 );

	Sleep(1000);
	sourc2.SaveAs(m_strCWD + "testout\\ET01_2.cpp", TRUE);
	Sleep(1000);
	iresult = CompareFiles(m_strCWD + "testout\\ET01_2.cpp",m_strCWD + "baseln\\EdBase21.cpp",1);
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED ET01_2/EdBase21 Enter tabs result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED Enter tabs 2");
	sourc2.Close();


    // UNDO Indent
	// SetTestName("ET01_7");
	sourc7.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc7.SelectText( 10,10,51,84 );
	sourc7.TypeTextAtCursor( "{TAB}{TAB}{TAB}" );
	
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
	
	sourc7.SaveAs(m_strCWD + "testout\\ET01_7.cpp", TRUE);
	
	iresult = CompareFiles(m_strCWD + "testout\\ET01_7.cpp",m_strCWD + "baseln\\EdBase0.cpp",1);
	
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED ET01_7/EdBase0 Undo unindent result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED Undo unindent result = %d", iresult);

    // REDO Indent
	// SetTestName("ET01_8");
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
	Sleep(1000);

	sourc7.SaveAs(m_strCWD + "testout\\ET01_8.cpp", TRUE);

	iresult = CompareFiles(m_strCWD + "testout\\ET01_8.cpp",m_strCWD + "baseln\\EdBase22.cpp",1);

	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED ET01_8/EdBase22 Redo Unindent result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED Redo Unindent result = %d", iresult);

	sourc7.Close();

	return(TRUE);
}


BOOL EdsnifOvrInsert::OvrInsert01( )
{
	COSource  sourc1;
	COSource  sourc2;
	COSource  sourc3;
	COSource  sourc4;
	COSource  sourc5;
	COSource  sourc6;
	int iresult= 0;
	int i;
	
	CString strTest1("qwertyuiop 1234567890 asdfghjkl zxcvbnm QWERTYUIOP ASDFGHJKL ZXCVBNM");  // 13
	CString strTest2("tab\t backspace tab\t quote\'double quote\" backslash\\ question\?");  // 13
	CString strTest3("!@#$&*_`-= ;:| <>,./");												   // 5
	//   ~^+%[]{}()   These chars have special MST usage don't use without {}
	// Open file, Overtype text, compare to baseline
	// SetTestName("OI01_1");

	// Open file, Insert text, UNDO, REDO, compare to baseline
	// SetTestName("OI01_6");
	sourc6.Open(m_strCWD + "testout\\EdBase0.cpp" );
	sourc6.SetCursor( 66,17 );
	sourc6.TypeTextAtCursor( strTest1 );
	sourc6.SetCursor( 76,22 );
	sourc6.TypeTextAtCursor( strTest2 );
	sourc6.SetCursor( 28,2 );
	sourc6.TypeTextAtCursor( strTest3 );

	for (i=0;i<32;i++) {	// total number of undos for three strings
		UIWB.DoCommand(ID_EDIT_UNDO,DC_ACCEL);
		Sleep(100);
	}

	Sleep(2000);
	
	if (UIWB.IsCommandEnabled(ID_EDIT_UNDO))
	{
		 m_pLog->RecordFailure("FAILED OI01_6 UNDO still enabled",0);		 
	}
	
	for (i=0;i<32;i++) {	// total number of undos for three strings
		UIWB.DoCommand(ID_EDIT_REDO,DC_ACCEL);
		Sleep(100);
	}

	Sleep(2000);
	
	if (UIWB.IsCommandEnabled(ID_EDIT_REDO))
	{
		 m_pLog->RecordFailure("FAILED OI01_6 REDO still enabled",0);		 
	}

	sourc6.SaveAs(m_strCWD + "testout\\OI01_6.cpp", TRUE);
	iresult = CompareFiles(m_strCWD + "testout\\OI01_6.cpp",m_strCWD + "baseln\\EdBase34.cpp",1);
	
	if (iresult != CF_SAME)
	{
		m_pLog->RecordFailure("FAILED OI01_6/EdBase34 REDO typing result = %d", iresult);
	}
	else m_pLog->RecordInfo("PASSED REDO typing result = %d", iresult);
	sourc6.Close();

	return(TRUE);
}



BOOL EdsnifToolsMenu::AddTool(CString strToolPath,
							 CString strArguments,
							 CString strMenuText,
 							 int     OutputForm)
{
/*	TCT.ShowPage(TAB_TOOLS,3);	            // Tools.Customize.Tools
	MST.DoKeys("%A");						// select Add
	MST.DoKeys(strToolPath);
	MST.DoKeys("{ENTER}");					// OK
	MST.DoKeys("%n");						// select arguments
	MST.DoKeys(strArguments);
	if (OutputForm == WINDOUT)              
		MST.DoKeys("%w");					// redirect to output
 	MST.DoKeys("%e");						// Close window on exit
	MST.DoKeys("%m");						// select Menu Text
	MST.DoKeys(strMenuText);
	MST.DoKeys("{ENTER}");					// OK
	*/
COWorkSpace CwkSpc ;
	CwkSpc.AddTool(strMenuText,strToolPath,strArguments,m_strCWD,0,OutputForm,0);
	return TRUE;
}


BOOL EdsnifToolsMenu::SaveOutputWindow(CString strOutputName)
{	
	UIOutput OutWin;
	OutWin.Activate();
	UIFileSaveAsDlg SaveOutWin = UIWB.FileSaveAsDlg();
	EXPECT( SaveOutWin.IsValid() );
	SaveOutWin.SetName(strOutputName);
	SaveOutWin.OK(TRUE);
	return TRUE;
}

BOOL EdsnifToolsMenu::Verify(CString strTestName,
							CString strArguments,
							CString strMenuText,
							CString strShortCut,
							int     CompareMethod,
							int     OutputForm,
							BOOL	FILEOPEN)
{
	CString KeysToDo = "%T" + strShortCut;
	CString strToolPath("switches.exe");
	CString strToolOut = m_strCWD + "testout\\switches.out";
	CString strToolExe = m_strCWD + "switches.exe";
	CString strOutputName = m_strCWD + "testout\\tools" + strShortCut + ".out";
	CString strSwitchName = m_strCWD + "testout\\switch" + strShortCut + ".out";
	CString strTestFile;
	int iresult= 0;	
	int cresult= 0;	// column 
	int lresult= 0;	// line
	COSource copy1;

	KillFile(NULL,strToolOut,TRUE);		   // switches.out at this point
	// execute Tool
	// TODO check menu for tool before executing
	MST.DoKeys(KeysToDo);
	// TODO check for switches.out before proceeding
	Sleep(100);  // just to make sure switches.exe has time to save file

	// decide which file will be used for test output
	if (CompareMethod == WINDOUT) // save output window and use it
	{
		SaveOutputWindow(strOutputName);
		Sleep(40); 
	    strTestFile = strOutputName;
	}
	else // OutputForm == FILEOUT  use file written directly by tool
	{
		copy1.Open(strToolOut);
		copy1.SaveAs(strSwitchName);
		copy1.Close();
// TODO need to fail if file can't be opened
		strTestFile = strSwitchName;
	}

	// first search for any "$" and fail if there is one
	iresult =  FindStrInFile(strTestFile,"$",1);
	if (iresult != 0)
	{
		 m_pLog->RecordFailure("FAILED Probable translated macro = %d", iresult);
		return FALSE;
	}
	else  // added with following cutour
	{
		m_pLog->RecordInfo("PASSED Add and run Tool", 1);
		return(TRUE);
	}
/*  Too much trouble for sniff test, just hit it once
	if (FILEOPEN == FALSE)
	{
		// search for "comstring:" only, no random data
      	iresult = CompareFiles(strTestFile,m_strCWD + "baseln\\swstring.out",1);
		if (iresult != CF_SAME) 
		{
			m_pLog->RecordFailure("FAILED comstring contains trash = %d", iresult);
			return FALSE;
		}
	}
	else switch (OutputForm)
		{
			case NUMERAL: // look for Line:6 and Col:23
					lresult =  FindStrInFile(strTestFile,"L:6",1);
					cresult =  FindStrInFile(strTestFile,"C:23",1);
					iresult = lresult+cresult;
					if ((cresult == 0) && (lresult == 0))
					{
						 m_pLog->RecordFailure("FAILED Line or Column number incorrect = %d", iresult);
						return FALSE;
					}
					else
					{
						m_pLog->RecordInfo("PASSED L/C Add and run Tool", 0);
						return TRUE;
					}
				break;	
			case TEXTUAL: // look for selected word "value"
					iresult =  FindStrInFile(strTestFile,"value",1);
					if (iresult == 0)
					{
						m_pLog->RecordFailure("FAILED Curtext incorrect = %d", iresult);
						return FALSE;
					}
					else
					{
						m_pLog->RecordInfo("PASSED CT Add and run Tool", 0);
						return TRUE;
					}
				break;
			case ARGLINE: // look for selected word "Argument"
					iresult =  FindStrInFile(strTestFile,"Argument",1);
					if (iresult == 0)
					{
						m_pLog->RecordFailure("FAILED Cmdline incorrect = %d", iresult);
						return FALSE;
					}
					else
					{
						m_pLog->RecordInfo("PASSED CL Add and run Tool", 0);
						return TRUE;
					}
				break;
// TODO find out the path from coprject.h GetDir() and build the paths
			case NOTCARE: // can't use hard coded paths for portable test 
				iresult = CompareFiles(strTestFile,m_strCWD,1);
				if (iresult != CF_SAME)
				{
					m_pLog->RecordFailure("FAILED Path Add and run Tool = %d", iresult);
					return FALSE;
				}
				else
				{
					m_pLog->RecordInfo("PASSED PP Add and run Tool", 0);
					return TRUE;
				}
				break;
		}

	m_pLog->RecordInfo("FAILED Unknown Add and run Tool", 1);
	return(FALSE);
  */

  }

BOOL EdsnifToolsMenu::General( )
{
	COSource outwind;
	COSource outfile;
	COSource source1;
	COProject proj1;
//	CString strBaseName = m_strCWD + "baseln\\base" + strShortCut + ".out";
	CString strToolOut = m_strCWD + "switches.out";
	CString strToolExe = m_strCWD + "switches.exe";

		// SetTestName(strTestName);
	KillFile(NULL,strToolOut,TRUE);		   // switches.out at this point

	proj1.Open(m_strCWD + "testout\\hello.mak" );
	source1.Open(m_strCWD + "testout\\hello.cpp");
//	source1.SelectText( 6,18,6,23 ); // select word "value" 

	// must close whetever else is open before starting
	AddTool(strToolExe,"${(}FilePath{)}",            "File    &1",NOTCARE);
	AddTool(strToolExe,"${(}FileName)}",			 "FileName&2",NOTCARE);
	AddTool(strToolExe,"${(}FileExt)}",				 "FileExt &5",NOTCARE);
	AddTool(strToolExe,"${(}WkspDir{)}${(}WkspName)","Proj    &4",NOTCARE);
	AddTool(strToolExe,"${(}TargetPath{)}",          "Target  &7",NOTCARE);
//	AddTool(strToolExe,"${(}TargetName)}",			 "TarName &j",NOTCARE);
//	AddTool(strToolExe,"${(}TargetExt)}",			 "TarExt  &k",NOTCARE);
//	AddTool(strToolExe,"${(}TargetArgs)}",			 "TarArgs &l",NOTCARE);
	AddTool(strToolExe,"${(}TargetDir)}",			 "TarDir  &3",NOTCARE);
	AddTool(strToolExe,"${(}CurLine)}",				 "CurLine &6",NOTCARE);
	AddTool(strToolExe,"${(}CurCol{)}",              "Column  &8",NUMERAL);
	AddTool(strToolExe,"${(}CurText)}",				 "CurText &9",NOTCARE);
	AddTool(strToolExe,"${(}CurDir)}",				 "CurDir  &0",NOTCARE);
																 
	Verify("Tools01","${(}FilePath{)}",            "File   &1","1",FILEOUT,NOTCARE,TRUE);
	Verify("Tools04","${(}WkspDir{)}${(}WkspName)","Proj   &4","4",FILEOUT,NOTCARE,TRUE);
	Verify("Tools07","${(}TargetPath{)}",          "Target &7","7",FILEOUT,NOTCARE,TRUE);
//	Verify("Tools08","${(}CurCol{)}",              "Column &8","8",WINDOUT,NUMERAL,FALSE);

	source1.Close();
	proj1.Close();

	return(TRUE);
}


IMPLEMENT_TEST(cVC2Position, CTest, "VC2 Position Emulation tests", -1, EditorSniff)

void cVC2Position ::Run(void)
{
	if (FirstTest)
		KillAllFiles(m_strCWD + "TESTOUT", FALSE);
    FirstTest = FALSE;

	SmartIndentMode(SMART_INDENT_OFF);
	VC2Position( );
};

IMPLEMENT_TEST(cVC3Position, CTest, "VC3 Position Emulation tests", -1, EditorSniff)

void cVC3Position ::Run(void)
{
	if (FirstTest)
		KillAllFiles(m_strCWD + "TESTOUT", FALSE);
    FirstTest = FALSE;

	SmartIndentMode(SMART_INDENT_OFF);
	VC3Position( );
};

IMPLEMENT_TEST(cBriefPosition, CTest, "Brief Position Emulation tests", -1, EditorSniff)

void cBriefPosition ::Run(void)
{
	if (FirstTest)
		KillAllFiles(m_strCWD + "TESTOUT", FALSE);
    FirstTest = FALSE;

	SmartIndentMode(SMART_INDENT_OFF);
	BriefPosition( );
};

IMPLEMENT_TEST(cEpsilonPosition, CTest, "Epsilon Position Emulation tests", -1, EditorSniff)
 
void cEpsilonPosition ::Run(void)
{
	if (FirstTest)
		KillAllFiles(m_strCWD + "TESTOUT", FALSE);
    FirstTest = FALSE;

	SmartIndentMode(SMART_INDENT_OFF);
	EpsilonPosition( );
};

/*
IMPLEMENT_TEST(EdsnifEmulations, CTest, "Emulations Base Class", -1, EditorSniff)

// since this class is a base class for tests, it must have a ctor
// that takes all test parameters for proper base class initialization
Emulations::Emulations(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
*/

BOOL PositionVerify(COSource testSource,
					 int testLine1,
					 int testCol1,
					 int baseLine1,
					 int baseCol1,
					 LPCTSTR KeysToDo)
{
	int iresult= 0;	
	int CurrentLine = 0;
	int CurrentCol = 0;

	testSource.AttachActiveEditor();
	if ((testLine1 > 0)&&(testCol1 > 0)) 
		testSource.SetCursor(testLine1,testCol1);  // else stay put
	MST.DoKeys(KeysToDo);
	CurrentLine = testSource.GetCurrentLine();
	CurrentCol = testSource.GetCurrentColumn();

// Set the position so if the check below fails, we'll still be in the right place for the
// next command.
	testSource.SetCursor(baseLine1,baseCol1);

	if ((CurrentLine != baseLine1) && (CurrentCol != baseCol1)) 	
	{
		LOG->RecordFailure("FAILED %s  Base L:%d C:%d Current L:%d C:%d",KeysToDo,baseLine1,baseCol1,CurrentLine,CurrentCol);  //debug
	}
	else LOG->RecordInfo("PASSED %s",KeysToDo);
	return(TRUE);
}


BOOL PositionSetup(COSource testSource,
				   int testLine1,
				   int testCol1,
				   LPCTSTR KeysToDo)
{
// This function exists only as a dummy to setup the position for the following command
// We don't know what the final position will be
	testSource.AttachActiveEditor();
	testSource.SetCursor(testLine1,testCol1);
	MST.DoKeys(KeysToDo);

	return(TRUE);
}

BOOL LocationVerify(COSource testSource,
							 int baseLine1,
							 int baseCol1)
{
	int iresult= 0;	
	int CurrentLine = 0;
	int CurrentCol = 0;

	testSource.AttachActiveEditor();

	CurrentLine = testSource.GetCurrentLine();
	CurrentCol = testSource.GetCurrentColumn();

	if ((CurrentLine != baseLine1) && (CurrentCol != baseCol1)) 	
	{
		LOG->RecordFailure("FAILED Base L:%d C:%d Current L:%d C:%d",baseLine1,baseCol1,CurrentLine,CurrentCol);  //debug
		MST.DoKeys("{ESC 2}");
		testSource.SetCursor(baseLine1,baseCol1);
	    MST.DoKeys("{RIGHT}");

		return(FALSE);  // move on to next test
	}  
	else LOG->RecordInfo  ("PASSED Base L:%d C:%d",baseLine1,baseCol1);  //debug

	return(TRUE);
}

BOOL cBriefPosition::BriefPosition( )
{
	COSource t;
	COSource b;
	t.Open(m_strCWD + "baseln\\baseposi.cpp");
	t.SelectEmulations(EMULATE_BRIEF);

//	PositionVerify(testSource,testLine1,testCol1,baseLine1,baseCol1,KeysToDo)

 	PositionVerify(t,95,37,95,27,KeyCommand[CursorBackWord][EMULATE_BRIEF]);
 	PositionVerify(t,140,75,1,1,KeyCommand[CursorBeginDocument][EMULATE_BRIEF]);
 	PositionVerify(t,6,1,6,23,KeyCommand[CursorBeginLine][EMULATE_BRIEF]);

 	PositionVerify(t,7,1,7,66,KeyCommand[CursorBriefEnd][EMULATE_BRIEF]);
 	PositionVerify(t,8,43,8,1,KeyCommand[CursorBriefHome][EMULATE_BRIEF]);
 	PositionVerify(t,137,52,138,52,KeyCommand[CursorDown][EMULATE_BRIEF]);
 	PositionVerify(t,137,52,183,1,KeyCommand[CursorEndDocument][EMULATE_BRIEF]);
 	PositionVerify(t,2,1,2,23,KeyCommand[CursorEndLine][EMULATE_BRIEF]);
 	PositionVerify(t,2,1,3,1,KeyCommand[CursorForwardWord][EMULATE_BRIEF]);
 	PositionVerify(t,2,81,3,1,KeyCommand[CursorForwardWord][EMULATE_BRIEF]);
 	PositionVerify(t,182,2,183,1,KeyCommand[CursorForwardWord][EMULATE_BRIEF]);
 	PositionVerify(t,183,1,183,1,KeyCommand[CursorForwardWord][EMULATE_BRIEF]);
 	PositionVerify(t,92,66,92,9,KeyCommand[CursorHome][EMULATE_BRIEF]);

	t.SelectEmulations(EMULATE_VC3); // leave in known state
	t.AttachActiveEditor();
	t.SaveAs(m_strCWD + "testout\\briefpos.cpp", TRUE);
	t.Close();
	return(TRUE);
}


BOOL cVC2Position::VC2Position( )
{
	COSource t;
	t.Open(m_strCWD + "baseln\\baseposi.cpp");
	t.SelectEmulations(EMULATE_VC2);

 	PositionVerify(t,92,46,89,1,KeyCommand[BackEpsilonSentence][EMULATE_VC2]);
 	PositionVerify(t,95,37,95,27,KeyCommand[CursorBackWord][EMULATE_VC2]);
 	PositionVerify(t,140,75,1,1,KeyCommand[CursorBeginDocument][EMULATE_VC2]);
 	PositionVerify(t,137,52,138,52,KeyCommand[CursorDown][EMULATE_VC2]);
 	PositionVerify(t,137,52,183,1,KeyCommand[CursorEndDocument][EMULATE_VC2]);
 	PositionVerify(t,2,1,2,23,KeyCommand[CursorEndLine][EMULATE_VC2]);
 	PositionVerify(t,2,1,2,81,KeyCommand[CursorForwardWord][EMULATE_VC2]);

	t.SelectEmulations(EMULATE_VC3); // leave in known state
	t.AttachActiveEditor();
	t.SaveAs(m_strCWD + "testout\\vc2posi.cpp", TRUE);
	t.Close();
	return(TRUE);
}

BOOL cVC3Position::VC3Position( )
{
	COSource t;
	COSource b;
	t.Open(m_strCWD + "baseln\\baseposi.cpp");
	t.SelectEmulations(EMULATE_VC3);

 	// If these two don't pass, VC3 emulation is not selected
	// pass only if virtual space if off
 	PositionVerify(t,6,1,5,2,KeyCommand[CursorLeft][EMULATE_VC3]);   
	// Nobody but VC uses this stupid syntax
	// Nore has such compiler problems	
	
//    t.Find("\\{\\{\".*\"\\}\\!\\{\'.*\'\\}\\}",FALSE,FALSE,TRUE);     

	PositionVerify(t,95,37,95,27,KeyCommand[CursorBackWord][EMULATE_VC3]);
 	PositionVerify(t,140,75,1,1,KeyCommand[CursorBeginDocument][EMULATE_VC3]);
 	PositionVerify(t,137,52,138,52,KeyCommand[CursorDown][EMULATE_VC3]);
 	PositionVerify(t,137,52,183,1,KeyCommand[CursorEndDocument][EMULATE_VC3]);
 	PositionVerify(t,2,1,2,23,KeyCommand[CursorEndLine][EMULATE_VC3]);
 	PositionVerify(t,2,1,2,81,KeyCommand[CursorForwardWord][EMULATE_VC3]);
	   
 	PositionSetup(t,3,12,KeyCommand[CursorBeginDocument][EMULATE_VC3]);
	t.AttachActiveEditor();
    t.Find("NULL");
    LocationVerify(t,59,37);
 	PositionSetup(t,59,37,KeyCommand[FindNext][EMULATE_VC3]);
    LocationVerify(t,59,50);
 	PositionSetup(t,59,46,KeyCommand[FindPrev][EMULATE_VC3]);
    LocationVerify(t,59,37);

	// Test reverse search 
	t.Find("ClearScreen");
	m_pLog->RecordInfo("Testing backword search");
	MST.DoKeys("+{F3}");
	
	LocationVerify(t,56,16);
	// End reverse search test.
		   
	t.SaveAs(m_strCWD + "testout\\vc3posi.cpp", TRUE);
	t.Close();
	return(TRUE);
}


BOOL cEpsilonPosition::EpsilonPosition( )
{
	COSource t;
	COSource b;
	t.Open(m_strCWD + "baseln\\baseposi.cpp");
	t.SelectEmulations(EMULATE_EPSILON);

// Order of following pairs must be maintained for End/BeginWindow to work in test
 	PositionVerify(t,140,75,1,1,KeyCommand[CursorBeginDocument][EMULATE_EPSILON]);
 	PositionVerify(t,4,35,1,1,KeyCommand[CursorBeginWindow][EMULATE_EPSILON]);
 	PositionVerify(t,166,22,183,1,KeyCommand[CursorEndDocument][EMULATE_EPSILON]);
 	PositionVerify(t,166,26,183,1,KeyCommand[CursorEndWindow][EMULATE_EPSILON]);

 	PositionSetup(t,98,1,KeyCommand[CursorPageUp][EMULATE_EPSILON]);
 	PositionVerify(t,-1,-1,98,1,KeyCommand[CursorPageDown][EMULATE_EPSILON]);
 	PositionSetup(t,77,1,KeyCommand[CursorPageDown][EMULATE_EPSILON]);
 	PositionVerify(t,-1,-1,77,1,KeyCommand[CursorPageUp][EMULATE_EPSILON]);

	m_pLog->RecordInfo("END Epsilon Position Verify Tests");

	t.SelectEmulations(EMULATE_VC3); // leave in known state
	t.AttachActiveEditor();
	t.SaveAs(m_strCWD + "testout\\epsiposi.cpp", TRUE);
	t.Close();
	return(TRUE);
}


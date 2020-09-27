//////////////////////////////////////////////////////////////////////////////
//	RESCASES.CPP
//
//	Created by :			Date :
//		DavidGa					2/23/94
//
//	Description :
//		Implementation of the DBCS Resource testcase classes
//

#include "stdafx.h"
#include "rescases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

extern ImeInit;

#define OPEN_IME \
	IME.Open(); \
	Sleep(500); \
	(ImeInit == FALSE ? (IME.SetConversionMode(3),ImeInit=TRUE):0) ; \
/*
	IME.Open(); IME.Enable(); \
	if( !IME.IsOpen() || !IME.IsEnabled() ) \
	{	WriteWarning("Unable to activate IME"); return EndTestSet(); }
*/
#define CLOSE_IME \
	IME.Open(FALSE); Sleep(500)

///////////////////////////////////////////////////////////////////////////////
//	CStringTestCases class

IMPLEMENT_TEST(CStringTestCases, CTest, "DBCS String Editor Tests", -1, CSysSubSuite)

void CStringTestCases::Run(void)
{
	COResScript res;

	// create a new string table resource
	GetLog()->RecordInfo("Creating a new string table resource...");
	if (!(GetLog()->RecordCompare(res.CreateResource(IDSS_RT_STRING_TABLE) == ERROR_SUCCESS, "Creating a new string table resource."))) {
		throw CTestException("CStringTestCases::Run: cannot create string resource; unable to continue.", CTestException::causeOperationFail);
	}

	UIStrEdit ed = UIWB.GetActiveEditor();

	CString strStr, strID;

	UIWB.ShowPropPage(TRUE);    // Bring up an IME able window
	MST.DoKeys("{TAB}");
	OPEN_IME;

	GetLog()->RecordInfo("Entering DBC string in the string editor...");

	IME.SetMode(CIME::KATAKANA, 2);		// DBC-Katakana
	int n1 = ed.AddString("とよたひゅんだい");	// zoso
	ed.SelectLine(n1);
	strStr = ed.GetString();
//	strID = ed.GetId();
	GetLog()->RecordCompare(strStr == "とよたひゅんだい", "Entering DBC string in the string editor. Expected the string '%s' to be entered; found '%s'.", "ゾソn", (LPCSTR)strStr);

	GetLog()->RecordInfo("Overwriting DBC string in the string editor...");

	// WM_CHAR forwarding -- bug #9877
	IME.SetMode(CIME::KATAKANA, 2);		// DBC-Katakana
	DoKeysSlow("katakana", 20);		// speed bug #10642, postponed
	int n2 = ed.GetCurLine();
	UIWB.ShowPropPage(FALSE);
	ed.SelectLine(n2);
	strStr = ed.GetString();
	GetLog()->RecordCompare(strStr == "カタカナ", "Overwriting DBC string in the string editor. Expected the string '%s' to be entered; found '%s'.", "カタカナ", (LPCSTR)strStr);

	CLOSE_IME;
	ed.Close();
}

///////////////////////////////////////////////////////////////////////////////
//	CAccelTestCases class

IMPLEMENT_TEST(CAccelTestCases, CTest, "DBCS Accelerator Editor Tests", -1, CSysSubSuite)

void CAccelTestCases::Run(void)
{
	COResScript res;

	// create a new accelerator editor resource
	GetLog()->RecordInfo("Creating a new accelerator editor resource...");
	if (!(GetLog()->RecordCompare(res.CreateResource(IDSS_RT_ACCELERATOR) == ERROR_SUCCESS, "Creating a new accelerator editor resource."))) {
		throw CTestException("CStringTestCases::Run: cannot create accelerator resource; unable to continue.", CTestException::causeOperationFail);
	}

	UIAccEdit ed = UIWB.GetActiveEditor();

	CString strKey, strID;
	DWORD dwKeyMods;

	OPEN_IME;

	GetLog()->RecordInfo("Adding an SBC katakana accelerator...");
	IME.SetMode(CIME::KATAKANA, 1);		// SBC-Alpha (so the IME doesn't screw up entry)
	int n1 = ed.AddAccel("ka");			// ka
	ed.SelectLine(n1);
	strKey = ed.GetKey();
//	strID = ed.GetId();
	dwKeyMods = ed.GetKeyMods();
//	WriteLog( strKey == "182", "SBC-katakana accelerators work");
	GetLog()->RecordCompare(strKey == "ｶ", "Adding an SBC katakana accelerator. Expected the key '%s'; found '%s'.", "ｶ", strKey);

	CLOSE_IME;
	ed.Close();
}

///////////////////////////////////////////////////////////////////////////////
//	CImageTestCases class

IMPLEMENT_TEST(CImageTestCases, CTest, "DBCS Image Editor Tests", -1, CSysSubSuite)

void CImageTestCases::Run(void)
{
	COResScript res;

	// create a new bitmap editor resource
	GetLog()->RecordInfo("Creating a new bitmap editor resource...");
	if (!(GetLog()->RecordCompare(res.CreateResource(IDSS_RT_BITMAP) == ERROR_SUCCESS, "Creating a new bitmap editor resource."))) {
		throw CTestException("CStringTestCases::Run: cannot create bitmap resource; unable to continue.", CTestException::causeOperationFail);
	}

	UIImgEdit ed = UIWB.GetActiveEditor();

	ed.SetColor(6, VK_LBUTTON);
	ed.ClickImgTool(GT_TEXT);

	GetLog()->RecordInfo("Entering DBC text in the image editor...");
	OPEN_IME;
	IME.SetMode(CIME::HIRAGANA, 2);		// DBC-Hiragana
	DoKeysSlow("sushi", 10);	// すし
	ed.SetFont("ＭＳ ゴシック", 18, FONT_NORMAL);		// MS-??, 18 pt
	//ed.ClickImgTool(GT_SELECT);					// Commits selection and enables Copy command			
	MST.DoKeys("{ESC}") ; //commit changes.
//	IME.SetMode(CIME::ALPHA, 1);
	CLOSE_IME;
	GetLog()->RecordSuccess("Entering DBC text in the image editor.");

// REVIEW(briancr): we can't successfully compare bitmap files on Win95
// apparently the padding is different between different saves on Win95
/*
	GetLog()->RecordInfo("Exporting image and comparing with baseline...");
	UIWB.ExportResource(m_strCWD + "TESTOUT\\すし1.BMP", TRUE);
	int iresult = CompareFiles(m_strCWD + "TESTOUT\\すし1.BMP", m_strCWD + "BASELN\\sushi1.BMP", 100);
	GetLog()->RecordCompare(iresult == CF_SAME, "Exporting image and comparing with baseline. Compared '%s' with '%s'; result of compare: %d.", "TESTOUT\\すし1.BMP", "BASELN\\sushi1.BMP", iresult);
	// don't close
*/

	GetLog()->RecordInfo("Setting filename property to DBC filename...");
	UIWB.ShowPropPage(TRUE);
//	IME.SetMode(CIME::ALPHA, 1);
	MST.DoKeys("\"i");	// overtype all but first character
	OPEN_IME;
	IME.SetMode(CIME::HIRAGANA, 2);		// DBC-Hiragana
	MST.DoKeys("{END}");
	MST.DoKeys("sashimi8");	// iさしみ８
	UIWB.ShowPropPage(FALSE);
	CString strExpectedFile = "Iさしみ.BMP";
	CString strActualFile = UIWB.GetProperty(P_FileName);
	strActualFile.MakeUpper();
	GetLog()->RecordCompare(strActualFile.Find(strExpectedFile) >= 0, "Setting filename property to DBC filename. Expected filename '%s'; found '%s'.", (LPCSTR)strExpectedFile, (LPCSTR)strActualFile);
	CLOSE_IME;
	ed.Close();
}

///////////////////////////////////////////////////////////////////////////////
//	CMenuTestCases class

IMPLEMENT_TEST(CMenuTestCases, CTest, "DBCS Menu Editor Tests", -1, CSysSubSuite)

void CMenuTestCases::Run(void)
{
	COResScript res;

	// create a new menu editor resource
	GetLog()->RecordInfo("Creating a new menu editor resource...");
	if (!(GetLog()->RecordCompare(res.CreateResource(IDSS_RT_MENU) == ERROR_SUCCESS, "Creating a new menu editor resource."))) {
		throw CTestException("CStringTestCases::Run: cannot create menu resource; unable to continue.", CTestException::causeOperationFail);
	}

	UIImgEdit ed = UIWB.GetActiveEditor();

	MST.DoKeys("{right}");	// select menu bar

	GetLog()->RecordInfo("Entering DBC menu id (this should be illegal)...");
	OPEN_IME;
	IME.SetMode(CIME::KATAKANA, 2);
	UIControlProp upp = UIWB.ShowPropPage(TRUE);		// crash - bug #9876
	MST.WComboSetText(upp.GetLabel(SHRDRES_IDC_ID), "ni");	// illegal id: ニ
	UIWBMessageBox mb;
	upp.Close();
	if (GetLog()->RecordCompare(mb.WaitAttachActive(500) != NULL, "Entering DBC menu id. Expecting message box.")) {
		mb.ButtonClick();
	}

	GetLog()->RecordInfo("Entering SBC katakana menu id (this should be illegal)...");
	IME.SetMode(CIME::KATAKANA, 1);
	MST.DoKeys("ichi");		// illegal id: ｲﾁ
	upp.Close();
	if (GetLog()->RecordCompare(mb.WaitAttachActive(500) != NULL, "Entering SBC katakana menu id. Expecting message box.")) {
		mb.ButtonClick();
	}
	MST.DoKeys("{escape}");
	MST.DoKeys("{left}");		// move selection back to new item

	GetLog()->RecordInfo("Adding menu item with DBC and verifying legal ID is generated...");
	IME.SetMode(CIME::KATAKANA, 2);
	UIWB.ShowPropPage(TRUE);	// bug #9877
	MST.WEditSetText(upp.GetLabel(SHRDRES_IDC_CAPTION), "フォオ");	// foo
	UIWB.ShowPropPage(FALSE);
	DoKeysSlow("baro", 10);		// バロ
	UIWB.ShowPropPage(FALSE);
	MST.DoKeys("{up}");

	CString strID = UIWB.GetProperty(P_ID);
	GetLog()->RecordCompare(strID.Find("IDM_MENUITEM4000"), "Adding menu item with DBC and verifying legal ID is generated. Expected menu ID to include '%s'; found '%s'.", "IDM_MENUITEM4000", (LPCSTR)strID);

	CLOSE_IME;
	ed.Close();
/*
	MST.DoKeys("%fc"); // File close resource browser
	MST.DoKeys("n");   // No, to "Save changes?"
*/
}

//////////////////////////////////////////////////////////////////////////////
//	SRCCASES.CPP
//
//	Created by :			Date :
//		DavidGa					4/11/94
//
//	Description :
//		Implementation of the CSrcTestCases class
//

#include "stdafx.h"
#include "srccases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

extern ImeInit;

#define OPEN_IME \
	IME.Open(); Sleep(500); IME.Enable(); \
	Sleep(500); \
	(ImeInit == FALSE ? (IME.SetConversionMode(3),ImeInit=TRUE):0) ; \

/*
	IME.Open(); IME.Enable(); \
	if( !IME.IsOpen() || !IME.IsEnabled() ) \
	{	WriteWarning("Unable to activate IME"); return; }
*/

#define CLOSE_IME \
	IME.Enable(FALSE); Sleep(500); IME.Open(FALSE); Sleep(500);

///////////////////////////////////////////////////////////////////////////////
//	Helper functions

BOOL VerifyCursorPosition(COSource& src, int line, int col)
{
	return (src.GetCurrentLine() == line) && (src.GetCurrentColumn() == col);
}


///////////////////////////////////////////////////////////////////////////////
//	CSourceTestCases class

IMPLEMENT_TEST(CSourceTestCases, CTest, "DBCS Source Tests", -1, CSysSubSuite)

void CSourceTestCases::Run(void)
{
	// create a new source file
	m_src.Create();

	Typing();
	Cursor();

	m_src.Close();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

void CSourceTestCases::Typing(void)
{
	GetLog()->RecordInfo("Typing various single byte/double byte strings into the source editor.");

	m_src.TypeTextAtCursor("single byte alpha~");
	OPEN_IME;
	IME.SetMode(CIME::KATAKANA, 1);
	DoKeysSlow("ichi bite katakana~", 20);
	IME.SetMode(CIME::KATAKANA, 2);
	DoKeysSlow("ni bite katakana~", 10);
	IME.SetMode(CIME::HIRAGANA, 2);
	DoKeysSlow("ni bite hiragana~", 10);
	IME.SetMode(CIME::ALPHA, 2);
	DoKeysSlow("double byte alpha~", 10);
	IME.SetMode(CIME::ALPHA, 1);
	DoKeysSlow("single byte alpha~", 10);
	CLOSE_IME;

	GetLog()->RecordInfo("Successfully typed various single byte/double byte strings into the source editor.");

	m_src.SelectText(1,1,1,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "single byte alpha", "Typing SBC-Alpha without IME. Typed text '%s'.", m_src.GetSelectedText());

	m_src.SelectText(2,1,2,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "ｲﾁ ﾋﾞﾃ ｶﾀｶﾅ", "Typing SBC-Katakana. Typed text '%s'.", m_src.GetSelectedText());

	m_src.SelectText(3,1,3,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "ニ ビテ カタカナ", "Typing DBC-Katakana. Typed text '%s'.", m_src.GetSelectedText());

	m_src.SelectText(4,1,4,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "に びて ひらがな", "Typing DBC-Hiragana. Typed text '%s'.", m_src.GetSelectedText());

	m_src.SelectText(5,1,5,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "ｄｏｕｂｌｅ ｂｙｔｅ ａｌｐｈａ", "Typing DBC-alpha. Typed text '%s'.", m_src.GetSelectedText());

	m_src.SelectText(6,1,6,0);
	GetLog()->RecordCompare(m_src.GetSelectedText() == "single byte alpha", "Typing SBC-alpha with IME. Typed text '%s'.", m_src.GetSelectedText());
}

void CSourceTestCases::Cursor(void)
{
	int nExpectedCol;
	int nActualCol;

	GetLog()->RecordInfo("Moving caret right 4 times in SBC-Katakana...");
	m_src.SetCursor(2,1);
	DoKeysSlow("{right 4}", 10);
	nExpectedCol = 5;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Moving caret right 3 times in SBC-Katakana. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);

	GetLog()->RecordInfo("Moving caret down into the middle of a DBC...");
	DoKeysSlow("{down}", 10);
	nExpectedCol = 4;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Moving caret down into the middle of a DBC. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);

	GetLog()->RecordInfo("Moving caret right 2 words and 1 space in DBC-Katakana...");
	DoKeysSlow("^{right 2}{right}", 10);
	nExpectedCol = 11;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Moving caret right 2 words and 1 space in DBC-Katakana. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);

	GetLog()->RecordInfo("Overtyping DBC with SBC...");
	DoKeysSlow("{down}{insert}a{end}b", 10);	// REVIEW: make sure Insert is off initially (SUSHI.KEY)
	nExpectedCol = 17;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Overtyping DBC with SBC. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);

	GetLog()->RecordInfo("Moving caret down through the middle of a DBC...");
	DoKeysSlow("{down 2}", 10);
	nExpectedCol = 17;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Moving caret down through the middle of a DBC. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);

	GetLog()->RecordInfo("Overtyping SBC with DBC...");
	OPEN_IME;
	IME.SetMode(CIME::ALPHA, 2);
	DoKeysSlow("abet{insert}", 10);		// overtype still on, turn it back off
	nExpectedCol = 25;
	nActualCol = m_src.GetCurrentColumn();
	GetLog()->RecordCompare(nActualCol == nExpectedCol, "Overtyping SBC with DBC. Expected column %d, navigated to column %d.", nExpectedCol, nActualCol);
	CLOSE_IME;
}

///////////////////////////////////////////////////////////////////////////////
//	CSearchTestCases class

IMPLEMENT_TEST(CSearchTestCases, CTest, "DBCS Search tests", -1, CSysSubSuite)

void CSearchTestCases::Run(void)
{
	GetLog()->RecordInfo("Copying 'dbcs.txt' to 'カタカナ.cpp'...");
	SetFileAttributes(m_strCWD + "カタカナ.cpp", FILE_ATTRIBUTE_NORMAL);	// who cares if it doesn't exist
	if (GetLog()->RecordCompare(::CopyFile(m_strCWD + "dbcs.txt", m_strCWD + "カタカナ.cpp", FALSE), "Copying 'dbcs.txt' to 'カタカナ.cpp'.")) {
		SetFileAttributes(m_strCWD + "カタカナ.cpp", FILE_ATTRIBUTE_NORMAL);	// must remove read-only flag
	}
	else {
		throw CTestException("Unable to set the test up correctly to run.", CTestException::causeOperationFail);
	}

	Text();
	Expression();
	InFiles();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

void CSearchTestCases::Text(void)
{
	COSource src;

	int nExpectedLine, nExpectedCol;
	int nActualLine, nActualCol;

	// open the source file
	src.Open(m_strCWD + "カタカナ.cpp");

	// navigate to the beginning of the file
	MST.DoKeys("^{home}");
	
	// find the first backslash
	GetLog()->RecordInfo("Finding the first '\\'...");
	src.Find("\\");
	nExpectedLine = 9;
	nExpectedCol = 51;
	nActualLine = src.GetCurrentLine();
	nActualCol = src.GetCurrentColumn();
	GetLog()->RecordCompare(VerifyCursorPosition(src, nExpectedLine, nExpectedCol), "Finding the first '\\'. Expected cursor at (%d, %d); navigated to (%d, %d).", nExpectedLine, nExpectedCol, nActualLine, nActualCol);

	// find the second backslash
	GetLog()->RecordInfo("Finding the second '\\'...");
	src.Find("\\");
	nExpectedLine = 10;
	nExpectedCol = 2;
	nActualLine = src.GetCurrentLine();
	nActualCol = src.GetCurrentColumn();
	GetLog()->RecordCompare(VerifyCursorPosition(src, nExpectedLine, nExpectedCol), "Finding the second '\\'. Expected cursor at (%d, %d); navigated to (%d, %d).", nExpectedLine, nExpectedCol, nActualLine, nActualCol);

	// find the first ａ
	GetLog()->RecordInfo("Finding the first 'ａ'...");
	src.Find("ａ");
	nExpectedLine = 6;
	nExpectedCol = 3;
	nActualLine = src.GetCurrentLine();
	nActualCol = src.GetCurrentColumn();
	GetLog()->RecordCompare(VerifyCursorPosition(src, nExpectedLine, nExpectedCol), "Finding the first 'ａ'. Expected cursor at (%d, %d); navigated to (%d, %d).", nExpectedLine, nExpectedCol, nActualLine, nActualCol);

	// find the second ａ
	GetLog()->RecordInfo("Finding the second 'ａ'...");
	src.Find("ａ");
	nExpectedLine = 7;
	nExpectedCol = 3;
	nActualLine = src.GetCurrentLine();
	nActualCol = src.GetCurrentColumn();
	GetLog()->RecordCompare(VerifyCursorPosition(src, nExpectedLine, nExpectedCol), "Finding the second 'ａ'. Expected cursor at (%d, %d); navigated to (%d, %d).", nExpectedLine, nExpectedCol, nActualLine, nActualCol);

	// replace a DBC string with an SBC string
	GetLog()->RecordInfo("Replacing a DBC string ('ｘｙｚ') with an SBC string ('123')...");
	int nExpectedReplaces = 2;
	int nActualReplaces = src.Replace("ｘｙｚ", "123", FALSE, FALSE, FALSE, COSource::RS_WHOLEFILE, TRUE);
	GetLog()->RecordCompare(nActualReplaces == nExpectedReplaces, "Replacing a DBC string ('ｘｙｚ') with an SBC string ('123'). Expected %d replaces; detected %d replaces.", nExpectedReplaces, nActualReplaces);

	src.Close();
}

void CSearchTestCases::Expression(void)
{
}

void CSearchTestCases::InFiles(void)
{
}

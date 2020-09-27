///////////////////////////////////////////////////////////////////////////////
//      SYMCASES.CPP
//
//      Created by :                    Date :
//              DavidGa                                 9/20/93
//
//      Description :
//              Implemtation of the CSymTestCases class
//

#include "stdafx.h"
#include "symcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//      CTestSet overrides

IMPLEMENT_TEST(CSymTestCases, CTest, "Symbol Browser Tests", -1, CSniffDriver)

void CSymTestCases::Run(void)
{
	UIEditor ed = UIWB.OpenFile(m_strCWD + SYMBOL_RC);
	EXPECT( ed.IsValid() );

	XSAFETY;
	CheckSymbolCounts();
	XSAFETY;
	CreateDefaultSymbol();
	XSAFETY;
	QuotedStringID();
	XSAFETY;
	SymbolDropDown();
	XSAFETY;
	UsedByList();
	XSAFETY;
	ViewUse();
	XSAFETY;
	CreatSymbol();
	XSAFETY;
	ChangeUnusedSymbol();
	XSAFETY;
	ChangeInUseSymbol();
	XSAFETY;
	ChangeUsingIDProp();
	XSAFETY;
	DeleteUnusedSymbol();
	XSAFETY;
	DeleteInUseSymbol();
	XSAFETY;
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//      Test Cases

BOOL CSymTestCases::CheckSymbolCounts()
{
// REVIEW(briancr): moved the code from UWBFrame::BrowseSymbols to here because
// we can't support this function in UWBFrame anymore.
// Find a better place for this function.
	UISymbolBrowser sb;
    UIWB.DoCommand(IDM_EDIT_SYMBOLS, DC_MNEMONIC);
    sb.WaitAttachActive(15000);
	Sleep(2000);
	int nSymbols = sb.CountSymbols(FALSE);  // count normal symbols
	if (nSymbols != 2) {
		m_pLog->RecordFailure("CheckSymbolCounts: Found %d normal symbols (expected %d)", nSymbols, 2);
	}

	nSymbols = sb.CountSymbols(TRUE) - nSymbols;    // count all symbols, and subtract normal ones
	if (nSymbols != 12) {
		m_pLog->RecordFailure("CheckSymbolCounts: Found %d read-only symbols (expected %d)", nSymbols, 12);
	}

	MST.DoKeys("{esc}");    // close the symbol browser
	return TRUE;
}

BOOL CSymTestCases::CreateDefaultSymbol()
{
	COResScript res;

	res.CreateResource(IDSS_RT_DIALOG);

// REVIEW(briancr): not supported in UIWB
//      UIEditor ed = UIWB.CreateNewResource(GetLocString(IDSS_RT_DIALOG));

	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	int nSymDiff = CountSymbols(FALSE, 3);
	if (nSymDiff != 0) {
		m_pLog->RecordFailure("CreateDefaultSymbol: Found %d symbols after creating dialog (expected %d)", 3 + nSymDiff, 3);
		return FALSE;
	}
	return TRUE;
}

BOOL CSymTestCases::QuotedStringID()
{
	MST.DoKeys("%({ENTER})");          // Bring up Property page REVIEW we should have a WB util to set the ID...
	MST.DoKeys("%i\"quoted_string_id");	// Set ID to "quoted_string_id"
	MST.DoKeys("~");

	int nSymDiff = CountSymbols(FALSE, 2);
	if (nSymDiff != 0) {
		m_pLog->RecordFailure("QuotedStringID: Found %d symbols after giving dialog a quoted-string ID (expected %d)", 2 + nSymDiff, 2);
		return FALSE;
	}
	return TRUE;
}

BOOL CSymTestCases::SymbolDropDown()
{
	return TRUE;
}

BOOL CSymTestCases::UsedByList()
{
	return TRUE;
}

BOOL CSymTestCases::ViewUse()
{
	return TRUE;
}

BOOL CSymTestCases::CreatSymbol()
{
	return TRUE;
}

BOOL CSymTestCases::ChangeUnusedSymbol()
{
	return TRUE;
}

BOOL CSymTestCases::ChangeInUseSymbol()
{
	return TRUE;
}

BOOL CSymTestCases::ChangeUsingIDProp()
{
	return TRUE;
}

BOOL CSymTestCases::DeleteUnusedSymbol()
{
	return TRUE;
}

BOOL CSymTestCases::DeleteInUseSymbol()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//      Utilities

int CSymTestCases::CountSymbols(BOOL bReadOnly, int nExpected /*=0*/)
{
// REVIEW(briancr): moved the code from UWBFrame::BrowseSymbols to here because
// we can't support this function in UWBFrame anymore.
// Find a better place for this function.
	UISymbolBrowser sb;
    UIWB.DoCommand(IDM_EDIT_SYMBOLS, DC_MNEMONIC);
    sb.WaitAttachActive(1000);
//      UISymbolBrowser sb = UIWB.BrowseSymbols();
	int count = sb.CountSymbols(bReadOnly);
	MST.DoKeys("{esc}");
	return  count - nExpected;
}

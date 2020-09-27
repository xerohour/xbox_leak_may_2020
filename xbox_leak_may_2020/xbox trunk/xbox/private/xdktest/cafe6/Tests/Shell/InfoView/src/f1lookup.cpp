///////////////////////////////////////////////////////////////////////////////
//	F1Lookup.CPP
//
//	Created by :					Date :
//		MarcI							8/20/96
//					  
//	Description :
//		InfoViewer's F1 Lookup Sniff Tests


#include "stdafx.h"
#include "mstwrap.h"
#include "F1Lookup.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

COSource	sourceFile;
COSource	*pSourceFile=&sourceFile;

IMPLEMENT_TEST(CF1Lookup, CTest, "InfoViewer F1Lookup", -1, CSniffDriver)

void CF1Lookup::Run(void)
{
	LPCSTR		aTitle	=	"";
	CString		aString =	"";
	CString&	raString =	aString;
	HWND		hSourceWnd = NULL;
	HWND		hTopicWindow = NULL;
	int			iCount = 0;

	//initialization for all tests goes here
	Initialize_F1();

//	DoCommand
	hSourceWnd = MST.WGetActWnd(0);

  m_pLog->Comment("Looking up a topic that will generate no hits");
	Lookup(IDS_F1LOOKUP_NOHITS);
	aString = GetLocString(IDSS_WORKBENCH_TITLEBAR, CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//	aString.LoadString(IDS_TITLE_NOTOPICSFOUND);
	aTitle = aString;
	hTopicWindow = MST.WFndWndWait(aTitle, FW_ACTIVE_ONLY | FW_DIALOG, 90);
	if (hTopicWindow == NULL) {
		iCount = iCount+1;
		m_pLog->Comment("Did NOT Find the NoTopicsFound error message.");
	}
	
	ASSERT(hTopicWindow != NULL);
	MST.DoKeys("{ESC}");
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC);
	MST.DoKeys("{DEL}");

  m_pLog->Comment("Looking up a topic that will generate only one hit");
	Lookup(IDS_F1LOOKUP_ONEHIT);
	aString = GetLocString(IDS_IV_TOPICWND_TITLE, CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//	aString.LoadString(IDS_TITLE_TopicWindow);
	aTitle = aString;
	hTopicWindow = MST.WFndWndWait(aTitle, FW_ACTIVE | FW_PART | FW_FOCUS | FW_RESTOREICON, MAXWAIT);
	if (hTopicWindow == NULL) {
		 iCount = iCount+1;
		m_pLog->Comment("Did NOT Find the Topic Window.");
	}
	ASSERT(hTopicWindow != NULL);
	MST.WSetActWnd(hSourceWnd);
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC);
	MST.DoKeys("{DEL}");

  m_pLog->Comment("Looking up a topic that will generate more than one hit");
	Lookup(IDS_F1LOOKUP_SOMEHITS);
	MST.DoKeys ("{DOWN}");					//arrow down to force the topic list dock to have focus
	MST.DoKeys("{ENTER}");					//load the Topic in the IV Window
	aString = GetLocString(IDS_IV_TOPICWND_TITLE, CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//	aString.LoadString(IDS_TITLE_TopicWindow);
	aTitle = aString;
	hTopicWindow = MST.WFndWndWait(aTitle, FW_ACTIVE | FW_PART | FW_FOCUS | FW_RESTOREICON, MAXWAIT);
	if (hTopicWindow == NULL) {
		iCount = iCount+1;
		m_pLog->Comment("Did NOT Find the Topic Window.");
	}
	MST.WSetActWnd(hSourceWnd);
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC);
	DoKeysSlow("{DEL}",SHORTWAIT*100);

	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);
	MST.DoKeys("{TAB}");								// select the "No" Button
	MST.DoKeys("{ENTER}");								// select the "No" Button

	if(iCount>0){
		m_pLog->RecordFailure("One or more dialogs/windows failed to appear. See Comments in log file for missing dialogs");
	}
};


void CF1Lookup::Initialize_F1() {
	pSourceFile->Create();

};


void CF1Lookup::Lookup(UINT uiID) {
	CString		preText;
	CString		targetText;
	CString		postText;

// Type a string with the target word from uiID
	preText.LoadString(IDS_SURROUND_TEXT1);
	targetText.LoadString(uiID);
	postText.LoadString(IDS_SURROUND_TEXT2);
	pSourceFile->TypeTextAtCursor(preText + targetText + postText);

	MST.DoKeys("^{HOME}");							//start at the beginning of the source file
	FindText(targetText, TRUE, FALSE, FALSE);		//Edit:Find
	MST.DoKeys ("{F1}",FALSE,5);
}


void CF1Lookup::FindText(LPCSTR szFind, BOOL bMatchWord, BOOL bMatchCase, BOOL bRegExpr) {

	UIFindDlg uiFind;

	uiFind.Activate();
	// set the find string
	uiFind.SetFindString(szFind);
	// set match word
	uiFind.SetMatchWord(bMatchWord);
	// set match case
	uiFind.SetMatchCase(bMatchCase);
	// set regular expression
	uiFind.SetRegularExpression(bRegExpr);
	// set find direction
	uiFind.SetDirection(UIFindDlg::FD_DOWN);

	// do the find
	MST.WButtonClick(GetLabel(VSHELL_IDC_FIND_NEXT));

//	return uiFind.FindNext();  // GAC original
}
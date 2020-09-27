///////////////////////////////////////////////////////////////////////////////
//	Generic.CPP
//
//	Created by :					Date : 
//		DougMan							1/7/97
//					  
//	Description :
//		InfoViewer's DialogWalk Sniff Tests
//		This test verifies that all IV5 dialogs are accessable
//


#include "stdafx.h"
//#include "wbutil.h"					//GetLocString function to grab Text from the string table
//#include "testutil.h"
#include "mstwrap.h"
//#include "umainfrm.h"
//#include "..\eds\ufindrep.h"
//#include "..\eds\COSource.h"
#include "sniff.h"
#include "DlgWalk.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


IMPLEMENT_TEST(CDlgWalk, CTest, "InfoViewer DialogWalk", -1, CSniffDriver)

void CDlgWalk::Run(void)
{

	int		iCount=0;
	CASES	sTestCases[TOTALDIALOGS];


	m_pLog->Comment("Begin DialogWalk Test - Maintained by DougMan");

//  Setup Initial Conditions for the Test
	iCount=0;
	// Initalize the sTestCases array
	InitalCases((struct CASES*) &sTestCases);

	
//  CDlgWalk main routine starts here.

	for( int i = 0; i < TOTALDIALOGS; ++i )
		iCount=iCount + FindDialog(sTestCases[i].PreTest, sTestCases[i].iDlg_Mnemonic, sTestCases[i].iVerification, sTestCases[i].aAdditionalKey, sTestCases[i].PostTest);

	if(iCount>0){
//	strReturn = iCount + " out of " + TOTALDIALOGS + " failed to appear./n See Comments in log file for missing dialogs.";
	m_pLog->RecordFailure("One or more dialogs/windows failed to appear./n See Comments in log file for missing dialogs");
	}

	m_pLog->Comment("Comment Printed in the Cafe Log - End of DialogWalk Test");

};	// End Run()


int CDlgWalk::FindDialog(CString strPreCase, UINT iMnemonic, UINT iVerify, CString strAddKeys, CString strPostCase)
{
	LPCSTR		aTitle = "";
	CString		aString = "";
	HWND		hWindow = NULL;
	int			iCounter=0;


	if(strPreCase != "") {
		MST.DoKeys(strPreCase);
	}

	if (iMnemonic != 0){
		UIWB.DoCommand(iMnemonic, DC_MNEMONIC);	// Activate requested dialog/window based on Mnemonic
	}

	if(strAddKeys != "") {
		MST.DoKeys(strAddKeys);
	}

	aString = GetLocString(iVerify,CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//	aString.LoadString(iVerify);  // used to get strings from projects rc file
	aTitle = aString;
	hWindow = MST.WFndWndWait(aTitle, FW_ALL | FW_DIALOGOK, SHORTWAIT);  // verify that requested dialog/window appeared
//m_pLog->Comment("The Window Title:  " + aString); // DEBUGGING LINE
	if (hWindow == NULL) {
		m_pLog->Comment("Did NOT Find " + aString + "!");
		iCounter=1;
	}

	if(strPostCase != "") {
		MST.DoKeys(strPostCase);
	}

	return(iCounter);
};  // End FindDialog()
 

void CDlgWalk::InitalCases(struct CASES *ptTestCases)
{

//	struct CASES {
//	UINT	iDlg_Mnemonic;
//	UINT	iVerification;
//	CString	aAdditionalKey;	
//}; // End TestCases Structure

//  Initialization of the Test Case Array
	//  iDlg_Mnemonic

	ptTestCases[0].iDlg_Mnemonic  = IDM_OPTIONS;	//second one for Tabs on the dialog
	ptTestCases[1].iDlg_Mnemonic  = IDM_CUSTOMIZE;	//second one for Tabs on the dialog
	ptTestCases[2].iDlg_Mnemonic  = IDM_VIEW_WORKSPACE;
	ptTestCases[3].iDlg_Mnemonic  = IDM_VIEW_RESULTS;
	ptTestCases[4].iDlg_Mnemonic  = IDM_VIEW_RESULTS;
	ptTestCases[5].iDlg_Mnemonic  = IDM_VIEW_TOPIC;
	ptTestCases[6].iDlg_Mnemonic  = 0;
	ptTestCases[7].iDlg_Mnemonic  = IDM_HELP_IVBOOKMARKS;
	ptTestCases[8].iDlg_Mnemonic  = IDM_HELP_IVBOOKMARKS;
	ptTestCases[9].iDlg_Mnemonic  = IDM_HELP_IVBOOKMARKS;
	ptTestCases[10].iDlg_Mnemonic  = ID_FILE_PRINT;
	ptTestCases[11].iDlg_Mnemonic = ID_FILE_PAGE_SETUP;
	ptTestCases[12].iDlg_Mnemonic = ID_EDIT_FIND_DLG;
	ptTestCases[13].iDlg_Mnemonic = ID_FILE_OPEN;
	ptTestCases[14].iDlg_Mnemonic = IDM_GOTO_LINE;
	ptTestCases[15].iDlg_Mnemonic = IDM_HELP_SEARCH; //second one for Tabs on the dialog
	ptTestCases[16].iDlg_Mnemonic = IDM_HELP_DEFINESUBSETS;
	ptTestCases[17].iDlg_Mnemonic = IDM_HELP_SELECTSUBSETS;
//	ptTestCases[18].iDlg_Mnemonic = ;

	//  iVerification
	ptTestCases[0].iVerification  = IDSS_OD_TITLE;
	ptTestCases[1].iVerification  = IDSS_CUSTOMIZE_TITLE;
	ptTestCases[2].iVerification  = IDSS_WORKSPACE_TITLE;
	ptTestCases[3].iVerification  = IDSS_RESULTS_TITLE;
	ptTestCases[4].iVerification  = IDSS_RESULTSPROPS_TITLE;
	ptTestCases[5].iVerification  = IDSS_IVTOPIC_TITLE;
	ptTestCases[6].iVerification  = IDS_IV_TOPICWND_TITLE;
	ptTestCases[7].iVerification  = IDSS_IVBOOKMARKS_TITLE;
	ptTestCases[8].iVerification  = IDSS_ADDIVBOOKMARK_TITLE;
	ptTestCases[9].iVerification  = IDSS_EDITIVBOOKMARK_TITLE;
	ptTestCases[10].iVerification  = IDSS_FP_TITLE;
	ptTestCases[11].iVerification = IDSS_FU_TITLE;
	ptTestCases[12].iVerification = IDSS_FIND_TITLE;
	ptTestCases[13].iVerification = IDSS_FO_TITLE;
	ptTestCases[14].iVerification = IDSS_GOTO_TITLE;
	ptTestCases[15].iVerification = IDSS_SEARCH_TITLE;
	ptTestCases[16].iVerification = IDSS_DEFINESUBSETS_TITLE;
	ptTestCases[17].iVerification = IDSS_SELECTSUBSETS_TITLE;
//	ptTestCases[18].iVerification = ;

	//  aAdditionalKeys
	ptTestCases[0].aAdditionalKey  = "";
	ptTestCases[1].aAdditionalKey  = "";
	ptTestCases[2].aAdditionalKey  = "";
	ptTestCases[3].aAdditionalKey  = "";
	ptTestCases[4].aAdditionalKey  = "%{ENTER}";	
	ptTestCases[5].aAdditionalKey  = "";
	ptTestCases[6].aAdditionalKey  = "";
	ptTestCases[7].aAdditionalKey  = "";
	ptTestCases[8].aAdditionalKey  = "%A";
	ptTestCases[9].aAdditionalKey  = "%E";
	ptTestCases[10].aAdditionalKey  = "";
	ptTestCases[11].aAdditionalKey = "";
	ptTestCases[12].aAdditionalKey = "";
	ptTestCases[13].aAdditionalKey = "";
	ptTestCases[14].aAdditionalKey = "";
	ptTestCases[15].aAdditionalKey = "";
	ptTestCases[16].aAdditionalKey = "";
	ptTestCases[17].aAdditionalKey = "";
//	ptTestCases[18].aAdditionalKey = "";

	ptTestCases[0].PreTest  = "";
	ptTestCases[1].PreTest  = "";
	ptTestCases[2].PreTest  = "%0%WD^{F4}";
	ptTestCases[3].PreTest  = "";
	ptTestCases[4].PreTest  = "";
	ptTestCases[5].PreTest  = "";
	ptTestCases[6].PreTest  = "";
	ptTestCases[7].PreTest  = "";
	ptTestCases[8].PreTest  = "";
	ptTestCases[9].PreTest  = "";
	ptTestCases[10].PreTest  = "";
	ptTestCases[11].PreTest = "";
	ptTestCases[12].PreTest = "";
	ptTestCases[13].PreTest = "";
	ptTestCases[14].PreTest = "";
	ptTestCases[15].PreTest = "";
	ptTestCases[16].PreTest = "";
	ptTestCases[17].PreTest = "";
//	ptTestCases[18].PreTest = "";

	ptTestCases[0].PostTest  = "{ESC}";
	ptTestCases[1].PostTest  = "{ESC}";
	ptTestCases[2].PostTest  = "%WD";
	ptTestCases[3].PostTest  = "";
	ptTestCases[4].PostTest  = "{ESC}";
	ptTestCases[5].PostTest  = "";
	ptTestCases[6].PostTest  = "";
	ptTestCases[7].PostTest  = "";
	ptTestCases[8].PostTest  = "{ENTER}";
	ptTestCases[9].PostTest  = "{ESC}%D{ESC}";
	ptTestCases[10].PostTest  = "{ESC}";
	ptTestCases[11].PostTest = "{ESC}";
	ptTestCases[12].PostTest = "{ESC}";
	ptTestCases[13].PostTest = "{ESC}";
	ptTestCases[14].PostTest = "{ESC}";
	ptTestCases[15].PostTest = "{ESC}";
	ptTestCases[16].PostTest = "{ESC}";
	ptTestCases[17].PostTest = "{ESC}";
//	ptTestCases[18].PostTest = "";


//  When adding/removing test cases remember to increase/decrease the TOTALDIALOGS constent in the dlgwalk.h file

//	ArryID#	iDlg_Mnemonic,				iVerification,				aAdditionalKeys,	Initial Conditions
//	0		{IDM_VIEW_WORKSPACE,		IDSS_WORKSPACE_TITLE, 		NULL},				// Workspace window should be closed (INITIALIZE)
//
//	1		{IDM_VIEW_RESULTS,			IDSS_RESULTS_TITLE,			NULL},				// Results List window should be closed (INITIALIZE)
//	2		{IDM_VIEW_RESULTS,			IDSS_RESULTSPROPS_TITLE,	"%{ENTER}"},		// Results List window must be selected
//
//	3		{IDM_VIEW_TOPIC,			IDSS_IVTOPIC_TITLE,			NULL},				// InfoViewer Topic window should be closed (INITIALIZE)
//	4		{IDM_HELP_IVBOOKMARKS,		IDSS_IVBOOKMARKS_TITLE,		NULL},				// None, except ADD Bookmark requires some content window must be selected(Topic Window)
//	5		{IDM_HELP_IVBOOKMARKS,		IDSS_ADDIVBOOKMARK_TITLE,	"%A"},				// Some content window must be selected(Topic Window)
//	6		{IDM_HELP_IVBOOKMARKS,		IDSS_EDITIVBOOKMARK_TITLE,	"%E"},	//***		// A bookmark must exist, Add Bookmark
//	7		{ID_FILE_PRINT,				IDSS_FP_TITLE,				NULL},				// Some content window must be selected(Topic Window)
//	8		{ID_FILE_PAGE_SETUP,		IDSS_FU_TITLE,				NULL},				// Some content window must be selected(Topic Window)
//	9		{ID_EDIT_FIND_DLG,			IDSS_FIND_TITLE,			NULL},				// Some content window must be selected(Topic Window)
//
//	10		{ID_FILE_OPEN,				IDSS_FO_TITLE,				NULL},				// None
//	11		{IDM_GOTO_LINE,				IDSS_GOTO_TITLE,			NULL},				// None
//	12		{IDM_HELP_SEARCH,			IDSS_SEARCH_TITLE,			NULL},				// None (2 Tabs on this dialog)
//		//	{IDM_HELP_SEARCH,			IDSS_SEARCH_TITLE,			"+{TAB}{RIGHT}"},	// None (2 Tabs on this dialog)
//	13		{IDM_HELP_DEFINESUBSETS,	IDSS_DEFINESUBSETS_TITLE,	NULL},				// None
//	14		{IDM_HELP_SELECTSUBSETS,	IDSS_SELECTSUBSETS_TITLE,	NULL},				// None
//	15		{IDM_CUSTOMIZE,				IDSS_CUSTOMIZE_TITLE,		NULL},				// None (5 Tabs on this dialog)
//		//	{IDM_CUSTOMIZE,				IDSS_CUSTOMIZE_TITLE,		"+{TAB}{RIGHT}"},	// None (5 Tabs on this dialog)
//	16		{IDM_OPTIONS,				IDSS_OD_TITLE,				NULL}				// None (14 Tabs on this dialog)
//		//	{IDM_OPTIONS,				IDSS_OD_TITLE,				"+{TAB}{RIGHT}"}	// None (14 Tabs on this dialog)
//		
//		//	To emable test to detect the various tabs of a dialog a better iVerication needs to be used, and more inital conditions will apply to dialogs with multiple tabs???

};	// End InitCases();
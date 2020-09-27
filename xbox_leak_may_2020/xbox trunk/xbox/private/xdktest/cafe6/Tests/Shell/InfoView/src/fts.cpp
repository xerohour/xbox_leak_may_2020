///////////////////////////////////////////////////////////////////////////////
//	FST.CPP
//
//	Created by :					Date : 
//		DougMan							1/14/97
//					  
//	Description :
//		InfoViewer's Full Text Search(FTS) Sniff Tests
//		This test verifies that basic Full Text Search(FTS) works in IV5
//


#include "stdafx.h"
#include "mstwrap.h"
#include "sniff.h"
#include "FTS.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


IMPLEMENT_TEST(CFTS, CTest, "InfoViewer Full Text Search(FTS)", -1, CSniffDriver)

void CFTS::Run(void)
{
	COSource	sourceFile_FTS;
	COSource	*pSourceFile_FTS=&sourceFile_FTS;
	int iCount;
	FTS_CASES	sTestCases[FTS_TC];
	
	
	m_pLog->Comment("Begin Full Text Search(FTS) Test - Maintained by DougMan");

//  Setup Initial Conditions for the Test
	iCount=0;
	// Initalize the sTestCases array
	InitalCases((struct FTS_CASES*) &sTestCases);
	// Setting up Visual Studio with a blank page.
	pSourceFile_FTS->Create();
	DoKeys("%-x");	// Maximize Text Windows.
	DoKeys("%0");	// Give TOC windows focus - In Cafe ONLY Ctrl+Shift+F will not work if Text window has focus.
		

//  FTS Main routine
	for( int i = 0; i < FTS_TC; ++i )
		iCount=iCount + VerifySearch(sTestCases[i].iSearchString, sTestCases[i].iVerificationString);

	if(iCount>0){
	m_pLog->RecordFailure("One or more Searchs failed./n See Comments in log file for missing dialogs");
	}

//  Post Test Cleanup
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	m_pLog->Comment("Comment Printed in the Cafe Log - End of DialogWalk Test");

};	// End Run()



int CFTS::VerifySearch(UINT iSearch, UINT iVerify)
{
	LPCSTR		aTitle  = "";
	CString		aString = "";
	CString		aVerify = "";
	CString		aError	= "";
	HWND		hWindow = NULL;
	int			iCounter=0;
	BOOL		bTest=FALSE;


//  Start Search
//	UIWB.DoCommand(IDM_HELP_SEARCH, DC_MNEMONIC);	// Activate the Search dialog based on its Mnemonic
	MST.DoKeys("^(+F)");  // *** NEED TO SETUP GLOBAL MNEMONIC IN CAFE FOR THIS Dialog. ***
	aString.LoadString(iSearch);  // used to get the search strings from projects rc file
	MST.DoKeys(aString);
	MST.DoKeys("{ENTER}");


//	hWindow=MST.WFndWndWait("Searching...", FW_NOEXIST,2);
//	hWindow = MST.WFndWndWait("Searching...", FW_NOEXIST & FW_DIALOG, 2);  //*****WHAT DOES THIS COMMAND RETURN IF IT TIMED OUT, IT WAS SUCCESSFUL??? *****
//	if(hWindow != NULL ){
//		m_pLog->Comment("Searching... has finished successfully.");  //***** DEBUG  *****
//	}


//  Verify Search

	aVerify = GetLocString(IDSS_WORKBENCH_TITLEBAR,CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//	aVerify.LoadString(IDS_FTS_ERRORTITLE);
	aTitle = aVerify;
	hWindow = MST.WFndWndWait(aTitle, FW_ACTIVE_ONLY , SHORTWAIT);  //FW_EXIST & FW_DIALOG
	if (hWindow != NULL) {
		// Either the "Search Failed" or "No Topics Found" warning dialog has appeared.
		aError = GetLocString(IDS_SEARCHFAILED, CAFE_LANG_ENGLISH);		// used to get strings from wb.rc file
//		aError.LoadString(IDS_FTS_SEARCHFAILED);
		aTitle = aError;
		bTest=MST.WStaticExists(aTitle);
	//	CString		strTest="";				//*****Declaration for variable in next line.*****
	//	MST.WStaticText(aTitle, strTest);	//*****This does work.*****
	//	if((MST.WStaticExists(aTitle))==TRUE){  // *****THIS SHOULD WORK!!!!*****
		if(bTest==TRUE){
			// "The search failed." warning dialog has appeared.  This TestCase Failed.
			iCounter=1;
			m_pLog->Comment("Search for '" + aString + "' Failed!!");
			DoKeys("{ENTER}");  //  Clear the "Search Failed." warning dialog.
			DoKeys("{ESC}");	//	Clear the "Search" dialog.
		}
		else{
			// "No Topic Found." warning dialog has appeared.
			// Verify that this is expected behavior.
			if(iVerify==0){
				//  This is not expected behavior.  This TestCase Failed..
				iCounter=1;
				m_pLog->Comment("Search for '" + aString + "' resulted in an unexpected 'No Topic Found.' warning dialog!!");
			}
			DoKeys("{ENTER}");  //  Clear the "No Topic Found." warning dialog.
			DoKeys("{ESC}");	//	Clear the "Search" dialog.
		}

	}

	return(iCounter);
};  // End VerifySearch()
 

void CFTS::InitalCases(struct FTS_CASES *ptTestCases)
{

ptTestCases[0].iSearchString  = IDS_FTS_SEARCH1;
ptTestCases[1].iSearchString  = IDS_FTS_SEARCH2;
ptTestCases[2].iSearchString  = IDS_FTS_SEARCH3;
ptTestCases[3].iSearchString  = IDS_FTS_SEARCH4;
ptTestCases[4].iSearchString  = IDS_FTS_SEARCH5;
ptTestCases[5].iSearchString  = IDS_FTS_SEARCH6;
ptTestCases[6].iSearchString  = IDS_FTS_SEARCH7;

ptTestCases[0].iVerificationString  = 0;
ptTestCases[1].iVerificationString  = 0;
ptTestCases[2].iVerificationString  = 1;	// 1 means that this search should not find any topic.
ptTestCases[3].iVerificationString  = 0;
ptTestCases[4].iVerificationString  = 0;
ptTestCases[5].iVerificationString  = 0;
ptTestCases[6].iVerificationString  = 1;	// 1 means that this search should not find any topic.


};	// End InitalCases();
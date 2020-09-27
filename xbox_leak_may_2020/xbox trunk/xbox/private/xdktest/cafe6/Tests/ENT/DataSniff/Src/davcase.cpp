///////////////////////////////////////////////////////////////////////////////
//      DAVCASE.CPP
//
//      Created by :                    Date :
//              ChrisKoz                                  9/9/97
//
//      Description :
//              Implementation of the Davinci Designers tools test cases
//

#include "stdafx.h"
#include <io.h>
#include "davcase.h"
#include "sniff.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////////
//TESTHOOK.DLL entry points
/*extern "C"*/ typedef LRESULT (APIENTRY* PInitializeTestHook)(HWND hwndTarget, ULONG ulSize);
extern "C" typedef LRESULT (APIENTRY* PSendMessageTestHook)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern "C" typedef LRESULT (APIENTRY* PUninitializeTestHook)(HWND hwndTarget);
extern "C" typedef LRESULT (APIENTRY* PSendGridMessageTestHook)(HWND hwndView, HWND hwndGrid,UINT uMsg, WPARAM wParam, LPARAM lParam);
//extern "C" typedef LRESULT (APIENTRY* PFormatResourceString)(PCH paszResult, const UINT cchResultMax, PCH paszFormat, 
//										VARIANT *pvarArg1, VARIANT *pvarArg2, VARIANT *pvarArg3, VARIANT *pvarArg4, VARIANT *pvarArg5);
//extern "C" typedef LRESULT (APIENTRY* PReplaceText)(PCH szFilename, PCH szReplacement, LONG lLeft, LONG lTop, LONG lRight, LONG lBottom);
//extern "C" typedef LRESULT (APIENTRY* PSystem)(PCH szCommand);
extern "C" typedef LRESULT (APIENTRY* PGetGridChecksum)(HWND hwndView, HWND hwndGrid, USHORT *pwChecksum);


PInitializeTestHook InitializeTestHook;
PSendMessageTestHook SendMessageTestHook;
PUninitializeTestHook UninitializeTestHook;
PSendGridMessageTestHook SendGridMessageTestHook;
PGetGridChecksum GetGridChecksum;

static char strCLSID[40]; //CLSID for the data designer window  //{E9B0E6D4-811C-11D0-AD51-00A0C90F5739}
static char queryCLSID[40]; //CLSID for the query designer window

///////////////////////////////////////////////////////////////////////////////
//	CDavinciTest class

IMPLEMENT_TEST(CDavinciTest, CTest, NULL, -1, CDataSubSuite)

BOOL CDavinciTest::RunAsDependent(void)
{
		return FALSE; //never run because it confuses
}

/////////////////////////////////////////////////////////////
//utility function(s)
//TODO(chriskoz) - change the COSource to the new class which would represent database 
// diagrams - Tables, Views, Queries
//returns !=NULL - if you can attach to the active editor & it's caption is valid
//		  ==NULL - cannot attach to the active editor
HWND AddNewDataItem(CString &strDataBase,CString &strDataItem,COSource &SrcFile)
{
    LOG->RecordInfo("Adding new %s to the project", strDataItem);
	MST.DoKeys("%ID"); // "&Insert" "New &Database Item..." UIWB.DoCommand(IDM_INSERT_DATAOBJECT, DC_MNEMONIC);
	if(!MST.WFndWnd("Insert Database Item"/*GetLocString()*/, FW_PART))
	{
		LOG->RecordInfo("Couldn't find Insert Database Item dialog");
		return 0;
	}
	MST.WComboItemClk(GetLabel(/*TODO(chriskoz)*/0x410),strDataBase);
	MST.WListItemClk(GetLabel(/*TODO(chriskoz)*/0x40c),strDataItem);
	MST.WButtonClick(GetLabel(IDOK)) ;
	WaitForInputIdle(g_hTargetProc, 10000);	// Wait for Davinci components to be loaded.
	SrcFile.AttachActiveEditor();
	HWND hwnd=UIWB.GetActiveEditor(); //don't know how to take the handler from COSource
	CString str;
	MST.WGetText(hwnd, str); //should be text window
	if(str.Find(strDataItem)<0) // the active editor title wrong
	{
		if(strDataItem == "Local Query")
		{//damn string is "Local Query" in the listbox while "Query" in document tilte
			if(str.Find(GetLocString(IDSS_QUERY))<0)
				hwnd=NULL;
		}
		else
			hwnd=NULL;
	}
	return hwnd;
} //AddNewDataItem



LPCSTR CDavinciTest::GetDavinciPathFromReg()
{
//poke into registry to retreive the Davinci CLSID & path of the Davinci components
	static char szPath[MAX_PATH]; //REVIEW: MAX_PATH is always > 40 ??
	DWORD size;
	DWORD type = REG_SZ;
	HKEY hSubKey;
//take the Query Designer key
	LONG lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, "MSDTQueryDesigner2\\Clsid", 0, 
					KEY_QUERY_VALUE, &hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("Query Designer Registry operation failed: %d", lResult);
		return NULL;
	}
	size = MAX_PATH;
	lResult = RegQueryValueEx( hSubKey, NULL /*??Default*/, NULL, 
					&type, (LPBYTE)szPath, &size);  
	RegCloseKey( hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("Query Designer Registry operation failed: %d", lResult);
		return NULL;
	}
	strncpy(queryCLSID, szPath, 40);
//take the Data Designer key
	lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, "MSDTDatabaseDesigner2\\Clsid", 0, 
					KEY_QUERY_VALUE, &hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("DataBase Designer Registry operation failed: %d", lResult);
		return NULL;
	}
	size = MAX_PATH;
	lResult = RegQueryValueEx( hSubKey, NULL /*??Default*/, NULL, 
					&type, (LPBYTE)szPath, &size);  
	RegCloseKey( hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("DataBase Designer Registry operation failed: %d", lResult);
		return NULL;
	}
	strncpy(strCLSID, szPath, 40);
//take the path from InProc server of DataBase Designer key
	strcat(strcat(strcpy(szPath,"CLSID\\"), strCLSID), "\\InprocServer32");
	lResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, szPath, 0, 
					KEY_QUERY_VALUE, &hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("DataBase Designer path from Registry failed: %d", lResult);
		return NULL;
	}
	size = MAX_PATH;
	lResult = RegQueryValueEx( hSubKey /*HKEY_CLASSES_ROOT*/, NULL /*??Default*/, NULL, 
					&type, (LPBYTE)szPath, &size);
	RegCloseKey( hSubKey );
	if(lResult != ERROR_SUCCESS)
	{
		LOG->RecordFailure("DataBase Designer path from Registry failed: %d", lResult);
		return NULL;
	}
	_strupr(szPath); //We'd better use uppercase
	char *filename=strrchr(szPath, '\\');
	if(strstr(szPath, ".DLL") !=NULL)
	{
		*filename = 0; //get rid of filename
	}
	return szPath;
}//end of registry operations	


#include <AFXDISP.H>

void CDavinciTest::Run(void)
{
	LPCSTR pdav_path=GetDavinciPathFromReg();
	if(pdav_path == NULL)
	{
		LOG->RecordFailure("Failed to retrieve MsDesigners components. Are they registered?");
		return;
	}
	CDataSubSuite *pSuite=GetSubSuite();
	pSuite->OpenDataProject();
	if(pSuite->ForcePassword(&pSuite->m_DataInfo)!=ERROR_SUCCESS)
	{
			GetLog()->RecordFailure("Coudn't find the DataSource - %s",
				GetSubSuite()->m_DataInfo.m_strDSN);
			throw CTestException("Cannot reopen the Datasource connection; unable to continue.", 
				CTestException::causeOperationFail);
	}
	// now we are at the DataSource Node let's examine the tables
	MST.DoKeys("{Right}"); //expand the tables of active datasource
	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		LOG->RecordFailure("Coudn't deploy the tables");
	
	CString strTable = pSuite->m_SuiteParams->GetTextValue("DESIGNTABLE", "sales");
	int nrows = pSuite->CleanAndInsertTable(strTable);	//here m_ComputerName is established

	HINSTANCE hTestHookDLL = NULL;
	if(!SetCurrentDirectory(pdav_path) )  //TODO how to determine the location?
	{
		LOG->RecordFailure("SetCurrentDirectory failed");
		return;
	}
	CoInitialize(NULL);
	hTestHookDLL = ::LoadLibrary(m_strCWD + "TESTHOOK.DLL");
	if (hTestHookDLL == NULL) 
	{
		LOG->RecordFailure("Unable to load TESTHOOK library. GetLastError: %d", GetLastError());
		CoUninitialize();
		return;
	}
	if(!SetCurrentDirectory(m_strCWD) )
	{
		LOG->RecordFailure("SetCurrentDirectory back to CWD failed");
		::FreeLibrary(hTestHookDLL);
		CoUninitialize();
		return;
	}
	InitializeTestHook = (PInitializeTestHook)::GetProcAddress(hTestHookDLL, "LrInitializeTestHook");
	SendMessageTestHook = (PSendMessageTestHook)::GetProcAddress(hTestHookDLL, "LrSendMessageTestHook");
	UninitializeTestHook = (PUninitializeTestHook)::GetProcAddress(hTestHookDLL, "LrUninitializeTestHook");
	SendGridMessageTestHook = (PSendGridMessageTestHook)::GetProcAddress(hTestHookDLL, "LrSendGridMessageTestHook");
	GetGridChecksum = (PGetGridChecksum)::GetProcAddress(hTestHookDLL, "LrGetGridChecksum");
	if(!InitializeTestHook || !SendMessageTestHook || !UninitializeTestHook || !SendGridMessageTestHook || !GetGridChecksum)
	{
		LOG->RecordFailure("Unable to find TESTHOOK entry points");
		::FreeLibrary(hTestHookDLL);
		CoUninitialize();
		return;
	}

	DataBaseDesignerTest(strTable+pSuite->m_Computername); //table name is appended by computer name for uniqueness
//	/*Query*/ViewDesignerTest(strTable+pSuite->m_Computername,nrows,FALSE);
	ViewDesignerTest(strTable+pSuite->m_Computername,nrows,TRUE);
	/*Query*/ViewDesignerTest(strTable+pSuite->m_Computername,nrows,FALSE);


	::FreeLibrary(hTestHookDLL);
	CoUninitialize();
	//close all windows & collapse the data pane
	MST.DoKeys("%Wo"); //UIWB.DoCommand(ID_FILE_WINDOWS_CLOSEALL, DC_MNEMONIC); //save it

	UIWorkspaceWindow wnd;
	wnd.ActivateDataPane();
	pSuite->CollapseObjects(-1); //collapse the whole tree
	pSuite->m_prj.Close(TRUE); pSuite->m_projOpen=0;
} //CDavinciTest::Run()


#include "davinc\grid.h"
#include "davinc\testhook.h"
void CDavinciTest::DataBaseDesignerTest(LPCSTR szTable)
{
	CDataSubSuite *pSuite=GetSubSuite();
	CString strComboitem = CString(PROJNAME) + ": " + 
		pSuite->m_DataInfo.m_strDataBase + 
		" (" + pSuite->m_DataInfo.m_strServer + ")";
	COSource diagramDoc;
	HWND hwnd; //have to have diagramDoc separately, because of ESD of COSource class
	if( (hwnd=AddNewDataItem(strComboitem, GetLocString(IDSS_DIAGRAM), diagramDoc)) !=NULL )
	{
		pSuite->ForcePassword(&pSuite->m_DataInfo); //new collapse
		MST.DoKeys("{Right}"); //expand the tables of active datasource
		pSuite->ExpandTableNode("");
		MST.DoKeys(szTable);
		CObjectInfo member;
		member.m_strName=szTable;
		member.m_Type = CObjectInfo::TABLE;
		member.m_strOwner = "dbo"; //TODO the current user is the owner
		if(pSuite->CheckObjectProp(&member)!=ERROR_ERROR)
		{
			UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);	// REVIEW: is there a better way? (D&D does not work in CAFE)
			Sleep(1000); //safety

			//REVIEW(chriskoz) bad thing but I have no better idea how to set focus 			
			//diagramDoc.Activate(); does not exist
			RECT rcPage;
			GetClientRect(hwnd, &rcPage);
			ClickMouse( VK_LBUTTON ,hwnd, (rcPage.left + rcPage.right)/2, 
				(rcPage.top+rcPage.bottom)/2);	  // Click on the window
			MST.WFndWndWait(GetLocString(IDSS_DIAGRAM), FW_FOCUS | FW_PART, 2);
			UIWB.DoCommand(ID_EDIT_PASTE, DC_MNEMONIC); // (DC_ACCEL)Ctrl-V does not work in IDE;	// REVIEW: is there a better way? (D&D does not work in CAFE)
			//diagramDoc.Paste(); //bogus, just calling DoCommand(ID_EDIT_PASTE) I have to make new class more robust 			
			Sleep(2000);
			WaitForInputIdle(g_hTargetProc, 10000);
			MST.DoKeys("%VbP"); //"&View" "Ta&ble" "&Pporerties" //change the table display to the grid	UIWB.DoCommand(IDM_VIEW_TABLEPROP, DC_MNEMONIC);
			Sleep(1000);
			WaitForInputIdle(g_hTargetProc, 10000);
		}
		else
		{
			LOG->RecordFailure("%s table not found in the database", szTable);
			return;
		}

		HWND hschema=MST.WFndWndC(strCLSID, "MSPaneWindow", FW_DEFAULT);
		ASSERT(hschema);
		LRESULT res = InitializeTestHook(hschema, 8192);
		ASSERT(res == S_OK);
		//......
		char szTables[256];
		res = SendMessageTestHook(hschema, WM_ListTables, sizeof(szTables)/sizeof(char), (LPARAM)szTables);
		ASSERT(res == S_OK);
		LOG->RecordCompare(strstr(szTables,szTable)!=NULL, "Find the table %s in the diagram", szTable);
//		ObjectLocation Location;
//		res = SendMessageTestHook(hschema, WM_TableLocation, (WPARAM)&Location, (LPARAM)szTable);
		HWND hgrid = (HWND)SendMessageTestHook(hschema, WM_HwndGridOfFieldList, 0, (LPARAM)szTable);  //grid changes as you change the properties
		ASSERT(hgrid);

		GCLS sGCLS;
		char buff[256];
		sGCLS.cbBuf = sizeof(buff)/sizeof(char)-1;
		sGCLS.lpb = (BYTE*)buff;		

		res = SendGridMessageTestHook(hschema, hgrid, G_GETORG, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.irow == 1 && sGCLS.icol == 1);

		MST.DoKeys("{Enter}"); Sleep(500); //set the edit focus on a first cell
		res = SendGridMessageTestHook(hschema, hgrid, G_GETFOCUS, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.irow == 1 && sGCLS.icol == 1);
		sGCLS.icol = 1; //col in grid displaying the column name
		for(sGCLS.irow = 1; sGCLS.irow<256; sGCLS.irow++) //256 is the safety number
		{
			buff[0]=0;
			res = SendGridMessageTestHook(hschema, hgrid, G_GETDATA, 0, (LPARAM)&sGCLS );
			ASSERT(res==S_OK && sGCLS.lpb == (BYTE*)buff && sGCLS.cbBuf == sizeof(buff)/sizeof(char)-1);
			if(strlen(buff) == 0)
			{ // sGCLS.irow holds number of rows+1
				break;
			}
			MST.DoKeys("{DOWN}"); //set the edit focus on a next row
		}
		 ASSERT(sGCLS.irow<256);
		// sGCLS.irow shows the first empty row - place to describe the new table column
		DWORD nextrow=sGCLS.irow;
		res = SendGridMessageTestHook(hschema, hgrid, G_GETFOCUS, 0, (LPARAM)&sGCLS );
		ASSERT(res == S_OK && sGCLS.irow == nextrow && sGCLS.icol == 1);

//		res = SendGridMessageTestHook(hschema, hgrid, G_SETFOCUS, 0, (LPARAM)&sGCLS );
//		ASSERT(res == S_OK);
//this one hangs CAFE(?)  		res = SendGridMessageTestHook(hschema, hgrid, G_BEGINEDIT, 0, (LPARAM)&sGCLS );


#ifdef __CHRISKOZ___
//set the text by mean of Davinci hooks
		strcpy(buff, "new_chardefault");
  		res = SendGridMessageTestHook(hschema, hgrid, G_SETDATA, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.lpb == (BYTE*)buff && sGCLS.cbBuf == sizeof(buff)/sizeof(char)-1);
		
  		res = SendGridMessageTestHook(hschema, hgrid, G_UPDATE, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.lpb == (BYTE*)buff && sGCLS.cbBuf == sizeof(buff)/sizeof(char)-1);
#else
//set the text by mean of typing
		MST.DoKeys("new_chardefault");
		MST.DoKeys("{Enter}");	
#endif
		// sGCLS shld still point to the cell I typed text into
		buff[0]=0;
		res = SendGridMessageTestHook(hschema, hgrid, G_GETDATA, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.lpb == (BYTE*)buff && sGCLS.cbBuf == sizeof(buff)/sizeof(char)-1);
		LOG->RecordCompare(strcmp(buff,"new_chardefault")==0,"Added new column to the table %s",szTable);
		
		buff[0]=0;sGCLS.icol = 2; //the next cell describes the type
		res = SendGridMessageTestHook(hschema, hgrid, G_GETDATA, 0, (LPARAM)&sGCLS );
		ASSERT(res==S_OK && sGCLS.lpb == (BYTE*)buff && sGCLS.cbBuf == sizeof(buff)/sizeof(char)-1);
		LOG->RecordCompare(strcmp(buff,"char")==0, "Default column type 'char' displayed");
		res = UninitializeTestHook(hschema);
		ASSERT(res==S_OK);

//TODO make the real Save method out of source diagram class
		MST.DoKeys("%FS"); //&File.&Save
		UIMessageBox wndMsgSaveAs;
		LOG->RecordCompare(wndMsgSaveAs.AttachActive(), "Save as diagram dialog");
		// wndMsgSaveAs.AttachActive() TODO: title shld be "Save As"
		CString strDiagram = CString(szTable) + " diagram";
		MST.WEditSetText(GetLabel(0x1e24), strDiagram);
		wndMsgSaveAs.ButtonClick(IDOK);
		Sleep(1000);
		WaitForInputIdle(g_hTargetProc, 10000);

		MST.DoKeys("%FC"); //TODO Close method for entire diagram
		WaitForInputIdle(g_hTargetProc, 10000);

		pSuite->ForcePassword(&pSuite->m_DataInfo); //new collapse
		MST.DoKeys("{Right}"); //expand the tables of active datasource
		WaitForInputIdle(g_hTargetProc, 10000);
//check the column "new_chardefault" is in the table "sales1" in data pane
		pSuite->ExpandTable(szTable);
		LOG->RecordCompare(pSuite->CheckObjectProp(&member)!=ERROR_ERROR,"Find table %s", szTable);
		MST.DoKeys("new_chardefault", TRUE);Sleep(500);
		member.m_strName="new_chardefault";
		member.m_Type = CObjectInfo::TABLE; //TODO: bogus, column type is not added yet
		member.m_strOwner = "dbo";
		LOG->RecordCompare(pSuite->CheckObjectProp(&member)!=ERROR_ERROR, "Find the column: %s","new_chardefault");

		pSuite->CollapseObjects(1);

		
//check the diagram in the diagram folder
		pSuite->ExpandDiagramsNode("");
		MST.DoKeys(strDiagram);
		member.m_strName=strDiagram;
		member.m_Type = CObjectInfo::DIAGRAM;
		member.m_strOwner = "dbo"; //TODO owner shld be the current user
		LOG->RecordCompare(pSuite->CheckObjectProp(&member)!=ERROR_ERROR, "'%s' diagram found in data pane",strDiagram);
		
	}
	else
	{
		LOG->RecordFailure("Didn't add new diagram");
	}
}



void CDavinciTest::ViewDesignerTest(LPCSTR szTable,int nrows,BOOL bIsView)
{
	CDataSubSuite *pSuite=GetSubSuite();
	CString strComboitem = CString(PROJNAME) + ": " + 
		pSuite->m_DataInfo.m_strDataBase + 
		" (" + pSuite->m_DataInfo.m_strServer + ")";
	COSource viewDoc;
	HWND hwnd; //have to have viewDoc separately, because of ESD of COSource class
	if( (hwnd=AddNewDataItem(strComboitem, 
		bIsView?GetLocString(IDSS_VIEW):CString("Local Query") /*GetLocString(IDSS_QUERY)*/,
		viewDoc)) !=NULL )
	{
//in addition, we need the Query Diagram Pane
		HWND hdiagram=MST.WFndWndC("Query Diagram Pane", "Desktop", FW_DEFAULT);
		ASSERT(hdiagram);

		pSuite->ForcePassword(&pSuite->m_DataInfo); //new collapse
		MST.DoKeys("{Right}"); //expand the tables of active datasource
		pSuite->ExpandTableNode("");
		MST.DoKeys(szTable);
		CObjectInfo member;
		member.m_strName=szTable;
		member.m_Type = CObjectInfo::TABLE;
		member.m_strOwner = "dbo"; //TODO the current user
		if(pSuite->CheckObjectProp(&member)!=ERROR_ERROR)
		{
			UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);	// REVIEW: is there a better way? (D&D does not work in CAFE)
			Sleep(1000); //safety

			//REVIEW(chriskoz) bad thing but I have no better idea how to set focus 			
			//diagramDoc.Activate(); does not exist
			RECT rcPage;
			GetClientRect(hdiagram, &rcPage); //must click on a diagram pane
			ClickMouse( VK_LBUTTON ,hdiagram, (rcPage.left + rcPage.right)/2, 
				(rcPage.top+rcPage.bottom)/2);	  // Click on the window //TODO: find better way to set the correct focus
			MST.WFndWndWait(GetLocString(IDSS_DIAGRAM), FW_FOCUS | FW_PART, 2); //TODO: what did I mean here in Diagram procedure?????
			UIWB.DoCommand(ID_EDIT_PASTE, DC_MNEMONIC); // (DC_ACCEL)Ctrl-V does not work in IDE;	// REVIEW: is there a better way? (D&D does not work in CAFE)
			//viewDoc.Paste(); //bogus, just calling DoCommand(ID_EDIT_PASTE) I have to make new class more robust 			
			Sleep(2000);
			WaitForInputIdle(g_hTargetProc, 10000);
			//TODO  obsolete in Query design MST.DoKeys("%VbP"); //"&View" "Ta&ble" "&Pporerties" //change the table display to the grid	UIWB.DoCommand(IDM_VIEW_TABLEPROP, DC_MNEMONIC);
			Sleep(1000);
			WaitForInputIdle(g_hTargetProc, 10000);
		}
		else
		{
			LOG->RecordFailure("%s table not found in the database", szTable);
			return;
		}


//select all columns
		MST.DoKeys("{Home}");Sleep(500);
		MST.DoKeys(" "); //space selects the current column
		HWND hschema=MST.WFndWndC(queryCLSID, "MSPaneWindow", FW_DEFAULT);
		ASSERT(hschema);
		HWND hResults=::GetWindow(hschema,GW_CHILD);		
		hResults =::GetWindow(hResults,GW_HWNDNEXT);
		hResults =::GetWindow(hResults,GW_HWNDNEXT); //REVIEW(chriskoz): AdamU sais result pane is always the third sibling
		LRESULT res = InitializeTestHook(hschema, 8192);
		ASSERT(res == S_OK);

if(!bIsView)
{	//add groupping & verifyibg SQL syntax for query
	UIWB.DoCommand(IDM_QUERY_GROUPBYMENU, DC_MNEMONIC); //&Query.&Group By
	WaitForInputIdle(g_hTargetProc, 5000); //safety
	Sleep(500);
	UIWB.DoCommand(IDM_QUERY_VERIFYMENU, DC_MNEMONIC); // &Query.&Verify SQL Syntax
	Sleep(500);
	UIMessageBox wndMsgBox;
	if(wndMsgBox.AttachActive())		
	{
		BOOL bResult=MST.WStaticExists(GetLocString(IDSS_SQLSYNTAX_OK));
		LOG->RecordCompare(bResult,"Verify GROUP BY query option");	
		wndMsgBox.ButtonClick(); //Get rid of the message
	}
	WaitForInputIdle(g_hTargetProc, 5000);
	//REVIEW(chriskoz) : I don't do it for View, because no menu entries are accessible (bug!)
}

	LOG->RecordInfo("Running the Query...");
	UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MENU); //&Tools.&Run
//TODO: how to wait until execution is done?
	Sleep(2000); //temporary
	WaitForInputIdle(g_hTargetProc, 5000); //temporary

//verify the table displays the coret column
/*query*/		UINT tablesID[10];
/*query*/		ANSIDiagramTableInfo TableInfo;
/*query*/		res = SendMessageTestHook(hschema, WM_DiagramEnumTables, 10, (LPARAM)tablesID);
				ASSERT(res == 1);  //one table only
/*query*/		res = SendMessageTestHook(hschema, WM_DiagramTableInfo, tablesID[0], (LPARAM)&TableInfo); //sales1 must be only one table
				ASSERT(res == S_OK);
		LOG->RecordCompare(strcmp(TableInfo.strName,szTable)==0,"Add table %s to the query",szTable);
		
		UINT columnID[10];
		res = SendMessageTestHook(hschema, WM_DiagramEnumTableColumns, tablesID[0], (LPARAM)columnID);
		LOG->RecordCompare(res==TableInfo.cColumns,"Correct number of columns displayed by grid");
 
		ANSIDiagramColumnInfo ColumnInfo;
		for(int iColumn=0; iColumn < TableInfo.cColumns; iColumn++)
		{
			res = SendMessageTestHook(hschema, WM_DiagramTableColumnInfo, columnID[iColumn], (LPARAM)&ColumnInfo);
			ASSERT(res == S_OK);
			//check the column name and if only 0-column is checked
			if(iColumn == 0)
			{
				LOG->RecordCompare(strchr(ColumnInfo.strName, '*')!=NULL, "First column must be '*'"); 
				if(bIsView)
					LOG->RecordCompare(ColumnInfo.lStateClick == 2, "First column selected");
			}
			else
			{
				LOG->RecordCompare(ColumnInfo.lStateClick == 1, "Column #%d not selected", iColumn);
				if(bIsView)
				{ //TODO compare the panes with the columns in result pane
					GCLS sGCLS;
					char buff[256];
					sGCLS.cbBuf = sizeof(buff)/sizeof(char)-1;
					sGCLS.lpb = (BYTE*)buff;		
					sGCLS.irow = 0;  sGCLS.icol = iColumn;
					res = SendGridMessageTestHook(hschema, hResults, G_GETDATA, 0, (LPARAM)&sGCLS );
					ASSERT(res==S_OK);
					LOG->RecordCompare(strcmp(buff,ColumnInfo.strName)==0, "Match colname. Design pane: '%s', result pane: '%s'", ColumnInfo.strName, buff);
				}
			}

		} //for
		//the last column must be the one entered before
		LOG->RecordCompare(strcmp(ColumnInfo.strName,"new_chardefault")==0,"Match the last colname entered 'new_chardefault'");



		if(bIsView)
		{ //verify thet nrows were displayed in the grid by view
			GCLS sGCLS;
			char buff[256];
			sGCLS.cbBuf = sizeof(buff)/sizeof(char)-1;
			sGCLS.lpb = (BYTE*)buff;
			//GROWS dummygrows;
			//memset(&dummygrows, 0, sizeof(dummygrows) );
			//res = SendGridMessageTestHook(hschema, hResults, GCL_GETNUMROWS, 0, (LPARAM)&dummygrows ); //does not work: returns 80004005
			//LOG->RecordInfo("Number of rows: %d",res);
			for( sGCLS.irow = 1,  sGCLS.icol = 1; sGCLS.irow <256 /*safety*/; sGCLS.irow++)
			{				
				res = SendGridMessageTestHook(hschema, hResults, G_SETORG, 0, (LPARAM)&sGCLS ); //maybe it forces the grid to update the rest of rows?
				ASSERT(res==S_OK);
				Sleep(500); //Is there better way to make sure we update all?
				//res = SendGridMessageTestHook(hschema, hResults, G_UPDATE, 0, 0 ); //hangs CAFE (likely because it waits for SETORG to finish & deadlocks
				//ASSERT(res==S_OK);
				res = SendGridMessageTestHook(hschema, hResults, G_GETDATA, 0, (LPARAM)&sGCLS );
				ASSERT(res==S_OK);
				if(buff[0]==0)
					break;
			}
			LOG->RecordCompare(--sGCLS.irow == (UINT)nrows,"Number of rows as in SQL script");
		}
		else
		{//verify that number of nrows is displayed by Query
			GCLS sGCLS;
			char buff[256];
			sGCLS.cbBuf = sizeof(buff)/sizeof(char)-1;
			sGCLS.lpb = (BYTE*)buff;		
			sGCLS.irow = 1;  sGCLS.icol = 1;
			res = SendGridMessageTestHook(hschema, hResults, G_GETDATA, 0, (LPARAM)&sGCLS );
			ASSERT(res==S_OK);
			LOG->RecordCompare(atoi(buff)==nrows,"Number of rows as in SQL script");
		}
		
		res = UninitializeTestHook(hschema);
		ASSERT(res==S_OK);

//TODO make the real Save method out of source diagram class
		MST.DoKeys("%FS"); //&File.&Save
		UIMessageBox wndMsgSaveAs;
		LOG->RecordCompare(wndMsgSaveAs.AttachActive(), "Save As");
		// wndMsgSaveAs.AttachActive() TODO: title shld be "Save As"
		CString strViewQueryname;
if(bIsView)
{
		strViewQueryname =  CString(szTable) +" view";
		MST.WEditSetText(GetLabel(0x1e24),strViewQueryname);
}
else
{
		strViewQueryname =  CString(szTable) +" query";
#pragma message(__LOC__ "get rid of the full path here when the bug is fixed")
		MST.WEditSetText(GetLabel(0x480),m_strCWD + PROJDIR + strViewQueryname); //different save as dialog (it saves into the workspace)
}
		wndMsgSaveAs.ButtonClick(IDOK);
		Sleep(1000);
		WaitForInputIdle(g_hTargetProc, 10000);

		MST.DoKeys("%FC"); //TODO Close method for entire diagram/view/query
		WaitForInputIdle(g_hTargetProc, 10000);
		if(bIsView)
		{ //check the "sales1 view" is in the workspace
		pSuite->ForcePassword(&pSuite->m_DataInfo); //new collapse
		MST.DoKeys("{Right}"); //expand the tables of active datasource
		WaitForInputIdle(g_hTargetProc, 10000);

		pSuite->ExpandViewNode("");
		MST.DoKeys(strViewQueryname);
		member.m_strName=strViewQueryname;
		member.m_Type = CObjectInfo::VIEW;
		member.m_strOwner = "dbo"; //TODO owner is current user
		LOG->RecordCompare(pSuite->CheckObjectProp(&member)!=ERROR_ERROR, "'%s' view found in data pane",strViewQueryname);
		}
		else
		{	//check "sales1 query" is in file view
			UIWorkspaceWindow wnd;
			wnd.ActivateBuildPane(); //haha, what a name!!! Query has nothing to do with build
			MST.DoKeys("{DOWN}{HOME}");
			MST.DoKeys("{NUMPAD*}"); //TODO: assuming there is only one DB project expand the whole damn thing
			MST.DoKeys(strViewQueryname);
			//TODO check the properties of the object (possible?)
			Sleep(500);
			MST.DoKeys("%({Enter})"); //bring up the properties
			LOG->RecordCompare(MST.WStaticExists(strViewQueryname), "find query in file pane");
			MST.DoKeys("{Escape}"); // close the property page
		}

	}
	else
	{
		LOG->RecordFailure("Didn't add new %s", bIsView?"view":"query");
	}
}



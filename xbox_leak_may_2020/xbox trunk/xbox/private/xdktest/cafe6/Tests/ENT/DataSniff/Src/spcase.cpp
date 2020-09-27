///////////////////////////////////////////////////////////////////////////////
//      SPCASE.CPP
//
//      Created by :                    Date :
//              ChrisKoz                                  1/25/96
//
//      Description :
//              Implementation of the SPCase
//

#include "stdafx.h"
#include <io.h>
#include "spcase.h"
#include "sniff.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//	CSPTest class
BOOL ExecSPParam(LPCSTR Params=NULL);
BOOL ExecSPParam(LPCSTR Params /*=NULL*/)
{
	int nSec=2;
	CString str;
	int ret=ERROR_SUCCESS;
	BOOL bExecuting=FALSE;
	while (nSec>0)
	{
		MST.WGetText(NULL, str);
		if(Params!=NULL &&
			str.Find(GetLocString(IDSS_RUN_STOREDPROC)) >=0)
		{
			LPCSTR pparam=strtok((char*)Params," \t\n");
			while(pparam!=NULL)
			{
				MST.DoKeys(pparam); //suply next param
				MST.DoKeys("{Down}"); //go to the next row in a grid
				pparam=strtok(NULL," \t\n");
			}
			MST.WButtonClick(GetLabel(IDOK)); // accept params
			bExecuting=TRUE;
			Sleep(2000);
			break;
		}
		if (nSec > 0) // If we still have some waiting time...
		{
			LOG->RecordInfo("Current wnd title: %s",str);
			LOG->RecordInfo("Waiting up to %d more second(s) for SP to execute",nSec);
			nSec--;
			Sleep(1000);
		}
	} //while */
	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())// If we can attach, an error must've occurred.
	{
		char title[128];
		::GetWindowText(wndMsgBox, title, 128);
		if(MST.WFndWndWait("Server Error(s)"/*GetLocString()*/, FW_PART, 0))
			LOG->Comment("'Server Error(s)' message box detected");
		else
		{
			LOG->Comment("Unknown (%s) message box detected",title);
			ret = ERROR_ERROR;
		}
		//wndMsgBox.ButtonClick(); //does not work for server error(s)
		MST.DoKeys("{ENTER}");
	}
//TODO: maybe wait for the message about the execution finished
	return ret;
} //ExecSPParam

//create the new stored procedure
void NewSP(LPCSTR filename)
{	
	MST.DoKeys("+{F10}");
	Sleep(1000); //xtra safety
	MST.DoKeys("W"); //new stored proc



	COSource srcFile;

	srcFile.AttachActiveEditor();
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC); //select all
	CStdioFile fileSource(filename,	CFile::modeRead | CFile::typeText);
    const int len=255;
    char buf[len];
	while(fileSource.ReadString(buf, len))
	{
		MST.DoKeys(buf,TRUE,3000);
		MST.DoKeys("{ENTER}{HOME}",FALSE,1000);
	}
	UIWB.DoCommand(ID_FILE_SAVE, DC_MNEMONIC); //save it
	::WaitForInputIdle(g_hTargetProc, 5000);
} //NewSP



IMPLEMENT_TEST(CSPTest, CTest, "SP/Trigger/AdHoc objects", -1, CDataSubSuite)


void CSPTest::Run(void)
{
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
		GetLog()->RecordFailure("Coudn't deploy the tables");
	
	TestSPTrigObjects();
	GetSubSuite()->m_prj.Close(TRUE); GetSubSuite()->m_projOpen=0;
} //CSPTest::Run()


void CSPTest::TestSPTrigObjects()
{
	CDataSubSuite *pSuite=GetSubSuite();
	CDataInfo *info=&pSuite->m_DataInfo;


    GetLog()->RecordInfo("Checking object of database: %s", info->GetName());

	UIWorkspaceWindow wnd;



	for(POSITION pos = info->m_listMembers.GetHeadPosition();
		pos!=NULL;info->m_listMembers.GetNext(pos))
	{
		CObjectInfo* pMember=info->m_listMembers.GetAt(pos);// has members.
		if(pMember->m_Type==CObjectInfo::TABLE ||
				  pMember->m_Type==CObjectInfo::VIEW ||
				  pMember->m_Type==CObjectInfo::DIAGRAM)
			continue; // dont test tables/diagrams
		char name[256];
		switch(pMember->m_Type)
		{
			case CObjectInfo::PROC:
			case CObjectInfo::NEWPROC:
				pSuite->ExpandProcedureNode("");
				break;
			case CObjectInfo::TRIGGER:
				strncpy(name,pMember->GetName(),255);
				pSuite->ExpandTable(strtok(name,";")); 
				// (trigger & columns go under table)
				break;
			case CObjectInfo::ADHOC:
				pSuite->ExpandTableNode("");//whatever node makes it here
				break;
			default:
				continue; // object type not supported
		} //switch
		strncpy(name,pMember->GetName(),255);
		if(pMember->m_Type==CObjectInfo::TRIGGER)
		{
			strtok(name,";");
			strcpy(name,strtok(NULL,";"));
		}
// if(ADHOC) do not DoKeys but open from file do not open source view
				//then execute like SP
//if(NEWPROC) do not fail on checkproperties but hit new SP and insert the contents from file
				//then execute like SP
		if(pMember->m_Type==CObjectInfo::ADHOC)
		{
			MST.DoKeys("{DOWN}"); //make it as if some particular table was affected
			UIWB.OpenFile(m_strCWD + pMember->m_strName);
			UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MENU); //&Tools.&Run
			BOOL bResult=ExecSPParam(); //no parameters
		}
		else
		{
			if(pMember->m_Type==CObjectInfo::NEWPROC)
			{	// make new stored procedure & get the text from pMember->m_strOwner;
				MST.DoKeys(name, TRUE);Sleep(500);
				MST.DoKeys("%({Enter})"); //bring up the properties
				if(MST.WFndWndWaitC(name, "Static", FW_FULL, 2))
				{ //correct SP highlighted
					MST.DoKeys("{Escape}"); // close the property page
					MST.DoKeys("{Delete}");
					if(MST.WFndWndWait(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_PART, 2) &&
						MST.WFndWndWaitC(CString("'")+name+"'", "Static", FW_PART, 2))
						MST.DoKeys("{ENTER}"); //OK - delete the proc
					else
						LOG->Comment("Stored Proc: '%s' not found in wkpce",name);
				}
				else
				{
					MST.DoKeys("{Escape}"); // close the property page
					LOG->Comment("Stored Proc: '%s' not found in wkpce",name);
				}
				pSuite->ExpandProcedureNode("");
				NewSP(m_strCWD+pMember->m_strOwner);
				// since we never know where we are after expanding non-exisitng object
				// we are forced to start from the top
				pSuite->ForcePassword(&pSuite->m_DataInfo);
				MST.DoKeys("{Right}"); //expand the tables of active datasource
				pSuite->ExpandProcedureNode("");
			}

			MST.DoKeys(name, TRUE);Sleep(500);
			if(pSuite->CheckObjectProp(pMember)!=ERROR_ERROR)
			{
				char ob_typename[128];
				GetLog()->RecordSuccess("Found the %s name: '%s'",pMember->GetTypeName(),name);
				strncpy(ob_typename,pMember->GetTypeName(),128);
				OpenSourceView(name,ob_typename,info); //openning trigger or SP
				if(pMember->m_Type==CObjectInfo::PROC ||
				   pMember->m_Type==CObjectInfo::NEWPROC)
				{
					LOG->RecordInfo("Running the Stored Procedure...");
					UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MENU); //&Tools.&Run
					WaitForInputIdle(g_hTargetProc, 5000);
					BOOL bResult=ExecSPParam(pMember->m_strParams);
					LOG->RecordCompare(bResult==ERROR_SUCCESS,"Run the SP");
				}
				else //pMember->m_Type==CObjectInfo::
				{ //Don't do anything for trigger
				}
			}
		}
		wnd.Activate(); //returns to the project workspace
		pSuite->CollapseObjects((pMember->m_Type!=CObjectInfo::TRIGGER)?0:1);
	}	// end for
	pSuite->CollapseObjects(1);	//final collapse of all objects
} //TestSPTrigObjects


/////////////////////////////////////////////////////////////////////
// Opens the source window for the object type != TABLE or VIEW
// and checks the create statement
// RETURNS: ERROR_SUCCESS if OK
////////////////////////////////////////////////////////////////////
int CSPTest::OpenSourceView(LPCSTR pszName,CString strType,
										CDataInfo *DataInfo)

{
	int ret = ERROR_SUCCESS;
	CString Name=pszName;
	Name.MakeUpper();
	CString Type="NOTHING";
	if(strType==GetLocString(IDSS_TRIGGER)) //Trigger
		Type="TRIGGER";
	if(strType==GetLocString(IDSS_STOREDPROC)) //Stored Procedure
		Type="PROC";
	MST.DoKeys("{Enter}"); //for open, provided the lack of better handle
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(1000);							// Wait for message box to arrive.

	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
	{
		ret = ERROR_NO_DEFINITION;
		wndMsgBox.ButtonClick();
	}
	else									// A source window must've become active.
	{
		COSource SrcFile;
//		CString title=DataInfo->m_strServer+"!"+DataInfo->m_strName;	
// Excruciatingly Stupid Design: server name is no part of the title anymore
		CString title=CString(pszName) + " : "+strType;	

		SrcFile.AttachActiveEditor();
		HWND hwnd=UIWB.GetActiveEditor(); //don't know how to take the handler from COSource

		CString str;
		MST.WGetText(hwnd, str); //should be text window
		if(str.Find(title) ==-1)
		{
			LOG->RecordInfo("Didn't find the source window for object: %s",pszName);
			ret=ERROR_ERROR;
		}
		CString strFound;int cont=10;
		MST.DoKeys("^{HOME}");
		do{ // skip comments
			MST.DoKeys("+({End})"); // Select to end of line.
			strFound = SrcFile.GetSelectedText();
			strFound.TrimLeft();
			strFound.MakeUpper();
			MST.DoKeys("{Left}{Down}");
		}while((strFound.GetLength( ) == 0 || strFound.Find("--") == 0) && --cont!=0);

		// Remove spaces from both the source and target strings for comparison.

		if (strFound.Find("CREATE") == -1 ||
			strFound.Find(Name) == -1 ||
			strFound.Find(Type) == -1)
		{
			LOG->RecordFailure("Didn't find the create statment for object: %s",pszName);
			ret = ERROR_ERROR;
		}
	}
	
	return ret;
} //CSPTest::OpenSourceView


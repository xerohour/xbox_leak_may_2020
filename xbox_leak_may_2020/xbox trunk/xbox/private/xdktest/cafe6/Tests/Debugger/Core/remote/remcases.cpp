///////////////////////////////////////////////////////////////////////////////
//	REMCASES.CPP
//
//	Created by:				Date:
//			MichMa				8/24/94
//
//	Description:
//			Remote Debugging Breadth Test

#include "stdafx.h"
#include "remcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define proj		"dbg.mak"
#define proj_exe	"dbg.exe"
#define diff_proj	"testgo01.mak"

IMPLEMENT_TEST(CRemoteDebuggingCases, CDebugTestSet, "Remote", -1, CCORESubSuite)

void CRemoteDebuggingCases::PreRun(void)
	{
	fGeneralCasesDone = FALSE;
	// call the base class
	CTest::PreRun();
	}


void CRemoteDebuggingCases::Run(void)
												
	{	
	int platforms = GetUserTargetPlatforms();
	int connections;
	pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	if(platforms & PLATFORM_WIN32_X86)
	
		{
		connections = pconnec->GetUserConnections(PLATFORM_WIN32_X86);
		
		if(connections & CONNECTION_TCP_IP)
			{
			GeneralCases(PLATFORM_WIN32_X86, CONNECTION_TCP_IP);
			TCPIPCases(PLATFORM_WIN32_X86);
			}

		if(connections & CONNECTION_SERIAL)
			{
			GeneralCases(PLATFORM_WIN32_X86, CONNECTION_SERIAL);
			Win32SerialCases(PLATFORM_WIN32_X86);
			}
		}
			 
	if(platforms & PLATFORM_WIN32S_X86)
		{
		// win32s only supports serial connection.
		GeneralCases(PLATFORM_WIN32S_X86, CONNECTION_SERIAL);
		Win32SerialCases(PLATFORM_WIN32S_X86);
		}

	if(platforms & PLATFORM_MAC_68K)
	
		{
		connections = pconnec->GetUserConnections(PLATFORM_MAC_68K);
		
		if(connections & CONNECTION_TCP_IP)
			{
			GeneralCases(PLATFORM_MAC_68K, CONNECTION_TCP_IP);
			TCPIPCases(PLATFORM_MAC_68K);
			}

		if(connections & CONNECTION_SERIAL)
			{
			GeneralCases(PLATFORM_MAC_68K, CONNECTION_SERIAL);
			Mac68kSerialCases();
			}

		// only nt supports appletalk
		if((connections & CONNECTION_APPLETALK) && (GetSystem() & SYSTEM_NT))
			{
			GeneralCases(PLATFORM_MAC_68K, CONNECTION_APPLETALK);
			AppleTalkCases(PLATFORM_MAC_68K);
			}
		}

	if(platforms & PLATFORM_MAC_PPC)
	
		{
		connections = pconnec->GetUserConnections(PLATFORM_MAC_PPC);

		if(connections & CONNECTION_TCP_IP)
			{
			GeneralCases(PLATFORM_MAC_PPC, CONNECTION_TCP_IP);
			TCPIPCases(PLATFORM_MAC_PPC);
			}

		if(connections & CONNECTION_SERIAL)
			{
			GeneralCases(PLATFORM_MAC_PPC, CONNECTION_SERIAL);
			MacPPCSerialCases();
			}

		// only nt supports appletalk
		if((connections & CONNECTION_APPLETALK) && (GetSystem() & SYSTEM_NT))
			{
			GeneralCases(PLATFORM_MAC_PPC, CONNECTION_APPLETALK);
			AppleTalkCases(PLATFORM_MAC_PPC);
			}
		}

	//EXPECT_TRUE(MST.WFndWndWaitC("Could not find the app", "Static", FW_PART, 30));
	//EXPECT_TRUE(MST.WFndWndWaitC("Appletalk error: Specified settings are invalid", "Static", FW_PART, 3));
	//EXPECT_TRUE(MST.WFndWndWaitC("zone 'xxx' could not be found", "Static", FW_PART, 30));
	//EXPECT_TRUE(MST.WFndWndWaitC("Macintosh 'xxx' could not be found", "Static", FW_PART, 30));
	//EXPECT_TRUE(MST.WFndWndWaitC("password is incorrect", "Static", FW_PART, 30));
	}      


// REVIEW (michma): the remote/local exe mismatch case should probably be performed
// on each platform/connection combo, but the remote path cases should only need
// to be performed against one platform/connection.

void CRemoteDebuggingCases::GeneralCases(PLATFORM_TYPE platform, CONNECTION_TYPE connection)

	{
	// this set of cases only needs to be performed against one platform/connection combo.
	if(fGeneralCasesDone)
		return;

	// Initialization
	CString remote_proj_path = pconnec->GetUserRemoteLocalPath(platform) + proj_exe;

	/***************************************************************
	 * start debugging with remote path not set; cancel debugging. *
	 ***************************************************************/
	
	SetProject(proj);
	prj.SetRemotePath("");
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	EXPECT_SUCCESS(dbg.CancelMsg(MSG_REMOTE_EXE_NAME_REQUIRED));

	/****************************************************************************
	 * start debugging with remote path not set; set it and continue debugging. *
	 ****************************************************************************/

	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	EXPECT_SUCCESS(dbg.HandleMsg(MSG_REMOTE_EXE_NAME_REQUIRED, remote_proj_path));
	dbg.Wait(WAIT_FOR_BREAK);
	EXPECT_TRUE(dbg.AtSymbol("WinMain"));
	dbg.StopDebugging();

	/**************************************************************************************
	 * start debugging with a remote executable that does not match the local executable. *
	 **************************************************************************************/
	
	prj.Open(diff_proj);
	prj.SetRemotePath(remote_proj_path);
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	EXPECT_SUCCESS(dbg.CancelMsg(MSG_REMOTE_LOCAL_EXE_MISMATCH));

	fGeneralCasesDone = TRUE;
	}


void CRemoteDebuggingCases::TCPIPCases(PLATFORM_TYPE platform)

	{	
	/***************************************************************
	 * start debugging with neither address nor password provided. *
	 ***************************************************************/
	pconnec->SetAll(platform, CONNECTION_TCP_IP, "", "");
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	//EXPECT_SUCCESS(dbg.CancelMsg());

	/*********************************************************************
	 * start debugging with address not provided (password is provided). *
	 *********************************************************************/
	pconnec->SetPassword(PASSWORD_USER, CONNECTION_TCP_IP);
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	//EXPECT_SUCCESS(dbg.CancelMsg());

	/*********************************************************************
	 * start debugging with password not provided (address is provided). *
	 *********************************************************************/
	pconnec->SetPassword(PASSWORD_USER, CONNECTION_TCP_IP);
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	//EXPECT_SUCCESS(dbg.CancelMsg());

	/*******************************************************
	 * start debugging with non-existent address provided. *
	 *******************************************************/

	/*****************************************************
	 * start debugging with incorrect password provided. *
	 *****************************************************/

	/***************************************************************
	 * start debugging with correct address and password provided. *
	 ***************************************************************/
	}


void CRemoteDebuggingCases::Win32SerialCases(PLATFORM_TYPE platform){}
void CRemoteDebuggingCases::Mac68kSerialCases(void){}
void CRemoteDebuggingCases::MacPPCSerialCases(void){}
void CRemoteDebuggingCases::AppleTalkCases(PLATFORM_TYPE platform){}

	//Win32 Serial Connection
	//start debugging with correct connector, baud rate, and flow control provided (use fastest baud rate and default flow control)
	//start debugging with non-existent connector
	//start debugging with existent but incorrect connector
	//start debugging with incorrect baud rate (connector and flow control correct)
	//start debugging with incorrect flow control (connector and baud rate correct)
	//start debugging with each of the other baud rate speeds
	//start debugging with each of the other flow control types

	//Mac 68K Serial Connection
	//start debugging with correct connector, baud rate, data bits, parity, and stop bits provided (use fastest baud rate and default data bits, parity, and stop bits)
	//start debugging with non-existent connector
	//start debugging with existent but incorrect connector
	//start debugging with incorrect baud rate (connector, data bits, parity, and stop bits correct)
	//start debugging with incorrect data bits (connector, baud rate, parity, and stop bits correct)
	//start debugging with incorrect parity (connector, baud rate, data bits, and stop bits correct)
	//start debugging with incorrect stop bits (connector, baud rate, data bits, and parity correct)
	//start debugging with each of the other baud rate speeds
	//start debugging with each of the other data bit counts
	//start debugging with each of the other parity types
	//start debugging with each of the other stop bit counts

	//Mac PPC Serial Connection
	//start debugging with correct connector provided
	//start debugging with non-existent connector
	//start debugging with existent but incorrect connector

	//Mac Appletalk Connection
	//start debugging with correct zone, machine name, and password provided
	//start debugging with neither zone nor machine name nor password provided
	//start debugging with no zone provided (machine name and password provided)
	//start debugging with no machine name provided (zone and password provided)
	//start debugging with no password provided (zone and machine name provided)
	//start debugging with non-existent zone address provided
	//start debugging with non-existent machine name provided



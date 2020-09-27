/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    MUTest.cpp

Abstract:

     A rewrite of MUProd and MURW to more fully test single MU's at a time. 

Author:

    Dennis Krueger <a-denkru> November 2001

--*/



#include "xtl.h"
#include "xboxp.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mutest.h"
#include "draw.h"
#include "Tsc.h"
#include "tchar.h"
#include "TextScreen.h"



#define MAX_MUS 32



/*************************************************************************************
Function:	init
Purpose:	performs all initialization routines for CPX
Params:		none
Out:		sets up the console window, control-pads, memory units, and cpx modes
Notes:		until direct3d is ported, the console window stuff is irrelevant...
*************************************************************************************/
void CXBoxUtility::Init()
{
	int i;

	DWORD dwRet;
	DWORD insert;
    
	 
    DebugPrint("Welcome to MU Production Test: Built on %s at %s\n", __DATE__,__TIME__);
	 
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

	drInit();

	TscInit(0);

	for(i = 0; i < MAX_MUS; i++)
	{
		// set all MUs to false
		m_fMUStatus[i] = FALSE;
	}

	DebugPrint("Done with initialization!\n");
}

/************************************************************************
*Function: CheckMus() is used to check whether MU is inserted or removed*
*Para: void                                                             *
*Return:void                                                            *
*************************************************************************/
void CXBoxUtility::CheckMus()
{
	DWORD insert, remove;
	int i;
	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insert, &remove)) {
		for (i = 0; i < MAX_MUS; i++) 
		{
			if(remove & (1 << i)) 
			{ 
				m_fMUStatus[i] = FALSE;
			}

			if(insert & (1 << i)) 
			{
				m_fMUStatus[i] = TRUE;
			}
		}
	}
}



#define TestPattern "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/*******************************************************************************
*Main Function
********************************************************************************/


void __cdecl main()
{

	CXBoxUtility TheApp;
	TheApp.Init();
	TheApp.Run();
}


void CXBoxUtility::Run()
{

	DWORD		dwResult;
	int			iFormatIndex;
	WCHAR		wcTempBuffer[400];
	BOOL		fResult;
	int			port = 0, slot = 0, temp;
	char		cDrive;


	Sleep(16);

	while(1) //loop until we're supposed to quit...
	{
		//Print out the title
		CTextScreenLine * pTitleLine = new CTextScreenLine(150,60,TITLECOLOR,L"MU Read/Write/Compare all free space",0);
		m_ThisScreen.Add(pTitleLine);
		CTextScreenLine * pInsertLine = new CTextScreenLine(150,120,REDCOLOR,L"Insert Memory Unit");
		m_ThisScreen.Add(pInsertLine);
		UpdateScreen();

		while(1)
		{
			CheckMus();
			for(int portctr = 0; portctr < MAX_MUS; portctr++) 
			{
				port = portctr &  15;
				slot = portctr < 16 ? 0 : 1;
				
				if(m_fMUStatus[portctr] == TRUE) 
				{
					m_ThisScreen.ClearScreen();
					CTextScreenLine * pTitleLine = new CTextScreenLine(150,60,TITLECOLOR,L"MU Read/Write/Compare all free space",0);
					m_ThisScreen.Add(pTitleLine);
					CTextScreenLine * pCautionLine = new CTextScreenLine(120,30,CORALCOLOR,L"CAUTION: Do not unplug MU during test!",0);
					m_ThisScreen.Add(pCautionLine);
					UpdateScreen();
					// first check if mountable
					int temp = XMountMURoot(port, slot, &cDrive);
					char cRootName[10];
					sprintf(cRootName,"%c:\\",cDrive);
					if(ERROR_UNRECOGNIZED_VOLUME == temp)
					{
						// drive not formated, notify and format
						CTextScreenLine * pFormatLine = new CTextScreenLine(170,90,REDCOLOR,L"MU Unformatted, formatting...",0);
						iFormatIndex = m_ThisScreen.Add(pFormatLine);
						UpdateScreen();
						fResult = FormatMU(port,slot);
						if(TRUE == fResult)
						{
							CTextScreenLine * pFormatLine1 = new CTextScreenLine(170,90,REDCOLOR,L"MU needed format and format failed!",0);
							m_ThisScreen.Replace(pFormatLine1,iFormatIndex);
							UpdateScreen();
							break;
						} else
						{
							CTextScreenLine * pFormatLine1 = new CTextScreenLine(170,90,LTGREENCOLOR,L"MU successfully formatted",0);
							m_ThisScreen.Replace(pFormatLine1,iFormatIndex);
							temp = XMountMURoot(port, slot, &cDrive);
							if(temp != ERROR_SUCCESS)
							{
								CTextScreenLine * pFormatLine2 = new CTextScreenLine(170,90,REDCOLOR,L"Unable to Mount MU - Fail",0);
								m_ThisScreen.Add(pFormatLine2);
								break;
							}
							sprintf(cRootName,"%c:\\",cDrive);

							
						}
					} else
					{
						CTextScreenLine * pFormatLine = new CTextScreenLine(200,90,WHITECOLOR,L"MU Format OK",0);
						iFormatIndex = m_ThisScreen.Add(pFormatLine);

					}
					UpdateScreen();
					Sleep(1000);
					ULARGE_INTEGER FreeBytesAvail,TotalBytes,TotalFree;
					fResult = GetDiskFreeSpaceEx(
								cRootName,
								&FreeBytesAvail,
								&TotalBytes,
								&TotalFree
								);
					WCHAR wcRootBuffer[30];

					swprintf(wcTempBuffer,L"Total Bytes = %d",
										TotalBytes
										);

					WCHAR * pwcTemp;
					pwcTemp = (WCHAR *) malloc((wcslen(wcTempBuffer)*2)+2);
					memcpy(pwcTemp,wcTempBuffer,(wcslen(wcTempBuffer)*2)+2);
					CTextScreenLine * pInfoLine1 = new CTextScreenLine(160,140,WHITECOLOR,pwcTemp,0);
					m_ThisScreen.Add(pInfoLine1);

					swprintf(wcTempBuffer,L"Available Bytes = %d",
										FreeBytesAvail
										);
					pwcTemp = (WCHAR *) malloc((wcslen(wcTempBuffer)*2)+2);
					memcpy(pwcTemp,wcTempBuffer,(wcslen(wcTempBuffer)*2)+2);
					CTextScreenLine * pInfoLine2 = new CTextScreenLine(160,160,WHITECOLOR,pwcTemp,0);
					m_ThisScreen.Add(pInfoLine2);

					swprintf(wcTempBuffer,L"Total Free = %d",
										TotalFree
										);

					pwcTemp = (WCHAR *) malloc((wcslen(wcTempBuffer)*2)+2);
					memcpy(pwcTemp,wcTempBuffer,(wcslen(wcTempBuffer)*2)+2);
					CTextScreenLine * pInfoLine3 = new CTextScreenLine(160,180,WHITECOLOR,pwcTemp,0);
					m_ThisScreen.Add(pInfoLine3);
					UpdateScreen();

					// now generate file for test
					// make buffer for write
					long BuffSize;
					BuffSize = FreeBytesAvail.LowPart - 500;
					if(BuffSize < 100)
					{
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,200,REDCOLOR,L"Not enough freespace on MU for test",0);
						m_ThisScreen.Add(pErrorLine);
						UpdateScreen();
						goto UnMount;
					}

					char * pWriteBuffer = (char * ) malloc(BuffSize);
					if(!pWriteBuffer)
					{
						// not enough memory error out
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,200,REDCOLOR,L"Out of Memory",0);
						m_ThisScreen.Add(pErrorLine);
						UpdateScreen();
						goto UnMount;
					}

					swprintf(wcTempBuffer,L"Writing file of %d bytes...",BuffSize);
					pwcTemp = (WCHAR *) malloc((wcslen(wcTempBuffer)*2)+2);
					memcpy(pwcTemp,wcTempBuffer,(wcslen(wcTempBuffer)*2)+2);
					CTextScreenLine * pWriteLine = new CTextScreenLine(160,200,WHITECOLOR,pwcTemp,0);
					int iWriteIndex = m_ThisScreen.Add(pWriteLine);
					UpdateScreen();

					// now fill test buffer with test pattern
					int iPatternSize = sizeof(TestPattern);
					long NumPatterns = BuffSize / iPatternSize;
					char * pcTemp, * pTempDest;
					pTempDest = pWriteBuffer;
					for(long i = 0; i < NumPatterns; i ++)
					{
						memcpy(pTempDest,TestPattern,sizeof(TestPattern));
						pTempDest += iPatternSize;
					}
					char cFileName[30];
					sprintf(cFileName,"%sTestFile",cRootName);
					// buffer ready for write, do it
					HANDLE hFile = CreateFile(
						cFileName,
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);

					if(INVALID_HANDLE_VALUE == hFile)
					{
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,200,REDCOLOR,L"Unable to create file on MU",0);
						m_ThisScreen.Replace(pErrorLine,iWriteIndex);
						UpdateScreen();
						goto UnMount;
					}
					DWORD dwBytesWritten;
					fResult = WriteFile(
						hFile,
						pWriteBuffer,
						BuffSize,
						&dwBytesWritten,
						NULL
						);
					if(FALSE == fResult)
					{
						// write failed
						dwResult = GetLastError();
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,200,REDCOLOR,L"WriteFile failed",0);
						m_ThisScreen.Replace(pErrorLine,iWriteIndex);
						UpdateScreen();
						goto UnMount;
					}

					CTextScreenLine * pWrite1Line = new CTextScreenLine(160,200,LTGREENCOLOR,L"Write Succeeded",0);
					m_ThisScreen.Replace(pWrite1Line, iWriteIndex);
					UpdateScreen();
					
					char * pReadBuffer = (char *) malloc(BuffSize);
					if(!pReadBuffer)
					{
						// not enough memory error out
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,200,REDCOLOR,L"Out of Memory - Unable to create read buffer",0);
						m_ThisScreen.Add(pErrorLine);
						UpdateScreen();
						goto UnMount;
					}
					CTextScreenLine * pReadLine = new CTextScreenLine(160,220,WHITECOLOR,L"Reading...",0);
					int iReadIndex = m_ThisScreen.Add(pReadLine);
					UpdateScreen();
					// set file pointer to beginning of file
					SetFilePointer(hFile,0,0,FILE_BEGIN);
					DWORD dwBytesRead;
					fResult = ReadFile(hFile,
						pReadBuffer,
						BuffSize,
						&dwBytesRead,
						NULL);
					if(FALSE == fResult)
					{
						dwResult = GetLastError();
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,220,REDCOLOR,L"Read of File from MU failed",0);
						m_ThisScreen.Replace(pErrorLine,iReadIndex);
						UpdateScreen();
						goto UnMount;
						// fail
					}
					CTextScreenLine * pReadLine1 = new CTextScreenLine(160,220,LTGREENCOLOR,L"Read Succeeded",0);
					m_ThisScreen.Replace(pReadLine1,iReadIndex);
					UpdateScreen();

					// delete the file on the MU
					CloseHandle(hFile);
					fResult = DeleteFile(cFileName);
					if(FALSE == fResult)
					{
						// delete failed
						CTextScreenLine * pErrorLine = new CTextScreenLine(160,240,REDCOLOR,L"Delete of test file from MU failed",0);
						m_ThisScreen.Add(pErrorLine);
						UpdateScreen();
						goto UnMount;
					}

					// compare buffers
					for(long i = 0; i < BuffSize; i++)
					{
						if(pReadBuffer[i] != pWriteBuffer[i])
						{
							swprintf(wcTempBuffer,L"Compare Failed at byte %d",i);
							pwcTemp = (WCHAR *) malloc((wcslen(wcTempBuffer)*2)+2);
							memcpy(pwcTemp,wcTempBuffer,(wcslen(wcTempBuffer)*2)+2);
							CTextScreenLine * pCompareLine = new CTextScreenLine(160,240,REDCOLOR,pwcTemp,0);
							m_ThisScreen.Add(pCompareLine);
							UpdateScreen();
							goto UnMount;
						}
					}
					free(pReadBuffer);
					free(pWriteBuffer);
					CTextScreenLine * pCmpLine = new CTextScreenLine(160,240,LTGREENCOLOR,L"Compare Succeeded",0);
					m_ThisScreen.Add(pCmpLine);
					UpdateScreen();

	UnMount:
					Sleep(1000);
					XUnmountMU(port,slot);
//					m_fMUStatus[portctr] = FALSE; //done with this MU
					CTextScreenLine * pLastLine = new CTextScreenLine(160,280,YELLOWCOLOR,L"Test Complete",0);
					m_ThisScreen.Add(pLastLine);
					UpdateScreen();

					// wait here for MU removal
					while(1)
					{
						CheckMus();
						if(FALSE == m_fMUStatus[portctr])
							break;
						Sleep(500);
					}
					m_ThisScreen.ClearScreen();
					UpdateScreen();
					break;
                        
				}
				 
			} // for
			break;
		} // while
	} // while

	//we should theoretically never get here on xbox......
}

void CXBoxUtility::UpdateScreen()
{
	CTextScreenLine * pThisLine;
	// draw text on the screen
	int ScreenLines = m_ThisScreen.GetLineCount();
	drCls();
	for(int i = 0; i<ScreenLines;i++)
	{
		// get current line
		pThisLine = m_ThisScreen.GetLine(i);
		if(pThisLine)
		{
			drSetSize(50, 100, pThisLine->m_dwColor, 0xff000000); //set the color
			drPrintf(pThisLine->m_sx,
					pThisLine->m_sy,
					pThisLine->m_strText);
		}
	}
	drShowScreen();

    return;
}


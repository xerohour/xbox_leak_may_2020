/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    murw.cpp

Abstract:

     Using muprod.cpp as the base, modified and changed to test MU functions: read, write, 
	  and compere two files to be identical. 

Author:

    Bingdong Li (a-bingli) March 2001

--*/

#include "xtl.h"
//#include <afxpriv.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "muprod.h"
//#include "ccl.h"
#include "draw.h"
#include "Tsc.h"
#include "tchar.h"

#define MAX_MUS 32

//Global variable 

int removeflag =0;

BOXSTATUS gMuStatus[MAX_MUS];

/*************************************************************************************
Function:	init
Purpose:	performs all initialization routines for CPX
Params:		none
Out:		sets up the console window, control-pads, memory units, and cpx modes
Notes:		until direct3d is ported, the console window stuff is irrelevant...
*************************************************************************************/
void init()
{
	int i;

	DWORD dwRet;
	DWORD insert;
    
	 
    DebugPrint("Welcome to MU Production Test: Built on %s at %s\n", __DATE__,__TIME__);
	 
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

	drInit();

	TscInit(0);

	DebugPrint("Getting MU devices...\n");
	insert = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

	for(i = 0; i < MAX_MUS; i++) {
		if(insert & (1 << i)) {
			gMuStatus[i] = testme;
		} else {
			gMuStatus[i] = notyet;
		}
	}

	DebugPrint("Done with initialization!\n");
}

/************************************************************************
*Function: CheckMus() is used to check whether MU is inserted or removed*
*Para: void                                                             *
*Return:void                                                            *
*************************************************************************/
void CheckMus()
{
	DWORD insert, remove;
	int i;
	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insert, &remove)) {
		for (i = 0; i < MAX_MUS; i++) {
			if(remove & (1 << i)) { 
				gMuStatus[i] = notyet;
				removeflag=1;
			}

			if(insert & (1 << i)) {
				gMuStatus[i] = testme;
				removeflag=0;
			}
		}
	}
}


/*************************************************************************************
Function:	deinit
Purpose:	de-initialize all cpx stuff (NONE, since we never should quit!)
Params:		none
Out:		un-registers the callbacks for controlpads and MUs, tells all cpx modes to quit
Notes:		We shouldn't ever go here, since xbox only has one process, & this is it...
*************************************************************************************/
void deinit()
{
	DebugPrint("Quitting???? ...");

	drDeinit();

	DebugPrint("Deinit is finished.\n");

	_asm int 3;
}


/*************************************************************************************
Function:	test() is used as stub to call WriteReadTest
Purpose:	the entry-point of WriteReadTest.
Params:		port, slot, bad block, failed flags
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
 
BOXSTATUS test(int port, int slot, int &BB1, int &BB2, int &BB3, int &readfailed, int &writefailed, int &comparefailed)
{
	__int64 tim;
	CHAR drive;
	BOXSTATUS overall = passed;
	int temp;

	//Mount MU
	temp = XMountMU(port, slot, &drive);
    if(temp != 0) {
		DebugPrint("XMountMU failed: 0x%x\n", GetLastError());
        readfailed = -1;
		overall = failed;
		goto nothing;
	}

   //Write and Read file to and from the MU
	if(failed == WriteReadTest(drive,readfailed,writefailed,comparefailed)) {
		DebugPrint("WriteReadTest(%c:) failed.\n", drive);
		overall = failed;
		//goto unmount;
	}


//unmount:
	if(temp!=0) goto nothing;

	temp = XUnmountMU(port, slot);
	//DebugPrint("XUnmountMU: %d\n", temp);
	if(temp != 0) {
		overall = failed;
	}

  nothing:
	return overall;
}



/*******************************************************************************
*Main Function
********************************************************************************/


void __cdecl main()
{
//	_asm int 3;

	int i,t,p,f,BB1 = 0,BB2 = 0, BB3 = 0;
	long double counter=0,oldc=0;;

	int readfailed =0,writefailed =0,comparefailed =0;

	init();
	int port = 0, slot = 0;
//char *currtime,*begitime;
//	TCHAR *currenttime;
//	TCHAR *begintime1;
//	TCHAR begintime[256];
//	TCHAR testing[256]= L"testing";

	//CHAR   *begintime1;
	//LPWSTR begintime = new WCHAR;
//	WCHAR begintime[256];


	struct tm *ptr;
	time_t lt;

	Sleep(16);

	while(1) //loop until we're supposed to quit...
	{
		//Print out the title
		drCls();
	    drPrintf(30,30,L"Xbox MU Read_Write_Compare Test!");
	    drShowScreen();

		//initilize as 0, there is on error now
        readfailed=writefailed=0;
		comparefailed=0;
		counter=0;

		CheckMus();
		for(i = 0; i < MAX_MUS; i++) {
			port = i &  15;
			slot = i < 16 ? 0 : 1;
			
			if(gMuStatus[i] == testme) {
				 
				lt=time('\0');
				ptr=localtime(&lt);
//				begintime1 = "0"; //_wasctime(ptr);
				/*** This is the bug, I tried to fix it, when drPrint, a small sqare is followed, it should not be
				 **/
                //wcscpy(begintime,begintime1);

				//DebugPrint("early begintime1=%s\n", begintime1);
//				swprintf(begintime,L"%s", begintime1);
				//DebugPrint("after swprintf begintime= %s\n",begintime);

				while(1) {
									    
					    //DebugPrint("Testing MU at port %d, slot %d\n", port, slot);
					
						drCls();
				        drPrintf(20,0,L"Xbox MU Read_Write_Compare             Testing ......");
//				        drPrintf(20,40,L"Begin Time: %s   ",begintime);
						
						CheckMus();
                        if(removeflag) break;

						gMuStatus[i] = test(port, slot, BB1, BB2, BB3,readfailed, writefailed,comparefailed);
						counter++;
                        
						drPrintf(50,100,L"Read failed: %d ",readfailed);
						drPrintf(50,120,L"Write failed: %d ",writefailed);
                        drPrintf(50,140,L"Compare failed: %d ",comparefailed);
						
						if(readfailed !=-1) oldc= abs((int)counter);

						if(readfailed == -1) { 

							drPrintf(300,380,L"XMountMU failed, Error: 0x%x", GetLastError());
							drLine(280,360,280,420,0x00ff0000);
							drLine(600,360,600,420,0x00ff0000);
							drLine(280,360,600,360,0x00ff0000);
							drLine(280,420,600,420,0x00ff0000);
							
							drPrintf(50, 180,L"Test #: %d", oldc);

						}

						if(readfailed != -1) drPrintf(50, 180,L"Test #: %d", abs((int)counter));

                        lt=time('\0');
			    	    ptr=localtime(&lt);					 

//						currenttime = "0"; //_wasctime(ptr);

//                        drPrintf(20, 60, L"Current Time: %s  ", currenttime);

						drShowScreen();
						
						CheckMus();

						if(removeflag) break;


				}

                        
			}
			 
		}
	}

	//we should theoretically never get here on xbox......
	deinit();
}


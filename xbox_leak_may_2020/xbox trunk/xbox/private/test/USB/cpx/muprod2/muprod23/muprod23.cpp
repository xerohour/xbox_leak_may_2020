/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    muprod23.cpp

Abstract:

     This version was built based on former version muprod.cpp.

     Initializes all global variables, defines user data types, contains main function to test the four MUs independently 
	 at the same time using multithreads. When the first bad block failed, we do not test the others in order to save time.


Author:

    Bing Li(a-bingli) Feb. 2001

--*/

 
 

#include <windows.h>
#include <stdlib.h>
#include <process.h>

#include "xtl.h"
#include <stdio.h>
#include "muprod.h"
#include "draw.h"
#include "Tsc.h"


#define MAX_MUS 32

BOXSTATUS gMuStatus[MAX_MUS];

//CRITICAL_SECTION criter;
HANDLE    hIOMutex; 

typedef struct parameterList *Paralist;


static const DWORD USBPortMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };


// this structure contains everything needed for the thread
struct parameterList { 
    int i;                        //index of thread
    int BB1;                      //bad block count 1
    int BB2;                       //bad block count 2 
    int BB3;                       //bad block count 3
    int port;                      // port number
    int slot;                      //slot number
	int inserted;                  //inserted flag
	int removed;                   //removed flag
	int showflag;                   // show to screen flag
	int printposition;              // where should be printed on the screen
	failedreason fail;              // the reason test is failed
};


//Global variables
Paralist testparalist[MAX_MUS];

HANDLE hDuke[4] = { NULL, NULL, NULL, NULL };

//Function declaration
void ShowAllInformation(Paralist list[]);
void ReleaseMemory(Paralist list[]); // may not useful
void  OperateMotors(WORD wMotorCode, BOOL fOn);
void MaintainMotor();
bool InputCheckRumbleOff(void);
bool InputCheckRumbleOn(void);
void InputDukeInsertions(DWORD add, DWORD remove);


 
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


	// start Motor Maintaince thread
	_beginthread((void (__cdecl*)(void *))MaintainMotor, NULL, NULL);
	DebugPrint("Done with initialization!\n");
}

void CheckMus()
{
	DWORD insert, remove;
	int i,j=0;
	//want to know whenever the devices have been changed
	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insert, &remove)) {
		for (i = 0; i < MAX_MUS; i++) {
			if(remove & (1 << i)) { 
				gMuStatus[i] = notyet;
				testparalist[i]->removed=1;
				testparalist[i]->showflag=1;
			}

			if(insert & (1 << i)) {
				gMuStatus[i] = testme;
				testparalist[i]->inserted=1;
				testparalist[i]->removed=0;
				testparalist[i]->showflag=1;
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

}


/*************************************************************************************
Function:	test()
Purpose:	call all other low level test functions
Params:		the structure of this thread
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/


void test( void *testparalist )
{
	__int64 tim;
	CHAR drive;
	BOXSTATUS overall = passed;

	//HANDLE    hIOMutex= CreateMutex (NULL, FALSE, NULL);

	int temp;
    int port, slot, BB1, BB2, BB3;
    BB1=((struct parameterList *)testparalist)->BB1;
    BB2=((struct parameterList *)testparalist)->BB2;
    BB3=((struct parameterList *)testparalist)->BB3;
    port=((struct parameterList *)testparalist)->port;
    slot=((struct parameterList *)testparalist)->slot;

	// start motor on appropriate Duke and Side

#ifdef DOLOWLEVELTESTS
    if(failed == LowLevelTest(port, slot, BB1, BB2, BB3))
    {
        overall = failed;
		((struct parameterList *)testparalist)->fail=lowleveltest;

		DebugPrint("Lowleveltest failed \n");
		goto nothing;
    }
#endif

	if(failed == FormatTest(port, slot)) {
		overall = failed;
		((struct parameterList *)testparalist)->fail=formattest;
		DebugPrint("Formattest failed \n");
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
		((struct parameterList *)testparalist)->fail=verifytest;
		DebugPrint("Verifytest failed \n");
		goto nothing;
	}

	temp = XMountMU(port, slot, &drive);
	DebugPrint("XMountMU: %d -- ", temp);
	DebugPrint("Drive: %c\n", drive);
	if(temp != 0) {
		DebugPrint("XMountMU failed: 0x%x\n", GetLastError());
		((struct parameterList *)testparalist)->fail=xmountmu;
		overall = failed;
		goto nothing;
	}

	Sleep(1000); //asynchronous

	
	if(failed == CountSectorWriteTest(drive)) {
		DebugPrint("CountSectorWriteTest(%c:) failed.\n", drive);
		((struct parameterList *)testparalist)->fail=countsectorwritetest;
		overall = failed;
		goto unmount;
	}

	Sleep(1000); //asynchronous


	// create a mutex handle to pretect the shared FILE * f in WriteReadTest(drive) function

   WaitForSingleObject( hIOMutex, INFINITE );
	if(failed == WriteReadTest(drive)) {
		DebugPrint("WriteReadTest(%c:) failed.\n", drive);
		((struct parameterList *)testparalist)->fail=writereadtest;
		overall = failed;
		ReleaseMutex( hIOMutex); // ensure mutex is released on failure
		goto unmount;
	}
   ReleaseMutex( hIOMutex);

	overall = passed;

unmount:
	temp = XUnmountMU(port, slot);
	DebugPrint("XUnmountMU: %d\n", temp);
	if(temp != 0) {
		overall = failed;
	}


	if(failed == FormatTest(port, slot)) {
		overall = failed;
		DebugPrint("Formattest failed \n");
		((struct parameterList *)testparalist)->fail=formattest;
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
		((struct parameterList *)testparalist)->fail=verifytest;
		DebugPrint("Verifytest failed \n");
		goto nothing;
	}




nothing:

    ((struct parameterList *)testparalist)->BB1=BB1;
    ((struct parameterList *)testparalist)->BB2=BB2;
    ((struct parameterList *)testparalist)->BB3=BB3;
    ((struct parameterList *)testparalist)->port=port;
    ((struct parameterList *)testparalist)->slot=slot;
	((struct parameterList *)testparalist)->showflag=1;
    
     gMuStatus[((struct parameterList *)testparalist)->i] =overall;

}


/*******************************************************************
*Function: Init_Int_Array() is used intiliaze integer arries to 0  *
*Parameters: a pointer to array, an interger of the size           *
*Return: void                                                      *
********************************************************************/
void Init_Int_Array( int array[], int size)
{
 int i=0;
 for(i=0;i<size;i++) array[i]=0;
}


/********************************************************************
*Function: main()
*********************************************************************/
void __cdecl main()
{

    int i=0,t=0,p=0,f=0,oldthread[MAX_MUS];
	init();
    Init_Int_Array(oldthread, MAX_MUS);

    //create a mutex;
	hIOMutex= CreateMutex (NULL, FALSE, NULL);


	int port=0, slot=0;
	float x1=0,y1=0;
   // static int count=0;
	Sleep(16);
    int screenflag=0,  muflag=0, showflag=0;
    float printflag=0.0;
    

	drCls();
	drPrintf(40,40,L"Xbox MU Test Version 2.00 - MU ASIC!");
	drShowScreen();
    
//allocate space and initiliaze all
	for(i=0;i<MAX_MUS;i++) {
		testparalist[i] = new (struct parameterList);
		testparalist[i]->port = -1;
        testparalist[i]->slot = -1;
        testparalist[i]->BB1=-1;
        testparalist[i]->BB2=-1;
        testparalist[i]->BB3=-1;
        testparalist[i]->i=-1;
		testparalist[i]->inserted=0;
		testparalist[i]->removed=0;
		testparalist[i]->showflag=0;
		testparalist[i]->printposition=0;
        testparalist[i]->fail=unknowreason;

	}

	while(1) //loop until we're supposed to quit...
	{
        
        screenflag=0;
        printflag=20.0;
        muflag=0;
        showflag=0;

        CheckMus();
	
        for(i = 0; i < MAX_MUS; i++) {
						
            //this device is ready to be tested
            if(gMuStatus[i] == testme) {
				DebugPrint("Testing MU at port %d, slot %d\n", port, slot);
                       
                port = i &  15;
                slot = i < 16 ? 0 : 1;

				if(port==0) testparalist[i]->printposition=0;
				else if(port==1) testparalist[i]->printposition=1;
				else if(port==2) testparalist[i]->printposition=2;
				else if(port==3) testparalist[i]->printposition=3;

               
				
				testparalist[i]->port = port;
                testparalist[i]->slot = slot;
                testparalist[i]->BB1=-1;
                testparalist[i]->BB2=-1;
                testparalist[i]->BB3=-1;
                testparalist[i]->i=i;
				testparalist[i]->fail=unknowreason;

                gMuStatus[i]= pending;
                

                _beginthread((void (__cdecl*)(void *))test, NULL, testparalist[i]);
                 

 
            }
			
			// anyone is changed to be showed on the screen, we will cal ShowAllInformation()
			if(testparalist[i]->showflag==1) showflag=1;
        }
        
        
        if(showflag==1) ShowAllInformation(testparalist);
        
        
	
} //end of while loop


    //release all space used
	ReleaseMemory(testparalist);

	//we should theoretically never get here on xbox......
	deinit();
}



/******************************************************************************************************
*Functions: ShowAllInformation(Paralist list[]) is used to show all information to the screen whenever*
*           the screen need to be updated.                                                            *
*Parameter: array of structure of Paralist.                                                           *
*******************************************************************************************************/

void ShowAllInformation(Paralist list[])
{
  
    int i=0,oldport=-1,screenflag=0,printflag=-1; // it should not be

    float x1=0,x2=0,x3=0,x4=0,y1=0,y2=0,y3=0,y4=0;
    float m1=0,m2=0,m3=0,m4=0,n1=0,n2=0,n3=0,n4=0;
    drCls();

	drPrintf(130,440,L"MU Test Version 2.00");
	drPrintf(325,440,L"Running test ......  ");
	drLine(0,235,640,235,0x000000ff);  //line for range
	drLine(310,0,310,480,0x000000ff);  //line for range

	 
	  
       
   
	
    for(i=0; i< MAX_MUS; i++) {

		//this MU has been removed, so show remove result from the screen

		if(testparalist[i]->removed==1) {

          testparalist[i]->showflag = 0;
		  screenflag=1;

          switch (testparalist[i]->printposition) {
            case 0: x1=40; y1=40; y2=60; y3=60; y4=100; break;  
            case 1: x1=320; y1=40;y2=60; y3=60; y4=100; break;  
            case 2: x1=40; y1=240; y2=260; y3=280; y4=300; break;  
            case 3: x1=320; y1=240;y2=260; y3=280; y4=300; break;  
            
            default: DebugPrint(" printflag is wrong \n");
          }
		drPrintf(x1,y1,L"Port#=%d,Slot#=%d removed", list[i]->port, list[i]->slot);
        drPrintf(x1,y2,L"                              ");
		drPrintf(x1,y3,L"                              ");
		drPrintf(x1,y4,L"                              ");
			
		if(gMuStatus[i] == failed) {
		  switch(testparalist[i]->printposition) {
		    case 0: x1=130; y1=100; x2=230; y2=200; x3=210; y3=220; x4=110; y4=120;
                    n1=210; m1=100; n2=230; m2=120; n3=130; m3=220; n4=110; m4=220;
                    break;
            case 1: x1=450; y1=100; x2=550; y2=200; x3=530; y3=220; x4=430; y4=120; 
                    n1=530; m1=100; n2=550; m2=120; n3=450; m3=220; n4=430; m4=200;
                    break;     
            case 2: x1=130; y1=320; x2=230; y2=420; x3=210; y3=440; x4=110; y4=340; 
                    n1=210; m1=320; n2=230; m2=340;n3=130;m3=440;n4=110;m4=420;
                    break; 
            case 3: x1=450; y1=320; x2=550; y2=420; x3=530; y3=440; x4=430; y4=340; 
                    n1=530; m1=320; n2=550; m2=340;n3=450;m3=440;n4=430;m4=420;
                    break;                             
            default: DebugPrint(" printflag is wrong \n");
		  }
				
		 drQuad(x1, y1, 0xff000000, x2,  y2, 0xff000000, x3, y3, 0xff000000, x4, y4, 0xff000000);
	     drQuad(n1, m1, 0xff000000, n2,  m2, 0xff000000, n3, m3, 0xff000000, n4, m4, 0xff000000);
		} else if(gMuStatus[i] == passed){
		   switch(testparalist[i]->printposition) {
		     case 0: x1=150; y1=180; x2=230; y2=100; x3=250; y3=120; x4=170; y4=200; 
                     n1=130; m1=160; n2=170; m2=200; n3=150; m3=220; n4=110; m4=180; 
					 break;
             case 1: x1=450; y1=180; x2=530; y2=100;  x3=550; y3=120; x4=470; y4=200; 
                     n1=430; m1=160; n2=470; m2=200; n3=450; m3=220; n4=410; m4=180; 
                     break; //draw second
             case 2: x1=150; y1=400; x2=230; y2=320; x3=250; y3=340; x4=170; y4=420; 
                     n1=130; m1=380; n2=170; m2=420;n3=150;m3=440;n4=110;m4=400; 
                     break; //draw third
             case 3:x1=450; y1=400; x2=530; y2=320; x3=550; y3=340; x4=470; y4=420; 
                    n1=430; m1=380; n2=470; m2=420; n3=450; m3=440; n4=410; m4=400;  
                    break; //draw fourth
             default: DebugPrint(" printflag is wrong \n");
			}
			
		   drQuad(x1, y1, 0xff000000, x2,  y2, 0xff000000, x3, y3, 0xff000000, x4, y4, 0xff000000);
           drQuad(n1, m1, 0xff000000, n2,  m2, 0xff000000, n3, m3, 0xff000000, n4, m4, 0xff000000);
		}

		continue;
		} 
		
		// MUs are testing now
		if(gMuStatus[i] == pending) {
		
			//drPrintf(0,0,L"Xbox MU Test Version 2.00 - MU ASIC! Running test...");
		    //drPrintf(0,20,L"Running test...");

			switch (testparalist[i]->printposition) {
			       case 0: x1=40; y1=40; break;  
                   case 1: x1=320; y1=40; break;  
                   case 2: x1=40; y1=240; break;  
                   case 3: x1=320; y1=240; break; 
				   default: DebugPrint(" printflag is wrong \n");
			}
				
			drPrintf(x1,y1,L"Port#=%d, Slot#=%d testing", testparalist[i]->port, testparalist[i]->slot);
		    screenflag=1;
			testparalist[i]->showflag = 0;
		
		}

//keep all old stuff in the screen
      if(((gMuStatus[i]==failed)||(gMuStatus[i] == passed))&&(testparalist[i]->removed!=1)) {         
          printflag++;

          testparalist[i]->showflag = 0;

          switch (testparalist[i]->printposition) {
            case 0: x1=40; y1=40; y2=55; y3=70; y4=85; break;  
            case 1: x1=320; y1=40;y2=55; y3=70; y4=85; break;  
            case 2: x1=40; y1=240; y2=260; y3=280; y4=300; break;  
            case 3: x1=320; y1=240;y2=260; y3=280; y4=300; break;  
            
            default: DebugPrint(" printflag is wrong \n");
          }
          
          drPrintf(x1,y1,L"Port#=%d, Slot#=%d", list[i]->port, list[i]->slot);
          if(list[i]->BB1!=-1) drPrintf(x1,y2,L"First bad block count   %d", list[i]->BB1);
		  if(list[i]->BB2!=-1) drPrintf(x1,y3,L"Second bad block count  %d", list[i]->BB2);
		  if((list[i]->BB3!=-1) && (gMuStatus[i] == passed)) drPrintf(x1,y4,L"Third bad block count   %d", list[i]->BB3);
		  else {
			  
			  switch(testparalist[i]->fail) {
			    case lowleveltest:                drPrintf(x1,y4,L"LowLeveTest Failed"); break;
				case formattest:                  drPrintf(x1,y4,L"FormatTest Failed"); break;
				case verifytest:                  drPrintf(x1,y4,L"VerifyTest Failed"); break;
				case writereadtest:               drPrintf(x1,y4,L"WriteReadTest Failed"); break;
				case countsectorwritetest:        drPrintf(x1,y4,L"CountSectorWriteTest Failed"); break;
				case xmountmu:                    drPrintf(x1,y4,L"XMountMU Failed"); break;
				case unknowreason:                drPrintf(x1,y4,L"Failed Unknow Reason"); break;
				default:                          drPrintf(x1,y4,L"Code is Wrong, Should not Show");
			  }
		  }
      }
      

	  // test is faild or passed
      if((gMuStatus[i] == failed)&&(testparalist[i]->removed!=1)) {
          
          switch(testparalist[i]->printposition) { 
            case 0: x1=130; y1=100; x2=230; y2=200; x3=210; y3=220; x4=110; y4=120;
                    n1=210; m1=100; n2=230; m2=120; n3=130; m3=220; n4=110; m4=200;
                    break;
            case 1: x1=450; y1=100; x2=550; y2=200; x3=530; y3=220; x4=430; y4=120; 
                    n1=530; m1=100; n2=550; m2=120; n3=450; m3=220; n4=430; m4=200;
                    break;     
            case 2: x1=130; y1=320; x2=230; y2=420; x3=210; y3=440; x4=110; y4=340; 
                    n1=210; m1=320; n2=230; m2=340;n3=130;m3=440;n4=110;m4=420;
                    break; 
            case 3: x1=450; y1=320; x2=550; y2=420; x3=530; y3=440; x4=430; y4=340; 
                    n1=530; m1=320; n2=550; m2=340;n3=450;m3=440;n4=430;m4=420;
                    break;                             
            default: DebugPrint(" printflag is wrong \n");
          }
                    
          drQuad(x1, y1, 0xffff0000, x2,  y2, 0xffff0000, x3, y3, 0xffff0000, x4, y4, 0xffff0000);
	      drQuad(n1, m1, 0xffff0000, n2,  m2, 0xffff0000, n3, m3, 0xffff0000, n4, m4, 0xffff0000);

      } else if((gMuStatus[i] == passed)&&(testparalist[i]->removed!=1)) {
          switch(testparalist[i]->printposition) {
             case 0: x1=150; y1=180; x2=230; y2=100; x3=250; y3=120; x4=170; y4=200; 
                     n1=130; m1=160; n2=170; m2=200; n3=150; m3=220; n4=110; m4=180;
					 break;
             case 1: x1=450; y1=180; x2=530; y2=100; x3=550; y3=120; x4=470; y4=200; 
                     n1=430; m1=160; n2=470; m2=200; n3=450; m3=220; n4=410; m4=180; 
                     break; //draw second
             case 2: x1=150; y1=400; x2=230; y2=320; x3=250; y3=340; x4=170; y4=420; 
                     n1=130; m1=380; n2=170; m2=420;n3=150;m3=440;n4=110;m4=400; 
                     break; //draw third
             case 3:x1=450; y1=400; x2=530; y2=320; x3=550; y3=340; x4=470; y4=420; 
                    n1=430; m1=380; n2=470; m2=420; n3=450; m3=440; n4=410; m4=400;  
                   break; //draw fourth
             default: DebugPrint(" printflag is wrong \n");                        
           }
          drQuad(x1, y1, 0xff00ff00, x2,  y2, 0xff00ff00, x3, y3, 0xff00ff00, x4, y4, 0xff00ff00);
          drQuad(n1, m1, 0xff00ff00, n2,  m2, 0xff00ff00, n3, m3, 0xff00ff00, n4, m4, 0xff00ff00);
      }

      if(printflag ==3 ) {
          //drCls();
           
          drShowScreen();
		  screenflag=0;
          
          printflag =-1;
      }
    } //end of for loop

//in case printflag =0,1,2,
     if(printflag!=-1) {
         drShowScreen();
         
         printflag =-1;
     }
	 else if(screenflag==1) drShowScreen();
}



void ReleaseMemory(Paralist list[])
{
    int i=0;
    
    for(i=0;i<MAX_MUS; i++) delete list[i];

}



void OperateMotors(WORD wMotorCode, BOOL fOn)
/*++
  Routine Description:
    Turns on the motors specified in the wMotorCode.
    
    Only the lower byte of wMotorCode is used.
    Here is the interpretation:

        Bit Position:        76543210  
        =============================
        Left(L) or Right(R): LRLRLRLR  
        Controller Number:   33221100

    Any number of bits may be set.

--*/
{
    XINPUT_FEEDBACK feedback;
    DWORD           dwPort;
    DWORD           dwError;
    HANDLE          hGamepad;
    BOOL            fSuccess;
	XINPUT_POLLING_PARAMETERS pp = {FALSE, TRUE, 0, 1, 1, 0};

    //
    //  Fill out the feedback header
    //
    memset(&feedback,0, sizeof(feedback));
    feedback.Header.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(!feedback.Header.hEvent)
    {
        DebugPrint("WARN: Could not create feedback wait event.");
        return;
    }
    
    //
    //  Loop over ports
    //
    for(dwPort = 0; dwPort < 4; dwPort++)
    {
        if(wMotorCode&3)
        {
            DebugPrint(
                "Turning on gamepad %d motors(%s%s%s)\n",
                dwPort,
                (wMotorCode&2) ? "LEFT" : "",
                (3 == (wMotorCode&3)) ? " and " : "",
                (wMotorCode&1) ? "RIGHT" : ""
                );
            fSuccess = FALSE;
			hGamepad = hDuke[dwPort];
            if(hGamepad)
            {
                if(fOn)
                {
                    feedback.Rumble.wLeftMotorSpeed = (wMotorCode&2) ? 0xFFF0 : 0;
                    feedback.Rumble.wRightMotorSpeed = (wMotorCode&1) ? 0xFFF0 : 0;
                } else
                {
                    feedback.Rumble.wLeftMotorSpeed = 0;
                    feedback.Rumble.wRightMotorSpeed = 0;
                }

                ResetEvent(feedback.Header.hEvent);
                dwError = XInputSetState(hGamepad, &feedback);
                if(ERROR_IO_PENDING == dwError)
                {
                    // Wait for 1 second max
                    WaitForSingleObject(feedback.Header.hEvent, 1000);
                }
			}
       }
        //
        //  Get ready for next iteration.
        //
        wMotorCode >>= 2;
    }
}

// thread to maintain motor state for the life of the process

void MaintainMotor()
{
	bool fNoisy = TRUE;
	bool fMotorState[MAX_MUS];
	bool iCurrentStatus[MAX_MUS];
	UINT StateMask[MAX_MUS];
	int port,slot;
	WORD MotorMask;
	DWORD addDevice,removeDevice;

	// init arrays;
	memset(&fMotorState,0,sizeof(bool) * MAX_MUS);
	memset(&iCurrentStatus, 0 ,sizeof(bool)* MAX_MUS);
	memset(&StateMask, 0 ,sizeof(int) * MAX_MUS);

	while(1)
	{
		
	    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
		// allow users to defeat rumble feature
		if(fNoisy == TRUE)
		{
			// check for noises off
			if(InputCheckRumbleOff())
			{
				fNoisy = FALSE;
			}
		} else
		{
			if(InputCheckRumbleOn())
			{
				fNoisy = TRUE;
			}
		}
			
		// maintain motor states
		for(int i = 0;i< MAX_MUS; i++)
		{
			if(!testparalist[i]) continue; // continue for no duke plugged in
			// setup motor state mask
			port = testparalist[i]->port;
			slot = testparalist[i]->slot;
			MotorMask = 1 << ((port * 2) +1);  // setup for left motor

			if(testparalist[i]->removed == 1 || fNoisy == FALSE)
			{
				// turn off motor
				OperateMotors(MotorMask | (MotorMask>>1), FALSE); // turn off both left and right motors
				continue;
			}
			switch (gMuStatus[i])
			{

				case pending:
				{
					
					if(fNoisy)
					{
						// active test, intermittant rumble
						// switch motor state
						StateMask[i] ^= 1;
						OperateMotors(MotorMask,  StateMask[i]);
					
						iCurrentStatus[i] = TRUE;
					}
					break;
				}

				case failed:
				{
					if(fNoisy)
					{
						// Kill Lefthand Motor
						OperateMotors(MotorMask,0); // set left motor off
						// turn on right motor full
						OperateMotors(MotorMask >> 1,1); // turn on right motor
						StateMask[i] = 0;
					}
					break;
				}
				case passed:
				{
					
					if(fNoisy)
					{
						//
						// turn on left motor full
						OperateMotors(MotorMask,1);
						StateMask[i] = 0;
					}
					break;
				}
				 
				case notyet:
				{
					if(iCurrentStatus[i] == TRUE)
					{
						// turn off motor
						OperateMotors(MotorMask | (MotorMask>>1), FALSE); // turn off both left and right motors
						iCurrentStatus[i] = FALSE;
						StateMask[i] = 0;
						break;
					}
				}
				default:
					break;

			} // switch
		} // for
		
		Sleep(500);
	}  // while forever
}

bool InputCheckRumbleOff(void)
{
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
    {
        if(hDuke[port])
        {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 128)
            {
                DebugPrint("\r\n\r\nFound Rumble off Combo on gamepad %u - rebooting\r\n", port+1);
                return true;
            }
		}
    }

    return false;
}

bool InputCheckRumbleOn(void)
{
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
    {
        if(hDuke[port])
        {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 128)
            {
                DebugPrint("\r\n\r\nFound Rumble on gamepad %u - rebooting\r\n", port+1);
                return true;
            }
		}
    }

    return false;
}

void InputDukeInsertions(DWORD add, DWORD remove)
{
    for(unsigned port=0; port<XGetPortCount(); port++)
    {
        if(remove & USBPortMasks[port])
        {
            if(hDuke[port]) XInputClose(hDuke[port]);
            hDuke[port] = NULL;
        }
        if(add & USBPortMasks[port])
        {
            hDuke[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, NULL);
            DebugPrint("Adding Duke in Port %u\n", port+1);
        }
	}
}
/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    muprod23.cpp

Abstract:

     This version was built based on former version muprod.cpp.

     Initializes all global variables, defines user data types, contains main function to test the MUs independently
	 using multithread test. When the first bad block failed, we do not test the others in order to save time.


Author:

    Bing Dong Li(a-bingli) Feb. 2001

--*/

 
 

#include <windows.h>
#include <stdlib.h>
//#include <string.h>
//#include <conio.h>
#include <process.h>

#include "xtl.h"
#include <stdio.h>
#include "muprod.h"
//#include "ccl.h"
#include "draw.h"
#include "Tsc.h"

//using namespace std;

#define MAX_MUS 32

BOXSTATUS gMuStatus[MAX_MUS];

typedef struct parameterList *Paralist;



struct parameterList { 
    int i;
    int BB1;
    int BB2;
    int BB3;
    int port;
    int slot;
	int inserted;
	int removed;
	int showflag;
	int printposition;
};


//Global variables
Paralist testparalist[MAX_MUS];


//Function declaration
void ShowAllInformation(Paralist list[]);
void ReleaseMemory(Paralist list[]);
 
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

void CheckMus()
{
	DWORD insert, remove;
	int i,j=0;
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
   //drCls();
      //drPrintf(0,30,L"remove=%d",(int*)remove);
      //drPrintf(0,50,L"insert=%d",(int*)insert);
      //drPrintf(0,70,L"gMuStatus[i]=%d, i=%d", (int)gMuStatus[i],i);
  // drPrintf(0,90,L"testme#=%d", j);
   //drShowScreen();
   //Sleep(2000);
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
Function:	wWinMain (...)
Purpose:	the entry-point of cpx. Calls init, does the main loop, then de-inits
Params:		irrelevant
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
//int __cdecl main(){return 0;}
//DWORD WINAPI TestMain()


void test( void *testparalist /*int port, int slot, int &BB1, int &BB2, int &BB3*/ )
{
	__int64 tim;
	CHAR drive;
	BOXSTATUS overall = passed;
	int temp;
    int port, slot, BB1, BB2, BB3;
    BB1=((struct parameterList *)testparalist)->BB1;
    BB2=((struct parameterList *)testparalist)->BB2;
    BB3=((struct parameterList *)testparalist)->BB3;
    port=((struct parameterList *)testparalist)->port;
    slot=((struct parameterList *)testparalist)->slot;

/*
    if(failed == LowLevelTest(port, slot, BB1, BB2, BB3))
    {
        overall = failed;
		goto nothing;
    }

	if(failed == FormatTest(port, slot)) {
		overall = failed;
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
		goto nothing;
	}

	temp = XMountMU(port, slot, &drive);
	DebugPrint("XMountMU: %d -- ", temp);
	DebugPrint("Drive: %c\n", drive);
	if(temp != 0) {
		DebugPrint("XMountMU failed: 0x%x\n", GetLastError());
		overall = failed;
		goto nothing;
	}

	Sleep(1000);

	if(failed == CountSectorWriteTest(drive)) {
		DebugPrint("CountSectorWriteTest(%c:) failed.\n", drive);
		overall = failed;
		goto unmount;
	}

	Sleep(1000);

*/
	// create a semphore to pretect the shared FILE * f in WriteReadTest(drive) function

   HANDLE    hIOMutex= CreateMutex (NULL, FALSE, NULL);

   WaitForSingleObject( hIOMutex, INFINITE );
	if(failed == WriteReadTest(drive)) {
		DebugPrint("WriteReadTest(%c:) failed.\n", drive);
		overall = failed;
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
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
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

    //return overall;
}



void __cdecl main()
{
//	_asm int 3;

    int i=0,t=0,p=0,f=0; //BB1[MAX_MUS],BB2[MAX_MUS], BB3[MAX_MUS];
	init();
     
	int port=0, slot=0;
	float x1=0,y1=0;

	char *currtime,*begitime;
	TCHAR *currenttime;
	TCHAR *begintime1;
	TCHAR begintime[256];
	TCHAR testing[256]= L"testing";

	struct tm *ptr;
	time_t lt;
   // static int count=0;
	Sleep(16);
    int screenflag=0,  muflag=0, showflag=0;
    float printflag=0.0;
    

	drCls();
	drPrintf(0,0,L"Xbox MU Test Version 2.00 - MU ASIC!");
	drShowScreen();
    //Sleep(200);
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
	}

	while(1) //loop until we're supposed to quit...
	{
        
        screenflag=0;
        printflag=20.0;
        muflag=0;
        showflag=0;

        CheckMus();
	
        for(i = 0; i < MAX_MUS; i++) {
						
            //if((i==1)||(i==2)||(i==3)||(i==4)) gMuStatus[i]=testme;
            if(gMuStatus[i] == testme) {
              //  DebugPrint("Testing MU at port %d, slot %d\n", port, slot);

				drCls();
				drPrintf(0,0,L"Xbox MU Read_Write_Compare Testing ......");

				lt=time('\0');
				ptr=localtime(&lt);
				//begitime=asctime(ptr);
				begintime1 = _wasctime(ptr);
                wcscpy(begintime,begintime1);

				//wsprintf(begintime, L"%S", begitime);

				drPrintf(0,200,L"Begin Time: %S", begintime);
				drShowScreen();
                       
                port = i &  15;
                slot = i < 16 ? 0 : 1;

				if((port==0)&&(slot==0)) testparalist[i]->printposition=0;
				else if((port==0)&&(slot==1)) testparalist[i]->printposition=1;
				else if((port==1)&&(slot==0)) testparalist[i]->printposition=2;
				else if((port==1)&&(slot==1)) testparalist[i]->printposition=3;

                testparalist[i]->port = port;
                testparalist[i]->slot = slot;
                testparalist[i]->BB1=-1;
                testparalist[i]->BB2=-1;
                testparalist[i]->BB3=-1;
                testparalist[i]->i=i;
				 
                gMuStatus[i]= pending;
                

                _beginthread((void (__cdecl*)(void *))test, NULL, testparalist[i]);
                 

 
            }
			
			
			if(testparalist[i]->showflag==1) showflag=1;
        }
        
        
        if(showflag==1) ShowAllInformation(testparalist);
        
        //Init_Int_Array(oldthread, MAX_MUS);
		//Init_Int_Array((int*)gMuStatus, MAX_MUS);
        
	
} //end of while loop

	ReleaseMemory(testparalist);

	//we should theoretically never get here on xbox......
	deinit();
}



void ShowAllInformation(Paralist list[])
{
  
    int i=0,oldport=-1,screenflag=0,printflag=-1; // it should not be

    float x1=0,x2=0,x3=0,x4=0,y1=0,y2=0,y3=0,y4=0;
    float m1=0,m2=0,m3=0,m4=0,n1=0,n2=0,n3=0,n4=0;
    drCls();

	drPrintf(130,460,L"MU Test Version 2.00");
	drPrintf(325,460,L"Running test ......  ");
	drLine(0,235,640,235,0x000000ff);
	drLine(310,0,310,480,0x000000ff); 

	 
	  
       
   
	//this MU has been removed, so remove result from the screen
    for(i=0; i< MAX_MUS; i++) {

		if(testparalist[i]->removed==1) {

          testparalist[i]->showflag = 0;
		  screenflag=1;

          switch (testparalist[i]->printposition) {
            case 0: x1=0; y1=20; y2=40; y3=60; y4=80; break;  
            case 1: x1=320; y1=20;y2=40; y3=60; y4=80; break;  
            case 2: x1=0; y1=240; y2=260; y3=280; y4=300; break;  
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
                     n1=130; m1=160; n2=170; m2=200; n3=150; m3=220; n4=110; m4=180; break;
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
		
		
		if(gMuStatus[i] == pending) {
		
			//drPrintf(0,0,L"Xbox MU Test Version 2.00 - MU ASIC! Running test...");
		    //drPrintf(0,20,L"Running test...");

			switch (testparalist[i]->printposition) {
			       case 0: x1=0; y1=20; break;  
                   case 1: x1=320; y1=20; break;  
                   case 2: x1=0; y1=240; break;  
                   case 3: x1=320; y1=240; break; 
				   default: DebugPrint(" printflag is wrong \n");
			}
				
			drPrintf(x1,y1,L"Port#=%d, Slot#=%d testing", testparalist[i]->port, testparalist[i]->slot);
		    screenflag=1;
			testparalist[i]->showflag = 0;
		
		}

//keep in the screen
      if(((gMuStatus[i]==failed)||(gMuStatus[i] == passed))&&(testparalist[i]->removed!=1)) {         
          printflag++;

          testparalist[i]->showflag = 0;

          switch (testparalist[i]->printposition) {
            case 0: x1=0; y1=20; y2=40; y3=60; y4=80; break;  
            case 1: x1=320; y1=20;y2=40; y3=60; y4=80; break;  
            case 2: x1=0; y1=240; y2=260; y3=280; y4=300; break;  
            case 3: x1=320; y1=240;y2=260; y3=280; y4=300; break;  
            
            default: DebugPrint(" printflag is wrong \n");
          }
          
          drPrintf(x1,y1,L"Port#=%d, Slot#=%d", list[i]->port, list[i]->slot);
          if(list[i]->BB1!=-1) drPrintf(x1,y2,L"First bad block count   %d", list[i]->BB1);
		  if(list[i]->BB2!=-1) drPrintf(x1,y3,L"Second bad block count  %d", list[i]->BB2);
		  if(list[i]->BB3!=-1) drPrintf(x1,y4,L"Third bad block count   %d", list[i]->BB3);

      }
      
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
                     n1=130; m1=160; n2=170; m2=200; n3=150; m3=220; n4=110; m4=180; break;
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
          //Sleep(1000);
          printflag =-1;
      }
    } //end of for loop
//in case printflag =0,1,2,3

       
     

     if(printflag!=-1) {
         drShowScreen();
         //Sleep(1000);
         printflag =-1;
     }
	 else if(screenflag==1) drShowScreen();
}



void ReleaseMemory(Paralist list[])
{
    int i=0;
    
    for(i=0;i<MAX_MUS; i++) delete list[i];

}
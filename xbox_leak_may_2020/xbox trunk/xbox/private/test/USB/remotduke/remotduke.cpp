/* ++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    remotduke.cpp

Abstract:

    initializes all global variables, contains main loop, init. The module is using remote controller to control duke's motor.
	there is a file called buttoncontrol.ini to specify all codes. it can be changed to satisfy different requirement. This 
	application supports two dukes right now, it can be modified to support more than two dukes easily.

Author:

     Bing Li (a-bingli) 2001 May

-- */



#include "irrem.h"    //all stuff for IR controller
 
#include <xboxp.h>
#include <time.h>
 
#include "dukeprod.h"  // all stuff for dukeprod
#include "ccl.h"
#include "device.h"     // library for device operation
 
#include "mmu.h"
#include "Tsc.h"




#define NO_XDCS_INSTANCE    ((DWORD)(-1))
//size can be changed whenever need to remember or to output screen more than one ir code
#define IR_CODE_BUFFER_SIZE 2  // can be enlarged to store more IR_CODE
#define MAXCOUNT 5              //useless now

#define LIFT 0
#define RIGHT 1

#define XDCS_STATUS_NO_DONGLE           0
#define XDCS_STATUS_DOWNLOADING         1
#define XDCS_STATUS_DOWNLOAD_SUCCEEDED  2
#define XDCS_STATUS_DOWNLOAD_FAILED     3
#define XDCS_STATUS_FILE_WRITE_FAILED   4


struct REMOTE_CODE_TABLE
{
    LPWSTR Name;                   //the name of the button on the remote controller
    WORD Code;                     //the code of the button
	WORD Controler;                 //the control code of the button to specify which motor will be controlled
};

//table size 28, 27 buttons in total
#define RemoteCodeSize 28          //there are 27 buttons for the current remote controller, it may be changed later on
DWORD  MAXTIME=120;                //the default time to finish the test

//Global Variable
DWORD TestStatus;

HANDLE target;                      //file handle to handle the download file
HANDLE source;                      //file handle to handle the source file which is used to compare with the download file

int Dongle=0, Rom=0, File=1, screen=0, timer=0;  // all these variables are used to control flow

//CDraw draw;
WCHAR FormatBuffer[256];
DWORD dwDownloadStatus = XDCS_STATUS_NO_DONGLE;   //default as NO DONGLE

//These are all status in this application now.
LPWSTR StatusText[] = 
    {
        L"No Dongle",
        L"Downloading",
        L"Success",
        L"Failure",
        L"Writing to File Failed",

    };


REMOTE_CODE_TABLE RemoteCodeTable[RemoteCodeSize];
/* = {                       
      Button Name  Code    control( supports 2*2*2*2 = 16 motor)

    {L"POWER",    0xAD5   0x0000},
    {L"REVERSE",  0xAE2   0x0000},
    {L"PLAY",     0xAEA   0x0000},
    {L"FORWARD",  0xAE3   0x0000},
    {L"SKIP -",   0xADD   0x0000},
    {L"STOP",     0xAE0   0x0000},
    {L"PAUSE",    0xAE6   0x0000},
    {L"SKIP +",   0xADF   0x0000},
    {L"GUIDE",    0xAE5   0x0000},
    {L"INFO",     0xAC3   0x0000},
    {L"MENU",     0xAF7   0x0000},
    {L"REPLAY",   0xAD8   0x0000},
    {L"UP",       0xAA6   0x0000},
    {L"LEFT",     0xAA9   0x0000},
    {L"OK",       0xA0B   0x0000},
    {L"RIGHT",    0xAA8   0x0000},
    {L"DOWN",     0xAA7   0x0000},
    {L"DIGIT 0",  0xACF   0x0000},
    {L"DIGIT 1",  0xACE   0x0000},
    {L"DIGIT 2",  0xACD   0x0000},
    {L"DIGIT 3",  0xACC   0x0000},
    {L"DIGIT 4",  0xACB   0x0000},
    {L"DIGIT 5",  0xACA   0x0000},
    {L"DIGIT 6",  0xAC9   0x0000},
    {L"DIGIT 7",  0xAC8   0x0000},
    {L"DIGIT 8",  0xAC7   0x0000},
    {L"DIGIT 9",  0xAC6   0x0000},
    {NULL, 0}
};

 this is the basic code table, now it is read from the file
*/

BOXSTATUS gPadStatus[MAX_SLOTS];

gpGAMEPAD * gData;  // defined in dukeprod.h

/*********************************************************************
*Function: TurnMotor() is used to turn on/off the motor              *
*Parameter:button code index, on==1 is on, otherwise turn off        *
*Return:   void                                                      *
*Notes:    Now it support two dukes at the same time, you can easily *
*          modify to support more than two dukes                     * 
**********************************************************************/

void TurnMotor(int button, /*int slot,*/ int on)
{
	gpPoll();
	Sleep(16);

	BYTE slot=0;
	DWORD r=0;

	WORD flag=0;

 //controler ==0 means the controler code is empty
	if(RemoteCodeTable[button].Controler==0) return;

 
  while(1) //loop until we're supposed to quit...
	{
	  
	   

	   if(slot == MAX_SLOTS ) return;
	    flag=0;
	   
    gpUpdateDeviceStateInfo();				//update the button state info...
		//muGetDeviceMessages();


        gpPoll();
		gData = ::gpGetState(slot);


		for(slot = 0; slot < MAX_SLOTS; slot++)	//go through all slots...
		{
		   
          //DebugPrint("slot=%d\n", slot);

		  if(gpDeviceExists(slot)) {
			  
			  //DebugPrint("slot=%d\n", slot);
			  //DebugPrint("controler=%d\n",RemoteCodeTable[button].Controler); 

			if( ( (!(slot&1)) && ( 4352 & RemoteCodeTable[button].Controler) ) ||
				( (slot&1) && ( 17 & RemoteCodeTable[button].Controler) )  )   {

			BYTE data;
			XINPUT_FEEDBACK feedback;
			memset(&feedback, 0, sizeof(XINPUT_FEEDBACK));
		 	 
			data=(on? 252:0);

			switch(RemoteCodeTable[button].Controler) {
			     case 4096: feedback.Rumble.wLeftMotorSpeed= (data<<8) +data; break;     //controler == 0x1000
				 case 16:   feedback.Rumble.wLeftMotorSpeed= (data<<8) +data; break;     //controler == 0x0010
			     case 256:  feedback.Rumble.wRightMotorSpeed= (data<<8) +data; break;    //controler == 0x0100
				 case 1:    feedback.Rumble.wRightMotorSpeed= (data<<8) +data; break;    //controler == 0x0001
				 case 4352: feedback.Rumble.wRightMotorSpeed= (data<<8) +data;           //controler == 0x1100
					        feedback.Rumble.wLeftMotorSpeed= (data<<8) +data;
							DebugPrint("both speed = %d \n", feedback.Rumble.wLeftMotorSpeed);
					        break;
				 case 17:   feedback.Rumble.wRightMotorSpeed= (data<<8) +data;            //controler == 0x0011
					        feedback.Rumble.wLeftMotorSpeed= (data<<8) +data;
                            DebugPrint("both speed = %d \n", feedback.Rumble.wLeftMotorSpeed);
					        break;
				 case 0: return;
				 default: DebugPrint("Unknow RemoteCodeTable[].Controler code \n");

			}

            r = gpInputSetState(slot, &feedback);

			while(feedback.Header.dwStatus == ERROR_IO_PENDING) 	; //asynchronous
			
			if(feedback.Header.dwStatus != ERROR_SUCCESS)
					DebugPrint("Error: %d\n", feedback.Header.dwStatus);
		 

			 if(ERROR_IO_PENDING!=r)
			        DebugPrint("gpInputSetState returned error %d\n", r);
			 else flag++;

			  //DebugPrint("flag=%d slot=%d\n", flag, slot);

			}

		  }
		  else ::gPadStatus[slot]=notyet;


		  if(flag) return;  //if the motor has been turned on, return

		}

		if(slot == MAX_SLOTS) return; //if there is no device at all,  return

	}


}




void InitRemotCodeTable()
{
   int i=0;

   for(i=0; i<RemoteCodeSize;i++) {

       //RemoteCodeTable[i]= new REMOTE_CODE_TABLE;
	   RemoteCodeTable[i].Name = new WCHAR;
	   RemoteCodeTable[i].Code = 0;
	   RemoteCodeTable[i].Controler=0;
   }

}

int hextode(CHAR hex[])
{

  int retn=0,i=0, k=0;
  int len=strlen(hex), pw=(int)pow(16,len-3);

  //hex begin 0x, so we count from the (0,1,)2, from the third one
  for(i=2;i<len; ++i) {
	  if((hex[i]>= '0') && (hex[i] <= '9'))   k=  hex[i] - '0'; //numbers from 0 - 9
      else {
		  
		  switch(hex[i]) {
		  case 'A' : k=10; break;
          case 'a' : k=10; break;
		  case 'B' : k=11; break;
		  case 'b' : k=11; break;
		  case 'C' : k=12; break;
          case 'c' : k=12; break;
		  case 'D' : k=13; break;
          case 'd' : k=13; break;
		  case 'E' : k=14; break;
		  case 'e' : k=14; break;
		  case 'F' : k=15; break;
          case 'f' : k=15; break;
		  default: DebugPrint("Button.ini has invalid hex charactor %c \n", hex[i]); return -1;
		  }

	  }

	  retn += k*pw;
	  pw /=16;
  }
  
  return retn;

}


  
// read remote_code_table from the file
void Readrct(char file[], int fsize)
{

  int size=RemoteCodeSize,i=0;
  CHAR *tempf = new CHAR [fsize];
  
  if(tempf==0) {
	  DebugPrint("Out of memory space \n");
	  return;
  }

  CHAR  *temp;
  
  strcpy(tempf,file);

  //ignore the first line of the comments
  if((temp=strtok(tempf, "\r\n")) == NULL) {
		  
		  DebugPrint("Empty file \n");
		  return;
	  }
   

  //this is the second line to read time limit as decimal in seconds unit

  temp=strtok(NULL, " \n");

  int t=atoi(temp);
  if(t!=0)  MAXTIME=t;  

  //parse the file to assign to the RemoteCodeTable[]
  for(i=0; i<size;i++) {

	  if((temp=strtok(NULL, " \r\n")) == NULL) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }  

	  swprintf(RemoteCodeTable[i].Name,L"%S", temp);

	  //DebugPrint("RemoteCodeTable[%d].Name=%s \n",i, temp);

	  if( (temp=strtok(NULL, " \r\n")) == NULL ) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }  

	  RemoteCodeTable[i].Code = (WORD)hextode(temp);
	  //DebugPrint("RemoteCodeTable[%d].Code=%s\n",i, temp);

      if( (temp=strtok(NULL, " \r\n")) == NULL ) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }  

	  RemoteCodeTable[i].Controler = (WORD)hextode(temp);
	  //DebugPrint("RemoteCodeTable[%d].Controler=%s\n",i, temp);

  }
}


    
/**************************************************************************************
*Function: ReadButton() is used to read RemoteCodeTable[] from the file, which include*
*          the name of the button, the IR code of the button, the controller which is *
*          used to control the duke motor.                                            *
*Parameters: the array of string to store the file path and name, actually it defined *
*Notes:    This function copied the whole file to array then deal with the array      *
***************************************************************************************/

void ReadButton(char buttonfilename[])
{
  //char buttonfilename[]="C:\\public\button.ini";

  HANDLE buttonfile;

  buttonfile = CreateFile(buttonfilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 

  if(buttonfile==INVALID_HANDLE_VALUE) {
	  DebugPrint("CreateFile failed, Error %d \n", GetLastError());
	  return;
  }

  DWORD fsize=0;
	
  CHAR *sm;
   
  DWORD temp;

	
	if(-1==(fsize=GetFileSize(buttonfile,NULL))) {
		 DebugPrint("GetFileSize(buttonfile,NULL) failed, Error: %d\n", GetLastError());
		return;
	}

	sm = new char[fsize];
	if(sm == NULL) {
		DebugPrint("new failed line %d FILE :%s!\n", __LINE__, __FILE__);
		delete[] sm;
		return;
	}
	
	if(FALSE == ReadFile(buttonfile, sm, fsize, &temp, 0)) {
		DebugPrint("ReadFile(buttonfile, .., %d, .., 0) failed! Error %d \n", fsize, GetLastError());
		delete[] sm;
		return;
	}

   Readrct(sm, fsize);


}



/*************************************************************************************************
*Function: WorkScreen() is used to show the user which button need to be pressed when IR receiver*
*           is pluged in, and every thing works well.                                            *
*Parameters: the button index of RemoteCodeTable[]                                               *
**************************************************************************************************/

void WorkScreen(int button)
{
	float wX=40, wY=50;
	screen=1;
	drCls();
	
	drPrintf(wX,wY,L"Xbox Tongle Test Version 1.00        Runing ...");


	drPrintf(wX,70.0,L"Time Limit Is %d Seconds",MAXTIME);

    wY+=100; 

    // set the  color for the screen, and then reset back again, set the size does not work for the original application
	drSetSize(40.0, 42.0, 0xff00ffff,0xff000000);

	if((button>=0)&&(button<28)) drPrintf(wX,wY,L"Press %s",RemoteCodeTable[button].Name);
	else drPrintf(wX, wY, L"Unknow Button");

	drSetSize(8.0, 15.0, 0xff00ff00, 0xff000000); //default color

    drShowScreen();
}




void ShowFinalResult()
{
    

    float x1=0,x2=0,x3=0,x4=0,y1=0,y2=0,y3=0,y4=0;
    float m1=0,m2=0,m3=0,m4=0,n1=0,n2=0,n3=0,n4=0;

    drCls();
 
    if((TestStatus==1)&&(File == 1)) {
		DebugPrint("Test passed.\n");

        drPrintf(20, 40,L"Test Passed");

		drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
		drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
	 

	} else {

		DebugPrint("Test failed.\n");

		if(File!=1) {
			drPrintf(20.0, 40.0, L"File Compare Failed");
			DebugPrint("File Compare Failed\n");
		}
		else if(timer) { 
			drPrintf(20.0, 40.0, L"Time Off");
			DebugPrint("Time Off\n");
		}
		else if(TestStatus == -1) {
			drPrintf(20, 40, L"Download Failed");
			DebugPrint("Download Failed \n");
		}
		else {
			drPrintf(20, 40, L"Button Test Failed");
			DebugPrint("Button test failed \n");
		}


		drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
		drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);

	}
     
	drShowScreen();

}


int DiffFile(/*HANDLE &target, HANDLE &source*/)
{
	  
	
	int diff=0; //initiliaze as false, it will changed as true whenever bad thing happened
	
	DWORD tfsize=0, sfsize=0;
	
	char *tm, *sm;
	DWORD temp;


	
	if(-1==(sfsize=GetFileSize(source,NULL))) {
		 DebugPrint("GetFileSize(source,NULL) failed, Error: %d\n", GetLastError());
		return 1;
	}
	
	if(-1==(tfsize=GetFileSize(target,NULL))) {
		DebugPrint("GetFileSize(target,NULL) failed, Error: %d\n", GetLastError());
		return 1;
	}

	if(tfsize!=sfsize) {
		DebugPrint("Size are different\nSource file size is %d but ", sfsize);
        DebugPrint("Target file size is %d\n", tfsize);
		return 1;
	} else DebugPrint("Equal file size, target is %d, source is %d \n", tfsize, sfsize);
	
	tm = new char[tfsize];
	if(tm == NULL) {
		DebugPrint("new failed line %d FILE :%s!\n", __LINE__, __FILE__);
		delete [] tm;
		return 1;
	}
	
	sm = new char[sfsize];
	if(sm == NULL) {
		DebugPrint("new failed line %d FILE :%s!\n", __LINE__, __FILE__);
		delete[] sm;
		return 1;
	}
	
	if(FALSE == ReadFile(target, tm, tfsize, &temp, 0)) {
		DebugPrint("ReadFile(h1, .., %d, .., 0) failed! Error %d \n", tfsize, GetLastError());
		 
		delete[] sm;
		delete[] tm;
		return 1;
	}
	
	if(FALSE == ReadFile(source, sm, sfsize, &temp, 0)) {
		DebugPrint("ReadFile(h2, .., %d, .., 0) failed! Error %d \n", sfsize, GetLastError());
		delete[] sm;
		delete[] tm;
		return 1;
	}
	
	if(memcmp(tm, sm, sfsize)) {
		DebugPrint("NOT the same file\n");
		return 1;
	}
	
    DebugPrint("The same file, Success File Compare\n");
	return diff;

}

void ShowScreen()
{

    drCls();
	 
	drPrintf(10,20,L"Xbox Tongle Test Version 1.00 ");

	drPrintf(10,50,L"No IR Remote Receiver");

//default as failed 
	drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
	drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);


	drShowScreen();


}



LPCWSTR GetCodeName(WORD wCode)
{
    REMOTE_CODE_TABLE *pRemoteEntry = RemoteCodeTable;
    while(pRemoteEntry->Code)
    {
        if(pRemoteEntry->Code == wCode)
        {
            return pRemoteEntry->Name;
        }
        pRemoteEntry++;
    }
    if(wCode)
    {
        return L"UNKNOWN";
    } else
    {
        return NULL;
    }
}


//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------
void __cdecl main()
{
    XDCS_DVD_CODE_INFORMATION XdcsInfo;
    DWORD dwXdcsInstance=NO_XDCS_INSTANCE;
//    DWORD dwDownloadStatus = XDCS_STATUS_NO_DONGLE;
    DWORD dwConnectedDevices = 0;
    DWORD dwPortInUse = 0;
    XDCS_ASYNC_DOWNLOAD_REQUEST downloadRequest;
    DWORD dwPercentComplete = 0;
    DWORD dwDownloadStartTick, dwCurrentTick;
	DWORD mystarttime=0,mycurrenttime=0;

    double fDownloadTime = 0;
    PVOID pvBuffer;
    DWORD dwInserstions, dwRemovals;
    CHAR  szFileName[]="t:\\dvdcode.lib";
	CHAR sourceFileName[]="t:\\dvdsource.lib";
	CHAR button[]="t:\\buttoncontrol.ini";

	

	if((source = CreateFile(sourceFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
	{ DebugPrint("Can not Create Source File \n");
	  File =0;
	  goto Final;
	}
    
    //HANDLE hFile;
	LPCWSTR pwszCodeName=0, oldCodeName=0;
 

    HANDLE hRemote = NULL;
    DWORD  dwRemotePort=0;
    XINPUT_STATE_INTERNAL IrCodeBuffer[IR_CODE_BUFFER_SIZE] = {0};

    DWORD dwIrCodeBufferPos=0;
    DWORD dwPacketNumber=0;

	 
	DWORD dwDisplayYPos;

	float myX=10, myY=20,y=0;
	int remotecode=0, press =0, count=0,pressOk=0, dc=0;
    int xinput=0;
 
	TestStatus =1; //initiliaze as success, any bad thing happend, it will fail

     
    //Initialize core peripheral port support
    XInitDevices(0,NULL);
	
	drInit();

	InitRemotCodeTable();

	ReadButton(button);
	

 
        
    //Loop Forever
	Rom=0;
	Dongle=1;
	Rom=1; File=-1;

	struct tm *ptr;
	time_t lt;
 
	ShowScreen();


	WORD right=0, motor=0;

    while(1) {

           if( (mycurrenttime - mystarttime) > MAXTIME) { 
			   DebugPrint("Time off \n");
			   timer=1;
			   TestStatus = 0;
			   TurnMotor(remotecode,0);
			   motor=0;
			   goto Final;
		   }


		   if(remotecode==0) {
			   lt=time('\0');
			   ptr=localtime(&lt);
                
			   mystarttime = (ptr->tm_hour) * 360 + (ptr->tm_min)*60 + ptr->tm_sec;
			   mycurrenttime=mystarttime;
		 
		   }


		   if((RemoteCodeTable[remotecode].Code!=0)&&(RemoteCodeTable[remotecode].Controler==0)) {

			   TurnMotor(remotecode-1,0);
			   goto Final;
		   }



		   oldCodeName = RemoteCodeTable[remotecode].Name;

		   

        if(XDCS_STATUS_DOWNLOADING == dwDownloadStatus)
        {
            //Update download time
            dwCurrentTick = GetTickCount();
            fDownloadTime = ((LONG)dwCurrentTick - (LONG)dwDownloadStartTick)/1000.0;
            //Update percent complete
            dwPercentComplete = (downloadRequest.ulBytesRead * 100) / downloadRequest.ulLength; 
            // Check status
            if(ERROR_SUCCESS == downloadRequest.ulStatus)
            {
                target = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
                if(INVALID_HANDLE_VALUE != target)
                {
                    DWORD dwBytesWritten;
                    if(WriteFile(target, downloadRequest.pvBuffer, downloadRequest.ulBytesRead, &dwBytesWritten, NULL))
                    {
                        dwDownloadStatus = XDCS_STATUS_DOWNLOAD_SUCCEEDED;
				
                    } else
                    {
                        dwDownloadStatus = XDCS_STATUS_FILE_WRITE_FAILED;
						DebugPrint("WriteFile() failed %d", GetLastError());
                    }
 //                   CloseHandle(target);
                } else
                {
                    dwDownloadStatus = XDCS_STATUS_FILE_WRITE_FAILED;
					DebugPrint("CreateFile() failed %d\n", GetLastError());
                }
                free(downloadRequest.pvBuffer);
            } else if(ERROR_IO_PENDING != downloadRequest.ulStatus)
            {
                dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                free(downloadRequest.pvBuffer);
            }
        }

        //
        //  Look for the insertion or removal of devices
        //
        if(XGetDeviceChanges(XDEVICE_TYPE_DVD_CODE_SERVER, &dwInserstions, &dwRemovals))
        {

			if (((remotecode > 0)&& (remotecode < (RemoteCodeSize -1))) || (dwDownloadStatus == XDCS_STATUS_DOWNLOADING)
				||(dwDownloadStatus == XDCS_STATUS_DOWNLOAD_SUCCEEDED))
			{
			
				TestStatus =0;
				TurnMotor(remotecode, 0);
				motor=0;
				goto Final;
			}

			Rom = 0;
            if((1<<dwPortInUse)&dwRemovals)
            {   
                dwDownloadStatus = XDCS_STATUS_NO_DONGLE;
            }
            dwConnectedDevices &= dwRemovals;       //Handle removals
            dwConnectedDevices |= dwInserstions;    //Then additions

            //If there currently is no dongle, start using the first device.
            if(XDCS_STATUS_NO_DONGLE == dwDownloadStatus)
            {
                if(dwInserstions)
                {
                    DWORD dwError;
                    dwPortInUse = 0;
                    while(!(dwConnectedDevices&(1<<dwPortInUse))) dwPortInUse++;
                    dwError = XDCSGetInformation(dwPortInUse, &dwXdcsInstance, &XdcsInfo);
                    if(dwError)
                    {
                        dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                    } else
                    {
                        dwDownloadStatus = XDCS_STATUS_DOWNLOADING;
                        downloadRequest.dwDeviceInstance = dwXdcsInstance;
                        downloadRequest.pvBuffer = malloc(XdcsInfo.dwCodeLength);
                        downloadRequest.ulOffset = 0;
                        downloadRequest.ulLength = XdcsInfo.dwCodeLength;
                        downloadRequest.ulBytesRead = 0;
                        downloadRequest.hCompleteEvent = NULL;
                        dwDownloadStartTick = GetTickCount();
                        if(!downloadRequest.pvBuffer)
                        {
                            dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
							DebugPrint("Memory Allocation Failed \n");

                        } else if(ERROR_IO_PENDING!=XDCSDownloadCodeAsync(&downloadRequest))
                        {
                            dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                        }
                    }
                }
            }
        }    
		
	
        if(Rom==1) continue;
         
        //DebugPrint("DownloadStatus %S \n",StatusText[dwDownloadStatus]); 

// Compare the downloaded file in t:\dvdcode.lib with t:\dvdsource.lib byte by byte using DiffFile(), 
// DiffFile return 1 if two files are different, 0 otherwise.

		if((dwDownloadStatus == XDCS_STATUS_DOWNLOAD_SUCCEEDED)&&(File==-1)) {
			
    		SetFilePointer(target,NULL,NULL,FILE_BEGIN);
            SetFilePointer(source,NULL,NULL,FILE_BEGIN);

	    	if(!DiffFile()) {
			//			drPrintf(10,myY,L"File Compare is SUCCESSFUL");
		    	DebugPrint("File Compare is Successful\n");
				File=1;
				TestStatus=1;
				if(!CloseHandle(target)) DebugPrint("CloseHandle(target) faile: %d\n", GetLastError());
 
			} else {
			    TestStatus =0; 
			    File=0;
			    DebugPrint("Compare File Failed\n");
                if(!CloseHandle(target)) DebugPrint("CloseHandle(target) faile: %d\n", GetLastError());
				
				TurnMotor(remotecode, 0);
				motor=0;
			    goto Final;

			}   

		} else if((dwDownloadStatus==XDCS_STATUS_DOWNLOAD_FAILED) ||(dwDownloadStatus==XDCS_STATUS_FILE_WRITE_FAILED)) {
			TestStatus=-1;
			DebugPrint("DownLoad Filed \n");
            if(!CloseHandle(target)) DebugPrint("CloseHandle(target) faile: %d\n", GetLastError());
			TurnMotor(remotecode, 0);
			goto Final;
		}


        /**
        *** Poll the XID Remote part.
        *** Just because we don't have lot's of creative ways to
        *** abuse the XInput API, we will be clever here, and use the API in a valid,
        *** but less than optimal ways.  It had better work too!
        **/
		static int bing=0;
        if(!hRemote) {
            DWORD dwDevices= (XGetDevices(XDEVICE_TYPE_IR_REMOTE)&0x0F); 
            if(dwDevices)
            {
                //Find the first bit set and open it.
                dwRemotePort = 0;
                while(!(dwDevices&(1<<dwRemotePort))) dwRemotePort++;
                hRemote = XInputOpen(XDEVICE_TYPE_IR_REMOTE, dwRemotePort, XDEVICE_NO_SLOT, NULL);
                dwPacketNumber = 0;
            }
        }

		//compare file first, otherwise do not need to test others
		if(File == -1) {
			
			drCls();
			drPrintf(10, 40, L" %s", StatusText[dwDownloadStatus]);
			drShowScreen();			
			continue;

		}

        if(hRemote)
        {
            
			WorkScreen(remotecode);

			if(motor==0) { 
				TurnMotor(remotecode,1);
				motor++;
			}


            if(ERROR_SUCCESS == XInputGetState(hRemote, (PXINPUT_STATE)(IrCodeBuffer+dwIrCodeBufferPos)))
            {
                //Don't move for duplicates
                if(dwPacketNumber != IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber)
                {
                    dwPacketNumber = IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber;
                    dwIrCodeBufferPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                    
					if(dwPacketNumber) {
						DWORD dwCodeBufferDisplayPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                        DWORD dwDisplayYPos = 265 + (IR_CODE_BUFFER_SIZE*15);
                        LPCWSTR pwszCodeName;
                   
						for(int i=1; i < IR_CODE_BUFFER_SIZE; i++) { 
							pwszCodeName = GetCodeName(IrCodeBuffer[dwCodeBufferDisplayPos].IrRemote.wKeyCode);
                           if(pwszCodeName)
						   {
                            
 
							if(oldCodeName==pwszCodeName) {
								TurnMotor(remotecode, 0);
								DebugPrint("remotecode = %d turn off\n", remotecode);
								remotecode++;
								motor=0;
								break;
							}
							//else goto Code;
						   }
                           dwDisplayYPos -= 15;
                           dwCodeBufferDisplayPos = (dwCodeBufferDisplayPos+1)%IR_CODE_BUFFER_SIZE;
						}
					}
				}
			
			} else {   
                XInputClose(hRemote);
                hRemote = NULL;
                memset(IrCodeBuffer, 0, sizeof(IrCodeBuffer));
				//screen=0;
                 
            }
        } else
        {
             
			screen=0;
			DebugPrint("hhandle = NULL \n");
        }

        //Preset
        if(screen==0) {
			ShowScreen();
            TurnMotor(remotecode,0);
			motor=0;
		}

		lt=time('\0');
		ptr=localtime(&lt);

		mycurrenttime = (ptr->tm_hour) * 360 + (ptr->tm_min)*60 + ptr->tm_sec;


//		currenttick=GetTickCount();
         
	 

		if((mycurrenttime - mystarttime) > MAXTIME) {
			TestStatus = 0;
			timer=1;
		}

//		DebugPrint("currenttime=%d \n starttime=%d\n", mycurrenttime,mystarttime);
		
		if( (remotecode == RemoteCodeSize-1) || ( (mycurrenttime-mystarttime) > MAXTIME ) ) {
Final:	ShowFinalResult();
		//initiliaze all variable
		    dwDownloadStatus = XDCS_STATUS_NO_DONGLE;
			Rom =1;
			timer=0;
			remotecode =0;
			motor=0;
			File=-1;
			mycurrenttime =0;
			mystarttime=0;
			screen=1;
		}
    }
	

}

 

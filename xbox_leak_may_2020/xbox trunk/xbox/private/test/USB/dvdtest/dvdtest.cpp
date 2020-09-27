
/* ++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    dvdtest.cpp

Abstract:

    initializes all global variables, contains main loop, init. The module is to test DVD IR Receiver and Remote Contorler
	there is a file called button.ini to specify all codes. it can be changed to satisfy different requirement. This 
	application supports one Receiver and One controler right now, it can be modified to support more than two dukes easily.

Author:

     Bing Li (a-bingli) 2001 May

-- */


#include "irrem.h"
//#include <stdio.h>
#include <xboxp.h>
#include <time.h>
//#include <math.h>
//#include <string>



#define NO_XDCS_INSTANCE    ((DWORD)(-1))
#define IR_CODE_BUFFER_SIZE 2                  //used to store IR_CODE, it can be enlarged to store more 
#define MAXCOUNT 5                              //useless now


#define XDCS_STATUS_NO_DONGLE           0
#define XDCS_STATUS_DOWNLOADING         1
#define XDCS_STATUS_DOWNLOAD_SUCCEEDED  2
#define XDCS_STATUS_DOWNLOAD_FAILED     3
#define XDCS_STATUS_FILE_WRITE_FAILED   4


struct REMOTE_CODE_TABLE
{
    LPWSTR Name;   //name of the remote controler button
    WORD Code;     //IR code of the remote controler
};

//table size 28, 27 buttons in total, different remote controler may have different number. 
//This works for all less than 27 butttons, otherwise, this need to be enlarged.
#define RemoteCodeSize 28
DWORD  MAXTIME=120;
//2 minutes

//Global Variable
DWORD TestStatus;

HANDLE target;  //file handle to handle the target file which is downloaded from the ROM
HANDLE source;  //file handle to handle the source file which had been stored at "t:\\dvdsource.lib" already

int Dongle=0, Rom=0, File=1, screen=0, timer=0; // all these variable are used to control, these may not need to be bad gloable

//CDraw draw;
WCHAR FormatBuffer[256];
DWORD dwDownloadStatus = XDCS_STATUS_NO_DONGLE;

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
    {L"POWER",    0xAD5},
    {L"REVERSE",  0xAE2},
    {L"PLAY",     0xAEA},
    {L"FORWARD",  0xAE3},
    {L"SKIP -",   0xADD},
    {L"STOP",     0xAE0},
    {L"PAUSE",    0xAE6},
    {L"SKIP +",   0xADF},
    {L"GUIDE",    0xAE5},
    {L"INFO",     0xAC3},
    {L"MENU",     0xAF7},
    {L"REPLAY",   0xAD8},
    {L"UP",       0xAA6},
    {L"LEFT",     0xAA9},
    {L"OK",       0xA0B},
    {L"RIGHT",    0xAA8},
    {L"DOWN",     0xAA7},
    {L"DIGIT 0",  0xACF},
    {L"DIGIT 1",  0xACE},
    {L"DIGIT 2",  0xACD},
    {L"DIGIT 3",  0xACC},
    {L"DIGIT 4",  0xACB},
    {L"DIGIT 5",  0xACA},
    {L"DIGIT 6",  0xAC9},
    {L"DIGIT 7",  0xAC8},
    {L"DIGIT 8",  0xAC7},
    {L"DIGIT 9",  0xAC6},
    {NULL, 0}
};

  this has been replaced by the file "t:\\button.ini" 
  
*/

/************************************************************************
*Function: InitRemotCodeTable() is used to initilize the RemotCodeTable *
*Para: void                                                             *
*************************************************************************/
void InitRemotCodeTable()
{
   int i=0;

   for(i=0; i<RemoteCodeSize;i++) {

       //RemoteCodeTable[i]= new REMOTE_CODE_TABLE;
	   RemoteCodeTable[i].Name = new WCHAR;
	   RemoteCodeTable[i].Code = 0;
   }

}

/***************************************************************************
*Function: hextpde() is ued to transfer hex string to decimal integer      *
*Para: string of CHAR which is 0x format                                   *
*Return: decimal integer of the hex string or -1 if error happened         *
****************************************************************************/
int hextode(CHAR hex[])
{

  int retn=0,i=0, k=0;
  int len=strlen(hex), pw=(int)pow(16,len-3);

  //hex begin 0x, so we count from the 0,1,2, from the third one
  for(i=2;i<len; ++i) {
	  if((hex[i]>= '0') && (hex[i] <= '9'))   k=  hex[i] - '0';
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


/*********************************************************************
*Function: Readrct() is used to read file to RemotCodeTable          *
*Para:     the array of file, and the size of the array              *
*Return:   void                                                      *
**********************************************************************/
  
void Readrct(char file[], int fsize)
{

  int size=RemoteCodeSize,i=0;
  CHAR *tempf = new CHAR [fsize];
  CHAR  *temp;
  
  strcpy(tempf,file);

  //ignore the first line of the comments
  if((temp=strtok(tempf, "\r\n")) == NULL) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }
   

  //this is the second line, to read time limit as decimal in seconds unit
  temp=strtok(NULL, " \r\n");

  int t=atoi(temp);
  if(t!=0)  MAXTIME=t; //if there is no timer, the default value 120 will be used

  for(i=0; i<size; i++) {

	  if((temp=strtok(NULL, " \r\n")) == NULL) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }  

	  swprintf(RemoteCodeTable[i].Name,L"%S", temp);

	  DebugPrint("RemoteCodeTable[%d].Name=%s \n",i, temp);

	  if( (temp=strtok(NULL, " \r\n")) == NULL ) {
		  
		  DebugPrint("Out of button.ini range, size is not correct \n");
		  return;
	  }  

	  RemoteCodeTable[i].Code = (WORD)hextode(temp);
	  DebugPrint("RemoteCodeTable[%d].Code=%s\n",i, temp);
  }
}


    
/***********************************************************
*Function:ReadButton() is used to read the button.ini file *
*Para: the file name                                       *
*Return: void                                              *
************************************************************/

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








void WorkScreen(int button)
{
	float wX=40, wY=50;
	screen=1;
	drCls();
	
	drPrintf(wX,wY,L"Xbox Tongle Test Version 1.00        Runing ...");

	drPrintf(wX,70.0,L"Time Limit Is %d Seconds",MAXTIME);

    wY+=60; 
	if((button>=0)&&(button<28)) drPrintf(wX,wY,L"Press Button %s",RemoteCodeTable[button].Name);
	else drPrintf(wX, wY, L"Unknow Button");
    drShowScreen();
}




void ShowFinalResult()
{
    

    float x1=0,x2=0,x3=0,x4=0,y1=0,y2=0,y3=0,y4=0;
    float m1=0,m2=0,m3=0,m4=0,n1=0,n2=0,n3=0,n4=0;

    drCls();
 
    if((TestStatus==1)&&(File==1)) {
		DebugPrint("Test passed.\n");

        drPrintf(40, 40,L"Test Passed");

		drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
		drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
	/*
		x1=450; y1=400; x2=530; y2=320; x3=550; y3=340; x4=470; y4=420; 
        n1=430; m1=380; n2=470; m2=420; n3=450; m3=440; n4=410; m4=400;  
		
		drQuad(x1, y1, 0xff000000, x2,  y2, 0xff000000, x3, y3, 0xff000000, x4, y4, 0xff000000);
        drQuad(n1, m1, 0xff000000, n2,  m2, 0xff000000, n3, m3, 0xff000000, n4, m4, 0xff000000);
	*/

	} else {

		DebugPrint("Test failed.\n");
		if(1!=File) drPrintf(40.0, 40.0, L"File Compare Failed");
		else if(timer) drPrintf(40.0, 40.0, L"Time Off");
		else if(TestStatus == -1) drPrintf(40, 40, L"Download Failed");
		else drPrintf(40, 40, L"Button Test Failed");

		drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
		drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);

    /*
		x1=450; y1=400; x2=530; y2=320; x3=550; y3=340; x4=470; y4=420; 
        n1=430; m1=380; n2=470; m2=420; n3=450; m3=440; n4=410; m4=400;  
                     
		drQuad(x1, y1, 0xff000000, x2,  y2, 0xff000000, x3, y3, 0xff000000, x4, y4, 0xff000000);
        drQuad(n1, m1, 0xff000000, n2,  m2, 0xff000000, n3, m3, 0xff000000, n4, m4, 0xff000000);

     */
	}
     
	drShowScreen();
	 

//       draw.DrawText(FormatBuffer, 50, 80, ProgressColor[dwDownloadStatus]);


}


/***********************************************************************************************
*Function: DiffFile() is used to compare two files: target file which is downloaded and the    *
*          source file which is stored at "t:\dvdsource.lib"                                   *
*Para: void                                                                                    *
*Return:1 different file, 0 same file                                                          *
************************************************************************************************/



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

    //if file size are different, the file is different
	if(tfsize!=sfsize) {
		DebugPrint("Size are different\nSource file size is %d but ", sfsize);
        DebugPrint("Target file size is %d\n", tfsize);
		return 1;
	}
	else DebugPrint("The same file, size is %d \n", sfsize);
	
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
	
	//compare the two file byte by byte
	if(memcmp(tm, sm, sfsize)) {
		DebugPrint("NOT the same file\n");
		return 1;
	}
	
    DebugPrint("The same file, Success File Compare\n");
	return diff;

}


/*****************************************************************
*Function: ShowScreen() is used to show there is now IR Receiver *
******************************************************************/

void ShowScreen()
{

    drCls();
	 
	drPrintf(10,20,L"Xbox Tongle Test Version 1.00 ");

	drPrintf(10,50,L"No IR Remote Receiver");

	drShowScreen();


}

/************************************************************************
*Function:GetCodeName() is used to return the button's name             *
*************************************************************************/

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
	CHAR button[]="t:\\button.ini";

	

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
//    CDraw draw;
//    WCHAR FormatBuffer[256];

	TestStatus =1; //initiliaze as success, any bad thing happend, it will fail

     
    //Initialize core peripheral port support
    XInitDevices(0,NULL);
	
	drInit();

	InitRemotCodeTable();

	ReadButton(button);
	

//	drShowScreen();
//	myY+=20;
	//drShowScreen();


//    draw.FillRect(0,0,640,480,PITCH_BLACK);
        
    //Loop Forever Printing out the Library information
	Rom=0;
	Dongle=1;
	Rom=1; File=-1;

	struct tm *ptr;
	time_t lt;
 
	ShowScreen();

    while(1) {

		
		   
//		DebugPrint("version 1\n");

           if( (mycurrenttime - mystarttime) > MAXTIME) { 
			   DebugPrint("Time off \n");
			   timer=1;
			   TestStatus = 0;
			   goto Final;
		   }


		   if(remotecode==0) {
			   lt=time('\0');
			   ptr=localtime(&lt);
                
			   mystarttime = (ptr->tm_hour) * 360 + (ptr->tm_min)*60 + ptr->tm_sec;
			   mycurrenttime=mystarttime;

		//	   starttick=GetTickCount();
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
         


// Compare the downloaded file in t:\dvdcode.lib with t:\dvdsource.lib byte by byte using DiffFile(), 
// DiffFile return 1 if two files are different, 0 otherwise.

		if((dwDownloadStatus == XDCS_STATUS_DOWNLOAD_SUCCEEDED)&&(File==-1)) {
			

		 
//			target = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
			    goto Final;

			}   

		} else if((dwDownloadStatus==XDCS_STATUS_DOWNLOAD_FAILED) ||(dwDownloadStatus==XDCS_STATUS_FILE_WRITE_FAILED)) {
			TestStatus=-1;
			DebugPrint("DownLoad Filed \n");
            if(!CloseHandle(target)) DebugPrint("CloseHandle(target) faile: %d\n", GetLastError());
			goto Final;
		}


        /**
        *** Poll the XID Remote part.
        *** Just because we don't have lot's of creative ways to
        *** abuse the XInput API, we will be clever here, and use the API in a valid,
        *** but less than optimal ways.  It had better work too!
        **/
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


            if(ERROR_SUCCESS == XInputGetState(hRemote, (PXINPUT_STATE)(IrCodeBuffer+dwIrCodeBufferPos)))
            {
                //Don't move for duplicates
                if(dwPacketNumber != IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber)
                {
                    dwPacketNumber = IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber;
                    dwIrCodeBufferPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                }
                
                
                if(dwPacketNumber)
                {
                    DWORD dwCodeBufferDisplayPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                    DWORD dwDisplayYPos = 265 + (IR_CODE_BUFFER_SIZE*15);
                    LPCWSTR pwszCodeName;
                   for(int i=1; i < IR_CODE_BUFFER_SIZE; i++)
                   {
                        pwszCodeName = GetCodeName(IrCodeBuffer[dwCodeBufferDisplayPos].IrRemote.wKeyCode);
                        if(pwszCodeName)
                        {
                            

							if(oldCodeName==pwszCodeName) remotecode++;
                        }
                        dwDisplayYPos -= 15;
                        dwCodeBufferDisplayPos = (dwCodeBufferDisplayPos+1)%IR_CODE_BUFFER_SIZE;
				   }
                }
            } else
            {   
                XInputClose(hRemote);
                hRemote = NULL;
                memset(IrCodeBuffer, 0, sizeof(IrCodeBuffer));
				//screen=0;
                 
            }
        } else
        {
             
			screen=0;
        }

        //Preset
        if(screen==0) ShowScreen();


		lt=time('\0');
		ptr=localtime(&lt);

		mycurrenttime = (ptr->tm_hour) * 360 + (ptr->tm_min)*60 + ptr->tm_sec;         
	 

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
			File=-1;
			mycurrenttime =0;
			mystarttime=0;
		}
    }
	

}

 

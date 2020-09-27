/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    thumb.cpp

Abstract:

    initializes global variables, contains main loop, init, and  test for the thumb positioning

Author:

    Bing Li (a-bingli) April 2001

Notes: This program can be modified to paint the screen using the thumb stick.
       Because OS has problems temporarily now, there are problems with pluging in and out. 
--*/

#include "xtl.h"
#include <stdio.h>
#include "dukeprod.h"
#include "ccl.h"
#include "device.h"
#include "draw.h"
#include "mmu.h"
#include "Tsc.h"

#define countof(_x_) (sizeof(_x_) / sizeof(_x_[0]))

BOXSTATUS gPadStatus[MAX_SLOTS];

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

    DebugPrint("Welcome to Duke Production Test: Built on %s at %s\n", __DATE__,__TIME__);

    //Initialize core peripheral port support
    XInitDevices(0,NULL);
    

	gpInit(0);
	drInit();
	//muInit();

	for(i = 0; i < MAX_SLOTS; i++) {
		gPadStatus[i] = notyet;
	}
	
	TscInit(0);

	DebugPrint("Done with initialization!\n");
}


void binginit()
{

	gpInit(0);
	drInit();
	//muInit();

	for(int i = 0; i < MAX_SLOTS; i++) {
		gPadStatus[i] = notyet;
	}
	
	TscInit(0);

	DebugPrint("Done with Bing initialization!\n");

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

//	gModeFunctions[gMode](STATUS_LEAVE, 0);
//	for(int i = 0; i < MAX_MODES; i++) {
//		gModeFunctions[i](STATUS_QUIT, 0);
//	}

	gpDeinit();
	drDeinit();
	//muDeinit();

	DebugPrint("Deinit is finished.\n");
	
}


/*************************************************************************************
Function:	test() is used to move thumb sticker around.
Params:		the slot number which device has been inserted
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
 

gpGAMEPAD * gData;

int ratio = 32768 / 240; //map from -32678 - 32678 to 0 - 480
float length = 20.0;    //length of the cross
float area = 480.0 * 480.0;      //area of the total sqare

BOXSTATUS test(int slot)
{
	int i=0, j=0, flag=0;
	float enlarge=0, smallarea=0.0;
	__int64 tim;
	BOXSTATUS overall = passed;
	 
	float xl=0.0, yl=0.0, xr=0.0,yr=0.0;
	float width=140.0, high=15.0;

	
    float x1l=0.0, y1l=0.0, x1r=0.0,y1r=0.0;
	float x2l=0.0, y2l=0.0, x2r=0.0,y2r=0.0;
	float x3l=0.0, y3l=0.0, x3r=0.0,y3r=0.0;
	float x4l=0.0, y4l=0.0, x4r=0.0,y4r=0.0;


	BYTE datar=0, datal=0;

	gData = ::gpGetState(slot);

	drCls();
	
	 //loop until you plug out the device
		while(1) {
	 
		   gpUpdateDeviceStateInfo();				//update the button state info...

		   if(!::gpDeviceExists(slot))	{
				return failed; 
			}
			
		   gpPoll();
		   gData = ::gpGetState(slot);
             
           //we can go back to start another test to chose A or B to test Pad or ThumbStick
		   if(gData->bAnalogButtons[GPX]) return pending;

           xl =  (float)( gData->sThumbLX / ratio + 240);
           yl =  (float)((-1) * gData->sThumbLY / ratio + 240);
           xr =  (float)( gData->sThumbRX / ratio + 240);
           yr =  (float)((-1) * gData->sThumbRY / ratio + 240);

			 

          datal = gData->bAnalogButtons[GPL];
		  datar = gData->bAnalogButtons[GPR];

		  
		  //DebugPrint("Left Trigger: %d \n", datal);
          //DebugPrint("Right Trigger: %d \n", datar);

          

		  if((datal==0)&&(datar==0)) {  //draw normal cross
			  drCls();
			  length=20.0;
		  } else if(datar>0) {
			  drCls();   //clear the screen
			  flag=0;    //turn on to normal
		  }
		  else if(datal>0) {            // draw dots and keep it on the screen always
			  length=0.0;
			  //draw a black box to erase the old value, so we can update the new x, y
			  drQuad(490,20-high,0xff000000,490+width,20-high,0xff000000,490+width,20+high,0xff000000,490,20+high,0xff000000);
			  drQuad(490,40-high,0xff000000,490+width,40-high,0xff000000,490+width,40+high,0xff000000,490,40+high,0xff000000);
			  drQuad(490,120-high,0xff000000,490+width,120-high,0xff000000,490+width,120+high,0xff000000,490,120+high,0xff000000);
			  drQuad(490,140-high,0xff000000,490+width,140-high,0xff000000,490+width,140+high,0xff000000,490,140+high,0xff000000);
		  }

//draw a square to show the range 0-480
          drLine(0.0, 0.0, 480.0, 0.0, 0x000000ff);
          drLine(0.0, 479.0, 479.0, 479.0, 0x000000ff);
          drLine(0.0, 0.0, 0.0, 480.0, 0x000000ff);
          drLine(480.0,0.0, 480.0,480.0, 0x000000ff);


//draw a small square to show the center if start is pushed
		  if((gData->wButtons) & 16) {
			  flag=1;
			  enlarge++;             //push start will enlarge the center
		  } else if((gData->wButtons) & 32 ) {
			  enlarge--;              //puch back will make the center small
		  }

          if(flag) {

			  if(enlarge > 235) enlarge = 235;   //this is limited in the range
			  else if(enlarge<0) enlarge =0;

			  //set the range of the center
			  x1l=(float)(235.0 - enlarge);  y1l=(float)(235.0 - enlarge); x1r=(float)(245.0 + enlarge); y1r=(float)(235.0 - enlarge);
	          x2l=(float)(235.0 - enlarge);  y2l=(float)(245.0 + enlarge); x2r=(float)(245.0 + enlarge); y2r=(float)(245.0 + enlarge);
	          
			  // get covered part
			  smallarea = x1r - x2l ;

			  //print out how much percent length is covered
			  double c= 100.0 * ( smallarea / 480.0 ) ;
			  double s= ( 32768.0 * (x1r - 240.0) ) / 240.0;

			  drPrintf(490, 220, L"Percent Covered:");
			  drPrintf(490, 240, L"%4.2f",c);
			  drPrintf(490, 260, L"Size is %5.0f",s);

			  if( ((xl<x1r) && (xl>x1l)) && ((yl>y1l) && (yl<y2l)) ) drPrintf(490, 60, L"ZERO");

			  if( ((xr<x1r) && (xr>x1l)) && ((yr>y1l) && (yr<y2l)) ) {
				  drSetSize(8.0, 15.0, 0x0000ffff, 0xff000000); //set the color
			      drPrintf(490, 160, L"ZERO");
                  drSetSize(8.0, 15.0, 0xff00ff00, 0xff000000); //default color
			  }
              //draw the small square
			  drLine(x1l, y1l, x1r,y1r, 0xff0000ff);
              drLine(x2l, y2l, x2r,y2r, 0xff0000ff);
              drLine(x1l,y1l,x2l,y2l, 0xff0000ff);
              drLine(x1r,y1r,x2r,y2r, 0xff0000ff);
		  }


//draw a big red dot to show the center
          drQuad(235.0, 235.0, 0xffff0000, 245.0,  235.0, 0xffff0000, 245.0, 245.0, 0xffff0000, 235.0, 245.0, 0xffff0000);

//print out the value of x and y

         //left thumb
		 drPrintf(490,0,L"LeftThumb:");
         drPrintf(490,20,L"X=%d", gData->sThumbLX);
         drPrintf(490,40,L"Y=%d", gData->sThumbLY);

        // right thumb, print the same blue color as the line, reset the default color and then reset back again
         drSetSize(8.0, 15.0, 0x0000ffff, 0xff000000);
         drPrintf(490,100,L"RightThumb:");
         drPrintf(490,120,L"X=%d", gData->sThumbRX);
         drPrintf(490,140,L"Y=%d", gData->sThumbRY);
         drSetSize(8.0, 15.0, 0xff00ff00, 0xff000000); //default color

//draw a green cross for left thumb and a blue cross for right thumb
		 //left thumb
          drLine(xl-length, yl, xl+length, yl, 0x0000ff00);
          drLine(xl, yl-length, xl, yl+length, 0x0000ff00);
		 
		  //right thumb
          drLine(xr-length, yr, xr+length, yr, 0x0000ffff);
          drLine(xr, yr-length, xr, yr+length, 0x0000ffff);

  
        drShowScreen();



		}
	return overall;
}

/*************************************************************************
*Function: PadTest(int slot) is used to test the Game_Pad                *
*Para: int exiting duke slot                                             *
*Return: test status                                                     *
**************************************************************************/

int COUNT = 20;
float xmove = 32.0, ymove = 24.0;

BOXSTATUS PadTest(int slot)
{

	float xl=0,yl=0, xr=0,yr=0,xdl=0,ydl=0,xdr=0,ydr=0;
	float minxy=0,maxx=640.0, maxy = 480.0;
	float xleft=0,yleft=0,xright=0,yright=0;
	 
	int count=0;
	BOXSTATUS overall = passed; //default as passed

    gData = ::gpGetState(slot);
    
	drCls();
	
	//draw a center first 
	for(count=0; count<20;count++) {
	 
		drLine(xleft, minxy, xleft, maxy, 0x000000ff);		
		drLine(minxy, yright, maxx, yright, 0x000000ff);
        
		xleft+=xmove;
		yright+=ymove;
	}

    drLine(xleft-2, minxy, xleft-2, maxy, 0x000000ff);		
    drLine(minxy, yright-2, maxx, yright-2, 0x000000ff);

	xl=(float)(320.0-xmove); yl=(float)(240.0-ymove);xr=320.0;yr=(float)(240.0-ymove);
    xdr=320.0;ydr=240.0;xdl=(float)(320.0-xmove);ydl=240.0;

    drQuad(xl, yl, 0xff00ff00, xr, yr,0xff00ff00, 
		           xdr, ydr, 0xff00ff00, xdl, ydl, 0xff00ff00);
    drShowScreen();
   
	// loop until we want to quit. we have a bug now, we have to start the application after remove the device
	while(1) {
		
		  gpUpdateDeviceStateInfo();				//update the button state info...

		   if(!::gpDeviceExists(slot))	{
				return failed; 
			}
			
		   gpPoll();
		   gData = ::gpGetState(slot);

		   //we can start another test to chose A or B to test Pad or ThumbStick
		   if(gData->bAnalogButtons[GPX]) return pending;

		   //no action taken, waiting to new action
           if(!gData->wButtons) continue;

		    
		   //move 
		   if(gData->wButtons == XINPUT_GAMEPAD_DPAD_UP) {    
               yl-=ymove;
			   yr-=ymove;
			   ydl-=ymove;
			   ydr-=ymove;

		   } else if(gData->wButtons == XINPUT_GAMEPAD_DPAD_DOWN)  {   
               yl+=ymove;
			   yr+=ymove;
			   ydl+=ymove;
			   ydr+=ymove;
		   } else if(gData->wButtons == XINPUT_GAMEPAD_DPAD_LEFT) {
			    
               xl-=xmove;
			   xr-=xmove;
			   xdl-=xmove;
			   xdr-=xmove;

		   } else if(gData->wButtons == XINPUT_GAMEPAD_DPAD_RIGHT) {
			    
			   xl+=xmove;
			   xr+=xmove;
			   xdl+=xmove;
			   xdr+=xmove;
		   } else if(gData->wButtons == (XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT)) {
			    
               yl-=ymove;
			   yr-=ymove;
			   ydl-=ymove;
			   ydr-=ymove;
               xl-=xmove;
			   xr-=xmove;
			   xdl-=xmove;
			   xdr-=xmove;
		   }  else if( gData->wButtons == ( XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT)) {
			   DebugPrint("UpRight \n");
			   yl-=ymove;
			   yr-=ymove;
			   ydl-=ymove;
			   ydr-=ymove;
               xl+=xmove;
			   xr+=xmove;
			   xdl+=xmove;
			   xdr+=xmove;
		   }  else if( gData->wButtons ==( XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT) ) { 
			   DebugPrint("Down Left \n");
               yl+=ymove;
			   yr+=ymove;
			   ydl+=ymove;
			   ydr+=ymove;
               xl-=xmove;
			   xr-=xmove;
			   xdl-=xmove;
			   xdr-=xmove;
		   } else if(gData->wButtons == (XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT)) {
               yl+=ymove;
			   yr+=ymove;
			   ydl+=ymove;
			   ydr+=ymove;
			   xl+=xmove;
			   xr+=xmove;
			   xdl+=xmove;
			   xdr+=xmove;

		   }  
		   
		   // set limitation 
		   if(xl<0) { 
			   xl=0;
			   xr=xmove;
			   xdl=0;
			   xdr=xmove;
		   }

		   if(xr>640) {
			   xr=xdr=639;
			   xl=xdl=640-xmove;
		   }

		   if(yl<0) {
			   yl=yr=0;
			   ydl=ydr=ymove;
		   }

		   if(ydl>480) {
			   yl=yr=480-ymove;
			   ydl=ydr=479;
		   }	   
			   
		    //draw a now position         
		  drCls();
		   
	      drQuad(xl, yl, 0xff00ff00, xr, yr,0xff00ff00,xdr, ydr, 0xff00ff00, xdl, ydl, 0xff00ff00);

		  xleft=0;yleft=0;xright=0;yright=0;
		  for(count=0; count<20;count++) {
			      drLine(xleft, minxy, xleft, maxy, 0x000000ff);		
		          drLine(minxy, yright, maxx, yright, 0x000000ff);
			      xleft+=xmove;
			      yright+=ymove;
		   }
          drLine(xleft-2, minxy, xleft-2, maxy, 0x000000ff);		
          drLine(minxy, yright-2, maxx, yright-2, 0x000000ff);
		  drShowScreen();

	}

	//never be here
	return overall;


}
	 


/**************************************************
*Main Function
***************************************************/
int __cdecl main()
{
//	_asm int 3;

	int i;

	init();

Start:
	gpPoll();
	 
	drCls();
	drPrintf(10, 20, L"Press A To Test Pad");
	drPrintf(10, 40, L"Press B To Test Thumb");
	drShowScreen();

     
	while(1) //loop until we're supposed to quit...
	{
		gpUpdateDeviceStateInfo();				//update the button state info...
		//muGetDeviceMessages();
          
		
		for(i = 0; i < MAX_SLOTS; i++) {
			if(!gpDeviceExists(i)) {
				::gPadStatus[i] = notyet;
				 
			} else if (gPadStatus[i] == notyet) {
                
				gpPoll();
		        gData = ::gpGetState(i);

				if(gData->bAnalogButtons[GPA]) { 
					gPadStatus[i]=PadTest(i);
					if(gPadStatus[i] == pending ) {
					    gPadStatus[i]=notyet;
						goto Start;
					}

				} else if(gData->bAnalogButtons[GPB])	{
					if((gPadStatus[i] = test(i))== pending) {
					    gPadStatus[i]=notyet;
						goto Start;
					}
				}
            	else continue;

				drCls();

				// this will be always failed now, we just use this as a sign to show whether the device is inserted.
				if(gPadStatus[i] == passed) {
					//passed
					drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
					drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
				} else {
					//failed
					drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
					drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);
				}
				drShowScreen();
			}
		}
	}
 

	//we should theoretically never get here on xbox......

	deinit();


	return 0;
}


#include <xtl.h>


#include "device.h"
#include "draw.h"
#include "modes.h"
#include "xpong.h"

extern "C" ULONG DebugPrint(PCHAR Format, ...);

//extern "C" int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)
int __cdecl main()
{
    //Initialize core peripheral port support
    XInitDevices(0,NULL);
    
    gpInit(1);		//initialize device stuff, do control-pad remapping
	drInit();		//initialize drawing routines
		
	XPong(STATUS_BOOT, 0);	//initialize pong	
	XPong(STATUS_ENTER, 1);	//initialize pong, step 2, and 

    while(1) { //for(int i = 0; i < 30; i++) {
		gpUpdateDeviceStateInfo();				//update the button state info...
		gpPoll();								//query for the next button states
		drCls();								//clear the screen
		XPong(STATUS_NORMAL, 0);				//update pong state
		drShowScreen();							//and draw the screen
	}

	XPong(STATUS_LEAVE, 1);	//deinitialize pong
	XPong(STATUS_QUIT, 0);	//quit pong

	return 0;
}

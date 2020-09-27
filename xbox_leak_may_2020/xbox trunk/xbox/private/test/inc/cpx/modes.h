#ifndef __modes_h_
#define __modes_h_

#define STATUS_BOOT     (-1)    //program is starting
#define STATUS_ENTER	1		//switch to this mode
#define STATUS_NORMAL	0		//normal...
#define STATUS_LEAVE    2		//leaving a mode
#define STATUS_QUIT		(-2)	//program is quitting, so clean up.
typedef VOID (*ModeFunc)(DWORD status, INT param);			//the mode functions...

#endif
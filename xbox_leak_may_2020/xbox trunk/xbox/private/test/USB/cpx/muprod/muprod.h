#ifndef __muprod_h_
#define __muprod_h_

#define MAX_SLOTS XGetPortCount()
//typedef enum {none, tested} padstatus;
typedef enum {notyet, testme, pending, failed, passed} BOXSTATUS;
typedef BOXSTATUS (*BOXFUNC)(int port, int slot);
extern BOXSTATUS gPadStatus[MAX_SLOTS];

//#include "mp.h"

extern BOXFUNC b[];
//extern deGAMEPAD * gData;

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

	BOXSTATUS LowLevelTest(int port, int slot, int &BB1, int &BB2, int &BB3);
    BOXSTATUS FormatTest(int port, int slot);
	BOXSTATUS VerifyTest(int port, int slot);
	BOXSTATUS WriteReadTest(CHAR drive);
	BOXSTATUS CountSectorWriteTest(CHAR drive);

#endif
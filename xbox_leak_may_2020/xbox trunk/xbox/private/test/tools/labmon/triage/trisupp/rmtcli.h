#ifndef _RMTCLI_
	#define _RMTCLI_

#include <time.h>

DWORD rmConnectToRemote(CHAR *server, CHAR *srvpipename);
BOOL rmCloseRemote();
BOOL SendMyCommand(const CHAR *theCommand, CHAR **RetBuffer, time_t tWaitTime);

#endif
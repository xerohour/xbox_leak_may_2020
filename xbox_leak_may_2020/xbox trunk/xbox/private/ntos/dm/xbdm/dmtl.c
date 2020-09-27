/*
 *
 * dmtl.c
 *
 * Debug monitor transport layer
 *
 */

#include "dmp.h"

BOOL DmStartRmtSession(ULONG ulIPAddr)
{
    /* We need to establish the connection before we can successfully spin off
     * the debug thread */
	SOCKET sockMain;
	SOCKET sockPoll;
	struct sockaddr_in addr;

    _asm int 3

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ulIPAddr);

	/* Connect the main thread */
	sockMain = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == sockMain) {
        _asm int 3
        return FALSE;
    }
	addr.sin_port = htons(0x801);
	if(0 != connect(sockMain, (struct sockaddr *)&addr, sizeof addr)) {
        _asm int 3
		closesocket(sockMain);
		return FALSE;
	}

	/* Connect the poll thread */
	sockPoll = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sockPoll) {
        _asm int 3
	    closesocket(sockMain);
	    return FALSE;
	}
	addr.sin_port = htons(0x802);
	if(0 != connect(sockPoll, (struct sockaddr *)&addr, sizeof addr)) {
        _asm int 3
		closesocket(sockPoll);
		closesocket(sockMain);
		return FALSE;
	}

	return TRUE;
}

void DmListenForRmtSession(void)
{
}

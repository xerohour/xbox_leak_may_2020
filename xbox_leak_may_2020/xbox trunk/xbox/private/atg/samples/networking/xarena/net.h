#ifndef __NET_H
#define __NET_H

#include "xtank.h"
#include "xmenu.h"

#ifdef __cplusplus
extern "C" {
#endif
			
#define MAX_PLAYER_NAME 14
struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    WCHAR wstrPlayerName[MAX_PLAYER_NAME];	// Player name

	CXTank *pTank;							// tank pointer
	DWORD dwTankIdx;						// index in tank list
};

extern DWORD g_dwBytesSent;
extern DWORD g_dwBytesReceived;

HRESULT Net_Init();
DWORD Net_Menu(DWORD cmd, XMenuItem *mi);

void Net_SendUpdateTank(CXTank *pTank);
void Net_SendFireWeapon(CXObject *pObj, BYTE bType);
void Net_SendKill(DPNID dpnidKiller);


#ifdef __cplusplus
}
#endif

#endif
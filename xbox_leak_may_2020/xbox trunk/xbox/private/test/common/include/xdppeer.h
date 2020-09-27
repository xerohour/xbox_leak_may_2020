#ifndef __XDPPEER_H
#define __XDPPEER_H

#include <xtl.h>
#include <dplay8.h>

extern LPDIRECTPLAY8PEER g_pDP;					// DirectPlay peer object

extern LPDIRECTPLAY8ADDRESS g_pDeviceAddress;	// device address of SP
extern LPDIRECTPLAY8ADDRESS g_pHostAddress ;	// host address of SP

extern DWORD g_dwNumPlayers;					// # of players in game
extern DWORD g_dpnidLocalPlayer;				// DPNID of local player
extern DWORD g_dpnidHost;						// DPNID of host

extern WCHAR g_wszXDPPlayerName[32];			// players name
extern WCHAR g_wszXDPSessionName[32];			// session name

// connect stuff
enum
{
	GAME_NOTCONNECTED=0,						// no network connection
	GAME_ENUMERATING,							// searching for hosts
	GAME_CONNECTING,							// connecting to a host
	GAME_CONNECTED,								// connected to a host
	GAME_HOSTING,								// hosting a game
	GAME_DISCONNECTING							// disconecting from a game
};

extern DWORD g_dwConnectStatus;

// active game list stuff
#define MAX_ACTIVEGAMES 32
typedef struct _ActiveGame
{
	DPN_APPLICATION_DESC AppDesc;
	IDirectPlay8Address	*pHostAddr;				// address of host who responded
	IDirectPlay8Address	*pDevAddr;				// device response was received on
	DWORD dwPing;								// ping time
	DWORD dwReportTime;							// last time this server reported
} ActiveGame;

// this message tells the app to delete a game from
// the game menu. hopefully it will never collide with
// any dplay message id's.
#define DPPEER_MSGID_DELETE_GAME (DPN_MSGID_OFFSET|0x0110)

#ifdef __cplusplus
extern "C" {
#endif

HRESULT XDPInit(HRESULT (WINAPI *DPMsgHandler)(void *, DWORD, void *));
void XDPShutdown();
HRESULT XDPDoWork(DWORD param);

HRESULT XDPEnumHosts(DWORD port, GUID *pGuid);
ActiveGame *XDPAddGame(DPNMSG_ENUM_HOSTS_RESPONSE *pHost);
void XDPCancelEnumHosts();
void XDPExpireGames();

HRESULT XDPCreate(DWORD port, GUID *pGuid, DWORD dwMaxPlayers, DWORD flags);
HRESULT XDPConnect(DPN_APPLICATION_DESC *pAppDesc, IDirectPlay8Address *pHostAddr, IDirectPlay8Address *pDevAddr);
HRESULT XDPDisconnect();
HRESULT XDPTerminate();

#define XDPIsConnected() (g_dwConnectStatus==GAME_CONNECTED||g_dwConnectStatus==GAME_HOSTING)

#ifdef __cplusplus
}
#endif

#endif
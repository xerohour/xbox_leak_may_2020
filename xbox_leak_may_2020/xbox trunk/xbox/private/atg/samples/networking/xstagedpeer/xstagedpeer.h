#ifndef __XSTAGEDPEER_H
#define __XSTAGEDPEER_H

// game state
#define GAMESTATE_ATTRACT 0						// in attract mode (not playing)
#define GAMESTATE_STAGING 1						// game is in multiplayer stage
#define GAMESTATE_PLAYING 2						// game is active
extern DWORD g_dwGameState;

// app specific Player information 
#define MAX_PLAYER_NAME 14
struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    WCHAR wstrPlayerName[MAX_PLAYER_NAME];	// Player name
};
#define PLAYER_ADDREF(a)    if(a) a->lRefCount++;
#define PLAYER_RELEASE(a)   if(a) { a->lRefCount--; if(a->lRefCount<=0) delete a; } a = NULL;

#endif
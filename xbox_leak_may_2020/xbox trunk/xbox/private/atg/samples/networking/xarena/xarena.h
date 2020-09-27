#ifndef __XARENA_H
#define __XARENA_H

#include "xtank.h"

#define FLOORX 90.0f					// size of floor
#define FLOORY 90.0f
#define FLOORZ 90.0f					// position range is +- SIZE/2
#define POSXBITS 7						// # of integer bits needed 
#define POSZBITS 7						// to represent a position. 
#define POSYBITS 7

#define OBJ_TANK		0x0001			// object is a tank
#define OBJ_OBSTACLE	0x0002			// object is an obstacle

#define MAX_TANKS 16					// max 16 players
extern CXTank *g_pTank[MAX_TANKS];
extern DWORD g_dwNumTanks;

#define frand(a) ((float)rand()*(a)/32768.0f)

#endif
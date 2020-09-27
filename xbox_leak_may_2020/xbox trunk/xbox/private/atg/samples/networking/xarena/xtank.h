//-----------------------------------------------------------------------------
// File: xtank.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __XTANK_H
#define __XTANK_H

#include "xbapp.h"
#include "xbfont.h"
#include "xobj.h"
#include "xbinput.h"

//-----------------------------------------------------------------------------
// weapons
//-----------------------------------------------------------------------------
#define MAX_WEAPONOBJS	32

#define WEAPON_GUN		0
#define WEAPON_ROCKET	1
#define MAX_WEAPONS		2

// fire flags
#define WF_REMOTE		0x01		// fired by remote player

// weapon structure
struct CXWeapon
{
	WCHAR m_wstrDesc[32];			// weapon name
	DWORD m_dwType;					// weapon type
	float m_fSpeed;					// weapon speed
	float m_fFireRate;				// seconds till can fire again
	float m_fDamage;				// damage caused
};

// weapon routines
void Weapon_Init();
CXObject *Weapon_Fire(D3DXVECTOR3 *pPos, float fYRot, BYTE bType, DWORD dwFlags);
void Weapon_Update(float fElapsedTime);
void Weapon_Render();

//-----------------------------------------------------------------------------
// tanks
//-----------------------------------------------------------------------------

// tank flags
#define TANK_REMOTE		0x0001		// this tank controlled remotely

// tank class
class CXTank
{
public:

	CXObject *m_pObj;				// display object
	DWORD m_dwFlags;				// tank flags
	DWORD m_dwTankIdx;				// index in g_pTank[] array

	// characteristics
	float m_fWidth;					// width of tank
	float m_fLength;				// length of tank
	float m_fHeight;				// height of tank
	float m_fMoveSpeed;				// movement speed
	float m_fRotSpeed;				// rotation speed

	// movement state
	D3DXVECTOR3 m_vVel;				// current velocity
	D3DXVECTOR3 m_vAcc;				// current acceleration
	float m_fRotVel;				// y rotational velocity

	DWORD m_dwNumWarpFrames;		// # of frames to do warp adjustment
	D3DXVECTOR3 m_vWarpDist;		// distance to adjust each frame

	// statistics
	DWORD m_dwShots;				// # shots fired
	DWORD m_dwHits;					// # times we hit opponent
	DWORD m_dwOppHits;				// # times opponent hit us

	// life
	float m_fArmor;					// current armor protection
	float m_fLife;					// current life level
	void *m_pvKillPlayer;			// player who killed me

	// weapons
	DWORD m_dwCurWeapon;			// current weapon
	float m_fAmmo[MAX_WEAPONS];		// weapon ammo
	float m_fLastShotTime;			// last time we shot

	CXTank(D3DXVECTOR3 *pos, float yrot);
	~CXTank();

	void Update(XBGAMEPAD *gamepad, float etime);
	void Hud(CXBFont *font);
};

#endif
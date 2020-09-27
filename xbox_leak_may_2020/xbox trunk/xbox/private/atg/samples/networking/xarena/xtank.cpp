//-----------------------------------------------------------------------------
// File: xtank.cpp
//
// Desc: hovertank code for xarena
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "xbapp.h"
#include "xbinput.h"
#include "xdppeer.h"

#include "xtank.h"
#include "xarena.h"
#include "coll.h"
#include "xpart.h"
#include "net.h"

//-----------------------------------------------------------------------------
// misc
//-----------------------------------------------------------------------------
CXBMesh *g_pTankMesh = NULL;					// tank model
extern CXParticleSystem *XAPart;				// particle system pointer


//-----------------------------------------------------------------------------
// weapons
//-----------------------------------------------------------------------------
CXModel Weapon_GunModel, Weapon_RocketModel;	// weapon models
CXWeapon Weapons[MAX_WEAPONS] =
{
	{L"BLASTER", WEAPON_GUN, 75.0f, 0.1f, 5},
	{L"ROCKET", WEAPON_ROCKET, 50.0f, 1.0f, 30},
};

CXObject WeaponObjs[MAX_WEAPONOBJS];
DWORD g_dwNumWeaponObjs = 0;


//-----------------------------------------------------------------------------
// Name: CXTank
// Desc: Tank object constructor
//-----------------------------------------------------------------------------
CXTank::CXTank(D3DXVECTOR3 *pos, float yrot)
{
	D3DXMATRIX *m;
	D3DXVECTOR4 v;

	ZeroMemory(this, sizeof(CXTank));

	// get new object and set position and rotation
	m_pObj = new CXObject;
	m_pObj->m_dwFlags = OBJ_TANK;
	m_pObj->m_pvInfo = this;		// point to tank info

	m_pObj->m_vPosition = *pos;
	m_pObj->m_vRotation.y = yrot;

	// init misc parameters
	m_fMoveSpeed = 20.0f;
	m_fRotSpeed = 1.5f;
	m_fWidth = 1.5f;
	m_fHeight = 1.0f;

	m_fArmor = 100.0f;
	m_fLife = 100.0f;

	// calc new matrix
	m = &m_pObj->m_matOrientation;
	D3DXMatrixRotationY(m, yrot);

	// load the tank mesh if not already loaded
	if(g_pTankMesh==NULL)
	{
		g_pTankMesh = new CXBMesh();
		g_pTankMesh->Create(g_pd3dDevice, "models\\tank.xbg" );
	}

	// set the mesh for our tank object
	m_pObj->SetXBMesh(g_pTankMesh);

	// init weapon info
	m_dwCurWeapon = WEAPON_GUN;
	m_fAmmo[WEAPON_GUN] = 1000.0f;
	m_fAmmo[WEAPON_ROCKET] = 100.0f;
	m_fLastShotTime = 0.0f;

	// init movement state
	m_vVel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAcc = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fRotVel = 0.0f;
}

//-----------------------------------------------------------------------------
// Name: ~CXTank
// Desc: Tank object destructor
//-----------------------------------------------------------------------------
CXTank::~CXTank()
{
	delete m_pObj;
}


//-----------------------------------------------------------------------------
// Name: Update
// Desc: Updates a tanks position, orientation, and firing
//       If it is a local tank, this information is taken from the joypad,
//       if the tank is remotely controlled, it is updated with dead-reckoning.
//-----------------------------------------------------------------------------
void CXTank::Update(XBGAMEPAD *gamepad, float etime)
{
	D3DXMATRIX *m;
	D3DXVECTOR3 pos, v;

	m = &m_pObj->m_matOrientation;			// get current matrix

	float xacc, zacc;
	float xvel, zvel, rvel;
	float sinry, cosry;

	if(m_dwFlags&TANK_REMOTE)
	{
		// this is a remotely controlled tank,
		// so do warp processing if necessary
		if(m_dwNumWarpFrames>0)
		{
			m_pObj->m_vPosition += m_vWarpDist;
			m_dwNumWarpFrames--;
		}
	}
	else
	{
		// this is a locally controlled tank

		// check if we were killed
		if(m_fLife<=0.0f)
		{
			// we're dead, teleport to some new location
			m_pObj->m_vPosition.x = frand(80.0f)-40.0f;
			m_pObj->m_vPosition.y = 0.0f;
			m_pObj->m_vPosition.z = frand(80.0f)-40.0f;
			m_pObj->m_vRotation.y = frand(3.14159f*2.0f);
			Net_SendUpdateTank(this);

			// reset statistics
			m_fArmor = 100.0f;
			m_fLife = 100.0f;
			m_fAmmo[WEAPON_GUN] = 1000.0f;
			m_fAmmo[WEAPON_ROCKET] = 100.0f;
			m_fLastShotTime = 0.0f;
		}
		else
		{
			// not dead, get acceleration from gamepad
			m_vAcc.x = gamepad->fX1*m_fMoveSpeed*4.0f;
			m_vAcc.z = gamepad->fY1*m_fMoveSpeed*4.0f;
		}
	}

	xacc = m_vAcc.x;
	zacc = m_vAcc.z;

	// if no acceleration, decelerate the tank
	if((xacc<0.1f) && (xacc>-0.1f))
		xacc = -(m_vVel.x*4.0f);
	if((zacc<0.1f) && (zacc>-0.1f))
		zacc = -(m_vVel.z*4.0f);

	// adjust velocity
	m_vVel.x += xacc*etime;
	m_vVel.z += zacc*etime;

	// clamp speed
	if(D3DXVec3LengthSq(&m_vVel)>(m_fMoveSpeed*m_fMoveSpeed))
	{
		D3DXVec3Normalize(&m_vVel, &m_vVel);
		m_vVel.x *= m_fMoveSpeed;
		m_vVel.z *= m_fMoveSpeed;
	}

	// calc new position
	sinry = (float)sin(m_pObj->m_vRotation.y);
	cosry = (float)cos(m_pObj->m_vRotation.y);
	xvel = m_vVel.x*cosry + m_vVel.z*sinry;
	zvel = m_vVel.z*cosry - m_vVel.x*sinry;
	m_pObj->m_vPosition.x += xvel*etime;
	m_pObj->m_vPosition.y = m_fHeight;
	m_pObj->m_vPosition.z += zvel*etime;

	// get new y rotation
	if(!(m_dwFlags&TANK_REMOTE))
	{
		rvel = gamepad->fX2;
		if(rvel>=0.0f)
			rvel *= rvel*m_fRotSpeed;
		else
			rvel = -(rvel*rvel)*m_fRotSpeed;

		if((rvel<0.1f) && (rvel>-0.1f))
			m_fRotVel *= 0.8f;
		else
			m_fRotVel = rvel;
	}
	m_pObj->m_vRotation.y += m_fRotVel*etime;

	// keep yrot in 0-TWOPI range
	if(m_pObj->m_vRotation.y>(D3DX_PI*2.0f))
		m_pObj->m_vRotation.y -= (D3DX_PI*2.0f);
	if(m_pObj->m_vRotation.y<0.0f)
		m_pObj->m_vRotation.y += (D3DX_PI*2.0f);

	// calc new matrix
	D3DXMatrixRotationY(m, m_pObj->m_vRotation.y);

	// keep in bounds
	CollCheck(&m_pObj->m_vPosition, m_fWidth/2.0f);

	// do weapon processing on local tank
	if(!(m_dwFlags&TANK_REMOTE))
	{
		// check for weapon change
		if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			m_dwCurWeapon++;
			if(m_dwCurWeapon==MAX_WEAPONS)
				m_dwCurWeapon = 0;
		}

		// check for weapon fire
		if(gamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			if(m_fAmmo[m_dwCurWeapon]>0.0f)
			{
				// place starting position of projectile at gun
				pos = m_pObj->m_vPosition;
				pos.x += m_pObj->m_matOrientation._31*2.0f;
				pos.z += m_pObj->m_matOrientation._33*2.0f;
				pos.x += m_pObj->m_matOrientation._11*0.475f;
				pos.z += m_pObj->m_matOrientation._13*0.475f;

				switch(m_dwCurWeapon)
				{
					case WEAPON_GUN:
					case WEAPON_ROCKET:
						if((m_fLastShotTime+Weapons[m_dwCurWeapon].m_fFireRate)<XBUtil_Timer(TIMER_GETABSOLUTETIME))
						{
							m_fAmmo[m_dwCurWeapon]--;
							m_fLastShotTime = XBUtil_Timer(TIMER_GETABSOLUTETIME);
							CXObject *obj;
							obj = Weapon_Fire(&pos, m_pObj->m_vRotation.y, (BYTE)m_dwCurWeapon, 0);
							obj->m_pvInfo = (void *)g_dpnidLocalPlayer;		// tell who fired
						}
						break;

					// add additional weapons here
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Hud
// Desc: Show the tanks main heads up display.
//-----------------------------------------------------------------------------
void CXTank::Hud(CXBFont *font)
{
	WCHAR s[80];
	DWORD color;

	swprintf(s, L"WEAPON: %s", Weapons[m_dwCurWeapon].m_wstrDesc);
    font->DrawText(64, 80, 0xffffffff, s);
	swprintf(s, L"AMMO: %3.1f", m_fAmmo[m_dwCurWeapon]);
    font->DrawText(64, 100, 0xffffffff, s);

	color = 0xff00ff00;
	if(m_fArmor<60.0f)
		color = 0xffffff00;
	if(m_fArmor<30.0f)
		color = 0xffff0000;
	swprintf(s, L"ARMOR: %3.0f", m_fArmor);
    font->DrawText(64, 120, color, s);

	color = 0xff00ff00;
	if(m_fLife<60.0f)
		color = 0xffffff00;
	if(m_fLife<30.0f)
		color = 0xffff0000;
	swprintf(s, L"LIFE: %3.0f", m_fLife);
    font->DrawText(64, 140, color, s);
}

//-----------------------------------------------------------------------------
// Weapon Code
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: Weapon_Init
// Desc: Initialize the weapons subsystem
//-----------------------------------------------------------------------------
void Weapon_Init()
{
	Weapon_GunModel.Sphere(0.1f, 8, 8, FVF_XYZNORMTEX1, "textures\\bullet.bmp", 1.0f);
}


//-----------------------------------------------------------------------------
// Name: Weapon_Fire
// Desc: Fire a weapon
//-----------------------------------------------------------------------------
CXObject *Weapon_Fire(D3DXVECTOR3 *pPos, float fYRot, BYTE bType, DWORD dwFlags)
{
	CXObject *obj;

	if(g_dwNumWeaponObjs==MAX_WEAPONOBJS)
	{
		OUTPUT_DEBUG_STRING( "MAX_WEAPONOBJS exceeded in Weapon_Fire" );
		return NULL;
	}

	obj = &WeaponObjs[g_dwNumWeaponObjs];

	// set position so it is outside of firing tanks radius
	obj->m_vPosition = *pPos;
	obj->m_vRotation.y = fYRot;
	D3DXMatrixRotationY(&obj->m_matOrientation, fYRot);
	obj->m_dwType = (DWORD)bType;
	obj->SetModel(&Weapon_GunModel);

	if(!(dwFlags&WF_REMOTE))
		if(XDPIsConnected())
			Net_SendFireWeapon(obj, bType);

	g_dwNumWeaponObjs++;

	return obj;
}


//-----------------------------------------------------------------------------
// Name: Weapon_Update
// Desc: Update all weapon objects
//-----------------------------------------------------------------------------
void Weapon_Update(float fElapsedTime)
{
	DWORD i;
	CXObject *obj, *collobj;
	CXTank *tank;
	float speed;

	for(i=0; i<g_dwNumWeaponObjs; i++)
	{
next:
		obj = &WeaponObjs[i];
		speed = Weapons[obj->m_dwType].m_fSpeed * fElapsedTime;

		obj->m_vPosition.x += obj->m_matOrientation._31*speed;
		obj->m_vPosition.y += obj->m_matOrientation._32*speed;
		obj->m_vPosition.z += obj->m_matOrientation._33*speed;

		obj->m_matOrientation._41 = obj->m_vPosition.x;
		obj->m_matOrientation._42 = obj->m_vPosition.y;
		obj->m_matOrientation._43 = obj->m_vPosition.z;

		collobj = CollCheck(&obj->m_vPosition, 1.0f);

		if(collobj)
		{
			// register damage on object we hit
			if(collobj!=COLLOBJ_ARENA)
			{
				// only damage other tanks
				if(collobj->m_dwFlags&OBJ_TANK)
				{
					tank = (CXTank *)collobj->m_pvInfo;

					// only do damage processing if this tank is
					// not already dead (for counting death)
					if(tank->m_fLife>0.0f)
					{
						tank->m_fArmor -= Weapons[obj->m_dwType].m_fDamage;
						if(tank->m_fArmor<0)
						{
							tank->m_fLife += tank->m_fArmor;
							tank->m_fArmor = 0;

							if(tank->m_fLife<=0.0f && !(tank->m_dwFlags&TANK_REMOTE))
							{
								// send a humiliating message that we were killed
								DPNID dpnidKiller;
								dpnidKiller = (DPNID)obj->m_pvInfo;
								Net_SendKill(dpnidKiller);
							}
						}
					}
				}
			}

			XAPart->Burst(&obj->m_vPosition, 100);

			// got a collision, delete this object
			obj->SetModel(NULL);

			g_dwNumWeaponObjs--;

			// move last object into deleted objects spot
			WeaponObjs[i] = WeaponObjs[g_dwNumWeaponObjs];

			// clear out the last object (so we dont mess up refcounts)
			memset(&WeaponObjs[g_dwNumWeaponObjs], 0, sizeof(CXObject));
			if(g_dwNumWeaponObjs>i)
				goto next;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: Weapon_Render
// Desc: Render all the weapon objects
//-----------------------------------------------------------------------------
void Weapon_Render()
{
	DWORD i;

	for(i=0; i<g_dwNumWeaponObjs; i++)
		WeaponObjs[i].Render(OBJ_NOMCALCS);
}
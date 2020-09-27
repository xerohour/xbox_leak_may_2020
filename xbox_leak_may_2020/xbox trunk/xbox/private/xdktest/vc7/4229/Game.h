#ifndef __GAME_H
#define __GAME_H

#undef X
#undef Y
#undef Z
#undef W
#define X m_X
#define Y m_Y
#define Z m_Z
#define W m_W
#ifdef CAR_CONSTRUCTOR
#include <windows.h>
#include <winbase.h>
#include <stdarg.h>
#include <stdio.h>
#include <d3d8.h>
#include <XboxTypes.h>
#include <Xbox3D8.h>
#else
#include <xtl.h>
#include <stdio.h>
#endif
#undef X
#undef Y
#undef Z
#undef W
#define X 0
#define Y 1
#define Z 2
#define W 3

#ifdef CAR_CONSTRUCTOR
#define D3DXBOX(n) XBOX3D##n
#define IDirect3DXbox(n) IXbox3D##n
#else
#define D3DXBOX(n) D3D##n
#define IDirect3DXbox(n) IDirect3D##n
#endif

void PrintCustomFault(const char *s,...);
#undef printf
#define printf myprintf
//int myprintf(const char *, ...);
void myprintf(const char *fmt,...);

extern float g_deltaTime,g_oneOverDeltaTime;
extern float g_displayFrequency;


// this is globally useful... determine the number of entries in
// a static array (ie. "short zot[] = { ... };").
// note this does NOT work on pointers, only actual arrays!
#define entries(array) (sizeof(array)-sizeof((array)[0]))


#define MAX_ONSCREEN_PLAYERS 4
#define MAX_CAR_SLOTS 8
#define MAX_RACE_LAPS 30

#ifndef __TYPES_H
#include "types.h"
#endif

#ifndef __FADE_H
#include "fade.h"
#endif

struct Game
{
	static Fade   m_fade;
	static bool   m_exiting;
	static u32    m_frame;
	static bool   m_allHumanCarsFinished;
	static int    m_over;
	
	static void (*m_nextElapseFunc)(void);
	static void   m_SetNextElapseFunc(void (*l)(void));
	
	static void   m_StartExit(void);
//	static void   m_DrawCallback(void)
	
	static void   m_Init(void);
	
	static void   m_DoFrame(bool timeHasElapsed);
	static void   m_DoAI(void);
	
	static void   m_Draw(void);
	static void   m_Elapse(void);
	
	static bool   m_ReadoutToggleOK(void);
	static bool   m_CameraToggleOK(void);
	static bool   m_SpecialInputHandling(void);
	static bool   m_PrematureExitPossible(void);
	static bool   m_IntroTimedOut(void);

	static void   m_CheckGeneralDebugInput(void);
	static void   m_CheckPrematureExit(void);

	static void   m_RunMaterialAnims(float dt);
	static void   m_RunSkinAnims(float dt);
	static void   m_RunPaletteCycles(float dt);
	static void   m_ApplyFade(void);
	static void   m_MultiFrameUpkeep(float dt);

	static void   m_OneFrameUpkeep(void);
	static void   m_ZeroFramesUpkeep(void);

	static bool   m_AllHumanCarsFinished(void);
	static void   m_CheckExiting(void);
	static void   m_GetResults(void);

	static void   m_HideInputIfExiting(void);

#if !FINAL_ROM
	static void   m_CarAndTrackSwitcher(void);
#else
	static inline void m_CarAndTrackSwitcher(void) {}
#endif
};

// temporary hack
struct Interface : public Game
{
};

struct Car;
extern Car *g_inputCar[MAX_ONSCREEN_PLAYERS];

#endif

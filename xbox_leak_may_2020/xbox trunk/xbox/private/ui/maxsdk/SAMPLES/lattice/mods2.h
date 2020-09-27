#ifndef __MODS2__H
#define __MODS2__H

#include "Max.h"
#include "lattice.h"
#include "iparamm.h"

#define FLOAT_EPSILON 1.0e-06

TCHAR *GetString(int id);

extern ClassDesc* GetLatticeModDesc(); 
extern ClassDesc* GetSplineSelModDesc();
extern ClassDesc* GetRandMatlModDesc();
extern ClassDesc* GetSpindleModDesc();

extern int FloatEQ0(float number);

extern HINSTANCE hInstance;
extern BOOL CALLBACK DefaultSOTProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
#endif

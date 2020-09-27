#ifndef __MAIN_H
#define __MAIN_H

extern D3DXVECTOR4 g_vEyePos;

extern D3DXMATRIX g_mProj;							
extern D3DXMATRIX g_mView;							

extern D3DLIGHT8 g_Light0;
extern D3DXVECTOR3 g_vLightPos;						

extern DWORD g_vsLafortune;
extern D3DXVECTOR4 g_vLafConst[4];
extern BOOL g_bLafActive;

#endif
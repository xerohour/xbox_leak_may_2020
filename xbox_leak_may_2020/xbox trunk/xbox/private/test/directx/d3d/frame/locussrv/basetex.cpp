#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock.h>
#include <d3dx8.h>
#include "transprt.h"
#include "util.h"
#include "typetran.h"
#include "server.h"

//***********************************************************************************
extern "C" void __cdecl M_BTX8_GetLevelCount(LPDIRECT3DBASETEXTURE8 pd3dtb, DWORD* pdwCount) {

    *pdwCount = pd3dtb->GetLevelCount();
}

//***********************************************************************************
extern "C" void __cdecl M_BTX8_GetLOD(LPDIRECT3DBASETEXTURE8 pd3dtb, DWORD* pdwLOD) {

    *pdwLOD = pd3dtb->GetLOD();
}

//***********************************************************************************
extern "C" void __cdecl M_BTX8_SetLOD(LPDIRECT3DBASETEXTURE8 pd3dtb, DWORD* pdwLOD, DWORD dwNewLOD) {

    *pdwLOD = pd3dtb->SetLOD(dwNewLOD);
}

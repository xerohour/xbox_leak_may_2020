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
extern "C" void __cdecl M_RES8_AddRef(LPDIRECT3DRESOURCE8 pd3dres, ULONG* pulRet) {

    *pulRet = pd3dres->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_Release(LPDIRECT3DRESOURCE8 pd3dres, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3dres);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_GetType(LPDIRECT3DRESOURCE8 pd3dres, D3DRESOURCETYPE* pd3drt) {

    *pd3drt = pd3dres->GetType();
    REMAPOUT(D3DTI_D3DRESOURCETYPE, *pd3drt);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_GetDevice(LPDIRECT3DRESOURCE8 pd3dres, HRESULT* phr, LPDIRECT3DDEVICE8* ppDevice) {

    *phr = pd3dres->GetDevice(ppDevice);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_GetPrivateData(LPDIRECT3DRESOURCE8 pd3dres, HRESULT* phr, GUID* prefguid, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3dres->GetPrivateData(*prefguid, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_SetPrivateData(LPDIRECT3DRESOURCE8 pd3dres, HRESULT* phr, GUID* prefguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    *phr = pd3dres->SetPrivateData(*prefguid, pvData, dwSizeOfData, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_FreePrivateData(LPDIRECT3DRESOURCE8 pd3dres, HRESULT* phr, GUID* prefguid) {

    *phr = pd3dres->FreePrivateData(*prefguid);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_GetPriority(LPDIRECT3DRESOURCE8 pd3dres, DWORD* pdwRet) {

    *pdwRet = pd3dres->GetPriority();
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_SetPriority(LPDIRECT3DRESOURCE8 pd3dres, DWORD* pdwRet, DWORD dwPriority) {

    *pdwRet = pd3dres->SetPriority(dwPriority);
}

//***********************************************************************************
extern "C" void __cdecl M_RES8_PreLoad(LPDIRECT3DRESOURCE8 pd3dres) {

    pd3dres->PreLoad();
}


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
extern "C" void __cdecl M_VRB8_GetDesc(LPDIRECT3DVERTEXBUFFER8 pd3dr, HRESULT* phr, D3DVERTEXBUFFER_DESC* pd3dvbd) {

    *phr = pd3dr->GetDesc(pd3dvbd);
    REMAPOUT(D3DTI_D3DFORMAT, pd3dvbd->Format);
    REMAPOUT(D3DTI_D3DRESOURCETYPE, pd3dvbd->Type);
    REMAPOUT(D3DTI_D3DPOOL, pd3dvbd->Pool);
}

//***********************************************************************************
extern "C" void __cdecl M_VRB8_Lock(LPDIRECT3DVERTEXBUFFER8 pd3dr, HRESULT* phr, UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

    *phr = pd3dr->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_VRB8_Unlock(LPDIRECT3DVERTEXBUFFER8 pd3dr, HRESULT* phr, LPBYTE pData, UINT uSize, LPBYTE pBits) {

    if (uSize) {
        memcpy(pData, pBits, uSize);
    }

    *phr = pd3dr->Unlock();
}

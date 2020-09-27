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
extern "C" void __cdecl M_IXB8_GetDesc(LPDIRECT3DINDEXBUFFER8 pd3di, HRESULT* phr, D3DINDEXBUFFER_DESC* pd3dibd) {

    *phr = pd3di->GetDesc(pd3dibd);
    REMAPOUT(D3DTI_D3DFORMAT, pd3dibd->Format);
    REMAPOUT(D3DTI_D3DRESOURCETYPE, pd3dibd->Type);
    REMAPOUT(D3DTI_D3DPOOL, pd3dibd->Pool);
}

//***********************************************************************************
extern "C" void __cdecl M_IXB8_Lock(LPDIRECT3DINDEXBUFFER8 pd3di, HRESULT* phr, UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

    *phr = pd3di->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_IXB8_Unlock(LPDIRECT3DINDEXBUFFER8 pd3di, HRESULT* phr, LPBYTE pData, UINT uSize, LPBYTE pBits) {

    if (uSize) {
        memcpy(pData, pBits, uSize);
    }

    *phr = pd3di->Unlock();
}

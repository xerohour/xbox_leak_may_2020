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
extern "C" void __cdecl M_DEV8_AddRef(LPDIRECT3DDEVICE8 pd3dd, ULONG* pulRet) {

    *pulRet = pd3dd->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_Release(LPDIRECT3DDEVICE8 pd3dd, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3dd);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateAdditionalSwapChain(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRESENT_PARAMETERS* pd3dpp, LPDIRECT3DSWAPCHAIN8* ppSwapChain) {

    g_pServer->RemapStruct(TRUE, pd3dpp);
    *phr = pd3dd->CreateAdditionalSwapChain(pd3dpp, ppSwapChain);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateCubeTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uEdgeLength, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, LPDIRECT3DCUBETEXTURE8* ppCubeTexture) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DPOOL, pool);
    if ((dwUsage & D3DUSAGE_RENDERTARGET) || (dwUsage & D3DUSAGE_DEPTHSTENCIL)) {
        pool = D3DPOOL_DEFAULT;
    }
    *phr = pd3dd->CreateCubeTexture(uEdgeLength, uLevels, dwUsage, fmt, pool, ppCubeTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateDepthStencilSurface(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, LPDIRECT3DSURFACE8* ppSurface) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DMULTISAMPLE_TYPE, mst);
    *phr = pd3dd->CreateDepthStencilSurface(uWidth, uHeight, fmt, mst, ppSurface);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateImageSurface(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uWidth, UINT uHeight, D3DFORMAT fmt, LPDIRECT3DSURFACE8* ppSurface) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    *phr = pd3dd->CreateImageSurface(uWidth, uHeight, fmt, ppSurface);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateIndexBuffer(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uLength, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, LPDIRECT3DINDEXBUFFER8* ppIndexBuffer) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DPOOL, pool);
    *phr = pd3dd->CreateIndexBuffer(uLength, dwUsage, fmt, pool, ppIndexBuffer);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateRenderTarget(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, BOOL bLockable, LPDIRECT3DSURFACE8* ppSurface) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DMULTISAMPLE_TYPE, mst);
    *phr = pd3dd->CreateRenderTarget(uWidth, uHeight, fmt, mst, bLockable, ppSurface);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uWidth, UINT uHeight, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, LPDIRECT3DTEXTURE8* ppTexture) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DPOOL, pool);
    if ((dwUsage & D3DUSAGE_RENDERTARGET) || (dwUsage & D3DUSAGE_DEPTHSTENCIL)) {
        pool = D3DPOOL_DEFAULT;
    }
    *phr = pd3dd->CreateTexture(uWidth, uHeight, uLevels, dwUsage, fmt, pool, ppTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateVertexBuffer(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uLength, DWORD dwUsage, DWORD dwFVF, D3DPOOL pool, LPDIRECT3DVERTEXBUFFER8* ppVertexBuffer) {

    REMAPIN(D3DTI_D3DPOOL, pool);
    *phr = pd3dd->CreateVertexBuffer(uLength, dwUsage, dwFVF, pool, ppVertexBuffer);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateVolumeTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uWidth, UINT uHeight, UINT uDepth, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, LPDIRECT3DVOLUMETEXTURE8* ppVolumeTexture) {

    REMAPIN(D3DTI_D3DFORMAT, fmt);
    REMAPIN(D3DTI_D3DPOOL, pool);
    *phr = pd3dd->CreateVolumeTexture(uWidth, uHeight, uDepth, uLevels, dwUsage, fmt, pool, ppVolumeTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_ApplyStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwToken) {

    *phr = pd3dd->ApplyStateBlock(dwToken);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_BeginStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr) {

    *phr = pd3dd->BeginStateBlock();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_EndStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDWORD pdwToken) {

    *phr = pd3dd->EndStateBlock(pdwToken);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DSTATEBLOCKTYPE sbt, DWORD* pdwToken) {

    REMAPIN(D3DTI_D3DSTATEBLOCKTYPE, sbt);
    *phr = pd3dd->CreateStateBlock(sbt, pdwToken);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DeleteStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwToken) {

    *phr = pd3dd->DeleteStateBlock(dwToken);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CaptureStateBlock(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwToken) {

    *phr = pd3dd->CaptureStateBlock(dwToken);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetClipStatus(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DCLIPSTATUS8* pd3dcs) {

    *phr = pd3dd->GetClipStatus(pd3dcs);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetClipStatus(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DCLIPSTATUS8* pd3dcs) {

    *phr = pd3dd->SetClipStatus(pd3dcs);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetRenderState(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DRENDERSTATETYPE rst, DWORD* pdwValue) {

    REMAPIN(D3DTI_D3DRENDERSTATETYPE, rst);
    *phr = pd3dd->GetRenderState(rst, pdwValue);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetRenderState(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DRENDERSTATETYPE rst, DWORD dwValue) {

    REMAPIN(D3DTI_D3DRENDERSTATETYPE, rst);
    switch (rst) {
        case D3DRS_ZENABLE:
            REMAPIN(D3DTI_D3DZBUFFERTYPE, dwValue);
            break;
        case D3DRS_FILLMODE:
            REMAPIN(D3DTI_D3DFILLMODE, dwValue);
            break;
        case D3DRS_SHADEMODE:
            REMAPIN(D3DTI_D3DSHADEMODE, dwValue);
            break;
        case D3DRS_SRCBLEND:
        case D3DRS_DESTBLEND:
            REMAPIN(D3DTI_D3DBLEND, dwValue);
            break;
        case D3DRS_CULLMODE:
            REMAPIN(D3DTI_D3DCULL, dwValue);
            break;
        case D3DRS_ZFUNC:
        case D3DRS_ALPHAFUNC:
        case D3DRS_STENCILFUNC:
            REMAPIN(D3DTI_D3DCMPFUNC, dwValue);
            break;
        case D3DRS_FOGTABLEMODE:
        case D3DRS_FOGVERTEXMODE:
            REMAPIN(D3DTI_D3DFOGMODE, dwValue);
            break;
        case D3DRS_STENCILFAIL:
        case D3DRS_STENCILZFAIL:
        case D3DRS_STENCILPASS:
            REMAPIN(D3DTI_D3DSTENCILOP, dwValue);
            break;
        case D3DRS_WRAP0:
        case D3DRS_WRAP1:
        case D3DRS_WRAP2:
        case D3DRS_WRAP3:
        case D3DRS_WRAP4:
        case D3DRS_WRAP5:
        case D3DRS_WRAP6:
        case D3DRS_WRAP7: {
            DWORD dwWrap = 0;
            if (dwValue & XBOX_D3DWRAPCOORD_0) {
                dwWrap |= D3DWRAPCOORD_0;
            }
            if (dwValue & XBOX_D3DWRAPCOORD_1) {
                dwWrap |= D3DWRAPCOORD_1;
            }
            if (dwValue & XBOX_D3DWRAPCOORD_2) {
                dwWrap |= D3DWRAPCOORD_2;
            }
            if (dwValue & XBOX_D3DWRAPCOORD_3) {
                dwWrap |= D3DWRAPCOORD_3;
            }
            dwValue = dwWrap;
            break;
        }
        case D3DRS_DIFFUSEMATERIALSOURCE:
        case D3DRS_AMBIENTMATERIALSOURCE:
        case D3DRS_SPECULARMATERIALSOURCE:
        case D3DRS_EMISSIVEMATERIALSOURCE:
            REMAPIN(D3DTI_D3DMATERIALCOLORSOURCE, dwValue);
            break;
        case D3DRS_VERTEXBLEND:
            REMAPIN(D3DTI_D3DVERTEXBLENDFLAGS, dwValue);
            break;
        case D3DRS_PATCHEDGESTYLE:
            REMAPIN(D3DTI_D3DPATCHEDGESTYLE, dwValue);
            break;
        case D3DRS_DEBUGMONITORTOKEN:
            REMAPIN(D3DTI_D3DDEBUGMONITORTOKENS, dwValue);
            break;
        case D3DRS_BLENDOP:
            REMAPIN(D3DTI_D3DBLENDOP, dwValue);
            break;
    }
    *phr = pd3dd->SetRenderState(rst, dwValue);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetRenderTarget(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DSURFACE8* ppRenderTarget) {

    *phr = pd3dd->GetRenderTarget(ppRenderTarget);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetRenderTarget(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DSURFACE8 pRenderTarget, LPDIRECT3DSURFACE8 pDepthStencil) {

    LPDIRECT3DSURFACE8  pd3dsRenderTarget = NULL, pd3dsDepthStencil = NULL;
    DWORD               dwKey;

    if (pRenderTarget) {
        pd3dd->GetRenderTarget(&pd3dsRenderTarget);
    }

    if (pDepthStencil) {
        pd3dd->GetDepthStencilSurface(&pd3dsDepthStencil);
    }

    *phr = pd3dd->SetRenderTarget(pRenderTarget, pDepthStencil);

    // Check if the old render target and/or depth buffer were released when the new targets were set.
    // If they were released, remove them from the address map
    if (pd3dsRenderTarget) {
        if (pd3dsRenderTarget->Release() == 0) {
            if (g_pServer->FindMapping(__ADDRESS, &dwKey, (DWORD)pd3dsRenderTarget)) {
                g_pServer->RemoveMapping(__ADDRESS, dwKey);
            }
        }
    }

    if (pd3dsDepthStencil) {
        if (pd3dsDepthStencil->Release() == 0) {
            if (g_pServer->FindMapping(__ADDRESS, &dwKey, (DWORD)pd3dsDepthStencil)) {
                g_pServer->RemoveMapping(__ADDRESS, dwKey);
            }
        }
    }
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetTransform(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    REMAPIN(D3DTI_D3DTRANSFORMSTATETYPE, tst);
    *phr = pd3dd->GetTransform(tst, pm);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetTransform(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    REMAPIN(D3DTI_D3DTRANSFORMSTATETYPE, tst);
    *phr = pd3dd->SetTransform(tst, pm);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_MultiplyTransform(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    REMAPIN(D3DTI_D3DTRANSFORMSTATETYPE, tst);
    *phr = pd3dd->MultiplyTransform(tst, pm);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_Clear(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwCount, D3DRECT* prc, DWORD dwFlags, D3DCOLOR c, DWORD fZ, DWORD dwStencil) {

    DWORD dwWinxFlags = 0;

    if (dwFlags & XBOX_D3DCLEAR_TARGET) {
        dwWinxFlags |= D3DCLEAR_TARGET;
    }
    if (dwFlags & XBOX_D3DCLEAR_ZBUFFER) {
        dwWinxFlags |= D3DCLEAR_ZBUFFER;
    }
    if (dwFlags & XBOX_D3DCLEAR_STENCIL) {
        dwWinxFlags |= D3DCLEAR_STENCIL;
    }

    if (prc->x1 == -1) {
        prc = NULL;
    }
    *phr = pd3dd->Clear(dwCount, prc, dwWinxFlags, c, DW2F(fZ), dwStencil);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetViewport(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DVIEWPORT8* pviewport) {

    *phr = pd3dd->GetViewport(pviewport);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetViewport(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DVIEWPORT8* pviewport) {

    *phr = pd3dd->SetViewport(pviewport);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetClipPlane(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, float* pfPlane) {

    *phr = pd3dd->GetClipPlane(dwIndex, pfPlane);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetClipPlane(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, float* pfPlane) {

    *phr = pd3dd->SetClipPlane(dwIndex, pfPlane);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetLight(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, D3DLIGHT8* plight) {

    *phr = pd3dd->GetLight(dwIndex, plight);
    REMAPOUT(D3DTI_D3DLIGHTTYPE, plight->Type);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetLight(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, D3DLIGHT8* plight) {

    REMAPIN(D3DTI_D3DLIGHTTYPE, plight->Type);
    *phr = pd3dd->SetLight(dwIndex, plight);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_LightEnable(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, BOOL bEnable) {

    *phr = pd3dd->LightEnable(dwIndex, bEnable);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetLightEnable(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwIndex, BOOL* pbEnable) {

    *phr = pd3dd->GetLightEnable(dwIndex, pbEnable);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetMaterial(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DMATERIAL8* pmaterial) {

    *phr = pd3dd->GetMaterial(pmaterial);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetMaterial(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DMATERIAL8* pmaterial) {

    *phr = pd3dd->SetMaterial(pmaterial);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwStage, LPDIRECT3DBASETEXTURE8* ppTexture) {

    *phr = pd3dd->GetTexture(dwStage, ppTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture) {

    *phr = pd3dd->SetTexture(dwStage, pTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetTextureStageState(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD* pdwValue) {

    REMAPIN(D3DTI_D3DTEXTURESTAGESTATETYPE, txsst);
    *phr = pd3dd->GetTextureStageState(dwStage, txsst, pdwValue);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetTextureStageState(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD dwValue) {

    REMAPIN(D3DTI_D3DTEXTURESTAGESTATETYPE, txsst);
    switch (txsst) {
        case D3DTSS_COLOROP:
        case D3DTSS_ALPHAOP:
            REMAPIN(D3DTI_D3DTEXTUREOP, dwValue);
            break;
        case D3DTSS_ADDRESSU:
        case D3DTSS_ADDRESSV:
        case D3DTSS_ADDRESSW:
            REMAPIN(D3DTI_D3DTEXTUREADDRESS, dwValue);
            break;
        case D3DTSS_MINFILTER:
        case D3DTSS_MAGFILTER:
        case D3DTSS_MIPFILTER:
            REMAPIN(D3DTI_D3DTEXTUREFILTERTYPE, dwValue);
            break;
        case D3DTSS_TEXTURETRANSFORMFLAGS:
            REMAPIN(D3DTI_D3DTEXTURETRANSFORMFLAGS, dwValue);
            break;
    }

    *phr = pd3dd->SetTextureStageState(dwStage, txsst, dwValue);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_UpdateTexture(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DBASETEXTURE8 pSourceTexture, LPDIRECT3DBASETEXTURE8 pDestinationTexture) {

    *phr = pd3dd->UpdateTexture(pSourceTexture, pDestinationTexture);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_ValidateDevice(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD* pdwNumPasses) {

    *phr = pd3dd->ValidateDevice(pdwNumPasses);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetCurrentTexturePalette(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT* puPaletteNumber) {

    *phr = pd3dd->GetCurrentTexturePalette(puPaletteNumber);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetCurrentTexturePalette(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uPaletteNumber) {

    *phr = pd3dd->SetCurrentTexturePalette(uPaletteNumber);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetPaletteEntries(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uPaletteNumber, PALETTEENTRY* ppe) {

    *phr = pd3dd->GetPaletteEntries(uPaletteNumber, ppe);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetPaletteEntries(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uPaletteNumber, PALETTEENTRY* ppe) {

    *phr = pd3dd->SetPaletteEntries(uPaletteNumber, ppe);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreateVertexShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage) {

    *phr = pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DeleteVertexShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle) {

    *phr = pd3dd->DeleteVertexShader(dwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetVertexShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD* pdwHandle) {

    *phr = pd3dd->GetVertexShader(pdwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetVertexShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle) {

    *phr = pd3dd->SetVertexShader(dwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetVertexShaderConstant(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    *phr = pd3dd->GetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetVertexShaderConstant(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    *phr = pd3dd->SetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetVertexShaderDeclaration(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3dd->GetVertexShaderDeclaration(dwHandle, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetVertexShaderFunction(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3dd->GetVertexShaderFunction(dwHandle, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CreatePixelShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD* pdwFunction, DWORD* pdwHandle) {

    *phr = pd3dd->CreatePixelShader(pdwFunction, pdwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DeletePixelShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle) {

    *phr = pd3dd->DeletePixelShader(dwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetPixelShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD* pdwHandle) {

    *phr = pd3dd->GetPixelShader(pdwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetPixelShader(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle) {

    *phr = pd3dd->SetPixelShader(dwHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetPixelShaderConstant(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    *phr = pd3dd->GetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetPixelShaderConstant(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    *phr = pd3dd->SetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetPixelShaderFunction(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3dd->GetPixelShaderFunction(dwHandle, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_Present(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, RECT* prectSrc, RECT* prectDst, HWND hDestWindowOverride, RGNDATA* prgnDirtyRegion) {

    if (prectSrc->left == -1) {
        prectSrc = NULL;
    }
    if (prectDst->left == -1) {
        prectDst = NULL;
    }
    if (prgnDirtyRegion->rdh.nCount == 0) {
        prgnDirtyRegion = NULL;
    }
    *phr = pd3dd->Present(prectSrc, prectDst, hDestWindowOverride, prgnDirtyRegion);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_Reset(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRESENT_PARAMETERS* pd3dpp) {

    g_pServer->RemapStruct(TRUE, pd3dpp);
    *phr = pd3dd->Reset(pd3dpp);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_BeginScene(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr) {

    *phr = pd3dd->BeginScene();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_EndScene(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr) {

    *phr = pd3dd->EndScene();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawIndexedPrimitive(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uStartIndex, UINT uPrimitiveCount) {

    REMAPIN(D3DTI_D3DPRIMITIVETYPE, d3dpt);
    *phr = pd3dd->DrawIndexedPrimitive(d3dpt, uMinVertexToShade, uNumVerticesToShade, uStartIndex, uPrimitiveCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uPrimitiveCount, void* pvIndices, D3DFORMAT fmtIndex, void* pvVertices, UINT uStride) {

    REMAPIN(D3DTI_D3DPRIMITIVETYPE, d3dpt);
    *phr = pd3dd->DrawIndexedPrimitiveUP(d3dpt, uMinVertexToShade, uNumVerticesToShade, uPrimitiveCount, pvIndices, fmtIndex, pvVertices, uStride);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawPrimitive(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uPrimitiveCount) {

    REMAPIN(D3DTI_D3DPRIMITIVETYPE, d3dpt);
    *phr = pd3dd->DrawPrimitive(d3dpt, uStartVertex, uPrimitiveCount);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawPrimitiveUP(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DPRIMITIVETYPE d3dpt, UINT uPrimitiveCount, void* pvVertices, UINT uStride) {

    REMAPIN(D3DTI_D3DPRIMITIVETYPE, d3dpt);
    *phr = pd3dd->DrawPrimitiveUP(d3dpt, uPrimitiveCount, pvVertices, uStride);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DeletePatch(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uHandle) {

    *phr = pd3dd->DeletePatch(uHandle);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawRectPatch(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uHandle, float* pfNumSegs, D3DRECTPATCH_INFO* prpi) {

    REMAPIN(D3DTI_D3DBASISTYPE, prpi->Basis);
    REMAPIN(D3DTI_D3DORDERTYPE, prpi->Order);
    *phr = pd3dd->DrawRectPatch(uHandle, pfNumSegs, prpi);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_DrawTriPatch(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uHandle, float* pfNumSegs, D3DTRIPATCH_INFO* ptsi) {

    REMAPIN(D3DTI_D3DBASISTYPE, ptsi->Basis);
    REMAPIN(D3DTI_D3DORDERTYPE, ptsi->Order);
    *phr = pd3dd->DrawTriPatch(uHandle, pfNumSegs, ptsi);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetStreamSource(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uStream, LPDIRECT3DVERTEXBUFFER8* ppVertexData, UINT* puStride) {

    *phr = pd3dd->GetStreamSource(uStream, ppVertexData, puStride);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetStreamSource(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uStream, LPDIRECT3DVERTEXBUFFER8 pVertexData, UINT uStride) {

    *phr = pd3dd->SetStreamSource(uStream, pVertexData, uStride);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetIndices(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DINDEXBUFFER8* ppIndexData, UINT* puBaseIndex) {

    *phr = pd3dd->GetIndices(ppIndexData, puBaseIndex);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetIndices(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DINDEXBUFFER8 pIndexData, UINT uBaseIndex) {

    *phr = pd3dd->SetIndices(pIndexData, uBaseIndex);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetAvailableTextureMem(LPDIRECT3DDEVICE8 pd3dd, UINT* puAvail) {

    *puAvail = pd3dd->GetAvailableTextureMem();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetCreationParameters(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DDEVICE_CREATION_PARAMETERS* pd3dcp) {

    *phr = pd3dd->GetCreationParameters(pd3dcp);
    REMAPOUT(D3DTI_D3DDEVTYPE, pd3dcp->DeviceType);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetDeviceCaps(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DCAPS8* pd3dcaps) {

    *phr = pd3dd->GetDeviceCaps(pd3dcaps);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetDirect3D(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3D8* ppDirect3D) {

    *phr = pd3dd->GetDirect3D(ppDirect3D);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetInfo(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwDevInfoType, void* pvDevInfo, DWORD dwDevInfoSize) {

    *phr = pd3dd->GetInfo(dwDevInfoType, pvDevInfo, dwDevInfoSize);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetRasterStatus(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DRASTER_STATUS* prast) {

    *phr = pd3dd->GetRasterStatus(prast);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetDisplayMode(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, D3DDISPLAYMODE* pMode) {

    *phr = pd3dd->GetDisplayMode(pMode);
    REMAPOUT(D3DTI_D3DFORMAT, pMode->Format);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetBackBuffer(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, LPDIRECT3DSURFACE8* ppBackBuffer) {

    REMAPIN(D3DTI_D3DBACKBUFFER_TYPE, bbt);
    *phr = pd3dd->GetBackBuffer(uBackBuffer, bbt, ppBackBuffer);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetDepthStencilSurface(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DSURFACE8* ppSurface) {

    *phr = pd3dd->GetDepthStencilSurface(ppSurface);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetGammaRamp(LPDIRECT3DDEVICE8 pd3dd, D3DGAMMARAMP* pgrRamp) {

    pd3dd->GetGammaRamp(pgrRamp);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetGammaRamp(LPDIRECT3DDEVICE8 pd3dd, DWORD dwFlags, D3DGAMMARAMP* pgrRamp) {

    pd3dd->SetGammaRamp(dwFlags, pgrRamp);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_CopyRects(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DSURFACE8 pSrcSurface, RECT* prectSrcRects, UINT uNumSrcRects, LPDIRECT3DSURFACE8 pDstSurface, POINT* ppntDstPoints) {

    *phr = pd3dd->CopyRects(pSrcSurface, prectSrcRects, uNumSrcRects, pDstSurface, ppntDstPoints);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_GetFrontBuffer(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, LPDIRECT3DSURFACE8 pSurface) {

    *phr = pd3dd->GetFrontBuffer(pSurface);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_ProcessVertices(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uStartVertexSrc, UINT uStartVertexDst, UINT uNumVertices, LPDIRECT3DVERTEXBUFFER8 pDstBuffer, DWORD dwFlags) {

    *phr = pd3dd->ProcessVertices(uStartVertexSrc, uStartVertexDst, uNumVertices, pDstBuffer, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_ResourceManagerDiscardBytes(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, DWORD dwNumBytes) {

    *phr = pd3dd->ResourceManagerDiscardBytes(dwNumBytes);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_TestCooperativeLevel(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr) {

    *phr = pd3dd->TestCooperativeLevel();
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetCursorPosition(LPDIRECT3DDEVICE8 pd3dd, UINT uSX, UINT uSY, DWORD dwFlags) {

    pd3dd->SetCursorPosition(uSX, uSY, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_SetCursorProperties(LPDIRECT3DDEVICE8 pd3dd, HRESULT* phr, UINT uHotSpotX, UINT uHotSpotY, LPDIRECT3DSURFACE8 pCursorBitmap) {

    *phr = pd3dd->SetCursorProperties(uHotSpotX, uHotSpotY, pCursorBitmap);
}

//***********************************************************************************
extern "C" void __cdecl M_DEV8_ShowCursor(LPDIRECT3DDEVICE8 pd3dd, BOOL* pbRet, BOOL bShow) {

    *pbRet = pd3dd->ShowCursor(bShow);
}


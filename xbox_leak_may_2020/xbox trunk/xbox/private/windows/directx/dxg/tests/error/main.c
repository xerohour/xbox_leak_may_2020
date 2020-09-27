#include <xtl.h>

#pragma pack(1)

typedef struct _IMAGE_RUN1 {
    UINT fOne : 1;
    UINT Size : 3;
    UINT Intensity : 4;
} IMAGE_RUN1;

typedef struct _IMAGE_RUN2 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 10;
    UINT Intensity : 4;
} IMAGE_RUN2;

typedef struct _IMAGE_RUN3 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 18;
    UINT Intensity : 4;
} IMAGE_RUN3;

#pragma pack()

#include "image.h"

const struct TheVerts { float x,y,z,w; float u, v; } Verts[] =
{
    {  0.0f,   0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
    {640.0f, 480.0f, 0.5f, 1.0f, 320.0f, 240.0f},
    {  0.0f, 480.0f, 0.5f, 1.0f,   0.0f, 240.0f},
    {640.0f,   0.0f, 0.5f, 1.0f, 320.0f,   0.0f},
    {640.0f, 480.0f, 0.5f, 1.0f, 320.0f, 240.0f},
    {  0.0f,   0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
};

IDirect3DDevice8* InitD3D(void)
{
    D3DPRESENT_PARAMETERS d3dpp;
    IDirect3DDevice8 *pDev;
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    if (pD3D == NULL)
    {
        return NULL;
    }

    pDev = NULL;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz      = 60;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    IDirect3D8_CreateDevice(pD3D,
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            NULL,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &pDev);

    IDirect3D8_Release(pD3D);

    return pDev;
}

BOOL InitVB(IDirect3DDevice8 *pDev)
{
    IDirect3DVertexBuffer8 *pVB;
    void *pVerts;

    if (FAILED(IDirect3DDevice8_CreateVertexBuffer(pDev,
            sizeof(Verts),
            D3DUSAGE_WRITEONLY,
            D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
            D3DPOOL_MANAGED,
            &pVB)))
    {
        return FALSE;
    }

    IDirect3DVertexBuffer8_Lock(pVB, 0, sizeof(Verts), (BYTE **)(&pVerts), 0);
    memcpy((void*)pVerts, (void*)Verts, sizeof(Verts));
    IDirect3DVertexBuffer8_Unlock(pVB);

    IDirect3DDevice8_SetStreamSource(pDev, 0, pVB, sizeof(Verts[0]));
    IDirect3DDevice8_SetVertexShader(pDev, D3DFVF_XYZRHW | D3DFVF_TEX1);

    return TRUE;
}

COLORREF ColorFromIntensity(UINT intensity4bits)
{
    UINT BaseR = 0x00;
    UINT BaseG = 0xFF;
    UINT BaseB = 0x00;

    UINT r = (UINT)((BaseR * intensity4bits) / 15.0f + 0.5f);
    UINT g = (UINT)((BaseG * intensity4bits) / 15.0f + 0.5f);
    UINT b = (UINT)((BaseB * intensity4bits) / 15.0f + 0.5f);

    return D3DCOLOR_ARGB(0xFF, r, g, b);
}

BOOL InitTexture(IDirect3DDevice8 *pDev)
{
    IDirect3DTexture8* pTex;
    D3DLOCKED_RECT lock;
    UINT x = 0, y = 0, i, j;
    DWORD dwLine;
    DWORD dwAddr;
    IMAGE_RUN1 *pRun1;
    IMAGE_RUN2 *pRun2;
    IMAGE_RUN3 *pRun3;
    UINT Size;
    UINT Intensity;

    if (FAILED(IDirect3DDevice8_CreateTexture(pDev,
            320,
            240,
            1,
            0,
            D3DFMT_LIN_X8R8G8B8,
            0,
            &pTex)))
    {
        return FALSE;
    }

    IDirect3DTexture8_LockRect(pTex, 0, &lock, NULL, 0);
    dwLine = (DWORD)lock.pBits;
    dwAddr = dwLine;
    i = 0;

    while(i < sizeof(g_Image))
    {
        pRun1 = (IMAGE_RUN1*)&g_Image[i];
        if (pRun1->fOne)
        {
            Size = pRun1->Size;
            Intensity = pRun1->Intensity;
            i += 1;
        }
        else
        {
            pRun2 = (IMAGE_RUN2*)&g_Image[i];
            if (pRun2->fTwo)
            {
                Size = pRun2->Size;
                Intensity = pRun2->Intensity;
                i += 2;
            }
            else
            {
                pRun3 = (IMAGE_RUN3*)&g_Image[i];
                Size = pRun3->Size;
                Intensity = pRun3->Intensity;
                i += 3;
            }
        }

        for (j = 0; j < Size; j++)
        {
            if (Intensity == 0)
            {
                *(DWORD*)dwAddr = 0xFF000000;
            }
            else
            {
                *(DWORD*)dwAddr = ColorFromIntensity(Intensity);
            }
            if (x >= 320)
            {
                x = 0;
                y++;
                dwLine += lock.Pitch;
                dwAddr = dwLine;
            }
            else
            {
                dwAddr += sizeof(DWORD);
            }
        }
    }

    IDirect3DTexture8_UnlockRect(pTex, 0);

    IDirect3DDevice8_SetTexture(pDev, 0, (D3DBaseTexture*)pTex);

    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    return TRUE;
}

void __cdecl main(void)
{
    IDirect3DDevice8 *pDev;

//    __asm int 3;

    if ((pDev = InitD3D()) == NULL)
    {
        return;
    }

    if (!InitVB(pDev) || !InitTexture(pDev))
    {
        IDirect3DDevice8_Release(pDev);
        return;
    }

    IDirect3DDevice8_SetRenderState(pDev, D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    IDirect3DDevice8_SetRenderState(pDev, D3DRS_DESTBLEND, D3DBLEND_ONE);

    IDirect3DDevice8_SetRenderState(pDev, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice8_SetRenderState(pDev, D3DRS_ALPHABLENDENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(pDev, D3DRS_ALPHATESTENABLE, FALSE);

    while (1)
    {
        IDirect3DDevice8_BeginScene(pDev);

        IDirect3DDevice8_DrawPrimitive(pDev,
                D3DPT_TRIANGLELIST,
                0,
                (sizeof(Verts) / sizeof(Verts[0])) / 3);

        IDirect3DDevice8_EndScene(pDev);

        IDirect3DDevice8_Present(pDev, NULL, NULL, NULL, NULL);
    }
}


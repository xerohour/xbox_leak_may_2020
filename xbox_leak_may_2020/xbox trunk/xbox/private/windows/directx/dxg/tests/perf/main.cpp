/*--Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/

extern "C"
{
    #include <ntos.h>
    #include <nturtl.h>
    #include <ani.h>
};

#include <SDKCommon.h>
#include <xgraphics.h>
#include <xmetal.h>

D3DDevice *pDevice;

extern DWORD PushBufferSize;
extern DWORD Repeat;

void Setup();
void Draw();
void Verify();

typedef enum
{
    SUBCH_UNUSED0            = 0,
    SUBCH_UNUSED1            = 1,
    SUBCH_UNUSED2            = 2,
    SUBCH_UNUSED3            = 3,
    SUBCH_UNUSED4            = 4,
    SUBCH_SW                 = 5,   // NVX_SOFTWARE_CLASS_HANDLE
    SUBCH_3D                 = 6,   // D3D_KELVIN_PRIMITIVE (0x097)
    SUBCH_MEMCOPY            = 7,   // D3D_MEMORY_TO_MEMORY_COPY (0x039)

} SubChannel;

#define PUSHER_METHOD(subch, method, count) \
            (((count) << 18) + ((subch) << 13) + (method))

extern "C" { extern volatile DWORD *D3D__GpuGet; }
extern "C" { extern volatile DWORD *D3D__GpuPut; }
extern "C" { extern volatile DWORD *D3D__GpuReg; }
extern "C" { extern volatile DWORD *D3D__GpuTime; }
extern "C" { extern DWORD *D3D__PushBase; }
extern "C" { extern DWORD *D3D__PushLimit; }
extern "C" { extern ULONGLONG D3D__TestTime; }

__forceinline ULONGLONG GetTime()
{
    _asm rdtsc
}


VOID FlushWCCache()
{
    DWORD* pPut = g_pPushBuffer->m_pPut;
    DWORD* pPushBase = D3D__PushBase;
    DWORD* pPushMiddle = D3D__PushBase + (D3D__PushLimit - D3D__PushBase) / 2;
    DWORD* pPushLimit = D3D__PushLimit;

    // The following is to combat a problem we're seeing where the write-
    // combine cache doesn't seem to get properly flushed using an 'sfence'.
    // An easy way to repro is to run Quake, and do a KickOffAndWaitForEmpty()
    // after every DrawPrimitive.

    _asm 
    {
        mov esi, pPut
        mov edi, pPushLimit
        add edi, 1020
        mov eax, [esi]
        xchg eax, [edi]

        xor eax, eax
        mov esi, pPushBase
        or [esi], eax
        mov esi, pPushMiddle
        or [esi], eax
    }

    // Flush the write-combine cache:

    _asm sfence
}

void __cdecl main()
{
    // Set the screen mode.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
//    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    Direct3D_SetPushBufferSize(PushBufferSize, PushBufferSize / 16);

    // Create the device.
    Direct3D_CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          NULL,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &d3dpp,
                          &pDevice);

    DbgPrint("Generating the commands to test.\n");

    /* 
     * Do setup...stuff we dont' want to time.
     */

    Setup();

    /*
     * Do some cheating to remember what is getting pushed.
     */

    D3DDevice_BlockUntilIdle();

    DWORD *pStartPush = g_pPushBuffer->m_pPut;

    // Hork the threshold so we won't kick off.  If you go beyond
    // then this whole thing will fall apart.
    //
    g_pPushBuffer->m_pThreshold = D3D__PushLimit;

    /*
     * Draw the commands to time.
     */

    Draw();

    /*
     * Snapshot what was drawn
     */

    DWORD *pEndPush = g_pPushBuffer->m_pPut;

    DWORD Size = (pEndPush - pStartPush) * 4;
    void *pCode = malloc(Size);

    memcpy(pCode, pStartPush, Size);

    /*
     * Do any verification (i.e Present) needed to see if this worked.
     */
    
    Verify();

    D3DDevice_BlockUntilIdle();

    /*
     * Prepare to run the test.
     */

    #define HwOffset(x)    ((DWORD)(x) & 0x0FFFFFFF)
    
    volatile DWORD *GpuGet  = D3D__GpuGet;
    volatile DWORD *GpuPut  = D3D__GpuPut;
    volatile DWORD *GpuReg  = D3D__GpuReg;
    volatile DWORD *GpuTime = D3D__GpuTime;

    DWORD *pPushBase = D3D__PushBase;
    DWORD *pPushLimit = D3D__PushLimit;

    DWORD *pPut = g_pPushBuffer->m_pPut;

    //
    // Reset the push buffer back to zero.
    //

    // Set reference.
    *(pPut++) = PUSHER_METHOD(SUBCH_3D, 0x00000050, 1);
    *(pPut++) = 0;

    // Jump to start of pb.
    *(pPut++) = (0x00000001 + HwOffset(pPushBase));

    FlushWCCache();

    // Run it.
    *GpuPut = HwOffset(pPushBase);

    // Wait until it finishes.
    while (*GpuGet != HwOffset(pPushBase))
    {
        Sleep(100);
    }

    while (GpuReg[0x00400700 / 4])
    {
        Sleep(100);
    }

    //
    // Figure out how many times we can run the commands.
    // 

    DbgPrint("Filling the push buffer.\n");

    DWORD ActualPushBufferSize = (pPushLimit - pPushBase - 5) * 4;
    DWORD Count = ActualPushBufferSize / Size;

    if (Count == 0)
    {
        _asm int 3;
    }

    BYTE *pDest = (BYTE *)pPushBase;

    for (DWORD i = 0; i < Count; i++)
    {
        memcpy(pDest, pCode, Size);

        pDest+= Size;
    }

    pPut = (DWORD *)pDest;

    //
    // Do the run.
    //

    DbgPrint("Executing the test.\n");

    // Write a funky value to the semaphore to indicate the end of the run.

    *(pPut++) = PUSHER_METHOD(SUBCH_3D, 0x00001d70, 1);
    *(pPut++) = 0x80088008;

    // Add a jump back to zero at the end of the buffer so we can rerun this test.
    *pPut = (0x00000001 + HwOffset(pPushBase));

    FlushWCCache();

    DWORD FinalPut = HwOffset(pPut);
    ULONGLONG TotalTime = 0;

    for (DWORD i = 0; i < Repeat; i++)
    {
        *GpuTime = 0;

        // 
        // Run the test.
        //

        ULONGLONG Begin, End;

        Begin = GetTime();

        *GpuPut = FinalPut;

        while (*GpuTime != 0x80088008)
            ;

        End = GetTime();

        ULONGLONG Time = End - Begin;
        TotalTime += Time;

        if (Count == 0)
        {
            DbgPrint("%I64u cycles.\n", Time);
        }
        else
        {
            DbgPrint("Time = %d cycles/iteration (%I64u cycles for %d iterations).\n", (DWORD)(Time / (ULONGLONG)Count), Time, Count);
        }

        //
        // Reset back to zero.
        //

        *GpuPut = HwOffset(pPushBase);

        // Wait until it finishes.
        while (*GpuGet != HwOffset(pPushBase))
        {
            Sleep(100);
        }

        while (GpuReg[0x00400700 / 4])
        {
            Sleep(100);
        }
    }

    if (Count == 0)
    {
        DbgPrint("\nAverage Time - %I64u cycles.\n", TotalTime / (ULONGLONG)Repeat);
    }
    else
    {
        DbgPrint("\nAverages: %d cycles/iteration, %I64u cycles.\n", (DWORD)(TotalTime / (ULONGLONG)Count / (ULONGLONG)Repeat), TotalTime / (ULONGLONG)Repeat);
    }

    _asm int 3;

    HalReturnToFirmware(HalQuickRebootRoutine);    
}


/***
 *** The tests. Uncomment something to go.
 **/

/*
 * Fill the buffer with NOPs.
 */

#if 0

// Variation, use KELVIN nops insteads.
#define USE_KELVIN_NOPS 0

DWORD PushBufferSize = (16 * 1024 * 1024);
DWORD Repeat = 10;

void Setup()
{
}

void Draw()
{
    DWORD *pPush = XMETAL_StartPush(g_pPushBuffer);

#if USE_KELVIN_NOPS

    *(pPush + 0) = PUSHER_METHOD(SUBCH_3D, 0x00000100, 1);
    *(pPush + 1) = 0;

    XMETAL_PushedRaw(pPush + 2);
    XMETAL_EndPush(g_pPushBuffer, pPush + 2);

#else 

    *(pPush + 0) = 0;

    XMETAL_PushedRaw(pPush + 1);
    XMETAL_EndPush(g_pPushBuffer, pPush + 1);

#endif
}

void Verify()
{
}

#endif

/*
 * Draw 4 triangles, each of which covers half of the screen.  The triangles
 * are textured from a very large linear 32-bit texture.
 */

#if 0

// Variation, push a 'wait for idle' after each set of 4 triangles.
#define ADD_WAIT_FOR_IDLE 0

DWORD PushBufferSize = (4 * 1024 * 1024);
DWORD Repeat = 5;

DWORD dwFVF = D3DFVF_XYZRHW|D3DFVF_TEX1;

//------------------------------------------------------------------------------
// Define our screen space triangle.

static struct TheVerts { float x,y,z,w,tu,tv; } Verts[] =
{
    {   0.0f,    0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
    { 640.0f,    0.0f, 0.5f, 1.0f, 255.0f,   0.0f},
    {   0.0f,  480.0f, 0.5f, 1.0f,   0.0f, 127.0f},
    {   0.0f,  480.0f, 0.5f, 1.0f,   0.0f, 127.0f},
    { 640.0f,    0.0f, 0.5f, 1.0f, 255.0f,   0.0f},
    { 640.0f,  480.0f, 0.5f, 1.0f, 255.0f, 127.0f},

    {   0.0f,    0.0f, 0.5f, 1.0f,   0.0f, 128.0f},
    { 640.0f,    0.0f, 0.5f, 1.0f, 255.0f, 128.0f},
    {   0.0f,  480.0f, 0.5f, 1.0f,   0.0f, 255.0f},
    {   0.0f,  480.0f, 0.5f, 1.0f,   0.0f, 255.0f},
    { 640.0f,    0.0f, 0.5f, 1.0f, 255.0f, 128.0f},
    { 640.0f,  480.0f, 0.5f, 1.0f, 255.0f, 255.0f},
    };

void Setup()
{
    // Create the texture.
    D3DTexture *pTexture;
    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;

    D3DDevice_CreateTexture(256, 256, 1, 0, D3DFMT_LIN_X8R8G8B8, D3DPOOL_MANAGED, &pTexture);

    pTexture->GetLevelDesc(0, &desc);
    pTexture->LockRect(0, &lockRect, NULL, D3DLOCK_RAWDATA);

    // Make the texture half white, half blue.
    DWORD *pb = (DWORD *)lockRect.pBits;
    DWORD y;
    
    for (y = 0; y < 256; y++)
    {
        for (DWORD x = 0; x < 256; x++)
        {
            if (y >= 128)
            {
                *pb = 0xFFFFFFFF;
            }
            else
            {
                *pb = 0xFF0000FF;
            }

            pb++;
        }
    }

    // Create a vertex buffer.
    D3DVertexBuffer *pVB;
    void *pVerts;

    D3DDevice_CreateVertexBuffer(sizeof(Verts), 0, 0, 0, &pVB);

    pVB->Lock(0, sizeof(Verts), (BYTE **)(&pVerts), 0);
    memcpy((void *)pVerts, (void *)Verts, sizeof(Verts));
    pVB->Unlock();

    // Set the states to ready for the render.
    D3DDevice_SetTexture(0, pTexture);
    D3DDevice_SetStreamSource(0, pVB, sizeof(Verts[0]));
    D3DDevice_SetVertexShader(dwFVF);

    D3DDevice_SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU ,D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV ,D3DTADDRESS_CLAMP);

    D3DDevice_Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0, 0);
}

void Draw()
{
    D3DDevice_DrawVertices(D3DPT_TRIANGLELIST, 0, 12); 

#if ADD_WAIT_FOR_IDLE

    DWORD *pPush = XMETAL_StartPush(g_pPushBuffer);

    *(pPush + 0) = PUSHER_METHOD(SUBCH_3D, 0x00000110, 1);
    *(pPush + 1) = 0;

    XMETAL_PushedRaw(pPush + 2);
    XMETAL_EndPush(g_pPushBuffer, pPush + 2);

#endif
}

void Verify()
{
    D3DDevice_Present(NULL, NULL, NULL, NULL);
}

#endif 0

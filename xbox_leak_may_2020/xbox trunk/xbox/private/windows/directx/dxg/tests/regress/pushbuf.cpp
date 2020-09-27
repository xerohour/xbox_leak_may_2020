/*==========================================================================;
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pushbuf.cpp
 *  Content:    Push-buffer resource regression test 
 *
 ***************************************************************************/
 
#include "precomp.hpp" 
 
IDirect3DVertexBuffer8*    pVertsVB = NULL;
IDirect3DVertexBuffer8*    pRedTriangleVB = NULL;
IDirect3DTexture8*         pTexture = NULL;
DWORD                      VertsShader;
DWORD                      RedTriangleShader;

//------------------------------------------------------------------------------
// Geometry declarations

DWORD VertsShaderDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT4), 
    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

static struct TheVerts { float x,y,z,w,tu,tv; } Verts[] =
{
    {320.0f,   0.0f, 0.5f, 1.0f,  0.5f, 0.0f},
    {527.0f, 360.0f, 0.5f, 1.0f,  1.0f, 1.0f},
    {113.0f, 360.0f, 0.5f, 1.0f,  0.0f, 1.0f},
};

DWORD RedTriangleShaderDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT4), 
    D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
    D3DVSD_END()
};

static struct TheRedTriangle { float x,y,z,w; DWORD color; } RedTriangle[] =
{
    {320.0f, 240.0f, 0.5f, 1.0f, 0xffff0000, },
    {310.0f, 250.0f, 0.5f, 1.0f, 0xffff0000, },
    {330.0f, 250.0f, 0.5f, 1.0f, 0xffff0000, },
};

//------------------------------------------------------------------------------
bool InitVB(D3DDevice* pDev)
{
    void* pVerts;
    void* pRedTriangle;

    CheckHR( pDev->CreateVertexBuffer( sizeof(Verts), 0, 0, 0, &pVertsVB ) );
    CheckHR( pDev->CreateVertexBuffer( sizeof(RedTriangle), 0, 0, 0, &pRedTriangleVB ) );

    pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );
    memcpy( (void*)pVerts, (void*)Verts, sizeof(Verts) );
    pVertsVB->Unlock();

    pRedTriangleVB->Lock( 0, sizeof(RedTriangle), (BYTE **)(&pRedTriangle), 0 );
    memcpy( (void*)pRedTriangle, (void*)RedTriangle, sizeof(RedTriangle) );
    pRedTriangleVB->Unlock();

    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;

    if ( pDev->CreateTexture( 256, 256, 1, 0, D3DFMT_P8, D3DPOOL_MANAGED, &pTexture ) != S_OK )
        return false;

    if ( pTexture->GetLevelDesc( 0, &desc ) != S_OK )
        return false;

    if ( pTexture->LockRect ( 0, &lockRect, NULL, 0 ) != S_OK)
        return false;

    PVOID pBits = LocalAlloc(0, 256 * 256 * 1);

    // Generate a set of vertical lines of varying color into a buffer.
    BYTE *pb = (BYTE *)pBits;
    DWORD y;
    
    for (y = 0; y < 256; y++)
    {
        for (DWORD x = 0; x < 256; x++)
        {
            *pb = (BYTE) x;

            pb++;
        }
    }

    // Now draw an 'X' using index 0:
    pb = (BYTE *)pBits;
    for (y = 0; y < 256; y++)
    {
        *pb = 0;

        pb += 257;
    }

    pb = (BYTE *)pBits + 255;
    for (y = 0; y < 256; y++)
    {
        *pb = 0;

        pb += 255;
    }


    XGSwizzleRect(pBits,
                  0,
                  NULL,
                  lockRect.pBits,
                  256,
                  256,
                  NULL,
                  1);

    LocalFree(pBits);

    // Unlock the map so it can be used
    pTexture->UnlockRect(0);

    pDev->SetTexture(0, pTexture);

    // We're done with this texture now
    pTexture->Release();

    // Create and select a fake test palette
    IDirect3DPalette8* pFakePalette;
    if ( pDev->CreatePalette(D3DPALETTE_32, &pFakePalette) != S_OK )
        return false;

    D3DCOLOR* pColors;
    if ( pFakePalette->Lock(&pColors, 0) != S_OK )
        return false;

    memset(pColors, 0, 32 * sizeof(D3DCOLOR));

    if ( pFakePalette->Unlock() != S_OK )
        return false;

    if ( pDev->SetPalette(0, pFakePalette) != S_OK )
        return false;

    // Now create and select a real palette
    IDirect3DPalette8* pPalette;
    if ( pDev->CreatePalette(D3DPALETTE_256, &pPalette) != S_OK )
        return false;

    if ( pPalette->Lock(&pColors, 0) != S_OK )
        return false;

    pColors[0] = 0xffff0000;                    // Index 0 is red
    pColors[255] = 0xff00ff00;                  // Index 255 is green

    for (DWORD i = 1; i < 255; i++)
    {
        pColors[i] = 0xff000000 | i;            // Rest is a blue wash
    }

    if ( pPalette->Unlock() != S_OK )
        return false;

    if ( pPalette->GetSize() != D3DPALETTE_256 )
        return false;

    if ( pDev->SetPalette(0, pPalette) != S_OK )
        return false;

    IDirect3DPalette8* pGetPalette;
    if ( pDev->GetPalette(0, &pGetPalette) != S_OK )
        return false;

    if ( pGetPalette != pPalette )
        return false;

    pGetPalette->Release();

    if ( pFakePalette->Release() != 0 )
        return false;

    // Set the texture stage states appropriately
    pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1);
    pDev->SetTextureStageState(0, D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    pDev->SetTextureStageState(0, D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
    pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Create the vertex shaders
    CheckHR( pDev->CreateVertexShader(VertsShaderDeclaration, NULL, &VertsShader, 0) );
    CheckHR( pDev->CreateVertexShader(RedTriangleShaderDeclaration, NULL, &RedTriangleShader, 0) );

    // Test some extra junk.

    CheckHR( pDev->Nop() );
    if (pDev->GetVertexBlendModelView(0, NULL, NULL) != S_FALSE)
        _asm int 3;

    D3DMATRIX mat[4];

    pDev->SetRenderState(D3DRS_VERTEXBLEND, TRUE);
    CheckHR( pDev->SetVertexBlendModelView(1, mat, mat, mat) );
    CheckHR( pDev->SetVertexBlendModelView(4, mat, mat, mat) );
    if (pDev->GetVertexBlendModelView(4, mat, mat) != S_OK)
        _asm int 3;

    pDev->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
    CheckHR(pDev->SetVertexBlendModelView(0, NULL, NULL, NULL));

    return true;
}

//------------------------------------------------------------------------------
void UnInitVB(D3DDevice* pDev)
{
    pVertsVB->Release();
    pRedTriangleVB->Release();

    pDev->DeleteVertexShader(VertsShader);
    pDev->DeleteVertexShader(RedTriangleShader);
}

//------------------------------------------------------------------------------
void Paint(D3DDevice* pDev)
{
    // Exercise the synchronization code.
    TheVerts* pVerts;
    D3DPushBuffer* pPushBuffer;
    DWORD i;
    DWORD offset;
    
    pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );

    // Transform the verticies to make the triangle spin so we can verify that
    // this actually works over a period of time.
    //
    static DWORD Time = GetTickCount();
    DWORD CurrentTime = GetTickCount();

    // 10 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 10000.0f;

    for (i = 0; i < 3; i++)
    {

        float x = Verts[i].x;
        float y = Verts[i].y;

        x = x - 320.0f;
        y = y - 240.0f;

        pVerts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        pVerts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        pVerts[i].x += 320.0f;
        pVerts[i].y += 240.0f;
    }

    pVertsVB->Unlock();

    pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00404040, 1.0, 0);

    D3DSTREAM_INPUT streamInput = { pVertsVB, sizeof(Verts[0]), 0 };
    pDev->SetStreamSource(0, pVertsVB, sizeof(Verts[0]));
    pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX1);
    pDev->SetVertexShaderInput(VertsShader, 1, &streamInput);

    pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    if (pDev->CreatePushBuffer(4 * 4096, FALSE, &pPushBuffer) != S_OK)
        _asm int 3;

    // Try a small empty push buffer:
    pDev->BeginPushBuffer(pPushBuffer);
    if (pDev->EndPushBuffer() != S_OK)
        _asm int 3;
    pDev->RunPushBuffer(pPushBuffer, NULL);

    // Try a big empty push buffer:
    pDev->BeginPushBuffer(pPushBuffer);
    for (i = 0; i < 1024; i++)
    {
        pDev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    }
    pDev->GetPushBufferOffset(&offset);
    if (offset != 8 * 1024)
        _asm int 3;
    if (pDev->EndPushBuffer() != S_OK)
        _asm int 3;
    pDev->RunPushBuffer(pPushBuffer, NULL);

    // Record our rotating triangle:
    pDev->BeginPushBuffer(pPushBuffer);
    pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    pDev->EndPushBuffer();
    pDev->RunPushBuffer(pPushBuffer, NULL);

    // Draw a red triangle on top:
    pDev->SetVertexShaderInput(NULL, 0, NULL);
    pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, RedTriangle, sizeof(RedTriangle[0]));

    // Flip the buffers.
    pDev->Present(NULL, NULL, NULL, NULL);

    pPushBuffer->Release();
}

//------------------------------------------------------------------------------
void MemoryCompare(VOID* a, VOID* b, DWORD size)
{
    BOOL mismatch = FALSE;
    BYTE* pa = (BYTE*) a;
    BYTE* pb = (BYTE*) b;
    CHAR stringBuffer[200];

    while (size-- != 0)
    {
        if (*pa++ != *pb++)
        {
            sprintf(stringBuffer, "Mismatch at offset %li\n", pa - (BYTE*) a);
            OutputDebugStringA(stringBuffer);
            mismatch = TRUE;
        }
    }

    if (mismatch)
    {
        sprintf(stringBuffer, "a: %lx  b: %lx\n", a, b);
        OutputDebugStringA(stringBuffer);
        _asm int 3;
    }
}

//------------------------------------------------------------------------------
void PlayResources(
    D3DDevice* pDev,
    D3DPushBuffer* pPushBuffer, // If NULL, we're recording
    DWORD visibilityIndex)
{
    ////////////////////////////////////////////////////////////////////////////
    // RunPushBuffer

    static DWORD runPushBufferOffset;
    static D3DPushBuffer* pPushBufferDestination = NULL;
    if (pPushBufferDestination == NULL)
    {
        // Create an empty, initialized push-buffer:
        CheckHR(pDev->CreatePushBuffer(1024, FALSE, &pPushBufferDestination));
        pPushBufferDestination->Size = 1024;
        ZeroMemory((void*) pPushBufferDestination->Data, 1024);
    }

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&runPushBufferOffset))
        CheckHR(pDev->RunPushBuffer(pPushBufferDestination, NULL));
    }
    else
    {
        CheckHR(pPushBuffer->RunPushBuffer(runPushBufferOffset, pPushBufferDestination, NULL));
    }
    ////////////////////////////////////////////////////////////////////////////
    // SetModelView

    static DWORD setModelViewOffset;
    static D3DMATRIX matProjection;
    D3DMATRIX get1;
    D3DMATRIX get2;
    D3DXMATRIX matModelView;
    D3DXMATRIX matInverseModelView;
    D3DXMATRIX matProjectionViewport;
    D3DXMatrixIdentity(&matModelView);
    D3DXMatrixIdentity(&matInverseModelView);
    matInverseModelView.m[0][0] = 2;

    CheckHR(pDev->GetProjectionViewportMatrix(&matProjectionViewport));

    if (!pPushBuffer)
    {
        if (pDev->GetModelView(NULL) != S_FALSE)
            _asm int 3;
        CheckHR(pDev->GetProjectionViewportMatrix(&matProjection));
        CheckHR(pDev->GetPushBufferOffset(&setModelViewOffset));
        CheckHR(pDev->SetModelView(&matModelView, &matInverseModelView, &matProjection));
        if (pDev->GetModelView(&get1) != S_OK)
            _asm int 3;
        MemoryCompare(&matModelView, &get1, sizeof(matModelView));
        CheckHR(pDev->SetModelView(NULL, NULL, NULL));
        if (pDev->GetModelView(NULL) != S_FALSE)
            _asm int 3;
    }
    else
    {
        CheckHR(pPushBuffer->SetModelView(setModelViewOffset, &matModelView,
                                          &matInverseModelView, &matProjection));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexBlendModelView

    static DWORD setVertexBlendModelViewOffset;

    if (!pPushBuffer)
    {
        pDev->SetRenderState(D3DRS_VERTEXBLEND, TRUE);
        if (pDev->GetVertexBlendModelView(0, NULL, NULL) != S_FALSE)
            _asm int 3;
        CheckHR(pDev->GetPushBufferOffset(&setVertexBlendModelViewOffset));
        CheckHR(pDev->SetVertexBlendModelView(1, &matModelView, &matInverseModelView, &matProjectionViewport));
        if (pDev->GetVertexBlendModelView(0, NULL, NULL) != S_OK)
            _asm int 3;
        CheckHR(pDev->SetVertexBlendModelView(0, NULL, NULL, NULL));
        if (pDev->GetVertexBlendModelView(1, &get1, &get2) != S_FALSE)
            _asm int 3;
        MemoryCompare(&matModelView, &get1, sizeof(matModelView));
        MemoryCompare(&matProjectionViewport, &get2, sizeof(matModelView));
        pDev->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexBlendModelView(setVertexBlendModelViewOffset, 1, &matModelView, &matInverseModelView, &matProjectionViewport));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexShaderInput

    static DWORD setVertexShaderInputOffset;
    static D3DVertexBuffer* pVertexBuffer = NULL;
    static DWORD hVertexShader;
    DWORD shaderDeclaration[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position
        D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
        D3DVSD_END()
    };

    if (pVertexBuffer == NULL)
    {
        CheckHR(pDev->CreateVertexBuffer(1024, 0, 0, 0, &pVertexBuffer));
        CheckHR(pDev->CreateVertexShader(shaderDeclaration, NULL, &hVertexShader, 0));
    }
    D3DSTREAM_INPUT streamInput = { pVertexBuffer, 32, 0 };

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&setVertexShaderInputOffset));
        CheckHR(pDev->SetVertexShaderInput(hVertexShader, 1, &streamInput));
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexShaderInput(setVertexShaderInputOffset,
                                                  hVertexShader, 1, &streamInput));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetRenderTarget

    static DWORD setRenderTargetOffset;
    D3DSurface* pRenderTarget;
    D3DSurface* pZBuffer;

    CheckHR(pDev->GetRenderTarget(&pRenderTarget));
    CheckHR(pDev->GetDepthStencilSurface(&pZBuffer));

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&setRenderTargetOffset));
        CheckHR(pDev->SetRenderTarget(pRenderTarget, pZBuffer));
    }
    else
    {
        CheckHR(pPushBuffer->SetRenderTarget(setRenderTargetOffset, pRenderTarget, pZBuffer));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetTexture

    static DWORD setTextureOffset;
    static D3DTexture* pTexture = NULL;

    if (pTexture == NULL)
    {
        CheckHR(pDev->CreateTexture(32, 32, 1, 0, D3DFMT_DXT1, 0, &pTexture));
    }

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&setTextureOffset));
        CheckHR(pDev->SetTexture(1, pTexture));
    }
    else
    {
        CheckHR(pPushBuffer->SetTexture(setTextureOffset, 1, pTexture));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SwitchTexture

    static DWORD switchTextureOffset;
    static D3DTexture* pSwitch = NULL;

    if (pSwitch == NULL)
    {
        CheckHR(pDev->CreateTexture(32, 32, 1, 0, D3DFMT_DXT3, 0, &pSwitch));
    }

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&switchTextureOffset));
        CheckHR(pDev->SwitchTexture(1, pSwitch));
    }
    else
    {
        CheckHR(pPushBuffer->SetTexture(switchTextureOffset, 1, pSwitch));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetPalette

    static DWORD setPaletteOffset;
    static D3DPalette *pPalette = NULL;

    if (pPalette == NULL)
    {
        CheckHR(pDev->CreatePalette(D3DPALETTE_256, &pPalette));
    }

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&setPaletteOffset));
        CheckHR(pDev->SetPalette(0, pPalette));
    }
    else
    {
        CheckHR(pPushBuffer->SetPalette(setPaletteOffset, 0, pPalette));
    }

    ////////////////////////////////////////////////////////////////////////////
    // EndVisibilityTest

    static DWORD endVisibilityTestOffset;

    if (!pPushBuffer)
    {
        CheckHR(pDev->BeginVisibilityTest());
        CheckHR(pDev->GetPushBufferOffset(&endVisibilityTestOffset));
        CheckHR(pDev->EndVisibilityTest(visibilityIndex));
    }
    else
    {
        CheckHR(pPushBuffer->EndVisibilityTest(endVisibilityTestOffset, visibilityIndex));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexShaderConstant

    static DWORD setVertexShaderConstantOffset;
    const UINT constantCount = 55;
    DWORD constantData[4 * constantCount];

    for (DWORD i = 0; i < 4 * constantCount; i++)
    {
        constantData[i] = i;
    }

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&setVertexShaderConstantOffset));
        CheckHR(pDev->SetVertexShaderConstant(3, constantData, constantCount));
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexShaderConstant(setVertexShaderConstantOffset, 3, constantData, constantCount));
    }

    ////////////////////////////////////////////////////////////////////////////
    // Jump

    static DWORD jumpOffset;

    if (!pPushBuffer)
    {
        CheckHR(pDev->GetPushBufferOffset(&jumpOffset));
        CheckHR(pDev->Nop());
    }
    else
    {
        CheckHR(pPushBuffer->Jump(jumpOffset, jumpOffset));
    }

    ////////////////////////////////////////////////////////////////////////////
    // Clean-up after recording

    if (!pPushBuffer)
    {
        CheckHR(pDev->SetTexture(1, NULL));
    }
}

//------------------------------------------------------------------------------
DWORD g_index = 2;

//------------------------------------------------------------------------------
void TestFixups(D3DDevice* pDev)
{
    static CopyBuffer[32768];
    D3DPushBuffer* pPushBuffer;
    D3DFixup* pFixup;
    DWORD size1;
    DWORD size2;


    CheckHR(pDev->CreatePushBuffer(32768, FALSE, &pPushBuffer));
    CheckHR(pDev->BeginPushBuffer(pPushBuffer));
    PlayResources(pDev, NULL, ++g_index);
    CheckHR(pDev->EndPushBuffer());

    // Copy the result for later:
    memcpy(CopyBuffer, (VOID*) pPushBuffer->Data, pPushBuffer->Size);

    // Stomp all the resource references:
    CheckHR(pPushBuffer->Verify(TRUE));

    // Now reapply everything using a fixup:
    CheckHR(pPushBuffer->BeginFixup(NULL, 0));
    PlayResources(pDev, pPushBuffer, g_index);
    CheckHR(pPushBuffer->EndFixup());
    CheckHR(pPushBuffer->Verify(FALSE));

    // Finally, check that the results match:
    MemoryCompare(CopyBuffer, (VOID*) pPushBuffer->Data, pPushBuffer->Size - 4);

    // Do a pass with an empty fix-up object to see if anything falls over:
    CheckHR(pDev->CreateFixup(0, &pFixup));
    CheckHR(pPushBuffer->BeginFixup(pFixup, 0));
    PlayResources(pDev, pPushBuffer, ++g_index);

    // With an empty fix-up buffer, we should always get1 a too small error (but
    // GetSize will still be valid):
    if (pPushBuffer->EndFixup() != D3DERR_BUFFERTOOSMALL)
        _asm int 3;
    CheckHR(pFixup->GetSize(&size1));
    pFixup->Release();

    // Try again with a humongous fix-up buffer to verify that the size is
    // the same:
    CheckHR(pDev->CreateFixup(4096*4, &pFixup));
    CheckHR(pPushBuffer->BeginFixup(pFixup, 0));
    PlayResources(pDev, pPushBuffer, ++g_index);
    CheckHR(pPushBuffer->EndFixup());
    CheckHR(pFixup->GetSize(&size2));
    pFixup->Release();

    if (size1 != size2)
        _asm int 3;
}

//------------------------------------------------------------------------------
void TestDrawVertices(D3DDevice* pDev)
{
    DWORD i;
    WORD indexBuffer[4096];
    D3DPushBuffer* pPushBuffer;
    D3DFixup* pFixup;
    BYTE compileBuffer[32768];
    DWORD compileSize;

    for (i = 0; i < 4096; i++)
        indexBuffer[i] = (WORD) i;

    CheckHR( pDev->CreatePushBuffer(32768, FALSE, &pPushBuffer) );

    CheckHR( pDev->BeginPushBuffer(pPushBuffer) );
    CheckHR( pDev->DrawIndexedVertices(D3DPT_TRIANGLELIST, 5, indexBuffer) );
    CheckHR( pDev->EndPushBuffer() );

    compileSize = sizeof(compileBuffer);
    CheckHR( XGCompileDrawIndexedVertices(compileBuffer, &compileSize,
                                          D3DPT_TRIANGLELIST, 5, indexBuffer) );

    if ( compileSize != pPushBuffer->Size )
        _asm int 3;
    
    MemoryCompare( (VOID*) pPushBuffer->Data, compileBuffer, compileSize - 4 );

    CheckHR( pDev->BeginPushBuffer(pPushBuffer) );
    CheckHR( pDev->DrawIndexedVertices(D3DPT_LINELIST, 3000, indexBuffer + 3) );
    CheckHR( pDev->EndPushBuffer() );

    compileSize = sizeof(compileBuffer);
    CheckHR( XGCompileDrawIndexedVertices(compileBuffer, &compileSize,
                                          D3DPT_LINELIST, 3000, indexBuffer + 3) );

    // Ack, MikeSart's optimized DrawIndexedVertices no longer produces the
    // same signature, so drop the compare:

    // if ( compileSize != pPushBuffer->Size )
    //     _asm int 3;
    //
    // MemoryCompare( (VOID*) pPushBuffer->Data, compileBuffer, compileSize - 4 );

    pPushBuffer->Release();

    CheckHR( pDev->CreateFixup(1024, &pFixup) );
    pFixup->Release();
}

//------------------------------------------------------------------------------
#define TRIANGLE_NUM    8

struct VERTEX {
    FLOAT x, y, z;
};

void TestSizes1(D3DDevice* pDev)
{
    // Create Vertex Buffer
    D3DVertexBuffer *vtx;
    pDev->CreateVertexBuffer( 8192,
                                D3DUSAGE_WRITEONLY, 
                                0, 
                                D3DPOOL_MANAGED, 
                                &vtx );

    VERTEX *p;
    vtx->Lock( 0, 0, (BYTE **)&p, 0 );
    ZeroMemory(p, 3 * sizeof(VERTEX));
    vtx->Unlock();

    // Create Push Buffer
    D3DPushBuffer *push;
    pDev->CreatePushBuffer( 8192, FALSE, &push );
    pDev->BeginPushBuffer( push );
    for ( int i = 0; i < TRIANGLE_NUM; i++ )
        pDev->DrawVertices( D3DPT_TRIANGLELIST, 0, 3 );

    pDev->EndPushBuffer();

    for ( int i = 0; i < 128; i++ )
    {
        *((DWORD*) (push->Data + push->Size - 4))
            = 0xdeadbeef;

        pDev->RunPushBuffer( push, NULL );

        pDev->BlockUntilIdle();
    }
}

//------------------------------------------------------------------------------
WORD g_Zero[1024];

#define PUSH_BUFFER_SIZE 8192
#define VERTEX_BUFFER_SIZE 8192

//------------------------------------------------------------------------------
void TestSizes2(D3DDevice* pDev)
{
    D3DPushBuffer* pPushBuffer;
    D3DVertexBuffer* pVertexBuffer;
    BYTE* pData;

    pDev->CreatePushBuffer(PUSH_BUFFER_SIZE, FALSE, &pPushBuffer);
    pDev->CreateVertexBuffer(VERTEX_BUFFER_SIZE, 0, 0, 0, &pVertexBuffer);

    pVertexBuffer->Lock(0, 0, &pData, 0);
    ZeroMemory(pData, VERTEX_BUFFER_SIZE);
    pVertexBuffer->Unlock();

    pDev->SetStreamSource(0, pVertexBuffer, 32);

    for (DWORD type = 0; type < 3; type++)
    {
        for (DWORD indices = 1; indices < 1024; indices++)
        {
            // For each, use a degenerate (zero-sized) triangle:

            RtlFillMemoryUlong((void*) pPushBuffer->Data, 
                               PUSH_BUFFER_SIZE,
                               0xdeadbeef);

            switch (type)
            {
            case 2:
                pPushBuffer->Size = PUSH_BUFFER_SIZE;
                CheckHR(XGCompileDrawIndexedVertices((void*) pPushBuffer->Data, 
                                                     &pPushBuffer->Size,
                                                     D3DPT_TRIANGLELIST,
                                                     indices,
                                                     g_Zero));
                break;
            case 1:
                pDev->BeginPushBuffer(pPushBuffer);
                pDev->DrawIndexedVertices(D3DPT_TRIANGLELIST, indices, &g_Zero[0]);
                CheckHR(pDev->EndPushBuffer());
                break;
            case 0:
                pDev->BeginPushBuffer(pPushBuffer);
                for (DWORD i = 0; i < indices / 8; i++)
                    pDev->DrawVertices(D3DPT_TRIANGLELIST, 0, 3);

                CheckHR(pDev->EndPushBuffer());
                break;
            }
    
            for (DWORD nops = 0; nops < 16; nops++)
            {
                for (DWORD repeats = 0; repeats < 16; repeats++)
                {
                    *((DWORD*) (pPushBuffer->Data + pPushBuffer->Size - 4))
                        = 0xdeadbeef;
    
                    for (DWORD i = 0; i < nops; i++)
                        pDev->Nop();
    
                    pDev->RunPushBuffer(pPushBuffer, NULL);
                    pDev->BlockUntilIdle();
                }
            }
        }
    }

    pPushBuffer->Release();
    pVertexBuffer->Release();
}

//------------------------------------------------------------------------------
void TestPushBuffers(D3DDevice* pDev)
{
    TestDrawVertices(pDev);
    TestFixups(pDev);
    TestSizes1(pDev);
    TestSizes2(pDev);

    InitVB(pDev);
    for (DWORD i = 0; i < 20; i++)
    {
        Paint(pDev);
    }
    UnInitVB(pDev);
}

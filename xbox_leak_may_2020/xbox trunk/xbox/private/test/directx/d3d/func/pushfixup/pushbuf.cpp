/*==========================================================================;
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pushbuf.cpp
 *  Content:    Push-buffer resource regression test 
 *
 ***************************************************************************/
 
#include "precomp.hpp" 
 
IDirect3DVertexBuffer8*    g_pVertsVB = NULL;
IDirect3DVertexBuffer8*    g_pRedTriangleVB = NULL;
IDirect3DTexture8*         g_pTexture = NULL;
DWORD                      g_VertsShader;
DWORD                      g_RedTriangleShader;

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
bool InitVB()
{
    void* pVerts;
    void* pRedTriangle;

    CheckHR( g_pDev->CreateVertexBuffer( sizeof(Verts), 0, 0, 0, &g_pVertsVB ) );
    CheckHR( g_pDev->CreateVertexBuffer( sizeof(RedTriangle), 0, 0, 0, &g_pRedTriangleVB ) );

    g_pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );
    memcpy( (void*)pVerts, (void*)Verts, sizeof(Verts) );
    g_pVertsVB->Unlock();

    g_pRedTriangleVB->Lock( 0, sizeof(RedTriangle), (BYTE **)(&pRedTriangle), 0 );
    memcpy( (void*)pRedTriangle, (void*)RedTriangle, sizeof(RedTriangle) );
    g_pRedTriangleVB->Unlock();

    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;

    if ( g_pDev->CreateTexture( 256, 256, 1, 0, D3DFMT_P8, D3DPOOL_MANAGED, &g_pTexture ) != S_OK )
        return false;

    if ( g_pTexture->GetLevelDesc( 0, &desc ) != S_OK )
        return false;

    if ( g_pTexture->LockRect ( 0, &lockRect, NULL, 0 ) != S_OK)
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
    g_pTexture->UnlockRect(0);

    g_pDev->SetTexture(0, g_pTexture);

    // We're done with this texture now
    SAFE_RELEASE(g_pTexture);

    // Create and select a fake test palette
    IDirect3DPalette8* pFakePalette;
    CheckHR( g_pDev->CreatePalette(D3DPALETTE_32, &pFakePalette)  );

    D3DCOLOR* pColors;
    CheckHR( pFakePalette->Lock(&pColors, 0)  );

    memset(pColors, 0, 32 * sizeof(D3DCOLOR));

    CheckHR( pFakePalette->Unlock() );

    CheckHR( g_pDev->SetPalette(0, pFakePalette)  );

    // Now create and select a real palette
    IDirect3DPalette8* pPalette;
    CheckHR ( g_pDev->CreatePalette(D3DPALETTE_256, &pPalette) );

    CheckHR ( pPalette->Lock(&pColors, 0) );

    pColors[0] = 0xffff0000;                    // Index 0 is red
    pColors[255] = 0xff00ff00;                  // Index 255 is green

    for (DWORD i = 1; i < 255; i++)
    {
        pColors[i] = 0xff000000 | i;            // Rest is a blue wash
    }

    CheckHR ( pPalette->Unlock() );

	if ( pPalette->GetSize() != D3DPALETTE_256 ) {
		DebugPrint("pushbuf: pPalette->GetSize() != D3DPALETTE_256");
        goto cleanup;
	}

    CheckHR ( g_pDev->SetPalette(0, pPalette) )

    IDirect3DPalette8* pGetPalette;
    CheckHR ( g_pDev->GetPalette(0, &pGetPalette) );

    Check ( pGetPalette == pPalette );

    SAFE_RELEASE(pGetPalette);

	SAFE_RELEASE( pFakePalette);

    // Set the texture stage states appropriately
    g_pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1);
    g_pDev->SetTextureStageState(0, D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    g_pDev->SetTextureStageState(0, D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
    g_pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Create the vertex shaders
    CheckHR( g_pDev->CreateVertexShader(VertsShaderDeclaration, NULL, &g_VertsShader, 0) );
    CheckHR( g_pDev->CreateVertexShader(RedTriangleShaderDeclaration, NULL, &g_RedTriangleShader, 0) );

    // Test some extra junk.

    CheckHR( g_pDev->Nop() );
    if (g_pDev->GetVertexBlendModelView(0, NULL, NULL) != S_FALSE)
        _asm int 3;

    D3DMATRIX mat[4];

    g_pDev->SetRenderState(D3DRS_VERTEXBLEND, TRUE);
    CheckHR( g_pDev->SetVertexBlendModelView(1, mat, mat, mat) );
    CheckHR( g_pDev->SetVertexBlendModelView(4, mat, mat, mat) );
    if (g_pDev->GetVertexBlendModelView(4, mat, mat) != S_OK)
        _asm int 3;

    g_pDev->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
    CheckHR(g_pDev->SetVertexBlendModelView(0, NULL, NULL, NULL));

	SAFE_RELEASE(pFakePalette);
	SAFE_RELEASE(pPalette);
	SAFE_RELEASE(pGetPalette);

    return true;

cleanup:
	SAFE_RELEASE(g_pVertsVB);
	SAFE_RELEASE(g_pRedTriangleVB);
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(pFakePalette);
	SAFE_RELEASE(pPalette);
	SAFE_RELEASE(pGetPalette);
	return false;

}

//------------------------------------------------------------------------------
void UnInitVB()
{
    SAFE_RELEASE(g_pVertsVB);
    SAFE_RELEASE(g_pRedTriangleVB);

    if(g_VertsShader) { g_pDev->DeleteVertexShader(g_VertsShader); g_VertsShader = 0; }
    if(g_RedTriangleShader) { g_pDev->DeleteVertexShader(g_RedTriangleShader); g_RedTriangleShader = 0; }
}

//------------------------------------------------------------------------------
HRESULT Paint()
{
    // Exercise the synchornization code.
    TheVerts* pVerts;
    D3DPushBuffer* pPushBuffer;
    DWORD i;
    DWORD offset;
    
    g_pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );

    // Transform the verticies to make the triangle spin so we can verify that
    // this actually works over a period of time.
    //
    static DWORD s_Time = GetTickCount();
    DWORD CurrentTime = GetTickCount();

    // 10 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - s_Time) / 10000.0f;

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

    g_pVertsVB->Unlock();

    g_pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00404040, 1.0, 0);

    D3DSTREAM_INPUT streamInput = { g_pVertsVB, sizeof(Verts[0]), 0 };
    g_pDev->SetStreamSource(0, g_pVertsVB, sizeof(Verts[0]));
    g_pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX1);
    g_pDev->SetVertexShaderInput(g_VertsShader, 1, &streamInput);

    g_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    CheckHR (g_pDev->CreatePushBuffer(4 * 4096, FALSE, &pPushBuffer) )

    // Try a small empty push buffer:
    g_pDev->BeginPushBuffer(pPushBuffer);
    if (g_pDev->EndPushBuffer() != S_OK)
        _asm int 3;
    g_pDev->RunPushBuffer(pPushBuffer, NULL);

    // Try a big empty push buffer:
    g_pDev->BeginPushBuffer(pPushBuffer);
    for (i = 0; i < 1024; i++)
    {
        g_pDev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    }
    g_pDev->GetPushBufferOffset(&offset);
    if (offset != 8 * 1024)
        _asm int 3;
    if (g_pDev->EndPushBuffer() != S_OK)
        _asm int 3;
    g_pDev->RunPushBuffer(pPushBuffer, NULL);

    // Record our rotating triangle:
    g_pDev->BeginPushBuffer(pPushBuffer);
    g_pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    g_pDev->EndPushBuffer();
    g_pDev->RunPushBuffer(pPushBuffer, NULL);

    // Draw a red triangle on top:
    g_pDev->SetVertexShaderInput(NULL, 0, NULL);
    g_pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    g_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    g_pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, RedTriangle, sizeof(RedTriangle[0]));

    // Flip the buffers.
    g_pDev->Present(NULL, NULL, NULL, NULL);

    SAFE_RELEASE(pPushBuffer);
	return S_OK;

cleanup:
    SAFE_RELEASE(pPushBuffer);
	return E_OUTOFMEMORY;
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
bool PlayResources(D3DPushBuffer* pPushBuffer, DWORD visibilityIndex)
{

	//initializations done here to avoid annoying pointless compiler warnings
    D3DXMATRIX matModelView;
    D3DXMATRIX matInverseModelView;
    D3DXMATRIX matProjectionViewport;


    ////////////////////////////////////////////////////////////////////////////
    // RunPushBuffer

    static DWORD s_runPushBufferOffset;
    static D3DPushBuffer* s_pPushBufferDestination = NULL;
    if (s_pPushBufferDestination == NULL)
    {
        // Create an empty, initialized push-buffer:
        CheckHR(g_pDev->CreatePushBuffer(1024, FALSE, &s_pPushBufferDestination));
        s_pPushBufferDestination->Size = 1024;
        ZeroMemory((void*) s_pPushBufferDestination->Data, 1024);
    }

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_runPushBufferOffset))
        CheckHR(g_pDev->RunPushBuffer(s_pPushBufferDestination, NULL));
    }
    else
    {
        CheckHR(pPushBuffer->RunPushBuffer(s_runPushBufferOffset, s_pPushBufferDestination, NULL));
    }
    ////////////////////////////////////////////////////////////////////////////
    // SetModelView

    static DWORD s_setModelViewOffset;
    static D3DMATRIX s_matProjection;
    D3DMATRIX get1;
    D3DMATRIX get2;
    D3DXMatrixIdentity(&matModelView);
    D3DXMatrixIdentity(&matInverseModelView);
    matInverseModelView.m[0][0] = 2;

    CheckHR(g_pDev->GetProjectionViewportMatrix(&matProjectionViewport));

    if (!pPushBuffer)
    {
        if (g_pDev->GetModelView(NULL) != S_FALSE)
            _asm int 3;
        CheckHR(g_pDev->GetProjectionViewportMatrix(&s_matProjection));
        CheckHR(g_pDev->GetPushBufferOffset(&s_setModelViewOffset));
        CheckHR(g_pDev->SetModelView(&matModelView, &matInverseModelView, &s_matProjection));
        if (g_pDev->GetModelView(&get1) != S_OK)
            _asm int 3;
        MemoryCompare(&matModelView, &get1, sizeof(matModelView));
        CheckHR(g_pDev->SetModelView(NULL, NULL, NULL));
        if (g_pDev->GetModelView(NULL) != S_FALSE)
            _asm int 3;
    }
    else
    {
        CheckHR(pPushBuffer->SetModelView(s_setModelViewOffset, &matModelView,
                                          &matInverseModelView, &s_matProjection));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexBlendModelView

    static DWORD s_setVertexBlendModelViewOffset;

    if (!pPushBuffer)
    {
        g_pDev->SetRenderState(D3DRS_VERTEXBLEND, TRUE);
        if (g_pDev->GetVertexBlendModelView(0, NULL, NULL) != S_FALSE)
            _asm int 3;
        CheckHR(g_pDev->GetPushBufferOffset(&s_setVertexBlendModelViewOffset));
        CheckHR(g_pDev->SetVertexBlendModelView(1, &matModelView, &matInverseModelView, &matProjectionViewport));
        if (g_pDev->GetVertexBlendModelView(0, NULL, NULL) != S_OK)
            _asm int 3;
        CheckHR(g_pDev->SetVertexBlendModelView(0, NULL, NULL, NULL));
        if (g_pDev->GetVertexBlendModelView(1, &get1, &get2) != S_FALSE)
            _asm int 3;
        MemoryCompare(&matModelView, &get1, sizeof(matModelView));
        MemoryCompare(&matProjectionViewport, &get2, sizeof(matModelView));
        g_pDev->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexBlendModelView(s_setVertexBlendModelViewOffset, 1, &matModelView, &matInverseModelView, &matProjectionViewport));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexShaderInput

    static DWORD s_setVertexShaderInputOffset;
    static DWORD s_hVertexShader;
    static D3DVertexBuffer* s_pVertexBuffer = NULL;

    DWORD shaderDeclaration[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position
        D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
        D3DVSD_END()
    };

    if (s_pVertexBuffer == NULL)
    {
        CheckHR(g_pDev->CreateVertexBuffer(1024, 0, 0, 0, &s_pVertexBuffer));
        CheckHR(g_pDev->CreateVertexShader(shaderDeclaration, NULL, &s_hVertexShader, 0));
    }
    D3DSTREAM_INPUT streamInput;
	streamInput.VertexBuffer = s_pVertexBuffer;
	streamInput.Stride = 32;
	streamInput.Offset = 0;

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_setVertexShaderInputOffset));
        CheckHR(g_pDev->SetVertexShaderInput(s_hVertexShader, 1, &streamInput));
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexShaderInput(s_setVertexShaderInputOffset,
                                                  s_hVertexShader, 1, &streamInput));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetRenderTarget

    static DWORD s_setRenderTargetOffset;
    D3DSurface* pRenderTarget;
    D3DSurface* pZBuffer;

    CheckHR(g_pDev->GetRenderTarget(&pRenderTarget));
    CheckHR(g_pDev->GetDepthStencilSurface(&pZBuffer));

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_setRenderTargetOffset));
        CheckHR(g_pDev->SetRenderTarget(pRenderTarget, pZBuffer));
    }
    else
    {
        CheckHR(pPushBuffer->SetRenderTarget(s_setRenderTargetOffset, pRenderTarget, pZBuffer));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetTexture

    static DWORD s_setTextureOffset;
    static D3DTexture* s_pTexture = NULL;

    if (s_pTexture == NULL)
    {
        CheckHR(g_pDev->CreateTexture(32, 32, 1, 0, D3DFMT_DXT1, 0, &s_pTexture));
    }

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_setTextureOffset));
        CheckHR(g_pDev->SetTexture(1, s_pTexture));
    }
    else
    {
        CheckHR(pPushBuffer->SetTexture(s_setTextureOffset, 1, s_pTexture));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SwitchTexture

    static DWORD s_switchTextureOffset;
    static D3DTexture* s_pSwitch = NULL;

    if (s_pSwitch == NULL)
    {
        CheckHR(g_pDev->CreateTexture(32, 32, 1, 0, D3DFMT_DXT3, 0, &s_pSwitch));
    }

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_switchTextureOffset));
        CheckHR(g_pDev->SwitchTexture(1, s_pSwitch));
    }
    else
    {
        CheckHR(pPushBuffer->SetTexture(s_switchTextureOffset, 1, s_pSwitch));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetPalette

    static DWORD s_setPaletteOffset;
    static D3DPalette *s_pPalette = NULL;

    if (s_pPalette == NULL)
    {
        CheckHR(g_pDev->CreatePalette(D3DPALETTE_256, &s_pPalette));
    }

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_setPaletteOffset));
        CheckHR(g_pDev->SetPalette(0, s_pPalette));
    }
    else
    {
        CheckHR(pPushBuffer->SetPalette(s_setPaletteOffset, 0, s_pPalette));
    }

    ////////////////////////////////////////////////////////////////////////////
    // EndVisibilityTest

    static DWORD s_endVisibilityTestOffset;

    CheckHR(g_pDev->BeginVisibilityTest());

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_endVisibilityTestOffset));
        CheckHR(g_pDev->EndVisibilityTest(visibilityIndex));
    }
    else
    {
        CheckHR(pPushBuffer->EndVisibilityTest(s_endVisibilityTestOffset, visibilityIndex));
    }

    ////////////////////////////////////////////////////////////////////////////
    // SetVertexShaderConstant

    static DWORD s_setVertexShaderConstantOffset;
    const UINT constantCount = 55;
    DWORD constantData[4 * constantCount];

    for (DWORD i = 0; i < 4 * constantCount; i++)
    {
        constantData[i] = i;
    }

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_setVertexShaderConstantOffset));
        CheckHR(g_pDev->SetVertexShaderConstant(3, constantData, constantCount));
    }
    else
    {
        CheckHR(pPushBuffer->SetVertexShaderConstant(s_setVertexShaderConstantOffset, 3, constantData, constantCount));
    }

    ////////////////////////////////////////////////////////////////////////////
    // Jump

    static DWORD s_jumpOffset;

    if (!pPushBuffer)
    {
        CheckHR(g_pDev->GetPushBufferOffset(&s_jumpOffset));
        CheckHR(g_pDev->Nop());
    }
    else
    {
        CheckHR(pPushBuffer->Jump(s_jumpOffset, s_jumpOffset));
    }

	return true;

cleanup:
	SAFE_RELEASE(s_pPushBufferDestination);
	SAFE_RELEASE(s_pVertexBuffer);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(pZBuffer);
	SAFE_RELEASE(s_pTexture);
	SAFE_RELEASE(s_pSwitch);
	SAFE_RELEASE(s_pPalette);
	return false;
}

//------------------------------------------------------------------------------
bool TestFixups()
{
    static CopyBuffer[32768];
    D3DPushBuffer* pPushBuffer;
    D3DFixup* pFixup;
    DWORD size1;
    DWORD size2;

    CheckHR(g_pDev->CreatePushBuffer(32768, FALSE, &pPushBuffer));
    CheckHR(g_pDev->BeginPushBuffer(pPushBuffer));
    PlayResources(NULL, 3);
    CheckHR(g_pDev->EndPushBuffer());

    // Copy the result for later:
    memcpy(CopyBuffer, (VOID*) pPushBuffer->Data, pPushBuffer->Size);

    // Stomp all the resource references:
    CheckHR(pPushBuffer->Verify(TRUE));

    // Now reapply everything using a fixup:
    CheckHR(pPushBuffer->BeginFixup(NULL, 0));
    PlayResources(pPushBuffer, 3);
    CheckHR(pPushBuffer->EndFixup());
    CheckHR(pPushBuffer->Verify(FALSE));

    // Finally, check that the results match:
    MemoryCompare(CopyBuffer, (VOID*) pPushBuffer->Data, pPushBuffer->Size - 4);

    // Do a pass with an empty fix-up object to see if anything falls over:
    CheckHR(g_pDev->CreateFixup(0, &pFixup));
    CheckHR(pPushBuffer->BeginFixup(pFixup, 0));
    PlayResources(pPushBuffer, 4);

    // With an empty fix-up buffer, we should always get1 a too small error (but
    // GetSize will still be valid):
    if (pPushBuffer->EndFixup() != D3DERR_BUFFERTOOSMALL)
        _asm int 3;
    CheckHR(pFixup->GetSize(&size1));
    SAFE_RELEASE(pFixup);

    // Try again with a humongous fix-up buffer to verify that the size is
    // the same:
    CheckHR(g_pDev->CreateFixup(4096*4, &pFixup));
    CheckHR(pPushBuffer->BeginFixup(pFixup, 0));
    PlayResources(pPushBuffer, 5);
    CheckHR(pPushBuffer->EndFixup());
    CheckHR(pFixup->GetSize(&size2));

    if (size1 != size2)
        _asm int 3;

    SAFE_RELEASE(pFixup);
	SAFE_RELEASE(pPushBuffer);


	return true;

cleanup:
	SAFE_RELEASE(pPushBuffer);
	SAFE_RELEASE(pFixup);
	return false;
}

//------------------------------------------------------------------------------
bool TestDrawVertices()
{
    DWORD i;
    WORD indexBuffer[4096];
    D3DPushBuffer* pPushBuffer;
    D3DFixup* pFixup;
    BYTE compileBuffer[32768];
    DWORD compileSize;

    for (i = 0; i < 4096; i++)
        indexBuffer[i] = (WORD) i;

    CheckHR( g_pDev->CreatePushBuffer(32768, FALSE, &pPushBuffer) );

    CheckHR( g_pDev->BeginPushBuffer(pPushBuffer) );
    CheckHR( g_pDev->DrawIndexedVertices(D3DPT_TRIANGLELIST, 5, indexBuffer) );
    CheckHR( g_pDev->EndPushBuffer() );

    compileSize = sizeof(compileBuffer);
    CheckHR( XGCompileDrawIndexedVertices(compileBuffer, &compileSize,
                                          D3DPT_TRIANGLELIST, 5, indexBuffer) );

    if ( compileSize != pPushBuffer->Size )
        _asm int 3;

    MemoryCompare( (VOID*) pPushBuffer->Data, compileBuffer, compileSize - 4 );

    CheckHR( g_pDev->BeginPushBuffer(pPushBuffer) );
    CheckHR( g_pDev->DrawIndexedVertices(D3DPT_LINELIST, 3000, indexBuffer + 3) );
    CheckHR( g_pDev->EndPushBuffer() );

    compileSize = sizeof(compileBuffer);
    CheckHR( XGCompileDrawIndexedVertices(compileBuffer, &compileSize,
                                          D3DPT_LINELIST, 3000, indexBuffer + 3) );

    // Ack, MikeSart's optimized DrawIndexedVertices no longer produces the
    // same signature, so make this '<=' and drop the MemoryCompare:

    if ( compileSize <= pPushBuffer->Size )
        _asm int 3;

    // MemoryCompare( (VOID*) pPushBuffer->Data, compileBuffer, compileSize - 4 );

    SAFE_RELEASE(pPushBuffer);

    CheckHR( g_pDev->CreateFixup(1024, &pFixup) );
    SAFE_RELEASE(pFixup);

	return true;

cleanup:
	SAFE_RELEASE(pPushBuffer);
	SAFE_RELEASE(pFixup);
	return false;
}

//------------------------------------------------------------------------------
void TestPushBuffers()
{
    TestDrawVertices(); //leaks 1-2 blocks per run
    TestFixups(); //leaks 26 blocks per run

	InitVB(); //leaks 3 per run
    for (DWORD i = 0; i < 20; i++)
    {
        Paint();
    }
    UnInitVB();
}

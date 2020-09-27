//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZBuffer.h"

//************************************************************************
// ZEnable Test functions

CZEnableTest::CZEnableTest()
{
	m_szTestName = TEXT("Z Buffer Enable");
	m_szCommandKey = TEXT("Enable");

	bEnable = true;
}

CZEnableTest::~CZEnableTest()
{
}

bool CZEnableTest::ExecuteTest(UINT uTestNum)
{
    UINT uTest = (uTestNum - 1) % m_uMaxTests + 1;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

    if (uTest == 1) {
        CSurface8* pd3ds;
        CSurface8* pd3dsRT;
        D3DSURFACE_DESC d3dsd;
        int nZBufferFormat = (uTestNum - 1) / m_uMaxTests;
        if (FAILED(m_pDevice->GetRenderTarget(&pd3dsRT))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderTarget failed"));
            SkipTests(m_uMaxTests);
            return false;
        }
        pd3dsRT->GetDesc(&d3dsd);
        pd3dsRT->Release();
        if (FAILED(m_pDevice->CreateDepthStencilSurface(d3dsd.Width, d3dsd.Height, m_fmtdCommon[nZBufferFormat], D3DMULTISAMPLE_NONE, &pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateDepthStencilSurface failed on format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            return false;
        }
    	msgString[0] = '\0';
	    sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
        WriteToLog(msgString);
        if (FAILED(m_pDevice->SetRenderTarget(NULL, pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed on depth format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            pd3ds->Release();
            return false;
        }
        pd3ds->Release();
    	msgString[0] = '\0';
    }

	// Let's build a triangle.
	DrawTriangle();

	// Set the appropriate renderstate
	switch (uTest)
	{
		case (1):
            // Force culling of triangle on Dx7 (Manbug #26983 not being fixed on Dx7)
//            if (m_dwVersion == 0x0700)
//                SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CW, CD3D_REF);

            // On Dx8, check for TL clip cap
            if (m_dwVersion >= 0x0800)
            {
            	DWORD dwPrimitiveMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

	            if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
	            {
		            WriteToLog("\nDevice capability not supported: Misc ClipTLVerts.");  
#ifndef DEMO_HACK
                    SkipTests(1);
		            return false;
#endif
	            }
            }

            sprintf(msgString, "%sZEnable: True, Green Triangle Visible: No", msgString);
			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_TRUE);
			break;
		case (2):
            // Reset culling to default value
//            if (m_dwVersion == 0x0700)
//                SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW, CD3D_REF);

			sprintf(msgString, "%sZEnable: False, Green Triangle Visible: Yes", msgString);
			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_FALSE);
            break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

//************************************************************************
// Internal API:    DrawTriangle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZEnableTest::DrawTriangle(void)
{
    D3DVECTOR v0 = {(float)m_vpTest.X + 115.0f, (float)m_vpTest.Y +  40.0f, 0.7f};
    D3DVECTOR v1 = {(float)m_vpTest.X + 205.0f, (float)m_vpTest.Y + 225.0f, 0.7f};
    D3DVECTOR v2 = {(float)m_vpTest.X +  25.0f, (float)m_vpTest.Y + 225.0f, 0.7f};

    // Green Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v0, (1.0f / 0.7f), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v1, (1.0f / 0.7f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v2, (1.0f / 0.7f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
}

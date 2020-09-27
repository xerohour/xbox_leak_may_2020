#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include <assert.h>
#include "VShader.h"

Fixed::Fixed()
{
    m_szTestName = TEXT("VSGen");
	m_szCommandKey = TEXT("VSGen");
}

Fixed::~Fixed()
{

}

UINT Fixed::TestInitialize(void)
{
    CShapes *pSphere;
    CShapes *pBox;
    PBYTE   pBuf;
    HRESULT hr;

    SetTestRange(1, 1);

    // Check for D3D version
    if (m_dwVersion < 0x0800)
    {   
        WriteToLog("VShader: Test requires DirectX version 8 or later.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // Check vertex shader support
    if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) < 1)
    {
        WriteToLog("VShader: Vertex shader version must be at least 1 for this test to run.\n");
        return D3DTESTINIT_SKIPALL;
    } 
    m_dwShaderVersion = m_d3dcaps.VertexShaderVersion;

    // Create shaders
	if (!CreateShaders())
    {
	    SetTestRange(1, 1);
        return D3DTESTINIT_ABORT;
    }
    m_dwTotalTests = m_dwShaders * 2;
m_dwTotalTests = 480;
    SetTestRange(1, m_dwTotalTests);


    // Create render object
    pSphere = new CShapes;
    if (NULL == pSphere)
    {
		WriteToLog("VShader: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	pSphere->NewShape(CS_SPHERE, 96);
    pBox = new CShapes;
    if (NULL == pBox)
    {
		WriteToLog("VShader: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	pBox->NewShape(CS_MESH, 100, 100);
	pBox->Rotate(-pi/2, 0.f, 0.f);
	pBox->Scale(1.5f, 0.1f, 1.5f);
	pBox->Translate(0.f, -0.5f, 0.f);
//	pBox->NewShape(CS_BOX);
//	pBox->Scale(1.5f, 0.1f, 1.5f);
//	pBox->Translate(0.f, -0.55f, 0.f);
    for (int i = 0; i < pBox->m_nIndices; i++)
        pBox->m_pIndices[i] = pBox->m_pIndices[i] + pSphere->m_nVertices;        

    // Fix mesh normals
	for (i = 0; i < pBox->m_nVertices; i++)
	{
		((VERTEX0*)pBox->m_pVertices)[i].nx = 0.f;
		((VERTEX0*)pBox->m_pVertices)[i].ny = 1.f;
		((VERTEX0*)pBox->m_pVertices)[i].nz = 0.f;
	}
	
	// Copy vertices / indices
    m_dwVertices = pSphere->m_nVertices + pBox->m_nVertices;
    m_dwIndices = pSphere->m_nIndices + pBox->m_nIndices;
    
    m_pSrcVB = CreateVertexBuffer(m_pDevice, NULL, sizeof(VERTEX0)*m_dwVertices);
//    m_pSrcVB = CreateVertexBuffer(0, sizeof(VERTEX0)*m_dwVertices, 0, false);
    if (NULL == m_pSrcVB)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
//    if (!m_pSrcVB->Lock((void**)&pBuf))
    hr = m_pSrcVB->Lock(0, sizeof(VERTEX0)*m_dwVertices, (LPBYTE*)&pBuf, 0);
    if (FAILED(hr))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pSphere->m_pVertices, pSphere->m_nVertices*sizeof(VERTEX0));
    memcpy(pBuf + pSphere->m_nVertices*sizeof(VERTEX0), pBox->m_pVertices, pBox->m_nVertices*sizeof(VERTEX0));
    m_pSrcVB->Unlock();
/*    
    m_pRefVB = CreateVertexBuffer(0, sizeof(VERTEX0)*m_dwVertices, 0, true);
    if (NULL == m_pRefVB)
    {
        WriteToLog("VShader: Could not create reference vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefVB->Lock((void**)&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pSphere->m_pVertices, pSphere->m_nVertices*sizeof(VERTEX0));
    memcpy(pBuf + pSphere->m_nVertices*sizeof(VERTEX0), pBox->m_pVertices, pBox->m_nVertices*sizeof(VERTEX0));
    m_pRefVB->Unlock();
*/
    m_pSrcIB = CreateIndexBuffer(m_pDevice, NULL, m_dwIndices * sizeof(WORD));
//    m_pSrcIB = CreateIndexBuffer(m_dwIndices, D3DFMT_INDEX16, false);
    if (NULL == m_pSrcIB)
    {
        WriteToLog("VShader: Could not create source index buffer\n");
        return D3DTESTINIT_ABORT;
    }
//    if (!m_pSrcIB->Lock((void**)&pBuf))
    hr = m_pSrcIB->Lock(0, m_dwIndices * sizeof(WORD), (LPBYTE*)&pBuf, 0);
    if (FAILED(hr))
    {
        WriteToLog("VShader: Could not lock source index buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pSphere->m_pIndices, pSphere->m_nIndices*sizeof(WORD));
    memcpy(pBuf + pSphere->m_nIndices*sizeof(WORD), pBox->m_pIndices, pBox->m_nIndices*sizeof(WORD));
    m_pSrcIB->Unlock();
/*    
    m_pRefIB = CreateIndexBuffer(m_dwIndices, D3DFMT_INDEX16, true);
    if (NULL == m_pRefIB)
    {
        WriteToLog("VShader: Could not create reference index buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefIB->Lock((void**)&pBuf))
    {
        WriteToLog("VShader: Could not lock reference index buffer");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pSphere->m_pIndices, pSphere->m_nIndices*sizeof(WORD));
    memcpy(pBuf + pSphere->m_nIndices*sizeof(WORD), pBox->m_pIndices, pBox->m_nIndices*sizeof(WORD));
    m_pRefIB->Unlock();
*/    
    delete pSphere;
    delete pBox;

	SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);

/*    
	// TEMP: user clipping plane
    D3DVALUE    maxX[4] = { 1.f,  0.f,  0.f, 0.495f};
    SetClipPlane(0, maxX);
	SetRenderState(D3DRS_CLIPPLANEENABLE, (DWORD)0x01);
*/

	return D3DTESTINIT_RUN;
}



bool Fixed::CreateShaders()
{
	DWORD   dwDecl[MAXSHADERSIZE];
    DWORD   dwFunc[MAXSHADERSIZE];
    DWORD   dwIndex;
	DWORD	dwShaderVersion = D3DVS_VERSION(1,0);
    HRESULT hr;

    m_dwTransformParts = 0;
	m_dwLightingParts = 0;
	m_dwTextureParts = 0;

    if (!CreateShaderParts())
        return false;

    for (int i = 0; i < (int)m_dwTransformParts; i++)
    {
        for (int j = 0; j < (int)m_dwLightingParts; j++)
        {
            // Determine shader version
			dwShaderVersion = (dwShaderVersion < m_vspTransform[i].dwVersion) ? m_vspTransform[i].dwVersion : dwShaderVersion;
			dwShaderVersion = (dwShaderVersion < m_vspLighting[i].dwVersion) ? m_vspLighting[i].dwVersion : dwShaderVersion;

			// Check if supported
			if ( (D3DSHADER_VERSION_MAJOR(dwShaderVersion) > D3DSHADER_VERSION_MAJOR(m_dwShaderVersion)) || 
				 (D3DSHADER_VERSION_MAJOR(dwShaderVersion) == D3DSHADER_VERSION_MAJOR(m_dwShaderVersion) && 
				  D3DSHADER_VERSION_MINOR(dwShaderVersion) > D3DSHADER_VERSION_MINOR(m_dwShaderVersion)) ) 
			{
				m_pShaders[m_dwShaders].dwHandle = 0;
				m_pShaders[m_dwShaders].dwVersion = dwShaderVersion;
				m_pShaders[m_dwShaders].bSupported = false;
				m_dwShaders++;
			}
			else
			{
				WriteDeclaration(dwDecl);
				dwIndex = 0;
				dwFunc[dwIndex++] = dwShaderVersion;
				memcpy(&dwFunc[dwIndex], m_vspTransform[i].dwCode, m_vspTransform[i].dwSize * sizeof(DWORD));
				dwIndex += m_vspTransform[i].dwSize;
				memcpy(&dwFunc[dwIndex], m_vspLighting[j].dwCode, m_vspLighting[j].dwSize * sizeof(DWORD));
				dwIndex += m_vspLighting[j].dwSize;
				dwFunc[dwIndex++] = D3DSIO_END;

				assert(m_dwShaders < MAXSHADERS);

//				if (!CreateVertexShader(dwDecl, dwFunc, &m_pShaders[m_dwShaders].dwHandle, 0))
                hr = m_pDevice->CreateVertexShaderTok(dwDecl, dwFunc, &m_pShaders[m_dwShaders].dwHandle, 0);
                if (FAILED(hr))
				{
					WriteToLog("VShader: Device failed to create vertex shader\n");
					return false;
				}
				m_pShaders[m_dwShaders].dwVersion = dwShaderVersion;
				m_pShaders[m_dwShaders].bSupported = true;
				m_dwShaders++;
			}
        }
    }
    return true;
}

/*******************************************

Constant registres:
	
	0-3		Projection matrix
	4-7 	View matrix
	8-11	World matrix
	12-15	World1 matrix
	16-19	World2 matrix
	20-23	World3 matrix
	24-27	Transformation matrix (World*View*Projection)
	28-31	Inverse transposed View matrix
	32-35	Inverse transposed World matrix
	36-39	Inverse transposed World1 matrix
	40-43	Inverse transposed World2 matrix
	44-47	Inverse transposed World3 matrix
	48-51	Inverse transposed WorldView matrix

* - all matrices needs to be transposed

Temporary registers:
	0		Transformed normal 

********************************************/

bool Fixed::CreateShaderParts()
{
    DWORD   dwIndex;
    DWORD   dwConst1			= 90;
    DWORD   dwConst2			= dwConst1 + 1;
    DWORD   dwMainMatrices		= dwConst1 + 2;
    DWORD   dwWorldMatrices		= dwConst1 + 3;
    DWORD   dwLightingMatrices	= dwConst1 + 4;
    DWORD   dwITWorldMatrices	= dwConst1 + 5;

    //************ Transformation parts *************

	// 0 - repro case
    dwIndex = 0;
    // Vertex transformation
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_Y | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 6 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 7 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 8 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 2 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 3 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 4 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 5 | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | 0 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	
	// View space vertex coordinates
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 25 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 26 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 27 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // View space normal
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 53 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 54 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,1);
    m_dwTransformParts++;

/*
    // 1 - single matrix multiply using DP4
    dwIndex = 0;
    // Vertex transformation
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 29 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 30 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // View space vertex coordinates
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 25 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 26 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 27 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // View space normal
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 53 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 54 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 2 - single matrix multiply using M4x4
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 3 - single matrix multiply using M4x3 and one DP4
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 4 - single matrix multiply using M3x4 and 4 MAD
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_X_X | D3DVS_Y_W | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 29 | D3DVS_X_X | D3DVS_Y_W | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 30 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 5 - single matrix multiply using M3x3, DP3 and single MAD
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_DP3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 29 | D3DVS_X_X | D3DVS_Y_W | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 30 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 6 - single matrix multiply using two M3x2, single MUL and ADD
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x2;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x2;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 30 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_X | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 28 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 29 | D3DVS_X_X | D3DVS_Y_W | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 30 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 31 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 7 - single matrix multiply using M4x4 and relative addresing
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwMainMatrices | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,1);
    m_dwTransformParts++;

    // 8 - 3 matrix multiply using M4x4 and two absolute and one relative addresing
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwMainMatrices | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,1);
    m_dwTransformParts++;

    // 9 - 3 matrix multiply using M4x4 and two relative addresing
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwMainMatrices | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 24 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,1);
    m_dwTransformParts++;

    // 10 - 2 matrix vertex blending
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MOV;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_ADDR | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwMainMatrices | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | D3DVS_ADDRMODE_RELATIVE | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT  | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst1 | D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 4 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst1 | D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP | D3DSPSM_NEG;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
	// r[0] already contains view space vertex coordinates
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,1);
    m_dwTransformParts++;

    // 11 - 2 matrix vertex blending
    dwIndex = 0;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 12 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAX;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT  | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST  | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_FRC;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 9 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAX;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 10 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT  | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DSPSM_NEG | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST  | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_FRC;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 11 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 10 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP | D3DSPSM_NEG;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
	// r[0] already contains view space vertex coordinates
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

    // 12 - 3 matrix vertex blending
    dwIndex = 0;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAX;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT  | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST  | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAX;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT  | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DSPSM_NEG | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST  | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_ADD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP  | 3 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST  | dwConst2 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 12 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 16 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MUL;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_MAD;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M4x4;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Pre-Lighting part
	// r[0] already contains view space vertex coordinates
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =  D3DSIO_M3x3;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwCode[dwIndex++] =      D3DSPR_CONST | 52 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspTransform[m_dwTransformParts].dwSize = dwIndex;
	m_vspTransform[m_dwTransformParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwTransformParts++;

	//************ Lighting parts *************
    // 1 - white directional light: ambient/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 72 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 74 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 2 - white directional light ambient/diffuse/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 72 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 74 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 71 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 3 - white directional light: specular
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	// Half way vector
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 88 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Specular
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LOG;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 75 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_EXP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_X | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 73 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 4 - white directional light ambient/diffuse/specular/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 72 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 74 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 71 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 88 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LOG;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 75 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_EXP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 73 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 5 - white directional light ambient/diffuse/specular/emissive (using LIT)
    dwIndex = 0;
	// N*L -> s.x
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// N*H ->s.y
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 58 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 88 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Power -> s.w
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 75 | D3DVS_W_X | D3DVS_OP;
    // Full lighting
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LIT;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;

	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 81 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_1 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 82 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 83 | D3DVS_NOSWIZZLE | D3DVS_OP;

	m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 6 - white point light: ambient/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 77 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 79 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 7 - white point light ambient/diffuse/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 77 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 79 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Light vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 64 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Diffuse
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 76 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 8 - white point light ambient/diffuse/emissive and attenuation
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 77 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 79 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Light vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 64 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Attenuation
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DST;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP4;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 66 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RCP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    // Diffuse
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 76 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;

    // 9 - white spot light ambient/diffuse/emissive
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 77 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 79 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Light vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 64 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Spot
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 65 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LOGP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_EXPP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// spot = 0 if rho < cos(phi/2)   
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_SGE;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z  | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// spot = 1 if rho > cos(theta/2)
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_SGE;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_SLT;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 70 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Diffuse
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 8 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 9 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 10 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 76 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;
*/
    // 10 - white point light: specular
    dwIndex = 0;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
	// Light vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 64 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// View vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 89 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Half way vector
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Specular
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LOGP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_3 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 80 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_EXPP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 78 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;
/*
    // 11 - white point light: ambient/diffuse/specular/emissive
    dwIndex = 0;
    // Ambient and emissive
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 77 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 79 | D3DVS_NOSWIZZLE | D3DVS_OP;
	
	// Light vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 64 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP;
	
	// Diffuse
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MOV;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | dwConst2 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 76 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP;
	
	// View vector
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 89 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Half way vector
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_ADD;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_RSQ;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 7 | D3DVS_NOSWIZZLE | D3DVS_OP;
	// Specular
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_DP3;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MAX;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 11 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_LOGP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_0 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 80 | D3DVS_NOSWIZZLE | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_EXPP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DSP_WRITEMASK_2 | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_W_X | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =   D3DSIO_MUL;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_ATTROUT | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_TEMP | 6 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP;
    m_vspLighting[m_dwLightingParts].dwCode[dwIndex++] =      D3DSPR_CONST | 78 | D3DVS_NOSWIZZLE | D3DVS_OP;
	m_vspLighting[m_dwLightingParts].dwSize = dwIndex;
	m_vspLighting[m_dwLightingParts].dwVersion = D3DVS_VERSION(1,0);
    m_dwLightingParts++;
*/
    return true;
}

bool Fixed::WriteDeclaration(DWORD *pdwDecl)
{
    #define     DECL(x)     { pdwDecl[dwIndex++] = (x); }
    #define     DECL4(x)   DECL((*(DWORD*)&(x[0]))) DECL((*(DWORD*)&(x[1]))) DECL((*(DWORD*)&(x[2]))) DECL((*(DWORD*)&(x[3])))
    
    float   fConst1[4] = { 0.1f, 0.2f, 0.3f, 0.5f };
    float   fConst2[4] = { 0.f, 1.f, 2.f, 10.f };
    float   fMainMatrices[4] = { 0.f, 4.f, 8.f, 28.f };
    float   fWorldMatrices[4] = { 8.f, 12.f, 16.f, 20.f };
	float	fLightMatrices[4] = { 24.f, 32.f, 36.f, 52.f };
	float	fITWorldMatrices[4] = { 36.f, 40.f, 44.f, 48.f };
    DWORD   dwIndex = 0;

    DECL( D3DVSD_STREAM(0) )
    DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
    DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
    DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
    DECL( D3DVSD_CONST(90, 6) )
    DECL4( fConst1 )
    DECL4( fConst2 )
    DECL4( fMainMatrices )
    DECL4( fWorldMatrices )
    DECL4( fLightMatrices )
    DECL4( fITWorldMatrices )
    DECL( D3DVSD_END() )

    #undef DECL
    return true;
}

struct V4
{
	float	x;
	float	y;
	float	z;
	float	w;

	operator= (D3DVECTOR v) {x = v.x; y = v.y; z = v.z; w = 0.f; return 0;} 
};

D3DVECTOR Normalize2(D3DVECTOR _v)
{
	D3DVECTOR	v;
	float m = 1.f/(float)sqrt(_v.x*_v.x + _v.y*_v.y + _v.z*_v.z);
	v.x = _v.x*m;
	v.y = _v.y*m;
	v.z = _v.z*m;
	return v;
}


bool Fixed::SetCurrentShaderConstants()
{
	float		fZn = 0.1f;
	float		fZf = 10.f;
	D3DVECTOR   from; 
    D3DVECTOR	at;
    D3DVECTOR	up;
    D3DMATRIX   Projection;
    D3DMATRIX   View;
    D3DMATRIX   World, World1, World2;
    D3DMATRIX   WorldView;
    D3DMATRIX   Full;
    D3DMATRIX   M;
	D3DVECTOR	Lp0;
	D3DVECTOR	Ld0;
	D3DVECTOR	v3;
	D3DVECTOR	Lp1;
	D3DVECTOR	Ld1;
	V4			V;
	float		att1[4] = {0.8f, 1.0f, 1.5f, 0.f}; 
	float		phi1 = pi/1.3f;
	float		theta1 = pi/4.f;
	float		fallof1 = 0.4f;

    from.x = 0.f;		from.y = 1.f;		from.z = -2.f;
    at.x = 0.f;			at.y = 0.f;			at.z = 0.f;
    up.x = 0.f;			up.y = 1.f;			up.z = 0.f;

	Lp0.x = 1.f;		Lp0.y = 1.f;		Lp0.z = -2.f;
	Ld0.x = 0.408248f;	Ld0.y = 0.408248f;	Ld0.z = -0.815497f;
	Lp1.x = -0.6f;		Lp1.y = -0.2f;		Lp1.z = -0.4f;
	Ld1.x = -0.577350f;	Ld1.y = 0.577350f;	Ld1.z = -0.577350f;

    // Projection matrix
    Projection = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
    M = MatrixTranspose(Projection);
    if (!SetVertexShaderConstant(0, &M, 4))
        return false;

    // View matrix
    View = ViewMatrix(from, at, up);
    M = MatrixTranspose(View);
    if (!SetVertexShaderConstant(4, &M, 4))
        return false;

    // World matrix
    World = RotateYMatrix(float(m_dwCurrentTestNumber)*0/20.f);
    M = MatrixTranspose(World);
    if (!SetVertexShaderConstant(8, &M, 4))
        return false;

    // Second World matrix
    World1 = MatrixMult(ScaleMatrix(0.5f, 1.f, 0.5f), World);
    M = MatrixTranspose(World1);
    if (!SetVertexShaderConstant(12, &M, 4))
        return false;

    // Third World matrix
    World2 = MatrixMult(ScaleMatrix(2.0f, 1.0f, 2.0f), World);
    M = MatrixTranspose(World2);
    if (!SetVertexShaderConstant(16, &M, 4))
        return false;

    // WorldView matrix
    WorldView = MatrixMult(View, World);
    M = MatrixTranspose(WorldView);
    if (!SetVertexShaderConstant(24, &M, 4))
        return false;
    
    WorldView = MatrixMult(View, World);
    Full = MatrixMult(Projection, WorldView);
    M = MatrixTranspose(Full);
    if (!SetVertexShaderConstant(28, &M, 4))
        return false;

	// Inverse transposed View matrix 
	// Skip transpose here because matrix in constant 
	// register needs to be transposed
	M = MatrixInverse(View);
    if (!SetVertexShaderConstant(32, &M, 4))
        return false;

	// Inverse transposed World matrix
	M = MatrixInverse(World);
    if (!SetVertexShaderConstant(36, &M, 4))
        return false;

	// Inverse transposed World1 matrix
	M = MatrixInverse(World1);
    if (!SetVertexShaderConstant(40, &M, 4))
        return false;

	// Inverse transposed World2 matrix
	M = MatrixInverse(World2);
    if (!SetVertexShaderConstant(44, &M, 4))
        return false;

	// Inverse transposed WorldView matrix
	M = MatrixInverse(WorldView);
    if (!SetVertexShaderConstant(52, &M, 4))
        return false;


	// ******************* Lighting parameters

	// Light0 position
    V = TransformVector(Lp0, View);
	if (!SetVertexShaderConstant(57, &V, 1))
        return false;

	// Light0 direction
//    v3 = TransformVector(Ld, View);
	v3.x = Ld0.x*View._11 + Ld0.y*View._21 + Ld0.z*View._31;
	v3.y = Ld0.x*View._12 + Ld0.y*View._22 + Ld0.z*View._32;
	v3.z = Ld0.x*View._13 + Ld0.y*View._23 + Ld0.z*View._33;
	V = Normalize2(v3);
	if (!SetVertexShaderConstant(58, &V, 1))
        return false;

	// Light1 position
    V = TransformVector(Lp1, View);
	if (!SetVertexShaderConstant(64, &V, 1))
        return false;

	// Light1 direction
//    v3 = TransformVector(Ld, View);s
	v3.x = Ld1.x*View._11 + Ld1.y*View._21 + Ld1.z*View._31;
	v3.y = Ld1.x*View._12 + Ld1.y*View._22 + Ld1.z*View._32;
	v3.z = Ld1.x*View._13 + Ld1.y*View._23 + Ld1.z*View._33;
	V = Normalize2(v3);
	if (!SetVertexShaderConstant(65, &V, 1))
        return false;
	// Light1 attenuation
	if (!SetVertexShaderConstant(66, att1, 1))
        return false;
	
	// Light1 spot
	V.x = (float)cos(phi1/2);
	V.y = (float)cos(theta1/2);
	V.z = 1.f / (V.y - V.x);
	V.w = fallof1;
	if (!SetVertexShaderConstant(70, &V, 1))
        return false;

	// ******************* Material parameters
	float	Material[2*5*4] = 
	{
		// Material 0
		0.751640f, 0.606480f, 0.226480f, 1.f,	// diffuse
		0.247250f, 0.199500f, 0.074500f, 1.f,	// ambient
		0.797357f, 0.723991f, 0.208006f, 1.f,	// specular
		0.000000f, 0.000000f, 0.000000f, 1.f,	// emissive
		51.20000f, 51.20000f, 51.20000f, 1.f,	// power

		// Material 1
		0.400000f, 0.400000f, 0.400000f, 1.f,	// diffuse
		0.250000f, 0.250000f, 0.250000f, 1.f,	// ambient
		0.774597f, 0.774597f, 0.774597f, 1.f,	// specular
		0.000000f, 0.000000f, 0.000000f, 1.f,	// emissive
		76.80000f, 76.80000f, 76.80000f, 1.f,	// power
	};
	if (!SetVertexShaderConstant(71, Material, 10))
        return false;
	float	MaterialColor[2*4*4-1] = 
	{
		// Material 0
		// ambient diffuse    specular   emissive
		0.247250f, 0.751640f, 0.797357f, 0.f,	// red
		0.199500f, 0.606480f, 0.723991f, 0.f,	// green
		0.074500f, 0.226480f, 0.208006f, 0.f,	// blue
		0.f,       0.f,       0.f,       1.f,	// alpha
		
		// Material 1
		0.250000f, 0.400000f, 0.774597f, 0.f,	// red
		0.250000f, 0.400000f, 0.774597f, 0.f,	// green
		0.250000f, 0.400000f, 0.774597f, 0.f,	// blue
	};
	if (!SetVertexShaderConstant(81, MaterialColor, 7))
        return false;

	// ******************* Camera
    float nlv[4] = {0.f, 0.f, -1.f, 0.f };
	if (!SetVertexShaderConstant(88, nlv, 1))
        return false;
	V = from;
	if (!SetVertexShaderConstant(89, &V, 1))
        return false;

	return true;
}



#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "statebeast.h"
//#include "d3dx8.h"
//#include "Types.h"

struct V4
{
	float	x;
	float	y;
	float	z;
	float	w;

	operator= (D3DVECTOR v) {x = v.x; y = v.y; z = v.z; w = 0.f; } 
};

DWORD decl[] =
{	D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(1, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_FLOAT2),
	D3DVSD_END()
};

// ----------------------------------------------------------------------------

void CStateBeast::SetDefaultShader()
{
    SetVertexShader(m_dwFVF);
}

// ----------------------------------------------------------------------------

void CStateBeast::PostApply8(UINT nBlock)
{
	DWORD dwSBT = m_pBlocks[m_nCurrentBlock].dwCreateType;

	switch(nBlock)
	{
	case 0:
		{
            D3DXMATRIX mat;
			D3DXMatrixMultiply( &mat, (D3DXMATRIX*)m_pVMatrix, (D3DXMATRIX*)m_pProjMatrix );
			D3DXMatrixTranspose( &mat, &mat );
			SetVertexShaderConstant(0, &mat, 4);
			SetVertexShader(m_dwVSShader, CD3D_REF);
            SetVertexShader(m_dwVSShader, CD3D_SRC);
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
		}
		break;
	case 1:
		{
            D3DXMATRIX mat;
			D3DXMatrixMultiply( &mat, (D3DXMATRIX*)m_pVMatrix, (D3DXMATRIX*)m_pProjMatrix );
			D3DXMatrixTranspose( &mat, &mat );
			SetVertexShaderConstant(0, &mat, 4);
			V4 v ={ 1.f, 1.f, 0.f, 1.f };
            SetVertexShaderConstant(4, &v, 1);
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
		}
		break;
	case 2:
		{
            D3DXMATRIX mat;
			D3DXMatrixMultiply( &mat, (D3DXMATRIX*)m_pVMatrix, (D3DXMATRIX*)m_pProjMatrix );
			D3DXMatrixTranspose( &mat, &mat );
			SetVertexShaderConstant(0, &mat, 4);
			V4 v ={ 1.f, 0.f, 0.f, 1.f };
            SetVertexShaderConstant(4, &v, 1);
			SetVertexShader(m_dwVSShader, CD3D_REF);
            SetVertexShader(m_dwVSShader, CD3D_SRC);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
			if(dwSBT == D3DSBT_PIXELSTATE || dwSBT == D3DSBT_VERTEXSTATE)
			{
				UINT uStride;
				CVertexBuffer8* pVB;
//				GetStreamSource8(0, &pVB, &uStride, NULL, NULL, CD3D_SRC);
                m_pDevice->GetStreamSource(0, &pVB, &uStride);
				if(pVB != NULL)
				{
					Fail();
					WriteToLog(_T("ExecuteTest() - SetStreamSource shouldn't be captured by this block.\n"));
				}
				SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
			}
		}
		break;
	case 3:
		{
            D3DXMATRIX mat;
			D3DXMatrixMultiply( &mat, (D3DXMATRIX*)m_pVMatrix, (D3DXMATRIX*)m_pProjMatrix );
			D3DXMatrixTranspose( &mat, &mat );
			SetVertexShaderConstant(0, &mat, 4);
			V4 v ={ 1.f, 0.f, 1.f, 1.f };
            SetVertexShaderConstant(4, &v, 1);
			SetVertexShader(m_dwVSShader, CD3D_REF);
            SetVertexShader(m_dwVSShader, CD3D_SRC);
            SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
		    if(dwSBT == D3DSBT_PIXELSTATE || dwSBT == D3DSBT_VERTEXSTATE)
			{
				UINT uIndex;
				CIndexBuffer8* pIB;
//				GetIndices8(&pIB, &uIndex, NULL, NULL, CD3D_SRC);
                m_pDevice->GetIndices(&pIB, &uIndex);
				if(pIB != NULL || uIndex != 0)
				{
					Fail();
					WriteToLog(_T("ExecuteTest() - SetIndices shouldn't be captured by this block.\n"));
				}
				SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
			}
		}
		break;
	case 4:
		{
			m_pDevice->SetPixelShader(m_dwPSShader);
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
		}
		break;
	case 5:
		{
			V4 v ={ 0.f, 1.f, 1.f, 1.f };
#ifdef UNDER_XBOX
			m_pDevice->SetPixelShader(m_dwPSShader);
#endif
			m_pDevice->SetPixelShaderConstant(0, &v, 1);
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
		}
		break;
	case 6:
		{
			m_pDevice->SetMaterial(&m_FrontMat);
			SetRenderState(D3DRS_AMBIENT, (DWORD)0xFFFFFFFF);
			SetRenderState(D3DRS_FILLMODE, (DWORD) D3DFILL_SOLID);
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), CD3D_BOTH);
            SetIndices(m_pSrcIB, m_pRefIB, 0, CD3D_BOTH);
			SetVertexShader(m_dwFVF);
		}
	default:
		break;
	}
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetState8(int nIndex, TCHAR *szName, int *nVersion, DWORD dwFlags)
{
//	DEVICEDESC SrcDesc = m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;
//    UINT n = m_pMode->nSrcDevice;
	if(nVersion) *nVersion = 8;

	switch(nIndex)
	{
	case 0:
		if(szName) _tcscpy(szName, _T("SetVertexShaderConstant"));
        if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) >= 1)
		{
			V4 v ={ 0.f, 1.f, 0.f, 1.f };
            SetVertexShaderConstant(4, &v, 1, dwFlags);
			return true;
		}
		break;
	case 1:
		if(szName) _tcscpy(szName, _T("SetVertexShader"));
        if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) >= 1)
		{
			SetVertexShader(m_dwVSShader, dwFlags);
			return true;
		}
		break;
	case 2:
		if(szName) _tcscpy(szName, _T("SetStreamSource"));
        if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) >= 1)
		{
			SetStreamSource(0, m_pSrcVB, m_pRefVB, GetVertexSize(m_dwFVF), dwFlags);
			return true;
		}
		break;
	case 3:
		if(szName) _tcscpy(szName, _T("SetIndices"));
        if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) >= 1)
		{
			SetIndices(m_pSrcIB, m_pRefIB, 0, dwFlags);
			return true;
		}
		break;
	case 4:
        if(szName) _tcscpy(szName, _T("SetPixelShaderConstant"));
		if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.PixelShaderVersion) >= 1)
		{
			V4 v ={ 1.f, 1.f, 1.f, 1.f };
#ifdef UNDER_XBOX
			m_pDevice->SetPixelShader(m_dwPSShader);//, dwFlags);
#endif
			m_pDevice->SetPixelShaderConstant(0, &v, 1);//, dwFlags);
			return true;
		}
		break;
	case 5:
        if(szName) _tcscpy(szName, _T("SetPixelShader"));
		if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.PixelShaderVersion) >= 1)
		{
			m_pDevice->SetPixelShader(m_dwPSShader);//, dwFlags);
			return true;
		}
		break;
	case 6:
		if(szName) _tcscpy(szName, _T("Toggle D3DRS_SOFTWAREVERTEXPROCESSING"));
#ifndef UNDER_XBOX
		if(m_d3dcaps.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
		{
			if(m_pAdapter->Devices[m_pMode->nSrcDevice].bSoftwareVertexProcessing)
				SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD)FALSE, dwFlags);
			else
				SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD)TRUE, dwFlags);
			return true;
		}
#endif
		break;
	default:
		break;
	}

	return false;
}

// ----------------------------------------------------------------------------

void CStateBeast::ClearState8()
{
#ifndef UNDER_XBOX
	if(m_d3dcaps.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
	{
		if(m_pAdapter->Devices[m_pMode->nSrcDevice].bSoftwareVertexProcessing)
			SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD)TRUE);
		else
			SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD)FALSE);
	}
#endif
}

// ----------------------------------------------------------------------------

bool CStateBeast::Init8()
{
	HRESULT hr;
//    DEVICEDESC SrcDesc = m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;

    m_dwFVF = D3DFVF_VERTEX;
#ifndef UNDER_XBOX
    m_nTotalDX8 = 7;
#else
    m_nTotalDX8 = 6;
#endif
	
	// Create the vertex shader
	if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) >= 1)
	{
		char *szShader = "vs.1.0\n"
						 "dp4 oPos.x, v0, c0\n"
						 "dp4 oPos.y, v0, c1\n"
						 "dp4 oPos.z, v0, c2\n"
						 "dp4 oPos.w, v0, c3\n"
						 "mov oD0, c4\n";
#ifndef UNDER_XBOX
        LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::Init8 -- D3DXAssembleShader(vertex) failed, hr=%X\n"),
						hr);
			return false;
		}		
		CreateVertexShader(decl, (DWORD*)pShader->GetBufferPointer(), &m_dwVSShader, NULL);

        if(pShader)
			pShader->Release();
#else
        m_pDevice->CreateVertexShaderAsm(decl, szShader, strlen(szShader), &m_dwVSShader, 0);
#endif // UNDER_XBOX
	}

	if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.PixelShaderVersion) >= 1)
	{
		// Create the pixel shader
		char *szShader = "ps.1.0\n"
						 "add r0, v0, c0\n";
#ifndef UNDER_XBOX
		LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::Init8 -- D3DXAssembleShader(pixel) failed, hr=%X\n"),
						hr);
			return false;
		}
    		
		
		if(FAILED(m_pDevice->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &m_dwPSShader)))
		{
			WriteToLog(_T("CStateBeast::Init8 -- CreatePixelShader failed\n"));
			return false;
		}
            
		if(pShader)
			pShader->Release();
#else
        m_pDevice->CreatePixelShaderAsm(szShader, strlen(szShader), &m_dwPSShader);
#endif // UNDER_XBOX
	}

	SetDefaultShader();
    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetupBuffers(CShapes *pShapes, CVertexBuffer8 **ppVB, CIndexBuffer8 **ppIB, bool bReference)
{
    PVOID pBuffer = NULL;
	HRESULT hr = S_OK;
    
	// Setup the vertex buffer
//	*ppVB = CreateVertexBuffer(m_dwFVF, pShapes->m_nVertices, 0, bReference, D3DUSAGE_DYNAMIC, D3DPOOL_SYSTEMMEM);
    *ppVB = CreateVertexBuffer(m_pDevice, pShapes->m_pVertices, pShapes->m_nVertices * GetVertexSize(m_dwFVF), 0, m_dwFVF);    
	if(*ppVB == NULL)
	{
		WriteToLog(_T("SetupBuffers() - CreateVertexBuffer failed.\n"));
		return false;
	}
/*
    hr = (*ppVB)->Lock(&pBuffer);
	if(FAILED(hr))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock VB. hr=%s\n"), GetHResultString(hr).c_str());
		(*ppVB)->Release();
		return false;
	}
    
	PBYTE pPos = (PBYTE)pBuffer;
	int nSize = GetVertexSize(m_dwFVF);
    for(int i=0; i<pShapes->m_nVertices; i++)
	{
		memcpy(pPos, &(pShapes->m_pVertices[i]), nSize);
		pPos += nSize;
	}
	
    (*ppVB)->Unlock();
*/

	// Setup the index buffer
//	*ppIB = CreateIndexBuffer(pShapes->m_nIndices, D3DFMT_INDEX16, bReference, D3DUSAGE_DYNAMIC, D3DPOOL_SYSTEMMEM);
    *ppIB = CreateIndexBuffer(m_pDevice, pShapes->m_pIndices, pShapes->m_nIndices * sizeof(WORD));
    if(*ppIB == NULL)
	{
		WriteToLog(_T("SetupBuffers() - CreateIndexBuffer failed.\n"));
		(*ppVB)->Release();
		return false;
	}
/*
    hr = (*ppIB)->Lock(&pBuffer);
	if(FAILED(hr))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock IB. hr=%s\n"), GetHResultString(hr).c_str());
		(*ppVB)->Release();
        (*ppIB)->Release();
		return false;
	}
	
    memcpy(pBuffer, pShapes->m_pIndices, pShapes->m_nIndices*sizeof(WORD));
    (*ppIB)->Unlock();
*/
	return true;
}

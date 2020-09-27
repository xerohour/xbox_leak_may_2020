/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

Ball.cpp

Abstract:

spark (from ball collisions

Author:

Jason Gould (jgould) 18-May-2001

Revision History:

18-May-2001 jgould
	Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <xtl.h>
#include <xdbg.h>
#include <xgraphics.h>
#include "spark.h"
#include "helpers.h"

using namespace Media4;

namespace Media4 {

//D3DFVF_XYZRHW
#ifndef PI 
#	define PI 3.14159265359f
#endif
#define M_PI PI
#define M_2PI 6.28318530718f
#define SAFE_RELEASE(x) do { if (x != NULL) { x->Release(); x = NULL; } } while(0)
#define FRAND(Min, Max) ((float)rand() / (float)RAND_MAX * (Max - Min) + Min)
#define FABS(x)     ((x) < 0.0f ? -(x) : (x))

D3DVertexBuffer* CSpark::m_pVB = NULL;

HRESULT CSpark::Create (IN D3DDevice* pDevice) {
	m_Next = 0;
	m_First = 0;
	HRESULT hr;

	if(m_pVB == NULL) {
		hr = pDevice->CreateVertexBuffer(g_TestParams.dwMaxSparks * 4*sizeof(float), 0, 0, D3DPOOL_DEFAULT, &m_pVB);
	}
	if(!FAILED(hr)) {
		//This gets us our pointer for adding new sparks. Note: nothing keeps track of what is locked...
		hr = m_pVB->Lock(0,0,(BYTE**)&m_pVertex,D3DLOCK_NOOVERWRITE); 
	}
	if(!FAILED(hr)) {
		m_pDirection = new XGVECTOR3[g_TestParams.dwMaxSparks];
		m_pSpeed = new float[g_TestParams.dwMaxSparks];
		if(!m_pDirection || !m_pSpeed) hr = E_OUTOFMEMORY;
	}
	if(FAILED(hr)) {
		Release();
	}

	return hr;
}

void CSpark::Release() {
	SAFE_RELEASE(m_pVB);
	delete[] m_pDirection;
	delete[] m_pSpeed;
}

void CSpark::AddSpark(XGVECTOR3& loc) {
	if(m_Next + 1 == m_First || m_Next + 1 - g_TestParams.dwMaxSparks == m_First) {
		return;
	}
	

	m_pVertex[m_Next].age = 0;
	m_pVertex[m_Next].location = loc;
	m_pDirection[m_Next].x = FRAND(-1.0f, 1.0f);//1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
	m_pDirection[m_Next].y = FRAND(-1.0f, 1.0f);//1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
	m_pDirection[m_Next].z = FRAND(-1.0f, 1.0f);//1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
	XGVec3Normalize(&m_pDirection[m_Next], &m_pDirection[m_Next]);
	m_pSpeed[m_Next] = FRAND(.001f, .01f);//g_TestParams.fMinSpeed, g_TestParams.fMaxSpeed);
	//(float)rand() / (float)RAND_MAX * (g_TestParams.fMaxSpeed - g_TestParams.fMinSpeed) + g_TestParams.fMinSpeed;
	m_Next = (m_Next + 1) % g_TestParams.dwMaxSparks;
}

//all sparks live the same length of time, so the first one created is the first one dead. 
//Just incriment "first", to point to the next living spark.
void CSpark::RemoveSpark() {
	m_First = (m_First + 1) % g_TestParams.dwMaxSparks;
}

void CSpark::Update(IN float amount) {
	UINT i;
	amount *= 200; //sparks will live about half a second, with maxage==100
	for(i = m_First; i != m_Next; i = (i + 1) % g_TestParams.dwMaxSparks) {
		m_pVertex[i].age += amount;
		if(m_pVertex[i].age > g_TestParams.dwSparkMaxAge) {
			RemoveSpark();
			continue;
		} 

		//no collision detection... they don't live that long anyway...
		m_pVertex[i].location += m_pDirection[i] * m_pSpeed[i] * amount;
	}
}

void CSpark::Render(
	D3DDevice* pDevice,
	XGMATRIX*  pmViewProj
	) 
{
	if(m_First == m_Next) {
		return;
	}
	XGMATRIX id;
   	pDevice->SetTransform(D3DTS_WORLD, XGMatrixIdentity(&id));
	XGMATRIX mtViewProj;
    XGMatrixTranspose(&mtViewProj, pmViewProj);

    pDevice->SetVertexShaderConstant(0, &mtViewProj, 4);

//    pDevice->SetVertexShader(D3DFVF_XYZ);


    pDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, rand()+ (rand() << 16));

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

    pDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	float m_radius = 0.04f;
    pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(m_radius * 2.0f));

//        pDevice->SetTexture(3, m_pd3dtLight);
	
	pDevice->SetStreamSource(0,m_pVB, sizeof(Vertex));

	if(m_First < m_Next) {
		pDevice->DrawPrimitive(D3DPT_POINTLIST, m_First, m_Next-m_First);
	} else {
		if(m_Next) pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_Next);
		pDevice->DrawPrimitive(D3DPT_POINTLIST, m_First, g_TestParams.dwMaxSparks - m_First);
	}
//        pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &vOrigin, sizeof(XGVECTOR3)+sizeof(float));

    pDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_POINT);
    pDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_POINT);

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);

    pDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
    pDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

}

int CSpark::GetNumSparks()
{
	int i = m_First>m_Next ? m_Next-m_First +g_TestParams.dwMaxSparks: m_Next-m_First;
	return i;
}


D3DLIGHT8* CSpark::GetLight(int iSpark)
{
	if(m_LastLight == iSpark) {
		return &m_light;
	}
	
	m_LastLight = iSpark;
	int index = (iSpark + m_First) % g_TestParams.dwMaxSparks;

	XGVECTOR3 vColor, vSpecular, vOrange (1.0f, 0.25f, 0.0f);

    XGVec3Normalize(&vColor, &(FRAND(0.6f, 1.0f) * vOrange));
    XGVec3Normalize(&vSpecular, &XGVECTOR3(vColor.x + 0.2f, vColor.y + 0.2f, vColor.z + 0.2f));

    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_POINT;
    m_light.Diffuse.r = vColor.x * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Diffuse.g = vColor.y * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Diffuse.b = vColor.z * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Diffuse.a = 1.0f;//FRND(1.0f);
    m_light.Specular.r = vSpecular.x * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Specular.g = vSpecular.y * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Specular.b = vSpecular.z * (1 - (m_pVertex[index].age / g_TestParams.dwSparkMaxAge));
    m_light.Specular.a = 1.0f;
    m_light.Range = 100.0f;
//    m_light.Attenuation0 = 1.0f;
//    m_light.Attenuation1 = 1.0f;
//    m_light.Attenuation2 = 2.0f;
    m_light.Attenuation0 = 0.0f;
    m_light.Attenuation1 = 0.0f;
    m_light.Attenuation2 = 6.4f + (float)rand() / (float)RAND_MAX * 3.2f;

    m_light.Position = m_pVertex[index].location;
	
	return &m_light;

}

}

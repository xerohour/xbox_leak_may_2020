/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Ball.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001 robheit
		Initial Version

    28-April-2001 danrose
	    Added sound

    21-June-2001 danhaff
        Made lights flash with beat of music.

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <xtl.h>
#include <xdbg.h>
#include <xgraphics.h>
#include "Ball.h"
#include "helpers.h"

using namespace Media4;

namespace Media4 {
//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define XYZ_NORMAL (D3DFVF_XYZ | D3DFVF_NORMAL)
#ifndef PI 
#	define PI 3.14159265359f
#endif
#define M_PI PI
#define M_2PI 6.28318530718f

#define FABS(x)     ((x) < 0.0f ? -(x) : (x))

#define AMBIENT_SOUND_FREQUENCY 26000

//------------------------------------------------------------------------------
//	Static member initialization
//------------------------------------------------------------------------------

UINT CBall::m_uRef = 0;
IDirect3DTexture8* CBall::m_pd3dtBase = NULL;
IDirect3DTexture8* CBall::m_pd3dtBump = NULL;
IDirect3DTexture8* CBall::m_pd3dtLight = NULL;
IDirect3DTexture8* CBall::m_pd3dtWhite = NULL;
IDirect3DTexture8* CBall::m_pd3dtFlat = NULL;
CAudioData CBall::m_CollisionSoundData;
CAudioData CBall::m_AmbientSoundData;

//------------------------------------------------------------------------------
//	CBall::CBall
//------------------------------------------------------------------------------
CBall::CBall(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_dontDraw	            = TRUE;
	m_pVB		            = NULL;
	m_pIB		            = NULL;
	m_pSubMix               = NULL;
	m_pAmbientSound         = NULL;
	m_pAmbientSoundData     = NULL;
	m_pCollisionSound	    = NULL;
	m_pCollisionSoundData   = NULL;
	m_fillMode	            = D3DFILL_SOLID;
	m_mass		            = 0;
    m_dwCollisionFrequency  = 0;
    m_uRef++;

    ZeroMemory( &m_dwInLightRange, 4 * sizeof(DWORD));
}

//------------------------------------------------------------------------------
//	CBall::~CBall
//------------------------------------------------------------------------------
CBall::~CBall(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	Release();

    if (--m_uRef == 0) {
        if (m_pd3dtBase) {
            m_pd3dtBase->Release();
            m_pd3dtBase = NULL;
        }
        if (m_pd3dtBump) {
            m_pd3dtBump->Release();
            m_pd3dtBump = NULL;
        }
        if (m_pd3dtLight) {
            m_pd3dtLight->Release();
            m_pd3dtLight = NULL;
        }
        if (m_pd3dtWhite) {
            m_pd3dtWhite->Release();
            m_pd3dtWhite = NULL;
        }
        if (m_pd3dtFlat) {
            m_pd3dtFlat->Release();
            m_pd3dtFlat = NULL;
        }
    }
}

//------------------------------------------------------------------------------
//	CBall::Create
//------------------------------------------------------------------------------
HRESULT
CBall::Create(
			  IN IDirect3DDevice8*	pDevice,
			  IN float				radius,
			  IN float				mass,
			  IN int				numLong,
			  IN int				numLat,
              IN BOOL               bLight,
              IN DWORD              dwID
			  )
/*++

Routine Description:

	Creats a sphere whose center is at the origin with a given radius

Arguments:

	IN pDevice -	D3D Device
	IN radius -		Radius of sphere
	IN numLong -	Number of longitutinal lines
	IN numLat -		Num latitutinal lines

Return Value:

	S_OK on success, any other value on failure

--*/
{
	HRESULT		hr;
	float		phi;
	float		rho;
	float		phiInc;
	float		rhoInc;
	int			p;
	int			r;
	UINT		index       = 0;
	VOID*		pVoid		= NULL;
	Vertex*		vertices	= NULL;
	WORD*		indices		= NULL;
	int			latitude1	= numLat - 1;
	int			latitude2	= numLat - 2;
	int			longitude1	= numLong - 1;
	int			p1l;
	int			pl;
	int			last;
    int         i, j;
    float       fX, fY, fTX, fSinY, fCosY;
    Vertex*     pr;


	// Release previous buffers
	Release();

	// Setup
    m_dwID      = dwID;
	m_radius	= radius;
	m_detail	= numLat;
	m_mass		= mass; // 4.0f / 3.0f * PI * m_radius * m_radius * m_radius;

    m_bLightSource = bLight;

    if (bLight) {

        XGVECTOR3 vColor, vSpecular;

        if (g_TestParams.bTextureBalls) {
            XGVec3Normalize(&vColor, &XGVECTOR3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX));
            XGVec3Normalize(&vSpecular, &XGVECTOR3(vColor.x + 0.2f, vColor.y + 0.2f, vColor.z + 0.2f));
        }
        else {
            vColor = XGVECTOR3(1.0f, 1.0f, 1.0f);
            vSpecular = XGVECTOR3(1.0f, 1.0f, 1.0f);
        }

        memset(&m_light, 0, sizeof(D3DLIGHT8));
        m_light.Type = D3DLIGHT_POINT;
        m_light.Diffuse.r = vColor.x;
        m_light.Diffuse.g = vColor.y;
        m_light.Diffuse.b = vColor.z;
        m_light.Diffuse.a = 1.0f;//FRND(1.0f);
        m_light.Specular.r = vSpecular.x;
        m_light.Specular.g = vSpecular.y;
        m_light.Specular.b = vSpecular.z;
        m_light.Specular.a = 1.0f;
        m_light.Ambient.r = 0.0f;
        m_light.Ambient.g = 0.0f;
        m_light.Ambient.b = 0.0f;
        m_light.Ambient.a = 0.0f;
        m_light.Range = 1000.0f;
        m_light.Attenuation0 = 0.0f;
        m_light.Attenuation1 = 0.8f + (float)rand() / (float)RAND_MAX * 0.4f;
        m_light.Attenuation2 = 0.0f;
    }
    else {

        if (g_TestParams.bTextureBalls) {

	        m_material.Diffuse.r	= 0.5f;
	        m_material.Diffuse.g	= 0.5f;
	        m_material.Diffuse.b	= 0.5f;
	        m_material.Diffuse.a	= 1.0f;
	        m_material.Ambient.r	= 0.25f;
	        m_material.Ambient.g	= 0.25f;
	        m_material.Ambient.b	= 0.25f;
	        m_material.Ambient.a	= 1.0f;
	        m_material.Specular.r	= 1.0f;
	        m_material.Specular.g	= 1.0f;
	        m_material.Specular.b	= 1.0f;
	        m_material.Specular.a	= 1.0f;
	        m_material.Emissive.r	= 0.0f;
	        m_material.Emissive.g	= 0.0f;
	        m_material.Emissive.b	= 0.0f;
	        m_material.Emissive.a	= 0.0f;
	        m_material.Power		= 32.0f;
        }
        else {

    	    // Create a random color for the material
	        do 
	        {
		        m_material.Diffuse.r	= (float)rand() / (float)RAND_MAX;
		        m_material.Diffuse.g	= (float)rand() / (float)RAND_MAX;
		        m_material.Diffuse.b	= (float)rand() / (float)RAND_MAX;
	        }
	        while ((m_material.Diffuse.r + m_material.Diffuse.g + m_material.Diffuse.b) < 1.0f);
	        m_material.Diffuse.a	= 1.0f;
	        m_material.Ambient.r	= 0.25f;//m_material.Diffuse.r * 0.4f;
	        m_material.Ambient.g	= 0.25f;//m_material.Diffuse.g * 0.4f;
	        m_material.Ambient.b	= 0.25f;//m_material.Diffuse.b * 0.4f;
	        m_material.Ambient.a	= 1.0f;
	        m_material.Specular.r	= 1.0f;
	        m_material.Specular.g	= 1.0f;
	        m_material.Specular.b	= 1.0f;
	        m_material.Specular.a	= 1.0f;
	        m_material.Emissive.r	= 0.0f;
	        m_material.Emissive.g	= 0.0f;
	        m_material.Emissive.b	= 0.0f;
	        m_material.Emissive.a	= 0.0f;
	        m_material.Power		= 32.0f;
        }
    }

	// Num verts and tris
	m_numVertices	= (numLong + 1) * numLat;
	m_numTriangles	= numLong * (numLat - 1) * 2;

    // Create the sphere vertex buffer
    hr = pDevice->CreateVertexBuffer(m_numVertices * sizeof(Vertex), 0, 
									 0, D3DPOOL_DEFAULT, &m_pVB);
	if(FAILED(hr))
	{
		return hr;
	}

	hr = pDevice->CreateIndexBuffer(m_numTriangles * 3 * sizeof(WORD),
									D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
									D3DPOOL_DEFAULT, &m_pIB);
	if(FAILED(hr)) 
	{
        m_pVB->Release();
		return hr;
	}

    m_pVB->Lock(0, 0, (LPBYTE*)&vertices, 0);
	m_pIB->Lock(0, 0, (LPBYTE*)&indices, 0);

    // Create the sphere
    for (j = 0; j < numLat; j++) {

        fY = (float)(j) / (float)(numLat - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i <= numLong; i++) {

            pr = &vertices[(numLong + 1) * j + i];
            fX = (float)(i) / (float)(numLong);
            fTX = fX * M_2PI;

            pr->vNormal.x = (float)(cos(fTX)) * fSinY;
            pr->vNormal.y = fCosY;
            pr->vNormal.z = (float)(sin(fTX)) * fSinY;
            pr->position.x = pr->vNormal.x * m_radius;
            pr->position.y = pr->vNormal.y * m_radius;
            pr->position.z = pr->vNormal.z * m_radius;
            pr->tu = fX * 2.0f;
            pr->tv = fY * 1.0f;
        }
    }

    for (j = 0; j < numLat - 1; j++) {

        for (i = 0; i < numLong; i++) {

            indices[index++] = (j + 1) * (numLong + 1) + i;
            indices[index++] = j * (numLong + 1) + i;
            indices[index++] = (j + 1) * (numLong + 1) + i + 1;
            indices[index++] = j * (numLong + 1) + i;
            indices[index++] = j * (numLong + 1) + i + 1;
            indices[index++] = (j + 1) * (numLong + 1) + i + 1;
        }
    }

    if (!bLight) {
        // Initialize the model to texture space transforms
        if (!ComputeTangentTransforms(vertices, indices, index, TRUE)) {
            m_pVB->Release();
            m_pIB->Release();
            return FALSE;
        }
    }

    m_pVB->Unlock();
    m_pIB->Unlock();

	m_dontDraw = FALSE;

	hr = InitAudio();
	if (FAILED( hr))
	{
		Release();
		delete [] indices;
		return hr;
	}

    // Create the base texture
    if (!m_pd3dtBase) {
        hr = D3DXCreateTextureFromFileEx(pDevice, g_TestParams.szBallBaseTex, D3DX_DEFAULT, D3DX_DEFAULT, 
                                    D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 0, D3DX_FILTER_TRIANGLE,
                                    D3DX_FILTER_TRIANGLE, 0, NULL, NULL, &m_pd3dtBase);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // Create the bump texture
    if (!m_pd3dtBump) {
        hr = CreateTextureFromHeightMap(pDevice, g_TestParams.szBallBumpTex, &m_pd3dtBump);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // Create the light texture
    if (!m_pd3dtLight) {
        hr = CreateLightTexture(pDevice, 128, &m_pd3dtLight);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // Create a white texture
    if (!m_pd3dtWhite) {

        D3DLOCKED_RECT d3dlr;

        hr = pDevice->CreateTexture(1, 1, 0, 0, D3DFMT_X8R8G8B8, 0, &m_pd3dtWhite);
        if (FAILED(hr)) {
            return hr;
        }

        m_pd3dtWhite->LockRect(0, &d3dlr, NULL, 0);
        *((LPDWORD)d3dlr.pBits) = 0xFFFFFFFF;
        m_pd3dtWhite->UnlockRect(0);
    }

    // Create a flat normal map
    if (!m_pd3dtFlat) {

        D3DLOCKED_RECT d3dlr;

        hr = pDevice->CreateTexture(1, 1, 0, 0, D3DFMT_X8R8G8B8, 0, &m_pd3dtFlat);
        if (FAILED(hr)) {
            return hr;
        }

        m_pd3dtFlat->LockRect(0, &d3dlr, NULL, 0);
        *((LPDWORD)d3dlr.pBits) = VectorToColor(&XGVECTOR3(0.0f, 0.0f, -1.0f));
        m_pd3dtFlat->UnlockRect(0);
    }

	return S_OK;
}

//------------------------------------------------------------------------------
//	CBall::SetPosition
//------------------------------------------------------------------------------
void 
CBall::SetPosition(
				   IN const XGVECTOR3& position
				   )
/*++

Routine Description:

	Sets the position of the ball

Arguments:

	IN position -	position

Return Value:

	None

--*/
{
	m_location = position;
	XGMatrixTranslation(&m_worldMatrix, m_location[0], m_location[1], 
						  m_location[2]);
	m_min	= XGVECTOR3(m_radius, m_radius, m_radius) + m_location;
	m_max	= XGVECTOR3(-m_radius, -m_radius, -m_radius) - m_location;
    if (m_bLightSource) {
        m_light.Position.x = m_location.x;
        m_light.Position.y = m_location.y;
        m_light.Position.z = m_location.z;
    }
}

//------------------------------------------------------------------------------
//	CBall::SetDirection
//------------------------------------------------------------------------------
void 
CBall::SetDirection(
					IN const XGVECTOR3& direction
					)
/*++

Routine Description:

	Sets the direction of the ball

Arguments:

	IN direction -	Direction

Return Value:

	None

--*/
{
	m_direction = direction;
}

//------------------------------------------------------------------------------
//	CBall::SetSpeed
//------------------------------------------------------------------------------
void 
CBall::SetSpeed(
				IN float speed
				)
				/*++

Routine Description:

	Sets the speed of the ball

Arguments:

	IN speed -	Speed

Return Value:

	None

--*/
{
	m_speed = speed;
}

//------------------------------------------------------------------------------
//	CBall::GetSpeed
//------------------------------------------------------------------------------
float 
CBall::GetSpeed(void) const
/*++

Routine Description:

	Returns the speed of the ball

Arguments:

	None

Return Value:

	The speed of the ball

--*/
{
	return m_speed;
}

//------------------------------------------------------------------------------
//	CBall::GetDirection
//------------------------------------------------------------------------------
const XGVECTOR3& 
CBall::GetDirection(void) const
/*++

Routine Description:

	Returns the direction of the ball

Arguments:

	None

Return Value:

	Direction of the ball

--*/
{
	return m_direction;
}

//------------------------------------------------------------------------------
//	CBall::Move
//------------------------------------------------------------------------------
void
CBall::Move(
			IN float amount
			)
/*++

Routine Description:

	Updates the position of the ball by moving it amount

Arguments:

	IN amount -	Amount to move

Return Value:

	None

--*/
{
	XGVECTOR3	da = m_direction * amount;

	m_location += da;
	m_worldMatrix(3,0) += da.x;
	m_worldMatrix(3,1) += da.y;
	m_worldMatrix(3,2) += da.z;
	m_min = XGVECTOR3(m_location.x - m_radius, m_location.y - m_radius, m_location.z - m_radius);
	m_max = XGVECTOR3(m_location.x + m_radius, m_location.y + m_radius, m_location.z + m_radius);
    if (m_bLightSource) {
        m_light.Position.x = m_location.x;
        m_light.Position.y = m_location.y;
        m_light.Position.z = m_location.z;
    }
}

//------------------------------------------------------------------------------
//	CBall::GetRadius
//------------------------------------------------------------------------------
float 
CBall::GetRadius(void) const
/*++

Routine Description:

	Returns the radius of the ball

Arguments:

	None

Return Value:

	The radius

--*/
{
	return m_radius;
}

//------------------------------------------------------------------------------
//	CBall::Render
//------------------------------------------------------------------------------
void
CBall::Render(
			  IN IDirect3DDevice8*	pDevice,
              IN XGMATRIX*          pmViewProj,
              IN XGVECTOR3*         pvEyePos,
              IN D3DLIGHT8*         pLight,
              IN D3DCOLORVALUE*     pdcvAmbient,
              IN UINT               uPass
			  )
/*++

Routine Description:

	Renders the Ball

Arguments:

	IN pDevice -	IDirect3DDevice8

Return Value:

	None

--*/
{
    XGMATRIX mBackWorld, mTransform;
    XGVECTOR3 vEyePos, vLightPos;
    static XGVECTOR3 vOrigin = XGVECTOR3(0.0f, 0.0f, 0.0f);

	if(m_dontDraw)
		return;

	// Vertex shader type
//	pDevice->SetVertexShader(XYZ_NORMAL);

	// Set the material
//	pDevice->SetMaterial(&m_material);

    if (!m_bLightSource) {

        // Set the transform
        XGMatrixMultiply(&mTransform, &m_worldMatrix, pmViewProj);
        XGMatrixTranspose(&mTransform, &mTransform);
        pDevice->SetVertexShaderConstant(0, &mTransform, 4);

        XGMatrixInverse(&mBackWorld, NULL, &m_worldMatrix);

        // Transform the eye position into model space
        XGVec3TransformCoord(&vEyePos, pvEyePos, &mBackWorld);
        pDevice->SetVertexShaderConstant(5, &vEyePos, 1);

        // Transform the light position into model space
        vLightPos.x = pLight->Position.x;
        vLightPos.y = pLight->Position.y;
        vLightPos.z = pLight->Position.z;
        XGVec3TransformCoord(&vLightPos, &vLightPos, &mBackWorld);
        pDevice->SetVertexShaderConstant(4, &vLightPos, 1);

        // Set the ambient-emissive term for lighting
        if (uPass) {
            pDevice->SetRenderState(D3DRS_PSCONSTANT0_3, 0);
        }
        else {
            D3DCOLORVALUE dcvAmbientEmissive;
            dcvAmbientEmissive.r = m_material.Emissive.r + m_material.Ambient.r * pdcvAmbient->r;
            dcvAmbientEmissive.g = m_material.Emissive.g + m_material.Ambient.g * pdcvAmbient->g;
            dcvAmbientEmissive.b = m_material.Emissive.b + m_material.Ambient.b * pdcvAmbient->b;
            dcvAmbientEmissive.a = m_material.Emissive.a + m_material.Ambient.a * pdcvAmbient->a;
            if (dcvAmbientEmissive.r > 1.0f) dcvAmbientEmissive.r = 1.0f;
            if (dcvAmbientEmissive.g > 1.0f) dcvAmbientEmissive.g = 1.0f;
            if (dcvAmbientEmissive.b > 1.0f) dcvAmbientEmissive.b = 1.0f;
            if (dcvAmbientEmissive.a > 1.0f) dcvAmbientEmissive.a = 1.0f;
            pDevice->SetRenderState(D3DRS_PSCONSTANT0_3, (BYTE)(dcvAmbientEmissive.a * 255.0f) << 24 | 
                                                           (BYTE)(dcvAmbientEmissive.r * 255.0f) << 16 |
                                                           (BYTE)(dcvAmbientEmissive.g * 255.0f) << 8 |
                                                           (BYTE)(dcvAmbientEmissive.b * 255.0f));
        }

        pDevice->SetVertexShaderConstant(7, &pLight->Attenuation0, 1);
        pDevice->SetVertexShaderConstant(8, &XGVECTOR4(0.0f, 0.0f, 0.0f, pLight->Range), 1);
        pDevice->SetRenderState(D3DRS_PSCONSTANT0_1, ModulateColors(&pLight->Diffuse, &m_material.Diffuse));
        pDevice->SetRenderState(D3DRS_PSCONSTANT1_1, ModulateColors(&pLight->Specular, &m_material.Specular));
        pDevice->SetRenderState(D3DRS_PSCONSTANT0_2, ModulateColors(&pLight->Ambient, &m_material.Ambient));

        // Draw opaque on the first pass, translucent on all subsequent passes
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, uPass ? TRUE : FALSE);

        if (g_TestParams.bTextureBalls) {
            pDevice->SetTexture(0, m_pd3dtBase);
            pDevice->SetTexture(1, m_pd3dtBump);
        }
        else {
            pDevice->SetTexture(0, m_pd3dtWhite);
            pDevice->SetTexture(1, m_pd3dtFlat);
        }

	    // Set the vertex buffer stream source
	    pDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));

	    // Fill mode
	    pDevice->SetRenderState(D3DRS_FILLMODE, m_fillMode);

	    // Draw the object
	    pDevice->SetIndices(m_pIB, 0);
	    pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
							          m_numTriangles);
    }
    else {

	    // Set the translation
    	pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

        pDevice->SetVertexShader(D3DFVF_XYZ);

        pDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

        //Render black if we're not on beat, otherwise the light's real color.
		pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA((BYTE)(255.0f * m_light.Diffuse.r), (BYTE)(255.0f * m_light.Diffuse.g), (BYTE)(255.0f * m_light.Diffuse.b), (BYTE)(255.0f * m_light.Diffuse.a)));

        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

        pDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_NONE);

        pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(m_radius * 2.0f));

        pDevice->SetTexture(3, m_pd3dtLight);

        pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &vOrigin, sizeof(XGVECTOR3));

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

    RenderAudio();
}

//------------------------------------------------------------------------------
//	CBall::GetMin
//------------------------------------------------------------------------------
const XGVECTOR3& 
CBall::GetMin(void) const
/*++

Routine Description:

	Returns the min coordinate of the Ball

Arguments:

	None

Return Value:

	The min coordinate of the Ball

--*/
{
	return m_min;
}

//------------------------------------------------------------------------------
//	CBall::GetMax
//------------------------------------------------------------------------------
const XGVECTOR3& 
CBall::GetMax(void) const
/*++

Routine Description:

	Returns the max coordinate of the Ball

Arguments:

	None

Return Value:

	The max coordinate of the Ball

--*/
{
	return m_max;
}

//------------------------------------------------------------------------------
//	CBall::GetNumVerts
//------------------------------------------------------------------------------
int 
CBall::GetNumVerts(void) const
/*++

Routine Description:

	Returns the number of vertices in the Ball

Arguments:

	None

Return Value:

	The number of vertices

--*/
{
	return m_numVertices;
}

//------------------------------------------------------------------------------
//	CBall::GetNumTris
//------------------------------------------------------------------------------
int 
CBall::GetNumTris(void) const
/*++

Routine Description:

	Returns the number of triangles in the Ball

Arguments:

	None

Return Value:

	number of triangles

--*/
{
	return m_numTriangles;
}

//------------------------------------------------------------------------------
//	CBall::GetLocation
//------------------------------------------------------------------------------
const XGVECTOR3&
CBall::GetLocation(void) const
/*++

Routine Description:

	Returns the location of the ball

Arguments:

	None

Return Value:

	The location of the ball

--*/
{
	return m_location;
}

//------------------------------------------------------------------------------
//	CBall::Release
//------------------------------------------------------------------------------
void
CBall::Release(void)
/*++

Routine Description:

	Releases the Ball

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pVB) {
		m_pVB->Release();
		m_pVB = NULL;
	}
	if(m_pIB) {
		m_pIB->Release();
		m_pIB = NULL;
	}

	if ( m_pCollisionSound )
	{
		m_pCollisionSound->Release();
		m_pCollisionSound = NULL;
	}

	if ( m_pAmbientSound )
	{
		m_pAmbientSound->Release();
		m_pAmbientSound = NULL;
	}

	if ( m_pSubMix )
	{
		m_pSubMix->Release();
		m_pSubMix = NULL;
	}

    if(m_pCollisionSoundData)
    {
        m_pCollisionSoundData->Release();
        m_pCollisionSoundData = NULL;
    }

    if(m_pAmbientSoundData)
    {
        m_pAmbientSoundData->Release();
        m_pAmbientSoundData = NULL;
    }
}

//------------------------------------------------------------------------------
//	CBall::InitAudio
//------------------------------------------------------------------------------
HRESULT
CBall::InitAudio( void )
/*++

Routine Description:

	Init the sound buffer

Arguments:

	None

Return Value:

	S_OK, E_XXXX

--*/
{	
    HRESULT                 hr  = DS_OK;

    return hr;
}

//------------------------------------------------------------------------------
//	CBall::CreateSubMixDestination
//------------------------------------------------------------------------------
HRESULT
CBall::CreateSubMixDestination( void )
{
    DSBUFFERDESC            dsbd;
    HRESULT                 hr;

    ASSERT(!m_pSubMix);

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwFlags = DSBCAPS_MIXIN;

    if(g_TestParams.b3DSound)
    {
        dsbd.dwFlags |= DSBCAPS_CTRL3D;
    }

    return DirectSoundCreateBuffer(&dsbd, &m_pSubMix);
}

//------------------------------------------------------------------------------
//	CBall::CreateCollisionSound
//------------------------------------------------------------------------------
HRESULT
CBall::CreateCollisionSound( void )
{
    DSBUFFERDESC            dsbd;
    DSMIXBINS               dsmixbins;
    LPVOID                  pvAudioData;
    DWORD                   dwAudioDataSize;
    HRESULT                 hr;

    ASSERT(!m_pCollisionSound);
    ASSERT(!m_pCollisionSoundData);

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_LOCDEFER;

    m_pCollisionSoundData = &m_CollisionSoundData;
    hr = m_pCollisionSoundData->CreateFile(g_TestParams.szBounceSound, (LPCWAVEFORMATEX *)&dsbd.lpwfxFormat, &pvAudioData, &dwAudioDataSize);

    if(SUCCEEDED(hr))
    {
        m_dwCollisionFrequency = dsbd.lpwfxFormat->nSamplesPerSec;
        
        if(m_pSubMix)
        {
            dsmixbins.dwMixBinCount = 0;
            dsbd.lpMixBins = &dsmixbins;
        }
        else if(g_TestParams.b3DSound)
        {
            dsbd.dwFlags |= DSBCAPS_CTRL3D;
        }

        hr = DirectSoundCreateBuffer(&dsbd, &m_pCollisionSound);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pCollisionSound->SetBufferData(pvAudioData, dwAudioDataSize);
    }

    if(SUCCEEDED(hr) && m_pSubMix)
    {
        hr = m_pCollisionSound->SetOutputBuffer(m_pSubMix);
    }

    if(FAILED(hr) && m_pCollisionSound)
    {
        m_pCollisionSound->Release();
        m_pCollisionSound = NULL;
    }

    return hr;
}

//------------------------------------------------------------------------------
//	CBall::CreateAmbientSound
//------------------------------------------------------------------------------
HRESULT
CBall::CreateAmbientSound( void )
{
    WAVEFORMATEX            wfx;
    DSBUFFERDESC            dsbd;
    DSMIXBINS               dsmixbins;
    LPVOID                  pvAudioData;
    DWORD                   dwSampleCount;
    DWORD                   dwAudioDataSize;
    BOOL                    fInit;
    DSLFODESC               lfo;
    HRESULT                 hr;
    int                     i;

    ASSERT(!m_pAmbientSound);
    ASSERT(!m_pAmbientSoundData);

    ZeroMemory(&dsbd, sizeof(dsbd));
    ZeroMemory(&lfo, sizeof(lfo));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_LOCDEFER;

    m_pAmbientSoundData = &m_AmbientSoundData;

    if(g_TestParams.szLightSound[0])
    {
        hr = m_pAmbientSoundData->CreateFile(g_TestParams.szLightSound, (LPCWAVEFORMATEX *)&dsbd.lpwfxFormat, &pvAudioData, &dwAudioDataSize);
    }
    else
    {
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = 1;
        wfx.nSamplesPerSec = AMBIENT_SOUND_FREQUENCY;
        wfx.wBitsPerSample = 16;
        wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize = 0;
    
        dsbd.lpwfxFormat = &wfx;

        dwSampleCount = 128;
        dwAudioDataSize = dwSampleCount * wfx.nBlockAlign;
    
        hr = m_pAmbientSoundData->CreateEmpty(&wfx, dwAudioDataSize, &pvAudioData, &fInit);

        if(SUCCEEDED(hr) && fInit)
        {
            for(i = 0; i < (int)dwSampleCount; i++)
            {
                *((short *)pvAudioData + i) = (short)(65536 * ((float)(i - (dwSampleCount / 2)) / (FLOAT)dwSampleCount));
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        if(m_pSubMix)
        {
            dsmixbins.dwMixBinCount = 0;
            dsbd.lpMixBins = &dsmixbins;
        }
        else if(g_TestParams.b3DSound)
        {
            dsbd.dwFlags |= DSBCAPS_CTRL3D;
        }

        hr = DirectSoundCreateBuffer(&dsbd, &m_pAmbientSound);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pAmbientSound->SetBufferData(pvAudioData, dwAudioDataSize);
    }

    if(SUCCEEDED(hr) && m_pSubMix)
    {
        hr = m_pAmbientSound->SetOutputBuffer(m_pSubMix);
    }

    if(SUCCEEDED(hr))
    {
        lfo.dwLFO = DSLFO_MULTI;
        lfo.dwDelta = 200;
        lfo.lPitchModulation = 64;
        lfo.lAmplitudeModulation = 48;

        hr = m_pAmbientSound->SetLFO(&lfo);
    }

    if(FAILED(hr) && m_pAmbientSound)
    {
        m_pAmbientSound->Release();
        m_pAmbientSound = NULL;
    }

    return hr;
}

//------------------------------------------------------------------------------
//	CBall::GetLight
//------------------------------------------------------------------------------
D3DLIGHT8*
CBall::GetLight( void )
{
  
    return &m_light;
}

//------------------------------------------------------------------------------
//	CBall::PlayBuffer
//------------------------------------------------------------------------------
void
CBall::PlayBuffer(
				  IN float	volume,
				  IN float	pitch
				  )
/*++

Routine Description:

	Play the sound Buffer

Arguments:

	IN volume	- Volume (0.0 - 1.0)
	IN pitch	- Pitch (0.0 - 1.0)

Return Value:

	None

--*/
{
    static const DWORD      dwVolumeRange   = 3000; // 0-30dB
    static const DWORD      dwPitchRange    = 4; // +/-2 octaves
    const LONG              lGlobalVolume   = g_TestParams.lDSVolume;
    const LONG              lGlobalPitch    = CalculatePitch(g_TestParams.dwDSFreq);
    LONG                    lLocalVolume;
    LONG                    lLocalPitch;
    
    lLocalVolume = (LONG)-((1.0f - volume) * (FLOAT)dwVolumeRange);
    lLocalPitch = (LONG)((0.5f - pitch) * (FLOAT)dwPitchRange) << 12;

	if(m_pCollisionSound)
	{
        if(!m_pSubMix)
        {
		    lLocalVolume += lGlobalVolume;
            lLocalPitch += lGlobalPitch;
        }
            
        if(lLocalVolume > DSBVOLUME_MAX)
        {
            lLocalVolume = DSBVOLUME_MAX;
        }
        else if(lLocalVolume < DSBVOLUME_MIN)
        {
            lLocalVolume = DSBVOLUME_MIN;
        }
        
        m_pCollisionSound->SetVolume(lLocalVolume);

        if(lLocalPitch > DSBPITCH_MAX)
        {
            lLocalPitch = DSBPITCH_MAX;
        }
        else if(lLocalPitch < DSBPITCH_MIN)
        {
            lLocalPitch = DSBPITCH_MIN;
        }

		m_pCollisionSound->SetPitch(lLocalPitch);

        if(!m_pSubMix && g_TestParams.b3DSound)
        {
            m_pCollisionSound->SetPosition(m_location.x, m_location.y, m_location.z, DS3D_DEFERRED);
        }

		m_pCollisionSound->Play(0, 0, DSBPLAY_FROMSTART);
	}
}

//------------------------------------------------------------------------------
//	CBall::RenderAudio
//------------------------------------------------------------------------------
void
CBall::RenderAudio(void)
{
    const LONG              lGlobalPitch    = CalculatePitch(g_TestParams.dwDSFreq);
    LONG                    lLocalPitch;
    
    if(m_pSubMix)
    {
        m_pSubMix->SetVolume(g_TestParams.lDSVolume);
        m_pSubMix->SetPitch(lGlobalPitch);

        if(g_TestParams.b3DSound)
        {
            m_pSubMix->SetPosition(m_location.x, m_location.y, m_location.z, DS3D_DEFERRED);
            m_pSubMix->SetVelocity(m_direction.x, m_direction.y, m_direction.z, DS3D_DEFERRED);
        }
    }

    if(m_pAmbientSound)
    {
        if(m_pSubMix)
        {
            m_pAmbientSound->SetVolume(g_TestParams.lAmbientVolume);
        }
        else
        {
            lLocalPitch = CalculatePitch(AMBIENT_SOUND_FREQUENCY);
            
            m_pAmbientSound->SetVolume(g_TestParams.lAmbientVolume + g_TestParams.lDSVolume);
            m_pAmbientSound->SetPitch(lLocalPitch + lGlobalPitch);

            if(g_TestParams.b3DSound)
            {
                m_pAmbientSound->SetPosition(m_location.x, m_location.y, m_location.z, DS3D_DEFERRED);
                m_pAmbientSound->SetVelocity(m_direction.x, m_direction.y, m_direction.z, DS3D_DEFERRED);
            }
        }

        m_pAmbientSound->Play(0, 0, DSBPLAY_LOOPING);
    }
}

//------------------------------------------------------------------------------
//	CBall::DisableAmbientAudio
//------------------------------------------------------------------------------
void
CBall::DisableAmbientAudio(void)
{
    if(m_pAmbientSound)
    {
        m_pAmbientSound->Stop();
    }
}

//------------------------------------------------------------------------------
//	CBall::DrawSolid
//------------------------------------------------------------------------------
void 
CBall::DrawSolid(void)
/*++

Routine Description:

	Sets the render state of the ball to render solid

Arguments:

	None

Return Value:

	None

--*/
{
	m_fillMode = D3DFILL_SOLID;
}

//------------------------------------------------------------------------------
//	CBall::DrawWireframe
//------------------------------------------------------------------------------
void 
CBall::DrawWireframe(void)
/*++

Routine Description:

	Sets the render state of the ball to render wireframe

Arguments:

	None

Return Value:

	None

--*/
{
	m_fillMode = D3DFILL_WIREFRAME;
}

//------------------------------------------------------------------------------
//	CBall::DrawPoints
//------------------------------------------------------------------------------
void 
CBall::DrawPoints(void)
/*++

Routine Description:

	Sets the render state of the ball to render as points

Arguments:

	None

Return Value:

	None

--*/
{
	m_fillMode = D3DFILL_POINT;
}

//------------------------------------------------------------------------------
//	CBall::DrawReduced
//------------------------------------------------------------------------------
void
CBall::DrawReduced(void)
/*++

Routine Description:

	Draws in a reduced mode based on the level of detail

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_detail > 16)
		DrawPoints();
	else
		DrawWireframe();
}

//******************************************************************************
void CBall::PlaceLightInRange(UINT uLight, BOOL bInRange) {

    DWORD dwLight;
    UINT index;

    for (index = 0; uLight > 31; uLight -= 32, index++);

    if (index > 4) {
        // Light index exceeds the maximum
        __asm int 3;
        return;
    }

    if (bInRange) {
        m_dwInLightRange[index] |= (1 << uLight);
    }
    else {
        m_dwInLightRange[index] &= ~(1 << uLight);
    }
}

//******************************************************************************
HRESULT CBall::CreateTextureFromHeightMap(IDirect3DDevice8* pDevice, LPCSTR szImage, IDirect3DTexture8** ppd3dt) {

    IDirect3DTexture8* pd3dtHeight, *pd3dt;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;
    float* pfHeightMap, *pfHeight;
    LPDWORD pdwPixel;
    UINT i, x, y, uCount;
    XGVECTOR3 v1, v2, n;
    HRESULT hr;

    if (!ppd3dt) {   
        return E_FAIL;
    }

    *ppd3dt = NULL;

    hr = D3DXCreateTextureFromFileEx(pDevice, szImage, D3DX_DEFAULT, D3DX_DEFAULT, 
                                D3DX_DEFAULT, 0, D3DFMT_X8R8G8B8, 0, D3DX_FILTER_TRIANGLE,
                                D3DX_FILTER_TRIANGLE, 0, NULL, NULL, &pd3dtHeight);
    if (FAILED(hr)) {
        return hr;
    }

    pd3dtHeight->GetLevelDesc(0, &d3dsd);

    // Create the bump map
    hr = pDevice->CreateTexture(d3dsd.Width, d3dsd.Height, 0, 0, D3DFMT_X8R8G8B8, 0, &pd3dt);
    if (!pd3dt) {
        pd3dtHeight->Release();
        return hr;
    }

    pfHeightMap = (float*)HeapAlloc(GetProcessHeap(), 0, d3dsd.Width * d3dsd.Height * sizeof(float));
    if (!pfHeightMap) {
        pd3dtHeight->Release();
        pd3dt->Release();
        return E_OUTOFMEMORY;
    }

    uCount = pd3dtHeight->GetLevelCount();

    for (i = 0; i < uCount; i++) {

        pd3dtHeight->GetLevelDesc(i, &d3dsd);

        // Convert the pixel intensities in the source image into height values
        // ranging from 0.0 to 1.0
        pd3dtHeight->LockRect(i, &d3dlr, NULL, 0);

        pdwPixel = (LPDWORD)d3dlr.pBits;

        Swizzler swz(d3dsd.Width, d3dsd.Height, 1);
        swz.SetU(0);
        swz.SetV(0);

        for (y = 0; y < d3dsd.Height; y++) {

            for (x = 0; x < d3dsd.Width; x++) {

                pfHeightMap[y * d3dsd.Width + x] = 
                       (float)(((pdwPixel[swz.Get2D()] >> 16) & 0xFF) +
                               ((pdwPixel[swz.Get2D()] >> 8)  & 0xFF) +
                               ((pdwPixel[swz.Get2D()])       & 0xFF)) / 3.0f / 255.0f;
                swz.IncU();
            }

//            pdwPixel += d3dlr.Pitch >> 2;
            swz.IncV();
        }

        pd3dtHeight->UnlockRect(i);

        // Calculate normal map vectors from the height map information
        pd3dt->LockRect(i, &d3dlr, NULL, 0);

        pdwPixel = (LPDWORD)d3dlr.pBits;

        v1 = XGVECTOR3(0.0f, 0.0f, 0.0f);
        v2 = XGVECTOR3(0.0f, 0.0f, 0.0f);

        swz.SetU(0);
        swz.SetV(0);

        for (y = 0; y < d3dsd.Height; y++) {

            for (x = 0; x < d3dsd.Width; x++) {

                // Tiled
                pfHeight = &pfHeightMap[y * d3dsd.Width + x];
                if (y == d3dsd.Height - 1) {
                    v2.z = -(pfHeightMap[x] - *pfHeight);
                }
                else {
                    v2.z = -(pfHeight[d3dsd.Width] - pfHeight[0]);
                }
                if (x == d3dsd.Width - 1) {
                    v1.z = -(*(pfHeight - x) - *pfHeight);
                }
                else {
                    v1.z = -(pfHeight[1] - pfHeight[0]);
                }
                v1.x = (float)sqrt(1.0f - v1.z * v1.z);
                v2.y = (float)sqrt(1.0f - v2.z * v2.z);
                XGVec3Cross(&n, &v2, &v1);
                XGVec3Normalize(&n, &n);
                pdwPixel[swz.Get2D()] = VectorToColor(&n);

                swz.IncU();
            }

            swz.IncV();
        }

        pd3dt->UnlockRect(i);
    }

    HeapFree(GetProcessHeap(), 0, pfHeightMap);
    pd3dtHeight->Release();

    *ppd3dt = pd3dt;

    return D3D_OK;
}

//******************************************************************************
HRESULT CBall::CreateLightTexture(IDirect3DDevice8* pDevice, UINT uLength, IDirect3DTexture8** ppd3dt) {

    IDirect3DTexture8* pd3dt;
    D3DLOCKED_RECT d3dlr;
    LPDWORD pdwTexel;
    D3DXVECTOR3 vNormal, vCenter, vUp;
    float fRadius, fRadiusSq;
    BYTE Intensity;
    UINT i, j;
    HRESULT hr;
    Swizzler swz(uLength, uLength, 1);

    if (!ppd3dt) {
        return FALSE;
    }

    *ppd3dt = NULL;

    hr = pDevice->CreateTexture(uLength, uLength, 0, 0, D3DFMT_A8R8G8B8, 0, &pd3dt);
    if (FAILED(hr)) {
        return hr;
    }

    pd3dt->LockRect(0, &d3dlr, NULL, 0);
    pdwTexel = (LPDWORD)d3dlr.pBits;
    fRadius = (float)(uLength / 2);
    fRadiusSq = fRadius * fRadius;
    vCenter = D3DXVECTOR3(fRadius + 0.5f, fRadius + 0.5f, 0.0f);
    vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    swz.SetU(0);
    swz.SetV(0);

    for (i = 0; i < uLength; i++) {

        for (j = 0; j < uLength; j++) {

            vNormal = D3DXVECTOR3((float)j, (float)i, 0.0f) - vCenter;
            if (D3DXVec3Length(&vNormal) < fRadius) {
                vNormal.z = (float)sqrt(fRadiusSq - vNormal.x * vNormal.x - vNormal.y * vNormal.y);
                D3DXVec3Normalize(&vNormal, &vNormal);
                Intensity = (BYTE)(D3DXVec3Dot(&vNormal, &vUp) * 255.0f);
                pdwTexel[swz.Get2D()] = D3DCOLOR_RGBA(Intensity, Intensity, Intensity, Intensity);
            }
            else {
                pdwTexel[swz.Get2D()] = 0;
            }

            swz.IncU();
        }

        swz.IncV();
    }

    pd3dt->UnlockRect(0);

    *ppd3dt = pd3dt;

    return D3D_OK;
}

//******************************************************************************
// Calculate non-normalized tangent and binormal vector terms for use in
// constructing an othonormal basis transform to rotate a vector from model
// space to tangent space.
//******************************************************************************
BOOL CBall::CalculateTangentTerms(XGVECTOR3* pvTangent, XGVECTOR3* pvBinormal, 
                                            Vertex* prVertices, LPWORD pwIndices, 
                                            UINT uNumIndices, BOOL bInterpolate)
{
    XGVECTOR3       vEdge0, vEdge1, vTemp;
    XGVECTOR3       vPlane[3];
    WORD            w0, w1, w2;
    float           fTemp;
    UINT            i, j, k;

    memset(pvTangent, 0, uNumIndices * sizeof(XGVECTOR3));
    memset(pvBinormal, 0, uNumIndices * sizeof(XGVECTOR3));

    // Calculate the tangent and binormal vectors for each vertex.  If the vertex is
    // indexed by more than one triangle, add the vectors for each triangle together
    // to obtain an average of the vectors for all triangles formed by the vertex.
    for (i = 0; i < uNumIndices; i+=3) {

        w0 = pwIndices[i];
        w1 = pwIndices[i+1];
        w2 = pwIndices[i+2];

        vEdge0 = XGVECTOR3(prVertices[w1].position.x - prVertices[w0].position.x, prVertices[w1].tu - prVertices[w0].tu, prVertices[w1].tv - prVertices[w0].tv);
        vEdge1 = XGVECTOR3(prVertices[w2].position.x - prVertices[w0].position.x, prVertices[w2].tu - prVertices[w0].tu, prVertices[w2].tv - prVertices[w0].tv);
        XGVec3Cross(&vPlane[0], &vEdge0, &vEdge1);
        vEdge0.x = prVertices[w1].position.y - prVertices[w0].position.y;
        vEdge1.x = prVertices[w2].position.y - prVertices[w0].position.y;
        XGVec3Cross(&vPlane[1], &vEdge0, &vEdge1);
        vEdge0.x = prVertices[w1].position.z - prVertices[w0].position.z;
        vEdge1.x = prVertices[w2].position.z - prVertices[w0].position.z;
        XGVec3Cross(&vPlane[2], &vEdge0, &vEdge1);

        if (FABS(vPlane[0].x) < 0.000000001f || FABS(vPlane[1].x) < 0.000000001f || FABS(vPlane[2].x) < 0.000000001f) {
            return FALSE;
        }

        vTemp = XGVECTOR3(-vPlane[0].y / vPlane[0].x, -vPlane[1].y / vPlane[1].x, -vPlane[2].y / vPlane[1].x);
        pvTangent[w0] += vTemp;
        pvTangent[w1] += vTemp;
        pvTangent[w2] += vTemp;

        vTemp = XGVECTOR3(-vPlane[0].z / vPlane[0].x, -vPlane[1].z / vPlane[1].x, -vPlane[2].z / vPlane[1].x);
        pvBinormal[w0] += vTemp;
        pvBinormal[w1] += vTemp;
        pvBinormal[w2] += vTemp;
    }

    if (bInterpolate) {

        LPWORD          pwMerge, pwProcessed;
        UINT            uNumMerges, uNumProcessed = 0;

        pwMerge = (LPWORD)HeapAlloc(GetProcessHeap(), 0, uNumIndices * sizeof(WORD));
        if (!pwMerge) {
            return FALSE;
        }

        pwProcessed = (LPWORD)HeapAlloc(GetProcessHeap(), 0, uNumIndices * sizeof(WORD));
        if (!pwProcessed) {
            HeapFree(GetProcessHeap(), 0, pwMerge);
            return FALSE;
        }

        for (i = 0; i < uNumIndices; i++) {

            // Verify pwIndices[i] has to already been processed...if it has continue
            for (j = 0; j < uNumProcessed; j++) {
                if (pwIndices[i] == pwProcessed[j]) {
                    break;
                }
            }
            if (j < uNumProcessed) {
                continue;
            }

            pwMerge[0] = pwIndices[i];
            pwProcessed[uNumProcessed++] = pwIndices[i];
            uNumMerges = 1;

            // Traverse the vertex list, identifying all vertices whose positions are
            // equal to the current vertex position
            for (j = i + 1; j < uNumIndices; j++) {

                fTemp = XGVec3LengthSq(&(prVertices[pwIndices[i]].position - prVertices[pwIndices[j]].position));
                if (fTemp < 0.0000001f) {

                    // See whether the matching vertex has already been added to the merge list
                    for (k = 0; k < uNumMerges; k++) {
                        if (pwIndices[j] == pwMerge[k]) {
                            break;
                        }
                    }
                    if (k == uNumMerges) {
                        pwMerge[uNumMerges++] = pwIndices[j];
                        pwProcessed[uNumProcessed++] = pwIndices[j];
                    }
                }
            }

            if (uNumMerges > 1 && uNumMerges < 5) {
                w0 = pwMerge[0];
                XGVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
                XGVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);
                for (j = 1; j < uNumMerges; j++) {
                    w1 = pwMerge[j];
                    XGVec3Normalize(&pvTangent[w1], &pvTangent[w1]);
                    XGVec3Normalize(&pvBinormal[w1], &pvBinormal[w1]);
                    pvTangent[w0] += pvTangent[w1];
                    pvBinormal[w0] += pvBinormal[w1];
                }
                for (j = 1; j < uNumMerges; j++) {
                    pvTangent[pwMerge[j]] = pvTangent[w0];
                    pvBinormal[pwMerge[j]] = pvBinormal[w0];
                }
            }
        }

        HeapFree(GetProcessHeap(), 0, pwProcessed);
        HeapFree(GetProcessHeap(), 0, pwMerge);
    }

    return TRUE;
}

//******************************************************************************
BOOL CBall::ComputeTangentTransforms(Vertex* prVertices, LPWORD pwIndices, UINT uNumIndices, BOOL bInterpolate) {

    XGVECTOR3       vNormal, vTemp;
    XGVECTOR3       *pvTangent, *pvBinormal;
    WORD            w0;
    XGVECTOR3*      pmT;
    XGVECTOR3       mZero[3];
	UINT            i;

    memset(&mZero, 0, 3 * sizeof(XGVECTOR3));

    for (i = 0; i < uNumIndices; i++) {
        memcpy((LPBYTE)&prVertices[pwIndices[i]].vTangent.x, &mZero, 3 * sizeof(XGVECTOR3));
    }

    pvTangent = (XGVECTOR3*)HeapAlloc(GetProcessHeap(), 0, 2 * uNumIndices * sizeof(XGVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + uNumIndices;

    if (!CalculateTangentTerms(pvTangent, pvBinormal, prVertices, pwIndices, uNumIndices, bInterpolate)) {
        HeapFree(GetProcessHeap(), 0, pvTangent);
        return FALSE;
    }

    for (i = 0; i < uNumIndices; i++) {

        w0 = pwIndices[i];
        pmT = (XGVECTOR3*)((LPBYTE)&prVertices[w0].vTangent.x);
        if (!memcmp(pmT, &mZero, 3 * sizeof(XGVECTOR3))) {

            XGVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            XGVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            XGVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

            pmT[0].x = -pvTangent[w0].x;
            pmT[0].y = -pvTangent[w0].y;
            pmT[0].z = -pvTangent[w0].z;

            pmT[1].x = -pvBinormal[w0].x;
            pmT[1].y = -pvBinormal[w0].y;
            pmT[1].z = -pvBinormal[w0].z;

            pmT[2].x = vNormal.x;
            pmT[2].y = vNormal.y;
            pmT[2].z = vNormal.z;
        }
    }

    HeapFree(GetProcessHeap(), 0, pvTangent);

    return TRUE;
}

//------------------------------------------------------------------------------
//	CBall::IsLightSource
//------------------------------------------------------------------------------
BOOL 
CBall::IsLightSource(void) const
/*++

Routine Description:

	Returns TRUE if the ball is a light source, FALSE otherwise

Arguments:

	None

Return Value:

	TRUE if the ball is a light source, FALSE otherwise

--*/
{
	return m_bLightSource;
}
}

//------------------------------------------------------------------------------
//	CBall::CalculatePitch
//------------------------------------------------------------------------------

LONG
CBall::CalculatePitch
(
    DWORD                   dwFrequency
)
{
    static const DWORD      dwBaseFrequency = 48000;
    FLOAT                   fl4096          = 4096.0f;
    FLOAT                   flRatio         = (FLOAT)dwFrequency / (FLOAT)dwBaseFrequency;
    LONG                    lPitch;

    if(dwBaseFrequency == dwFrequency)
    {
        lPitch = 0;
    }
    else if(!dwFrequency)
    {
        lPitch = DSBPITCH_MIN;
    }
    else
    {
        __asm 
        {
            fld     fl4096
            fld     flRatio
            fyl2x
            fistp   lPitch
        }
    }

    return lPitch;
}

//------------------------------------------------------------------------------
//	CAudioData::CAudioData
//------------------------------------------------------------------------------

CAudioData::CAudioData(void)
{
    m_pwfxFormat = NULL;
    m_pvAudioData = NULL;
    m_dwAudioDataSize = NULL;
    m_dwRefCount = 0;
}

//------------------------------------------------------------------------------
//	CAudioData::~CAudioData
//------------------------------------------------------------------------------

CAudioData::~CAudioData(void)
{
    if(m_pwfxFormat)
    {
        delete [] m_pwfxFormat;
    }

    if(m_pvAudioData)
    {
        delete [] m_pvAudioData;
    }
}

//------------------------------------------------------------------------------
//	CAudioData::CreateFile
//------------------------------------------------------------------------------

HRESULT
CAudioData::CreateFile(LPCSTR pszFile, LPCWAVEFORMATEX *ppwfxFormat, LPVOID *ppvAudioData, LPDWORD pdwAudioDataSize)
{
    LPXFILEMEDIAOBJECT      pFile       = NULL;
    HRESULT                 hr          = DS_OK;
    LPCWAVEFORMATEX         pwfxFormat;
    XMEDIAPACKET            xmp;

    if(!m_dwRefCount)
    {
        ASSERT(!m_pwfxFormat);
        ASSERT(!m_pvAudioData);
        ASSERT(!m_dwAudioDataSize);

        hr = XWaveFileCreateMediaObject(pszFile, &pwfxFormat, &pFile);

        if(SUCCEEDED(hr))
        {
            if(!(m_pwfxFormat = (LPWAVEFORMATEX)new BYTE [sizeof(*pwfxFormat) + pwfxFormat->cbSize]))
            {
                hr = DSERR_OUTOFMEMORY;
            }
        }

        if(SUCCEEDED(hr))
        {
            CopyMemory(m_pwfxFormat, pwfxFormat, sizeof(*pwfxFormat) + pwfxFormat->cbSize);
        }

        if(SUCCEEDED(hr))
        {
            hr = pFile->GetLength(&m_dwAudioDataSize);
        }

        if(SUCCEEDED(hr))
        {
            if(!(m_pvAudioData = new BYTE [m_dwAudioDataSize]))
            {
                hr = DSERR_OUTOFMEMORY;
            }
        }

        if(SUCCEEDED(hr))
        {
            ZeroMemory(&xmp, sizeof(xmp));

            xmp.pvBuffer = m_pvAudioData;
            xmp.dwMaxSize = m_dwAudioDataSize;
            
            hr = pFile->Process(NULL, &xmp);
        }

        if(pFile)
        {
            pFile->Release();
        }
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(m_pwfxFormat);
        ASSERT(m_pvAudioData);
        ASSERT(m_dwAudioDataSize);
        
        AddRef();

        *ppwfxFormat = m_pwfxFormat;
        *ppvAudioData = m_pvAudioData;
        *pdwAudioDataSize = m_dwAudioDataSize;
    }

    return hr;
}

//------------------------------------------------------------------------------
//	CAudioData::CreateEmpty
//------------------------------------------------------------------------------

HRESULT
CAudioData::CreateEmpty(LPCWAVEFORMATEX pwfxFormat, DWORD dwAudioDataSize, LPVOID *ppvAudioData, LPBOOL pfInitialize)
{
    HRESULT                 hr  = DS_OK;
    
    if(!m_dwRefCount)
    {
        ASSERT(!m_pwfxFormat);
        ASSERT(!m_pvAudioData);
        ASSERT(!m_dwAudioDataSize);

        m_dwAudioDataSize = dwAudioDataSize;

        if(!(m_pwfxFormat = (LPWAVEFORMATEX)new BYTE [sizeof(*pwfxFormat) + pwfxFormat->cbSize]))
        {
            hr = DSERR_OUTOFMEMORY;
        }

        if(SUCCEEDED(hr))
        {
            CopyMemory(m_pwfxFormat, pwfxFormat, sizeof(*pwfxFormat) + pwfxFormat->cbSize);
        }

        if(SUCCEEDED(hr))
        {
            if(!(m_pvAudioData = new BYTE [m_dwAudioDataSize]))
            {
                hr = DSERR_OUTOFMEMORY;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(m_pwfxFormat);
        ASSERT(m_pvAudioData);
        ASSERT(m_dwAudioDataSize);
        
        AddRef();

        *ppvAudioData = m_pvAudioData;
        *pfInitialize = (1 == m_dwRefCount);
    }

    return hr;
}


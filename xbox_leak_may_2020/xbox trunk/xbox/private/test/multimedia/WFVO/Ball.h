/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Ball.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __BALL_H__
#define __BALL_H__

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "xtl.h"
#include "xgmath.h"

namespace WFVO {

//------------------------------------------------------------------------------
//	CAudioData
//------------------------------------------------------------------------------
class CAudioData
{
protected:
    LPWAVEFORMATEX          m_pwfxFormat;
    LPVOID                  m_pvAudioData;
    DWORD                   m_dwAudioDataSize;
    DWORD                   m_dwRefCount;

public:
    CAudioData(void);
    virtual ~CAudioData(void);

public:
    HRESULT CreateFile(LPCSTR pszFile, LPCWAVEFORMATEX *ppwfxFormat, LPVOID *ppvAudioData, LPDWORD pdwAudioDataSize);
    HRESULT CreateEmpty(LPCWAVEFORMATEX pwfxFormat, DWORD dwAudioDataSize, LPVOID *ppvAudioData, LPBOOL pfInitialize);
    DWORD AddRef(void);
    DWORD Release(void);
};

__inline DWORD CAudioData::AddRef(void)
{
    return ++m_dwRefCount;
}

__inline DWORD CAudioData::Release(void)
{
    if(m_dwRefCount)
    {
        return --m_dwRefCount;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
//	CBall
//------------------------------------------------------------------------------
class CBall
{
public:

	struct Vertex
	{
		XGVECTOR3	position;
		float		tu, tv;
        XGVECTOR3   vTangent;
        XGVECTOR3   vBinormal;
        XGVECTOR3   vNormal;
	};

public:

	CBall(void);
	virtual ~CBall(void);
	
	HRESULT Create(IDirect3DDevice8*, float, float, int, int, BOOL, DWORD);
	void SetPosition(const XGVECTOR3&);
	void SetDirection(const XGVECTOR3&);
	void SetSpeed(float);
	float GetSpeed(void) const;
	const XGVECTOR3& GetDirection(void) const;
	void Move(float);
	void Render(IDirect3DDevice8*, XGMATRIX*, XGVECTOR3*, D3DLIGHT8*, D3DCOLORVALUE*, UINT);

	void DrawSolid(void);
	void DrawReduced(void);
	void DrawWireframe(void);
	void DrawPoints(void);

	const XGVECTOR3& GetMin(void) const;
	const XGVECTOR3& GetMax(void) const;
	int GetNumVerts(void) const;
	int GetNumTris(void) const;
	float GetRadius(void) const;
	const XGVECTOR3& GetLocation(void) const;

    D3DLIGHT8* GetLight(void);
    void PlaceLightInRange(UINT uLight, BOOL bInRange);
	BOOL IsLightSource(void) const;

	void PlayBuffer(float volume /* 0.0-1.0 */, float pitch /* 0.0-1.0 */);
    void DisableAmbientAudio( void );

	float GetMass(void) const { return m_mass; };

private:

	void Release(void);

    HRESULT CreateTextureFromHeightMap(IDirect3DDevice8* pDevice, 
                            LPCSTR szImage, IDirect3DTexture8** ppd3dt);
    HRESULT CreateLightTexture(IDirect3DDevice8* pDevice, UINT uLength, 
                            IDirect3DTexture8** ppd3dt);

    BOOL ComputeTangentTransforms(Vertex* prVertices, LPWORD pwIndices, 
                            UINT uNumIndices, BOOL bInterpolate);
    BOOL CalculateTangentTerms(XGVECTOR3* pvTangent, XGVECTOR3* pvBinormal, 
                            Vertex* prVertices, LPWORD pwIndices, 
                            UINT uNumIndices, BOOL bInterpolate);

	HRESULT InitAudio(void);
    HRESULT CreateSubMixDestination(void);
    HRESULT CreateCollisionSound(void);
    HRESULT CreateAmbientSound(void);
    void RenderAudio(void);

    LONG CalculatePitch(DWORD dwFrequency);

private:

	D3DMATERIAL8			m_material;
    D3DLIGHT8               m_light;
    D3DLIGHT8               m_lightDark;
	IDirect3DVertexBuffer8*	m_pVB;
	int						m_numVertices;
	IDirect3DIndexBuffer8*	m_pIB;
	int						m_numTriangles;
	XGVECTOR3				m_min;
	XGVECTOR3				m_max;
	BOOL					m_dontDraw;
	XGMATRIX				m_worldMatrix;
	XGVECTOR3				m_direction;
	float					m_speed;
	float					m_radius;
	XGVECTOR3				m_location;
	DWORD					m_fillMode;
	LPDIRECTSOUNDBUFFER     m_pCollisionSound;
    LPDIRECTSOUNDBUFFER     m_pAmbientSound;
    LPDIRECTSOUNDBUFFER     m_pSubMix;
    static CAudioData       m_CollisionSoundData;
    CAudioData *            m_pCollisionSoundData;
    static CAudioData       m_AmbientSoundData;
    CAudioData *            m_pAmbientSoundData;
	int						m_detail;
    BOOL                    m_bLightSource;
    DWORD                   m_dwInLightRange[4];
	float					m_mass;
    DWORD                   m_dwID;
    DWORD                   m_dwCollisionFrequency;

    static UINT             m_uRef;
    static IDirect3DTexture8* m_pd3dtBase;
    static IDirect3DTexture8* m_pd3dtBump;
    static IDirect3DTexture8* m_pd3dtLight;
    static IDirect3DTexture8* m_pd3dtWhite;
    static IDirect3DTexture8* m_pd3dtFlat;
};
}
#endif

///////////////////////////////////////////////////////////////////////////////
// File: Shield.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef SHIELD_H
#define SHIELD_H


#include "qrand.h"



// The vertex list holding the unit sphere coordinates remain on the card.
// Each frame another list is streamed.

struct ShieldVertex
{
	D3DVECTOR position;
	D3DVECTOR normal;
};




// The shields actually look like they are the surface of a sphere that has been cut
// off by cuts perpendicular/parallel to each other and in the direction from
// the center of the shield to the center of the sphere. IOW, a piece bounded by
// lat and long lines where the center is on the equator.
   

class Shield
{
protected:
	D3DMATRIX				m_CurOTPMatrix;
	D3DMATRIX				m_CurMatrix;
	D3DVECTOR				m_CurCenter;	// world coordinates
	D3DVECTOR				m_ObjectCenter;	// object coordinates

	D3DMATRIX				m_StartRotation;
	D3DVECTOR				m_RotationDir;
	FLOAT					m_ThetaZero;	// start angle offset
	FLOAT					m_Speed;


	float					m_RadiusScale;

	static D3DVECTOR		ms_Pos;		// position of center


public:
	void Init();
	void UnInit()	{ destroy(); }

	void create();
	void destroy();

	void render(const D3DMATRIX& mat_wtp);	// sets object to world matrix constants
	void advanceTime(float fElapsedTime, float fDt);

	void restart(float radian_extent);	// pairs of floats


	const D3DVECTOR& getCenter() const { return m_CurCenter; }

	void		setRadiusScale(float rs, float dist)
	{
		m_RadiusScale = rs;
		Set(&m_ObjectCenter, dist*rs, 0.0f, 0.0f);
	}

	float	getRadiusScale()	const	{ return m_RadiusScale; }
	float	getSpeed()			const	{ return m_Speed; }

	const D3DMATRIX&	getObjectToWorldMatrix()	const	{ return m_CurMatrix; }

	static void sSetCenter(const D3DVECTOR& pos) { ms_Pos = pos; }
};



class ZShield
{
protected:
    LPDIRECT3DVERTEXBUFFER8 m_pShieldVB;
    LPDIRECT3DINDEXBUFFER8  m_pShieldIB;
    DWORD					m_dwNumVertices;
    DWORD					m_dwNumIndices;

	D3DMATRIX				m_CurMatrix;
	float					m_Speed;
	float					m_Theta;

public:
	void Init();
	void UnInit()	{ destroy(); }

	void create();
	void destroy();

	void render(const D3DMATRIX& mat_wtp);
	void advanceTime(float fElapsedTime, float fDt);

	void restart(float start_radian, float end_radian, float outside_radius);
};





class ShieldMgr
{
protected:
	enum { MAX_SHIELDS = 3 };
	enum { MAX_ZSHIELDS = 5 };
	Shield					m_Shields[MAX_SHIELDS];
	ZShield					m_ZShields[MAX_ZSHIELDS];
	int						m_NumShields;
	int						m_NumZShields;

	void					restartShields();


    LPDIRECT3DVERTEXBUFFER8 m_pShieldVB;
    LPDIRECT3DINDEXBUFFER8  m_pShieldIB;
    DWORD					m_dwNumVertices;
    DWORD					m_dwNumIndices;

    DWORD                   m_dwVShader;
    DWORD					m_dwPShader;



	D3DVECTOR				m_Pos;		// position of center
	float					m_RadiusScale;
	float					m_MidRadius;	// assumes full scale


public:
	void Init();
	void UnInit()
    {
        int i;

        for (i = 0; i < MAX_SHIELDS; i++)
        {
            m_Shields[i].UnInit();
        }

        for (int i = 0; i < MAX_ZSHIELDS; i++)
        {
            m_ZShields[i].UnInit();
        }

        destroy();
    }


	void create();
	void destroy();

	void advanceTime(float fElapsedTime, float fDt);	// resets if fElapsedTime is zero

	void render(bool b_far_side);

	void restart();
};



#endif // SHIELD_H

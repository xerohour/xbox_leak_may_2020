///////////////////////////////////////////////////////////////////////////////
// File: Shield.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include "precomp.h"
#include "xbs_math.h"
#include "xbs_app.h"
#include "qrand.h"
#include "Shield.h"
#include "tex_gen.h"

///////////////////////////////////////////////////////////////////////////////
D3DVECTOR Shield::ms_Pos;
const float SHIELD_ROTATION_RATE = (2.0f);
///////////////////////////////////////////////////////////////////////////////
void Shield::Init()
{
	m_RadiusScale = 1.0f;
	m_Speed = 1.0f;
}
///////////////////////////////////////////////////////////////////////////////
void Shield::create()
{
}
///////////////////////////////////////////////////////////////////////////////
void Shield::destroy()
{
	// Nothing to do.
}
///////////////////////////////////////////////////////////////////////////////
void Shield::render(const D3DMATRIX& mat_wtp)
{
	D3DMATRIX transp;
	SetTranspose(m_CurMatrix, &transp);
	gpd3dDev->SetVertexShaderConstant(0,(CONST void *)&transp,4);
	// The ShieldMgr will do the rest of the rendering.
}
///////////////////////////////////////////////////////////////////////////////
void Shield::advanceTime(float fElapsedTime, float fDt)
{
	D3DVECTOR4 quat;

	float pushout_radius = max(0.0f, (PUSHOUT_START_TIME + PUSHOUT_DELTA - fElapsedTime) * OO_PUSHOUT_DELTA);
	pushout_radius = START_PUSHOUT_RADIUS * pushout_radius*pushout_radius;

//MTS	if (gApp.getPulseIntensity() > 0.0f) m_Speed += fDt * 1.65f;
	m_Speed += fDt * 0.8f;

	float theta = fDt * SHIELD_ROTATION_RATE * m_Speed + m_ThetaZero;
	m_ThetaZero = theta;

	SetQuatFromAxis(m_RotationDir, theta, &quat);
	D3DMATRIX mat;
	SetRotationFromRHQuat(quat, &mat);
	MulMats(m_StartRotation, mat, &m_CurMatrix);

	m_CurMatrix._11 *= m_RadiusScale;
	m_CurMatrix._12 *= m_RadiusScale;
	m_CurMatrix._13 *= m_RadiusScale;
	m_CurMatrix._21 *= m_RadiusScale;
	m_CurMatrix._22 *= m_RadiusScale;
	m_CurMatrix._23 *= m_RadiusScale;
	m_CurMatrix._31 *= m_RadiusScale;
	m_CurMatrix._32 *= m_RadiusScale;
	m_CurMatrix._33 *= m_RadiusScale;
//MTS	m_CurMatrix._41 = ms_Pos.x;
//MTS	m_CurMatrix._42 = ms_Pos.y;
//MTS	m_CurMatrix._43 = ms_Pos.z;
	// Adjusted so that the reflection is now stationary as the shield moves around.
	m_CurMatrix._41 = ms_Pos.x + m_CurMatrix._11 * (2.0f + pushout_radius);
	m_CurMatrix._42 = ms_Pos.y + m_CurMatrix._12 * (2.0f + pushout_radius);
	m_CurMatrix._43 = ms_Pos.z + m_CurMatrix._13 * (2.0f);

//MTS	bool b_far_side = m_CurCenter.y > ms_Pos.y;
	TransformPoint(m_ObjectCenter, m_CurMatrix, &m_CurCenter);
}
///////////////////////////////////////////////////////////////////////////////
void Shield::restart(float radian_extent)
{
	float crossing_radian = gApp.fRand01() * 2.09f * Pi;

	// Find the constants to make this happen.
	// First pick a "peak" orientation.
	const float f_RY_ARC = Pi * 1.2f;
	bool b_flipped = false;

	float rz = gApp.fRand01() * 2.0f * Pi;
	float ry = gApp.fRand01() * f_RY_ARC * 2.0f - f_RY_ARC*0.5f;

	if (ry > f_RY_ARC * 0.5f)
	{
		ry += Pi - f_RY_ARC;
		b_flipped = true;
	}

	if (b_flipped)
	{
		m_ThetaZero = rz + Pi - crossing_radian;
	}
	else
	{
		m_ThetaZero = - rz - crossing_radian;
	}

//MTS	char buf[512];
//MTS	sprintf(buf, "Ry=%f, Rz=%f, theta_zero=%f, crossing_radian=%f\n",
//MTS		ry, rz, m_ThetaZero, crossing_radian);
//MTS	OutputDebugString(buf);

	// Rotate around ry
	// Rotate around rz
	D3DMATRIX mat1, mat2;
	SetYRotation(ry, &mat1);
	SetZRotation(rz, &mat2);
	MulMats(mat1, mat2, &m_StartRotation);
	m_RotationDir.x = m_StartRotation._31;
	m_RotationDir.y = m_StartRotation._32;
	m_RotationDir.z = m_StartRotation._33;
//MTS	sprintf(buf, "    RotationDir=%+f,%+f,%+f\n",
//MTS		m_RotationDir.x, m_RotationDir.y, m_RotationDir.z);
//MTS	OutputDebugString(buf);

	m_Speed = 0.0f;
	
	advanceTime(0.0f, 0.0f);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ZShield::Init()
{
	m_pShieldVB = NULL;
	m_pShieldIB = NULL;
	m_dwNumVertices = m_dwNumIndices = 0;
	m_Speed = 0.0f;
	m_Theta = 0.0f;
}
///////////////////////////////////////////////////////////////////////////////
void ZShield::create()
{
}
///////////////////////////////////////////////////////////////////////////////
void ZShield::destroy()
{
#define XBS_RELEASE(a) if (a) a->Release(); a = NULL;
	XBS_RELEASE(m_pShieldVB);
	XBS_RELEASE(m_pShieldIB);
#undef XBS_RELEASE
	m_dwNumVertices = 0;
	m_dwNumIndices = 0;
}
///////////////////////////////////////////////////////////////////////////////
void ZShield::render(const D3DMATRIX& mat_wtp)
{
	D3DMATRIX transp;
	SetTranspose(m_CurMatrix, &transp);
	gpd3dDev->SetVertexShaderConstant(0,(CONST void *)&transp,4);

	gpd3dDev->SetStreamSource(0, m_pShieldVB, sizeof(ShieldVertex));
	gpd3dDev->SetIndices(m_pShieldIB, 0);
	gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, m_dwNumVertices,  0, m_dwNumIndices-2 );
}
///////////////////////////////////////////////////////////////////////////////
void ZShield::advanceTime(float fElapsedTime, float fDt)
{
//MTS	if (gApp.getPulseIntensity() > 0.0f) m_Speed += fDt * 1.0f;
	m_Speed += fDt * 0.8f;
	m_Theta += m_Speed * fDt;


	float pushout_radius = max(0.0f, (PUSHOUT_START_TIME + PUSHOUT_DELTA - fElapsedTime) * OO_PUSHOUT_DELTA);
	pushout_radius = START_PUSHOUT_RADIUS * pushout_radius*pushout_radius;

	SetZRotation(m_Theta, &m_CurMatrix);
	m_CurMatrix._41 += m_CurMatrix._11 * (2.0f + pushout_radius);
	m_CurMatrix._42 += m_CurMatrix._12 * (2.0f + pushout_radius);
	m_CurMatrix._43 += m_CurMatrix._13 * (2.0f);
}
///////////////////////////////////////////////////////////////////////////////
// returns the new start_radian, from bottom (-Pi/2) up.
void ZShield::restart(float start_radian, float end_radian, float outside_radius)
{
	destroy();

	m_Theta = gApp.fRand01() * 2.0f * Pi;
	m_Speed = 0.0f;


	// Create the index and vertex buffers.
	const int width = 8;		// number of panels, one less than the number of vertices
	const int height = 6;
	const float inside_radius = outside_radius - 0.5f;
	const float f_vert_radians = end_radian - start_radian;
	const float f_horiz_radians = 1.2f;

	int num_verts_per_face = (height+1) * (width+1);
	int num_side_verts = 2*2*(height+1) + 2*2*(width+1);
	m_dwNumVertices = 
				2*num_verts_per_face +			// top and bottom
				num_side_verts;					// edges

	m_dwNumIndices =
				GetNumberOfIndicesForTristripMesh(width, height, false, true) +
				GetNumberOfIndicesForTristripMesh(width, height, true, true) +
				(height+1)*2 + 2 +
				(width+1)*2 + 2 +
				(height+1)*2 + 2 +
				(width+1)*2 + 1;

	gpd3dDev->CreateVertexBuffer( m_dwNumVertices * sizeof(ShieldVertex), 0, 0, 0, &m_pShieldVB);
	gpd3dDev->CreateIndexBuffer(  m_dwNumIndices  * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pShieldIB);




	// Vertex index is (y*(width+1) + x) + ((inside_face) ? num_verts_per_face+num_side_verts : 0)
	// or for the sides num_verts_per_face + 2*(clockwise position from ll) + ((inside_face) ? 1 : 0


	ShieldVertex* p_verts;
	m_pShieldVB->Lock(0, 0, (BYTE**)&p_verts, 0);
	ShieldVertex* pverto = &p_verts[0];
	ShieldVertex* pverti = &p_verts[num_verts_per_face+num_side_verts];

	float f_left_rad   = -0.5f * f_horiz_radians;
	float f_right_rad  = +0.5f * f_horiz_radians;
	float f_top_rad    = end_radian;
	float f_bottom_rad = start_radian;
	float f_horiz_step = (f_right_rad-f_left_rad) / ((float)width);
	float f_vert_step  = (f_top_rad-f_bottom_rad) / ((float)height);

	int i,j;
	float f_i, f_j;

	for (j=0, f_j=f_bottom_rad; j<=height; j++, f_j+=f_vert_step)
	{
		float vs,vc;
		SinCos(f_j, &vs, &vc);
		for (i=0, f_i=f_left_rad; i<=width; i++, f_i+=f_horiz_step)
		{
			float hs,hc;
			SinCos(f_i, &hs, &hc);

			D3DVECTOR norm;
			Set(&norm, vc*hc, vc*hs, vs);

			Set(&pverto->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
			pverto->normal = norm;
			pverto++;
			Set(&pverti->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
			pverti->normal = norm;
			Scale(&pverti->normal, -1.0f);
			pverti++;

			if ((!j) && (i==width)) f_right_rad = f_i;		// set it exactly to what we iterate to
		}
		if (j==height) f_top_rad = f_j;						// set it exactly to what we iterate to
	}

	ShieldVertex* pvert = pverto;


	// Fill in the vertices around the edges.
	f_i = f_left_rad;
	float vs,vc,hs,hc;

	// Start at left, move up.
	SinCos(f_i, &hs, &hc);
	for (j=0, f_j=f_bottom_rad; j<=height; j++, f_j+=f_vert_step)
	{
		SinCos(f_j, &vs, &vc);

		D3DVECTOR norm, side;
		Set(&norm, vc*hc, vc*hs, vs);
		Set(&side, hs, -hc, 0.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}
	j = height;
	f_j = f_top_rad;

	// At UL, move right
	for (i=0, f_i=f_left_rad; i<=width; i++, f_i+=f_horiz_step)
	{
		SinCos(f_i, &hs, &hc);

		D3DVECTOR norm, side;
		Set(&norm, vc*hc, vc*hs, vs);
		Set(&side, -vs*hc, -vs*hs, vc);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// At UR, move down
	for (j=height, f_j=f_top_rad; j>=0; j--, f_j-=f_vert_step)
	{
		if (!j) f_j=f_bottom_rad;
		SinCos(f_j, &vs, &vc);

		D3DVECTOR norm, side;
		Set(&norm, vc*hc, vc*hs, vs);
		Set(&side, -hs, hc, 0.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// At LR, move left
	for (i=width, f_i=f_right_rad; i>=0; i--, f_i-=f_horiz_step)
	{
		if (!i) f_i=f_left_rad;
		SinCos(f_i, &hs, &hc);

		D3DVECTOR norm, side;
		Set(&norm, vc*hc, vc*hs, vs);
		Set(&side, vs*hc, vs*hs, -vc);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// Done with the vertices!
	m_pShieldVB->Unlock();






	WORD* p_indices;
	m_pShieldIB->Lock(0, 0, (BYTE**)&p_indices, 0);

	// Vertex index is (y*(width+1) + x) + ((inside_face) ? num_verts_per_face : 0)
	// or for the sides 2*num_verts_per_face + 2*(clockwise position from ll) + ((inside_face) ? 1 : 0

	// Outside surface.
	int index_num = 0;
	index_num += CreateTristripForMesh(&p_indices[index_num], width, height, false, true, 0);
	index_num += CreateTristripForMesh(&p_indices[index_num], width, height,  true, true, num_verts_per_face+num_side_verts+width, 0, -1);

	int vertex_index = num_verts_per_face;

	// Sides
	for (i=0; i<4; i++)
	{
		p_indices[index_num++] = (WORD)vertex_index;	// first tap of a double-tap

		int length = (i&1) ? width : height;
		for (j=0; j<=length; j++)
		{
			p_indices[index_num++] = vertex_index + 0;
			p_indices[index_num++] = vertex_index + 1;
			vertex_index += 2;
		}
		if (i<3) p_indices[index_num++] = vertex_index - 1;	// double-tap
	}

	m_pShieldIB->Unlock();

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::Init()
{
    int i;

    for (i = 0; i < MAX_SHIELDS; i++)
    {
        m_Shields[i].Init();
    }

    for (int i = 0; i < MAX_ZSHIELDS; i++)
    {
        m_ZShields[i].Init();
    }

	m_NumShields = 0;
	m_pShieldVB = NULL;
	m_pShieldIB = NULL;
	m_dwNumVertices = 0;
	m_dwNumIndices = 0;
	m_dwPShader = 0;
	m_dwVShader = 0;

	Set(&m_Pos, 0.0f, 0.0f, 0.0f);
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::create()
{
	// Make the meshes and stuff.
	const int width = 8;		// number of panels, one less than the number of vertices
	const int height = 6;
	const float inside_radius = 13.1f;
	const float outside_radius = 14.0f;
	const float f_vert_dim = 0.9f;
	const float f_horiz_dim = 1.2f;
	// Partial sphere surface is made by generating a mesh at x=1.0f, and normalizing the vertices.

	m_RadiusScale = 1.0f - 1.2f * (outside_radius-inside_radius) / outside_radius;
	m_MidRadius = (inside_radius + outside_radius) * 0.5f;

	int num_verts_per_face = (height+1) * (width+1);
	int num_side_verts = 2*2*(height+1) + 2*2*(width+1);
	m_dwNumVertices = 
				2*num_verts_per_face +			// top and bottom
				num_side_verts;					// edges

	m_dwNumIndices =
				GetNumberOfIndicesForTristripMesh(width, height, false, true) +
				GetNumberOfIndicesForTristripMesh(width, height, true, true) +
				(height+1)*2 + 2 +
				(width+1)*2 + 2 +
				(height+1)*2 + 2 +
				(width+1)*2 + 1;

	gpd3dDev->CreateVertexBuffer( m_dwNumVertices * sizeof(ShieldVertex), 0, 0, 0, &m_pShieldVB);
	gpd3dDev->CreateIndexBuffer(  m_dwNumIndices  * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pShieldIB);




	// Vertex index is (y*(width+1) + x) + ((inside_face) ? num_verts_per_face+num_side_verts : 0)
	// or for the sides num_verts_per_face + 2*(clockwise position from ll) + ((inside_face) ? 1 : 0


	ShieldVertex* p_verts;
	m_pShieldVB->Lock(0, 0, (BYTE**)&p_verts, 0);
	ShieldVertex* pverto = &p_verts[0];
	ShieldVertex* pverti = &p_verts[num_verts_per_face+num_side_verts];

	float f_left_c		= -0.5f * f_horiz_dim;
	float f_right_c		= +0.5f * f_horiz_dim;
	float f_top_c		= +0.5f * f_vert_dim;
	float f_bottom_c	= -0.5f * f_vert_dim;
	float f_horiz_step	= (f_right_c-f_left_c) / ((float)width);
	float f_vert_step	= (f_top_c-f_bottom_c) / ((float)height);

	int i,j;
	float f_i, f_j;

	for (j=0, f_j=f_bottom_c; j<=height; j++, f_j+=f_vert_step)
	{
		for (i=0, f_i=f_left_c; i<=width; i++, f_i+=f_horiz_step)
		{
			D3DVECTOR norm;
			Set(&norm, 1.0f, f_i, f_j);
			Normalize(&norm);

			Set(&pverto->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
			pverto->normal = norm;
			pverto++;
			Set(&pverti->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
			pverti->normal = norm;
			Scale(&pverti->normal, -1.0f);
			pverti++;

			if ((!j) && (i==width)) f_right_c = f_i;		// set it exactly to what we iterate to
		}
		if (j==height) f_top_c = f_j;						// set it exactly to what we iterate to
	}

	ShieldVertex* pvert = pverto;


	// Fill in the vertices around the edges.
	f_i = f_left_c;

	// Start at left, move up.
	for (j=0, f_j=f_bottom_c; j<=height; j++, f_j+=f_vert_step)
	{
		D3DVECTOR norm, side;
		Set(&norm, 1.0f, f_i, f_j);
		Normalize(&norm);

		Set(&side, 0.0f, -1.0f, 0.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}
	j = height;
	f_j = f_top_c;

	// At UL, move right
	for (i=0, f_i=f_left_c; i<=width; i++, f_i+=f_horiz_step)
	{
		D3DVECTOR norm, side;
		Set(&norm, 1.0f, f_i, f_j);
		Normalize(&norm);

		Set(&side, 0.0f, 0.0f, +1.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// At UR, move down
	f_i = f_right_c;
	for (j=height, f_j=f_top_c; j>=0; j--, f_j-=f_vert_step)
	{
		if (!j) f_j=f_bottom_c;

		D3DVECTOR norm, side;
		Set(&norm, 1.0f, f_i, f_j);
		Normalize(&norm);

		Set(&side, 0.0f, +1.0f, 0.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// At LR, move left
	f_j = f_bottom_c;
	for (i=width, f_i=f_right_c; i>=0; i--, f_i-=f_horiz_step)
	{
		if (!i) f_i=f_left_c;

		D3DVECTOR norm, side;
		Set(&norm, 1.0f, f_i, f_j);
		Normalize(&norm);

		Set(&side, 0.0f, 0.0f, -1.0f);

		Set(&pvert->position, outside_radius*norm.x, outside_radius*norm.y, outside_radius*norm.z);
		pvert->normal = side;
		pvert++;
		Set(&pvert->position,  inside_radius*norm.x,  inside_radius*norm.y,  inside_radius*norm.z);
		pvert->normal = side;
		pvert++;
	}

	// Done with the vertices!
	m_pShieldVB->Unlock();






	WORD* p_indices;
	m_pShieldIB->Lock(0, 0, (BYTE**)&p_indices, 0);

	// Vertex index is (y*(width+1) + x) + ((inside_face) ? num_verts_per_face : 0)
	// or for the sides 2*num_verts_per_face + 2*(clockwise position from ll) + ((inside_face) ? 1 : 0

	// Outside surface.
	int index_num = 0;
	index_num += CreateTristripForMesh(&p_indices[index_num], width, height, false, true, 0);
	index_num += CreateTristripForMesh(&p_indices[index_num], width, height,  true, true, num_verts_per_face+num_side_verts+width, 0, -1);

	int vertex_index = num_verts_per_face;

	// Sides
	for (i=0; i<4; i++)
	{
		p_indices[index_num++] = (WORD)vertex_index;	// first tap of a double-tap

		int length = (i&1) ? width : height;
		for (j=0; j<=length; j++)
		{
			p_indices[index_num++] = vertex_index + 0;
			p_indices[index_num++] = vertex_index + 1;
			vertex_index += 2;
		}
		if (i<3) p_indices[index_num++] = vertex_index - 1;	// double-tap
	}

	m_pShieldIB->Unlock();



	// Create the shaders.

	
	// Initialize the pixel shaders.
    if( m_dwPShader )
    {
        gpd3dDev->DeletePixelShader( m_dwPShader );
        m_dwPShader = 0;
    }
#ifndef BINARY_RESOURCE
	m_dwPShader = gApp.loadPixelShader("D:\\Shaders\\shield.xpu");
#else // BINARY_RESOURCE
	m_dwPShader = gApp.loadPixelShader(g_shield_xpu);
#endif // BINARY_RESOURCE

	

	// Initialize the vertex shaders.
    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),	// normal
        D3DVSD_END()
    };
	if (m_dwVShader)
	{
		gpd3dDev->DeleteVertexShader( m_dwVShader );
		m_dwVShader = 0;
	}
#ifndef BINARY_RESOURCE
	m_dwVShader = gApp.loadVertexShader("D:\\Shaders\\shield.xvu", dwShaderVertexDecl);
#else // BINARY_RESOURCE
	m_dwVShader = gApp.loadVertexShader(g_shield_xvu, dwShaderVertexDecl);
#endif // BINARY_RESOURCE



	for (i=0; i<MAX_SHIELDS; i++) m_Shields[i].create();
	for (i=0; i<MAX_ZSHIELDS; i++) m_ZShields[i].create();


	restart();
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::destroy()
{
	for (int i=0; i<m_NumShields; i++) m_Shields[i].destroy();
	m_NumShields = 0;

    if (m_dwPShader) gpd3dDev->DeletePixelShader(  m_dwPShader );
	if (m_dwVShader) gpd3dDev->DeleteVertexShader( m_dwVShader );
    m_dwPShader = 0;
	m_dwVShader = 0;

#define XBS_RELEASE(a) if (a) a->Release(); a = NULL;
	XBS_RELEASE(m_pShieldVB);
	XBS_RELEASE(m_pShieldIB);
#undef XBS_RELEASE
	m_dwNumVertices = 0;
	m_dwNumIndices  = 0;
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::render(bool b_far_side)
{
	// Set default states
    gpd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    gpd3dDev->SetRenderState( D3DRS_ZENABLE,  TRUE );
    gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );

    gpd3dDev->SetTexture( 0, gApp.pStaticReflectionCubeMap );
	gpd3dDev->SetTexture( 1, gApp.pNormalCubeMapHiRes );
	gpd3dDev->SetTexture( 2, gApp.pNormalCubeMapHiRes );
	gpd3dDev->SetTexture( 3, gApp.pNormalCubeMapHiRes );


    gpd3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

	gpd3dDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

	gpd3dDev->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

	
	gpd3dDev->SetTextureStageState( 3, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 3, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 3, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);


    gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    gpd3dDev->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

    gpd3dDev->SetTexture( 0, gApp.pStaticReflectionCubeMap );
	gpd3dDev->SetTexture( 1, gApp.pNormalCubeMapHiRes );
	gpd3dDev->SetTexture( 2, gApp.pNormalCubeMapHiRes );
	gpd3dDev->SetTexture( 3, gApp.pNormalCubeMapHiRes );



    gpd3dDev->SetVertexShader( m_dwVShader );
    gpd3dDev->SetPixelShader(  m_dwPShader );




	D3DVECTOR cam_pos, look_dir, look_at;
	gApp.theCamera.getCameraPos (&cam_pos);
	gApp.theCamera.getCameraLook(&look_at);
	Sub(look_at, cam_pos, &look_dir);


	//; Expected vertex shaders constants
	//;    c0-c3    = Transpose of object to world matrix
	//;    c4-c7    = Transpose of view*projection matrix
	//;    c8       = some constants, x=0, y=1, z=2, w=0.5
	//;    c9       = eye location in world space
	//;    c10      = light pos in world space

    // Note: when passing matrices to a vertex shader, we transpose them, since
    // matrix multiplies are done with dot product operations on the matrix rows.
	D3DMATRIX matFinal,matWTP;
	MulMats(gApp.theCamera.matWTC,gApp.theCamera.matProj,&matWTP);
	SetTranspose(matWTP,&matFinal);
	gpd3dDev->SetVertexShaderConstant(4,(CONST void *)&matFinal,4);

	// Constants
	D3DVECTOR4 val[4];
	Set(&val[0], 0.0f, 1.0f, 2.0f, 0.5f);

	// Eye position
	Set(&val[1], cam_pos.x, cam_pos.y, cam_pos.z, 0.0f);

	// Blob Light position
	Set(&val[2],0.f,0.f,0.f,1.f);

	// Mood Light position
	const D3DVECTOR &mlp = gApp.moodLight.Position;
	Set(&val[3],mlp.x,mlp.y,mlp.z,1.f);

	gpd3dDev->SetVertexShaderConstant( 8, &val[0], 4 );

	// alpha
	float f_shading = 0.75f;	// 1 = black, 0 = no shading
	if (gApp.getElapsedTime() < SHIELD_FADE_IN_START_TIME + SHIELD_FADE_IN_DELTA)
	{
		f_shading *= (gApp.getElapsedTime() - SHIELD_FADE_IN_START_TIME) * OO_SHIELD_FADE_IN_DELTA;
	}
	else if (gApp.getElapsedTime() > SHIELD_FADE_OUT_START_TIME)
	{
		f_shading *= (SHIELD_FADE_OUT_START_TIME + SHIELD_FADE_OUT_DELTA - gApp.getElapsedTime()) * OO_SHIELD_FADE_OUT_DELTA;
	}
	f_shading = min(1.0f, max(0.0f, f_shading));
	Set(&val[0], 0.0f, 0.0f, 0.0f, f_shading);	// final alpha is 1.0f - f_shading

	// blob light
	float f_intensity = gApp.getBlobIntensity() * 2.f;
	float fscale = max(0.0f, min(1.0f, (gApp.getElapsedTime()-PUSHOUT_START_TIME) * OO_PUSHOUT_DELTA));
	f_intensity *= fscale*fscale;
	
	Set(&val[1], f_intensity, f_intensity, f_intensity, f_intensity);

	// Specular coefficient
	Set(&val[2], 0.4f,1.f,0.3f,1.f);
	gpd3dDev->SetPixelShaderConstant( 0, &val[0], 3 );


	gpd3dDev->SetIndices( m_pShieldIB, 0 );
    gpd3dDev->SetStreamSource( 0, m_pShieldVB, sizeof(ShieldVertex) );


	float shield_dot[MAX_SHIELDS];
	int shield_order[MAX_SHIELDS];
	int i;
	for (i=0; i<m_NumShields; i++)
	{
		shield_dot[i] = Dot(m_Shields[i].getCenter(), look_dir);
		shield_order[i] = i;
	}


	float f_blob_dot = Dot(m_Pos, look_dir);
	for (int j=m_NumShields-1; j>=0; j--)
	{
		int i = (b_far_side) ? m_NumShields-1-j : j;
		if (  (( b_far_side) && (shield_dot[i]>=f_blob_dot)) ||
			  ((!b_far_side) && (shield_dot[i]< f_blob_dot)) )
		{
			m_Shields[shield_order[i]].render(matWTP);
			gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, m_dwNumVertices,  0, m_dwNumIndices-2 );

		}
	}

	for (int j=m_NumZShields-1; j>=0; j--)
	{
		if (b_far_side) break;
		// Oh, don't bother sorting, see how it looks.
		m_ZShields[j].render(matWTP);	// sets the object to world transpose transform
	}


    // Restore the state
    gpd3dDev->SetPixelShader(  NULL );
    gpd3dDev->SetVertexShader( NULL );

	gpd3dDev->SetTexture(0, NULL);
	gpd3dDev->SetTexture(1, NULL);

    gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
	 gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::advanceTime(float fElapsedTime, float fDt)
{
	int i;
	for (i=0; i<m_NumShields; i++) m_Shields[i].advanceTime(fElapsedTime, fDt);
	for (i=0; i<m_NumZShields; i++) m_ZShields[i].advanceTime(fElapsedTime, fDt);
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::restart()
{
	Set(&m_Pos, 0.0f, 0.0f, 1.0f);
	Shield::sSetCenter(m_Pos);

	restartShields();
}
///////////////////////////////////////////////////////////////////////////////
void ShieldMgr::restartShields()
{
	float radian_extent = Pi / 6.0f;
	float scale = 1.0f;

	for (m_NumShields=0; m_NumShields<MAX_SHIELDS; m_NumShields++)
	{
		m_Shields[m_NumShields].restart(radian_extent);
		m_Shields[m_NumShields].setRadiusScale(scale, m_MidRadius);
		scale *= m_RadiusScale;
	}

	float min_rad = -0.45f * Pi;
	float max_rad = +0.45f * Pi;
	float rad_step = (max_rad-min_rad) / MAX_ZSHIELDS;
	for (m_NumZShields=0; m_NumZShields<MAX_ZSHIELDS; m_NumZShields++)
	{
		float mid_rad = min_rad + rad_step;
		m_ZShields[m_NumZShields].restart(min_rad, mid_rad, scale * m_MidRadius);
		min_rad = mid_rad;
	}
}
///////////////////////////////////////////////////////////////////////////////

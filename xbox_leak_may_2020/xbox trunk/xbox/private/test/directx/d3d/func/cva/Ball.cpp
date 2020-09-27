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

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "Ball.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	Vertex formats:
//------------------------------------------------------------------------------

#pragma pack(1)

struct VFP_D3DVSDT_FLOAT1 {	// 1D float expanded to (value, 0., 0., 1.)
	float x;
	float nx, ny, nz;
};
DWORD VFP_D3DVSDT_FLOAT1_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT1),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_FLOAT2 {	// 2D float expanded to (value, value, 0., 1.)
	float x, y;
	float nx, ny, nz;
};
DWORD VFP_D3DVSDT_FLOAT2_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT2),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_FLOAT3 {	// 3D float expanded to (value, value, value, 1.)
	float x, y, z;
	float nx, ny, nz;
};
DWORD VFP_D3DVSDT_FLOAT3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_FLOAT4 {	// 4D float
	float x, y, z, w;
	float nx, ny, nz;
};
DWORD VFP_D3DVSDT_FLOAT4_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT4),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_SHORT2 {	// 2D signed short expanded to (value, value, 0., 1.)  Signed shorts map to the range [-32768, 32767]
	short	x, y;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_SHORT2_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_SHORT2),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_SHORT4 {	// 4D signed short
	short	x, y, z, w;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_SHORT4_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_SHORT4),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_NORMSHORT1 {	// 1D signed, normalized short expanded to (value, 0, 0., 1.) (signed, normalized short maps from -1.0 to 1.0)
	short	x;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_NORMSHORT1_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_NORMSHORT1),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_NORMSHORT2 {	// 2D signed, normalized short expanded to (value, value, 0., 1.)
	short	x, y;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_NORMSHORT2_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_NORMSHORT2),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_NORMSHORT3 {	// 3D signed, normalized short expanded to (value, value, value, 1.)  
	short	x, y, z;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_NORMSHORT3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_NORMSHORT3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_NORMSHORT4 {	// 4D signed, normalized short expanded to (value, value, value, value)  
	short	x, y, z, w;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_NORMSHORT4_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_NORMSHORT4),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_NORMPACKED3 {	// 3 signed, normalized components packed in 32-bits.  (11,11,10).  Each component ranges from -1.0 to 1.0.  Expanded to (value, value, value, 1.)
	DWORD	xyz;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_NORMPACKED3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_NORMPACKED3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_SHORT1 {	// 1D signed short expanded to (value, 0., 0., 1.)  Signed shorts map to the range [-32768, 32767]
	short	x;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_SHORT1_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_SHORT1),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_SHORT3 {	// 3D signed short expanded to (value, value, value, 1.)
	short	x, y, z;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_SHORT3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_SHORT3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_PBYTE1 {	// 1D packed byte expanded to (value, 0., 0., 1.)  Packed bytes map to the range [0, 1]
	BYTE	x;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_PBYTE1_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_PBYTE1),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_PBYTE2 {	// 2D packed byte expanded to (value, value, 0., 1.)
	BYTE	x, y;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_PBYTE2_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_PBYTE2),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_PBYTE3 {	// 3D packed byte expanded to (value, value, value, 1.)
	BYTE	x, y, z;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_PBYTE3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_PBYTE3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_PBYTE4 {	// 4D packed byte expanded to (value, value, value, value) 
	BYTE	x, y, z, w;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_PBYTE4_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_PBYTE4),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFP_D3DVSDT_FLOAT2H {	// 2D homogeneous float expanded to (value, value,0., value.) Useful for projective texture coordinates.
	float	x, y, z;
	float	nx, ny, nz;
};
DWORD VFP_D3DVSDT_FLOAT2H_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT2H),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
	D3DVSD_END()
};

struct VFN_D3DVSDT_NORMSHORT3 {	// 3D signed, normalized short expanded to (value, value, value, 1.)  
	float	x, y, z;
	short	nx, ny, nz;
};
DWORD VFN_D3DVSDT_NORMSHORT3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_NORMSHORT3),
	D3DVSD_END()
};

struct VFN_D3DVSDT_NORMPACKED3 {	// 3 signed, normalized components packed in 32-bits.  (11,11,10).  Each component ranges from -1.0 to 1.0.  Expanded to (value, value, value, 1.)
	float	x, y, z;
	DWORD	nxyz;
};
DWORD VFN_D3DVSDT_NORMPACKED3_DECL[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_NORMPACKED3),
	D3DVSD_END()
};

#pragma pack()

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
	m_dontDraw	= TRUE;
	m_pVB		= NULL;
	m_pIB		= NULL;
	m_matrices	= NULL;
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
}

//------------------------------------------------------------------------------
//	CBall::Create
//------------------------------------------------------------------------------
void
CBall::Create(
			  IN IDirect3DDevice8*	pDevice,
			  IN int				positionType,
			  IN int				normalType,
			  IN float				radius,
			  IN int				numLong,
			  IN int				numLat
			  )
/*++

Routine Description:

	Creats a sphere whose center is at the origin with a given radius. There
	are several assumptions made in this routine. the primary one is that the
	radius of the sphere will never be larger than SHRT_MAX (32767). If it is,
	one of the compressed vertex formats will overflow. The other important 
	assumption is that the radius of the balls will be significantly large
	in the neighborhood of 10,000+. If the radius is too small, conversion
	errors can seriously distort the balls.

Arguments:

	IN pDevice -		D3D Device
	IN positionType -	Type of ball to create (vertex format of position)
	IN normalType -		Type of ball to create (vertex format of normal
	IN radius -			Radius of sphere
	IN numLong -		Number of longitutinal lines
	IN numLat -			Num latitutinal lines

Return Value:

	None

--*/
{
	float			phi;
	float			rho;
	float			phiInc;
	float			rhoInc;
	int				p;
	int				r;
	int				index;
	VOID*			pVoid		= NULL;
	D3DXVECTOR3*	position;
	D3DXVECTOR3*	normal;
	WORD*			indices		= NULL;
	int				latitude1	= numLat - 1;
	int				latitude2	= numLat - 2;
	int				longitude1	= numLong - 1;
	int				p1l;
	int				pl;
	int				last;
	UINT			bufferSize;
	DWORD*			decl;
	void*			vertices	= NULL;
	D3DXMATRIX		matrix;
	DWORD			x;
	DWORD			y;
	DWORD			z;
	int				i;

	// Release previous buffers
	Release();

	// Setup
	m_radius = radius;

	// Num verts and tris
	m_numVertices	= latitude1 * numLong + 2;
	m_numTriangles	= (latitude1 * 2) * numLong;
	
	// Allocate vertices
	position	= new D3DXVECTOR3 [m_numVertices];
	normal		= new D3DXVECTOR3 [m_numVertices];

	// Increments
	phiInc	= (2.0f * PI) / (float)numLong;
	rhoInc	= PI / (float)numLat;

	// Calculate vertices
	normal[0]	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	position[0]	= D3DXVECTOR3(0.0f, m_radius, 0.0f);
	for(index=1, phi=0.0f, p=0; p<numLong; ++p, phi += phiInc) 
	{
		for(rho=PI/2.0f - rhoInc, r=0; r<latitude1; ++r, rho-=rhoInc, ++index) 
		{
			normal[index] = D3DXVECTOR3((float)(cos(phi) * fabs(cos(rho))), 
										(float)sin(rho), 
										-(float)(sin(phi) * fabs(cos(rho))));
			position[index]	= D3DXVECTOR3(normal[index].x * m_radius, 
										  normal[index].y * m_radius, 
										  normal[index].z * m_radius);
		}
	}
	normal[index]	= D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	position[index]	= D3DXVECTOR3(0.0f, -m_radius, 0.0f);
	last = index;

	// Determine the type of vertices to use
	// 1D float expanded to (value, 0., 0., 1.)
	if((positionType == D3DVSDT_FLOAT1) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_FLOAT1);
		decl			= VFP_D3DVSDT_FLOAT1_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_FLOAT1*)vertices)[i].x	= position[i].x;
			((VFP_D3DVSDT_FLOAT1*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_FLOAT1*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_FLOAT1*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, position[i].y,
								  position[i].z);
		}
		m_drawTris = FALSE;
	}

	// 2D float expanded to (value, value, 0., 1.)
	else if((positionType == D3DVSDT_FLOAT2) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_FLOAT2);
		decl			= VFP_D3DVSDT_FLOAT2_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_FLOAT2*)vertices)[i].x	= position[i].x;
			((VFP_D3DVSDT_FLOAT2*)vertices)[i].y	= position[i].y;
			((VFP_D3DVSDT_FLOAT2*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_FLOAT2*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_FLOAT2*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, 0.0f, position[i].z);
		}
		m_drawTris = FALSE;
	}

	// 3D float expanded to (value, value, value, 1.)
	else if((positionType == D3DVSDT_FLOAT3) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_FLOAT3);
		decl			= VFP_D3DVSDT_FLOAT3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].x	= position[i].x;
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].y	= position[i].y;
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].z	= position[i].z;
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_FLOAT3*)vertices)[i].nz	= normal[i].z;
		}
		m_drawTris = TRUE;
	}

	// 4D float
	else if((positionType == D3DVSDT_FLOAT4) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_FLOAT4);
		decl			= VFP_D3DVSDT_FLOAT4_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].x	= position[i].x;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].y	= position[i].y;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].z	= position[i].z;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].w	= 1.0f;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_FLOAT4*)vertices)[i].nz	= normal[i].z;
		}
		m_drawTris = TRUE;
	}

	// 2D signed short expanded to (value, value, 0., 1.)  Signed shorts map to the range [-32768, 32767]
	else if((positionType == D3DVSDT_SHORT2) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_SHORT2);
		decl			= VFP_D3DVSDT_SHORT2_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_SHORT2*)vertices)[i].x	= (short)position[i].x;
			((VFP_D3DVSDT_SHORT2*)vertices)[i].y	= (short)position[i].y;
			((VFP_D3DVSDT_SHORT2*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_SHORT2*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_SHORT2*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, 0.0f, position[i].z);
		}
		m_drawTris = FALSE;
	}

	// 4D signed short
	else if((positionType == D3DVSDT_SHORT4) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_SHORT4);
		decl			= VFP_D3DVSDT_SHORT4_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_SHORT4*)vertices)[i].x	= (short)position[i].x;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].y	= (short)position[i].y;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].z	= (short)position[i].z;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].w	= 1;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_SHORT4*)vertices)[i].nz	= normal[i].z;
		}
		m_drawTris = TRUE;
	}

	// 1D signed, normalized short expanded to (value, 0, 0., 1.) (signed, normalized short maps from -1.0 to 1.0)
	else if((positionType == D3DVSDT_NORMSHORT1) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_NORMSHORT1);
		decl			= VFP_D3DVSDT_NORMSHORT1_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_NORMSHORT1*)vertices)[i].x	= ToShort(position[i].x, m_radius);
			((VFP_D3DVSDT_NORMSHORT1*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_NORMSHORT1*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_NORMSHORT1*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, position[i].y,
								  position[i].z);
			D3DXMatrixMultiply(&m_matrices[i], &m_matrices[i], 
							   D3DXMatrixScaling(&matrix, m_radius, 1.0f, 1.0f));
		}
		m_drawTris = FALSE;
	}

	// 2D signed, normalized short expanded to (value, value, 0., 1.)
	else if((positionType == D3DVSDT_NORMSHORT2) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_NORMSHORT2);
		decl			= VFP_D3DVSDT_NORMSHORT2_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_NORMSHORT2*)vertices)[i].x	= ToShort(position[i].x, m_radius);
			((VFP_D3DVSDT_NORMSHORT2*)vertices)[i].y	= ToShort(position[i].y, m_radius);
			((VFP_D3DVSDT_NORMSHORT2*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_NORMSHORT2*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_NORMSHORT2*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, 0.0f, position[i].z);
			D3DXMatrixMultiply(&m_matrices[i], &m_matrices[i], 
							   D3DXMatrixScaling(&matrix, m_radius, m_radius, 1.0f));
		}
		m_drawTris = FALSE;
	}

	// 3D signed, normalized short expanded to (value, value, value, 1.)  
	else if((positionType == D3DVSDT_NORMSHORT3) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_NORMSHORT3);
		decl			= VFP_D3DVSDT_NORMSHORT3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [1];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].x	= ToShort(position[i].x, m_radius);
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].y	= ToShort(position[i].y, m_radius);
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].z	= ToShort(position[i].z, m_radius);
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_NORMSHORT3*)vertices)[i].nz	= normal[i].z;
		}
		D3DXMatrixScaling(&m_matrices[0], m_radius, m_radius, m_radius);
		m_drawTris = TRUE;
	}

	// // 4D signed, normalized short expanded to (value, value, value, value)  
	else if((positionType == D3DVSDT_NORMSHORT4) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_NORMSHORT4);
		decl			= VFP_D3DVSDT_NORMSHORT4_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [1];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].x	= ToShort(position[i].x, m_radius);
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].y	= ToShort(position[i].y, m_radius);
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].z	= ToShort(position[i].z, m_radius);
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].w	= SHRT_MAX;
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_NORMSHORT4*)vertices)[i].nz	= normal[i].z;
		}
		D3DXMatrixScaling(&m_matrices[0], m_radius, m_radius, m_radius);
		m_drawTris = TRUE;
	}

	// 3 signed, normalized components packed in 32-bits.  (11,11,10).  Each component ranges from -1.0 to 1.0.  Expanded to (value, value, value, 1.)
	else if((positionType == D3DVSDT_NORMPACKED3) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_NORMPACKED3);
		decl			= VFP_D3DVSDT_NORMPACKED3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [1];
		for(i=0; i<m_numVertices; ++i) 
		{
			if(position[i].x >= 0.0f)
				x	= ((DWORD)(position[i].x / m_radius * 1023.0f)) & 0x7ff;
			else
				x	= ((DWORD)(position[i].x / m_radius * 1024.0f)) & 0x7ff;
			if(position[i].y >= 0.0f)
				y	= ((DWORD)(position[i].y / m_radius * 1023.0f)) & 0x7ff;
			else
				y	= ((DWORD)(position[i].y / m_radius * 1024.0f)) & 0x7ff;
			if(position[i].z >= 0.0f)
				z	= ((DWORD)(position[i].z / m_radius * 511.0f)) & 0x3ff;
			else
				z	= ((DWORD)(position[i].z / m_radius * 512.0f)) & 0x3ff;

			((VFP_D3DVSDT_NORMPACKED3*)vertices)[i].xyz	= (x << 21) | (y << 10) | z;
			((VFP_D3DVSDT_NORMPACKED3*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_NORMPACKED3*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_NORMPACKED3*)vertices)[i].nz	= normal[i].z;
		}
		D3DXMatrixScaling(&m_matrices[0], m_radius, m_radius, m_radius);
		m_drawTris = TRUE;
	}

	// 1D signed short expanded to (value, 0., 0., 1.)  Signed shorts map to the range [-32768, 32767]
	else if((positionType == D3DVSDT_SHORT1) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_SHORT1);
		decl			= VFP_D3DVSDT_SHORT1_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_SHORT1*)vertices)[i].x	= (short)position[i].x;
			((VFP_D3DVSDT_SHORT1*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_SHORT1*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_SHORT1*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixTranslation(&m_matrices[i], 0.0f, position[i].y,
								  position[i].z);
		}
		m_drawTris = FALSE;
	}

	// 3D signed short expanded to (value, value, value, 1.)
	else if((positionType == D3DVSDT_SHORT3) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_SHORT3);
		decl			= VFP_D3DVSDT_SHORT3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_SHORT3*)vertices)[i].x	= (short)position[i].x;
			((VFP_D3DVSDT_SHORT3*)vertices)[i].y	= (short)position[i].y;
			((VFP_D3DVSDT_SHORT3*)vertices)[i].z	= (short)position[i].z;
			((VFP_D3DVSDT_SHORT3*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_SHORT3*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_SHORT3*)vertices)[i].nz	= normal[i].z;
		}
		m_drawTris = TRUE;
	}

	// 1D packed byte expanded to (value, 0., 0., 1.)  Packed bytes map to the range [0, 1]
	else if((positionType == D3DVSDT_PBYTE1) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_PBYTE1);
		decl			= VFP_D3DVSDT_PBYTE1_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_PBYTE1*)vertices)[i].x	= ToOffsetBYTE(position[i].x, m_radius);
			((VFP_D3DVSDT_PBYTE1*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_PBYTE1*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_PBYTE1*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixScaling(&m_matrices[i], 2.0f * m_radius, 1.0f, 1.0f);
			D3DXMatrixMultiply(&m_matrices[i], &m_matrices[i],
							   D3DXMatrixTranslation(&matrix, -m_radius, position[i].y,
												     position[i].z));
		}
		m_drawTris = FALSE;
	}

	// 2D packed byte expanded to (value, value, 0., 1.)
	else if((positionType == D3DVSDT_PBYTE2) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_PBYTE2);
		decl			= VFP_D3DVSDT_PBYTE2_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [m_numVertices];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_PBYTE2*)vertices)[i].x	= ToOffsetBYTE(position[i].x, m_radius);
			((VFP_D3DVSDT_PBYTE2*)vertices)[i].y	= ToOffsetBYTE(position[i].y, m_radius);
			((VFP_D3DVSDT_PBYTE2*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_PBYTE2*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_PBYTE2*)vertices)[i].nz	= normal[i].z;
			D3DXMatrixScaling(&m_matrices[i], 2.0f * m_radius, 2.0f * m_radius, 1.0f);
			D3DXMatrixMultiply(&m_matrices[i], &m_matrices[i],
							   D3DXMatrixTranslation(&matrix, -m_radius, -m_radius,
												     position[i].z));
		}
		m_drawTris = FALSE;
	}

	// 3D packed byte expanded to (value, value, value, 1.)
	else if((positionType == D3DVSDT_PBYTE3) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_PBYTE3);
		decl			= VFP_D3DVSDT_PBYTE3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [1];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].x	= ToOffsetBYTE(position[i].x, m_radius);
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].y	= ToOffsetBYTE(position[i].y, m_radius);
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].z	= ToOffsetBYTE(position[i].z, m_radius);
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_PBYTE3*)vertices)[i].nz	= normal[i].z;
		}
		D3DXMatrixScaling(&m_matrices[0], 2.0f * m_radius, 2.0f * m_radius, 2.0f * m_radius);
		D3DXMatrixMultiply(&m_matrices[0], &m_matrices[0],
						   D3DXMatrixTranslation(&matrix, -m_radius, -m_radius, -m_radius));
		m_drawTris = TRUE;
	}

	// 4D packed byte expanded to (value, value, value, value) 
	else if((positionType == D3DVSDT_PBYTE4) && (normalType == D3DVSDT_FLOAT3))
	{
		m_vertexSize	= sizeof(VFP_D3DVSDT_PBYTE4);
		decl			= VFP_D3DVSDT_PBYTE4_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		m_matrices		= new D3DXMATRIX [1];
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].x	= ToOffsetBYTE(position[i].x, m_radius);
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].y	= ToOffsetBYTE(position[i].y, m_radius);
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].z	= ToOffsetBYTE(position[i].z, m_radius);
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].w	= UCHAR_MAX;
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].nx	= normal[i].x;
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].ny	= normal[i].y;
			((VFP_D3DVSDT_PBYTE4*)vertices)[i].nz	= normal[i].z;
		}
		D3DXMatrixScaling(&m_matrices[0], 2.0f * m_radius, 2.0f * m_radius, 2.0f * m_radius);
		D3DXMatrixMultiply(&m_matrices[0], &m_matrices[0],
						   D3DXMatrixTranslation(&matrix, -m_radius, -m_radius, -m_radius));
		m_drawTris = TRUE;
	}

	// TODO: This type is not complete
	// 2D homogeneous float expanded to (value, value,0., value.) Useful for projective texture coordinates.
//	else if((positionType == D3DVSDT_FLOAT2H) && (normalType == D3DVSDT_FLOAT3))
//	{
//		m_vertexSize	= sizeof(VF_case );
//		decl			= VFP_D3DVSDT_FLOAT2H_DECL;
//		bufferSize		= m_numVertices * m_vertexSize;
//		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
//		for(i=0; i<m_numVertices; ++i) 
//		{
//			((VFP_D3DVSDT_FLOAT2H*)vertices)[i]	= ;
//			((VFP_D3DVSDT_FLOAT2H*)vertices)[i].nx	= normal[i].x;
//			((VFP_D3DVSDT_FLOAT2H*)vertices)[i].ny	= normal[i].y;
//			((VFP_D3DVSDT_FLOAT2H*)vertices)[i].nz	= normal[i].z;
//		}
//	}
//	D3DVSDT_D3DCOLOR:	// 4D packed unsigned bytes mapped to 0. to 1. range.  Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	
	else if((positionType == D3DVSDT_FLOAT3) && (normalType == D3DVSDT_NORMSHORT3))
	{
		m_vertexSize	= sizeof(VFN_D3DVSDT_NORMSHORT3);
		decl			= VFN_D3DVSDT_NORMSHORT3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].x	= position[i].x;
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].y	= position[i].y;
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].z	= position[i].z;
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].nx	= ToShort(normal[i].x, 1.0f);
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].ny	= ToShort(normal[i].y, 1.0f);
			((VFN_D3DVSDT_NORMSHORT3*)vertices)[i].nz	= ToShort(normal[i].z, 1.0f);
		}
		m_drawTris = TRUE;
	}
	else if((positionType == D3DVSDT_FLOAT3) && (normalType == D3DVSDT_NORMPACKED3))
	{
		m_vertexSize	= sizeof(VFN_D3DVSDT_NORMPACKED3);
		decl			= VFN_D3DVSDT_NORMPACKED3_DECL;
		bufferSize		= m_numVertices * m_vertexSize;
		vertices		= HeapAlloc(GetProcessHeap(), 0, bufferSize);
		for(i=0; i<m_numVertices; ++i) 
		{
			((VFN_D3DVSDT_NORMPACKED3*)vertices)[i].x	= position[i].x;
			((VFN_D3DVSDT_NORMPACKED3*)vertices)[i].y	= position[i].y;
			((VFN_D3DVSDT_NORMPACKED3*)vertices)[i].z	= position[i].z;
			if(normal[i].x >= 0.0f)
				x = ((DWORD)(normal[i].x * 1023.0f)) & 0x7ff;
			else
				x = ((DWORD)(normal[i].x * 1024.0f)) & 0x7ff;
			if(normal[i].y >= 0.0f)
				y = ((DWORD)(normal[i].y * 1023.0f)) & 0x7ff;
			else
				y = ((DWORD)(normal[i].y * 1024.0f)) & 0x7ff;
			if(normal[i].z >= 0.0f)
				z = ((DWORD)(normal[i].z * 511.0f)) & 0x3ff;
			else
				z = ((DWORD)(normal[i].z * 512.0f)) & 0x3ff;

			((VFN_D3DVSDT_NORMPACKED3*)vertices)[i].nxyz = (x << 21) | (y << 10) | z;
		}
		m_drawTris = TRUE;
	}

	else
		__asm int 3;

	// Free memory
	delete [] normal;
	delete [] position;

	// Create the vertex shader
	if(ResultFailed(m_pDevice->CreateVertexShader(decl, NULL, &m_vertexShader, 0),
					TEXT("m_pDevice->CreateVertexShader"))) 
	{
		HeapFree(GetProcessHeap(), 0, vertices);
		return;
	}

	// Setup the vertex buffer for the Ball
	if(ResultFailed(m_pDevice->CreateVertexBuffer(bufferSize, 
												  0, /* ignored */ 
												  0, /* ignored */ 
												  0, /* ignored */
												  &m_pVB),
					TEXT("m_pDevice->CreateVerexBuffer"))) 
	{
		HeapFree(GetProcessHeap(), 0, vertices);
		return;
	}

	if(ResultFailed(m_pVB->Lock(0, bufferSize, (BYTE**)&pVoid, 0),
				    TEXT("m_pVB->Lock")))
	{
		Release();
		HeapFree(GetProcessHeap(), 0, vertices);
		return;
	}

	CopyMemory(pVoid, vertices, bufferSize);
	m_pVB->Unlock();
	HeapFree(GetProcessHeap(), 0, vertices);

	// Do the indices need to be built?
	if(m_drawTris) 
	{
		indices = new WORD [m_numTriangles * 3];
		for(index = 0, p=0; p<numLong; ++p) 
		{
			// Shortcuts
			pl = p * latitude1 + 1;
			if(p != longitude1)
				p1l = (p+1) * latitude1 + 1;
			else
				p1l = 1;

			// Top triangle
			indices[index++] = 0;
			indices[index++] = (WORD)pl;
			indices[index++] = (WORD)p1l;

			// Middle triangles
			for(r=0; r<latitude2; ++r) {
				indices[index++] = (WORD)(pl + r);
				indices[index++] = (WORD)(pl + r + 1);
				indices[index++] = (WORD)(p1l + r);

				indices[index++] = (WORD)(pl + r + 1);
				indices[index++] = (WORD)(p1l + r + 1);
				indices[index++] = (WORD)(p1l + r);
			}

			// Bottom triangle
			indices[index++] = (WORD)(last);
			indices[index++] = (WORD)(p1l + r);
			indices[index++] = (WORD)(pl + r);
		}

		// Setup the index buffer for the Ball
		if(FAILED(m_pDevice->CreateIndexBuffer(m_numTriangles * 3 * sizeof(WORD),
											   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
											   D3DPOOL_DEFAULT, &m_pIB))) 
		{
			Release();
			delete [] indices;
			return;
		}

		if(FAILED(m_pIB->Lock(0, m_numTriangles * 3 * sizeof(WORD), (BYTE**)&pVoid, 0))) 
		{
			Release();
			delete [] indices;
			return;
		}

		CopyMemory(pVoid, indices, m_numTriangles * 3 * sizeof(WORD));
		m_pIB->Unlock();
		delete [] indices;
	}
	m_dontDraw = FALSE;
}

//------------------------------------------------------------------------------
//	CBall::SetPosition
//------------------------------------------------------------------------------
void 
CBall::SetPosition(
				   IN const D3DXVECTOR3& position
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
	D3DXMatrixTranslation(&m_worldMatrix, m_location[0], m_location[1], 
						  m_location[2]);
	m_min	= D3DXVECTOR3(m_radius, m_radius, m_radius) + m_location;
	m_max	= D3DXVECTOR3(-m_radius, -m_radius, -m_radius) - m_location;
}

//------------------------------------------------------------------------------
//	CBall::SetDirection
//------------------------------------------------------------------------------
void 
CBall::SetDirection(
					IN const D3DXVECTOR3& direction
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
const D3DXVECTOR3& 
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
	D3DXVECTOR3	da = m_direction * amount;

	m_location += da;
	m_worldMatrix(3,0) += da.x;
	m_worldMatrix(3,1) += da.y;
	m_worldMatrix(3,2) += da.z;
	m_min = D3DXVECTOR3(m_location.x - m_radius, m_location.y - m_radius, m_location.z - m_radius);
	m_max = D3DXVECTOR3(m_location.x + m_radius, m_location.y + m_radius, m_location.z + m_radius);
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
CBall::Render(void)
/*++

Routine Description:

	Renders the Ball

Arguments:

	None

Return Value:

	None

--*/
{
	D3DXMATRIX	matrix;
	int			i;

	if(m_dontDraw)
		return;

	// Vertex shader type
	m_pDevice->SetVertexShader(m_vertexShader);

	// Set the material
	m_pDevice->SetMaterial(&m_material);

	// Set the vertex buffer stream source
	m_pDevice->SetStreamSource(0, m_pVB, m_vertexSize);

	// Draw the ball as triangles?
	if(m_drawTris)
	{
		// Is a separate transorm needed?
		if(m_matrices) 
		{
			D3DXMatrixMultiply(&matrix, m_matrices, &m_worldMatrix);
			m_pDevice->SetTransform(D3DTS_WORLD, &matrix);
		}

		// Or just the one?
		else
			m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

		// Draw the object
		m_pDevice->SetIndices(m_pIB, 0);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
										m_numTriangles);
	}

	// Or points?
	// This case is really reserved for the balls that have a separate
	// transformation matrix for every point
	else
	{
		for(i=0; i<m_numVertices; ++i)
		{
			D3DXMatrixMultiply(&matrix, &m_matrices[i], &m_worldMatrix);
			m_pDevice->SetTransform(D3DTS_WORLD, &matrix);

			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, i, 1);
		}
	}
}

//------------------------------------------------------------------------------
//	CBall::GetMin
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
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
const D3DXVECTOR3& 
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
const D3DXVECTOR3&
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
//	CBall::SetColor
//------------------------------------------------------------------------------
void
CBall::SetColor(
				IN float	red,
				IN float	green,
				IN float	blue
				)
/*++

Routine Description:

	Sets the color of the ball

Arguments:

	IN red -	Red component (0.0 - 1.0)
	IN green -	Green component (0.0 - 1.0)
	IN blue -	Blue component (0.0 - 1.0)

Return Value:

	None

--*/
{
	m_material.Diffuse.r	= red;
	m_material.Diffuse.g	= green;
	m_material.Diffuse.b	= blue;
	m_material.Diffuse.a	= 1.0f;
	m_material.Ambient.r	= m_material.Diffuse.r * 0.4f;
	m_material.Ambient.g	= m_material.Diffuse.g * 0.4f;
	m_material.Ambient.b	= m_material.Diffuse.b * 0.4f;
	m_material.Ambient.a	= 1.0f;
	m_material.Specular.r	= 1.0f;
	m_material.Specular.g	= 1.0f;
	m_material.Specular.b	= 1.0f;
	m_material.Specular.a	= 1.0f;
	m_material.Emissive.r	= 0.0f;
	m_material.Emissive.g	= 0.0f;
	m_material.Emissive.b	= 0.0f;
	m_material.Emissive.a	= 0.0f;
	m_material.Power		= 100.0f;
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
	if(m_matrices) {
		delete [] m_matrices;
		m_matrices = NULL;
	}
}

//------------------------------------------------------------------------------
//	CBall:ToShort
//------------------------------------------------------------------------------
short
CBall::ToShort(
			   IN float	a,
			   IN float	range
			   )
/*++

Routine Description:

	Converts a floating point value centered at 0.0 within a given range to 
	a normalized short within the range of SHRT_MIN - SHRT_MAX

Arguments:

	IN a -	Floating point value
	IN range -	Range (+/- this value)

Return Value:

	Short - SHRT_MIN -> SHRT_MAX

--*/
{
	if(a >= 0.0f)
		return (short)(a / range * (float)SHRT_MAX);
	return (short)(-a / range * (float)SHRT_MIN);
}

//------------------------------------------------------------------------------
//	CBall:ToOffsetBYTE
//------------------------------------------------------------------------------
BYTE
CBall::ToOffsetBYTE(
					IN float	a,
					IN float	range
					)
/*++

Routine Description:

	Converts a floating point value centered at 0.0 within a given range to 
	an offset normalized BYTE within the range of 0 - 255

Arguments:

	IN a -		Floating point value
	IN range -	Range (+/- this value)

Return Value:

	BYTE - 0 - 255

--*/
{
	return (BYTE)((a + range) / (range * 2.0f) * (float)UCHAR_MAX);
}


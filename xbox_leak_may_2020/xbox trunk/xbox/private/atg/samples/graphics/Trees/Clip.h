//-----------------------------------------------------------------------------
//  Blinn clipping.
//
//  Note that clipping range is assumed to be 0 to 1 in X, Y, and Z,
//  so the standard D3D clipping coords must be scaled and offset in
//  X and Y by 0.5, as done by BlinnClipMatrix().
//
//  See Jim Blinn's "A Trip Down the Graphics Pipeline", pp. 119-134.
//
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Convert D3D world.view.projection matrix to Blinn-style clip matrix.
//    D3D's clip volume is [-1,1] in X and Y, and [0,1] in Z.
//    Blinn's clip volume is [0,1] in X, Y, and Z.
//
inline D3DXMATRIX *BlinnClipMatrix(D3DXMATRIX *pmatBlinnClip, const D3DXMATRIX *pmatD3DClip)
{
	D3DXMATRIX matClipScale;
	D3DXMatrixScaling(&matClipScale, 0.5f, 0.5f, 1.f);
	D3DXMATRIX matClipOffset;
	D3DXMatrixTranslation(&matClipOffset, 0.5f, 0.5f, 0.f);
	*pmatBlinnClip = *pmatD3DClip * matClipScale * matClipOffset;
	return pmatBlinnClip;
}

//////////////////////////////////////////////////////////////////////
// Move float sign bit into bit 0 of the returned DWORD
//
inline DWORD FloatSignBit(float f)
{
	return *(DWORD *)&f >> 31;
}

//////////////////////////////////////////////////////////////////////
// Blinn boundary codes packed into a DWORD
//
inline DWORD BlinnBoundaryCode(const float *p)
{
	return FloatSignBit(p[0])				// X == 0
		| (FloatSignBit(p[3] - p[0]) << 1)	// X == 1
		| (FloatSignBit(p[1])        << 2)	// Y == 0
		| (FloatSignBit(p[3] - p[1]) << 3)	// Y == 1
		| (FloatSignBit(p[2])        << 4)	// Z == 0
		| (FloatSignBit(p[3] - p[2]) << 5); // Z == 1
}

//////////////////////////////////////////////////////////////////////
// Checks to see if points in [0,1] clip coordinates touch the clip frustum.
//
inline bool BoundingPointsInFrustum(UINT nPoint,				// number of points to test
									const D3DXVECTOR4 *rvPoint)	// array of bounding points in [0,1] clip coords
{
	DWORD Ocumulate = 0;
	DWORD Acumulate = -1;
	for (UINT i = 0; i < nPoint; i++)	// 8 points on bounding volume
	{
		DWORD BC = BlinnBoundaryCode(rvPoint[i]);
		Ocumulate |= BC;
		Acumulate &= BC;
	}
	if (Acumulate != 0)
		return false;	// all points are outside of frustum
	if (Ocumulate == 0)
		return true;	// all points are inside of frustum
	// TODO: Now a more elaborate test is needed, since the bounding box is
	// straddling a clip plane. Currently, this returns false positives.
	return true;
}

//////////////////////////////////////////////////////////////////////
// Culling helper. Checks to see if bounding box touches the clip frustum.
//    The matClip transformaion takes world coords to [0,1] clip range in X, Y, and Z.
//
inline bool BoundingBoxInFrustum(const D3DXMATRIX &matClip,  // current world/view/proj/clip matrix
								 const D3DXVECTOR3 &vMin, // bounding box
								 const D3DXVECTOR3 &vMax)
{
	float rv[8][3] = {
		vMin.x, vMin.y, vMin.z,
		vMin.x, vMin.y, vMax.z,
		vMin.x, vMax.y, vMin.z,
		vMin.x, vMax.y, vMax.z,
		vMax.x, vMin.y, vMin.z,
		vMax.x, vMin.y, vMax.z,
		vMax.x, vMax.y, vMin.z,
		vMax.x, vMax.y, vMax.z
	};
	D3DXVECTOR4 rvClip[8];
	for (UINT i = 0; i < 8; i++)
		D3DXVec3Transform(&rvClip[i], (D3DXVECTOR3 *)&rv[i], &matClip);
	return BoundingPointsInFrustum(8, rvClip);
}

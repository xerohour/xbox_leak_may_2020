#include <d3dlocus.h>
#include "..\..\..\inc\3DMath.h"

// ============================================================================
//
//  Functions
//
// ============================================================================

// ----------------------------------------------------------------------------
//  Name:       ZeroMatrix
//  Purpose:	sets D3D matrix to all 0's
// ----------------------------------------------------------------------------

D3DMATRIX ZeroMatrix(void)
{
    D3DMATRIX ret;
    ret._11 = 0.0f;
    ret._12 = 0.0f;
    ret._13 = 0.0f;
    ret._14 = 0.0f;
    ret._21 = 0.0f;
    ret._22 = 0.0f;
    ret._23 = 0.0f;
    ret._24 = 0.0f;
    ret._31 = 0.0f;
    ret._32 = 0.0f;
    ret._33 = 0.0f;
    ret._34 = 0.0f;
    ret._41 = 0.0f;
    ret._42 = 0.0f;
    ret._43 = 0.0f;
    ret._44 = 0.0f;
    return ret;
} // end ZeroMatrix

// ----------------------------------------------------------------------------
//  Name:       IdentityMatrix
//  Purpose:	sets D3D matrix to Identiy (1's on diagonal, zero's elsewhere)
// ----------------------------------------------------------------------------

D3DMATRIX IdentityMatrix(void)
{
    D3DMATRIX ret;
    ret._11 = 1.0f;
    ret._12 = 0.0f;
    ret._13 = 0.0f;
    ret._14 = 0.0f;
    ret._21 = 0.0f;
    ret._22 = 1.0f;
    ret._23 = 0.0f;
    ret._24 = 0.0f;
    ret._31 = 0.0f;
    ret._32 = 0.0f;
    ret._33 = 1.0f;
    ret._34 = 0.0f;
    ret._41 = 0.0f;
    ret._42 = 0.0f;
    ret._43 = 0.0f;
    ret._44 = 1.0f;
    return ret;
} // end IdentityMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets Projection matrix from fovy, aspect ratio, near and far planes
//  Notes:		
//		1. fovy		- field of view in Y direction in radians (for example pi/2)
//		2. aspect	- window_width / window_height
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar) 
{
	float f = 1.f / (float)tan(fovy/2.f);

	D3DMATRIX ret = ZeroMatrix();
	ret._11 = f / aspect;
	ret._22 = f;
	ret._33 = (zFar)/(zFar-zNear);
	ret._34 = 1.f;
	ret._43 = zNear*zFar / (zNear - zFar);

	return ret;
} // end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets "shifted" projection matrix from left, right, bottom, top, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) 
{

	D3DMATRIX ret = ZeroMatrix();
	ret._11 = 2.f * zNear / (right - left);
	ret._22 = 2.f * zNear / (top - bottom);
	ret._31 = (right + left) / (left - right);
	ret._32 = (top + bottom) / (bottom - top);
	ret._33 = (zFar)/(zFar-zNear);
	ret._34 = 1.f;
	ret._43 = zNear*zFar / (zNear - zFar);

	return ret;
} // end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets Projection matrix from fov, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float near_plane, 
				 const float far_plane, 
				 const float fov)
{
	float	c, s, Q;

	c = (float) cos(fov*0.5);
	s = (float) sin(fov*0.5);
	Q = s/(1.0f - near_plane/far_plane);

    D3DMATRIX ret = ZeroMatrix();
    ret._11 = c;
    ret._22 = c;
	ret._33 = Q;
	ret._43 = -Q*near_plane;
    ret._34 = s;
    return ret;
}	// end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       OrthoMatrix
//  Purpose:	sets orthogonal projection matrix from width, height, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX OrthoMatrix(const float width, const float height, const float zNear, const float zFar)
{
	D3DMATRIX ret = IdentityMatrix();
	ret._11 = 2.f / width;
	ret._22 = 2.f / height;
	ret._33 = 1.f / (zFar - zNear);
	ret._43 = zNear / (zNear - zFar);
	return ret;
}

// ----------------------------------------------------------------------------
//  Name:       OrthoMatrix
//  Purpose:	sets "shifted" orthogonal projection matrix from left, right, bottom, top, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX OrthoMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	D3DMATRIX ret = IdentityMatrix();
	ret._11 = 2.f / (right - left);
	ret._22 = 2.f / (top - bottom);
	ret._33 = 1.f / (zFar - zNear);
	ret._41 = (right + left) / (left - right);
	ret._42 = (top + bottom) / (bottom - top);
	ret._43 = zNear / (zNear - zFar);
	return ret;
}

// ----------------------------------------------------------------------------
//  Name:       ViewMatrix
//  Purpose:	Controls where the camara is.
//  Notes:		
//		1. Note the roll parameter is in radians and rools the viewpoint
//			around the viewing direction
// ----------------------------------------------------------------------------

D3DMATRIX ViewMatrix(const D3DVECTOR& from, 
		   const D3DVECTOR& at, 
		   const D3DVECTOR& world_up, 
		   const float roll)
{
    D3DMATRIX view = IdentityMatrix();
    D3DVECTOR up, right, view_dir;

    view_dir.x = at.x - from.x;
    view_dir.y = at.y - from.y;
    view_dir.z = at.z - from.z;
    view_dir = Normalize(view_dir);
	right = CrossProduct(world_up, view_dir);
	up = CrossProduct(view_dir, right);

	right = Normalize(right);
	up = Normalize(up);
	
    view._11 = right.x;
    view._21 = right.y;
    view._31 = right.z;
    view._12 = up.x;
    view._22 = up.y;
    view._32 = up.z;
    view._13 = view_dir.x;
    view._23 = view_dir.y;
    view._33 = view_dir.z;
	
    view._41 = -DotProduct(right, from);
    view._42 = -DotProduct(up, from);
    view._43 = -DotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f) {
		view = MatrixMult(RotateZMatrix(-roll), view);
	}

    return view;
} // end ViewMatrix

// ----------------------------------------------------------------------------
//  Name:       RotateXMatrix
//  Purpose:	Rotate matrix about X axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateXMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret._22 = cosine;
	ret._33 = cosine;
	ret._23 = -sine;
	ret._32 = sine;
    return ret;
} // end RotateXMatrix

// ----------------------------------------------------------------------------
//  Name:       RotateYMatrix
//  Purpose:	Rotate matrix about Y axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateYMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret._11 = cosine;
	ret._33 = cosine;
	ret._13 = sine;
	ret._31 = -sine;
    return ret;
} // end RotateY
  
// ----------------------------------------------------------------------------
//  Name:       RotateZMatrix
//  Purpose:	Rotate matrix about Z axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateZMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret._11 = cosine;
	ret._22 = cosine;
	ret._12 = -sine;
	ret._21 = sine;
    return ret;
} // end RotateZMatrix

// ----------------------------------------------------------------------------
//  Name:       TranslateMatrix
//  Purpose:    Returns matrix to translate by (dx, dy, dz)
// ----------------------------------------------------------------------------

D3DMATRIX TranslateMatrix(const float dx, const float dy, const float dz)
{
    D3DMATRIX ret = IdentityMatrix();
	ret._41 = dx;
	ret._42 = dy;
	ret._43 = dz;
	return ret;
} // end TranslateMatrix

// ----------------------------------------------------------------------------
//  Name:       TranslateMatrix
//  Purpose:    Returns matrix to translate by v
// ----------------------------------------------------------------------------

D3DMATRIX TranslateMatrix(const D3DVECTOR& v)
{
    D3DMATRIX ret = IdentityMatrix();
	ret._41 = v.x;
	ret._42 = v.y;
	ret._43 = v.z;
	return ret;
} // end TranslateMatrix

// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:    scale matrix (uniform)
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const float size)
{
    D3DMATRIX ret = IdentityMatrix();
	ret._11 = size;
	ret._22 = size;
	ret._33 = size;
	return ret;
} // end ScaleMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:	scale matrix
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const float a, const float b, const float c)
{
    D3DMATRIX ret = IdentityMatrix();
	ret._11 = a;
	ret._22 = b;
	ret._33 = c;
	return ret;
} // end ScaleMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:	scale matrix
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const D3DVECTOR& v)
{
    D3DMATRIX ret = IdentityMatrix();
	ret._11 = v.x;
	ret._22 = v.y;
	ret._33 = v.z;
	return ret;
} // end ScaleMatrix

// ----------------------------------------------------------------------------
//  Name:       MatrixMult
//  Purpose:	[C] = [A] * [B]
// ----------------------------------------------------------------------------

D3DMATRIX MatrixMult(const D3DMATRIX & a, const D3DMATRIX & b)
{
	D3DMATRIX ret = ZeroMatrix();

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			for (int k=0; k<4; k++) {
				*((float*)&ret + i * 4 + j) += *((float*)&a + k * 4 + j) * *((float*)&b + i * 4 + k);
			}
		}
	}
	return ret;
} // end MatrixMult


// ----------------------------------------------------------------------------
//  Name:       MatrixMult
//  Purpose:	[C] = [A] * scalar
// ----------------------------------------------------------------------------

D3DMATRIX MatrixMult(const D3DMATRIX & a, const float scalar)
{
	D3DMATRIX ret = ZeroMatrix();

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
				*((float*)&ret + i * 4 + j) += *((float*)&a + i * 4 + j) * scalar;
		}
	}
	return ret;
} // end MatrixMult


// ----------------------------------------------------------------------------
//  Name:       TransformVector
//  Purpose:	V' = V * [M]
// ----------------------------------------------------------------------------

D3DVECTOR TransformVector(const D3DVECTOR& v, const D3DMATRIX & m)
{
	float	hvec[4];

	for (int i=0; i<4; i++) {
		hvec[i] = 0.0f;
		for (int j=0; j<4; j++) {
			if (j==3) {
				hvec[i] += *((float*)&m + j * 4 + i);
			} else {
				hvec[i] += *((float*)&v + j) * *((float*)&m + j * 4 + i);
			}
		}
	}
	D3DVECTOR ret;
    
    ret.x = hvec[0]/hvec[3];
    ret.y = hvec[1]/hvec[3];
    ret.z = hvec[2]/hvec[3];

	return ret;
} // end TransformVector

// ----------------------------------------------------------------------------
//  Name:       TransformNormal
//  Purpose:	N' = N * [M]
// ----------------------------------------------------------------------------

D3DVECTOR TransformNormal(const D3DVECTOR& v, const D3DMATRIX & mat)
{
	D3DMATRIX	m;
	m = MatrixInverse(mat);
	m = MatrixTranspose(m);
	return Normalize(TransformVector(v, m));
	return TransformVector(v, m);
}  // end TransformNormal

// ----------------------------------------------------------------------------
//  Name:       MatrixInverse
//  Purpose:	Creates the inverse of a 4x4 matrix
// ----------------------------------------------------------------------------

static void	lubksb(D3DMATRIX & a, int *indx, float *b);
static void ludcmp(D3DMATRIX & a, int *indx, float *d);

D3DMATRIX MatrixInverse(const D3DMATRIX & m)
{
	D3DMATRIX	n, y;
	int			i, j, indx[4];
	float		d, col[4];

	n = m;
	ludcmp(n, indx, &d);

	for (j=0; j<4; j++) {
		for (i=0; i<4; i++) {
			col[i] = 0.0f;
		}
		col[j] = 1.0f;
		lubksb(n, indx, col);
		for (i=0; i<4; i++) {
			*((float*)&y + i * 4 + j) = col[i];
		}
	}
	return y;
} // end MatrixInverse

// ----------------------------------------------------------------------------
//  Name:       lubksb
//  Purpose:	backward subsitution
// ----------------------------------------------------------------------------

static void lubksb(D3DMATRIX & a, int *indx, float *b)
{
	int		i, j, ii=-1, ip;
	float	sum;

	for (i=0; i<4; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii>=0) {
			for (j=ii; j<=i-1; j++) {
				sum -= *((float*)&a + i * 4 + j) * b[j];
			}
		} else if (sum != 0.0) {
			ii = i;
		}
		b[i] = sum;
	}
	for (i=3; i>=0; i--) {
		sum = b[i];
		for (j=i+1; j<4; j++) {
			sum -= *((float*)&a + i * 4 + j) * b[j];
		}
		b[i] = sum/ *((float*)&a + i * 4 + i);
	}
} // end lubksb

// ----------------------------------------------------------------------------
//  Name:       ludcmp
//  Purpose:	LU decomposition
// ----------------------------------------------------------------------------

static void ludcmp(D3DMATRIX & a, int *indx, float *d)
{
	float	vv[4]; // implicit scale for each row
	float	big, dum, sum, tmp;
	int		i, imax, j, k;

	*d = 1.0f;
	for (i=0; i<4; i++) {
		big = 0.0f;
		for (j=0; j<4; j++) {
			if ((tmp = (float) fabs(*((float*)&a + i * 4 + j))) > big) {
				big = tmp;
			}
		}
		vv[i] = 1.0f/big;
	}
	for (j=0; j<4; j++) {
		for (i=0; i<j; i++) {
			sum = *((float*)&a + i * 4 + j);
			for (k=0; k<i; k++) {
				sum -= *((float*)&a + i * 4 + k) * *((float*)&a + k * 4 + j);
			}
			*((float*)&a + i * 4 + j) = sum;
		}
		big = 0.0f;
		for (i=j; i<4; i++) {
			sum = *((float*)&a + i * 4 + j);
			for (k=0; k<j; k++) {
				sum -= *((float*)&a + i * 4 + k)* *((float*)&a + k * 4 + j);
			}
			*((float*)&a + i * 4 + j) = sum;
			if ((dum = vv[i] * (float)fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k=0; k<4; k++) {
				dum = *((float*)&a + imax * 4 + k);
				*((float*)&a + imax * 4 + k) = *((float*)&a + j * 4 + k);
				*((float*)&a + j * 4 + k) = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (*((float*)&a + j * 4 + j) == 0.0f) {
			*((float*)&a + j * 4 + j) = 1.0e-20f; // can be 0.0 also...
		}
		if (j != 3) {
			dum = 1.0f/ *((float*)&a + j * 4 + j);
			for (i=j+1; i<4; i++) {
				*((float*)&a + i * 4 + j) *= dum;
			}
		}
	}
} // end ludcmp
  
// ----------------------------------------------------------------------------
//  Name:       Matrix Transpose
//  Purpose:	[M] = [M]'
// ----------------------------------------------------------------------------

D3DMATRIX MatrixTranspose(const D3DMATRIX & m)
{
	D3DMATRIX	ret;
	int			i, j;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			*((float*)&ret + i * 4 + j) = *((float*)&m + j * 4 + i);
		}
	}

	return ret;
} // end MatrixTranspose

// ----------------------------------------------------------------------------
//  Name:       TextureMapSpherical
//  Purpose:    Spherically map a texture to an object
//              Requires untransformed data, assumes object is centered
//              at the origin, and that all vertices are surface vertices.
// ----------------------------------------------------------------------------

void TextureMapSpherical(D3DVERTEX * pVertices, int nCount)
{
	const float	pi = 3.1415926535f;
	float u, v;
	float maxx, minx, x_radius;
	float maxy, miny, y_radius;
	int i;

	maxx = minx = maxy = miny = 0;

	// get the x and y radius of the shape
	for(i = 0; i < nCount; i++)
	{
		if((pVertices[i].x) < minx)
			minx = pVertices[i].x;

		if((pVertices[i].x) > maxx)
			maxx = pVertices[i].x;

		if((pVertices[i].y) < miny)
			miny = pVertices[i].y;

		if((pVertices[i].y) > maxy)
			maxy = pVertices[i].y;
	}
	
	// we need the x_radius to be *slightly* off, so we don't get any results where
	// acos(x / x_radius) == 0 or pi.  This would mess up our later calculation of u+=pi.

	x_radius = (maxx - minx) / 1.99999f; // we need this to be slightly off
	y_radius = (maxy - miny) / 2.0f;

	for(i = 0; i < nCount; i++)
	{
		// calculate texture U coordinate based on model x-z coordinates.
		u = (float)acos(pVertices[i].x / x_radius);

		if(pVertices[i].z < 0.0f)
			u += pi;

		u /= (2.0f * pi); // convert to a value between 0 and 1.

		// calculate texture V coordinate based on model y coordinates.
		v = (float)acos(pVertices[i].y / y_radius);

		v /= pi;

		// now assign the calculated values.
		pVertices[i].tu = u;
		pVertices[i].tv = v;
	}
}

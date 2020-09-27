#ifndef __MOREMATH_H
#define __MOREMATH_H

#ifndef __FLOATTYPE_H
#include "floattype.h"
#endif

#  include <math.h>

//#define MATH_USE_ASM

#  ifndef X
#    define X 0
#  elif X!=0
#    error "X should be 0!"
#  endif

#  ifndef Y
#    define Y 1
#  elif X!=0
#    error "Y should be 1!"
#  endif

#  ifndef Z
#    define Z 2
#  elif Z!=2
#    error "Z should be 2!"
#  endif

#  ifndef W
#    define W 3
#  elif W!=3
#    error "W should be 3!"
#  endif


#  ifndef FORWARD
#    define FORWARD X
#  elif FORWARD!=X
#    error "FORWARD should be X!"
#  endif

#  ifndef SIDE
#    define SIDE Y
#  elif SIDE!=Y
#    error "SIDE should be Y!"
#  endif

#  ifndef UP
#    define UP Z
#  elif UP!=Z
#    error "UP should be Z!"
#  endif

#  ifndef POSITION
#    define POSITION 3
#  elif POSITION!=3
#    error "POSITION should be 3!"
#  endif


#define GAME_PI     ((floatType)(3.14159265358979323846))
#define GAME_2PI    ((floatType)(3.14159265358979323846*2.0))
#define GAME_PI2    ((floatType)(3.14159265358979323846*0.5))
#define GAME_HALFPI GAME_PI2
	
#define SIN(x)   sinf(x)
#define COS(x)   cosf(x)
#define TAN(x)   tanf(x)
#define ATAN(x)  atanf(x)
#define ACOS(x)  acosf(x)
#define ASIN(x)  asinf(x)
#define SQRT(x)  sqrtf(x)
// note that mypowf is not accurate
#define POW(x,y) powf(x,y)
#define FLOOR(x) floorf(x)
#define FMOD(x,y) fmodf(x,y)
#define CEIL(x) ceilf(x)

#define RAD2DEG     57.295779f
#define RPM2RADPSEC (GAME_2PI / 60.0f)
#define SIGN(x) ((x)>=0.0f? 1.0f: -1.0f)
//#define SGN(x)  ((x)>0.0f?  1:    -1)
#define SIGN_AS(a, y)  ABS(a)*SIGN(y)
#define ABS(x) ((x)>0.0f?(x):-(x))
#define MAX(a,b) ((a)>(b)? (a) : (b))



#define trunci(f) ((int)(truncf(f)))

__forceinline void FloatToInt(int *int_pointer, float f) 
{
	__asm
	{
		fld		f
		mov		edx,int_pointer
		FRNDINT
		fistp	dword ptr [edx];
	}
}



inline void V4Zero(floatType dest[3])
{
	// I cast to long because otherwise the silly compiler
	// copies the zero register to a floating point register
	// before it stores to dest.
#if FLOATTYPE_BITS==64
	((unsigned long long *)dest)[X]=0;
	((unsigned long long *)dest)[Y]=0;
	((unsigned long long *)dest)[Z]=0;
	((unsigned long long *)dest)[W]=0;
#else
	((unsigned long *)dest)[X]=0;
	((unsigned long *)dest)[Y]=0;
	((unsigned long *)dest)[Z]=0;
	((unsigned long *)dest)[W]=0;
#endif
}
inline void V3Zero(floatType dest[3])
{
	// I cast to long because otherwise the silly compiler
	// copies the zero register to a floating point register
	// before it stores to dest.
#if FLOATTYPE_BITS==64
	((unsigned long long *)dest)[X]=0;
	((unsigned long long *)dest)[Y]=0;
	((unsigned long long *)dest)[Z]=0;
#else
	((unsigned long *)dest)[X]=0;
	((unsigned long *)dest)[Y]=0;
	((unsigned long *)dest)[Z]=0;
#endif
}
inline void V2Zero(floatType dest[2])
{
	// I cast to long because otherwise the silly compiler
	// copies the zero register to a floating point register
	// before it stores to dest.
#if FLOATTYPE_BITS==64
	((unsigned long long *)dest)[X]=0;
	((unsigned long long *)dest)[Y]=0;
#else
	((unsigned long *)dest)[X]=0;
	((unsigned long *)dest)[Y]=0;
#endif
}

#ifdef INLINE_VECTOR_SET
inline void V4Set(register floatType dest[4],register floatType x,register floatType y,register floatType z,register floatType w)
{
	dest[X]=x;
	dest[Y]=y;
	dest[Z]=z;
	dest[W]=w;
}

inline void V3Set(register floatType dest[3],register floatType x,register floatType y,register floatType z)
{
	dest[X]=x;
	dest[Y]=y;
	dest[Z]=z;
}

inline void V2Set(register floatType dest[2],register floatType x,register floatType y)
{
	dest[X]=x;
	dest[Y]=y;
}

inline void V4Set(register floatType dest[4],register floatType src[4])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
	dest[Z]=src[Z];
	dest[W]=src[W];
}

inline void V3Set(register floatType dest[3],register floatType src[3])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
	dest[Z]=src[Z];
}

inline void V2Set(register floatType dest[2],register floatType src[2])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
}
#else
void V4Set(register floatType dest[4],register floatType x,register floatType y,register floatType z,register floatType w);
void V3Set(register floatType dest[3],register floatType x,register floatType y,register floatType z);
void V2Set(register floatType dest[2],register floatType x,register floatType y);
void V4Set(register floatType dest[4],register floatType src[4]);
void V3Set(register floatType dest[3],register floatType src[3]);
void V2Set(register floatType dest[2],register floatType src[2]);
void V3Set(floatType dest[3],short src[3]);
#endif



void V4Plane(floatType *dest, floatType *src1, floatType *src2, floatType *src3);

void V3Cross(floatType dest[3],floatType src1[3],floatType src2[3]);
void V2Rot90(floatType dest[2],floatType src[2]);
void V2RotNeg90(floatType dest[2],floatType src[2]);
floatType V2Cross(floatType src1[2],floatType src2[2]);
void V2Mul(floatType dest[2],floatType src1[2],floatType src2[2]);
void V2Mul(floatType dest[2],floatType src[2],floatType mult);
void V2Mul(floatType dest[2],floatType src[2]);
void V2Mul(floatType dest[2],floatType mult);
void V3Mul(floatType dest[3],floatType src1[3],floatType src2[3]);
void V3Mul(floatType dest[3],floatType src[3],floatType mult);
void V3Mul(floatType dest[3],floatType src[3]);
void V3Mul(floatType dest[3],floatType mult);
void V4Mul(floatType dest[3],floatType src1[3],floatType src2[3]);
void V4Mul(floatType dest[3],floatType src[3],floatType mult);
void V4Mul(floatType dest[3],floatType src[3]);
void V4Mul(floatType dest[3],floatType mult);
void V2Div(floatType dest[2],floatType src[2],floatType div);
void V2Div(floatType dest[2],floatType div);
void V3Div(floatType dest[3],floatType src[3],floatType div);
void V3Div(floatType dest[3],floatType div);
// normalize v or src
void V3Norm(floatType v[3]);
void V2Norm(floatType v[2]);
void V3Norm(floatType dest[3],floatType src[3]);
void V2Norm(floatType dest[2],floatType src[2]);
// normal pointing from first to second arg
void V3Norm(floatType dest[3],floatType from[3],floatType to[3]);
void V2Norm(floatType dest[2],floatType from[2],floatType to[2]);
// normal pointing out of plane of p0,p1,p2
void V3Norm(floatType dest[3],floatType p0[3],floatType p1[3],floatType p2[3]);
void V3Sub(floatType dest[3],floatType src1[3],floatType src2[3]);
void V3Sub(floatType dest[3],floatType src[3]);
void V2Sub(floatType dest[2],floatType src1[2],floatType src2[2]);
void V2Sub(floatType dest[3],floatType src[3]);
void V2Add(floatType dest[2],floatType src1[2],floatType src2[2]);
void V2Add(floatType dest[2],floatType src[2]);
void V3Add(floatType dest[3],floatType src1[3],floatType src2[3]);
void V3Add(floatType dest[3],floatType src[3]);
// l is the amount of src1 you want, 1-l is how much src2
void V4Lerp(floatType dest[4],floatType src1[4],floatType src2[4],floatType l);
void V3Lerp(floatType dest[3],floatType src1[3],floatType src2[3],floatType l);
void V2Lerp(floatType dest[2],floatType src1[2],floatType src2[2],floatType l);
#define ScalarLerp(d,s1,s2,p) d=((s1)-(s2))*(p)+(s2)
#define ScalarSet(d,s) d=(s)
void V3Mac(floatType dest[3],floatType src1[3],floatType src2[3],floatType s);
void V3Mac(floatType dest[3],floatType src[3],floatType s);
void V2Mac(floatType dest[2],floatType src1[2],floatType src2[2],floatType s);
void V2Mac(floatType dest[2],floatType src[2],floatType s);
void V3Avg(floatType dest[3],floatType src1[3],floatType src2[3]);
void V2Avg(floatType dest[2],floatType src1[2],floatType src2[2]);
floatType V2Dist(floatType dest[2],floatType src[2]);
floatType V2DistSq(floatType dest[2],floatType src[2]);
floatType V3Dist(floatType dest[3],floatType src[3]);
floatType V3DistSq(floatType dest[3],floatType src[3]);
floatType V3Len(floatType v[3]);
floatType V3LenSq(floatType v[3]);
floatType V2Len(floatType v[2]);
floatType V2LenSq(floatType v[2]);
floatType V3Dot(floatType v1[3],floatType v2[3]);
floatType V2Dot(floatType v1[2],floatType v2[2]);
void V3Rot(floatType dest[3],floatType angle,floatType axis[3]);
void V2Rot(floatType dest[3],floatType angle);
void V3Neg(floatType dest[3],floatType src[3]);
void V3Neg(floatType dest[3]);
void V2Neg(floatType dest[2],floatType src[2]);
void V2Neg(floatType dest[2]);

// some matrix stuff
void V3Mul4x4(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul4x3(register floatType dest[3],register floatType src1[3],register floatType src2[4][4]);
void V3Mul4x4_XY(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul4x3_XY(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul4x4_XYZW(floatType dest[4],floatType src1[3],floatType src2[4][4]);
void VS3Mul4x4_XYZW(floatType dest[4],short src1[3],floatType src2[4][4]);
void V3Mul3x4(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul3x3(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul3x3T(floatType dest[3],floatType src1[3],floatType src2[4][4]);
void V3Mul3x3_XY(floatType dest[3],floatType src1[3],floatType src2[4][4]);
	
void M4Mul(floatType dest[4][4],floatType src1[4][4],floatType src2[4][4]);
void M4Mul(floatType dest[4][4],floatType src1[4][4]);
void M4Mul3x3(floatType dest[4][4],floatType src1[4][4],floatType src2[4][4]);
void M4Mul4x3(floatType dest[4][4],floatType src1[4][4],floatType src2[4][4]);
void M4Mul4x3(floatType dest[4][4],floatType src[4][4]);
void M4PreMul4x3(floatType dest[4][4],floatType src[4][4]);
void M4Transpose(floatType dest[4][4],floatType src[4][4]);
void M4Transpose3x3(floatType dest[4][4],floatType src[4][4]);
void M4Set(floatType dest[4][4],floatType src[4][4]);
void M4Ident(floatType mf[4][4]);
void M4Scale3x3(floatType dest[4][4],floatType scale);

typedef struct _D3DMATRIX D3DMATRIX;
void M4Set(D3DMATRIX *md, floatType mf[4][4]);
void M4Set(floatType mf[4][4],D3DMATRIX *md);

void M4ViewFromOriginCamera(register floatType vm[4][4],register floatType cm[4][4]);

void M4FromXYZRot(register floatType dest[4][4],register floatType rx,register floatType ry,register floatType rz);
void M4FromYZXRot(register floatType dest[4][4],register floatType rx,register floatType ry,register floatType rz);
void M4Joint(floatType m[4][4],floatType scale,floatType translateIn[3],floatType rotate[3],floatType translateOut[3]);
void M4WeightedJoint(floatType m[4][4],floatType scale,floatType translateIn[3],floatType rotate[3],floatType translateOut[3],floatType weight);

int  M4Inv(floatType out[4][4],floatType in[4][4]);
void M4OrthoInv(floatType out[4][4],floatType in[4][4]);

void M4Align(floatType mf[4][4],floatType up[3],floatType target[3],floatType pos[3]);
void M4Align(floatType mf[4][4],floatType up[3],floatType forward[3]);
	
float mypowf(float x,float y);
floatType table_sinf(floatType a);
floatType table_cosf(floatType a);

floatType CoarseAngle(floatType x,floatType y);

bool V3InTri(floatType *p, floatType *v0, floatType *v1, floatType *v2, floatType *n);
bool V3InTri(floatType *p, floatType *v0, floatType *v1, floatType *v2, floatType *n, floatType tolerance);
bool V2InTri(floatType *p, floatType *v0, floatType *v1, floatType *v2);
bool V2InTri(floatType *p, floatType *v0, floatType *v1, floatType *v2, floatType *n, floatType tolerance);
bool V2OnTri(floatType *p, floatType *v0, floatType *v1, floatType *v2);
bool V2OnTri(floatType *p, floatType *v0, floatType *v1, floatType *v2, floatType *n, floatType tolerance);
int  SegIntersect(floatType *l1p1,floatType *l1p2,floatType *l2p1,floatType *l2p2);
int  SegIntersect(floatType *l1p1,floatType *l1p2,floatType *l2p1,floatType *l2p2,floatType *percent);
int  LineSegIntersect(floatType *l1p1,floatType *l1p2,floatType *l2p1,floatType *l2p2);

void M4RotFromUF(floatType mat[4][4]);
int  positionInsideTriangle(floatType *p, floatType *v0, floatType *v1, floatType *v2, floatType *n, floatType tolerance);

void M4LookAt(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3]);
void M4LookAtD3D(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3]);
void M4LookAtD3DFlipX(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3]);
// NOTE: the angle is in RADIANS and is HALF the vertical angle
void M4Persp(floatType dest[4][4], floatType angle, floatType aspect, floatType n, floatType f);
void M4PerspFlipX(floatType dest[4][4], floatType angle, floatType aspect, floatType n, floatType f);
void M4Frust(floatType dest[4][4], floatType l, floatType r, floatType b, floatType t, floatType n, floatType f);
void M4Ortho(floatType dest[4][4], floatType l, floatType r, floatType b, floatType t, floatType n, floatType f);
#define M4OrthoFlipX(dest, l,r, b,t, n,f) M4Ortho(dest, r,l, b,t, n,f)

// rotate dest's upper 3x3 by angle around axis
void M4Rot3x3(floatType dest[4][4], floatType angle, floatType axis[3]);


bool V4Equal(floatType src1[4], floatType src2[4]);
bool V3Equal(floatType src1[3], floatType src2[3]);
bool V2Equal(floatType src1[2], floatType src2[2]);

bool V2OnSeg(floatType *teFrac, floatType p[2], floatType tev0[2], floatType tev1[2]);
// snaps to segment and also snaps to 0.0,1.0 if near the endpoint
bool V2SnapToSeg(floatType *teFrac, floatType p[2], floatType tev0[2], floatType tev1[2]);

void V2Gouraud4(floatType dest[4], floatType point[2],
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[4],  floatType triSrc1[4],  floatType triSrc2[4]);
void V2Gouraud3(floatType dest[3], floatType point[2],
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[3],  floatType triSrc1[3],  floatType triSrc2[3]);
void V2Gouraud2(floatType dest[2], floatType point[2],
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[2],  floatType triSrc1[2],  floatType triSrc2[2]);
void V2GouraudN(floatType dest[], floatType point[2], int n,
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[],   floatType triSrc1[],   floatType triSrc2[]);

void V3Ceil(floatType vec[3]);
void V2Ceil(floatType vec[2]);
void V3Floor(floatType vec[3]);
void V2Floor(floatType vec[2]);

void GameRandomSeed(unsigned long seed);
int  GameRandom(void);
unsigned long GetGameRandomSeed(void);

#ifdef GAME
void InitSinTable(void);

// for #ifdef GAME
#endif

// for #ifndef __GAMEMATH
#endif

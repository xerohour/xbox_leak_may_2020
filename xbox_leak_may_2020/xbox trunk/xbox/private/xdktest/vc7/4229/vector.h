#ifndef __VECTOR_H
#define __VECTOR_H

#ifndef __FLOATTYPE_H
#include "floattype.h"
#endif


//-----------------------------------------------------------------
// this file is shared between mkdata and the game.  therefore,
// use the special "(floatType)" to cast constants (allowing 
// mkdata to use high-but-slow precision and the game to use
// low-but-fast precision with the same constants).  also,
// declare any members of classes or structures as "gameFloatType"
// if the classes will be used in the game itself (to insure
// consistent structure sizes), otherwise just use floatType.
//-----------------------------------------------------------------


//==================================================================
// Typedefs and class declarations
//==================================================================

#define X 0
#define Y 1
#define Z 2
#define W 3

// doesn't seem to be a good idea right now.  gets larger and runs
// slower.
//#define INLINE_VECTOR_SET

//------------------------------------------------------------------
// V2, V3, V4, M4, M3, M2
//------------------------------------------------------------------


typedef floatType V8[8];
typedef floatType V7[7];
typedef floatType V6[6];
typedef floatType V5[5];
typedef floatType V4[4];
typedef floatType V3[3];
typedef floatType V2[2];

typedef double DV8[8];
typedef double DV7[7];
typedef double DV6[6];
typedef double DV5[5];
typedef double DV4[4];
typedef double DV3[3];
typedef double DV2[2];

#define AV8 __declspec(align(16)) V8
#define AV7 __declspec(align(16)) V7
#define AV6 __declspec(align(16)) V6
#define AV5 __declspec(align(16)) V5
#define AV4 __declspec(align(16)) V4
#define AV3 __declspec(align(16)) V3
#define AV2 __declspec(align(16)) V2

typedef floatType M4[4][4];
typedef floatType M3[3][3];
typedef floatType M2[2][2];
typedef floatType M44[4][4];
typedef floatType M43[4][3];
typedef floatType M34[3][4];
typedef floatType M33[3][3];
typedef floatType M32[3][2];
typedef floatType M22[2][2];

#define AM4  __declspec(align(16)) M4
#define AM2  __declspec(align(16)) M3
#define AM3  __declspec(align(16)) M2
#define AM44 __declspec(align(16)) M44
#define AM34 __declspec(align(16)) M34
#define AM33 __declspec(align(16)) M33
#define AM32 __declspec(align(16)) M32
#define AM22 __declspec(align(16)) M22

typedef short VS8[8];
typedef short VS7[7];
typedef short VS6[6];
typedef short VS5[5];
typedef short VS4[4];
typedef short VS3[3];
typedef short VS2[2];

typedef short MS4[4][4];
typedef short MS3[3][3];
typedef short MS2[2][2];
typedef short MS44[4][4];
typedef short MS43[4][3];
typedef short MS33[3][3];
typedef short MS32[3][2];
typedef short MS22[2][2];

#endif

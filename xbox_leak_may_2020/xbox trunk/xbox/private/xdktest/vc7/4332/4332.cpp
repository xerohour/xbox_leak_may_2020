#include <xtl.h>

#define BOOL bool

class VisibleTest
{
BOOL BoundingBoxVisible( 
D3DMATRIX *objectToClipMatrix, 
D3DVECTOR *boxCenter, 
D3DVECTOR *boxHalf, 
DWORD *visCodes 
);
};


BOOL VisibleTest::BoundingBoxVisible( 
D3DMATRIX *objectToClipMatrix, 
D3DVECTOR *boxCenter, 
D3DVECTOR *boxHalf, 
DWORD *visCodes 
) 
{ 
__declspec(align(16)) static const unsigned long xsign[4] = {0x80000000, 0x00000000, 0x80000000, 0x00000000}; 
__declspec(align(16)) static const unsigned long ysign[4] = {0x80000000, 0x80000000, 0x00000000, 0x00000000}; 
__declspec(align(16)) static const unsigned long notsign[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff}; 
__declspec(align(16)) static float box_xs[4], box_ys[4], box_zs_front[4], box_zs_back[4]; 
__declspec(align(16)) static float xs[4], ys[4]; 

int allCodes; 
int clipCodes; 

int res; 

__asm 
{ 
// push ebp 
// mov ebp, esp 
// sub esp, 8 // make room for the variables 

push ebx 
mov eax, dword ptr [visCodes] // ebp + 8 + 4*3 

push edi 
mov edx, [boxCenter] // ebp + 8 + 4*1 
mov ecx, [objectToClipMatrix] // ebp + 8 + 4*0 

push esi 
test eax, eax 
jnz COMPUTE_BOX_CORNERS 

// **************************************************************** 
// the caller doesn't care to find out if all points are visible, 
// hence a center point test may save us some time 
// **************************************************************** 
movups xmm4, xmmword ptr [edx] 

movups xmm0, xmmword ptr [ecx + 0] 
movlhps xmm5, xmm4 
movhlps xmm6, xmm4 

movups xmm1, xmmword ptr [ecx + 16] 
shufps xmm4, xmm4, 00000000b 
movups xmm2, xmmword ptr [ecx + 32] 
shufps xmm5, xmm5, 11111111b 
movups xmm3, xmmword ptr [ecx + 48] 
shufps xmm6, xmm6, 00000000b 
// 0: m03 | m02 | m01 | m00 
// 1: m13 | m12 | m11 | m10 
// 2: m23 | m22 | m21 | m20 
// 3: m33 | m32 | m31 | m30 
// 4: x | x | x | x 
// 5: y | y | y | y 
// 6: z | z | z | z 
mulps xmm0, xmm4 
mulps xmm1, xmm5 
mulps xmm2, xmm6 
// 0: m03*x | m02*x | m01*x | m00*x 
// 1: m13*y | m12*y | m11*y | m10*y 
// 2: m23*z | m22*z | m21*z | m20*z 
// 3: m33 | m32 | m31 | m30 
addps xmm0, xmm1 
addps xmm2, xmm3 
addps xmm0, xmm2 
// 0: W | Z | Y | X 
movaps xmm1, xmm0 
// 0: W | Z | Y | X 
// 1: W | Z | Y | X 
movhlps xmm2, xmm0 
movaps xmm3, xmmword ptr [notsign] 
shufps xmm2, xmm2, 01010101b 
// take absolute value of all Ws by zeroing out the sign bit 
andps xmm2, xmm3 
// 0: W | Z | Y | X 
// 1: W | Z | Y | X 
// 2: W | W | W | W 
addps xmm1, xmm2 
subps xmm2, xmm0 
// 1: W+W | Z+W | Y+W | X+W 
// 2: W-W | W-Z | W-Y | W-X 
movaps xmm0, xmm2 
// 0: W-W | W-Z | W-Y | W-X 
// 1: W+W | Z+W | Y+W | X+W 
// 2: W-W | W-Z | W-Y | W-X 
unpcklps xmm2, xmm1 
unpckhps xmm0, xmm1 
// 0: W+W | W-W | Z+W | W-Z 
// 2: Y+W | W-Y | X+W | W-X 
movmskps eax, xmm2 
movmskps ecx, xmm0 

shl ecx, 4 
and ecx, 0x30 
or eax, ecx 

jnz COMPUTE_BOX_CORNERS 

mov eax, 1 
jmp DONE 

COMPUTE_BOX_CORNERS: 
// **************************************************************** 
// either the projected points are required or the center point was not visible 
// we check all the points 
// **************************************************************** 

// **************************************************************** 
// Transform the box points 
// **************************************************************** 
// +Y 
// | 
// | 
// 6*------*7 
// /| /| 
// / | / | 
// 2*------*3 | 
// | 4*---|--*5 
// | / | /____+X 
// |/ |/ 
// 0*------*1 
// / 
// / 
// +Z 
// 
// 0: bcX-bhX, bcY-bhY, bcZ+bhZ 
// 1: bcX+bhX, bcY-bhY, bcZ+bhZ 
// 2: bcX-bhX, bcY+bhY, bcZ+bhZ 
// 3: bcX+bhX, bcY+bhY, bcZ+bhZ 
// 4: bcX-bhX, bcY-bhY, bcZ-bhZ 
// 5: bcX+bhX, bcY-bhY, bcZ-bhZ 
// 6: bcX-bhX, bcY+bhY, bcZ-bhZ 
// 7: bcX+bhX, bcY+bhY, bcZ-bhZ 
// 

// **************************************************************** 
// first we compute the bounding box corner points 
// so we can transform them 
// **************************************************************** 
mov dword ptr [allCodes], 0xffffffff // ebp - 8 
mov eax, dword ptr [boxCenter] // ebp + 8 + 4*1 
prefetchnta dword ptr [xsign] 

mov dword ptr [clipCodes], 0 // ebp - 4 
mov ecx, dword ptr [boxHalf] // ebp + 8 + 4*2 
prefetchnta dword ptr [ysign] 

movups xmm0, xmmword ptr [eax] 
// 0: bcW | bcZ | bcY | bcX 
movups xmm5, xmmword ptr [ecx] 
// 0: bcW | bcZ | bcY | bcX 
// 5: bhW | bhZ | bhY | bhX 
movlhps xmm1, xmm0 
movhlps xmm2, xmm0 
// 0: bcW | bcZ | bcY | bcX 
// 1: bcY | bcX | | 
// 2: | | bcW | bcZ 
// 5: bhW | bhZ | bhY | bhX 
shufps xmm0, xmm0, 00000000b 
movlhps xmm6, xmm5 
movhlps xmm7, xmm5 

shufps xmm1, xmm1, 11111111b 
movss xmm3, xmm2 
// 0: bcX | bcX | bcX | bcX 
// 1: bcY | bcY | bcY | bcY 
// 2: | | bcW | bcZ 
// 3: | | | bcZ 
// 5: bhW | bhZ | bhY | bhX 
// 6: bhY | bhX | | 
// 7: | | bhW | bhZ 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 11111111b 
// 0: bcX | bcX | bcX | bcX 
// 1: bcY | bcY | bcY | bcY 
// 2: | | bcW | bcZ 
// 3: | | | bcZ 
// 5: bhX | bhX | bhX | bhX 
// 6: bhY | bhY | bhY | bhY 
// 7: | | bhW | bhZ 
addss xmm2, xmm7 
subss xmm3, xmm7 
// 0: bcX | bcX | bcX | bcX 
// 1: bcY | bcY | bcY | bcY 
// 2: | | bcW | bcZ+bhZ 
// 3: | | | bcZ-bhZ 
// 5: bhX | bhX | bhX | bhX 
// 6: bhY | bhY | bhY | bhY 
// 7: | | bhW | bhZ 
xorps xmm5, xmmword ptr [xsign] 
xorps xmm6, xmmword ptr [ysign] 
// 0: bcX | bcX | bcX | bcX 
// 1: bcY | bcY | bcY | bcY 
// 2: | | bcW | bcZ+bhZ 
// 5:-bhX | bhX |-bhX | bhX 
// 6:-bhY |-bhY | bhY | bhY 
// 7: | | bhW | bhZ 
shufps xmm2, xmm2, 00000000b 
shufps xmm3, xmm3, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: bcX | bcX | bcX | bcX 
// 1: bcY | bcY | bcY | bcY 
// 2: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
// 5:-bhX | bhX |-bhX | bhX 
// 6:-bhY |-bhY | bhY | bhY 
// 7: bhZ | bhZ | bhZ | bhZ 
addps xmm0, xmm5 
addps xmm1, xmm6 
// 0: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 1: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 2: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 

// **************************************************************** 
// store the box_xs, box_ys, and zs so we can use them again later 
// **************************************************************** 
movntps xmmword ptr [box_xs], xmm0 
movntps xmmword ptr [box_ys], xmm1 
movntps xmmword ptr [box_zs_front], xmm2 
movntps xmmword ptr [box_zs_back], xmm3 

// **************************************************************** 
// now we transform the first 4 box corner points (box's front face) 
// **************************************************************** 
mov edx, dword ptr [objectToClipMatrix] // ebp + 8 + 4*0 

movss xmm4, dword ptr [edx + 0 + 12] 
movss xmm5, dword ptr [edx + 16 + 12] 
movss xmm6, dword ptr [edx + 32 + 12] 
movss xmm7, dword ptr [edx + 48 + 12] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 1: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 2: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 4: m03 | m03 | m03 | m03 
// 5: m13 | m13 | m13 | m13 
// 6: m23 | m23 | m23 | m23 
// 7: m33 | m33 | m33 | m33 
mulps xmm0, xmm4 
mulps xmm1, xmm5 
mulps xmm2, xmm6 
// 0: (bcX-bhX)*m03 | (bcX+bhX)*m03 | (bcX-bhX)*m03 | (bcX+bhX)*m03 
// 1: (bcY-bhY)*m13 | (bcY-bhY)*m13 | (bcY+bhY)*m13 | (bcY+bhY)*m13 
// 2: (bcZ+bhZ)*m23 | (bcZ+bhZ)*m23 | (bcZ+bhZ)*m23 | (bcZ+bhZ)*m23 
// 7: m33 | m33 | m33 | m33 
addps xmm0, xmm1 
addps xmm2, xmm7 
addps xmm0, xmm2 
// 0: w0 | w1 | w2 | w3 
movaps xmm4, xmmword ptr [notsign] 
movaps xmm1, xmmword ptr [box_xs] 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_front] 
andps xmm0, xmm4 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
movss xmm4, dword ptr [edx + 0 + 0] 
movss xmm5, dword ptr [edx + 16 + 0] 
movss xmm6, dword ptr [edx + 32 + 0] 
movss xmm7, dword ptr [edx + 48 + 0] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 4: m00 | m00 | m00 | m00 
// 5: m10 | m10 | m10 | m10 
// 6: m20 | m20 | m20 | m20 
// 7: m30 | m30 | m30 | m30 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m00 | (bcX+bhX)*m00 | (bcX-bhX)*m00 | (bcX+bhX)*m00 
// 2: (bcY-bhY)*m10 | (bcY-bhY)*m10 | (bcY+bhY)*m10 | (bcY+bhY)*m10 
// 3: (bcZ+bhZ)*m20 | (bcZ+bhZ)*m20 | (bcZ+bhZ)*m20 | (bcZ+bhZ)*m20 
// 7: m30 | m30 | m30 | m30 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_front] 
movntps xmmword ptr [xs], xmm1 
movaps xmm1, xmmword ptr [box_xs] 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
movss xmm4, dword ptr [edx + 0 + 4] 
movss xmm5, dword ptr [edx + 16 + 4] 
movss xmm6, dword ptr [edx + 32 + 4] 
movss xmm7, dword ptr [edx + 48 + 4] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 4: m01 | m01 | m01 | m01 
// 5: m11 | m11 | m11 | m11 
// 6: m21 | m21 | m21 | m21 
// 7: m31 | m31 | m31 | m31 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m01 | (bcX+bhX)*m01 | (bcX-bhX)*m01 | (bcX+bhX)*m01 
// 2: (bcY-bhY)*m11 | (bcY-bhY)*m11 | (bcY+bhY)*m11 | (bcY+bhY)*m11 
// 3: (bcZ+bhZ)*m21 | (bcZ+bhZ)*m21 | (bcZ+bhZ)*m21 | (bcZ+bhZ)*m21 
// 7: m31 | m31 | m31 | m31 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: y0 | y1 | y2 | y3 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_front] 
movntps xmmword ptr [ys], xmm1 
movaps xmm1, xmmword ptr [box_xs] 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
movss xmm4, dword ptr [edx + 0 + 8] 
movss xmm5, dword ptr [edx + 16 + 8] 
movss xmm6, dword ptr [edx + 32 + 8] 
movss xmm7, dword ptr [edx + 48 + 8] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ+bhZ | bcZ+bhZ | bcZ+bhZ | bcZ+bhZ 
// 4: m02 | m02 | m02 | m02 
// 5: m12 | m12 | m12 | m12 
// 6: m22 | m22 | m22 | m22 
// 7: m32 | m32 | m32 | m32 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m02 | (bcX+bhX)*m02 | (bcX-bhX)*m02 | (bcX+bhX)*m02 
// 2: (bcY-bhY)*m12 | (bcY-bhY)*m12 | (bcY+bhY)*m12 | (bcY+bhY)*m12 
// 3: (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 
// 7: m32 | m32 | m32 | m32 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: z0 | z1 | z2 | z3 

// **************************************************************** 
// we've compute the 4 xyzw's, so now compute their clip codes 
// we compute the z clip codes first 
// **************************************************************** 
movaps xmm2, xmm0 
// 0: w0 | w1 | w2 | w3 
// 1: z0 | z1 | z2 | z3 
// 2: w0 | w1 | w2 | w3 
subps xmm2, xmm1 
addps xmm1, xmm0 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: w0-z0 | w1-z1 | w2-z2 | w3-z3 
movaps xmm3, xmm2 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: w0-z0 | w1-z1 | w2-z2 | w3-z3 
// 3: w0-z0 | w1-z1 | w2-z2 | w3-z3 
unpcklps xmm2, xmm1 
unpckhps xmm3, xmm1 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: z2+w2 | w2-z2 | z3+w3 | w3-z3 
// 3: z0+w0 | w0-z0 | z1+w1 | w1-z1 
movmskps ecx, xmm2 // z2 | z3 clip codes 
movmskps eax, xmm3 // z0 | z1 clip codes 

// **************************************************************** 
// now that we have the z clip codes, compute the xy clip codes as well 
// **************************************************************** 
movaps xmm1, xmmword ptr [xs] 
movaps xmm2, xmmword ptr [ys] 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
// 2: y0 | y1 | y2 | y3 
movaps xmm3, xmm0 
mov edx, ecx 
movaps xmm4, xmm0 
mov ebx, eax 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
// 2: y0 | y1 | y2 | y3 
// 3: w0 | w1 | w2 | w3 
// 4: w0 | w1 | w2 | w3 
subps xmm3, xmm1 
shl eax, 2 
subps xmm4, xmm2 
shl ebx, 4 
addps xmm1, xmm0 
shl ecx, 2 
addps xmm2, xmm0 
shl edx, 4 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movaps xmm5, xmm1 
and eax, 0x30 // z0 clip codes 
movaps xmm6, xmm3 
and ebx, 0x30 // z1 clip codes 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 6: w0-x0 | w1-x1 | w2-x2 | w3-x3 
unpcklps xmm5, xmm2 
and ecx, 0x30 // z2 clip codes 
unpcklps xmm6, xmm4 
and edx, 0x30 // z3 clip codes 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: w2-y2 | w2-x2 | w3-y3 | w3-x3 
movaps xmm0, xmm6 
// 0: w2-y2 | w2-x2 | w3-y3 | w3-x3 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: w2-y2 | w2-x2 | w3-y3 | w3-x3 
unpcklps xmm6, xmm5 
unpckhps xmm0, xmm5 
// 0: y2+w2 | w2-y2 | x2+w2 | w2-x2 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: y3+w3 | w3-y3 | x3+w3 | w3-x3 
movmskps edi, xmm6 // xy3 clip codes 
movmskps esi, xmm0 // xy2 clip codes 

or edx, edi // xyz3 clip codes 
or ecx, esi // xyz2 clip codes 

// **************************************************************** 
// if any clip code was zero, and we don't need to compute all clip codes 
// we can just return now, knowing at least one corner of the box is visible 
// **************************************************************** 
mov edi, dword ptr [visCodes] // ebp + 8 + 4*3 
test edi, edi 
jnz CLIPCODES_2MORE 

test ecx, ecx 
jnz CHECK_XYZ3_CLIPCODE 

mov eax, 1 
jmp DONE 

CHECK_XYZ3_CLIPCODE: 
test edx, edx 
jnz CLIPCODES_2MORE 

mov eax, 1 
jmp DONE 

CLIPCODES_2MORE: 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
unpckhps xmm3, xmm4 
unpckhps xmm1, xmm2 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movaps xmm0, xmm3 
// 0: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
unpcklps xmm3, xmm1 
unpckhps xmm0, xmm1 
// 0: y0+w0 | w0-y0 | x0+w0 | w0-x0 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: y1+w1 | w1-y1 | x1+w1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movmskps edi, xmm3 // xy1 clip codes 
movmskps esi, xmm0 // xy0 clip codes 

or ebx, edi // xyz1 clip codes 
or eax, esi // xyz0 clip codes 

mov edi, dword ptr [visCodes] // ebp + 8 + 4*3 
test edi, edi 
jnz UPDATE_CLIPCODES 

test eax, eax 
jnz CHECK_XYZ1_CLIPCODE 

mov eax, 1 
jmp DONE 

CHECK_XYZ1_CLIPCODE: 
test ebx, ebx 
jnz UPDATE_CLIPCODES 

mov eax, 1 
jmp DONE 

UPDATE_CLIPCODES: 
// **************************************************************** 
// combine all the clip codes 
// **************************************************************** 
mov edi, dword ptr [allCodes] // ebp - 8 
mov esi, dword ptr [clipCodes] // ebp - 4 

and edi, eax 
or esi, eax 
and edi, ebx 
or esi, ebx 
and edi, ecx 
or esi, ecx 
and edi, edx 
or esi, edx 

mov dword ptr [allCodes], edi // ebp - 8 
mov dword ptr [clipCodes], esi // ebp - 4 

// **************************************************************** 
// now we transform the last 4 box corner points (box's back face) 
// **************************************************************** 
mov edx, dword ptr [objectToClipMatrix] // ebp + 8 + 4*0 

movaps xmm0, xmmword ptr [box_xs] 
movaps xmm1, xmmword ptr [box_ys] 
movaps xmm2, xmmword ptr [box_zs_back] 
// 0: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 1: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 2: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
movss xmm4, dword ptr [edx + 0 + 12] 
movss xmm5, dword ptr [edx + 16 + 12] 
movss xmm6, dword ptr [edx + 32 + 12] 
movss xmm7, dword ptr [edx + 48 + 12] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 1: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 2: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
// 4: m03 | m03 | m03 | m03 
// 5: m13 | m13 | m13 | m13 
// 6: m23 | m23 | m23 | m23 
// 7: m33 | m33 | m33 | m33 
mulps xmm0, xmm4 
mulps xmm1, xmm5 
mulps xmm2, xmm6 
// 0: (bcX-bhX)*m03 | (bcX+bhX)*m03 | (bcX-bhX)*m03 | (bcX+bhX)*m03 
// 1: (bcY-bhY)*m13 | (bcY-bhY)*m13 | (bcY+bhY)*m13 | (bcY+bhY)*m13 
// 2: (bcZ-bhZ)*m23 | (bcZ-bhZ)*m23 | (bcZ-bhZ)*m23 | (bcZ-bhZ)*m23 
// 7: m33 | m33 | m33 | m33 
addps xmm0, xmm1 
addps xmm2, xmm7 
addps xmm0, xmm2 
// 0: w0 | w1 | w2 | w3 
// take absolute value of all Ws by zeroing out the sign bit 
movaps xmm4, xmmword ptr [notsign] 
movaps xmm1, xmmword ptr [box_xs] 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_back] 
andps xmm0, xmm4 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
movss xmm4, dword ptr [edx + 0 + 0] 
movss xmm5, dword ptr [edx + 16 + 0] 
movss xmm6, dword ptr [edx + 32 + 0] 
movss xmm7, dword ptr [edx + 48 + 0] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
// 4: m00 | m00 | m00 | m00 
// 5: m10 | m10 | m10 | m10 
// 6: m20 | m20 | m20 | m20 
// 7: m30 | m30 | m30 | m30 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m00 | (bcX+bhX)*m00 | (bcX-bhX)*m00 | (bcX+bhX)*m00 
// 2: (bcY-bhY)*m10 | (bcY-bhY)*m10 | (bcY+bhY)*m10 | (bcY+bhY)*m10 
// 3: (bcZ-bhZ)*m20 | (bcZ-bhZ)*m20 | (bcZ-bhZ)*m20 | (bcZ-bhZ)*m20 
// 7: m30 | m30 | m30 | m30 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_back] 
movntps xmmword ptr [xs], xmm1 
movaps xmm1, xmmword ptr [box_xs] 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
movss xmm4, dword ptr [edx + 0 + 4] 
movss xmm5, dword ptr [edx + 16 + 4] 
movss xmm6, dword ptr [edx + 32 + 4] 
movss xmm7, dword ptr [edx + 48 + 4] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
// 4: m01 | m01 | m01 | m01 
// 5: m11 | m11 | m11 | m11 
// 6: m21 | m21 | m21 | m21 
// 7: m31 | m31 | m31 | m31 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m01 | (bcX+bhX)*m01 | (bcX-bhX)*m01 | (bcX+bhX)*m01 
// 2: (bcY-bhY)*m11 | (bcY-bhY)*m11 | (bcY+bhY)*m11 | (bcY+bhY)*m11 
// 3: (bcZ-bhZ)*m21 | (bcZ-bhZ)*m21 | (bcZ-bhZ)*m21 | (bcZ-bhZ)*m21 
// 7: m31 | m31 | m31 | m31 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: y0 | y1 | y2 | y3 
movaps xmm2, xmmword ptr [box_ys] 
movaps xmm3, xmmword ptr [box_zs_back] 
movntps xmmword ptr [ys], xmm1 
movaps xmm1, xmmword ptr [box_xs] 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
movss xmm4, dword ptr [edx + 0 + 8] 
movss xmm5, dword ptr [edx + 16 + 8] 
movss xmm6, dword ptr [edx + 32 + 8] 
movss xmm7, dword ptr [edx + 48 + 8] 
shufps xmm4, xmm4, 00000000b 
shufps xmm5, xmm5, 00000000b 
shufps xmm6, xmm6, 00000000b 
shufps xmm7, xmm7, 00000000b 
// 0: w0 | w1 | w2 | w3 
// 1: bcX-bhX | bcX+bhX | bcX-bhX | bcX+bhX 
// 2: bcY-bhY | bcY-bhY | bcY+bhY | bcY+bhY 
// 3: bcZ-bhZ | bcZ-bhZ | bcZ-bhZ | bcZ-bhZ 
// 4: m02 | m02 | m02 | m02 
// 5: m12 | m12 | m12 | m12 
// 6: m22 | m22 | m22 | m22 
// 7: m32 | m32 | m32 | m32 
mulps xmm1, xmm4 
mulps xmm2, xmm5 
mulps xmm3, xmm6 
// 0: w0 | w1 | w2 | w3 
// 1: (bcX-bhX)*m02 | (bcX+bhX)*m02 | (bcX-bhX)*m02 | (bcX+bhX)*m02 
// 2: (bcY-bhY)*m12 | (bcY-bhY)*m12 | (bcY+bhY)*m12 | (bcY+bhY)*m12 
// 3: (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 | (bcZ+bhZ)*m22 
// 7: m32 | m32 | m32 | m32 
addps xmm1, xmm2 
addps xmm3, xmm7 
addps xmm1, xmm3 
// 0: w0 | w1 | w2 | w3 
// 1: z0 | z1 | z2 | z3 

// **************************************************************** 
// we've compute the 4 xyzw's, so now compute their clip codes 
// we compute the z clip codes first 
// **************************************************************** 
movaps xmm2, xmm0 
// 0: w0 | w1 | w2 | w3 
// 1: z0 | z1 | z2 | z3 
// 2: w0 | w1 | w2 | w3 
subps xmm2, xmm1 
addps xmm1, xmm0 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: w0-z0 | w1-z1 | w2-z2 | w3-z3 
movaps xmm3, xmm2 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: w0-z0 | w1-z1 | w2-z2 | w3-z3 
// 3: w0-z0 | w1-z1 | w2-z2 | w3-z3 
unpcklps xmm2, xmm1 
unpckhps xmm3, xmm1 
// 0: w0 | w1 | w2 | w3 
// 1: z0+w0 | z1+w1 | z2+w2 | z3+w3 
// 2: z2+w2 | w2-z2 | z3+w3 | w3-z3 
// 3: z0+w0 | w0-z0 | z1+w1 | w1-z1 
movmskps ecx, xmm2 // z2 | z3 clip codes 
movmskps eax, xmm3 // z0 | z1 clip codes 

// **************************************************************** 
// now that we have the z clip codes, compute the xy clip codes as well 
// **************************************************************** 
movaps xmm1, xmmword ptr [xs] 
movaps xmm2, xmmword ptr [ys] 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
// 2: y0 | y1 | y2 | y3 
movaps xmm3, xmm0 
mov edx, ecx 
movaps xmm4, xmm0 
mov ebx, eax 
// 0: w0 | w1 | w2 | w3 
// 1: x0 | x1 | x2 | x3 
// 2: y0 | y1 | y2 | y3 
// 3: w0 | w1 | w2 | w3 
// 4: w0 | w1 | w2 | w3 
subps xmm3, xmm1 
shl eax, 2 
subps xmm4, xmm2 
shl ebx, 4 
addps xmm1, xmm0 
shl ecx, 2 
addps xmm2, xmm0 
shl edx, 4 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movaps xmm5, xmm1 
and eax, 0x30 // z0 clip codes 
movaps xmm6, xmm3 
and ebx, 0x30 // z1 clip codes 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 6: w0-x0 | w1-x1 | w2-x2 | w3-x3 
unpcklps xmm5, xmm2 
and ecx, 0x30 // z2 clip codes 
unpcklps xmm6, xmm4 
and edx, 0x30 // z3 clip codes 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: w2-y2 | w2-x2 | w3-y3 | w3-x3 
movaps xmm0, xmm6 
// 0: w2-y2 | w2-x2 | w3-y3 | w3-x3 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: w2-y2 | w2-x2 | w3-y3 | w3-x3 
unpcklps xmm6, xmm5 
unpckhps xmm0, xmm5 
// 0: y2+w2 | w2-y2 | x2+w2 | w2-x2 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
// 5: y2+w2 | x2+w2 | y3+w3 | x3+w3 
// 6: y3+w3 | w3-y3 | x3+w3 | w3-x3 
movmskps edi, xmm6 // xy3 clip codes 
movmskps esi, xmm0 // xy2 clip codes 

or edx, edi // xyz3 clip codes 
or ecx, esi // xyz2 clip codes 

// **************************************************************** 
// if any clip code was zero, and we don't need to compute all clip codes 
// we can just return now, knowing at least one corner of the box is visible 
// **************************************************************** 
mov edi, dword ptr [visCodes] // ebp + 8 + 4*3 
test edi, edi 
jnz CLIPCODES_2MORE_2ND 

test ecx, ecx 
jnz CHECK_XYZ2_CLIPCODE 

mov eax, 1 
jmp DONE 

CHECK_XYZ2_CLIPCODE: 
test edx, edx 
jnz CLIPCODES_2MORE_2ND 

mov eax, 1 
jmp DONE 

CLIPCODES_2MORE_2ND: 
// 1: x0+w0 | x1+w1 | x2+w2 | x3+w3 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-x0 | w1-x1 | w2-x2 | w3-x3 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
unpckhps xmm3, xmm4 
unpckhps xmm1, xmm2 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movaps xmm0, xmm3 
// 0: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: w0-y0 | w0-x0 | w1-y1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
unpcklps xmm3, xmm1 
unpckhps xmm0, xmm1 
// 0: y0+w0 | w0-y0 | x0+w0 | w0-x0 
// 1: y0+w0 | x0+w0 | y1+w1 | x1+w1 
// 2: y0+w0 | y1+w1 | y2+w2 | y3+w3 
// 3: y1+w1 | w1-y1 | x1+w1 | w1-x1 
// 4: w0-y0 | w1-y1 | w2-y2 | w3-y3 
movmskps edi, xmm3 // xy1 clip codes 
movmskps esi, xmm0 // xy0 clip codes 

or ebx, edi // xyz1 clip codes 
or eax, esi // xyz0 clip codes 

mov edi, dword ptr [visCodes] // ebp + 8 + 4*3 
test edi, edi 
jnz UPDATE_CLIPCODES_2ND 

test eax, eax 
jnz CHECK_XYZ0_CLIPCODE 

mov eax, 1 
jmp DONE 

CHECK_XYZ0_CLIPCODE: 
test ebx, ebx 
jnz UPDATE_CLIPCODES_2ND 

mov eax, 1 
jmp DONE 

UPDATE_CLIPCODES_2ND: 
// **************************************************************** 
// combine all the clip codes 
// **************************************************************** 
mov edi, dword ptr [allCodes] // ebp - 8 
mov esi, dword ptr [clipCodes] // ebp - 4 

and edi, eax 
or esi, eax 
and edi, ebx 
or esi, ebx 
and edi, ecx 
or esi, ecx 
and edi, edx 
or esi, edx 

mov dword ptr [allCodes], edi // ebp - 8 
mov dword ptr [clipCodes], esi // ebp - 4 

mov eax, dword ptr [visCodes] // ebp + 8 + 4*3 
test eax, eax 
jz TEST_ALLCODES 

mov dword ptr [eax], esi // *visCodes = clipCodes; 

TEST_ALLCODES: 

mov eax, 1 // return TRUE 

test edi, edi 
jz DONE 

xor eax, eax // return FALSE 

DONE: 
pop esi 
pop edi 
pop ebx 

// mov esp, ebp 
// pop ebp 

// ret 

mov dword ptr [res], eax // store the result 
} 

return res; 
} 

void __cdecl main() { return; }

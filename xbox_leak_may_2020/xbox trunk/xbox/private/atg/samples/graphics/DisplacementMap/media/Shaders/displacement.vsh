;------------------------------------------------------------------------------
; Displacement map vertex shader
;
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
xvs.1.1

#include "displacement.h"

;------------------------------------------------------------------------------
; VERTEX STREAMS:
;  v0 = indices
;  v1 = weights
;  v2 = displacement, separate channels in x, y, z, and w (4 octaves)
;
; CONSTANTS:
;  c[VSC_WORLDVIEWPROJECTION]         = world * view * projection matrix
;  c[VSC_INDEX]                       = x:index scale
;  c[VSC_DISPLACEMENT]                = x:displacement scale y:displacement offset
;  c[VSC_ONES]                        = 1 1 1 1
;  c[VSC_CARRIER_VERTEX_PALETTE ... ] = carrier mesh xyz:vertex position w:u texture coord
;  c[VSC_CARRIER_NORMAL_PALETTE ... ] = carrier mesh xyz:vertex normal   w:v texture coord
;------------------------------------------------------------------------------

; Rename a0.x to read more easily below.
#define index a0.x

; Unfortunately, we have to do a scale to convert the normalized
; color to an index.  It would have been nice to have UBYTE4!
macro setindex source
    mul r1.x, c[VSC_INDEX].x, %source
    mov index, r1.x
endm

        ; blend vertices, normals, and texture coords,
	; packed into the constants as (x y z u) (nx ny nz v)
	setindex v0.x
        mul r0, v1.x, c[index + VSC_CARRIER_VERTEX_PALETTE]
	mul r2, v1.x, c[index + VSC_CARRIER_NORMAL_PALETTE]

	setindex v0.y
        mad r0, v1.y, c[index + VSC_CARRIER_VERTEX_PALETTE], r0
        mad r2, v1.y, c[index + VSC_CARRIER_NORMAL_PALETTE], r2

	setindex v0.z
        mad r0, v1.z, c[index + VSC_CARRIER_VERTEX_PALETTE], r0
        mad r2, v1.z, c[index + VSC_CARRIER_NORMAL_PALETTE], r2

	; set texture coords
	mul oT0.x, c[VSC_TEXTURESCALE], r0.w
	mul oT0.y, c[VSC_TEXTURESCALE], r2.w

	; set w to 1
	sge r0.w, r0.w, r0.w

	; scale and bias displacement, 4 octaves
	mad r3.x, v2.x, c[VSC_DISPLACEMENT0].x, c[VSC_DISPLACEMENT0].y
	mad r3.y, v2.y, c[VSC_DISPLACEMENT1].x, c[VSC_DISPLACEMENT1].y
	mad r3.z, v2.z, c[VSC_DISPLACEMENT2].x, c[VSC_DISPLACEMENT2].y
	mad r3.w, v2.w, c[VSC_DISPLACEMENT3].x, c[VSC_DISPLACEMENT3].y
	dp4 r3.x, r3, c[VSC_ONES]	; add up all the scaled and biased displacements

	; offset along normal
	mad r0.xyz, r3.x, r2, r0		; add displacement along normal

        ; transform vertex->screen
        m4x4 oPos, r0, c[VSC_WORLDVIEWPROJECTION]

;------------------------------------------------------------------------------
; Hair/Fur/Grass shell vertex shader
;
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; VERTEX STREAMS:
;	v0 = vert
;	v1 = normal
;	v2 = tex coords
;
;	
; REGISTERS:
;	r0 = object-space vertex
;	r1 = scratch
;	r2 = scratch
;	r3 = E : local eye direction
;	r4 = L : light direction
;	r5 = H : (E+L)/2
;	r6 = hair tangent
;	r7 = object-space tangent plane normal
;	r8 = object-space tangent plane origin
;	r9 = expansion factor / distance from tangent plane
;	
; CONSTANTS:
;	c[0,3] = world*view*proj matrix
;	c12 = x:expansion along normal, y:total height of fur (for banks shadow term), z:0.5, w:0.0
;	c13 = eye position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c14 = light position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c15 = half vector (used only when DIRECTIONAL_LIGHTING is defined)
;	c16 = diffuse conditioning
;	c17 = ambient conditioning
;	c20 = wind origin x,y,z and w:local wind amplitude
;	c21 = wind x:penalty term for going below tangent plane, y:fraction of out-of-tangent plane motion
;	c22 = wind up
;------------------------------------------------------------------------------

	mov r0,v0		; original vertex position
	mov r9,c12.xxxw 	; expansion factor
	mad r0,r9,v1,r0		; move position along scaled expansion vector

	; texture coords for hair volume
	mov oT0,v2
	mov oT2,v2 		; DEBUG: this should not be needed, but SetTextureStageState(2, 0) doesn't seem to work

	; normalize normal
	dp3 r7.w,v1,v1		; get squared length of normal
	rsq r7.w,r7.w		; 1/length of normal
	mul r7,v1,r7.www	; normalize

	; r7 is tangent plane normal
	mov r8,v0		; tangent plane origin
#ifdef WIND
	rcp r7.w,r7.w		; length of normal
	mul r9, r9, r7.w	; length of scaled expansion vector along normal

	; r0 is current vertex position
	; r9.x = windiness  r9.y = 1/squared maximum offset from tangent plane origin
#include "wind.vsh"
	; r0 is blown vertex position
	; r6 is blown hair tangent
#else
	; r0 is left unchanged
	mov r6,r7		; hair tangent is currently the same as the underlying mesh
#endif

#include "eyelighthalf.vsh"
	mov r1,c16			; diffuse conditioning color
#include "hairlighting.vsh"

	; vertex->screen
	dp4 oPos.x,r0,c0
	dp4 oPos.y,r0,c1
	dp4 oPos.z,r0,c2
	dp4 oPos.w,r0,c3

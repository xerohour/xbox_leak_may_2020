;------------------------------------------------------------------------------
; Hair/Fur/Grass Volume texture vertex shader
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; VERTEX STREAMS:
;	v0 = vert
;	v1 = normal
;	v2 = tex coord
;	v3 = S texture vector (if MULTISHELL is defined)
;	v4 = T texture vector (if MULTISHELL is defined)
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
;	
; CONSTANTS:
;	c[0,3] = world*view*proj matrix
;	c12 = x:shell1 expansion along normal, z:1/2, w:0
;	c13 = eye position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c14 = light position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c15 = half vector (used only when DIRECTIONAL_LIGHTING is defined)
;	c16 = diffuse conditioning
;	c17 = ambient conditioning
;	c18 = self shadowing constants
;	c19 = y:shell2 expansion delta from shell1, z:shell3 expansion delta from shell1
;	c20 = wind origin x,y,z and w:local wind amplitude
;	c21 = wind x:penalty term for going below tangent plane, y:fraction of out-of-tangent plane motion
;	c22 = wind up
;------------------------------------------------------------------------------
		
	; expand along normal for shell1
	mul r0,v1,c12.xxxw
	add r0,r0,v0

	; texture coords for hair volume
	mov oT0,v2

	mov r7,v1		; tangent plane normal
	mov r8,v0		; tangent plane origin
#ifdef WIND
	; r0 is current vertex position
	mov r3,c12.xxxw	; x:windiness, y:maximum offset length from tangent plane origin
#include "wind.vsh"
#else
	; r0 is left unchanged
	mov r6,r7		; hair tangent is currently the same as the underlying mesh
#endif

#include "hairlighting.vsh"
	; texture T1 is used to compute the hair lighting
	; r3 is now eye vector

#ifdef MULTISHELL
	; Offset texture layers 2 and 3 based on projected eye vector.
	dp3 r1.w, r3, r6		; calc dot(eye, offsetdir)
	mad r1.xyz, r1.w, -r6, r3	; calc eye - dot(eye, offsetdir) offsetdir
	dp3 r2.x, r1, v3	; get S texture displacement
	dp3 r2.y, r1, v4	; get T texture displacement
	; Output texture coords for shell2 and shell3
	mad oT2.xy, r2.xy, c19.yy, v2.xy	; scale texture offset by distance between shell1 and shell2
	mad oT3.xy, r2.xy, c19.zz, v2.xy	; scale texture offset by distance between shell1 and shell3
#endif

	; vertex->screen
	dp4 oPos.x,r0,c0
	dp4 oPos.y,r0,c1
	dp4 oPos.z,r0,c2
	dp4 oPos.w,r0,c3

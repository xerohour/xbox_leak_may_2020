;------------------------------------------------------------------------------
; Hair/Fur/Grass fin vertex shader
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;v0 - verts0
;v1 - normals0
;v2 - uv0
;
;v3 - verts1
;v4 - normals1
;v5 - uv1
;
;v6 - selector
;
;registers
;r0 - object-space vertex
;r1 - scratch
;r2 - scratch
;r3 - E = local eye direction
;r4 - L = light direction
;r5 - H = (E+L)/2
;r6 - hair tangent
;r7 - object-space normal
;
;inputs
;c[0,3] - world*view*proj matrix
;c12 - x = expansion along normal, y=total height of fur (for banks shadow term), z=0.5, w=0.0
;c13 - eye position in object space
;c14 - light position in object space
;c16 - diffuse conditioning
;c17 - ambient conditioning
;c20 - wind origin x,y,z and w=local wind amplitude
;c21 - wind x=penalty term for going below tangent plane, y=fraction of out-of-tangent plane motion
;c22 - wind up
;------------------------------------------------------------------------------
		
	; select vertex based on selector stream

	; if stream 0 is active, then load vertex position, normal, and uv's
	mov r1,v6.xxxx		; a
	mul r2,v1,c12.xxxw	; expand fin by epsilon to move off of the surface
	add r2,r2,v0		; offset by base vertex
	mul r0,r2,r1		; position
#ifdef WIND
	mul r3,c12.xxxw,r1	; windiness, maximum offset length
	mul r8,v0,r1		; tangent plane origin
#endif
	mul r6,v1,r1		; hair tangent
	mul r7,v2,r1		; uv's

	; if stream 1 is active, then load offset vertex position, normal, and uv's
	sge r1,r1,r1	
	sub r1,r1,v6.xxxx	; 1-a
	mul r2,v4,c12.yyyw	; expand fin by total height of fur
	add r2,r2,v3		; offset by base vertex
	mad r0,r2,r1,r0		; position
#ifdef WIND
	mad r3,c12.yyyw,r1,r3	; windiness, maximum offset length
	mad r8,v3,r1,r8		; tangent plane origin
#endif
	mad r6,v4,r1,r6		; hair tangent
	mad r7,v5,r1,r7		; uv's

	; texture coords for hair volume
	add oT0.x,r7.x,r7.y	; DEBUG: this should really be per-fin, not per-vertex
	mov oT0.y,v6.x

	mov r7,r6			; tangent plane normal
#ifdef WIND
	; r0 is current vertex position
	; r8 is tangent plane origin
#include "wind.vsh"
	; r0 is blown vertex position
	; r6 is blown hair tangent
#endif

#include "hairlighting.vsh"

	; vertex->screen
	dp4 oPos.x,r0,c0
	dp4 oPos.y,r0,c1	
	dp4 oPos.z,r0,c2
	dp4 oPos.w,r0,c3



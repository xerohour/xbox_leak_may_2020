; mxpalskin3.vsh -- 3-matrix indexed vertex blending in vs - can take 12 world matrices
; written by Trevor Schrock

; constants:
;				c0:			(0.0f, 1.0f, 4.0f, 0.0f)
;				c1 - c4:	transform mx into clip space from world
;				c8 - c11:	blending mx #0
;				c12 - c15:	blending mx #1
;				c16 - c19:	blending mx #3
;				...
;				c52 - c55:	blending mx #11

; vertex spec:	v0:			position (world space)
;				v1:			blending weights
;				v2:			blending indices
;				v3:			normal
;				v7:			texture coords

vs.1.1
def		c0,		0.0f,	1.0f,	4.0f,	0.0f

; multiply by the first indexed matrix
mul		r0,		v2.x,	c0.z				; idx * 4
mov		a0.x,	r0
m4x4	r0,		v0,		c[8 + a0.x]			; first blend

; multiply by the second indexed matrix
mul		r1,		v2.y,	c0.z				; idx * 4
mov		a0.x,	r1
m4x4	r1,		v0,		c[8 + a0.x]			; second blend

; multiply by the third indexed matrix
mul		r2,		v2.z,	c0.z				; idx * 4
mov		a0.x,	r2
m4x4	r2,		v0,		c[8 + a0.x]			; third blend

; lerp the three based on the weight value
add		r3,		c0.y,	-v1.x				; 1.0f - v1.x
add		r3,		r3,		-v1.y				; 1.0f - v1.x - v1.y
mul		r4,		r0,		v1.x				; first blend * first weight
mad		r4,		r1,		v1.y,	r4			; + second blend * second weight
mad		r4,		r2,		r3,		r4			; + third blend * remaining weight

; multiply by the view/proj matrix
m4x4		oPos,	r4,		c1

; fake the lighting for now
mov		oD0,	c0.y						; diffuse lighting = 1.0f
mov		oD1,	c0.y						; specular lighting = 1.0f

; textures just copy directly
mov		oT0,	v7							; texture coords remain the same

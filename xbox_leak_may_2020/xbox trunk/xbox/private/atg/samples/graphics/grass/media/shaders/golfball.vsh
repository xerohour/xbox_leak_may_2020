;------------------------------------------------------------------------------
; Golfball vertex shader
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;	v0	= verts
;	v1	= normals
;
;registers
;	r0	= final vertex
;
;inputs
;	c20-c23	= world*view*proj matrix
;	c24	= light direction in object coords
;	c25	= diffuse conditioning color
;	c26	= ambient conditioning color
;	c27	= clamping color
;	c28	= diffuse color
;	c29	= ambient color
;------------------------------------------------------------------------------

	; get global normal for hemisphere conditioning
	dp3	r1,v0,v0	; golf ball center is at origin
	rsq	r0,r1		; optimization: put golf ball in unit coords to avoid square root
	mul	r1,v0,r0	; rescale to unit normal

	; directional light, global hemisphere conditioning
	;  Put dimples on "dark side of the moon" in shadow.
	mov	r3,c25
	dp3	r2,r1,c24	; hemisphere lighting
	max	r2,r2,c27	; clamp
	mad	r2,r3,r2,c26	; scale and bias

	; directional light, local for dimples
	mul	r3,c28,r2	; dimple diffuse color modulated by global hemisphere conditioning
	dp3	r0,v1,c24
	mad	oD0,r3,r0,c29

	; vertex->screen
	dp4	oPos.x,v0,c20
	dp4	oPos.y,v0,c21
	dp4	oPos.z,v0,c22
	dp4	oPos.w,v0,c23


;------------------------------------------------------------------------------
; Tree vertex shader
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;	v0	= verts
;	v1	= normals
;	v2	= texture coords
;
;inputs
;	c50-c53	= world*view*proj matrix
;	c54	= center of tree shadow
;	c55	= color_offset
;	c56	= color_scale
;------------------------------------------------------------------------------

	; vertex->screen
	dp4	oPos.x,v0,c50
	dp4	oPos.y,v0,c51
	dp4	oPos.z,v0,c52
	dp4	oPos.w,v0,c53

	; scale color based on distance from center of shadow
	sub	r1,v0,c54	; vector from current point to center of shadow
	mov	r0,c55		; min color
	dp3	r2,r1,r1	; get squared distance from center of shadow
	mad	oD0,r2,c56,r0	; color = color_scale * r2 + color_offset

	; pass texture coords through
	mov	oT0,v2

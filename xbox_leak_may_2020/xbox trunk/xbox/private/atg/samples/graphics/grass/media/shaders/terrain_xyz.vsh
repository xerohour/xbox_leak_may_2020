;------------------------------------------------------------------------------
; Terrain vertex shader
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
vs.1.0
;------------------------------------------------------------------------------
; Constants specified by the app
;   c1      = shellOffset
;   c2      = texture scaling
;   c3      = color for debugging
;   c4-c7   = matWorldViewProjection
;   c8	    = shell fraction (1, 0, 1-f, f)
;   c9      = z: 1-maxheight
;
; Vertex components (as specified in the vertex DECL)
;   v0      = xyzBase   ; input height field and texture coords
;   v1      = xyzTip    ; offset shell for tops of grass
;------------------------------------------------------------------------------
	mul	r0, v0, c8.zzzz		; base position
	mad	r0, v1, c8.wwww, r0	; lerp
	m4x4	oPos, r0, c4  ; transform position to clipping space
	mul	oT0.xy, c2.xy, r0.xy ; texture coords

	;mov	oD0, c3
	; darken grass near roots
	sub	r1, r0, v0		; current height
	sub	r1, c9.zzzz, r1		; subtract height from max height
	mad	r1, c9.xxxx, r1.zzzz, c9.yyyy ; scale and bias
	mul	oD0, c3, r1.zzzz

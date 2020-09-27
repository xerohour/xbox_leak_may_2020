;------------------------------------------------------------------------------
; Constants specified by the app
;   c0      = {1, 0, 1-fTileLODFraction, fTileLODFraction}
;   c1      = shellOffset
;   c2      = texture scaling
;   c3      = color for debugging
;   c4-c7   = matWorldViewProjection
;
; Vertex components (as specified in the vertex DECL)
;   v0      = uhv1   ; input height field and texture coords
;   v1      = uhv1   ; next mip level up
;
; We use the second coord for the height so that when
; pixel data is used as input, the height is encoded in the
; green channel.  The green channel is often given more bits when compressed.
;------------------------------------------------------------------------------
vs.1.0

; interpolate height-field mipmaps to get position
mul	r1, c0.wwww, v1.xzyw		; f * v1        flip z and y
mad	r0, c0.zzzz, v0.xzyw, r1	; (1-f) * v0    flip z and y
add	r0, r0, c1

m4x4 oPos, r0, c4  ; transform position to clipping space

mul	oT0.xy, c2.xy, r0.xy ; texture coords
mov	oD0, c3

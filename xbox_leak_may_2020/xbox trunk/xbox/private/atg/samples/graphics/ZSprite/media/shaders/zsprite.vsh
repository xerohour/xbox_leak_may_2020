;------------------------------------------------------------------------------
; Vertex shader for ZSprite quad
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;v0 - verts
;v7 - texture coords
;
;inputs
;c[0,3] - world*view*proj matrix
;c4     - Stage 2 texture coords (1, 0, 0)
;c5     - Stage 3 texture coords (0, 0, 1/D3DZ_MAX_D24S8)
;c6     - diffuse color
;------------------------------------------------------------------------------

;vertex->screen
;dp4		oPos.x,v0,c0
;dp4		oPos.y,v0,c1
;dp4		oPos.z,v0,c2
;dp4		oPos.w,v0,c3
mov oPos, v0

;texture coords
mov oT0, v7
mov oT1, v7
mov oT2, c4
mov oT3, c5

;color
mov oD0, c6
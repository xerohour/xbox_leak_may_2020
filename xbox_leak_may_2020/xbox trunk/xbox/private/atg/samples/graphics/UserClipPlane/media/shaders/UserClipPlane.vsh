;------------------------------------------------------------------------------
; Vertex shader for UserClipPlane
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;v0 - verts
;v2 - normal
;
;inputs
;c[0, 3] - world*view*proj matrix
;c[4, 7] - world matrix
;c[8,11] - Clip Plane Matrix
;c13     - Light direction
;c14     - Material color
;TODO - it'd be faster to transform the clip planes and light into model
;       space than to transform each vertex into world space for these calcs
;------------------------------------------------------------------------------

;vertex->screen
m4x4 oPos, v0, c0

;texture coords
;mov oT0, v7

;Transform position into world space
m4x4 r0, v0, c4

;Apply clip plane matrix to get texture coords
m4x4 oT0, r0, c8

;Transform normal into world space
m4x4 r2, v2, c4

;Dot product for lighting
dp3 r3, c13, r2
max r4, r3, c13.w
mul oD0, r4, c14

;------------------------------------------------------------------------------
; Vertex shader to perform vertex blending
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = (0,0,0,0)
;    c1      = (1,1,1,1)
;    c2      = (0,1,2,3)
;    c3      = (4,5,6,7)
;    c4-c7   = matWorld0
;    c12-c15 = matViewProj
;    c20     = light direction
;    c21     = material diffuse color * light diffuse color
;    c22     = material ambient color
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v2    = Normal
;    v9    = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Position
;------------------------------------------------------------------------------

; Transform position for world matrix
dp4 r0.x, v0, c4
dp4 r0.y, v0, c5
dp4 r0.z, v0, c6
dp4 r0.w, v0, c7

; Transform to projection space
dp4 oPos.x, r0, c12 
dp4 oPos.y, r0, c13
dp4 oPos.z, r0, c14
dp4 oPos.w, r0, c15


;------------------------------------------------------------------------------
; Lighting
;------------------------------------------------------------------------------

; Transform normal for world matrix
dp4 r2.x, v2, c4
dp4 r2.y, v2, c5
dp4 r2.z, v2, c6
dp4 r2.w, v2, c7

; Do the lighting calculation
dp3 r1.x, r2, c20    ; r1 = normal dot light
max r1, r1.x, c0     ; if dot < 0 then dot = 0
mul r0, r1.x, c21    ; Multiply with diffuse
add r0, r0, c22      ; Add in ambient
min oD0, r0, c1.x    ; clamp if > 1


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Just copy the texture coordinates
mov oT0,  v9



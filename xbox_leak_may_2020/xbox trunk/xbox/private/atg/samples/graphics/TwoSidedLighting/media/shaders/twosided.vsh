;------------------------------------------------------------------------------
; Vertex shader to perform vertex blending
;------------------------------------------------------------------------------
xvs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = (0,0,0,0)
;    c1      = (1,1,1,1)
;    c4-c7   = matWorld
;    c12-c15 = matViewProj
;    c20     = directional light's direction
;    c21     = frontside material diffuse color * light diffuse color
;    c22     = frontside material ambient color
;    c23     = backside material diffuse color * light diffuse color
;    c24     = backside material ambient color
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Normal
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Position
;------------------------------------------------------------------------------

; Transform to projection space
dp4 oPos.x, v0, c12 
dp4 oPos.y, v0, c13
dp4 oPos.z, v0, c14
dp4 oPos.w, v0, c15


;------------------------------------------------------------------------------
; Lighting calculation
; Note: this is not optimized, for clarity's sake
;------------------------------------------------------------------------------

; Transform normal for world matrix
dp4 r0.x, v3, c4
dp4 r0.y, v3, c5
dp4 r0.z, v3, c6
dp4 r0.w, v3, c7

; Do the frontside lighting calculation
dp3 r1.x, -r0, c20   ; r1 = normal dot light
max r1, r1.x, c0     ; if dot < 0 then dot = 0
add r1, r1, c22      ; Add in ambient
min r1, r1, c1.x     ; clamp if > 1
mul oD0, r1, c21     ; Multiply with diffuse

; Do the backside lighting calculation
dp3 r1.x, r0, c20    ; r1 = normal dot light
max r1, r1.x, c0     ; if dot < 0 then dot = 0
add r1, r1, c24      ; Add in ambient
min r1, r1, c1.x     ; clamp if > 1
mul oB0, r1, c23     ; Multiply with diffuse

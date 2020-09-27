;------------------------------------------------------------------------------
; Vertex shader for the seafloor
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 2, 4 )
;    c8-c11  = matView
;    c12-c15 = matProj
;    c20     = light direction
;    c21     = material diffuse color * light diffuse color
;    c22     = material ambient color
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Normal
;    v6    = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform to view space (world matrix is identity)
m4x4 r9, v0, c8

; Transform to projection space
m4x4 r10, r9, c12

; Store output position
mov oPos, r10


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Transform normal for view matrix (world matrix is identity)
;dp3 r3.x, v3, c8
;dp3 r3.y, v3, c9
;dp3 r3.z, v3, c10
mov r3, v3

; Do the lighting calculation
dp3 r1.x, r3, c20    ; r1 = normal dot light
max r1, r1.x, c0.x   ; if dot < 0 then dot = 0
mul r0, r1.x, c21    ; Multiply with diffuse
mov oD0, c22         ; Ambient contribution
min oD1, r0, c1.x    ; Dir. light contribution


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Copy tex coords
mov oT0.xy, v6

; Gen tex coords from vertex xz position
mul r0.xy, c24.x, v0.xz
add oT1.xy, r0.xy, c24.z


;------------------------------------------------------------------------------
; Fog calculation
;------------------------------------------------------------------------------

; compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
add r0.x, -r9.z, c23.y
mul r0.x, r0.x, c23.z
max r0.x, r0.x, c0.x       ; clamp fog to > 0.0
min oFog.x, r0.x, c1.x     ; clamp fog to < 1.0




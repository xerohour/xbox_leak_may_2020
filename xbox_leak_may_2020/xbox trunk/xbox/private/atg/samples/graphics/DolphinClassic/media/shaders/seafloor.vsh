;------------------------------------------------------------------------------
; Vertex shader for the seafloor
;------------------------------------------------------------------------------
xvs.1.1
#pragma screenspace
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 2, 4 )
;    c8-c11  = matView*matProjection
;    c20     = light direction
;    c21     = material diffuse color * light diffuse color
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Normal
;    v6    = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform position
dp4 oPos.x, v0, c8
dp4 oPos.y, v0, c9
dp4 oPos.z, v0, c10
+ dp4 r10.z, v0, c10
dp4 oPos.w, v0, c11


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Do the lighting calculation
dp3 r1.x, v3, c20    ; r1 = normal dot light
max r1, r1.x, c0.x   ; if dot < 0 then dot = 0
mul r0, r1.x, c21    ; Multiply with diffuse
+ mov oT0.xy, v6     ; Copy base texture coords
min oD0, r0, c1.x    ; Dir. light contribution


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Gen tex coords from vertex xz position
mad oT1.xy, c24.x, v0.xz, c24.z


;------------------------------------------------------------------------------
; Fog calculation
;------------------------------------------------------------------------------

; compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
;screenspace instructions are interwoven to prevent a stall
mul oPos.xyz, r12, c-38
      +   rcc r1.x, r12.w

mad oFog.x, -r10.z, c23.z, c23.y

mad oPos.xyz, r12, r1.x, c-37

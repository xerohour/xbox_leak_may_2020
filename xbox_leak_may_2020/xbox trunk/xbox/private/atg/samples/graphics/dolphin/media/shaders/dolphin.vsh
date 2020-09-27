;------------------------------------------------------------------------------
; Vertex shader to perform tweening of the dolphin mesh
;------------------------------------------------------------------------------
xvs.1.0
#pragma screenspace

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 2, 4 )
;    c2      = ( fWeight1, fWeight2, fWeight3, 0 )
;    c20-c23 = matWorld * matView
;    c24-c27 = matWorld * matView * matProj
;    c40     = light direction
;    c41     = material diffuse color * light diffuse color
;    c42     = material ambient color
;
; Vertex components (as specified in the vertex DECL)
;    v0,v1,v2 = Position (from the 3 streams)
;    v3,v4,v5 = Normal (from the 3 streams)
;    v6       = Texcoords (from the 1st stream only)
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Tween the 3 positions (v0,v1,v2) into one position
mul r3, v0, c2.x
mad r3, v1, c2.y, r3
mad r3, v2, c2.z, r3

; Transform position to view space xz-plane (for texgen, below)
dp4 r9.x, r3, c20
dp4 r9.z, r3, c22

; Transform blended position to projection space
m4x4 oPos, r3, c24


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Tween the 3 normals (v3,v4,v5) into one normal
mul r3, v3, c2.x
mad r3, v4, c2.y, r3
mad r3, v5, c2.z, r3

; Do the lighting calculation
dp3 r1.x, r3, c40    ; r1 = normal dot light
mul oD1, r1.x, c41    ; Multiply with diffuse
mov oD0, c42         ; Ambient contribution


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Copy tex coords for base dolphin texture
mov oT0.xy, v6

; Gen tex coords from vertex xz position for caustic texture
mul oT1.xy, c1.y, r9.xz


;------------------------------------------------------------------------------
; Fog calculation
;------------------------------------------------------------------------------

; Compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
mad oFog.x, -r12.z, c43.z, c43.y

; Screenspace instructions are interwoven to prevent a stall
mul oPos.xyz, r12, c-38
      +   rcc r1.x, r12.w

mad oPos.xyz, r12, r1.x, c-37



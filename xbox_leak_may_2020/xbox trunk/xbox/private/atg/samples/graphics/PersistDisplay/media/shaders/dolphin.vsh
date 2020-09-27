;------------------------------------------------------------------------------
; Vertex shader to perform tweening of the dolphin mesh
;------------------------------------------------------------------------------
xvs.1.1
#pragma screenspace
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 2, 4 )
;    c2      = ( fWeight1, fWeight2, 0, 0 )
;    c4-c7   = matDolphin*matView*matProj
;	 c12-c15 = texgen matrix for T1.
;    c20     = light direction
;    c21     = material diffuse color * light diffuse color
;
; Vertex components (as specified in the vertex DECL)
;    v0,v1   = Position (from the 2 streams)
;    v3,v4   = Normal (from the 2 streams)
;    v6      = Texcoords (from the 1st stream only)
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Tween the 2 positions (v0,v1) into one position
mul r0, v0, c2.x
mad r2, v1, c2.y, r0

; Transform position
dp4 oPos.x, r2, c4
dp4 oPos.y, r2, c5
dp4 oPos.z, r2, c6
+ dp4 r10.z, r2, c6
dp4 oPos.w, r2, c7


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Tween the 2 normals (v3,v4) into one normal
mul r0, v3, c2.x
mad r3, v4, c2.y, r0

; Do the lighting calculation
dp3 r1.x, r3, c20    ; r1 = normal dot light
max r1, r1.x, c0.x   ; if dot < 0 then dot = 0
mul r0, r1.x, c21    ; Multiply with diffuse
+ mov oT0.xy, v6	 ; Copy base texture coords
min oD0, r0, c1.x    ; Dirlight contribution


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Gen tex coords from vertex xz position for caustic texture
dp4 oT1.x, r2, c12
dp4 oT1.y, r2, c14


;------------------------------------------------------------------------------
; Fog calculation
;------------------------------------------------------------------------------

; compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
;screenspace instructions are interwoven to prevent a stall
mul oPos.xyz, r12, c-38
      +   rcc r1.x, r12.w

mad oFog.x, -r10.z, c23.z, c23.y

mad oPos.xyz, r12, r1.x, c-37

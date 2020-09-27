;------------------------------------------------------------------------------
; Vertex shader for the seafloor
;------------------------------------------------------------------------------
xvs.1.0
#pragma screenspace

;------------------------------------------------------------------------------
; Constants specified by the app
;    c30-c33 = matWorld x matView x matProj
;    c40     = light direction
;    c41     = material diffuse color * light diffuse color
;    c42     = material ambient color
;    c43     = fog values
;    c50     = Texture matrix for the shadow texture projection
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Normal
;    v6    = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform vertex position
m4x4 r5, v0, c20


m4x4 oPos, v0, c30


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Do the lighting calculation
dp3 r1.x, v3, c40             ; r1 = normal dot light
mov r2, c42                   ; Load ambient
mad oD0, r1.x, c41, r2        ; Multiply with diffuse
mad oD0.w, r5.z, c44.x, c44.y ; Set alpha factor based on distance


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Copy tex coords
mov oT0.xy, v6

; Gen tex coords from vertex xz position
mul r0.xy, c44.x, v0.xz
add oT1.xy, r0.xy, c44.z

; Project the shadow texture
dp4 r0.x, v0, c50
dp4 r0.y, v0, c51
dp4 r0.z, v0, c52
rcp r0.z, r0.z
mul oT2.xy, r0.xy, r0.z


;------------------------------------------------------------------------------
; Final stuff
;------------------------------------------------------------------------------

; Screenspace instructions are interwoven to prevent a stall
mul oPos.xyz, r12, c-38
+rcc r1.x, r12.w

mad oPos.xyz, r12, r1.x, c-37

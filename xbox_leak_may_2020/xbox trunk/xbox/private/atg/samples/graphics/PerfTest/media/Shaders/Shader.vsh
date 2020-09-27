;------------------------------------------------------------------------------
; Vertex shader for the volumelight scene
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 2, 4 )
;    c4-c7   = matWorld
;    c8-c11  = matView * matWorld
;    c12-c15 = matProj * matView * matWorld
;    c20     = Light position
;    c21     = Light color
;    c22     = Ambient color
;
; Vertex components (as specified in the vertex decl)
;    v0      = Position
;    v2      = Normal
;    v9      = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform to projection space
m4x4 oPos, v0, c12


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------

; Compute the light (vertex to eye) vector
add  r0, c20, -v0     ; Lighting vector, L
dp3  r1.x, r0, r0     ; d^2
rsq  r1.z, r1.x       ; 1 / d
mul  r0, r0, r1.z     ; Normalized L

; For a white, non-attenuated point light with no ambient, use the following:
dp3  r2.x, r0, v2     ; L dot N
max  oD0, r2.x, c0.x  ; max( L dot N )

; For a full, attenuated, colored point light with ambient, use the following:
;dp3  r2.x, r0, v2     ; L dot N
;max  r3, r2.x, c0.x   ; max( L dot N )
;rcp  r1.y, r1.x       ; 1 / d^2
;mul  r3, r3, r1.y     ; Times light attenuation
;mul  r3, r3, c21      ; Times light color
;add  r3, r3, c22      ; Add ambient color
;min  oD0, r3, c1.x    ; Clamp to 1


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Copy base tex coords
mov oT0.xy, v9

; Generate volumelight's texcoords from the vertex position
dp4 oT1.x, v0, c16
dp4 oT1.y, v0, c17
dp4 oT1.z, v0, c18



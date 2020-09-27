;------------------------------------------------------------------------------
; Vertex shader to perform billboarding of sprites
;------------------------------------------------------------------------------
vs.1.0


;------------------------------------------------------------------------------
; Vertex type expected by this shader
;    struct VERTEX
;    {
;       D3DXVECTOR3 p;        // v0.xyz  = Vertex position
;       D3DXVECTOR2 t;        // v3.xy   = Vertex tex coords
;    };
;
; Expected vertex shaders constants
;    c0      = Sprite offset (to position it within the scene)
;    c1      = Sprite color
;    c2      = Sprite scale factors
;    c4-c7   = Transpose of Billboard matrix
;    c12-c15 = Transpose of ViewProj matrix
;
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Scale the vertex
mul r1, v0, c2

; Rotate the sprites with the billboard matrix
m4x4 r0, r1, c4

; Position the sprites 
add r0.xyz, r0.xyz, c0.xyz  ; [r0] += [c0]

; Transform to projection space
m4x4 oPos, r0, c12


;------------------------------------------------------------------------------
; Vertex lighting
;------------------------------------------------------------------------------

mov oD0, c1


;------------------------------------------------------------------------------
; Texture coordinates
;-------------------

mov oT0.xy, v3.xy


;------------------------------------------------------------------------------
; Vertex shader for pointsprites that use volume textures
;------------------------------------------------------------------------------
vs.1.0


;------------------------------------------------------------------------------
; Vertex type expected by this shader
;    struct VERTEX
;    {
;       D3DXVECTOR3 p;        // v0.xyz  = Vertex position
;       DWORD       color;    // v3.xyzw = Vertex color
;    };
;
; Expected vertex shaders constants
;    c8-c11  = Transpose of view matrix
;    c12-c15 = Transpose of transformation matrix
;
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform to projection space
m4x4 oPos, v0, c12


;------------------------------------------------------------------------------
; Pointsprite size
;------------------------------------------------------------------------------

; Scale the pointsprite by the dist to the eye
add r0, v0, -c1
dp3 r0.x, r0, r0
rsq r0.x, r0.x
mul oPts, r0.x, c0.x


;------------------------------------------------------------------------------
; Vertex lighting
;------------------------------------------------------------------------------

mov oD0, v3


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Write out the texture coordinate that becomes the tw for the volume texture
mov oT3.x, c0.z



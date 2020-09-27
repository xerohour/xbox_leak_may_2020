;
; Vertex input registers:
;
; v0            Vertex position
; v1            Base and bump texture coordinates
; v2 - v4       Tangent space transform (orthonormal basis function)
;
; Constant registers:
;
; c0 - c3       Transposed concatenation of world, view, and projection matrices
; c4            Normalized light direction negated in model space
; c5            Eye position in model space
; c6            Ambient light intensity

vs.1.0			; Vertex shader version 1.0

; Transform the vertex

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

; Transform the negated light direction into tangent space

dp3 oT2.x, c4, v2
dp3 oT2.y, c4, v3
dp3 oT2.z, c4, v4

; Calculate a vector from the vertex to the eye and normalize it

sub r0, c5, v0
dp3 r1.w, r0, r0
rsq r1.w, r1.w
mul r1.xyz, r0, r1.w

; Calculate a halfway vector between the vertex-eye vector and the vertex-light vector and normalize it

add r0, r1, c4
dp3 r1.w, r0, r0
rsq r1.w, r1.w
mul r1.xyz, r0, r1.w

; Transform the halfway vector into tangent space

dp3 oT3.x, r1, v2
dp3 oT3.y, r1, v3
dp3 oT3.z, r1, v4

; Propagate base and bump texture coordinates

mov oT0, v1
mov oT1, v1

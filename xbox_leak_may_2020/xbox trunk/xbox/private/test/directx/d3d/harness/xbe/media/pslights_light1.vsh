;
; Vertex input registers:
;
; v0            Vertex position in world space
; v1            Base and bump texture coordinates
; v2            Tangent space transform (Tangent)
; v3            Tangent space transform (Binormal)
; v4            Tangent space transform (Normal)
;
; Constant registers:
;
; c0 - c3       Transposed concatenation of view and projection matrices
; c4            Light position in model space
; c5            Eye position in model space
; c7.x          Light Attenuation0 term (Constant)
; c7.y          Light Attenuation1 term (Linear)
; c7.z          Light Attenuation2 term (Quadratic)
; c8.w          Light range
;

xvs.1.1

; Transform the vertex

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

; Calculate a vector from the vertex to the light and normalize it

sub r0, c4, v0
dp3 r1.w, r0, r0
rsq r1.w, r1.w
mul r1.xyz, r0, r1.w

; Transform the light direction into tangent space
; ##REVIEW## Do the tangent normal and binormal vectors that make up tangent matrix need to be transposed??

dp3 oT2.x, r1, v2
dp3 oT2.y, r1, v3
dp3 oT2.z, r1, v4

; Calculate a vector from the vertex to the eye and normalize it

sub r2, c5, v0
dp3 r3.w, r2, r2
rsq r3.w, r3.w
mul r3.xyz, r2, r3.w

; Calculate a halfway vector between the vertex-eye vector and the vertex-light vector

add r2.xyz, r1.xyz, r3.xyz

; Transform the halfway vector into tangent space

dp3 oT3.x, r2, v2
dp3 oT3.y, r2, v3
dp3 oT3.z, r2, v4

; Propagate base and bump texture coordinates

mov oT0, v1
mov oT1, v1

;
; Vertex input registers:
;
; v0              Vertex position in model space
; v3              Vertex normal in model space
;
; Constant registers:
;
; c15             Side color
; c16             Front color
;
; c40 - c43       Transposed concatenation of world and view matrices
; c44 - c47       Transposed projection matrix
;
; c48             (0.0, 0.0, 1.0, 0.5)
;
; c49			  Light vector in view space
;

xvs.1.1

; Transform the vertex into view space

dp4 r0.x, v0, c40
dp4 r0.y, v0, c41
dp4 r0.z, v0, c42
dp4 r0.w, v0, c43

; Transform the normal into view space

dp3 r1.x, v3, c40
dp3 r1.y, v3, c41
dp3 r1.z, v3, c42

; Transform the vertex into projection space

dp4 oPos.x, r0, c44
dp4 oPos.y, r0, c45
dp4 oPos.z, r0, c46
dp4 oPos.w, r0, c47

; Normalize the vertex normal in view space

dp3 r2.w, r1, r1
rsq r2.w, r2.w
mul r2.xyz, r1, r2.w

; Normalize the vertex position in view space

dp3 r1.w, r0, r0
rsq r1.w, r1.w
mul r1.xyz, r0, r1.w

; Calculate the diffuse color

;dp3 r3.w, r2, c49
dp3 r3.w, v3, c49
mov r4, c16
max r3.w, r3.w, -r3.w
mad oD0, r4, r3.w, c15

; Calculate the reflection vector

dp3 r3.w, r1, r2
add r4.w, r3.w, r3.w
mad r3.xyz, r4.w, r2.xyz, -r1.xyz
add r4.xyz, r3.xyz, c48.xyz
dp3 r3.w, r4, r4
rsq r4.w, r3.w
mul r3.w, r4.w, c48.w
mad oT0.xyz, r3.xyz, r3.w, c48.w

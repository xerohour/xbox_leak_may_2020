;
; Vertex input registers:
;
; v0            Vertex position in model space
; v3            Vertex normal in model space
;
; Constant registers:
;
; c0 - c3       Transposed concatenation of world and view matrices
; c4 - c7       Transposed projection matrix
;
; c8            (0.0, 0.0, 1.0, 0.5)
;

xvs.1.1

; Transform the vertex into view space

dp4 r0.x, v0, c0
dp4 r0.y, v0, c1
dp4 r0.z, v0, c2
dp4 r0.w, v0, c3

; Transform the normal into view space

dp3 r1.x, v3, c0
dp3 r1.y, v3, c1
dp3 r1.z, v3, c2

; Transform the vertex into projection space

dp4 oPos.x, r0, c4
dp4 oPos.y, r0, c5
dp4 oPos.z, r0, c6
dp4 oPos.w, r0, c7

; Normalize the vertex position in view space

dp3 r2.w, r0, r0
rsq r2.w, r2.w
mul r2.xyz, r0, r2.w

; Calculate the reflection vector

dp3 r3.w, r2, r1
add r4.w, r3.w, r3.w
mad r3.xyz, r4.w, r1.xyz, -r2.xyz
add r4.xyz, r3.xyz, c8.xyz
dp3 r3.w, r4, r4
rsq r4.w, r3.w
mul r3.w, r4.w, c8.w
mad oT0.xyz, r3.xyz, r3.w, c8.w

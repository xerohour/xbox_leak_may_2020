; v0     = position
; v3     = normal
; v6     = tex coord
;
; c0     = Light vector
; c5     = Eye point
; c6	 = 1.0f
; c7	 = Material color
; c50-53   = world/view/proj matrix

vs.1.0

; Transform position
dp4 oPos.x, v0, c[50]
dp4 oPos.y, v0, c[51]
dp4 oPos.z, v0, c[52]
dp4 oPos.w, v0, c[53]

; Output the diffuse color
mov	oD0, c[7]

; Output the object-sapce normal as texcoords for stage 0
mov	oT0, v3

; Output the light vector as texcoords for stage 1
mov oT1, c[0]

; Output normalized object-space eye vector in stage 2
add r0, -v0, c[5]

mov r0.x,  r0.x
mov r0.y,  r0.y
mov r0.z,  r0.z


dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul oT2, r0, r0.w
mov oT2.w, c[6]

; Output the object-sapce normal as texcoords for stage 3
mov oT3, v3


xvs.1.1

mov r0.w, c-52.xyzy
mov a0.x, c-52.zyzw
dp4 r0.x, v0, c[6+a0.x]
dp4 r0.y, v0, c[7+a0.x]
dp4 r0.z, v0, c[8+a0.x]

//vertex information
mov a0.x, c-52.zyzw
dp4 r1.x, r0, c[2+a0.x]
dp4 r1.y, r0, c[3+a0.x]
dp4 r1.z, r0, c[4+a0.x]
dp4 r1.w, r0, c[5+a0.x]

dp4 oPos.x, r1, c0
dp4 oPos.y, r1, c1
dp4 oPos.z, r1, c2
dp4 oPos.w, r1, c3

//view space vertex coordinates
dp4 r0.x, v0, c24
dp4 r0.y, v0, c25
dp4 r0.z, v0, c26
dp4 r0.w, v0, c27

//view space normal
dp3 r1.x, v1, c52
dp3 r1.y, v1, c53
dp3 r1.z, v1, c54

///white spot light ambient/diffuse/emissive

mov r2, c77
add r2, r2, c79

//light vector
add r3, c64, -r0
dp3 r4, r3, r3
rsq r5, r4
mul r6, r3, r5

//spot
dp3 r7, r6, c65
add r8, r7, -c79.xxxx
mul r8, r8, c70.zzzz
logp r8, r8
mul r8, r8.zzzz, c70.wwww
expp r8, r8

//spot = 0 if rho < cos(phi/2)
sge r9, r7.zzzz, c70.xxxx
mul r8, r8.zzzz, r9

//spot = 1 if rho > cos(theta/2)
sge r9, r7.zzzz, c70.yyyy
slt r10, r7.zzzz, c70.yyyy
mad r8, r8, r10, r9

//diffuse
dp3 r9, r6, r1
mul r10, r8, r9
mov r0, c-52.xxxx
max r11, r0, r10
mad oD0, r11, c76, r2


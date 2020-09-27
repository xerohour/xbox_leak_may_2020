vs.1.1
mov r0.xz, v0.xyyy
mov r0.yw, c4.xyzw
mov r4, r0
add r4.x, r4.x, c5.z
add r4.z, r4.z, c5.w
dp3 r4.x, r4, r4
rsq r4.x, r4.x
rcp r4.x, r4.x
add r4.x, r4.x, c5.x
mul r4.x, r4.x, c5.y
add  r4.x, r4.x, c7.x
mul  r4.x, r4.x, c7.y
expp r4.y, r4.x
mul  r4.x, r4.y, c7.z
add  r4.x, r4.x,-c7.x
mov r5.x, c4.w
mov r4.x, r4.x
mul r5.y, r4.x, r4.x
mul r4.y, r4.x, r5.y
mul r5.z, r5.y, r5.y
mul r4.z, r4.x, r5.z
mul r5.w, r5.y, r5.z
mul r4.w, r4.x, r5.w
mul r4, r4, c10
dp4 r4.x, r4, c4.w
mul r5, r5, c11
dp4 r5.x, r5, c4.w
mul r0.y, r4.x, c7.w
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3
add r6.x, c4.x, r0.x
mul r6.x, r6.x, c24.z
expp r6.y, r6.x
mul r6.x, r6.y, c24.y
mov a0.x, r6.x
expp r6.y, r6.x
mul r7, c[12+a0.x], r6.yyyy
add r7, r7, c[18+a0.x]
mad r5.x, r5.xxxx, c4.zzzz, c4.zzzz
mul oD0, r5.xxxx, r7.rgba

xvsw.1.1

mov a0.x, -c1.x
add r1, c[a0.x + 1], c[a0.x + 1]

add r2, r1, r1


dp4 oPos.x, v0, c10
dp4 oPos.y, v0, c11 
dp4 oPos.z, v0, c12
dp4 oPos.w, v0, c13

mov oD0, v2

mov oT0, v3
mov oT1, v4
mov oT2, v5


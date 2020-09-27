xvsw.1.1
mov a0.x, v0
mad r1, v0, v0, v0
mad r2, c[0 + a0.x], r1, r1
mov c[5], v1
mov oPos, r2

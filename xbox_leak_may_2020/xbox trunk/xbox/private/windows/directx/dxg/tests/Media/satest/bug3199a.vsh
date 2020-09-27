xvsw.1.1
#pragma screenspace
mov r4,v4
mov a0.x,v0
slt  c-58, c37.zxyw, v5
dp3  r4.xyw, c[a0.x+50], c[a0.x+50].zyxz
min  c-58, v7, c-20.xyxx
mul  oPos, c25.wzxw, r4

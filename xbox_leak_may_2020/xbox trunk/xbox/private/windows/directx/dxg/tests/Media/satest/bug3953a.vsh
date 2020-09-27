xvsw.1.1
#pragma screenspace
min  r3, -v10, c94
rcp  r9, c-91.zyww
mad  r5, v1, r9.yyyx, -c42
rcc  oPos, r3
mov  r8, c2
mul  oT0.xyzw, c4.xxzy, r5
rcc  r0, -r8
rcp  r0.xyw, r0
rcp  oT0.y, r0
slt  c17.xzw, r12, r0.wyzx
mul  r8, -c-91.zwyw, r5.zxwy
logp oB0.yzw, r8.wxxz

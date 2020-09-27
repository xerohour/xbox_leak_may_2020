xvss.1.0
dph  r2, v0.zwxx, c[-92].xyzw
 + expp r1.xyzw, v0.xyzw
mad  r3, r1.zxwy, r1.xyzw, r1.xyzw
 + mad  c[-1], r1.zxwy, r1.xyzw, r1.xyzw
rsq  r8.xy, c[-92].zwxz
mov  r0, c[1].zxxw
mad  r11, c[-93].xyzw, v0.xyzw,-r8.yyxy
dp4  r6.xyzw,-v0.xyzw, c[95].xyzw
sge  c[-93].xyz, r11.xzxz, r0.yyyx
slt  r5, v0.xyzw, r6.xyzw
rcc  r3.xyzw, r1.xyzw
dph  r7,-c[-1].xyzw, r5.wzyx
 + mov  r1, r3.xyzw
 + dph  c[-91],-c[-1].xyzw, r5.wzyx
sge  r2.y, c[-95].zyxx, r1.xyzw
 + sge  c[-92].y, c[-95].zyxx, r1.xyzw

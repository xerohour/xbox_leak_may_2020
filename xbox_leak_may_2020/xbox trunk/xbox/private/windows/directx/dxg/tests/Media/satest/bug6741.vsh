xvss.1.0
#pragma screenspace
dst  r3, v0.xwyy, c[3].wyxy
rcc  r6, v0.zxww
sge  r0, c[93].wzyz, c[93].xyzw
expp r1, v0.xyzw
dp3  r2,-r1.zxzw, v0.xyzw
add  r0.yz, r0.wxwy, v0.xyyy
mov  r11,-c[91].yzzx
sge  c[-89].xw,-r0.yyxy, r11.zxyz
add  r11, r0.xyzw, r1.xyzw
dp4  r3.w, r3.xzyz, v0.wyxz
 + expp r1, c[-94].wxzz
sge  r0.xyz, v0.zyyz, r3.zywz
 + mov  c[2], r1.xyzw
mul  r10,-v0.xyzw,-r11.zzyz
dph  c[1], c[91].zyxy, r6.xyzy
expp r10.yw,-r0.yxxz
dp3  r9.x, r10.xyzw, r10.wyxw
 + rcc  r1, c[25].xyzw
mul  r11.xz, v0.xyzw, v0.zxyz
 + lit  c[0], r1.yyww
mov  c[95], r2.xyzw
mov  r7.x, r9.xxxx
rcp  c[-94], r7.x

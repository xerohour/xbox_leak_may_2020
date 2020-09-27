xvsw.1.1
lit  r8, v2.xyzw
nop  
 + nop  
rcc  r7.xzw,-v3.xxyw
 + rcc  oT3.xzw,-v3.xxyw
nop  
dp4  r2.yw, v9.xyzw, v9.yzzy
 + lit  oT2.x, c[3].xyzw
dst  r6.yz, v15.xyzw, v15.xyzw
expp r1,-v0.xyzw
 + mov  c[4].y, v0.xyzw
nop  
 + nop  
expp oB1.yz, c[-92].xyzw
mov  a0.x  , c[-62].xyzw
 + rcp  r1, v14.xyzw
 + mov  a0.x  , c[-62].xyzw
dst  r5.x, c[0].xyzw, c[0].xyzw
 + dst  c[1].zw, c[0].xyzw, c[0].xyzw
rcc  r6,-c[91].xyzw
 + rcc  oD0.xyzw,-c[91].xyzw
mov  r1, c[1].xyzw
 + nop  
 + mov  oPos, c[1].xyzw
expp oT1,-v1.xyzw
mov  r8.xzw, v0.xyzw
 + lit  r1, v0.wzyw
min  r10.xyw,-v10.xyzw,-c[68].xyzw
 + mov  r1.xyz, v10.xyzw
 + mov  c[2].xyz, v10.xyzw
mov  a0.x  ,-v11.xwzy
rcc  oPos.xy, v12.xyzw
rcc  r9,-c[94].xyzw
max  r11.y,-v6.xyzw, c[-91].xyzw
 + mov  r1.w,-c[-91].xyzw
mul  r6,-v5.yxwy, v5.xzwx
 + rsq  r1.yzw, r1.xyzw
 + rsq  oB1.yzw, r1.xyzw
dp4  r3, v0.yxzw, v0.xyzw
 + nop  
 + nop  
slt  r10.z, r1.zxww, v10.zzwz
dph  r10, r1.yxxz, r1.wxwy
 + expp r1.zw, c[-92].wxzy
 + dph  oT2, r1.yxxz, r1.wxwy
rsq  r10, r6.zyzy
slt  r0,-r10.zyzx, v2.xyzw
mov  r3,-r10.xyzw
dp4  r7.z, v6.zzxz,-r1.xyzw
mul  r6, r10.xyzw, r1.yzzz
max  r5.yz, c[-95].wwxz, c[-95].xyzw
mov  r1, r7.wxzx
 + mov  oB0, r7.wxzx
slt  r9, r10.xyzw, c[1].xyzw
 + rcc  r1.z, v14.wzwy
 + slt  c[3], r10.xyzw, c[1].xyzw
slt  r11.y, r1.yyyy, c[-96].wxxx
 + nop  
dst  r0.xyw, c[3].yzyw, r1.wwzy
dst  r11.x, r8.xzzw, c[1].xyzw
 + dst  oT0.xy, r8.xzzw, c[1].xyzw
lit  r10, v5.zxyy
rcc  r6.xw,-r1.zyyz
dp4  r8, c[1].wyzz, c[1].zxww
rcp  oB1.xyzw, r7.wwwx
dp3  oT1, v14.wwxx, v14.zxwy
dp3  r10, r10.zxxz,-r10.xyzw
nop  
 + mov  r1.w, c[93].zyzx
add  r9, c[-53].yxxz, c[-53].xyzw
rcp  r10.yz, c[83].xwwz
 + rcp  c[91].yz, c[83].xwwz
dp3  r2, c[-95].xyzw, c[-95].xyzw
 + dp3  c[93], c[-95].xyzw, c[-95].xyzw
mov  r4.x, r10.zwzx
 + mov  r1.yzw, v0.xyzw
 + mov  oPos.x, r10.zwzx
lit  r2.yw,-c[2].zxzx
expp r2.xzw, r1.yzzy
rsq  r5, r10.xyzw
max  r6, v4.wyxw, v4.yxxz
 + expp r1.y, v4.zwwy
 + expp oFog.y, v4.zwwy
nop  
 + rsq  r1, c[0].xyzy
 + nop  
lit  r1.xy, v5.xyzw
 + dp3  oFog.z, r7.zwxx,-c[-93].zxxw
max  r7,-c[2].yzzw,-v12.xzxy
 + logp oB0.yzw, c[2].wyww
lit  r6.z, r5.xyzz
min  r10,-v13.xyzw,-c[3].xyzw
 + min  oFog,-v13.xyzw,-c[3].xyzw
rsq  c[-95].x, r6.xyzw
rcc  r5.y, r2.xyzw
rsq  oB0.xyw, c[91].xyzw
dph  r0.z, r1.xyzw, v0.xyzw
 + rcc  r1.yw, c[92].yxyz
 + rcc  c[92].yw, c[92].yxyz
sge  r5,-v12.xyzw, r7.wzxw
nop  
lit  r4.xz, c[0].xyzw
 + lit  oT1.xz, c[0].xyzw
lit  r5.y, v13.xyzw
dst  r7, r6.yzxy, r10.xyzw
sge  r4, r6.xyzw, r10.wyww
 + rcp  oT2.w, v11.xyzw
lit  r4,-v9.wyxw
 + lit  oD1,-v9.wyxw
lit  c[2],-v3.zzwz
mul  r0, c[88].xyzw,-r2.wxyz
mul  r9.zw, r10.xyzw,-v14.zxzy
nop  
 + mov  r1,-r10.xyzw
 + nop  
sge  r0.xyw, r6.xyzw, v14.yxxz
 + lit  r1, v14.xyzw
logp c[4].xy,-c[-94].xyzw
mov  a0.x  , r8.zxzw
slt  r7.y, r2.zzwz, v10.wwwx
 + rcp  oD0, r6.xyzw
rsq  r1, c[-95].xxxy
 + slt  c[-1], r7.zyyw,-r6.xyzw
mov  c[0].w, r0.xyzw
dp4  oD0, r1.xyzw, r5.xyxy
dp4  r9, r0.xyzw, r1.xyzw
 + mov  c[-93].x, r7.wwzz
nop  
 + mov  oT3.xyzw, v2.xyzw
mov  a0.x  , r10.xyzw
mov  a0.x  , c[-1].wwxz
 + mov  a0.x  , c[-1].wwxz
rcc  r1.z, r0.xyzw
 + nop  
logp r4, c[-93].xyzw
rsq  r5.xw, v6.xyzw
sge  r7, r0.wzxx, r6.xzwx
 + logp r1, c[3].zzzy
 + sge  oB0, r0.wzxx, r6.xzwx
lit  r10, c[3].zwwy
max  r8, r10.zzwy, r10.xyzw
dph  r7.w,-c[94].xyzw, r8.zzww
nop  
 + nop  
slt  r5.y,-r2.wxxz, c[92].xyzw
 + slt  c[93].x,-r2.wxxz, c[92].xyzw
lit  r1.w, r2.zxwx
 + mov  oT3, c[-59].ywzx
rcp  c[-92].xw, c[2].xyzw
add  r7.x, c[-94].xyzw, v9.zyxw
dph  c[-95],-c[91].yxxy, c[91].xyzw
sge  r11,-c[94].zwxx, r5.xyzw
 + mov  r1, r2.wwxw
rcp  r1, c[1].xyzw
 + max  c[93], r5.xyzw, r5.xyzw
max  r3.x, v2.xwzx, r1.xyzw
 + rcc  r1,-v2.xyzw
slt  r4.xw, r0.xyzw,-r7.zzxz
 + rsq  r1.xw, r5.xyzw
 + slt  oT3.xw, r0.xyzw,-r7.zzxz
expp r8.yw, r1.xyzw
 + expp oT1.yw, r1.xyzw
lit  r6.yzw, v9.xwwy
rsq  oD0,-r10.xwzw
nop  
add  oT1.xw, r10.xzww, v7.xxyw
dp3  r3.z, c[-92].xyzw, v3.xyzw
 + expp r1, v3.xyzw
 + expp c[-1], v3.xyzw
expp r2,-r0.xyyz
rcc  r9,-r10.xyzw
nop  
 + nop  
logp c[-91], r10.wzzz
expp r4,-v4.xyzw
dp4  r8,-c[3].xyzw, r2.xyzw
 + dp4  oPos,-c[3].xyzw, r2.xyzw
mov  a0.x  , r7.xwzy
dst  r4.x, c[94].xyzw, r7.xwzx
 + lit  r1.yw,-v3.xyzw
mov  a0.x  , c[93].yxyz
 + rsq  r1, v10.xyzw
 + mov  a0.x  , c[93].yxyz
dph  r9, r5.wxww, r10.zwxz
 + mov  r1.xyzw, r6.xwxx
 + mov  oT0, r6.xwxx
mul  r7,-r8.wxxx, r5.xyzw
 + rcp  oT2.xyzw, r7.xyzw
dst  r11.z, r10.wyxy, r0.yyxx
 + logp c[-91].y, r6.yzyw
rcp  r2.xz, v2.xyzz
 + rcp  oFog.xz, v2.xyzz
min  r5, r8.ywyw,-v12.xyzw
nop  
 + logp r1.x, r0.yzxw
 + logp c[-95].x, r0.yzxw
max  r11, r0.ywwz,-v2.wxwz
 + rcp  r1, r4.wzxz
dst  r6, r0.xyzw, v14.xyzw
 + mov  r1.x,-c[4].zyzw
mad  c[4].z, c[-1].xwyw, r10.xyzw, r8.xwzz
nop  
slt  r5, r6.xyyx, c[-96].xyzw
 + lit  r1, r6.zzzy
 + slt  oT2, r6.xyyx, c[-96].xyzw
nop  
 + nop  
expp oT2,-r2.zxxz
min  r6.x, r6.xyzw, r6.yxxy
 + mov  r1,-c[91].wxxx
 + mov  oT3,-c[91].wxxx
max  r4, r9.zwzw, r7.xyzw
 + rsq  r1.xyz, r2.wwyz
slt  r2.xy, r0.xwyx, v5.xyzw
 + rcp  oPts.z, r8.xywz
mov  c[-96].xzw,-r7.wywx
dp4  r0.w, r6.zxzy, v0.xyzw
 + dp4  oD0.w, r6.zxzy, v0.xyzw
slt  r11,-r2.xyzw, v6.wzzy
mov  r6.y, r8.yzwz
 + nop  
mov  a0.x  ,-v0.xzww
mov  r5.yw, v12.xyzw
 + rcp  oD1.xyw, v12.xwzw
mov  r10.x,-c[-95].zxyw
 + mov  oFog.xw,-c[-95].zxyw
nop  
 + nop  
mov  r10.yzw, r9.zzzz
 + mov  oT0.yzw, r9.zzzz
rcp  oT3.y,-r2.xyzw
mul  r3, r1.xyzw, r0.xyzw
min  r8,-c[-95].xyzy,-r1.xyzw
 + rsq  r1,-r10.zzzy
 + min  oB0.xz,-c[-95].xyzy,-r1.xyzw
sge  r0,-c[-92].xyzw, r6.xyzw
 + rsq  r1.yw, r7.xxyy
add  r6.yw,-r2.xwxw, r6.xyzw
 + add  c[1].w,-r2.xwxw, r6.xyzw
nop  
nop  
 + rcp  r1.xy, r1.zwyz
 + rcp  oT1.xy, r1.zwyz
dst  r2, v3.xyzw,-r5.xyzw
min  r8, r1.xyzw,-c[3].xyxw
 + logp c[-95].xz, r2.xxzz
expp r6.xw, r7.xyzw
mad  r0, c[91].xyzw, r5.xyzw, r5.xyzw
 + mad  oFog, c[91].xyzw, r5.xyzw, r5.xyzw
mov  a0.x  ,-r8.xyzw
logp r11.x, r11.xyyy
min  r2, c[2].xyzw, c[2].wwzy
nop  
max  r11,-r7.xyzw,-r6.wxxy
dp4  r2.y, v9.wyyz, r1.xyzw
 + dp4  c[93].y, v9.wyyz, r1.xyzw
rsq  r8.y, r8.xyzw
rcp  r2.zw, r2.xyzw
 + rcp  c[-94].zw, r2.xyzw
expp r8, r10.xyxw
 + expp oPts.xyzw, r10.xyxw
rsq  r1,-r10.zyxx
 + dp3  c[1].xw,-r5.xyzw, r8.wwxx
dp4  r3.x, r8.wxzy,-r0.xyzw
 + rcc  r1.yzw, r7.xyzw
mov  r8.x, r7.xyzw
 + logp r1.yz, v11.zyyz
 + mov  c[3].xz, r7.xyzw
max  r8, c[-95].xyzw, v11.yyyw
 + expp r1.xy, r1.yyxz
 + expp c[-1].xy, r1.yyxz
rsq  oPos.xy, -v7

xvs.1.1
dph  r8, c[-94].wyxx, c[-94].xwyw
 + lit  r8,-c[-94].xwzz
 + dph  r8, c[-94].wyxx, c[-94].xwyw
 + dph  r8.yz, c[-94].wyxx, c[-94].xwyw
 + lit  r8,-c[-94].xwzz
max  r7,-c[2].xyzw, c[2].xzwx
 + max  oD1,-c[2].xyzw, c[2].xzwx
 + rcp  r7,-c[2].wzxz
max  r2.y, c[-93].xzwx, c[-93].zywx
 + max  r2, c[-93].xzwx, c[-93].zywx
mad  r8,-c[-1].xyzw, c[-1].xyzw, c[-1].xyzw
 + expp r8, c[-1].xyzw
 + mad  r8,-c[-1].xyzw, c[-1].xyzw, c[-1].xyzw
 + expp r8, c[-1].xyzw
 + mad  r8,-c[-1].xyzw, c[-1].xyzw, c[-1].xyzw
nop  
logp r4, c[3].zzzy
 + mad  r4, c[3].xyzw,-c[3].xyzw, c[3].zzzy
 + mad  r4.xzw, c[3].xyzw,-c[3].xyzw, c[3].zzzy
 + logp r4, c[3].zzzy
 + logp oFog, c[3].zzzy
 + logp oFog, c[3].zzzy
 + mad  r4, c[3].xyzw,-c[3].xyzw, c[3].zzzy
dp3  oPts, c[0].zxyy, c[0].xyzw
 + rcp  r5, c[0].xyzw
 + dp3  oPts, c[0].zxyy, c[0].xyzw
rcc  oPos.xw, c[1].zxyz
 + rcc  r5, c[1].zxyz
 + min  r5,-c[1].wzyz, c[1].yywy
logp r0, c[a0.x+-91].zwwy
 + max  oT1, c[a0.x+-91].xyzw, c[a0.x+-91].xyzw
dp4  r3.yw, c[-1].xywz, c[-1].xxyx
lit  r4, c[3].xyzw
 + lit  r4, c[3].xyzw
rcp  oB1.w, c[0].xyzw
 + rcp  oB1.w, c[0].xyzw
 + rcp  oB1.w, c[0].xyzw
 + dst  r10.w, c[0].xyzw, c[0].xyzw
 + dst  r10.w, c[0].xyzw, c[0].xyzw
 + rcp  r10.w, c[0].xyzw
 + dst  r10.w, c[0].xyzw, c[0].xyzw
mov  r5.yz,-c[91].xyzw
 + mov  oB1.yz, c[91].xxxx
rcp  r10.xw, c[3].xyzw
 + mad  oT0.xw, c[3].xyzw, c[3].xyxx, c[3].xyzw
slt  r0.xyw, c[94].xxxx, r5.xyxz
 + slt  r0.xw, c[94].xxxx, r5.xyxz
 + rcc  r0.xw, c[94].zzxy
 + rcc  r0.y, c[94].zzxy
 + slt  oPts.y, c[94].xxxx, r5.xyxz
 + slt  r0.w, c[94].xxxx, r5.xyxz
 + rcc  r0.xyw, c[94].zzxy
max  r2.yzw, c[-1].yzwx, c[-1].xyzw
 + max  r2.zw, c[-1].yzwx, c[-1].xyzw
 + expp r2.yzw,-c[-1].wzww
 + max  oD1.z, c[-1].yzwx, c[-1].xyzw
 + max  oD1.w, c[-1].yzwx, c[-1].xyzw
 + max  oD1.y, c[-1].yzwx, c[-1].xyzw
mov  a0.x  ,-c[1].xyzw
 + mov  a0.x  ,-c[1].xyzw
 + mov  a0.x  ,-c[1].xyzw
 + mov  a0.x  ,-c[1].xyzw
expp r3,-c[0].wzyy
 + expp oFog,-c[0].wzyy
mov  a0.x  , c[95].xzzw
 + logp r8, c[95].xyzw
 + logp r8.w, c[95].xyzw
expp oPos, c[92].xyzw
 + dp4  r3, c[92].xyzw, r5.yyzy
 + expp r3, c[92].xyzw
 + expp oPos, c[92].xyzw
 + dp4  r3.xyzw, c[92].xyzw, r5.yyzy
 + dp4  r3, c[92].xyzw, r5.yyzy
mad  r4.w,-c[3].ywyz, c[3].xyzw, c[3].zzyx
 + mad  r4.w,-c[3].ywyz, c[3].xyzw, c[3].zzyx
mov  r0, c[4].xyzw
 + nop  
 + nop  
 + mov  r0, c[4].xyzw
 + nop  
 + nop  
rcp  r2.zw, c[-91].xyzw
 + rcp  oPos.zw, c[-91].xyzw
 + rcp  r2.w, c[-91].xyzw
 + rcp  oPos.zw, c[-91].xyzw
 + dp3  r2.w, c[-91].xyzw, c[-91].zwyx
mov  a0.x  , c[-60].xzww
 + nop  
rcp  oT1.x,-c[a0.x+1].xxzx
 + dp4  r7, c[a0.x+1].zzwx, c[a0.x+1].xyzw
mov  oPts.yw, c[-38].wxwz
 + mov  oPts.yw, c[-38].wxwz
 + mov  oPts.w, c[-38].wxwz
rcp  r5,-c[0].yyzz
 + sge  r5, c[0].wywx, c[0].yxww
 + sge  r5, c[0].wywx, c[0].yxww
logp oT3.xw, r3.xyzw
 + mov  a0.x  ,-c[2].yxyw
 + logp r8.y, r3.xyzw
rcc  r9.yz, c[4].zxzz
 + dst  oD1.y, c[4].yzzy, c[4].xyzw
 + dst  r9.w, c[4].yzzy, c[4].xyzw
add  r7.yw, c[a0.x+-91].xyzw, c[a0.x+-91].xyzy
max  r9.xz, c[56].yyxx,-c[56].wzwy
mov  a0.x  , r8.xyzw
 + mov  a0.x  , r8.xyzw
 + mov  a0.x  , r8.xyzw
 + mov  a0.x  , r8.xyzw
 + mov  a0.x  , r8.xyzw
 + lit  r6, c[94].xyzw
 + mov  a0.x  , r8.xyzw
mad  r8, c[-95].xyzw, c[-95].xxzw, c[-95].zwxx
 + rcc  oB0.xw, c[-95].zwxx
 + rcc  r8, c[-95].zwxx
 + mad  r8, c[-95].xyzw, c[-95].xxzw, c[-95].zwxx
 + rcc  r8.yz, c[-95].zwxx
 + mad  r8, c[-95].xyzw, c[-95].xxzw, c[-95].zwxx
 + rcc  oB0, c[-95].zwxx
rcc  oPos.w, c[-61].xyzw
 + dph  r8.w,-r4.xyzw, r2.zwyz
 + dph  r8.w,-r4.xyzw, r2.zwyz
 + rcc  oPos.w, c[-61].xyzw
 + rcc  oPos.w, c[-61].xyzw
 + rcc  oPos.w, c[-61].xyzw
mov  a0.x  ,-c[95].xyzw
 + mov  a0.x  ,-c[95].xyzw
 + rsq  r3,-r4.zxzz
 + rsq  r3,-r4.zxzz
 + rsq  r3.xw,-r4.zxzz
 + mov  a0.x  ,-c[95].xyzw
 + mov  a0.x  ,-c[95].xyzw
slt  r8.w, r4.yxxy, r9.xyzw
 + rcc  r8.yw, c[-95].yywx
 + rcc  r8.yw, c[-95].yywx
dp4  oPts.xy, c[-92].zwyx, c[-92].wzzx
 + dp4  oPts.xy, c[-92].zwyx, c[-92].wzzx
 + rcp  r5.y,-c[-92].xyxx
 + rcp  r5.x,-c[-92].xyxx
 + rcp  r5.y,-c[-92].xyxx
 + dp4  oPts.xy, c[-92].zwyx, c[-92].wzzx
 + rcp  r5.y,-c[-92].xyxx
dp4  oD0.z, c[-94].yxyx,-c[-94].zwxw
 + dp4  r5.xw, c[-94].yxyx,-c[-94].zwxw
nop  
lit  r10.xy,-c[-92].ywyy
 + dp4  r10.xyw, c[-92].zyxw, c[-92].xyyw
 + dp4  oT2.yw, c[-92].zyxw, c[-92].xyyw
 + dp4  oT2.yw, c[-92].zyxw, c[-92].xyyw
 + dp4  r10.xyw, c[-92].zyxw, c[-92].xyyw
mov  oPts.x, c[-95].xyzw
 + mov  r2.w, c[-95].xyzw
 + mov  oPts.w, c[-95].xyzw
 + mov  oPts.w, c[-95].xyzw
 + mov  oPts.x, c[-95].xyzw
 + mov  r2.xw, c[-95].xyzw
 + mov  a0.x  , c[-95].xzyz
mul  r3.yz, r8.xzyz, r5.xyzw
 + logp r3.yz,-c[-92].xyxy
 + logp r3.yz,-c[-92].xyxy
expp r5, r9.xyzw
 + min  r5, c[a0.x+-1].xyzw, r9.zxxw
logp r0, r9.xyzw
 + dp3  r0,-c[95].wwxz, r8.xyzw
min  r3, c[92].xyzw,-c[92].xyzw
 + rcp  oPos, c[92].xyzw
 + rcp  oPos, c[92].xyzw
 + rcp  r3, c[92].xyzw
 + rcp  oPos, c[92].xyzw
 + min  r3, c[92].xyzw,-c[92].xyzw
mad  r8,-r5.zyxx, c[a0.x+91].xyyz, c[a0.x+91].zzyz
 + rsq  r8, c[a0.x+91].zzyz
 + rsq  r8, c[a0.x+91].zzyz
 + mad  oB1,-r5.zyxx, c[a0.x+91].xyyz, c[a0.x+91].zzyz
 + mad  oB1,-r5.zyxx, c[a0.x+91].xyyz, c[a0.x+91].zzyz
 + rsq  r8, c[a0.x+91].zzyz
 + rsq  r8, c[a0.x+91].zzyz
dph  r6,-c[-93].xyzw, c[-93].wxxw
 + lit  oD0, r5.xyzw
 + lit  oD0, r5.xyzw
 + lit  r6, r5.xyzw
 + dph  r6,-c[-93].xyzw, c[-93].wxxw
sge  r11.xy, c[-91].xyzw, c[-91].xyzw
 + expp r11.z, c[-91].xyzw
 + sge  oT3.z, c[-91].xyzw, c[-91].xyzw
dph  oB1, c[-95].xyzw, c[-95].xyzw
 + dph  oB1.xz, c[-95].xyzw, c[-95].xyzw
 + dph  oB1, c[-95].xyzw, c[-95].xyzw
nop  
 + nop  
 + dph  oD1.xy, c[-92].zwzw, r8.xyzw
 + dph  r6.y, c[-92].zwzw, r8.xyzw
 + dph  r6.xy, c[-92].zwzw, r8.xyzw
 + nop  
mov  r10, c[-92].xzzw
 + mov  oT1.zw, c[-92].zwzx
 + mov  oT1, c[-92].zwzx
 + mov  r10, c[-92].zwzx
 + mov  r10.xyzw, c[-92].zwzx
nop  
 + mad  r5.yw, c[a0.x+95].xyzw,-r2.wzyw,-c[a0.x+95].wxyz
 + nop  
 + nop  
 + mad  r5.yw, c[a0.x+95].xyzw,-r2.wzyw,-c[a0.x+95].wxyz
mov  r6, r9.zzzx
 + logp r6, c[92].xyzw
 + logp r6, c[92].xyzw
 + mov  oT0, r9.zzzx
 + mov  r6, r9.zzzx
 + mov  oT0, r9.zzzx
dst  oPos.z, c[a0.x+-94].xyzw, c[a0.x+-94].xyzw
 + dst  oPos.zw, c[a0.x+-94].xyzw, c[a0.x+-94].xyzw
 + dst  oPos.zw, c[a0.x+-94].xyzw, c[a0.x+-94].xyzw
 + dst  r2.w, c[a0.x+-94].xyzw, c[a0.x+-94].xyzw
logp oFog.xz, c[91].ywzx
 + logp r3.x, c[91].ywzx
 + logp oFog.xz, c[91].ywzx
 + mov  a0.x  , r11.xyxx
 + logp oFog.z, c[91].ywzx
nop  
lit  oB1.xyzw, c[-91].wyxw
 + lit  oB1, c[-91].wyxw
 + lit  oB1, c[-91].wyxw
 + lit  oB1, c[-91].wyxw
 + mov  a0.x  , c[-91].xyzw
 + mov  a0.x  , c[-91].xyzw
 + mov  a0.x  , c[-91].xyzw
rcc  r2.y, c[-1].xyzw
 + dst  r2.y, c[-1].xwyy,-c[-1].zzzy
 + rcc  r2.y, c[-1].xyzw
rsq  r10, r10.xyzw
 + rsq  r10, r10.xyzw
 + mov  r10, c[a0.x+3].wxxw
 + mov  r10, c[a0.x+3].wxxw
 + rsq  oB0, r10.xyzw
sge  oPos.z, c[-91].xyzw,-r4.xyzw
 + nop  
 + nop  
 + sge  r9, c[-91].xyzw,-r4.xyzw
rcc  oT1.yzw,-c[4].zxwz
 + rcc  oT1.w,-c[4].zxwz
 + rcc  r9.z,-c[4].zxwz
 + add  r9.yzw, c[4].xyzw,-c[4].zxwz
 + rcc  oT1.zw,-c[4].zxwz
 + rcc  r9.y,-c[4].zxwz
 + add  r9.w, c[4].xyzw,-c[4].zxwz
rcc  r11,-c[-1].xyzw
 + rcc  r11,-c[-1].xyzw
 + rcc  r11,-c[-1].xyzw
 + rcc  oD0,-c[-1].xyzw
 + mov  a0.x  , c[-1].xyzw
 + mov  a0.x  , c[-1].xyzw
mul  oB0, c[a0.x+0].xyzw, r8.xyzw
 + nop  
 + mul  r0, c[a0.x+0].xyzw, r8.xyzw
rcc  oB1.w, c[-96].xyzw
 + rcc  oB1.y, c[-96].xyzw
rcc  r9,-c[-1].zyzz
 + rcc  r9,-c[-1].zyzz
 + dp4  r9,-r5.xyzw, c[-1].xyzw
 + rcc  r9.xy,-c[-1].zyzz
 + rcc  oT1,-c[-1].zyzz
 + rcc  r9,-c[-1].zyzz
slt  r9, c[-92].xwwx, c[-92].xyzw
 + slt  r9, c[-92].xwwx, c[-92].xyzw
 + slt  r9, c[-92].xwwx, c[-92].xyzw
expp r2, c[3].xyzw
 + min  r2, r4.xyzw, r0.xyzw
 + min  r2, r4.xyzw, r0.xyzw
 + expp r2, c[3].xyzw
rcp  r3, r4.xyzw
 + rcp  r3, r4.xyzw
 + rcp  r3, r4.xyzw
 + rcp  r3, r4.xyzw
logp r8, r10.xxww
 + logp r8, r10.xxww
expp oD0, c[91].xxxw
 + expp r3, c[91].xxxw
min  oT3.z, c[55].yxwz, c[55].yxxy
 + logp r10.zw, c[55].xyzw
 + min  r10.z, c[55].yxwz, c[55].yxxy
 + logp r10.zw, c[55].xyzw
nop  
 + nop  
 + nop  
 + nop  
rcp  r11.x, c[-69].xyzw
 + add  r11.xz, c[-69].xyzw, c[-69].xyzw
mov  r5, c[-91].xyzw
 + mov  r5.yw, c[-91].xyzw
 + mov  r5, c[-91].xyzw
 + sge  r5, c[-91].xyzw, c[-91].wzwz
 + mov  r5, c[-91].xyzw
 + sge  oPts, c[-91].xyzw, c[-91].wzwz
 + mov  r5, c[-91].xyzw
mov  oT2.w, c[0].xyzz
 + mov  oT2.z, c[0].xyzz
 + mov  r11.w, c[0].xyzz
 + mov  oT2.zw, c[0].xyzz
max  oD1, r5.wzyx, c[-93].yyyz
 + max  oD1, r5.wzyx, c[-93].yyyz
 + max  oD1, r5.wzyx, c[-93].yyyz
 + rsq  r3, c[-93].zyyx
 + max  oD1, r5.wzyx, c[-93].yyyz
 + max  oD1, r5.wzyx, c[-93].yyyz
 + max  oD1, r5.wzyx, c[-93].yyyz
sge  r0, r3.xyzw, r8.xyxy
 + sge  r0, r3.xyzw, r8.xyxy
 + sge  oT2, r3.xyzw, r8.xyxy
mov  r5,-c[2].zyzy
 + mad  oB0, r4.xyzw,-r4.xyzw,-c[2].zyzy
 + mad  oB0, r4.xyzw,-r4.xyzw,-c[2].zyzy
 + mad  oB0, r4.xyzw,-r4.xyzw,-c[2].zyzy
 + mad  r5, r4.xyzw,-r4.xyzw,-c[2].zyzy
dp3  oB1,-c[-1].xxxz, c[-1].zxxx
 + dp3  r0,-c[-1].xxxz, c[-1].zxxx
lit  r5.xyz, r3.zxyy
 + lit  r5.xyz, r3.zxyy
dst  r7,-c[2].ywww,-r8.zzxw
 + dst  r7,-c[2].ywww,-r8.zzxw
 + rsq  r7,-r5.xyzw
 + dst  oPts,-c[2].ywww,-r8.zzxw
 + rsq  r7,-r5.xyzw
 + dst  oPts,-c[2].ywww,-r8.zzxw
rcc  r6.xyz, c[1].xyzw
lit  r5.x, r8.xzzx
 + dp3  r5.x,-c[3].xyzw,-r11.zzyy
dph  r5, c[-92].xyzw, c[-92].xyzw
 + dph  oPos, c[-92].xyzw, c[-92].xyzw
 + dph  oPos.xw, c[-92].xyzw, c[-92].xyzw
 + dph  r5, c[-92].xyzw, c[-92].xyzw
 + rcc  r5,-c[-92].yxyy
 + dph  r5, c[-92].xyzw, c[-92].xyzw
 + dph  oPos.xy, c[-92].xyzw, c[-92].xyzw
mad  oT0.x, r7.xywx, c[2].xyzw, c[2].xzwx
 + mad  r10.x, r7.xywx, c[2].xyzw, c[2].xzwx
rcc  r11.w, r0.wwwx
 + rcc  oD1.w, r0.wwwx
 + rcc  oD1.y, r0.wwwx
 + mul  r11.yw, r3.wzzw, c[-95].wwzx
 + rcc  r11.yw, r0.wwwx
 + rcc  oD1.yw, r0.wwwx
lit  r9, r5.xyzw
 + lit  oT2, r5.xyzw
 + lit  r9, r5.xyzw
 + lit  r9, r5.xyzw
 + add  r9.w, c[a0.x+-91].xyzw, r5.xyzw
 + lit  oT2, r5.xyzw
mov  oB0, r8.zxwx
 + mov  r7, r8.zxwx
 + rcc  r7, c[91].xyzw
 + mov  oB0, r8.zxwx
 + rcc  r7, c[91].xyzw
sge  r11,-c[-94].xyzw, c[-94].xyzw
 + sge  oT1,-c[-94].xyzw, c[-94].xyzw
 + sge  r11,-c[-94].xyzw, c[-94].xyzw
 + sge  r11,-c[-94].xyzw, c[-94].xyzw
logp r0, r9.xyzw
 + logp r0, r9.xyzw
 + logp r0, r9.xyzw
mov  oPos.x, c[-95].wwyx
 + mov  oPos.x, c[-95].wwyx
 + rcc  r5.x, r4.xyzw
mov  oD1.zw,-r5.yyxy
 + mov  oD1.x,-r5.yyxy
 + mov  oD1.xw,-r5.yyxy
 + mov  a0.x  , r9.yxyz
 + mov  a0.x  , r9.yxyz
 + mov  r7.xw,-r5.yyxy
 + mov  a0.x  , r9.yxyz
min  r10.x,-c[0].xyzw, c[0].xyzw
 + expp oD0.x, c[0].xyzw
 + expp r10.x, c[0].xyzw
mov  a0.x  , r10.yxyz
mov  oB0.yz, c[0].xyzw
 + mov  oB0.y, c[0].xyzw
 + mov  oB0.z, c[0].xyzw
 + mov  r10.z, c[0].xyzw
 + sge  r10.yz, c[0].xyzw, c[0].wzzz
 + mov  r10.yz, c[0].xyzw
dp4  r8.x, c[91].xyzw,-r0.xyzw
 + expp r8.xz, c[91].wyyw
 + expp oB0.xz, c[91].wyyw
 + expp oB0.xz, c[91].wyyw
expp r8.xyz, c[a0.x+94].xyzw
 + expp r8.y, c[a0.x+94].xyzw
 + nop  
 + expp r8.xyz, c[a0.x+94].xyzw
rcc  oFog.z, r5.xyzw
 + rcc  r2.xz, r5.xyzw
 + mad  r2.xz, c[3].ywwy, c[3].xyzw, r5.xyzw
 + mad  r2.z, c[3].ywwy, c[3].xyzw, r5.xyzw
 + rcc  r2.x, r5.xyzw
 + rcc  r2.xz, r5.xyzw
slt  r9, c[-92].xyzw, r9.xyzw
 + rcc  r9, c[-92].wwzz
 + slt  r9, c[-92].xyzw, r9.xyzw
 + slt  oFog, c[-92].xyzw, r9.xyzw
 + slt  oFog, c[-92].xyzw, r9.xyzw
 + slt  r9, c[-92].xyzw, r9.xyzw
dph  oB0, c[-96].xwyz, r11.xyzw
 + dph  oB0, c[-96].xwyz, r11.xyzw
mov  r0.yzw,-r11.xyzw
mad  r2, r9.zywz,-c[-95].wzzw, r4.zxxy
 + mad  r2, r9.zywz,-c[-95].wzzw, r4.zxxy
 + logp oPts, r4.zxxy
 + mad  r2, r9.zywz,-c[-95].wzzw, r4.zxxy
max  oT0.y, c[0].xyzw, r10.ywyy
 + max  r7.z, c[0].xyzw, r10.ywyy
 + max  r7.z, c[0].xyzw, r10.ywyy
 + max  r7.y, c[0].xyzw, r10.ywyy
 + max  oT0.y, c[0].xyzw, r10.ywyy
 + max  r7.xyz, c[0].xyzw, r10.ywyy
rcp  r9.x,-r4.xwzy
 + rcp  r9,-r4.xwzy
 + nop  
 + nop  
nop  
 + nop  
 + nop  
 + nop  
 + nop  
min  r6.w, r7.xyzw, c[4].yyyz
 + min  oD0.x, r7.xyzw, c[4].yyyz
 + mov  r6.xy, c[4].zzxz
 + min  oD0.yw, r7.xyzw, c[4].yyyz
max  oB0.y, r0.xyzw, c[95].xyzw
 + max  r3.yz, r0.xyzw, c[95].xyzw
 + logp r3.y, r2.yzwz
 + max  oB0.z, r0.xyzw, c[95].xyzw
dph  r3.x, c[a0.x+95].xyzw, r7.xyzw
 + rsq  r3.xw, c[a0.x+95].xwxz
 + dph  r3.x, c[a0.x+95].xyzw, r7.xyzw
 + dph  oFog.xw, c[a0.x+95].xyzw, r7.xyzw
 + dph  r3.w, c[a0.x+95].xyzw, r7.xyzw
 + rsq  r3.xw, c[a0.x+95].xwxz
 + rsq  r3.xw, c[a0.x+95].xwxz
lit  r4.x,-c[1].xyzw
 + dst  r4.x, c[1].xyzw, r0.xyzw
 + dst  oD0.x, c[1].xyzw, r0.xyzw
dp4  r3.yz,-r11.ywxz, r3.xyzw
 + expp r3.yz, r6.zxwx
 + dp4  r3.yz,-r11.ywxz, r3.xyzw
 + expp oT3.yz, r6.zxwx
 + dp4  r3.yz,-r11.ywxz, r3.xyzw
 + expp r3.yz, r6.zxwx
nop  
 + nop  
 + lit  r8, c[a0.x+93].xyzw
 + nop  
 + lit  r8, c[a0.x+93].xyzw
add  oT0.x,-c[-1].zxzx, r8.zzyw
 + add  r5.z,-c[-1].zxzx, r8.zzyw
 + rsq  r5.xyz, r8.zzyw
rsq  r4,-c[a0.x+-1].xyzw
 + max  r4,-r7.yyxy, r4.wxyz
 + rsq  oD0,-c[a0.x+-1].xyzw
 + rsq  r4,-c[a0.x+-1].xyzw
 + rsq  r4,-c[a0.x+-1].xyzw
 + rsq  oD0,-c[a0.x+-1].xyzw
 + max  r4,-r7.yyxy, r4.wxyz
rcc  oT0.y, c[-96].wzyw
 + mov  a0.x  ,-c[-96].xyzw
 + rcc  r2.y, c[-96].wzyw
 + mov  a0.x  ,-c[-96].xyzw
mov  oD1, c[a0.x+93].wzxx
 + mov  r7, c[a0.x+93].wzxx
 + mov  a0.x  , r5.xyzw
 + mov  oD1, c[a0.x+93].wzxx
 + mov  oD1, c[a0.x+93].wzxx
add  r10.w, r8.xyzw,-c[-92].xyzw
 + add  oD1.w, r8.xyzw,-c[-92].xyzw
 + lit  r10,-c[-92].xyzw
min  r10, r4.xyzw, r8.ywwy
 + min  r10, r4.xyzw, r8.ywwy
 + expp oT0,-r8.wzxz
rcc  r6.xw,-r10.xyzw
 + rcc  r6.zw,-r10.xyzw
 + dp4  r6.w, r3.xyzw, c[-1].xyzw
 + dp4  oB1.xzw, r3.xyzw, c[-1].xyzw
 + rcc  r6.w,-r10.xyzw
 + rcc  r6.zw,-r10.xyzw
dp3  r0, r0.xyzw, c[a0.x+95].zwyx
 + dp3  r0, r0.xyzw, c[a0.x+95].zwyx
rcc  r6,-c[91].yzwy
 + rcc  r6,-c[91].yzwy
mad  r3, r5.xyzw, r11.wwyy, c[-91].wzzy
 + logp oPts, c[-91].wzzy
 + logp oPts, c[-91].wzzy
 + mad  r3, r5.xyzw, r11.wwyy, c[-91].wzzy
 + logp r3, c[-91].wzzy
rcc  r5,-c[53].zyxw
 + rcc  r5,-c[53].zyxw
 + mul  oD1,-r8.xyzw, r7.xyzw
rcp  oT3, r8.xyzw
 + rcp  r7, r8.xyzw
dst  oT0,-r11.xyzw, r9.wxwx
 + dst  oT0,-r11.xyzw, r9.wxwx
 + lit  r8, c[-94].xyzw
 + lit  r8, c[-94].xyzw
 + dst  oT0,-r11.xyzw, r9.wxwx
rsq  r10, r6.xywz
 + slt  r10.xyz, r4.xyzw, r10.xyzw
 + rsq  r10.yzw, r6.xywz
mov  r2, c[4].xyzw
 + mov  r2, c[4].xyzw
max  r5.y, r4.xyzw, r5.xyzw
dph  oPts.x, c[93].yxyy, c[93].xzzz
 + dph  oPts.xw, c[93].yxyy, c[93].xzzz
 + dph  r3.xw, c[93].yxyy, c[93].xzzz
 + dph  r3.w, c[93].yxyy, c[93].xzzz
 + logp r3.xw, c[93].zyyy
 + dph  oPts.xw, c[93].yxyy, c[93].xzzz
 + dph  r3.xw, c[93].yxyy, c[93].xzzz
rsq  oB1.z, c[4].xzwz
 + rsq  oB1.z, c[4].xzwz
 + rsq  r0.xzw, c[4].xzwz
max  r10.xzw, c[1].xyzw,-r3.zxww
 + rcc  r10, c[1].wxzz
 + max  r10, c[1].xyzw,-r3.zxww
 + rcc  oB1.yzw, c[1].wxzz
mov  r5, r9.xyzw
 + dp3  r5, r0.zyzz,-r4.zwwz
 + mov  oPts, r9.xyzw
rcc  r7,-r6.xyzw
 + dst  r7,-c[-95].ywzz, r10.zzzx
 + dst  oPts.yz,-c[-95].ywzz, r10.zzzx
dp4  oPts, c[-92].xyzw, r4.xyzw
nop  
 + nop  
 + mov  a0.x  ,-r4.xyzw
 + nop  
 + nop  
mul  r7,-c[92].wzzx,-r0.xyzw
 + rsq  r7,-r5.xyzw
 + rsq  r7,-r5.xyzw
 + mul  oPts.zw,-c[92].wzzx,-r0.xyzw
max  oT3, r0.xyzy, r4.xyzw
 + max  r0, r0.xyzy, r4.xyzw
 + max  oT3, r0.xyzy, r4.xyzw
 + max  oT3, r0.xyzy, r4.xyzw
 + max  r0, r0.xyzy, r4.xyzw
 + mov  r0,-c[-92].yzzy
rcc  r0,-c[95].wwxy
 + mov  a0.x  ,-r11.xyzw
 + mov  a0.x  ,-r11.xyzw
 + rcc  oT0,-c[95].wwxy
 + mov  a0.x  ,-r11.xyzw
sge  oD1, c[-96].xzxx,-c[-96].yzwy
 + sge  r0, c[-96].xzxx,-c[-96].yzwy
 + sge  r0, c[-96].xzxx,-c[-96].yzwy
expp oFog.xw, c[-95].zyzy
 + add  r9.xw, c[-95].xyzw, c[-95].zyzy
 + expp oFog.xw, c[-95].zyzy
rsq  oT2,-r6.yxzz
 + rsq  r6,-r6.yxzz
 + rsq  r6,-r6.yxzz
 + mov  a0.x  , c[92].yxxy
 + rsq  r6,-r6.yxzz
 + mov  a0.x  , c[92].yxxy
logp r7, r10.xyzw
 + mov  a0.x  , c[a0.x+-19].xzxx
 + logp r7.w, r10.xyzw
 + mov  a0.x  , c[a0.x+-19].xzxx
sge  r7, c[a0.x+2].xyww,-c[a0.x+2].wxzw
 + sge  r7, c[a0.x+2].xyww,-c[a0.x+2].wxzw
 + rcc  r7, c[a0.x+2].zzxw
 + sge  r7, c[a0.x+2].xyww,-c[a0.x+2].wxzw
expp r2, c[-96].zwzy
lit  r11.yw,-r6.ywzz
 + lit  r11.y,-r6.ywzz
 + lit  r11.yw,-r6.ywzz
 + lit  r11.yw,-r6.ywzz
 + lit  r11.y,-r6.ywzz
 + lit  r11.w,-r6.ywzz
dst  oPts, r2.zxww, c[-94].xyzw
mov  a0.x  ,-r2.xyzw
 + mov  a0.x  ,-r2.xyzw
mad  r11, c[3].xyzw, c[3].zxwx, r3.xyzw
 + mad  r11, c[3].xyzw, c[3].zxwx, r3.xyzw
 + mad  r11, c[3].xyzw, c[3].zxwx, r3.xyzw
mov  r7, r7.wwzz
 + mov  oT2, r7.wwzz
 + logp r7,-r0.yxxz
 + logp r7,-r0.yxxz
 + mov  r7, r7.wwzz
 + mov  oT2, r7.wwzz
 + mov  oT2.xyzw, r7.wwzz
slt  oT3, c[0].wxyx, r8.xyzw
dp3  r7.xzw, c[-94].yxyy, r2.wyxx
 + dp3  oB1, c[-94].yxyy, r2.wyxx
 + dp3  oB1, c[-94].yxyy, r2.wyxx
 + nop  
 + nop  
 + nop  
 + nop  
dp3  oD0.xzw, r6.yzzw, c[-91].xyzw
 + rcc  r7, c[-91].xyzw
dph  oT2.yw,-r4.xyzw, r7.xyzw
 + dph  oT2.yw,-r4.xyzw, r7.xyzw
 + mov  r9.yw, c[-94].zwyx
 + mov  r9.w, c[-94].zwyx
 + mov  r9.yw, c[-94].zwyx
 + dph  r9.w,-r4.xyzw, r7.xyzw
 + dph  oT2.yw,-r4.xyzw, r7.xyzw
nop  
 + nop  
 + nop  
max  oD1.w, r5.yxyw, c[92].xyzw
 + max  oD1.w, r5.yxyw, c[92].xyzw
 + max  oD1.w, r5.yxyw, c[92].xyzw
 + logp r11.w,-c[92].xyzw
 + max  r11.w, r5.yxyw, c[92].xyzw
 + max  oD1.w, r5.yxyw, c[92].xyzw
 + max  oD1.w, r5.yxyw, c[92].xyzw
mad  oPts, c[a0.x+92].zzww, c[a0.x+92].wzwx,-c[a0.x+92].zzyw
nop  
 + nop  
 + nop  
 + nop  
dp3  r0.x, c[a0.x+4].xyzw, r6.ywyy
 + expp r0.x, r3.xyzw
 + dp3  r0.x, c[a0.x+4].xyzw, r6.ywyy
 + expp r0.x, r3.xyzw
dst  oT0, r5.zwxw,-c[92].xyzw
 + dst  r11, r5.zwxw,-c[92].xyzw
mov  r9, c[-93].xzww
 + min  oPts, r2.yyzx,-r6.xyzw
 + min  r9, r2.yyzx,-r6.xyzw
 + min  r9, r2.yyzx,-r6.xyzw
 + min  oPts, r2.yyzx,-r6.xyzw
 + min  oPts, r2.yyzx,-r6.xyzw
 + min  oPts, r2.yyzx,-r6.xyzw
rcc  r6.x, c[93].xyzw
 + dph  oFog.x, r4.xyzw, c[93].xwxw
 + rcc  r6.x, c[93].xyzw
 + rcc  r6.x, c[93].xyzw
 + dph  r6.x, r4.xyzw, c[93].xwxw
dph  r7, r2.xyzw,-r2.yzxy
 + dph  r7, r2.xyzw,-r2.yzxy
 + mov  r7.xyw, c[4].yyxx
expp oB0.yzw, c[a0.x+95].wwyz
 + expp oB0, c[a0.x+95].wwyz
 + slt  r9,-r0.wzwz, c[a0.x+95].xyzw
 + expp oB0, c[a0.x+95].wwyz
 + slt  r9,-r0.wzwz, c[a0.x+95].xyzw
rcp  oPos.xy, r2.yxyy

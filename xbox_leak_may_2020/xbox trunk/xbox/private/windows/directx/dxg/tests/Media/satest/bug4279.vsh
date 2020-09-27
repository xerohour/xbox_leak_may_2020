xvs.1.0
#pragma screenspace
slt  oT1,-c[-91].xyzw, c[-91].xyzw
dst  oT1, v9.xyzw, c[94].xyzw
expp r5.xy, v6.wyzw
mad  r5, c[91].wwwy, c[91].xyzw, c[91].zyww
 + mad  oT1, c[91].wwwy, c[91].xyzw, c[91].zyww
dph  r4, v2.xzzz, v2.xyzw
mad  r11, v10.xzxw, c[4].yxwx, r5.yxyx
dph  r9.z, c[-92].yywy, v7.xyzw
max  r5.xz,-v2.wzyx, c[-95].ywxw
 + rcc  r1.yzw, c[-95].xyzw
dst  r4, v5.yzwz, c[91].zzyw
 + dst  oB1, v5.yzwz, c[91].zzyw
dp3  r8.y, c[-91].xxxz, c[-91].xyzw
 + rsq  r1.xzw, c[-91].xyzw
sge  r9.y, v9.xwyz, v9.xyzw
 + sge  oB0.w, v9.xwyz, v9.xyzw
rsq  r7, c[3].xyzw
dst  r5.xzw, c[-92].xyzw, v6.xyzw
expp r2.x, c[4].xyzw
logp r5.zw, c[-1].zwxz
 + logp oT2.zw, c[-1].zwxz
rcp  r10.y, c[94].yyxw
rsq  r5.w,-r9.zyzz
 + rsq  oT0.w,-r9.zyzz
mul  r4,-r5.xyzw, v12.zzwz
 + logp r1.y, c[1].zywz
mov  r3.x, r5.zyww
min  r0.yw, v9.xwxx, c[-95].yxyz
 + rcp  oT0,-r1.yzzz
expp r11,-r5.xyzw
 + expp oT2,-r5.xyzw
dp4  r7,-r5.wxyx, r5.xyzw
 + rsq  r1.xy, v15.wwzx
rcp  r10.xz, v2.yzyz
 + rcp  oD1.xz, v2.yzyz
lit  r11,-v10.xzzz
dp4  r10, r5.zzww, r1.xwwx
 + expp r1.xyz, c[1].wzwy
slt  r8.w,-c[91].yywx, r5.xyzw
logp r1.xz,-c[4].xyzw
 + dp3  oD1.w,-v8.xyzw, v8.xyzw
mad  r3.y, v12.wwxy, v12.yzxz, c[91].xyzw
mov  r4.w,-r5.yxzz
 + mov  oPos.w,-r5.yxzz
expp oPos.w, c[2].zyzw
rcp  r1.w, r5.xyzw
 + min  oD1, r5.wxyy, r1.zzyy
min  r3, v3.wyzx, r1.zzzw
 + rcp  r1.zw,-r10.yzxx
mad  oT3.xw, v13.xyzw,-r1.yzwx, v13.xyzw
mad  oB0.y, c[93].xyzw, v6.xyzw,-r1.xyzw
rsq  r1.x, c[4].xyzw
 + min  oPos.y, r1.xyzw, c[4].xyzw
expp r4.zw, c[-95].yxyw
mul  r11, r5.ywwz, r5.wzyz
 + mov  r1.yw,-v4.xyzw
 + mul  oT0, r5.ywwz, r5.wzyz
sge  r7, r5.xzzx, v12.zywx
logp r8, r1.xyzw
 + logp oB1, r1.xyzw
rcc  r6,-r1.xyzw
mad  r3.w,-v13.yzwz, r5.zwyx, v13.ywzz
mul  r7.w, c[-95].zwzz, v0.xyzw
 + mov  r1.xzw, v0.xyzw
mov  r9.xw, v8.wzxy
 + mov  oD1.xw, v8.wzxy
expp r0.y, r1.wyww
 + expp oT1.y, r1.wyww
mov  r9, v4.xyzw
rcp  r3.xyz, c[-95].zxwy
sge  r11, c[2].xxzz, c[2].xyzw
logp r7.xyzw,-c[0].xyzw
dp3  r0.z, v8.wyzy,-r7.xxyw
mov  r10, r5.zwwz
 + rcc  r1.y,-r0.zyyz
 + rcc  oT0.y,-r0.zyyz
max  r3.w, r9.xyzw, r9.xyzw
rsq  r0, c[3].xyzw
dph  r8.zw,-r1.xyzw, r10.zyzy
 + logp oD0,-c[-1].xyzw
rcc  r7.z, v12.xyzw
mov  r0.xw,-v15.xyzw
mad  r9.w, v11.xzxx, v11.xzxz, c[94].xyzw
lit  r1.yzw, c[91].wwxx
 + dp4  oT0.w, c[91].wywx, c[91].wxzx
rcp  r1.xzw, r0.zxzz
 + dp3  oT3.y, r1.wyzx,-r1.xyzw
rcc  r3.y, c[0].xzzw
logp r9.xyz,-c[-96].xyzw
lit  r4.z,-r1.xywx
expp r3.xyzw, r7.wwzw
 + expp oD1.xyzw, r7.wwzw
rcp  oD0.xyz, r5.xyzw
mul  r8, v14.zzwx, r7.wxxw
 + mul  oPts, v14.zzwx, r7.wxxw
rcp  oFog.z,-r9.zzyw
mov  r0, r9.xwyx
 + expp oB1.w,-r9.xyzw
mul  r8.w, c[2].zywx,-r5.zyxz
 + mul  oPos.w, c[2].zywx,-r5.zyxz
mov  r2.z, r5.zxxw
lit  r11, v15.xyzw
 + lit  oD0, v15.xyzw
mov  r1.xyw,-r9.xyzw
 + min  oT0.xz, r5.zyzy, v15.zxwx
dp4  r6.y, r3.yyxy,-c[-50].xyzw
 + lit  oT2, r3.xyzw
logp r5, c[-93].wwwx
 + logp oD1, c[-93].wwwx
sge  r9, r5.wxxy, r1.zwww
mov  r6, c[-1].xyzw
 + mov  oPos, c[-1].xyzw
lit  r2.x, r5.xyzw
max  r9.x, r7.xyzw, r9.wwwz
mov  r7, r5.wwyx
add  r3, v14.xyzw,-r1.wxzx
 + add  oPts, v14.xyzw,-r1.wxzx
max  r9.w, r3.zwxx, c[0].xyzw
 + mov  r1,-r3.xzxy
dst  r2, v14.wxyy, r0.wwxy
 + rsq  r1.w, r1.xxwx
 + rsq  oPos.w, r1.xxwx
dst  r2, v9.xyzw, r5.xyxz
 + expp r1, c[-96].xyww
 + expp oD0, c[-96].xyww
expp r1.xw,-c[94].zwxy
 + dst  oB0, v11.xyzw, c[94].wxyw
mov  r5.z, r7.zyxx
slt  r11.x, r5.xyzw, r1.xyzw
max  oB1, v2.xyzw, r0.zwzz
rcp  oT1,-r7.wyzx
dst  r10.w,-r7.xzzy, r7.wzwz
 + dst  oT3.w,-r7.xzzy, r7.wzwz
add  r0.z, v4.wyzz, r7.xyzw
dp3  r6,-r5.zxyy, r1.ywww
 + rsq  r1.xw,-c[4].xxzy
 + rsq  oT2.xw,-c[4].xxzy
sge  r6.xw, c[-95].yxzy, r1.zzzw
 + rsq  r1.xyw, r5.xyzw
expp r8.xyw, v11.wxxw
min  oFog, r0.xyzw, r1.xyzw
sge  r6,-r7.ywzy, r10.xyzw
 + mov  r1.xw,-v2.xyzw
 + mov  oFog.xw,-v2.xyzw
mul  r4.xz,-c[4].xwzx,-r10.xyzw
 + mul  oPos.xzw,-c[4].xwzx,-r10.xyzw
lit  r3.yz,-v11.wzwz
rcp  oPts, r1.xyzw
dp3  r10.y, v6.xyzw,-r0.xyzw
 + lit  r1.x, r3.xzyw
 + dp3  oT1.y, v6.xyzw,-r0.xyzw
expp r1.z, r7.yzxx
 + sge  oB1,-v3.xyzw, r9.xyzw
lit  r9,-r7.xzww
rsq  r6, c[-1].xyzw
logp r1.xz,-v14.zyyz
 + dst  oFog, r8.wywz, c[-1].xyzw
dph  oT0, c[94].zxwy, r9.xyzy
lit  r7.w, r3.yyyy
mov  r2.xy, r5.xyzw
logp oFog.yw,-c[95].xyzw
dp4  oFog, r1.xyzw, r3.yzzy
dph  r11.xyz, r9.xyzw,-r5.xyzw
 + dph  oPos.xyz, r9.xyzw,-r5.xyzw
rcp  r7, c[94].xyzw
dp4  r0.yw, r0.xyzw, r0.xyzw
 + expp r1, v10.zyww
 + dp4  oFog.zw, r0.xyzw, r0.xyzw
slt  r8, r8.xyzw, r5.zwzx
 + mov  r1.yz, r7.xwzy
 + slt  oT1, r8.xyzw, r5.zwzx
mov  r2.xzw, r3.wywy
 + mov  oFog, r3.wywy
rcc  oPos.xz, c[0].wzwz
expp r3, r9.wwyy
max  r6, v15.xyzw,-r9.xzzz
 + rcp  r1.yzw,-r5.yyzz
rcp  r6.xz, v7.xyzw
mov  r8,-v8.zzzx
lit  r9, c[-94].xzzx
logp r3.w, r0.xyzw
rcp  oPts, r1.xyzw
dp3  r4, r9.xyzw, r3.xyzw
 + mov  oPts,-r3.wxwx
min  r3.x, v1.xyzw, r7.zzyz
 + min  oD1.x, v1.xyzw, r7.zzyz
rsq  r8.z, r9.xyzw
 + rsq  oT2.z, r9.xyzw
rcp  oD1, r9.xyzw
dph  r3,-c[-1].zzxz, r0.xyzw
 + lit  r1.xy, c[-1].xyzw
 + dph  oFog,-c[-1].zzxz, r0.xyzw
rcc  oT3, r4.xxxz
logp r4, r9.xyzw
slt  r3.yw, v15.xyzw,-r3.zwxz
 + mov  oFog.xy, r0.xyzw
mul  r4.xw, r5.ywwx, r6.yyww
 + mul  oT1.x, r5.ywwx, r6.yyww
mad  r2, r10.ywwz, r10.xyzw, r3.xyzw
 + mad  oB0, r10.ywwz, r10.xyzw, r3.xyzw
dst  r7, r10.xyzw, r6.yyyw
 + mov  oB1.xyw, r0.xyyw
dp4  r6.y, r9.wzzz, r7.zyxw
 + rcc  oB1.xyz, r1.wyxw
dp4  r8.yz, v10.xyzw, r7.xyzw
 + rcc  r1.w,-r5.wyyz
rsq  r10.yzw,-v5.xyzw
max  r5, r7.xyzw, r10.zxwx
logp r11.xzw, r1.xyzw
 + logp oT1.xzw, r1.xyzw
dst  r5.w, r4.wxwx, r5.wwwx
 + lit  oB1, c[-93].xyzw
mul  r0, c[3].xyzw, r7.wwxx
 + mul  oPos, c[3].xyzw, r7.wwxx
sge  oD1,-c[4].wzyz, c[4].xyzw
rsq  r11.x, r2.yxzz
expp oPos.xy, r8.xxxz

vs.1.1
mov  a0.x, v6.zyyy
expp r11, c66
slt  oT3, v10, c71
mul  oT1, v10, v10
mul  r8, v0.zyzz, c69
dp3  r3, v12.zzwz, v12
sub  r11.xyzw, c70, v10.xxyy
lit  r11.yzw, r11
slt  r0.xy, c25, r8
lit  r7, c15
sub  r9, c27, v2.zyxx
mad  r8.w, r8.wyww, c[a0.x+11].yzwy, v8.wxxw
add  oPos.zw, r11.zxyx, v12.xwyz
dp4  r4.yzw, v6, v6.yyww
mul  r0.x, c2, c2
dst  r3.z, c74.zxzw, r8.ywzw
min  r10, r8.yzyw, c48.wyzw
dst  oFog, c53, c53
mul  r1, c80, v5.ywxy
mad  r4, v5.zwzy, r10.zwyy, r8
mad  r5.xy, r11, c31.zwww, c31
max  r1.yz, r3.zyyw, v13
min  r6.xyw, c[a0.x+24].wzxz, v13
expp r11.y, r4.ywyw
expp oPts, c[a0.x+60]
logp oD0.z, c50
mov  r11, c76.zzww
mul  oD0.xzw, v0, r4
mul  r11, c6, c6.wyzz
sub  oD0.x, v10.xxwx, c25.yyyw
nop  
max  oT3.w, r9, v11
mov  r0, v4.xyzz
dp4  r11.xyw, r4.zywy, c11
min  r2.xy, c10.yyyx, r1.xyxx
min  r1, r10.wxzy, c70.zyzz
expp oFog.xy, r3
slt  oT3.w, r3, r3.xzzw
mul  r0.x, c64.wyxz, v5.zzwz
mad  r2.xyw, r8.yxxw, r7.xwyz, r4.yzwz
dp3  r9.w, v6.xyzz, r1
dp4  r1, c30.xwyw, v13
sge  r0.yz, v5.ywwx, r6.yxyy
dp4  r2.xz, r0, v10.wyww
dst  oT3.y, c80, r2
mov  r0, r8.ywzz
mad  r11, c57, r10, r9
sge  r0.zw, c51.xwyx, r3.zyww
dst  r10, c[a0.x+5], v12
sub  r3, c87, c87
dp3  r9.xy, c4, c4
add  r11, r11.wywy, v8
sub  r2.yz, r11.wwwy, v11.xxyy
rcp  oT0, r1
nop  
slt  r10.yzw, r10, r2.wzwy
min  r9.xz, r0, r3.yyzw
min  r3.y, c30.zzyx, c30
dp3  r10.xy, r8.wyxz, v12.xwyy
sge  r2, c[a0.x+11], v9.xzzy
rsq  r0, v14.xzxy
mul  oT0.xyz, r2.wyyz, r7
expp r11.yz, v12.wzzz
sge  r1, r0.ywxw, v4
mad  r5.xyz, r0, c57, r9
rsq  r0, r9.wzzw
min  r9, r9.yxyy, v11
lit  r5.xyzw, v8
min  oT0.xzw, r11, r7
dp3  oT1, r3, v13.zzwz
dp4  oPos.xyz, r2.ywzy, c38.yxyy
lit  r0, r2
slt  r10.xw, r9.wwzz, c46.zywx
rsq  oT0.yw, r6.yyyx
rcp  r3, v1.wwzx
mad  oT3.yz, r5, r10.wzyx, r2.yzyx
rsq  oT0.zw, v2.yzyz
nop  
rcp  r9, c51.xyyw
dp4  r1.xz, r4.zxzz, c35
add  r9.xyzw, r0, c80.xywy
add  r6, c66.xzxz, c66
mad  oT2, r1, c33, r9.xxyz
dst  r8.y, r5, v11
nop  
sge  r7, c89.xwxy, c89.xzxy
mul  r10, c95.zwxz, r4.xwww
max  oT2.xw, r0, v11.xzxx
mov  oPos.x, v4.zywy
dst  r7, r3.zzxy, r4
expp r2, r5
dp4  oT1, v12.xyyx, r1.wyzy
nop  
mul  r0, r5, r8.wwww
slt  oT2.xyz, r5.yxxw, c91.yxww
dst  r5.yw, r2, c64
rcp  r10.yw, v5
add  r3, r3.yzwy, v8
logp r3, r11.wxwz
mul  oD0, c48, r9
rsq  r4.yz, r5
dst  r0.zw, v4.xwwx, r5
logp r11, v13
min  oT0, r5, v6.wzwx
add  r10.z, v11, r3
mul  r9.xyzw, v15.yzwz, c63
mov  r3.yz, v7.xzzw
min  oT3.z, v2, r10
lit  r9.xyz, r2.yywz
slt  r8, c74.yxzw, r0
dp3  r1, c79.xyzy, v9
expp r6.x, r0
mov  r4.x, v15
min  oT3, c11, c11.xxzx
mad  r11, r4, c40.wzwy, c40
min  r10.yw, r11, r0
slt  r6.xy, v15, v15.zzxw
lit  r7, c45.zxxw
mov  r8, v7.wyww
slt  r2.w, c4, c4.xyxy
dst  r2, v10, r2
dp3  r5.xy, c72, r9.yzwz
lit  r0, c78.zxyz
mad  r9, c67.yyxy, r4, c67
mad  oD1.zw, r10, r0, c3.xyyz
mad  oT1.xyw, r0.xzzw, c33.wywz, r3.yzzx
rcp  r3.xz, v6

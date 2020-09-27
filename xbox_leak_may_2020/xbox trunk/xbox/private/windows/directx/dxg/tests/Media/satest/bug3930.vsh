vs.1.1
#pragma screenspace
min  r0.y, c3, -v6
lit  r11.xy, -c0
max  r5, v3.xzyx, v3
rcp  r0.xzw, -v5
mul  r2, c92, r0.wwyy
rsq  r5.yz, r2
logp r3.x, v4
sub  r11.yz, v2.yyxz, v2.wywx
dp4  r2, v2, -c0
expp r10, c0.xzzx
mul  r3.yzw, -r5, c93
mul  oPos, c0, -r2
max  r0, -r3, -c3.wxww
mov  r7, r2
expp r6.xyz, c94
lit  r11.yw, r6.zxyz
dp4  r7.w, r5, r3.yzwx
add  r4, r11.zwyx, r7
sub  r8, r0, r4
mad  r10.zw, r8, -r7.ywwy, r10
max  r11.xyzw, -r3, r10
mad  r2, r0.yxxx, r2.wwzx, -r3
logp r10, r3
mov  a0.x, v6
rcp  oT0.xyw, c[a0.x+90].wywz
add  r6, -r3, c2
logp oFog, r10.ywwy
mul  r4.xyzw, r6.zzxy, c3
rcp  r1, r7
mul  r5, v11, -r1
lit  r9, v10
lit  r8, r4.yxwz
mad  r1, r10.wyzw, r9, r5
expp r6, r8
expp oT0.xw, -r11.zxzz
expp oD1, r11.ywzx
expp oT3, r10.xxzx
dst  r6.xz, r6, v6.xyyz
dp3  r5, c2.wwxw, r2.xzwy
mad  oD0, r6.ywwz, r2.yyzx, c2
max  oPts, r1, r5.yyxz


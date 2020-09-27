xvsw.1.1
mov  r0.yzw, c93
add  r11, c95.wxzw, -v5.xwwx
expp r7, v13.wyzy
rcc  r9, v5
rcp  oT2, -r7.zxwz
sge  r8.xyzw, -v5, -v5.zyzw
sub  r3, r8, -c93.xyzy
rcp  r4, -r8
add  r7.yw, r4.yxww, r8
mov  r8, c-95
mov  c95, -r7
rcc  r4, c-1.xyzy
mul  c4.y, -r3.wwyx, v1.zyzy
rsq  r6, c95
add  oFog, r0.wwyy, r4
logp c25, r4
sub  oD1.w, v9, r9
mov  oT2.z, c91.zwzw
min  r7, c-93, r6
mul  r2, r4, -c-1
dp3  r10, -v1.wwzy, r2
sub  r10.xz, r11.xxyz, r6.wyxy
sge  oT1, r8.ywwz, -v5
sub  r3, -r9, r10.zzxw
mul  r5, c-94.yzzz, -v11.xxyx
mad  r0.xz, c93, -r3.zyxw, -c93.zxxw
dst  c85, -r0.zzwy, r10.wxxy
dp4  c-20.z, -r9, -r5.zzzy
min  c95, c-94.ywyw, r2.zyxy
expp r2.yz, -r7.wyyw
sub  oPos, r4, -r2

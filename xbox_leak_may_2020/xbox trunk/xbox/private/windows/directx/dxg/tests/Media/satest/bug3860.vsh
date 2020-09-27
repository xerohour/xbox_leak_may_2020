xvss.1.1
#pragma screenspace
dph  r9, v0, v0
add  r0, v0, r9.yyxz
dst  r11, -c54, -c54.xyxz
mad  r1.xyzw, c-95, r9, v0
sge  r10, r11.zywy, r11
mad  r7, c93, -c93.xxwz, r10.zwyy
dph  r0.yz, -r0.ywzw, -r1
logp r1, r10
lit  r0.xyw, r7
dp3  c-16, r1.wxyx, -r1
dst  c-61.w, r0.ywzy, v0
mul  c26, v0.zxzy, c92


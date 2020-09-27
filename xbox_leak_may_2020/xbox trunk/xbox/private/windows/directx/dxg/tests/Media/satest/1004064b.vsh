vs.1.1
#pragma screenspace
mov  r4, c3
mov  r10, c5
min  r6, r4.xzzz, -c91
mov  r11, r6.wwzx
rcp  r7.xyz, r11
dst  oT2, c0.xzxx, r4.wwwz
expp oPos, -r10.wyyw
mad  r6, r4.xwyy, -r4.xxzz, -c3.yyzx
mul  r6.x, -r7, -v7
mad  oD1, r4.wyzx, r6, r6
add  oPts.z, r6, r11

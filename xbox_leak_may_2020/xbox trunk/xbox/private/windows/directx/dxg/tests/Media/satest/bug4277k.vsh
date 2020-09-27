xvs.1.1
rcc  r1, c[-96].xyzw
dp3  r0, v1.yxyx, c[95].xyzw
max  r8, c[94].wwwx, v0.xyzw
lit  oPos,-r0.yxww
max  r0, v14.xzzy, r1.xyzw
 + mov  r1, r0.xxyw
dp3  r9,-c[94].zwwy,-c[94].yyxz
 + rcp  r1.z,-c[94].yzxw
mad  oT3.xyz,-r0.ywyy, r8.xyzw, r1.wxzw

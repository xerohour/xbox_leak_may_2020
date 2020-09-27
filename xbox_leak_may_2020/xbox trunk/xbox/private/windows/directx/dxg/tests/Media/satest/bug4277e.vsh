xvs.1.1
min  r5, c[-92].xyzw, c[-92].zyxw
mov  oPos.xyzw, c[4].wwwx
mad  r7.xyzw, v1.xyzw, c[-92].ywxz, v1.xyww
lit  r9.xyzw,-v2.xyzw
mov  a0.x  , c[0].xyzw
 + rcc  r1,-v13.xyzw
expp r0, c[0].xyzw
slt  r5,-r5.xyzw, r9.xyzw
 + mov  r1.xw, r5.zxzw
mad  oB0,-r7.xwwz, r0.zzwy,-r1.wywx
add  oB0.w, r7.yyzx, r5.wyzw

xvss.1.1
sge  r8.w, v0.ywxw, v0.zxzy
 + mov  r1.xyzw, v0.xyzw
mov  r3,-c[-94].yxww
lit  r2.xyzw, r1.yxzx
sge  r3,-c[-94].xyzw, r2.xyzw
 + mov  r1.xw, r3.xyzw
mad  c[94].xyzw, r3.wwwx, r1.xxzz, r1.yyyy

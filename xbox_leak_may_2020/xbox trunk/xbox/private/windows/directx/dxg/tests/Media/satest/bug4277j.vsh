xvss.1.1
mul  r10.w, v0.xyzw,-v0.wwxy
 + rsq  r1.zw,-c[4].xyzw
max  r10.xy,-v0.zxzy,-c[95].ywzx
 + mov  r1.xw, v0.xyzw
expp r5, v0.xyzw
mov  r2, c[92].zyyw
 + mov  c[94].xyz,-r1.zxxz
add  r0, c[95].zwzy,-v0.xwzz
add  r10.z, v0.xyzw, c[-96].wxyy
mul  r5,-r10.yzwy,-r0.xyzw
 + mov  r1, r5.xyzw
 + mul  c[-93],-r10.yzwy,-r0.xyzw
dp3  r10.w,-c[-94].xyzw,-v0.xyzw
 + logp c[-1].xyzw, r1.wxyz

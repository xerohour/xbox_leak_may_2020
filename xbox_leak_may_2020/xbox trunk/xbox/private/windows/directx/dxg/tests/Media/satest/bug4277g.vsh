xvs.1.1
mul  r3,-c[0].zwwx, c[0].xyzw
rsq  r1.xyw,-v13.xyzw
max  r5, c[-94].wzzw, v15.xwyw
 + rcp  r1.yzw,-v15.xyzw
rsq  r11.xyzw, c[-92].xyzw
logp r7, c[-94].ywww
rcp  r1.xyz, v6.zzwz
 + sge  oPos, r1.xyzw,-r3.yxxz
expp r6.xyzw, c[-95].wyzz
sge  r11.zw,-r7.xyzw,-r7.wyzw
 + mov  r1.w, r11.wwzw
dph  r8.yzw, r1.wywz, r6.yxzw
 + dph  oT0.w, r1.wywz, r6.yxzw
 + mov  r1.w, r11.xyzw
rcp  r10.xyzw, r1.xyzw
 + rcp  oT1, r1.xyzw

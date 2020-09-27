xvss.1.0
rcp  r0.xz,-v0.xyzw
dp3  r10, c[2].xyzw,-c[2].yxxy
 + logp r1.xyzw, v0.xyzw
min  r0.z, r1.xyzw, v0.xyzw
 + mov  r1, v0.xyzw
min  r5.xy, r0.xzzx,-c[-92].xwwy
 + min  c[93], r0.xzzx,-c[-92].xwwy
mov  r1,-c[3].xyzw
 + dp3  c[3].xw,-r1.wwwx, c[3].xyzw

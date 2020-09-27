xvss.1.0
logp r3, c[-1].xyzw
expp r4.yzw, v0.xxyy
max  r4.xz, c[0].xyzw, v0.wwwz
lit  r6.xzw, r4.xyzw
dph  r0, r6.xwzx, r4.xyzw
dph  r4, r0.zyxz, r3.xyzw
 + mov  r1, r4.xyzw
 + dph  c[2], r0.zyxz, r3.xyzw
dp3  r0, r1.zzxz, r1.zyzx
 + dp3  c[-91], r1.zzxz, r1.zyzx

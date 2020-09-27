xvs.1.0
add  r7, c4.yzxz, -v10
min  r8, c95, r7
mad  r10, -r8.wzzy, c95.xyyy, -c95
slt  r1.xyzw, r7, v0.ywwx
lit  oPos, r10.xxyz
dp4  r4, -v14.yyzx, r1.zwyy
mov  r0, c95
mov  oFog, v4
dst  r2, r0.xwyw, v7
rsq  oT0.y, -r2.zzww
dph  oFog.xz, r7, r4

xvsw.1.0
max  r5.yz, c[-1].yxyw,-c[-1].xyzw
 + rsq  r1, v10.xzyz
logp r10.yz, c[-87].xzzy
mov  r10.xw,-c[91].xyzw
dp4  r10.w, r1.xyzw, c[-91].yxxx
 + mov  r1, r10.xyzw
dp4  r3.w, c[-96].xwwx, r1.xyzw
 + dp4  c[0].w, c[-96].xwwx, r1.xyzw
slt  r2,-r10.xyzw, v15.xyzw
 + slt  c[93],-r10.xyzw, v15.xyzw
add  oPos.xy, c-93, -c-93.yyzx

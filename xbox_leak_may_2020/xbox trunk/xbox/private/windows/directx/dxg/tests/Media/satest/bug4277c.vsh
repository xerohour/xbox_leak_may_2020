xvsw.1.1
#pragma screenspace
mov  r9.yz, c[-93].xyzw
 + mov  c[-93], c[-93].xyzw
slt  r9.x, v0.xyzw, c[91].xwww
rsq  r0, c[-95].wzzy
 + rsq  oPos, c[-95].wzzy
rcp  r8.xyw, c[3].wxyy
slt  r9.xw,-r8.wxzx, c[1].xyzw
 + mov  c[-94].xw,-r9.zzxx



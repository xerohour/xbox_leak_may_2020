xvs.1.0
#pragma screenspace
dph  r3, -v10.yyxw, -v10
rcp  r8, v0.yyxy
dph  oT3.yw, c-95, c-95
rcc  r3.yw, r8.xyyz
min  oPos.xy, r8.zxxz, -r3

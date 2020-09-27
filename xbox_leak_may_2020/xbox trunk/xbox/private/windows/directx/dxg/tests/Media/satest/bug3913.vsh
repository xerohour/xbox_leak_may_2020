vs.1.0
#pragma screenspace
sub  r10, c92, -c92
mov  r10, r10
mov  oT2.y, r10
sge  r2, r10, v1.ywyy
max  oPos.xy, r2.xxzy, c2.yzyx

xvs.1.0
#pragma screenspace
lit  r1, v11
rcp  r3, c91
lit  r4.yzw, c3
lit  oPts, c3.xyxw
mul  oD1, r3.wwwz, r1
dst  oPos, v14, -r4
lit  oT3, v0.xxxx

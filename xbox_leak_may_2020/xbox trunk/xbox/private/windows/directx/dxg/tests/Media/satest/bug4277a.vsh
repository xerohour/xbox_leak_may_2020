xvs.1.0
#pragma screenspace
dp3  r8.y, c[-91].xxxz, c[-91].xyzw
 + rsq  r1.xzw, c[-91].xyzw
logp r8, r1.xyzw
mov  r9.xw, v8.wzxy
 + rsq  r1.w, r1.xxwx
rcp  oPts, r1.xyzw
expp oPos.xy, r8.xxxz

xvss.1.0
logp r11, v0
dph  r6, r11.yzxx, -c-91
logp r1.xyz, v0
mad  r4, r6, r1.xzyx, -c91.wyxx
slt  r3, r4, -c0.xxxw
dp3  c0.x, -c-93, r3

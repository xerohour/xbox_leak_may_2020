xvs.1.1
mad r1.yzw, c0.xxyz, c0.xxyz, v1.xxyz
rsq r3, v1
mad r2.yzw, c1.xxyz, c1.xxyz, v2.xxyz
rsq r4, v2
mul oPos.xyz, r2.yzw, r4.yzw
mul oD0.xyz,  r1.yzw, r3.yzw


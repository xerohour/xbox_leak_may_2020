vs.1.0
mov oPos,v0
sge r1.x,v0,c0.wyzw
slt r2.x,v0,c0.wyzw
mad r0.x,c0,r1,v0
frc r3.y,r0.xxzw
mul r4.x,r1,r3.yyzw
mad oPos.x,r2,v0,r4
mov oD0,v1
mov oT0,v2


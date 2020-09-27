xvs.1.1
#pragma screenspace

// v0  -- position
// v3  -- normal
//
// c0-3   -- transpose world/view/proj matrix
// c5-8   -- inverse world/view matrix
// c10-13 -- transpose world/view matrix
// c15    -- front color
// c16    -- sideColor - frontColor

dp4 r2.x, v0, c10
dp4 r2.y, v0, c11
dp3 r0.x, v3, c5
dp4 r2.z, v0, c12
dp3 r0.y, v3, c6
dp3 r0.z, v3, c7
dp3 r2.w, r2, r2
dp4 oPos.w, v0, c3
dp3 r0.w, r0, r0
+   rsq r1.w, r2.w
dp4 oPos.x, v0, c0
mul r2.xyz, r2, r1.w
+   rsq r1.w, r0.w
dp4 oPos.y, v0, c1
mul r0.xyz, r0, r1.w
+   rcc r1.w, r12.w
dp4 oPos.z, v0, c2
mul r5.xyz, r1.w, c-38
dp3 r0.w, r0, r2
+   mov r1, c16
mad oPos.xyz, r12, r5, c-37
max r0.w, r0.w, -r0.w
mad oD0, r1, r0.w, c15

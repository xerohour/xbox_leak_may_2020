xvs.1.1  

#define FINAL_MAT_COL0 c0
#define FINAL_MAT_COL1 c1
#define FINAL_MAT_COL2 c2
#define FINAL_MAT_COL3 c3	

#define LIGHT_POS      c4
#define EYE_POS        c5
#define OBJECT_SCALE   c6
#define LIGHT_ATTEN    c7

#define OBJ_SHADOW_MAT c8

#define VERT_POS	   v0
#define VERT_S		   v1
#define VERT_T		   v2
#define VERT_N	       v3

; Transform and project vertex.
dp4 oPos.x, VERT_POS, FINAL_MAT_COL0
dp4 oPos.y, VERT_POS, FINAL_MAT_COL1
dp4 oPos.z, VERT_POS, FINAL_MAT_COL2
dp4 oPos.w, VERT_POS, FINAL_MAT_COL3

; Transform and project vertex to shadow map space
dp4 oT3.x,VERT_POS,c8
dp4 oT3.y,VERT_POS,c9
dp4 oT3.z,VERT_POS,c10
dp4 r0.w, VERT_POS,c11

;clamp w (q) to 0
slt r1, c0, c0
max r0.w, r0.w, r1.w
mov oT3.w, r0.w
 
; Compute light vector.
mul r8.xyz, VERT_POS.xyz,OBJECT_SCALE
sub r0.xyz, LIGHT_POS.xyz,r8.xyz
dp3 r1.x, r0.xyz,r0.xyz
rsq r0.w, r1.x

; Compute per-vertex light attenuation.
dst r6, r1.xxxx, r0.wwww
dp3 r7.x, r6,LIGHT_ATTEN ; this gives us c0 + c1*d + c2*d*d
rcc oD0.xyz, r7.x		 

; Normalize light vector.
mul r1.xyz, r0.w,r0.xyz

; Transform light vector to tangent space.
dp3 oT1.x,r1.xyz,VERT_S
dp3 oT1.y,r1.xyz,VERT_T
dp3 oT1.z,r1.xyz,VERT_N

; Normalize eye vector
sub r3.xyz, EYE_POS.xyz,r8.xyz
dp3 r4.x, r3.xyz,r3.xyz
rsq r5.x, r4.x
mul r2.xyz, r5.x,r3.xyz

; Compute halfway vector and transform to tangent space
add r0.xyz,r2.xyz,r1.xyz
dp3 oT2.x,r0.xyz,VERT_S
dp3 oT2.y,r0.xyz,VERT_T
dp3 oT2.z,r0.xyz,VERT_N





















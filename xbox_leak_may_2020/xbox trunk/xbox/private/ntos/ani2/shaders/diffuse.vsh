xvs.1.1  

#define CV_FINAL_MAT      0
#define CV_LIGHT_POS      4
#define CV_EYE_POS        5

; Transform point from object space to screen space
m4x4 oPos, v0, c[CV_FINAL_MAT]  

;Normalize light vector 
mov  r1,c[CV_LIGHT_POS]
sub  r0.xyz, r1.xyz, v0.xyz 
dp3  r2.x, r0.xyz, r0.xyz
rsq  r1.x, r2.x 
mul  r2.xyz, r1.x, r0.xyz

;Normalize eye vector 
sub  r3.xyz, c[CV_EYE_POS].xyz, v0.xyz 
dp3  r4.x, r3.xyz, r3.xyz
rsq  r1.x, r4.x 
mul  r3.xyz, r1.x, r3.xyz

;Normalize half-way vector = light vector + eye vector
add r4.xyz,r3.xyz,r2.xyz
dp3 r3.x, r4.xyz, r4.xyz
rsq r1.x, r3.x 
mul r4.xyz, r1.x, r4.xyz

;Transform light vector into tangent space
m3x3 oT0,r2,v2

;Transform halfway vector into tangent space
m3x3 oT1, r4,v2













  




 


 



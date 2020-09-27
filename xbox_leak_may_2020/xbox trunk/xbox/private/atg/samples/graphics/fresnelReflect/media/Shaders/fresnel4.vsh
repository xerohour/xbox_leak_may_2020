;------------------------------------------------------------------------------
; Vertex shader to do Standard stuff.  So we can add lighting
;------------------------------------------------------------------------------
vs.1.0

;----------------------------------------------------------
; Constants:
;
;  c0-c3  = World, view, & projection matrix concatenation
;
;  c4     = Camera look vector
;
;  c5     = Color of Light
;
;  c6,c7  = Some useful constants
;
;  c8     = lighting vector
;
;  c9     = ambient color
;
;----------------------------------------------------------

;----------------------------------------------------------
; Calculate lighting.
; r0.x = c8 DOT v1 where v1 is input vertex normal 
dp3 r0.x, c8, v1

;----------------------------------------------------------
; Clamp it so it doesn't go negative
max r0.x, r0.x, c7.y

;----------------------------------------------------------
; Multiply diffuse input color by result
mov r0.yz,r0.xx
mov r0.w,c5.w
mul r0,r0,c5

;----------------------------------------------------------
; Add in the ambient light
add r0,r0,c9

;----------------------------------------------------------
; Fresnel modulates the alpha based on the dot product
; of the camera look vector and the normal of the vertex.
;----------------------------------------------------------

;----------------------------------------------------------
; r4 = r0 where r0 is input diffuse color with lighting
mov r4, r0

;----------------------------------------------------------
; r0.x = c4 DOT v1 where v1 is input vertex normal 
dp3 r0.x, c4, v1

;----------------------------------------------------------
; r1.x = 1.f - r1.x
; r1.x *= scaling factor (Controls how large the transparent 
;  "fresnel spot" is in reflection
sub r0.x,c6.x,r0.x
mul r0.x,r0.x,c6.z

;----------------------------------------------------------
; Clamp values - c6.w controls how transparent the "fresnel 
;  spot" is in the reflection.  c7.x controls how transparent
;  the rest of the glass is.
min r4.w,r0.x,c7.x
max r4.w,r4.w,c6.w

;----------------------------------------------------------
; Transform position
m4x4 oPos, v0, c0       ; [oPos] = [r0]| c0..c3 |

;----------------------------------------------------------
; oD0 = r4 where oD0 is the output diffuse color
mov oD0, r4





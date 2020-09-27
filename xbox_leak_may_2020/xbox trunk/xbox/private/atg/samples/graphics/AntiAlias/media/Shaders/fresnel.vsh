;------------------------------------------------------------------------------
; Vertex shader to do Fresnel Transparency
; Vertex becomes more transparent the closer to the camera look vector
; (i.e. the fresnel vector) the vertex normal is.  With texture for sphere map
;------------------------------------------------------------------------------
vs.1.0

;----------------------------------------------------------
; Constants:
;
;  c0-c3  = World, view, & projection matrix concatenation
;
;  c4     = Camera look vector
;
;  c5     = Color to force into vertices
;
;  c6,c7  = Some useful constants
;
;  c15-18 = Normal to Sphere Reflection Space Matrix
;
;----------------------------------------------------------

;----------------------------------------------------------
; Fresnel modulates the alpha based on the dot product
; of the camera look vector and the normal of the vertex.
;----------------------------------------------------------

;----------------------------------------------------------
; r0.x = c4 DOT v1 where v1 is input vertex normal 
dp3 r0.x, c4, v1

;----------------------------------------------------------
; r4 = c5 where c5 is the color of the vertices
mov r4, c5

;----------------------------------------------------------
; r0.x = 1.f - r0.x
; r0.x *= scaling factor (Controls how large the transparent 
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

;----------------------------------------------------------
; oT0 = Texture coordinates transformed from Normal

;----------------------------------------------------------
; Insure that w coord of normal is 1.0 or things don't work
mov r5, v1
mov r5.w,c6.x

;----------------------------------------------------------
; Multiply to create sphere mapping into texture
m4x4 oT0, r5, c15


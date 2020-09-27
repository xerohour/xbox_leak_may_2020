;------------------------------------------------------------------------------
; Vertex shader to do Fresnel Transparency
; Vertex becomes more transparent the closer to the camera look vector
; (i.e. the fresnel vector) the vertex normal is.
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
;  c6     = Some useful constants
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
; r4 = c5 where c5 is input diffuse color
mov r4, c5

;----------------------------------------------------------
; r4.w = 1.0 - r0.x
add r0.x,c6.x,-r0.x

;----------------------------------------------------------
; Clamp value so we never go completely transparent
max r4.w,r0.x,c6.w

;----------------------------------------------------------
; Transform position
m4x4 oPos, v0, c0       ; [oPos] = [r0]| c0..c3 |

;----------------------------------------------------------
; oD0 = r4 where oD0 is the output diffuse color
mov oD0, r4




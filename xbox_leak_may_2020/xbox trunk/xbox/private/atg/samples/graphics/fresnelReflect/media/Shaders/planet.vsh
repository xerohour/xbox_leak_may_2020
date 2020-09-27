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
; Transform position
m4x4 oPos, v0, c0       ; [oPos] = [r0]| c0..c3 |

;----------------------------------------------------------
; oD0 = r4 where oD0 is the output diffuse color
mov oD0, r0

;----------------------------------------------------------
; oT0 = v2 where v2 are the texture coords.
mov oT0, v2







;------------------------------------------------------------------------------
; Vertex shader to perform cartoon-style shading
;------------------------------------------------------------------------------
vs.1.0


;------------------------------------------------------------------------------
; Vertex type expected by this shader
;    v0 = Vertex position
;    v2 = Vertex normal
;
; Expected vertex shaders constants
;    c0      = Diffuse color
;    c1      = Light direction
;    c4-c7   = Transpose of transform matrix set
;    c8      = World transform
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform the vertex
m4x4 oPos, v0, c4             ; [oPos] = [v0] * | c4..c7 |


;------------------------------------------------------------------------------
; Vertex lighting
;------------------------------------------------------------------------------

; Set the diffuse color
mov oD0, c0


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Transform the normal into view space
dp3 r0.x, v2, c8  
dp3 r0.y, v2, c9
dp3 r0.z, v2, c10  

; tu = vNormal dot vLight
dp3 oT0.x, r0, c1

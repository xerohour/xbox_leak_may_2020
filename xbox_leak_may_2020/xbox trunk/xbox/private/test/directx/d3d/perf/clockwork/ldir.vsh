;
; Vertex input registers:
;
; v0            Vertex position
; v3            Vertex normal
; v7            Vertex texture coordinates
;
; Constant registers:
;
; c0 - c3       Transposed concatenation of world, view, and projection matrices
;
; c4            Normalized light direction
; c5            View position
; c6.x          Material power
; c6.y          0.0f
; c6.z          1.0f
; c7            Diffuse base color (diffuse light color modulated with diffuse material color)
; c8            Diffuse offset color (ambient room color modulated with ambient material color +
;                                     ambient light color modulated with ambient material color +
;                                     emissive material color)
; c9            Specular base color (specular light color modulated with specular material color)
;

vs.1.0								; Vertex shader version 1.0

; Transform the vertex

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

; Light the vertex with a directional light

dp3 r2.x, v3, -c4                   ; Diffuse intensity = dotproduct(vertex normal, light direction)

mad r0.xyz, r2.x, c7.xyz, c8.xyz    ; Diffuse color = diffuse intensity * diffuse base color + diffuse offset color
mov r0.w, c7.w                      ; Diffuse alpha = diffuse base alpha
min oD0, r0, c6.z                   ; Clamp diffuse colors to 1.0f

mov oT0, v7                         ; Propagate texture coordinates

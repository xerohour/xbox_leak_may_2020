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

; Transform the vertex

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

; Light the vertex with a directional light

dp3 r2.x, v3, -c4                   ; Diffuse intensity = dotproduct(vertex normal, light direction)

add r0, v0, -c5                     ; Calculate a vector from the eye to the vertex
nm3 r0, r0
add r1, r0, c4                      ; Calculate the half-vector
nm3 r1, r1
dp3 r2.y, v3, -r1                   ; Specular intensity = dotproduct(vertex normal, light/eye half-vector)

mov r2.w, c6.x                      ; Material power

lit r3, r2                          ; Light coefficients

mad r0.xyz, r3.y, c7.xyz, c8.xyz    ; Diffuse color = diffuse intensity * diffuse base color + diffuse offset color
mov r0.w, c7.w                      ; Diffuse alpha = diffuse base alpha
min oD0, r0, r3.w                   ; Clamp diffuse colors to 1.0f

mul r0.xyz, r3.z, c9.xyz            ; Specular color = specular intensity * specular base color
mov r0.w, c9.w                      ; Specular alpha = specular base alpha
min oD1, r0, r3.w                   ; Clamp specular colors to 1.0f

mov oT0, v7                         ; Propagate texture coordinates

; Light intensity factored into alpha calculations

;dp3 r2.x, v3, -c4       ; Diffuse intensity = dotproduct(vertex normal, light direction)

;add r0, v0, -c5         ; Calculate a vector from the eye to the vertex
;nm3 r0, r0
;add r1, r0, c4          ; Calculate the half-vector
;nm3 r1, r1
;dp3 r2.y, v3, -r1       ; Specular intensity = dotproduct(vertex normal, light/eye half-vector)

;mov r2.w, c6.x          ; Material power

;lit r3, r2              ; Light coefficients

;mad r0, r3.y, c7, c8    ; Diffuse color = diffuse intensity * diffuse base color + diffuse offset color
;min oD0, r0, r3.w       ; Clamp diffuse colors to 1.0f

;mul r0, r3.z, c9        ; Specular color = specular intensity * specular base color
;min oD1, r0, r3.w       ; Clamp specular colors to 1.0f

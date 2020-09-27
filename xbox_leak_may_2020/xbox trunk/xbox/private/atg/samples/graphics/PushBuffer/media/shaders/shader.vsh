vs.1.0

; Shader declaration
;
; From vertex stream is 0:
;    v0      = Position (float3)
;    v1      = Color (packed byte DWORD)
;
; Constants:
;    c[0..3] = WVP matrix


; Transform vertex position
m4x4	oPos, v0, c[0]


; Copy diffuse color
mov oD0, v1

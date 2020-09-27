; Simple vertex shader to transform geometry
xvs.1.1

; Position
dp4 oPos.x, v0, c8
dp4 oPos.y, v0, c9
dp4 oPos.z, v0, c10
dp4 oPos.w, v0, c11

; Color
mov oD0, v1


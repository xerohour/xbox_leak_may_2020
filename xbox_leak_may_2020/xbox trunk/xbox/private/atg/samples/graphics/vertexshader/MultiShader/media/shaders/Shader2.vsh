; Simple vertex shader to transform geometry
xvs.1.1

; Position
dp4 oPos.x, v0, c4
dp4 oPos.y, v0, c5
dp4 oPos.z, v0, c6
dp4 oPos.w, v0, c7

; Color
mov oD0, v1


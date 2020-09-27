xvs.1.1

;
; v0 = pos
; v1 = texture coords
;
; c0-c4	= 
;
; c5 = local light dir.
; c6 = [0.5, 0.5, 1.0, 0.0]
;

dp4 oPos.x,v0,c0
dp4 oPos.y,v0,c1
dp4 oPos.z,v0,c2
dp4 oPos.w,v0,c3

mov oT0,v1
mov oT1,v1
mov oT2,v1

; Calculate the projection of the light into texture space.
;
; lu = c5.x
; lv = c5.y
; lu^2 lv^2 lu*lv lu lv 1
;

mul r0,c5.xyx,c5.xyy
mov r1,c5

mad oD0.xyz,r0.xyz,c6.xxx,c6.xxx
mad oD1.xyz,r1.xyz,c6.xxw,c6.xxz

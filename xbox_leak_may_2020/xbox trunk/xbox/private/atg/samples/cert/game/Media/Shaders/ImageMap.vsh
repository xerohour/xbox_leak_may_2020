xvs.1.1

;
; v0 = position
; v1 = normal
; v2 = texture co-ordinates
;
; c0-c3 = world * view * projection.
;

; output texture co-ordinates.
mov		oT0,v2

; projection.
dp4		oPos.x,v0,c0
dp4		oPos.y,v0,c1
dp4		oPos.z,v0,c2
dp4		oPos.w,v0,c3

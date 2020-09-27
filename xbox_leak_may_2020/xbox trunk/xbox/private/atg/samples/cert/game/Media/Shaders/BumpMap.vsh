xvs.1.1

;
; v0 = position
; v1 = normal
; v2 = texture co-ordinates
; v3 = basis vector 
; v4 = basis vector
; v5 = basis vector
;
; c0-c3 = world * view * projection.
; c4-c6 = local light transform (3x4)
; c7 = local light position
; c8 = local viewer position
;

; output texture co-ordinates.
mov		oT0,v2

; get light vector
sub		r2,c7,v0			; L (vector to light)

dp3		r2.w,r2,r2			; x^2 + y^2 + z^2

rsq		r1.x,r2.w			; 1 / (x^2 + y^2 + z^2)

mul		r2,r2,r1.x			; normalize L

; move into tangent space
dp3		oT1.x,r2,v3
dp3		oT1.y,r2,v4
dp3		oT1.z,r2,v5

; get light half angle vector
sub		r3,c8,v0			; V (vector to viewer)

dp3		r3.w,r3,r3			; x^2 + y^2 + z^2

rsq		r1.y,r3.w			; 1 / (x^2 + y^2 + z^2)

mul		r3,r3,r1.y			; normalize V

add		r3,r2,r3			; H = L + V

dp3		r3.w,r3,r3			; x^2 + y^2 + z^2

rsq		r1.z,r3.w			; 1 / (x^2 + y^2 + z^2)

mul		r3,r3,r1.z			; normalize H

; move into tangent space
dp3		oT2.x,r3,v3
dp3		oT2.y,r3,v4
dp3		oT2.z,r3,v5

; gen texture co-ordinates for falloff texture.
dp4		oT3.x,v0,c4
dp4		oT3.y,v0,c5
dp4		oT3.z,v0,c6

; projection.
dp4		oPos.x,v0,c0
dp4		oPos.y,v0,c1
dp4		oPos.z,v0,c2
dp4		oPos.w,v0,c3

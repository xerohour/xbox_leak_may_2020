xvs.1.1

;
; v0 = position
;
; c0-c3 = world * view * projection * viewport
; c4    = viewport offset
; c5    = local viewer position.
; c6.x  = distance scale
;

#pragma screenspace

; projection.
dp4 oPos.x,v0,c0
dp4 oPos.y,v0,c1
dp4 oPos.z,v0,c2
dp4 oPos.w,v0,c3

; texture co-ordinate generation.
sub r0,c5,v0
dp3 r0,r0,r0            ; d*d
rsq r1.x,r0.x           ; 1/sqrt(d*d)
mul r0,r0,r1.x          ; sqrt(d*d)
mad oT0.x,r0,c6.x,c6.y  ; scale to range [0..1]

; multiply by reciprocal and offset.
rcc r1.x, r12.w
mad oPos.xyz, r12, r1.x, c4

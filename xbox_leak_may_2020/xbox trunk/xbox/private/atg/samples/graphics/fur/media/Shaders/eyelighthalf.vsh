;------------------------------------------------------------------------------
; Local eye, local light, and local half-vector calculation
;
; INPUT:
;	r0 = object-space vertex position
;
;	c13 = eye position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c14 = light position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c15 = half vector (used only when DIRECTIONAL_LIGHTING is defined)

; OUTPUT:
;	r3 = eye vector, E
;	r4 = light vector, L
;	r5 = half vector, H = (E + L)/2
;   
;------------------------------------------------------------------------------

#ifdef LOCAL_LIGHTING
	; local eye vector  E = E0 - V
	add r1,-r0,c13
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w	
	mul r3,r1,r2.w	; normalize E

	; local light vector L = L0 - V
	add r1,-r0,c14
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w	
	mul r4,r1,r2.w	; normalize L

	; local H vector  H = (E + L) / 2
	add r1,r3,r4
	; no need to multiply by 1/2, since we're normalizing anyway
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w
	mul r5,r1,r2.w	; normalize H
#else
	mov r3,c13	; directional eye
	mov r4,c14	; directional light
	mov r5,c15	; directional half vector
#endif

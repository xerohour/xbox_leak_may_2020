;------------------------------------------------------------------------------
; Hair-lighting vertex shader fragment
; Modified Kajiya-Kay-Banks hair lighting model
;
; INPUT:
;	r0 = object-space vertex position 
;	r6 = object-space hair tangent
;	r7 = object-space tangent plane normal
;
;	c13 = eye position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c14 = light position in object space (or direction, if DIRECTIONAL_LIGHTING is defined)
;	c15 = half vector (used only when DIRECTIONAL_LIGHTING is defined)
;	
;	c16 = diffuse conditioning
;	c17 = ambient conditioning
;	c18 = Banks self shadowing, x=minimum for N dot L, y=-log2(1-p)*h, where p is attenuation and h is height from top of hair
;	
; OUTPUT:
;	oD0 = diffuse conditioning term and Banks' self-shadowing
;	oT1 = hair-lighting texture coords (T.L, T.H)
;
; SIDE-EFFECTS:
;	r3 = local eye vector, E
;	r4 = local light vector, L
;	r5 = local half vector, H = (E + L)/2
;   
;------------------------------------------------------------------------------

#ifdef LOCAL_LIGHTING
	; local eye vector  E = E0 - V
	add r1,-r0,c13
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w;
	mul r3,r1,r2.w

	; local light vector L = L0 - V
	add r1,-r0,c14
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w;
	mul r4,r1,r2.w

	; local H vector  H = (E + L) / 2
	add r1,r3,r4
	; no need to multiply by 1/2, since we're normalizing anyway
	dp3 r1.w,r1,r1
	rsq r2.w,r1.w
	mul r5,r1,r2.w
#else
	mov r3,c13	; directional eye
	mov r4,c14	; directional light
	mov r5,c15	; directional half vector
#endif

	; dot light direction with normal for diffuse conditioning
	dp3 r2,r7,r4		; N dot L
	max r2.w,r2.w,c18.x	; clamp to epsilon
	mov r1,c16			; diffuse color
	mad r1.xyz,r2.www,r1,c17	; Kd * NdotL + Ka, keep diffuse alpha
#ifdef SELF_SHADOWING
	; At this point we have no hair self-shadowing, just N dot L
	; "conditioning".  Now, we multiply by Banks' shadow term
	; based on penetration depth, d = h / NdotL, (1-p)^d, where p
	; is the absorption per unit length. We use 'exp' to compute
	; power of two, (1-p)^d = 2^(log2(1-p)*d).  Since exponent has
	; to be positive for exp, and log2(1-p) < 0, we negate to get
	; 2^(-log(1-p)*d) and then divide.
	rcp r2.w,r2.w			; 1/NdotL
	mul r2.w,r2.w,c18.y		; -log2(1-p)*d, d = h/NdotL
	expp r2,r2.w				; (1-p)^(-d)
	rcp r2.w,r2.z			; (1-p)^d
	mul r1.xyz,r1.xyz,r2.www
#endif
	mov oD0,r1

	; texture coords for hair lighting texture
	dp3 r1.x,r6,r4		; T dot L
	dp3 r1.y,r6,r5		; T dot H
	mul oT1.xy,r1.xy,c12.zz	 ; scale by 1/2 to get into [-1/2, 1/2] range

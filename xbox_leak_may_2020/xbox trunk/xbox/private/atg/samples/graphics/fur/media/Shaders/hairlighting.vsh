;------------------------------------------------------------------------------
; Hair-lighting vertex shader fragment
; Modified Kajiya-Kay-Banks hair lighting model
;
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;
; INPUT:
;	r1 = diffuse conditioning color
;	r4 = light vector
;	r5 = half vector
;	r6 = object-space hair tangent
;	r7 = object-space tangent plane normal
;
;	c17 = ambient conditioning
;	c18 = Banks self shadowing, x=minimum for N dot L, y=-log2(1-p), where p is attenuation and h is height from top of hair
;	
; OUTPUT:
;	oD0 = diffuse conditioning term and Banks' self-shadowing
;	oT1 = hair-lighting texture coords (T.L, T.H)
;
;------------------------------------------------------------------------------

	; dot light direction with normal for diffuse conditioning
	dp3 r2,r7,r4		; N dot L
	max r2.w,r2.w,c18.x	; clamp to epsilon
	mad r1.xyz,r2.www,r1,c17	; Kd * NdotL + Ka, keep diffuse alpha
	
#ifdef SELF_SHADOWING
	; Multiply N.L conditioning term by Banks' shadow term based on
	; penetration depth, d = h/(N.L), where h is the perpendicular distance
	; from the outermost hair down to the current position. Dividing by N.L
	; converts the perpendicular length to a length along the ray to the
	; light. The attenuation term is (1-p)^d, where p is the absorption per
	; unit length. We use 'exp' to compute power of two, (1-p)^d =
	; 2^(log2(1-p)*d).  Since exponent has to be positive for exp, and
	; log2(1-p) < 0, we negate to get 2^(-log(1-p)*d) and then divide.
	add r2.y,c12.y,-r9.x		; h = max height - current height
	rcp r2.w,r2.w			; 1/NdotL
	mul r2.w,r2.w,r2.y		; d = h/NdotL
	mul r2.w,r2.w,c18.y		; -log2(1-p)*d
	expp r2,r2.w			; (1-p)^(-d)
	rcp r2.w,r2.z			; (1-p)^d
	mul r1.xyz,r1.xyz,r2.www
#endif
	mov oD0,r1

	; texture coords for hair lighting texture
	dp3 r1.x,r6,r4		; T dot L
	dp3 r1.y,r6,r5		; T dot H
	mul oT1.xy,r1.xy,c12.zz	 ; scale by 1/2 to get into [-1/2, 1/2] range

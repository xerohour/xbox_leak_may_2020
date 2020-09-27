;------------------------------------------------------------------------------
; Wind vertex shader fragment
;
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;
; INPUTS:
;	r0 = object-space vertex position 
;	r7 = object-space tangent plane normal
;	r8 = object-space tangent plane origin
;	r9 = x:windiness  y:maximum offset from tangent plane origin
;
;	c20 = wind origin x,y,z and w:local wind amplitude
;	c21 = wind up x,y,z and w:fraction of out-of-tangent plane motion
;	c22 = wind left x,y,z
;	
; OUTPUTS:
;	r0 = blown object-space vertex position
;	r6 = blown hair tangent
;------------------------------------------------------------------------------

	add r1.xyz,-r0.xyz,c20.xyz	; local wind vector W = W0 - V
	mul r2.w,r9.x,c20.w		; wind amplitude scaled by per-vertex "windiness"
	dp3 r1.w,r1,r1			; d^2
	mul r1.xyz,r2.wwww,r1.xyz	; wind vector scaled by local amplitude
	rcp r1.w,r1.w			; 1 / d^2
	mul r1.w,r1.w,r1.w		; 1 / d^4
	mul r1.xyz,r1.wwww,r1.xyz	; wind vector scaled by distance attenuation

	dp3 r1.w,r1,r7			; project offset vector along original normal
	mul r1.w,r1.w,c21.w		; fraction out of tangent plane
	mad r1.xyz,r7.xyz,-r1.www,r1.xyz	; restrict motion to be near tangent plane
	add r0.xyz,r0.xyz,r1.xyz		; get offset point

	add r6,r0,-r8			; get offset vector from tangent plane origin
	dp3 r6.w,r6,r6			; get squared length of offset vector
	rsq r6.w,r6.w			; normalize hair tangent
	mul r6,r6,r6.www
	mad r0.xyz,r9.yyy,r6.xyz,r8.xyz		; limit distance of offset point from tangent plane origin

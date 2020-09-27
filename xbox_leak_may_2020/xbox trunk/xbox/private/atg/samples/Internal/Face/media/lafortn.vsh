;------------------------------------------------------------------------------
; Lafortune lighting model vertex shader
;------------------------------------------------------------------------------
vs.1.0

;
; optimizations - 
;    start with eyepos and lightpos in objectspace so we 
;    dont have to do vertex->worldspace and normal->worldspace
;    for every vertex
;
;    interleave calculations so we dont get pipeline stalls
;

;vertex streams
;v0 - vertices
;v1 - normals
;v2 - texture coordinates
;
;inputs
;c[0,3] - world matrix
;c[4,7] - world*view*proj matrix
;c8 = eye pos (worldspace)
;c9 = light pos (worldspace)
;c10 = diffuse color
;c11 = specular color
;
;Lafortune constants
;c12.x = Cxy1, c12.y = Cxy2, c12.z = Cxy3,	c12.w = w1
;c13.x = Cz1,  c13.y = Cz2,  c13.z = Cz3,	c13.w = w2
;c14.x = n1,   c14.y = n2,   c14.z = n3,	c14.w = w3
;
;Zero
;c15 = 0,0,0,0

;register usage
;r0 - u (light-vertex)
;r1 - v (eye-vertex)
;r2 - u dot n, v dot n, v dot u
;r3 - (v dot n)*(u dot n), (v dot u)-(v dot n)*(n dot u)
;r8 - worldspace vertex
;r9 - worldspace normal

	;vertex->screen
	dp4		oPos.x,v0,c4
	dp4		oPos.y,v0,c5
	dp4		oPos.z,v0,c6
	dp4		oPos.w,v0,c7

	;texture
	mov		oT0,v2

	;vertex->worldspace
	dp4		r8.x,v0,c0			
	dp4		r8.y,v0,c1
	dp4		r8.z,v0,c2
	dp4		r8.w,v0,c3

	;normal->worldspace
	dp3		r9.x,v1,c0			
	dp3		r9.y,v1,c1
	dp3		r9.z,v1,c2

;srp this instruction should not be necessary
;    there is a bug in the dp3 error checker that
;    suggests that the w component is needed in
;	 the dp3 instruction
	dp3		r9.w,v1,c3

;
;diffuse calcs
;
	;u = normalize(lightpos-vertex)
	add		r0,c9,-r8			
	dp3		r4.x,r0,r0
	rsq		r4.x,r4.x
	mul		r0,r0,r4.x			;r0 = u

	dp3		r2.x,r0,r9			;r2.x = u dot n
	mul		oD0,c10,r2.x		;oD0 = diffuse color * diffuse scale

;
;LaFortune specular calcs (33 instructions)
;
; srp - further optimization, swizzle so r5 result
;       goes in yzw instead of xyz to eliminate
;       the mov r5.w,r5.x instruction
;
	;v = normalize(eyepos-vertex)
	add		r1,c8,-r8			
	dp3		r4.x,r1,r1
	rsq		r4.x,r4.x
	mul		r1,r1,r4.x			;r1 = v

	;precalculate vn, vu, vn*un, vu-vn*un (un calculated above)
	dp3		r2.y,r1,r9			;r2.y = r1 dot r9 (vn = v dot n)
	dp3		r2.z,r1,r0			;r2.z = r0 dot r1 (vu = v dot u)
	mul		r3,r2.y,r2.x		;r3 = (vn*un), (vn*un), (vn*un)
	add		r4,r2.z,-r3.x		;r4 = (vu-vn*un), (vu-vn*un), (vu-vn*un)

	;lobe Cxy, Cz calculations
	mul		r5,r4,c12			;Cxy(vu-vn*un)
	mul		r6,r3,c13			;Cz(vn*un)
	add		r5,r5,r6			;Cxy(vu-vn*un) + Cz(vn*un)
	sge		r10,r5,c15			;lobe>0 ? 1 : 0

	;lobe ^n calculations
	mov		r5.w,r5.x
	logp		r4,r5				;r4.z = log(lobe1)
	mul		r4.w,c14.x,r4.z		;r4.w = n1*log(lobe1)
	expp		r4,r4				;r4.z = exp(n1*log(lobe1))
	mul		r6.x,r4.z,c12.w		;r6.x = weight*(lobe1^n1)

	mov		r5.w,r5.y
	logp		r4,r5				;r4.z = log(lobe2)
	mul		r4.w,c14.y,r4.z		;r4.w = n2*log(lobe2)
	expp		r4,r4				;r4.z = exp(n2*log(lobe2))
	mul		r6.y,r4.z,c13.w		;r6.y = weight*(lobe2^n2)

	mov		r5.w,r5.z
	logp		r4,r5				;r4.z = log(lobe3)
	mul		r4.w,c14.z,r4.z		;r4.w = n3*log(lobe3)
	expp		r4,r4				;r4.z = exp(n3*log(lobe3))
	mul		r6.z,r4.z,c14.w		;r6.z = weight*(lobe3^n3)

	mul		r5,r6,r10			;only include lf lobe^n is defined

	add		r4.x,r5.x,r5.y		;lobe1+lobe2
	add		r4.x,r4.x,r5.z		;lobe1+lobe2+lobe3
	mul		r4,c11,r4.x			;specular color
;	sge		r2,r2.x,c15			;spec = 0 if diffuse = 0
	mul		oD1,r4,r2.x

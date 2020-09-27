xvs.1.1

;
; v0 = position
; v1 = normals
; v2 = texture coords
; v3 = matrix indices
; v4 = weights
;
; c0-c3 = world*view*projection matrix
; c4 = local space light direction
;
; c5+ = bone matrices
;

; First bone
mov		a0.x, v3.x
mul		r2, v4.x, c[a0.x+0]
mul	 	r3, v4.x, c[a0.x+1]
mul		r4, v4.x, c[a0.x+2]

; Second bone
mov		a0.x, v3.y
mad		r2, v4.y, c[a0.x+0], r2
mad		r3, v4.y, c[a0.x+1], r3
mad		r4, v4.y, c[a0.x+2], r4

; Third bone
mov		a0.x, v3.z
mad		r2, v4.z, c[a0.x+0], r2
mad		r3, v4.z, c[a0.x+1], r3
mad		r4, v4.z, c[a0.x+2], r4

; Transform weighted position
dp4		r5.x, v0, r2
dp4		r5.y, v0, r3
dp4		r5.z, v0, r4

; Rotate weighted normal
dp3		r6.x, v1, r2
dp3		r6.y, v1, r3
dp3		r6.z, v1, r4

; Normalize normal?

; Lighting
dp3		oD0.rgb, r6, c4

; Texture coordinates
mov		oT0, v2

; World, view, projection
dph		oPos.x, r5, c0
dph		oPos.y, r5, c1
dph		oPos.z, r5, c2
dph		oPos.w, r5, c3

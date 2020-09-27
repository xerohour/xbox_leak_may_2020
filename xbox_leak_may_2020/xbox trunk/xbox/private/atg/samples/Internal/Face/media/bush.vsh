;------------------------------------------------------------------------------
; Hair/Fur/Grass Volume texture vertex shader
;------------------------------------------------------------------------------
vs.1.0

; optimizations - 
;    start with eyepos and lightpos in objectspace so we 
;    dont have to do vertex->worldspace and normal->worldspace
;    for every vertex
;

;------------------------------------------------------------------------------
;vertex streams
;v0 - verts
;v1 - normals
;v2 - tex coords
;
;registers
;r0 - final vertex
;
;inputs
;c[0,3] - world*view*proj matrix
;c[4,7] - world matrix
;c12 - x = expansion along normal, w=0.0
;c13 - light direction
;c14 - eye position
;------------------------------------------------------------------------------

	;expand normal
	mul		r0,v1,c12.xxxw
	add		r0,r0,v0

	;vertex->screen
	dp4		oPos.x,r0,c0
	dp4		oPos.y,r0,c1
	dp4		oPos.z,r0,c2
	dp4		oPos.w,r0,c3

	;texture
	mov		oT0,v2

	;directional light

	;normal->worldspace
	dp3		r1.x,v1,c4
	dp3		r1.y,v1,c5
	dp3		r1.z,v1,c6

	;dot light direction with normal 
	;for diffuse illumination
	dp3		oD0,r1,c13

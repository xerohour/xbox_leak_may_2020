xvs.1.1

;simple diffuse vertex shader to test state shaders

;vertex->screen
	dp4		oPos.x,v0,c0
	dp4		oPos.y,v0,c1
	dp4		oPos.z,v0,c2
	dp4		oPos.w,v0,c3

;set color
	mov		oD0, v1


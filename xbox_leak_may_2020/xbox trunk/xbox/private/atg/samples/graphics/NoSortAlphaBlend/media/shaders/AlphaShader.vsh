xvs.1.1

;
; v0 = position
;
; c0-c3 = world * view * projection.
; c4 = texture scale
; c5 = texture offset
;

; projection
dp4 oPos.x,v0,c0
+	dp4 r0.x,v0,c0
dp4 oPos.y,v0,c1
+	dp4 r0.y,v0,c1
dp4 oPos.z,v0,c2
+	dp4 r0.z,v0,c2
dp4 oPos.w,v0,c3
+	dp4 r0.w,v0,c3

; texture coord generation
mul r2,r0.w,c5
mad oT0,r0,c4,r2
+	mad r0,r0,c4,r2

; additional texture co-ords for dotzw
mov oT1.xy,c6
mov oT1.z,r0.z
mov oT2.xy,c6
mov oT2.z,r0.w

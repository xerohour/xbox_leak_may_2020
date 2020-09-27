xvss.1.1

;matrix inversion code from nvidia

;input 4x4 matrix in c[0-3]
;output 4x4 matrix in c[0-3]

mov		r4, c[0]
mov		r5, c[1]
mov		r6, c[2]
mov		r7, c[3]

; generate first half of matrix
mul		r0, r6.wyzx, r7.zwyx
mul		r1, r4.wyzx, r5.zwyx
mul		r2, r6.wxzy, r7.zwxy
mul		r3, r4.wxzy, r5.zwxy

mad		r0, r6.zwyx, r7.wyzx, -r0
mad		r1, r4.zwyx, r5.wyzx, -r1
mad		r2, r6.zwxy, r7.wxzy, -r2
mad		r3, r4.zwxy, r5.wxzy, -r3

dp3		r8.x, r5.yzwx, r0
dp3		r9.x, r4.yzwx, -r0
dp3		r10.x, r7.yzwx, r1
dp3		r11.x, r6.yzwx, -r1

dp3		r8.y, r5.xzwy, -r2
dp3		r9.y, r4.xzwy, r2
dp3		r10.y, r7.xzwy, -r3
dp3		r11.y, r6.xzwy, r3

; generate 2nd half of matrix
mul		r0, r6.wxyz, r7.ywxz
mul		r1, r4.wxyz, r5.ywxz
mul		r2, r6.zxyw, r7.yzxw
mul		r3, r4.zxyw, r5.yzxw

mad		r0, r6.ywxz, r7.wxyz, -r0
mad		r1, r4.ywxz, r5.wxyz, -r1
mad		r2, r6.yzxw, r7.zxyw, -r2
mad		r3, r4.yzxw, r5.zxyw, -r3

dp3		r8.z, r5.xywz, r0
dp3		r9.z, r4.xywz, -r0
dp3		r10.z, r7.xywz, r1
dp3		r11.z, r6.xywz, -r1

dp3		r8.w, r5.xyzw, -r2
dp3		r9.w, r4.xyzw, r2
dp3		r10.w, r7.xyzw, -r3
dp3		r11.w, r6.xyzw, r3

dp4		r7.w, r8, r4				;determinant
rcp		r7.w, r7.w					;reciprocal
mul		c[0], r8, r7.w				;multiply into matrix
mul		c[1], r9, r7.w
mul		c[2], r10, r7.w
mul		c[3], r11, r7.w

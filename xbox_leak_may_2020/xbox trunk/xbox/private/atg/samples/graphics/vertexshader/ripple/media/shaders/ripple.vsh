;------------------------------------------------------------------------------
; Vertex shader to make a ripple effect
;------------------------------------------------------------------------------
vs.1.0

;----------------------------------------------------------
; Constants:
;
;  c0-c3  = World, view, & projection matrix concatenation
;
;  c4.x   = ?
;  c4.y   = 0.0
;  c4.z   = 0.5
;  c4.w   = 1.0
;
;  c5.x   = Time
;  c5.y   = Frequency
;  c5.z   = X offset
;  c5.w   = Y offset
;
;  c7.x   - pi
;  c7.y   - 1/2pi
;  c7.z   - 2pi
;  c7.w   - Height
;
;  c10    - first 4 taylor coefficients for sin(x)
;  c11    - first 4 taylor coefficients for cos(x)  
;----------------------------------------------------------


;----------------------------------------------------------
; Decompress position

; r0 = Vector4( v0.x, 0.0f, v0.z, 1.0f )
mov r0.xz, v0.xxyy        ; r0.x = v0.x,   r0.z = v0.y
mov r0.yw, c4.yyww        ; r0.y =  0.0,   r0.w =  1.0

;----------------------------------------------------------
; Compute theta from distance, time, and frequency

; r4 = r0 + vOffset
mov r4, r0                ; Start with position
add r4.xz, r4.xz, c5.zzww ; Add X and Y offsets

; r4 = Vec3Length( r4 )
dp3 r4.x, r4, r4          ; r4.x = r4^2
rsq r4.x, r4.x            ; r4.x = 1.0 / sqrt( r4.x )
rcp r4.x, r4.x            ; r4.x = 1.0 / r4.x

; r4 = fFreq * ( r4 + fTime )
add r4.x, r4.x, c5.x      ; Add time 
mul r4.x, r4.x, c5.y      ; Scale by frequency

;----------------------------------------------------------
; Clamp theta to -PI..+PI.
; Note: The docs on the expp function will make it apparent
;       why we use it for the fraction computation.

; r4 = fraction( (r4+PI)/2PI ) * 2PI - PI
add  r4.x, r4.x, c7.x        ; r4.x += PI
mul  r4.x, r4.x, c7.y        ; r4.x *= 1/(2*PI)
expp r4.y, r4.x              ; r4.y  = r4.x - floor(r4.x)
mad  r4.x, r4.y, c7.z, -c7.x ; r4.x = (2*PI*r4.y)-PI

;----------------------------------------------------------
; Compute sin and cos for theta using Taylor series
;    r4.x = theta
;    r5.x = sin
;    r6.x = cos
;    c10  = Taylor coeffecients for sin
;    c11  = Taylor coeffecients for cos
;    c12  = ( 1/2, 1/2*pi, 0, 0 )

mad r5.x, r4.x, c12.y, c12.x
expp r5.yw, r5.xxxx
add r5.x, r5.y, -c12.x

mul r6, r5.wwxx, r5.wxxx  //0,1,2,2
mul r6.w, r6.w, r6.y      //0,1,2,3 
mul r6, r6, r6            //0,2,4,6
mul r5, r5.xxxx, r6       //1,3,5,7

dp4 r5.x, r5, c10
dp4 r6.x, r6, c11

;----------------------------------------------------------
; Scale height

mul r0.y, r5.x, c7.w    ; r0.y = fHeight * sin(theta)

;----------------------------------------------------------
; Transform position
m4x4 oPos, r0, c0       ; [oPos] = [r0]| c0..c3 |

;----------------------------------------------------------
; Set color

; (r,g,b,a) = ( (1-cos(theta)/2, (1-cos(theta)/2, 0.0, (1-cos(theta)/2 )
add r6, -r6.x, c4.w     ; [diffuse]   = 1-cos(theta)
mov r6.z, c4.y          ; [diffuse].b = 0.0

;mul oD0, r6, c4.z       ; [diffuse]  *= 0.5

mov oD0, r6


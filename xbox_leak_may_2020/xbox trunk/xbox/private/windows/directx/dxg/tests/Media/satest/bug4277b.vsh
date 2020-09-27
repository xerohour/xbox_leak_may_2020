xvsw.1.1
mov r1, c9                
mul r3, r1.wwww, c6.xyzz  

    mad r11.x, r3.x     , c[82].y, c[82].x
    expp r11.yw, r11.xxxx
    add r11.x, r11.y, -c[82].x
    mul r11, r11.wwxx, r11.wxxx    
    mul r11.zw, r11.xyzw, r11.xxyz 
    mul r11, r11.xxyz, r11.yzww    
    dp4 r5.x, r11, c[80 ]
    mad r11.x, r3.y     , c[82].y, c[82].x
    expp r11.yw, r11.xxxx
    add r11.x, r11.y, -c[82].x
    mul r11, r11.wwxx, r11.wxxx    
    mul r11.zw, r11.xyzw, r11.xxyz 
    mul r11, r11.xxyz, r11.yzww    
    dp4 r5.y, r11, c[80 ]
    mad r11.x, r3.z  , c[82].y, c[82].x
    expp r11.yw, r11.xxxx
    add r11.x, r11.y, -c[82].x
    mul r11, r11.wwxx, r11.wxxx    
    mul r11.zw, r11.xyzw, r11.xxyz 
    mul r11, r11.xxyz, r11.yzww    
    dp4 r5.z, r11, c[80 ]

add r4.x, r1.w, r5.x    
add r4.x, r4.x, r5.y    
mov r4.z, r5.z

    mad r10.x, r4.x, c[82].y, c[82].x
    expp r10.yw, r10.xxxx
    add r10.x, r10.y, -c[82].x

    mul r11, r10.wwxx, r10.wxxx 
    mul r11.w, r11.w, r11.y      
    mul r11, r11, r11           
    mul r10, r10.xxxx, r11      

    dp4 r4.x, r10, c[80 ]
    dp4 r4.y, r11, c[81]
mov r4.zw, c7.xyxy

mad c9.xyzw, r4.yxzw, c6.wwww, r1.xyzw 
mad r4.xy, r4.yxzw, c6.wwww, r1.xyzw 

dp4 oPos.x, r4, c0
dp4 oPos.y, r4, c1
dp4 oPos.z, r4, c2
dp4 oPos.w, r4, c3




mov oD0.xyz, c9.xyz


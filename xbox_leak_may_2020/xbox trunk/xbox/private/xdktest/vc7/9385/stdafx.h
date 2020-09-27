/*
*********************************************************** 
*** VC7 with manual precompiled headers (/Yc & /Yu) and *** 
*** global optimizations (/Og) can initialize a chain   *** 
*** of dependent global constants in the wrong order.   *** 
*********************************************************** 
*/

//-------------------- stdafx.h -------------------- 
const float Pie  = 3.14f; 
const float Pie1 = Pie*1;   // "Pie1" depends on "Pie" 
const float Pie2 = Pie1*2;  // "Pie2" depends on "Pie1" 

/*
*********************************************************** 
*** VC7 with manual precompiled headers (/Yc & /Yu) and *** 
*** global optimizations (/Og) can initialize a chain   *** 
*** of dependent global constants in the wrong order.   *** 
*********************************************************** 
*/

//-------------------- stdafx.cpp -------------------- 
#include "stdafx.h" 

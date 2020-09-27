// templates generate names > 255 chars in debug info
// 4786: identifier was truncated to 'number' characters in the debug information
#pragma warning(disable : 4786)

#include <d3dx8seg.h>
#include <stdio.h>
#include <malloc.h>

#define NOD3D
#define NODSOUND
#include <xtl.h>
#undef  NOD3D
#undef NODSOUND

#include <d3d8.h>
#include "d3dx8p.h"
#include <d3d8types.h>
#include "common.h"
#include "gxcrackfvf.h"

#ifndef HeapValidate
#define HeapValidate
#endif


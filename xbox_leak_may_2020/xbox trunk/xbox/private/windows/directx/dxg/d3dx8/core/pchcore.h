//
// D3DX core precompiled header
//

#ifndef __PCHCORE_H__
#define __PCHCORE_H__
#include <d3dx8seg.h>

#define D3DCOMPILE_BEGINSTATEBLOCK 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <xtl.h>

#include "d3dx8.h"
#include "d3dx8dbg.h"
#include "d3d8types.h"

#include "CD3DXFile.h"
#include "CD3DXStack.h"
#include "CD3DXBuffer.h"
#include "CD3DXSprite.h"
#include "CD3DXRenderToSurface.h"
#include "CD3DXRenderToEnvMap.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)

#endif //__PCHCORE_H__
//
// D3DX effect precompiled header
//

#ifndef __PCHEFFECT_H__
#define __PCHEFFECT_H__

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

class CEffectNode;
class CD3DXValue;
class CD3DXParameter;
class CD3DXAssignment;
class CD3DXPass;
class CD3DXTechnique;
class CD3DXEffect;

#include "CD3DXStack.h"
#include "CCompiler.h"
#include "CDeclaration.h"
#include "CEffect.h"
#include "CTechnique.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)


#endif //__PCHEFFECT_H__//

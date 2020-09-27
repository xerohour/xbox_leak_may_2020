#include <d3dx8seg.h>
#include <xtl.h>
#include "d3dx8dbg.h"
#include "malloc.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)

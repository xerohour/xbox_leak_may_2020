MAJORCOMP=directx
MINORCOMP=d3d

TARGETTYPE=LIBRARY

INCLUDES=..;$(BASEDIR)\private\ntos\inc

# PERFFLAGS=/fastcap

# Optimize for speed.  We do this because we use write-combined memory a lot,
# and when compiling for size the compiler will sometimes change writes-to-
# memory into read-and-modify-writes (such as when moving zero to memory,
# where the form 'and dword ptr [esi], 0' has a smaller byte encoding than
# 'move dword ptr [esi], 0').  Reads from write-combined memory are VERY
# painful.

!if "$(MSC_OPTIMIZATION)"==""
MSC_OPTIMIZATION=/Ox
!endif

SOURCES=\
    block.cpp \
    buffer.cpp \
    clear.cpp \
    combiner.cpp \
    d3dbase.cpp \
    d3ddev.cpp \
    debug.cpp \
    drawprim.cpp \
    dumper.cpp \
    dxgcreate.cpp \
    enum.cpp \
    floatmath.cpp \
    globals.cpp \
    lazy.cpp \
    math.cpp \
    memory.cpp \
    mpcore.cpp  \
    mpdac.cpp \
    mphal.cpp \
    mpintr.cpp \
    mpmode.cpp \
    overlay.cpp \
    patch.cpp \
    patchbackend.cpp \
    patchconst.cpp \
    patchutil.cpp \
    pixeljar.cpp \
    present.cpp \
    pshader.cpp \
    pusher.cpp \
    pushres.cpp \
    rdi.cpp \
    resource.cpp \
    shadersnapshot.cpp \
    state.cpp \
    stats.cpp \
    surface.cpp \
    texture.cpp \
    vshader.cpp 
    
    
PRECOMPILED_INCLUDE=..\precomp.hpp
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj

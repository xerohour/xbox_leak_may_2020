INCLUDES=\
    $(BASEDIR)\private\ntos\xnet\inc; \
    $(BASEDIR)\private\ntos\inc

C_DEFINES=\
    $(C_DEFINES) \
    -D_UNICODE \
    -DUNICODE \
    -DNT

MSC_WARNING_LEVEL=/W3 /WX

PRECOMPILED_INCLUDE=precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=$(TARGETNAME)pre.obj

!if defined(XNET_DEBUG_COMPILE_LEVEL)
C_DEFINES=$(C_DEFINES) -DXNET_DEBUG_COMPILE_LEVEL=$(XNET_DEBUG_COMPILE_LEVEL)
!endif


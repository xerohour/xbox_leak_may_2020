MAJORCOMP=directx
MINORCOMP=d3d

TARGETNAME=shadeasm$(D)

TARGETPATH=obj
TARGETTYPE=LIBRARY

INCLUDES=$(INCLUDES);..;..\..\inc

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchshadeasm.h
PRECOMPILED_PCH=pchshadeasm.pch
PRECOMPILED_OBJ=pchshadeasm.obj

SOURCES =                 \
    CD3DXStack.cpp \
    CD3DXAssembler.cpp \
	preprocessor.cpp \
    api.cpp \
    valbase.cpp \
    pshdrval.cpp \
    vshdrval.cpp \
    pixelshader.cpp


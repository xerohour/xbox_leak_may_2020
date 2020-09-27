!include ..\..\..\sources.inc

TARGETNAME=l_pshade
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    PShader.cpp \
    op.cpp \
    address.cpp \
    register.cpp \
    srcmod.cpp \
    dstmod.cpp \
    instmod.cpp

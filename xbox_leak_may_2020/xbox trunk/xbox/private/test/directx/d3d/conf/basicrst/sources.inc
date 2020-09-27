!include ..\..\..\sources.inc

TARGETNAME=l_basrst
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    basicrst.cpp \
    colorwrite.cpp \
    culling.cpp \
    dither.cpp \
    fill.cpp \
    meshd3d.cpp \
    meshl.cpp \
    meshtl.cpp \
    nodiffuse.cpp \
    shading.cpp \
    zwrite.cpp

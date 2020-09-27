!include ..\..\..\sources.inc

TARGETNAME=l_cubemp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    Blend.cpp \
    CubeMap.cpp \
    Filter.cpp \
    Matrix.cpp \
    Mipmaps.cpp \
    PitchRotation.cpp \
    TexGen.cpp \
    Utility.cpp \
    Utility8.cpp \
    YawRotation.cpp

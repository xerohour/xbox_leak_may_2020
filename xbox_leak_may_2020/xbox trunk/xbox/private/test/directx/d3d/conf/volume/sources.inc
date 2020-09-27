!include ..\..\..\sources.inc

TARGETNAME=l_volume
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    blend.cpp \
    draw.cpp \
    filter.cpp \
    matrix.cpp \
    mipmap.cpp \
    texgen.cpp \
    texture.cpp \
    volume.cpp

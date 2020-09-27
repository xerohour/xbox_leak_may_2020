!include ..\..\..\sources.inc

TARGETNAME=l_bump
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    bump.cpp \
    flags.cpp \
    address.cpp \
    blend.cpp \
    matrix.cpp \
    offset.cpp \
    scale.cpp

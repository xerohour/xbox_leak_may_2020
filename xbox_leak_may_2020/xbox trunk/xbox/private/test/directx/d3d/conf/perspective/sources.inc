!include ..\..\..\sources.inc

TARGETNAME=l_perspc
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    color.cpp \
    drawmesh.cpp \
    perspective.cpp \
    texture.cpp

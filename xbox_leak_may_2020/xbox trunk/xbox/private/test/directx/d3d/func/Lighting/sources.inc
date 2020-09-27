!include ..\..\..\sources.inc

TARGETNAME=l_lightn
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

C_DEFINES=$(C_DEFINES) -DTESTTYPE_LIGHTING

SOURCES=\
    lighting.cpp \
    properties.cpp \
    vertices.cpp

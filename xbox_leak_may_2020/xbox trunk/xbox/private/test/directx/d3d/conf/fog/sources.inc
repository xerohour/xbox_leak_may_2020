!include ..\..\..\sources.inc

TARGETNAME=l_fog
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    fog.cpp \
    table.cpp \
    vertex.cpp \
    w.cpp

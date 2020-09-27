!include ..\..\..\sources.inc

TARGETNAME=l_gradnt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    drawgrid.cpp \
    gradient.cpp \
    texsize.cpp

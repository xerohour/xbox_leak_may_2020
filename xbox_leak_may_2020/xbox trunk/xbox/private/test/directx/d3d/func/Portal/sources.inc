!include ..\..\..\sources.inc

TARGETNAME=l_portal
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    portal.cpp \
    floor.bmp.cpp \
    tex1.bmp.cpp \
    shade.bmp.cpp

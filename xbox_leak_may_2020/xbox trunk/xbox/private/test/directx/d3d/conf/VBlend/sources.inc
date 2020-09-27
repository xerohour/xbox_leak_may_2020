!include ..\..\..\sources.inc

TARGETNAME=l_vblend
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    VBlend.cpp \
    groups.cpp

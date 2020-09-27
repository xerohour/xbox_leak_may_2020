!include ..\..\..\sources.inc

TARGETNAME=l_fogmod
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    fogmode.cpp \
    redvelv.bmp.cpp \
    back.bmp.cpp

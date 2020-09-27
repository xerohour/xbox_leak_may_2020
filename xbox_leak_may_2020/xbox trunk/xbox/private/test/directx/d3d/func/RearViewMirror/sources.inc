!include ..\..\..\sources.inc

TARGETNAME=l_rearvm
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    rearviewmirror.cpp \
    tile.bmp.cpp \
    shade.bmp.cpp
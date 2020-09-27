!include ..\..\..\sources.inc

TARGETNAME=l_ovrdrw
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    alpha.cpp \
    alphagrid.cpp \
    blt.cpp \
    bltgrid.cpp \
    misc.cpp \
    overdraw.cpp

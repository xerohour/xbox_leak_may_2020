!include ..\..\..\sources.inc

TARGETNAME=l_mipfil
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        drawgrid.cpp \
        full.cpp \
        maxbias.cpp \
        min.cpp \
        mipfilter.cpp \
        nonsquare.cpp

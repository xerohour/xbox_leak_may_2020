!include ..\..\..\sources.inc

TARGETNAME=l_zbuffr
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        zbuffer.cpp \
        enable.cpp \
        print.cpp \
        verify.cpp \
        verify8.cpp

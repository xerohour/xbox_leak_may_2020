!include ..\..\..\sources.inc

TARGETNAME=l_litmap
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    lightmap.cpp \
    flags.cpp

!include ..\..\..\sources.inc

TARGETNAME=l_linetx
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    LineTex.cpp \
    Default.cpp

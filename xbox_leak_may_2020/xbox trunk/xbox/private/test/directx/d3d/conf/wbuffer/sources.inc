!include ..\..\..\sources.inc

TARGETNAME=l_wbuffr
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    wbuffer.cpp
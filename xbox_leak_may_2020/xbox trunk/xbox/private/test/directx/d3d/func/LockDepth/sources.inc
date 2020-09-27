!include ..\..\..\sources.inc

TARGETNAME=l_lckdep
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    lockdepth.cpp

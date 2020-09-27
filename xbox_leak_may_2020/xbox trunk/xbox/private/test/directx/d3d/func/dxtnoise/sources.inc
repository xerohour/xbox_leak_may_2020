!include ..\..\..\sources.inc

TARGETNAME=l_dnoise
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    dxtnoise.cpp

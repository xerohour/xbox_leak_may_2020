!include ..\..\..\sources.inc

TARGETNAME=l_ibuffr
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    ibuffer.cpp \
    default.cpp \
    properties.cpp

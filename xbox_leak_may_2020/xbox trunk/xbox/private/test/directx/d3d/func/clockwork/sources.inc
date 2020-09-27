!include ..\..\..\sources.inc

TARGETNAME=l_clwork
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    clockwork.cpp \
    cogs.cpp

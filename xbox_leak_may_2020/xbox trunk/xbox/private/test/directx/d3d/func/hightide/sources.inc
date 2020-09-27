!include ..\..\..\sources.inc

TARGETNAME=l_htide
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    hightide.cpp \
    revolve.cpp

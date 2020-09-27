!include ..\..\..\sources.inc

MSC_OPTIMIZATION =  $(MSC_OPTIMIZATION) /Od

TARGETNAME=l_vistest
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

INCLUDES=$(INCLUDES);$(BASEDIR)\private\ntos\inc

SOURCES=\
    vistest.cpp

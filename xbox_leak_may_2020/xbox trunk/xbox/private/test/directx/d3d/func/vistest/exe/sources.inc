TARGETNAME=vistest
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_vistest.lib 

INCLUDES=$(INCLUDES);$(BASEDIR)\private\ntos\inc

SOURCES=\
    include.cpp

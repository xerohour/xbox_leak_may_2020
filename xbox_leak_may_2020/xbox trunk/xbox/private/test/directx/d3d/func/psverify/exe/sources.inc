TARGETNAME=psverify
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\psgen.lib \
    $(TEST_LIB_PATH)\l_psver.lib

SOURCES=\
    include.cpp

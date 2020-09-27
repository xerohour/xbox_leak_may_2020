TARGETNAME=xbox550
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

UMRES=$(TEST_RES_PATH)\test.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx550.lib

SOURCES=\
    include.cpp

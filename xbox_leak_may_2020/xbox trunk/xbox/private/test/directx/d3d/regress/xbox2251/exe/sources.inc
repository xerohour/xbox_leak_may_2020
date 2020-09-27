TARGETNAME=xbox2251
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

UMRES=$(TEST_RES_PATH)\basic.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx2251.lib

SOURCES=\
    include.cpp


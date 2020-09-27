TARGETNAME=terrain
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\terrain.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_terain.lib

SOURCES=\
    include.cpp

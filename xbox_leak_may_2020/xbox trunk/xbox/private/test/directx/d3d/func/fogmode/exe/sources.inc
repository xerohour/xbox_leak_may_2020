TARGETNAME=fogmode
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\fogmode.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_fogmod.lib

SOURCES=\
    include.cpp

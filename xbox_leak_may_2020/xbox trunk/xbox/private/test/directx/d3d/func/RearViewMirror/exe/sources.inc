TARGETNAME=RearViewMirror
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\rearviewmirror.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rearvm.lib

SOURCES=\
    include.cpp

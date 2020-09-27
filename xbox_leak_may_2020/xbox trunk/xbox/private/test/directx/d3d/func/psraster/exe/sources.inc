TARGETNAME=psraster
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\test.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\psgen.lib \
    $(TEST_LIB_PATH)\l_psrast.lib

SOURCES=\
    include.cpp

TARGETNAME=d3dbvt
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\sources.inc

UMRES=$(TEST_RES_PATH)\d3dbvt.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\l_d3dbvt.lib

SOURCES=\
    main.cpp

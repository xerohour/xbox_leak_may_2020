TARGETNAME=d3dapi
TARGETTYPE=PROGRAM
TARGETPATH=obj


BROWSER_INFO=1
USE_MAPSYM=1

!include ..\..\..\sources.inc

UMRES=$(TEST_RES_PATH)\d3dapi.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\l_d3dapi.lib \
    $(TEST_LIB_PATH)\xlog.lib \

SOURCES=\
    main.cpp

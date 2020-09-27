!include ..\..\..\sources.inc

TARGETNAME=d3dbvt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\l_d3dbvt.lib

SOURCES=\
    main.cpp

TARGETNAME=luminanc
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
	$(TEST_LIB_PATH)\cshapes.lib \
    $(TEST_LIB_PATH)\l_lumin.lib

SOURCES=\
    include.cpp

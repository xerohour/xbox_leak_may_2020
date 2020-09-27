!include ..\..\..\..\sources.inc

TARGETNAME=gradient
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_gradnt.lib

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
    $(TEST_LIB_PATH)\l_gradnt.lib

SOURCES=\
    include.cpp

!include ..\..\..\..\sources.inc

TARGETNAME=alphacmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_alpcmp.lib

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
    $(TEST_LIB_PATH)\CShapes.lib \
    $(TEST_LIB_PATH)\l_alpcmp.lib

SOURCES=\
    include.cpp

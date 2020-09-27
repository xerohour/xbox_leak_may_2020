!include ..\..\..\..\sources.inc

TARGETNAME=xbox3050
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
    $(TEST_LIB_PATH)\CShapes.lib \
    $(TEST_LIB_PATH)\l_vshade.lib

SOURCES=\
    include.cpp

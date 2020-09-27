!include ..\..\..\..\sources.inc

TARGETNAME=fvertex1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_fvert1.lib

SOURCES=\
    include.cpp

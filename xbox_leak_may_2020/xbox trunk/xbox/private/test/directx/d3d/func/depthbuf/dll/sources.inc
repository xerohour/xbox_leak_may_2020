!include ..\..\..\..\sources.inc

TARGETNAME=depthbuf
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_depbuf.lib

SOURCES=\
    include.cpp

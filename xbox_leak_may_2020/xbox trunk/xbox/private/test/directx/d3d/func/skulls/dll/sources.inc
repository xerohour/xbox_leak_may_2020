!include ..\..\..\..\sources.inc

TARGETNAME=skulls
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_skulls.lib

SOURCES=\
    include.cpp

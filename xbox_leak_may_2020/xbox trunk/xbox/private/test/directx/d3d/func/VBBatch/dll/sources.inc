!include ..\..\..\..\sources.inc

TARGETNAME=VBBatch
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vbatch.lib

SOURCES=\
    include.cpp

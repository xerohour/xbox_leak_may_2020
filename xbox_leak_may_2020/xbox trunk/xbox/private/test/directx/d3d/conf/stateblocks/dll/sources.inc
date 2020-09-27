!include ..\..\..\..\sources.inc

TARGETNAME=StateBlocks
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_sblock.lib

SOURCES=\
    include.cpp
